//----------------------------------------------------------------------------
// INIファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

/*
★用途
	INIファイルの1つ分のセクションを保持します。

★セクションとは
	「セクション」とは、

	[BASIC]			←タイトル
	HOGE=1
	FUGA=c:\windows
	PIYO=0xFFFF		←キーと値の組

	のように、[]でくくられたタイトルと、キーと値の組からなるカタマリです。
	また、[]の代わりに<>でタイトルをくくると、行単位での読み込みになります(「数字だけのキーについて」を参照)

	次の[]または<>が出てくるまでが１つのセクションになります。
	同一のセクション内で同名のキーが出現すると、後から定義したものを使います。
	同じセクション名を2度使うと、後から出てきたほうで置き換えます

★ドット(.)で始まるキーについて
	ドットで始まるキーは、サブキー名です。直近のドットのないキーに連結してキー名を形成します。
	サブキーにはインデックスをつけることは出来ません。

	[BASIC]
	HogeValue=10		; HogeValue=10			キー=HogeValue インデックス=なし サブキー=なし
	.Opt=11				; HogeValue.Opt=11		キー=HogeValue インデックス=なし サブキー=Opt
	FugaVelue[1]=20		; FugaValue[1]=20		キー=FugaValue インデックス=1    サブキー=なし
	.Opt=21				; FugaValue[1].Opt=21	キー=FugaValue インデックス=1    サブキー=Opt

★インデックスについて
	インデックスは、
	NUM[1]=100
	NUM[2]=100
	のように、キーの直後につく10進数の値です。
	インデックスを使うと、1つのキーに対して連番でアクセスできます。

★数字だけのキーについて

	<BASIC>
	あいうえお
	HOGE=かきくけこ		;コメント
	さしすせそ

	のように、タイトルを<>でくくると、行単位の読み込みになります。
	行単位の読み込みの場合、
		・キーは無視されます
		・コメントの;は認識されます
	キーはセクション内での行番号となります。以下の場合は行番号にカウントしません。
		・コメントのみの行

	キー「0」→値「あいうえお」
	キー「1」→値「HOGE=かきくけこ」
	キー「2」→値「さしすせそ」
	になります。
	
	キーを「2」にする以外に、キーを空文字列として、インデックスを2としても「さしすせそ」となります。
	
★このクラスについて
	mInitFileSection１個には１つのセクションが格納されます。
	キーと読み取りたい形式を指定すると、その形式で値が読み取られます。
	PIYO=0xFFFF
	この例では、PIYOを文字列として読み取ると"0xFFFF"が読み取られます。
	一方、PIYOを数値として読み取ると65535が読み取られます。
*/


#ifndef MINITFILESECTION_H_INCLUDED
#define MINITFILESECTION_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"
#include "General/mFileWriteStream.h"
#include "General/mHexdecimal.h"
#include "GDI/mWindowPosition.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

class mInitFileSection
{
public:
	mInitFileSection();
	virtual ~mInitFileSection();
	mInitFileSection( const mInitFileSection& src );
	mInitFileSection& operator=( const mInitFileSection& src );

	//16進数オブジェクト
	//この方を使わない場合でも、文字列に0xがついていれば16進数で読み取るが、
	//ついていない場合でも強制的に16進数で読みたい場合はこのオブジェクトを使う
	typedef mHexdecimal Hexdecimal;

	//------------------------------------------------------
	//☆値の読み取り関数群
	//------------------------------------------------------

	//指定したキーが存在するか返す
	//key : 存在を確認したいキー
	//ret : 指定したキーが存在すればtrue
	bool IsValidKey( const WString& key )const noexcept;

	//指定したキーが存在するか返す
	//key : 存在を確認したいキー
	//index : 何番目のキーを検証するか( key + INT2TEXT( index )のキーを確認します)
	//ret : 指定したキーが存在すればtrue
	bool IsValidKey( const WString& key , INT index )const noexcept;

	//指定したキーが存在するか返す
	//key : 存在を確認したいキー
	//index : 何番目のキーを検証するか( key + INT2TEXT( index )のキーを確認します)
	//subkey : 存在を確認したいサブキー
	//ret : 指定したキーが存在すればtrue
	bool IsValidKey( const WString& key , INT index , const WString& subkey )const noexcept;

	//指定したキーが存在するか返す
	//key : 存在を確認したいキー
	//subkey : 存在を確認したいサブキー
	//ret : 指定したキーが存在すればtrue
	bool IsValidKey( const WString& key , const WString& subkey )const noexcept;

