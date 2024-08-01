//----------------------------------------------------------------------------
// デバイス列挙クラス
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mDeviceEnumerar.h"
#include "General/mErrorLogger.h"

mDeviceEnumerar::mDeviceEnumerar( const WString& ClassName , bool IsSetupClass )
	: mDeviceEnumerarBase( ClassName , IsSetupClass )
{
}

mDeviceEnumerar::mDeviceEnumerar( const GUID& ClassGuid , bool IsSetupClass )
	: mDeviceEnumerarBase( ClassGuid , IsSetupClass )
{
}

mDeviceEnumerar::~mDeviceEnumerar()
{
}

bool mDeviceEnumerar::Reload( void )
{
	//既存情報の破棄
	MyDeviceCatalog.clear();

	if( !CreateCatalog( true ) )
	{
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		bool result = true;
		DeviceCatalogEntry entry;

		entry.index = DWORD( i );
		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.FriendlyName );	//コントロールパネルの「フレンドリ名」と同じ
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//コントロールパネルの「デバイスの説明」と同じ
		result &= GetProperty( i , SPDRP_HARDWAREID , entry.HardwareId );		//コントロールパネルの「ハードウエアID」と同じ
		entry.DevicePath = MyDevInfoData[ i ].DevicePath;

		if( !result )
		{
			RaiseError( g_ErrorLogger , 0 , L"情報取得が失敗しました" );
			return false;
		}
		MyDeviceCatalog.push_back( entry );
	}
	return true;
}

bool mDeviceEnumerar::GetDeviceCatalog( DeviceCatalog& retCatalog , bool reload )
{
	if( reload || MyDeviceCatalog.empty() )
	{
		if( !Reload() )
		{
			retCatalog.clear();
			return false;
		}
	}
	retCatalog = MyDeviceCatalog;
	return true;
}

const mDeviceEnumerar::DeviceCatalog& mDeviceEnumerar::GetDeviceCatalog( bool reload )
{
	if( reload || MyDeviceCatalog.empty() )
	{
		Reload();
	}
	return MyDeviceCatalog;
}




