//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MODBCENVIRONMENT_CPP_COMPILING
#include "mOdbcEnvironment.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mErrorLogger.h"

//静的変数の実態宣言
HENV mOdbcEnvironment::MyHenv = 0;
DWORD mOdbcEnvironment::MyRefCount = 0;

mOdbcEnvironment::mOdbcEnvironment()
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );
	MyRefCount++;
	return;
}

mOdbcEnvironment::~mOdbcEnvironment()
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );

	//参照カウントを更新し、０になっていればハンドルを解放する
	MyRefCount--;
	if( MyRefCount )
	{
		return;	//０じゃないので何もしない
	}

	//０になったので、ハンドルを解放
	DisposeHandle();
	return;
}

mOdbcEnvironment::operator bool() const
{
	return MyHenv != 0;
}

bool mOdbcEnvironment::NewConnect( const ConnectInfo& info , mOdbcConnection& retConn )
{
	//環境ハンドルの生成
	if( !CreateHandle() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"環境ハンドルの生成エラー" );
		return false;
	}

	//コネクションハンドルの作成
	HDBC hdbc = 0;
	if( !SQL_RESULT_CHECK( SQLAllocHandle( SQL_HANDLE_DBC , MyHenv , &hdbc ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"コネクションハンドルを生成できません" );
		return false;
	}
	retConn.MyDBCHandle = std::make_shared<mOdbcConnectionHandleContents>( hdbc );

	//ODBCドライバに接続
	if( !retConn.SQL_RESULT_CHECK( SQLConnectW(
		*retConn.MyDBCHandle.get(),
		const_cast<SQLWCHAR*>( info.DataSource.c_str() ),	//データソース名
		(SQLSMALLINT)info.DataSource.size(),
		const_cast<SQLWCHAR*>( info.User.c_str() ),			//ユーザー名
		(SQLSMALLINT)info.User.size(),
		const_cast<SQLWCHAR*>( info.Password.c_str() ),		//パスワード
		(SQLSMALLINT)info.Password.size()
	) ) )
	{
		//エラー情報は接続ハンドル側に記録されるため、オブジェクトの作成自体は成功する
		RaiseError( g_ErrorLogger , 0 , L"データベースに接続できませんでした" , info.DataSource );
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"データベースに接続しました" , info.DataSource );
	}
	return true;
}

bool mOdbcEnvironment::CreateHandle( void )
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );

	//すでに環境ハンドルが存在する場合はこれを返す
	if( MyHenv )
	{
		return true;
	}

	//存在しない場合は新たに環境ハンドルを確保する
	if( !SQL_SUCCEEDED( SQLAllocHandle( SQL_HANDLE_ENV , SQL_NULL_HANDLE , &MyHenv ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"環境ハンドルを確保できません" );
		goto end;
	}

	//ODBCバージョンの通知
	if( !SQL_SUCCEEDED( SQLSetEnvAttr( MyHenv , SQL_ATTR_ODBC_VERSION , (SQLPOINTER)SQL_OV_ODBC3_80 , SQL_IS_UINTEGER ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ODBCバージョンの設定エラーです" );
		goto end;
	}

	CreateLogEntry( g_ErrorLogger , 0  , L"ODBC環境ハンドルを新規に作成しました" );
	return true;

end:
	DisposeHandle();
	return 0;
}

void mOdbcEnvironment::DisposeHandle( void )
{
	if( MyHenv )
	{
		if( !SQL_SUCCEEDED( SQLFreeHandle( SQL_HANDLE_ENV , MyHenv ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"ODBC環境ハンドルの解放に失敗しました" );
		}
		else
		{
			CreateLogEntry( g_ErrorLogger , 0  , L"ODBC環境ハンドルが解放されました" );
		}
		MyHenv = 0;
	}
	return;
}

bool mOdbcEnvironment::SQL_RESULT_CHECK( SQLRETURN rc )
{
	//成功以外の場合には、何らかの追加情報があるとみて、取得を試みる。
	if( rc != SQL_SUCCESS )
	{
		mOdbcSqlState::AppendLog( MyHenv , mOdbcSqlState::HandleKind::Environment );
	}
	return SQL_SUCCEEDED( rc );
}