	//キーの値を32ビットINTの値として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	INT GetValue( const WString& key , INT index , const WString& subkey , INT defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を32ビットLONGの値として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	LONG GetValue( const WString& key , INT index , const WString& subkey , LONG defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を64ビットINTの値として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	LONGLONG GetValue( const WString& key , INT index , const WString& subkey , LONGLONG defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を32ビットUINTとして読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	UINT GetValue( const WString& key , INT index , const WString& subkey , UINT defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を32ビットDWORDとして読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	DWORD GetValue( const WString& key , INT index , const WString& subkey , DWORD defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を64ビットUINTとして読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	ULONGLONG GetValue( const WString& key , INT index , const WString& subkey , ULONGLONG defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を32ビットFLOATとして読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	FLOAT GetValue( const WString& key , INT index , const WString& subkey , FLOAT defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を64ビットDOUBLEとして読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	DOUBLE GetValue( const WString& key , INT index , const WString& subkey , DOUBLE defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値をUNICODE文字列として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	WString GetValue( const WString& key , INT index , const WString& subkey , const WString& defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値をASCII文字列として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	AString GetValue( const WString& key , INT index , const WString& subkey , const AString& defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値をUNICODE文字列として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	const wchar_t* GetValue( const WString& key , INT index , const WString& subkey , const wchar_t* defvalue , bool* retIsReadable = nullptr )const noexcept;

	//GetValue()のconst char*版はありません
	//内部表現はUNICODEになっており、ASCII文字列を直接返すことが出来ないためです。
	//const char* GetValue( const WString& key , const char* defvalue )const noexcept;

	//キーの値を色として読み取る
	//・#に続く16進数6桁を#rrggbbとして読み取ります。
	//・#に続く16進数8桁を#rrggbbaaとして読み取ります。aaを読み取った場合、RGBQUAD構造体のreservedメンバに格納します。
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	RGBQUAD GetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値をブール値として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	//※trueと判定されるもの
	//　true、yes
	//※falseと判定されるもの
	//  false、no
	//※defvalueの値が返る場合
	//  キーが存在しない場合、上記true・falseの条件を満たさない場合
	bool GetValue( const WString& key , INT index , const WString& subkey , bool defvalue , bool* retIsReadable = nullptr )const noexcept;

	//キーの値を16進数の値として読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	Hexdecimal GetValue( const WString& key , INT index , const WString& subkey , Hexdecimal defvalue , bool* retIsReadable = nullptr )const noexcept;

	//------------------------------------------------------
	//☆文字列⇒値のテーブルを使ったGetValue
	//------------------------------------------------------

	//文字列⇒値のテーブルのエントリ
	template< class T > struct LookupValuesEntry
	{
		WString String;	//この文字列に
		T Value;		//この値を関連付ける(文字列リテラルにする場合は、テンプレートのtypenameにconstをお忘れなく)
	};

	//LookupValuesEntryのvectorをtypedefしたもの
	template< class T > using LookupValues = std::vector< LookupValuesEntry< T > >;

	//文字列⇒値のテーブルを使って、キーの値を読み取る
	//key : 読み取りたいキー
	//values : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
	//defvalue : キーを読み取れなかった場合、valuesに一致するものがなかった場合の値
	//ret : valuesからルックアップした値、または、defvalueの値。
	//【valuesに渡す構造体の作成例】
	//  const mInitFileSection::LookupValues< bool > BooleanValues =
	//  {
	//		{ L"yes"	, true  },	//設定値がyesだとtrueが返る
	//		{ L"no"		, false },	//設定値がnoだとfalseが返る
	//  };
	template< class T > T GetValue( const WString& key , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		return GetValue( key , 0 , L"" , values , defvalue , retIsReadable );
	}

	//文字列⇒値のテーブルを使って、キーの値を読み取る
	//key : 読み取りたいキー
	//index : 何番目のキーを読み取るか( key + INT2TEXT( index )のキーを読み取ります)
	//values : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
	//defvalue : キーを読み取れなかった場合、valuesに一致するものがなかった場合の値
	//ret : valuesからルックアップした値、または、defvalueの値。
	template< class T > T GetValue( const WString& key , INT index , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		return GetValue( key , index , L"" , values , defvalue , retIsReadable );
	}

