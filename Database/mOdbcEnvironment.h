//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCENVIRONMENT_H_INCLUDED
#define MODBCENVIRONMENT_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "mOdbc.h"
#include "mOdbcSqlState.h"
#include "mOdbcConnection.h"

#include <deque>
#include <sql.h>
#include <sqlext.h>
#include <memory>

#pragma comment(lib, "odbc32.lib")

//ODBCの環境ハンドルをプロセス内で共用するためのクラスです
//※環境ハンドルは１プロセス１個だけです
//・最初に参照されたときに、ODBC3.0の環境ハンドルを生成し、以降それを使いまわします。
//・このクラスのインスタンスがなくなった場合、環境ハンドルを破棄します

class mOdbcEnvironment final
{
public:

	mOdbcEnvironment();
	virtual ~mOdbcEnvironment();

	//データベースに接続するときの引数を格納する構造体
	struct ConnectInfo
	{
		WString DataSource;		//データベースソースの名前
		WString User;			//ユーザー名
		WString Password;		//パスワード
	};

	//データベースに接続する
	//info : 接続先情報
	//retConn : 接続した結果得られたハンドル
	//ret : 接続成功時真
	bool NewConnect( const ConnectInfo& info , mOdbcConnection& retConn );

	//環境ハンドルが生成されているかを返す
	operator bool() const;

private:

	mOdbcEnvironment( const mOdbcEnvironment& source );			//コピー禁止クラス
	void operator=( const mOdbcEnvironment& source ) = delete;	//コピー禁止クラス

private:

	//ODBC環境ハンドル
	static HENV MyHenv;

	//参照カウント
	static DWORD MyRefCount;

	//環境ハンドルの確保
	bool CreateHandle( void );

	//環境ハンドルの解放
	void DisposeHandle( void );

	//SQLRETURN型の結果コードが正常終了かどうかを判定する
	//また、SQLステートメントのメンバを設定します
	//rc : 結果コード
	//ret : 結果コードが成功を示すものであれば真
	bool SQL_RESULT_CHECK( SQLRETURN rc );

};

//グローバルオブジェクト
#ifndef MODBCENVIRONMENT_CPP_COMPILING
extern mOdbcEnvironment g_OdbcEnvironment;
#else
mOdbcEnvironment g_OdbcEnvironment;
#endif

#endif
