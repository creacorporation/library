//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCSQLSTATE_H_INCLUDED
#define MODBCSQLSTATE_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "../General/mErrorLogger.h"

#include <vector>
#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")


namespace mOdbcSqlState
{
	//ハンドルの種類
	enum HandleKind
	{
		Environment,	//環境ハンドル
		Connection,		//接続ハンドル
		Statement,		//ステートメントハンドル
		Descriotion,	//ディスクリプタハンドル
	};

	//指定したODBCオブジェクトからSQLステートメントを取得する
	//handle : ステートメントを取得するハンドル
	//kind : handleに指定したハンドルの種類
	//ret : 成功時真
	bool AppendLog( SQLHANDLE handle , HandleKind kind , mErrorLogger* logger = &g_ErrorLogger );

};

#endif