	//文字列⇒値のテーブルを使って、キーの値を読み取る
	//key : 読み取りたいキー
	//subkey : 読み取りたいサブキー
	//values : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
	//defvalue : キーを読み取れなかった場合、valuesに一致するものがなかった場合の値
	//ret : valuesからルックアップした値、または、defvalueの値。
	template< class T > T GetValue( const WString& key , const WString& subkey , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		return GetValue( key , 0 , subkey , values , defvalue , retIsReadable );
	}

	//文字列⇒値のテーブルを使って、キーの値を読み取る
	//key : 読み取りたいキー
	//index : 何番目のキーを読み取るか( key + INT2TEXT( index )のキーを読み取ります)
	//subkey : 読み取りたいサブキー
	//values : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
	//defvalue : キーを読み取れなかった場合、valuesに一致するものがなかった場合の値
	//ret : valuesからルックアップした値、または、defvalueの値。
	template< class T > T GetValue( const WString& key , INT index , const WString& subkey , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		//設定値を取得
		Key tmpkey;
		tmpkey.key = key;
		tmpkey.index = index;
		tmpkey.subkey = subkey;

		KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
		if( itr == MyKeyValueMap.end() )
		{
			//キーが存在しないのでデフォルトを返す
			ResetReadable( retIsReadable );
			return defvalue;
		}
		//取得した値を全て小文字に変換
		WString str1 = itr->second;
		std::transform( str1.cbegin() , str1.cend() , str1.begin() , wchar_tolower );

		//valuesの中をスキャンし、一致するものが見つかれば関連づけられている値を返す
		typename LookupValues< T >::const_iterator values_itr = values.begin();
		for( ; values_itr != values.end() ; values_itr++ )
		{
			WString str2 = values_itr->String;
			std::transform( str2.cbegin() , str2.cend() , str2.begin() , wchar_tolower );
			if( str1 == str2 )
			{
				//一致するものがあったのでこれ。
				SetReadable( retIsReadable );
				return values_itr->Value;
			}
		}
		//一致なし。
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//------------------------------------------------------
	//☆その他形式のテンプレート
	//------------------------------------------------------

	//==================================================
	//＜読み取り結果を返さないバージョン＞
	//==================================================

	//キーの値を読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	 //※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	inline T GetValue( const WString& key , T defvalue )const noexcept
	{
		return GetValue( key , 0 , L"" , defvalue );
	}

	//キーの値を読み取る
	//key : 読み取りたいキー
	//index : 何番目のキーを読み取るか( key + INT2TEXT( index )のキーを読み取ります)
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	 //※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	T GetValue( const WString& key , INT index , T defvalue )const noexcept
	{
		return GetValue( key , index , L"" , defvalue );
	}

	//キーの値を読み取る
	//key : 読み取りたいキー
	//subkey : 読み取りたいサブキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	 //※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	T GetValue( const WString& key , const WString& subkey , T defvalue )const noexcept
	{
		return GetValue( key , 0 , subkey , defvalue );
	}

	//取り扱い不可な型が指定された場合エラーにするための関数
	template< typename T >
	inline T GetValue( const WString& key , INT index , const WString& subkey , T defvalue )const noexcept
	{
		//==================================================
		//　　　　　　　この型は使用できません
		//==================================================
		//・defvalueに非対応の型を指定していませんか？
		//  ユーザー定義の型などは対応していません。
		//・defvalueにASCII文字列や、char*型の引数を指定していませんか？
		//　この場合は、AStringに一旦変換してください。
		//  ×　printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , "ABC" ) ) );
		//  ○　printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , AString( "ABC" ) ).c_str() );
		static_assert( false , "This typename is not allowed (see comment)" );
	}

	//==================================================
	//＜読み取り結果を返すバージョン＞
	//==================================================

	//キーの値を読み取る
	//key : 読み取りたいキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	//※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	inline T GetValue( bool* retIsReadable , const WString& key , T defvalue )const noexcept
	{
		return GetValue( key , 0 , L"" , defvalue , retIsReadable );
	}

	//キーの値を読み取る
	//key : 読み取りたいキー
	//index : 何番目のキーを読み取るか( key + INT2TEXT( index )のキーを読み取ります)
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	//※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	T GetValue( bool* retIsReadable ,  const WString& key , INT index , T defvalue )const noexcept
	{
		return GetValue( key , index , L"" , defvalue , retIsReadable );
	}

	//キーの値を読み取る
	//key : 読み取りたいキー
	//subkey : 読み取りたいサブキー
	//defvalue : キーを読み取れなかった場合の値
	//ret : 読み取った値。読み取れなかった場合はdefvalueの値
	//※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	T GetValue( bool* retIsReadable ,  const WString& key , const WString& subkey , T defvalue )const noexcept
	{
		return GetValue( key , 0 , subkey , defvalue , retIsReadable );
	}

	//取り扱い不可な型が指定された場合エラーにするための関数
	template< typename T >
	inline T GetValue( bool* retIsReadable ,  const WString& key , INT index , const WString& subkey , T defvalue )const noexcept
	{
		//==================================================
		//　　　　　　　この型は使用できません
		//==================================================
		//・defvalueに非対応の型を指定していませんか？
		//  ユーザー定義の型などは対応していません。
		//・defvalueにASCII文字列や、char*型の引数を指定していませんか？
		//　この場合は、AStringに一旦変換してください。
		//  ×　printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , "ABC" ) ) );
		//  ○　printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , AString( "ABC" ) ).c_str() );
		static_assert( false , "This typename is not allowed (see comment)" );
	}

	//==================================================
	//＜読み取り結果を返り値で返すバージョン＞
	//==================================================

	//キーの値を読み取る
	//key : 読み取りたいキー
	//iovalue : 格納先。読み取れなかった場合は変化しない。
	//ret : 読み取った場合真
	//※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	inline bool QueryValue( const WString& key , T iovalue )const noexcept
	{
		bool result = false;
		iovalue =  GetValue( key , 0 , L"" , iovalue , &result );
		return result;
	}

	//キーの値を読み取る
	//key : 読み取りたいキー
	//index : 何番目のキーを読み取るか( key + INT2TEXT( index )のキーを読み取ります)
	//iovalue : 格納先。読み取れなかった場合は変化しない。
	//ret : 読み取った場合真
	//※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	bool QueryValue( const WString& key , INT index , T iovalue )const noexcept
	{
		bool result = false;
		iovalue =  GetValue( key , index , L"" , iovalue , &result );
		return result;
	}

	//キーの値を読み取る
	//key : 読み取りたいキー
	//subkey : 読み取りたいサブキー
	//iovalue : 格納先。読み取れなかった場合は変化しない。
	//ret : 読み取った場合真
	//※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	bool QueryValue( const WString& key , const WString& subkey , T iovalue )const noexcept
	{
		bool result = false;
		iovalue = GetValue( key , 0 , subkey , iovalue , &result );
		return result;
	}

	//取り扱い不可な型が指定された場合エラーにするための関数
	template< typename T >
	inline bool QueryValue( const WString& key , INT index , const WString& subkey , T iovalue )const noexcept
	{
		//==================================================
		//　　　　　　　この型は使用できません
		//==================================================
		//・defvalueに非対応の型を指定していませんか？
		//  ユーザー定義の型などは対応していません。
		//・defvalueにASCII文字列や、char*型の引数を指定していませんか？
		//　この場合は、AStringに一旦変換してください。
		//  ×　printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , "ABC" ) ) );
		//  ○　printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , AString( "ABC" ) ).c_str() );
		static_assert( false , "This typename is not allowed (see comment)" );
	}


	//------------------------------------------------------
	//☆値のセット
	//------------------------------------------------------

	//指定のキーに32ビットINTの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , INT newval )noexcept;

	//指定のキーに32ビットLONGの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , LONG newval )noexcept;

	 //指定のキーに64ビットINTの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , LONGLONG newval )noexcept;

	//指定のキーに32ビットUINTの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , DWORD newval )noexcept;

	//指定のキーに64ビットUINTの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , ULONGLONG newval )noexcept;

	//指定のキーに32ビットFLOATの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , FLOAT newval )noexcept;

	//指定のキーに64ビットDOUBLEの値を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , DOUBLE newval )noexcept;

	//指定のキーにUNICODE文字列を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , const WString& newval )noexcept;

	//指定のキーにASCII文字列を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , const AString& newval )noexcept;

	//指定のキーにUNICODE文字列を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , const wchar_t* newval )noexcept;

	//指定のキーにASCII文字列を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , const char* newval )noexcept;
	 
	 //指定のキーに輝度値を設定する
	//・RGBQUADの値を、#に続く16進数8桁を#rrggbbaaとして書き込みます。
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& newval )noexcept;

