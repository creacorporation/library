//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2005/08/19～
//----------------------------------------------------------------------------

#ifndef MODBC_H_INCLDUED
#define MODBC_H_INCLDUED

/*
リファレンス：
	・さとーＣ＋＋ぶろぐ
	Windows環境にてC言語やC++でODBCを使うには
	http://sato-si.at.webry.info/200503/article_11.html
	・ODBC API Reference
	http://msdn.microsoft.com/en-us/library/ms714562.aspx
	・ばぁばのＯＤＢＣ実験室
	http://www.amy.hi-ho.ne.jp/jbaba/index.htm
	・Inside ODBC
	  ISBN4-7561-1617-5
	
*/

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "../General/mErrorLogger.h"

#include <sql.h>
#include <sqlext.h>
#include <memory>
#pragma comment(lib, "odbc32.lib")

namespace mOdbc
{
	//パラメータの種類
	enum ParameterType
	{
		Int64,			//符号あり64ビット整数型(符号なしはありません)
		Int32,			//符号あり32ビット整数型(符号なしはありません)
		Int16,			//符号あり16ビット整数型(符号なしはありません)
		Int8,			//符号あり8ビット整数型(符号なしはありません)
		Float,			//単精度浮動小数点
		Double,			//倍精度浮動小数点
		AString,		//ANSI文字列
		WString,		//UNICODE文字列
		Binary,			//バイナリ
		Date,			//日付
		Time,			//時刻
		Timestamp,		//タイムスタンプ
	};

}

//使用方法
#if 0
int main( int argc , char** argv )
{
	InitializeLibrary();

	//●接続

	//ODBC接続のための情報をセットします
	mOdbcEnvironment::ConnectInfo info;
	info.DataSource = L"LOCALDB";	//データソース
	info.User = L"";				//ユーザー名 ←MS SQL Serverの場合空欄にするとWindows認証になる
	info.Password = L"";			//パスワード

	//データベースへの接続を確立します
	mOdbcConnection db;
	g_OdbcEnvironment.NewConnect( info , db );

	//[オプション]
	//必要なら、トランザクションの設定をします。
	//db.SetAutoCommit( false );

	//クエリのためのオブジェクトを作成します
	mOdbcQuery query;
	db.NewQuery( query );

	//●クエリ

	//クエリを行います
	query.Execute( L"use TESTDB" );
	query.Execute( L"select * from Name" );

	//[オプション]
	//長い文字列のレコードがある場合は、バッファを再確保する必要があります。
	//バッファに入りきらない場合は、データは切り詰められてしまいます。
	//既定のバッファ長は、mOdbcQuery::MAX_FETCH_BUFFER_SIZEに定義されます。
	//query.ResizeFetchBuffer( L"UserName" , 200000 );

	//[オプション]
	//クエリした結果に関する情報を取得します。
	//※このオブジェクトの詳細は、デバッガで覗いてみてください。
	const mOdbcResultDescription& resultdesc = query.GetResultDescription();
	const mOdbcDescriptionEntry& descentry1 = resultdesc.at( L"Name" );
	const mOdbcDescriptionEntry& descentry2 = resultdesc.at( L"Id" );
	const mOdbcDescriptionEntry& descentry3 = resultdesc.at( L"Score" );

	//行をフェッチします。
	mOdbcResultParam result;
	query.Fetch( result );
	printf( "%d(%ws) : %d\n" , result[ L"Id" ].Get<int32_t>() , result[ L"Name" ].Get<WString>().c_str() , result[ L"Score" ].Get<int32_t>() );
	query.Fetch( result );
	printf( "%d(%ws) : %d\n" , result[ L"Id" ].Get<int32_t>() , result[ L"Name" ].Get<WString>().c_str() , result[ L"Score" ].Get<int32_t>() );


	//●行の追加
	query.Prepare( L"insert into Name ( Id , Name , Score ) values ( ? , ? , ? )" );

	//[オプション]
	//パラメータの情報を取得
	const mOdbcParameterDescription& desc = query.GetParameterDescription();

	//※サブクエリやjoinを使いたい場合、メタデータの取り出しがうまくいかないことがある
	//　このときは、いったんダミーのクエリでメタデータを取り出してから、本番のクエリを行う。以下サンプル。
	//  (1)パラメータ情報を取得するためのダミークエリ。本番のクエリと?の出現順は同じにすること。
	//  query.Prepare(
	//  	L"select * from TableA ,TableB"
	//  	L" where TableA.Date = ? and TableB.Class = ?" );
	//  
	//  //(2)本来行いたいクエリ
	//  query.Prepare(
	//  	L"select * from TableB"
	//  	L" left outer join TableA on TableB.Id = TableA.Id and TableA.Date = ?"
	//  	L" where TableB.Class = ?"
	//  	, query.GetParameterDescription() ); //←こうすると前のダミークエリのメタデータをそのまま使用する

	//追加するパラメータの設定
	mOdbcQueryParams params;
	params.push_back( (int32_t) 300 );	//1番目(Id)
	params.push_back( L"Added User" );	//2番目(Name)
	params.push_back( (int32_t) 301 );	//3番目(Score)

	//追加実行
	query.Execute( params );
	return 0;
}
#endif


#endif
