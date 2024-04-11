//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCCONNECTIONHANDLE_H_INCLUDED
#define MODBCCONNECTIONHANDLE_H_INCLUDED


#include "mstandard.h"
#include "../General/mTCHAR.h"
#include "../General/mErrorLogger.h"

#include <sql.h>
#include <sqlext.h>
#include <deque>
#pragma comment(lib, "odbc32.lib")

class mOdbcConnectionHandleContents final
{
public:
	mOdbcConnectionHandleContents()
	{
		MyDbc = 0;
	}
	mOdbcConnectionHandleContents( HDBC dbc )
	{
		MyDbc = dbc;
	}
	~mOdbcConnectionHandleContents()
	{
		if( MyDbc == 0 )
		{
			return;
		}
		if( !SQL_SUCCEEDED( SQLDisconnect( MyDbc ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"接続解除に失敗しました" );
		}
		if( !SQL_SUCCEEDED( SQLFreeHandle( SQL_HANDLE_DBC , MyDbc ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"接続ハンドルの解放に失敗しました" );
		}
	}
	operator HDBC()
	{
		return MyDbc;
	}

private:
	mOdbcConnectionHandleContents( const mOdbcConnectionHandleContents& source );		//コピー禁止クラス
	void operator=( const mOdbcConnectionHandleContents& source ) = delete;	//コピー禁止クラス

private:
	//ハンドルの実体
	HDBC MyDbc;
};
typedef std::shared_ptr<mOdbcConnectionHandleContents> mOdbcConnectionHandle;


#endif
