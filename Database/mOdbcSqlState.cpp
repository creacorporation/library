//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MODBCSQLSTATE_CPP_COMPILING
#include "mOdbcSqlState.h"
#include "../General/mErrorLogger.h"
#include "../General/mException.h"


bool mOdbcSqlState::AppendLog( SQLHANDLE handle , HandleKind kind , mErrorLogger* logger )
{
	SQLSMALLINT handletype;

	//ハンドルの種類を判定
	switch( kind )
	{
	case HandleKind::Environment:	//環境ハンドル
		handletype = SQL_HANDLE_ENV;
		break;
	case HandleKind::Connection:	//接続ハンドル
		handletype = SQL_HANDLE_DBC;
		break;
	case HandleKind::Statement:		//ステートメントハンドル
		handletype = SQL_HANDLE_STMT;
		break;
	case HandleKind::Descriotion:	//ディスクリプタハンドル
		handletype = SQL_HANDLE_DESC;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"ハンドルの種別が不正です" );
		return false;
	}
	
	//レコード数の取得
	SQLLEN records = 0;	//格納されているレコード数
	switch( SQLGetDiagFieldW( handletype , handle , 0 , SQL_DIAG_NUMBER , &records , 0 , 0 ) )
	{
	case SQL_SUCCESS:				//取得成功
	case SQL_SUCCESS_WITH_INFO:		//条件付き取得成功
		break;

	case SQL_INVALID_HANDLE:		//ハンドルが無効
	case SQL_ERROR:					//その他エラー
		RaiseAssert( g_ErrorLogger , 0 , L"診断レコード数を取得できません" );
		return false;

	case SQL_NO_DATA:				//データ登録なし
		return true;
	}

	//レコード内容の取得
	SQLWCHAR SqlState[ 6 ];
	SQLWCHAR SqlMsg[ SQL_MAX_MESSAGE_LENGTH ];  
	SQLINTEGER NativeError = 0;
	SQLSMALLINT MsgLen = 0;

	for( SQLLEN i = 1 ; i <= records ; i++ )
	{
		switch( SQLGetDiagRecW( handletype , handle , (SQLSMALLINT)i , SqlState , &NativeError , SqlMsg , sizeof( SqlMsg ) , &MsgLen ) )
		{
		case SQL_SUCCESS:				//取得成功
		case SQL_SUCCESS_WITH_INFO:		//条件付き取得成功
			{
				SetLastError( NativeError );
				RaiseError( g_ErrorLogger , 0 , SqlMsg , SqlState );
			}
			break;

		case SQL_INVALID_HANDLE:		//ハンドルが無効
		case SQL_ERROR:					//その他エラー
			RaiseAssert( g_ErrorLogger , 0 , L"診断レコードを取得できません" );
			return false;

		case SQL_NO_DATA:				//データ登録なし
			break;
		}
	}
	return true;
}