	//指定のキーにGDI座標を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 bool SetValue( const WString& key , INT index , const WString& subkey , const mWindowPosition::POSITION& newval )noexcept;

	//指定のキーにWindow座標を設定する
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	//指定したキーに対して、以下のサブキーの値を設定します。
	//・top		…上辺 WindowPosition::topに対応
	//・bottom	…底辺 WindowPosition::bottomに対応 
	//・left	…左辺 WindowPosition::leftに対応
	//・right	…右辺 WindowPosition::rightに対応
	 bool SetValue( const WString& key , const mWindowPosition::WindowPosition& newval )noexcept;

	//指定のキーにWindow座標を設定する
	//key : 設定したいキー
	//index : 何番目のキーを設定するか( key + INT2TEXT( index )のキーを設定します)
	//newval : 設定する値
	//ret : 成功時真
	//指定したキーに対して、以下のサブキーの値を設定します。
	//・top		…上辺 WindowPosition::topに対応
	//・bottom	…底辺 WindowPosition::bottomに対応 
	//・left	…左辺 WindowPosition::leftに対応
	//・right	…右辺 WindowPosition::rightに対応
	 bool SetValue( const WString& key , INT index , const mWindowPosition::WindowPosition& newval )noexcept;

	//指定のキーにブール値を書き込む
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	//※true/falseで書き込みます
	 bool SetValue( const WString& key , INT index , const WString& subkey , bool newval )noexcept;

