//----------------------------------------------------------------------------
// INIファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MINITFILE_H_INCLUDED
#define MINITFILE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"
#include "General/mFileReadStream.h"
#include "General/mFileWriteStream.h"
#include "General/mInitFileSection.h"
#include <unordered_map>

class mInitFile
{
public:
	mInitFile();
	virtual ~mInitFile();
	mInitFile( const mInitFile& src );
	mInitFile& operator=( const mInitFile& src );

	//読み込み済みのデータを破棄する
	void Clear( void )noexcept;

	//Initファイル読み書き時のエンコード
	using Encode = mFileReadStream::Encode;

	//INIファイルを読み込みます
	//すでにINIファイルを読み込んでいる場合は、既存のデータと併合します。
	//filename : 読み込むファイル名
	//tolower : キーを小文字に変換して読み込みます
	//ret : 成功すればtrue
	//※エンコードを指定しない場合、UTF16(BOMなし)として読み込みます
	bool Read( const WString& filename , bool tolower = false , Encode enc = Encode::ENCODE_ASIS )noexcept;

	//INIファイルを読み込みます
	//fp : 読み込むファイルのストリーム
	//※エンコードはfpに設定して下さい。なお、設定していないインスタンスを渡した場合、
	//  UTF16(BOMなし)として読み込みます
	bool Read( mFileReadStream& fp , bool tolower = false );

	//INIファイルを書き込みます
	// filename : 書き込み先のファイル名
	// ret : 成功すればtrue
	//※同一ファイルが存在する場合は、上書きされます
	bool Write( const WString& filename );

	//INIファイルを書き込みます
	// fp : 書き込み先のオブジェクト
	// ret : 成功すればtrue
	bool Write( mFileWriteStream& fp );

	//セクションを取得します
	//section : 取得したセクションの名称
	//ret : 指定したセクションの内容を含むmInitFileSectionクラスへの参照。
	//      該当するセクションがない場合はヌルポインタ。
	const mInitFileSection* GetSection( const WString& section )const noexcept;

	//セクションを取得します
	//※更新のため、constでないポインタが欲しい場合はこちらを指定します。
	//section : 取得したセクションの名称
	//ret : 指定したセクションの内容を含むmInitFileSectionクラスへの参照。
	//      該当するセクションがない場合はヌルポインタ。
	mInitFileSection* GetSectionForModification( const WString& section )noexcept;

	//セクションの一覧を取得します
	//section : 取得した結果
	//ret : 成功時真
	bool SectionList( WStringDeque& section )const noexcept;

	//セクションが存在するかを判定します
	//section : 存在を判定したいセクションの名前
	//ret : セクションが存在すればtrue。存在しなければfalse。
	bool IsExistSection( const WString& section )const noexcept;

	//セクションを追加します
	//すでに同名のセクションがある場合は、そのセクションを取得します
	// section : 作成または取得するセクションの名称
	// ret : 作成または取得したセクションのポインタ
	mInitFileSection* CreateNewSection( const WString& section )noexcept;

	//セクションを削除します
	// section : 削除するセクションの名称
	// ret : true セクションを削除した
	//       false そのようなセクションはなかった
	bool DeleteSection( const WString& section )noexcept;

	//指定したセクションの、指定キーの値を得る。
	//指定セクション・指定キーがない場合は、デフォルトの値を得る。
	//section : セクションの名前
	//key : キーの名前
	//defvalue : 指定セクション・指定キーがない場合に得る値
	//ret : 指定したキーの値か、defvalueの値
	//※効率がよくないので、同じセクションに何度もアクセスするときは、
	//　この関数を使わずにGetSectionを使ってセクションのポインタを取得して使ってください。
	template< class T > T GetValue( const WString& section , const WString& key , T defvalue )const
	{
		const mInitFileSection* ptr = GetSection( section );
		if( ptr == nullptr )
		{
			return defvalue;
		}
		return ptr->GetValue( key , defvalue );
	}

	//指定したセクションの、指定キーの値を得る。
	//指定セクション・指定キーがない場合は、デフォルトの値を得る。
	//section : セクションの名前
	//key : キーの名前
	//table : キーの値をルックアップするテーブル。キーの値と完全一致するものが一覧の中にあれば、対応する値を返す
	//　　　＃このテーブルの詳細はmInitFileSection::LookupValuesの宣言を参照してください。
	//defvalue : 指定セクション・指定キーが読み取れなかった場合、valuesに一致するものがなかった場合の値
	//ret : valuesからルックアップした値、または、defvalueの値。
	//※効率がよくないので、同じセクションに何度もアクセスするときは、
	//　この関数を使わずにGetSectionを使ってセクションのポインタを取得して使ってください。
	template< class T > T GetValue( const WString& section , const WString& key , const mInitFileSection::LookupValues< T >& table , T defvalue )const
	{
		const mInitFileSection* ptr = GetSection( section );
		if( ptr == nullptr )
		{
			return defvalue;
		}
		return ptr->GetValue( key , table , defvalue );
	}

	//セクション名の一覧を取得する
	//retList : セクション名の一覧を格納する
	//ret : 成功時真
	bool GetSectionList( WStringDeque& retList )const;

	//ダミー用エンプティセクション
	static const mInitFileSection EmptySection;

private:

	//INIファイルの読み取った結果
	typedef std::unordered_map<WString,mInitFileSection> SectionDataMap;
	SectionDataMap MySectionDataMap;

};

#endif	//MINITFILE_H_INCLUDED

