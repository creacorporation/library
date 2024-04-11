//----------------------------------------------------------------------------
// COMオブジェクト用ユーティリティ関数群
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#include "mComObject.h"
#include "General/mErrorLogger.h"

mComObject::mComObject()
{
	HRESULT result = CoInitializeEx( 0 , COINIT_APARTMENTTHREADED );
	if( result == S_OK )
	{
		//新たに初期化を行った場合
		CreateLogEntry( g_ErrorLogger , 0 , L"COMを初期化しました" );
		MyIsInitialized = true;
		CoInitializeSecurity(NULL, -1, NULL, NULL,
								RPC_C_AUTHN_LEVEL_DEFAULT,
								RPC_C_IMP_LEVEL_IMPERSONATE,
								NULL, EOAC_NONE, NULL);
	}
	else if( result == S_FALSE )
	{
		//すでに初期化済みだった場合
		MyIsInitialized = true;
	}
	else
	{
		//エラーの場合
		RaiseError( g_ErrorLogger , 0 , L"COMの初期化が失敗しました" , result );
		MyIsInitialized = false;
	}
}

mComObject::~mComObject()
{
	if( MyIsInitialized )
	{
		CoUninitialize();
	}
}