	//------------------------------------------------------
	//☆その他形式のテンプレート
	//------------------------------------------------------

	//指定のキーに値を書き込む
	//key : 設定したいキー
	//newval : 設定する値
	//ret : 成功時真
	 //※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	template< typename T >
	bool SetValue( const WString& key , T newval )noexcept
	{
		return SetValue( key , 0 , L"" , newval );
	}

	//指定のキーに値を書き込む
	//key : 設定したいキー
	//index : 何番目のキーを設定するか( key + INT2TEXT( index )のキーを設定します)
	//newval : 設定する値
	//ret : 成功時真
	 //※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	 template< class T >
	 bool SetValue( const WString& key , INT index , T newval )noexcept
	 {
		return SetValue( key , index , L"" , newval );
	 }

	//指定のキーに値を書き込む
	//key : 設定したいキー
	//subkey : 設定したいサブキー
	//newval : 設定する値
	//ret : 成功時真
	 //※個別の型で読み書きする場合の注意点は、index、subkey無しの基本形の関数にコメントが書いてあります。
	 template< class T >
	 inline bool SetValue( const WString& key , const WString& subkey , T newval )noexcept
	 {
		return SetValue( key , 0 , subkey , newval );
	 }

	//取り扱い不可な型が指定された場合エラーにするための関数
	 template< class T >
	 bool SetValue( const WString& key , INT index , const WString& subkey , T newval )noexcept
	 {
		//==================================================
		//　　　　　　　この型は使用できません
		//==================================================
		//・defvalueに非対応の型を指定していませんか？
		//  ユーザー定義の型などは対応していません。
		static_assert( false , "This typename is not allowed (see comment)" );
	 }
	 
	 //------------------------------------------------------
	//☆その他
	//------------------------------------------------------

	 //検索用キー
	struct Key
	{
		WString key;		//キー
		INT index;			//インデックス
		WString subkey;		//サブキー

		Key()
		{
			index = 0;
		}
		bool operator==( const Key& src )const
		{
			return ( index == src.index ) && ( key == src.key ) && ( subkey == src.subkey );
		}
		bool operator!=( const Key& src )const
		{
			return !( this->operator==( src ) ); 
		}
		bool operator<( const Key& src )const
		{
			if( key != src.key )
			{
				return key < src.key;
			}
			if( index != src.index )
			{
				return index < src.index;
			}
			return subkey < src.subkey;
		}
	};

	//キー一覧表
	typedef std::deque<Key> KeyList;
	typedef std::deque<INT> IndexList;

	//存在するキーの一覧を返す
	//※この関数は、キー名だけを返します
	bool GetKeyList( WStringDeque& retList )const;

	//存在するキーの一覧を返す
	//※この関数は、全てのキーを返します
	//retList : キーの一覧を格納する
	//ret : 成功時真
	bool GetKeyList( KeyList& retList )const;

	//存在するキーの一覧を返す
	//※この関数は、指定したキーのサブキーを返します
	//retList : キーの一覧を格納する
	//ret : 成功時真
	bool GetKeyList( const WString& key , KeyList& retList )const;

