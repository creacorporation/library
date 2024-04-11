//----------------------------------------------------------------------------
// デバイス用レジストリハンドル
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mDeviceRegistry.h"
#include <General/mErrorLogger.h>

mDeviceRegistry::mDeviceRegistry()
{
}

mDeviceRegistry::~mDeviceRegistry()
{
}

bool mDeviceRegistry::Open( HDEVINFO devinfo , const SP_DEVINFO_DATA& infodata )
{
	SP_DEVINFO_DATA tmpinfo = infodata;

	MyKey = SetupDiOpenDevRegKey( devinfo , &tmpinfo , DICS_FLAG_GLOBAL , 0 , DIREG_DEV , KEY_READ );
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーのハンドルを取得できませんでした" );
		return false;
	}
	return true;
}
