//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2005/08/19〜
//----------------------------------------------------------------------------

#ifndef MODBCCONNECTION_H_INCLUDED
#define MODBCCONNECTION_H_INCLUDED

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

#include "mstandard.h"
#include "mOdbc.h"
#include "mOdbcQuery.h"
#include "mOdbcSqlState.h"
#include "mOdbcConnectionHandle.h"

#include "../General/mTCHAR.h"
#include <sql.h>
#include <sqlext.h>
#include <deque>
#include <memory>
#pragma comment(lib, "odbc32.lib")

class mOdbcConnection
{

public:
	mOdbcConnection();
	virtual ~mOdbcConnection();

	//新たなクエリーを作成する
	//retQuery : 新たに作成したクエリーのハンドル
	//ret : 接続成功時真
	bool NewQuery( mOdbcQuery& retQuery );

	//自動コミットモードを使用するか
	// enable : 真＝自動コミットモードを使用する(既定)
	//          偽＝トランザクションを行う
	//トランザクションを行う場合は、Commit、Rollbackの操作が必要になります
	bool SetAutoCommit( bool enable );

	//コミットする
	// ret : 成功時真
	bool Commit( void );

	//ロールバックする
	// ret : 成功時真
	bool Rollback( void );

	//処理中のクエリが存在するか
	// ret : 存在する場合true
	bool IsActive( void ) const noexcept;

private:

	mOdbcConnection( const mOdbcConnection& source );			//コピー禁止クラス
	void operator=( const mOdbcConnection& source ) = delete;	//コピー禁止クラス

	friend class mOdbcEnvironment;

protected:

	//ODBC接続中のハンドル
	mOdbcConnectionHandle MyDBCHandle;

	//この接続に関連付いているクエリのチケット型
	using QueryTicket = std::weak_ptr<ULONG_PTR>;

	//この接続に関連付いているクエリのチケット一覧
	using Queries = std::deque< QueryTicket >;

	//この接続に関連付いているクエリのチケット一覧
	mutable Queries MyQueries;

	//SQLRETURN型の結果コードが正常終了かどうかを判定する
	//追加情報がある場合は、SQLステートメントのメンバ(MyOdbcSqlState)を更新する
	//rc : 結果コード
	//ret : 結果コードが成功を示すものであれば真
	bool SQL_RESULT_CHECK( SQLRETURN rc );

};

#endif