	//指定したキーに対する最大・最小のインデックス番号を得ます
	//ret : 成功時真、失敗時偽(指定したキーが存在しない場合など)
	//※インデックス番号は飛び飛びかもしれないことに注意してください
	bool GetIndexRange( const WString& key , INT& ret_min_index , INT& ret_max_index )const;

	//存在するキーに含まれるインデックスのリストを返す
	//retList : インデックスの一覧を格納する
	//ret : 成功時真、失敗時偽(指定したキーが存在しない場合など)
	bool GetIndexList( const WString& key , IndexList& retList )const;

	//ファイルにセクションの内容を出力する
	// fp : 出力先オブジェクト
	// ret : 成功時真
	bool Write( mFileWriteStream& fp )const;

	//存在するキーの数を取得する
	// ret : 格納されているキーの数
	DWORD GetCount( void )const;

	//セクションの名前を設定する
	bool SetSectionName( const WString& newname );

	//セクションの名前を取得する
	const WString& GetSectionName( void )const;

	//セクション形式
	enum SectionType 
	{
		SECTIONTYPE_NORMAL,		// []形式
		SECTIONTYPE_ROWBASE,	// <>形式 行単位の読み込み
	};

	//セクション形式を変更する
	// newtype : 新しい形式
	// ret : 成功時真
	bool SetSectionType( SectionType newtype );

	//セクション形式を取得する
	// ret : セクション形式
	SectionType GetSectionType( void )const;

	//更新されているか否かを返す
	// ret : 更新されていればtrue
	bool IsModified( void )const;

protected:

	//親クラスからの直接書き込み用
	friend class mInitFile;

	//このセクションの名前
	WString MySectionName;

	//セクション形式
	SectionType MySectionType;

	//キーのハッシュ取得関数
	struct KeyHash {
		typedef std::size_t result_type;
		inline std::size_t operator()( const Key& key ) const
		{
			std::hash<WString> hash_function;
			return hash_function( key.key );
		}
	};

	//左：項目名　右：項目の設定値
	typedef std::unordered_map<Key,WString,KeyHash> KeyValueMap;
	KeyValueMap MyKeyValueMap;

	//更新されているか？
	bool MyIsModified;

	inline void ResetReadable( bool* retIsReadable )const
	{
		if( retIsReadable )
		{
			*retIsReadable = false;
		}
	}
	inline void SetReadable( bool* retIsReadable )const
	{
		if( retIsReadable )
		{
			*retIsReadable = true;
		}
	}

};

//mInitFileSectionから指定キーの値を得る。
//ポインタがヌルか、指定キーがない場合は、デフォルトの値を得る。
//ptr : mInitFileSectionへのポインタ
//key : mInitFileSectionから値を取得するときのキー
//defvalue : ポインタがヌルか、指定キーがない場合に得る値
//ret : 指定したキーの値か、defvalueの値
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , defvalue );
}

//mInitFileSectionから指定キーの値を得る。
//ポインタがヌルか、指定キーがない場合は、デフォルトの値を得る。
//ptr : mInitFileSectionへのポインタ
//key : mInitFileSectionから値を取得するときのキー
//index : 何番目のキーを読み取るか( key + INT2TEXT( index )のキーを読み取ります)
//defvalue : ポインタがヌルか、指定キーがない場合に得る値
//ret : 指定したキーの値か、defvalueの値
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , INT index , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , index , defvalue );
}

//mInitFileSectionから指定キーの値を得る。
//ポインタがヌルか、指定キーがない場合は、デフォルトの値を得る。
//key : 読み取りたいキー
//values : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
//defvalue : キーを読み取れなかった場合、valuesに一致するものがなかった場合の値
//ret : valuesからルックアップした値、または、defvalueの値。
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , const mInitFileSection::LookupValues< T >& table , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , table , defvalue );
}

//mInitFileSectionから指定キーの値を得る。
//ポインタがヌルか、指定キーがない場合は、デフォルトの値を得る。
//key : 読み取りたいキー
//values : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
//defvalue : キーを読み取れなかった場合、valuesに一致するものがなかった場合の値
//ret : valuesからルックアップした値、または、defvalueの値。
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , INT index , const mInitFileSection::LookupValues< T >& table , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , index , table , defvalue );
}

#endif	//MINITFILESECTION_H_INCLUDED

