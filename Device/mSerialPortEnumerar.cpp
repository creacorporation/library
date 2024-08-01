//----------------------------------------------------------------------------
// デバイス列挙クラス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mSerialPortEnumerar.h"
#include "General/mErrorLogger.h"

mSerialPortEnumerar::mSerialPortEnumerar( bool IsSetupClass ) 
	: mDeviceEnumerarBase( GUID_DEVCLASS_PORTS , IsSetupClass )
{
	return;
}

mSerialPortEnumerar::~mSerialPortEnumerar()
{
	return;
}

bool mSerialPortEnumerar::Reload( void )
{
	MySerialPortCatalog.clear();

	if( !CreateCatalog() )
	{
		RaiseError( g_ErrorLogger , 0 , L"デバイス一覧を作成できません" );
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		//基本的な情報を移行
		bool result = true;
		SerialPortCatalogEntry entry;

		entry.index = DWORD( i );
		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.FriendlyName );	//コントロールパネルの「フレンドリ名」と同じ
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//コントロールパネルの「デバイスの説明」と同じ
		result &= GetProperty( i , SPDRP_HARDWAREID , entry.HardwareId );		//コントロールパネルの「ハードウエアID」と同じ
		entry.DevicePath = MyDevInfoData[ i ].DevicePath;

		//ポート名を検索
		mDeviceRegistry reg;
		if( !GetDeviceRegistry( i , reg ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"シリアルポートのレジストリにアクセスできません" , i );
			return false;
		}
		//※ポート名が取れない場合でも、本当にアサインされていないのかもしれないから空白のままにしておく
		entry.PortName = reg.GetString( L"PortName" , L"" );

		//アレイに追加
		MySerialPortCatalog.push_back( entry );
	}
	return true;
}

bool mSerialPortEnumerar::GetSerialPortCatalog( SerialPortCatalog& retinfo , bool reload )
{
	if( reload || MySerialPortCatalog.empty() )
	{
		if( !Reload() )
		{
			retinfo.clear();
			return false;
		}
	}
	retinfo = MySerialPortCatalog;
	return true;
}

const mSerialPortEnumerar::SerialPortCatalog& mSerialPortEnumerar::GetSerialPortCatalog( bool reload )
{
	if( reload || MySerialPortCatalog.empty() )
	{
		Reload();
	}
	return MySerialPortCatalog;
}
