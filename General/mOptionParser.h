//----------------------------------------------------------------------------
// コマンドライン文字列管理クラス
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MOPTIONPARSER_H_INCLUDED
#define MOPTIONPARSER_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"

/*
●用途
コマンドラインの文字列を解析して、どのオプション(/hとか--hogeとか)が指定されているか
そのオプションに付属している文字列がなにかとかを得ます。

●使い方例
mOptionParser opt;	//勝手にコマンドライン文字列を取得しに行きますargc,argvを
					//渡す必要はないです。

if( opt.Check( L'?' ) ){...}	//「/?」または「-?」というオプションがあるかを調べます

if( opt.Check( L"hoge" ) ){...}	//「--hoge」というオプションがあるかを調べます

●注意点
コマンドライン文字列の最初の要素は、実行ファイル名になりますが、
パスが全部付いているとは限りません(WinAPIの仕様)。

●認識できるオプション
	/a			オプションaが指定
	-a			オプションaが指定
	/abc		オプションaとbとcが指定
	-abc		オプションaとbとcが指定
	--abc		オプションabcが指定(abcと完全一致)

	-a xyz		オプションaに対して文字列xyzを指定
	/a xyz		オプションaに対して文字列xyzを指定
	--abc xyz	オプションabcに対して文字列xyzを指定(abcと完全一致)

	-abc xyz	オプションaとbとcに対して文字列xyzを指定
	/abc xyz	オプションaとbとcに対して文字列xyzを指定
				(オプションa,b,cどれに対してもxyzが取得される)
	・大文字と小文字は区別する
*/

class mOptionParser
{
public:

	//コンストラクタ
	//※Windowアプリはargv[0]が実行ファイル名ではないが、
	//  自動的に補います。
	mOptionParser();

	virtual ~mOptionParser() = default;

	//指定のオプションが指定されているかチェック
	//opt_char 存在するかチェックするオプション(1文字)
	//opt_string 存在するかチェックするオプション(文字列)
	//ret : 指定がある場合真
	bool Check( WCHAR opt_char )const;
	bool Check( const WString& opt_string )const;
	bool Check( WCHAR opt_char , const WString& opt_string )const;

	//指定のオプションの位置(何番目のオプションとして存在するか)を返す
	//opt_char 存在するかチェックするオプション(1文字)
	//opt_string 存在するかチェックするオプション(文字列)
	//ret : 見つけた場合はその位置(0開始)。見つからない場合は負の値。
	//文字・文字列の両方指定し、かつ両方あった場合は文字列の位置が返る
	INT GetPosition( WCHAR opt_char )const;
	INT GetPosition( const WString& opt_string )const;
	INT GetPosition( WCHAR opt_char , const WString& opt_string )const;


	//最初のオプションの指定位置を得ます
	//ret : 最初にあるオプションの位置
	//      そもそもオプションが1個もない場合は負の数
	INT GetFirstOptPosition( void )const;

	//最後のオプションの指定位置を得ます
	//ret : 最後にあるオプションの位置
	//      そもそもオプションが1個もない場合は負の数
	//使い方ヒント：この関数の結果をGetStringArray()に渡すと、最後のオプション以降全部の配列を得られます。
	INT GetLastOptPosition( void )const;

	//指定オプションに紐付いている文字列を最初の1個だけ取得します
	//opt_char : 存在するかチェックするオプション(1文字)
	//opt_string : 存在するかチェックするオプション(文字列)
	//def_string : エラーの場合に返す結果
	//ret : 見つけた場合はそれに関連づけられている文字列の配列。
	//      以下の場合、def_stringに指定した文字列。
	//			・オプションが見つからない場合
	//			・オプションはあるが関連する文字列がない場合
	WString GetString( WCHAR opt_char , const WString& def_string = L"" )const;
	WString GetString( const WString& opt_string , const WString& def_string = L"" )const;
	WString GetString( WCHAR opt_char , const WString& opt_string , const WString& def_string = L"" )const;

	//指定位置の文字列を1つ取得します
	//このメソッドは、それが文字列であれオプションの一部であれ強制的に取得します。
	//index : 取得したい位置
	//def_string : エラーの場合に返す結果
	//ret : 見つけた場合はそれに関連づけられている文字列の配列。
	//      エラーの場合、def_stringに指定した文字列。
	WString GetStringIndex( INT index , const WString& def_string = L"" )const;


	//指定オプションに紐付いている文字列を取得します
	//opt_char : 存在するかチェックするオプション(1文字)
	//opt_string : 存在するかチェックするオプション(文字列)
	//def_string : エラーの場合に返す結果
	//ret : 見つけた場合はそれに関連づけられている文字列の配列。
	//      以下の場合、def_stringに指定した文字列。
	//			・オプションが見つからない場合
	//			・オプションはあるが関連する文字列がない場合
	WStringVector GetStringArray( WCHAR opt_char , const WString& def_string = L"" )const;
	WStringVector GetStringArray( const WString& opt_string , const WString& def_string = L"" )const;
	WStringVector GetStringArray( WCHAR opt_char , const WString& opt_string , const WString& def_string = L"" )const;

	//指定位置から、最初にオプションでないところまで文字列を取得します
	//index : 開始位置
	//def_string : エラーの場合に返す結果
	//ret : 取得した文字列の配列
	//      以下の場合、def_stringに指定した文字列。
	//			・indexが配列外の場合
	//			・1つも格納できる文字列が無かった場合
	WStringVector GetStringArrayIndex( INT index , const WString& def_string = L"" )const;


private:

	//コピー禁止
	mOptionParser( const mOptionParser& source ) = delete;
	void operator=( const mOptionParser& source ) = delete;

protected:

	//指定位置の文字列が何なのかを取得します
	enum ArgType
	{
		ARGTYPE_NOTOPTION,	//オプションを示すものではない
		ARGTYPE_OPT_CHAR,	//1文字オプション
		ARGTYPE_OPT_STRING,	//文字列オプション
		ARGTYPE_ERROR,		//エラー(範囲外など)
	};
	ArgType GetArgType( INT index )const;

protected:

	//コマンドライン文字列の配列
	WStringVector MyArgs;

};

#endif	//MOPTIONPARSER_H_INCLUDED
