//----------------------------------------------------------------------------
// Bluetoothハンドラクラス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mBluetoothEnumerar.h"
#include "mDeviceEnumerar.h"
#include "General/mErrorLogger.h"
#include <bluetoothapis.h>
#include <bluetoothleapis.h>

#pragma comment(lib, "Bthprops.lib")
#pragma comment(lib, "BluetoothAPIs.lib" )

//インターフェイス経由でClassicデバイスの一覧を取る例
//  mDeviceEnumerar dev( GUID_BTH_RFCOMM_SERVICE_DEVICE_INTERFACE , false );
//インターフェイス経由でBLEデバイスの一覧を取る例
//  mDeviceEnumerar dev( GUID_BLUETOOTHLE_DEVICE_INTERFACE , false );

mBluetoothEnumerar::mBluetoothEnumerar()
{
}

mBluetoothEnumerar::~mBluetoothEnumerar()
{
}

bool mBluetoothEnumerar::SearchDevice( BluetoothDeviceCatalog& retCatalog , bool authed , bool remembered , bool unknown , bool connected , bool issue_inquiry , DWORD timeout_ms )
{
	retCatalog.clear();

	BLUETOOTH_DEVICE_SEARCH_PARAMS params;
	ZeroMemory( &params , sizeof( params ) ); 
	params.dwSize =  sizeof( params );
	params.fReturnAuthenticated = authed;
	params.fReturnRemembered = remembered;
	params.fReturnUnknown = unknown;
	params.fReturnConnected = connected;
	params.fIssueInquiry = issue_inquiry;
	if( issue_inquiry )
	{
		//128単位に切り上げ
		DWORD multiplier = ( timeout_ms + 127 ) / 128;
		if( multiplier == 0 )
		{
			params.cTimeoutMultiplier = 1;
		}
		else if( 48 < multiplier )
		{
			//待ち時間の最大値は48
			params.cTimeoutMultiplier = 48;
		}
		else
		{
			params.cTimeoutMultiplier = (UCHAR)multiplier;
		}
	}
	else
	{
		params.cTimeoutMultiplier = 0;
	}

	BLUETOOTH_DEVICE_INFO devinfo;
	devinfo.dwSize = sizeof( devinfo );

	HBLUETOOTH_DEVICE_FIND devfind = BluetoothFindFirstDevice( &params , &devinfo );
	while( devfind != nullptr )
	{
		BluetoothDeviceCatalogEntry entry;
		entry.Address = devinfo.Address.ullLong;
		entry.Name = devinfo.szName;
		retCatalog.push_back( entry );

		if( !BluetoothFindNextDevice( devfind , &devinfo ) )
		{
			BluetoothFindDeviceClose( devfind );
			devfind = nullptr;
		}
	}
	return true;
}

bool mBluetoothEnumerar::SearchRadio( BluetoothRadioCatalog& retCatalog )
{
	retCatalog.clear();

	BLUETOOTH_FIND_RADIO_PARAMS params;
	ZeroMemory( &params , sizeof( params ) ); 
	params.dwSize =  sizeof( params );

	HANDLE devhandle;
	HBLUETOOTH_RADIO_FIND devfind = BluetoothFindFirstRadio( &params , &devhandle );
	while( devfind != nullptr )
	{
		BLUETOOTH_RADIO_INFO devinfo;
		devinfo.dwSize = sizeof( devinfo );
		
		if( BluetoothGetRadioInfo( devhandle , &devinfo ) == ERROR_SUCCESS )
		{
			BluetoothRadioCatalogEntry entry;
			entry.Name = devinfo.szName;
			entry.Address = devinfo.address.ullLong;
			retCatalog.push_back( entry );

			CloseHandle( devhandle );
		}

		if( !BluetoothFindNextRadio( devfind , &devhandle ) )
		{
			BluetoothFindRadioClose( devfind );
			devfind = nullptr;
		}
	}
	return true;
}


static bool GetGATTServiceCharacteristic( HANDLE handle , const BTH_LE_GATT_SERVICE& svc , mBluetoothEnumerar::BluetoothLECatalogEntryBase& retentry )
{
	HRESULT hr;	//結果コード
	USHORT sz;	//バッファサイズ

	hr = BluetoothGATTGetCharacteristics( handle , const_cast<PBTH_LE_GATT_SERVICE>( &svc ) , 0 , nullptr , &sz , BLUETOOTH_GATT_FLAG_NONE );
	if( hr != HRESULT_FROM_WIN32( ERROR_MORE_DATA ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"BLEデバイスの詳細情報エントリ数を取得できません" );
		return false;
	}

	using GattCharacteristics = std::vector< BTH_LE_GATT_CHARACTERISTIC >;
	GattCharacteristics gatt_characteristics;
	gatt_characteristics.resize( sz );

	hr = BluetoothGATTGetCharacteristics( handle , const_cast<PBTH_LE_GATT_SERVICE>( &svc ) , sz , gatt_characteristics.data() , &sz , BLUETOOTH_GATT_FLAG_NONE );
	if( hr != S_OK )
	{
		RaiseError( g_ErrorLogger , 0 , L"BLEデバイスのサービスを取得できません" );
		return false;
	}

	//各エントリの詳細情報を取得する
	for( GattCharacteristics::const_iterator gattitr = gatt_characteristics.begin() ; gattitr != gatt_characteristics.end() ; gattitr++ )
	{
		if( !gattitr->IsReadable )
		{
			continue;
		}
		hr = BluetoothGATTGetCharacteristicValue( handle , const_cast<PBTH_LE_GATT_CHARACTERISTIC>( &(*gattitr) ) , 0 , nullptr , &sz , BLUETOOTH_GATT_FLAG_NONE );
		if( hr != HRESULT_FROM_WIN32( ERROR_MORE_DATA ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"BLEデバイスのサービスのエントリ数を取得できません" );
			return false;
		}

		BTH_LE_GATT_CHARACTERISTIC_VALUE* ptr = (BTH_LE_GATT_CHARACTERISTIC_VALUE*) mNew BYTE[ sz ]; 
		hr = BluetoothGATTGetCharacteristicValue( handle , const_cast<PBTH_LE_GATT_CHARACTERISTIC>( &(*gattitr) ) , sz , ptr , &sz , BLUETOOTH_GATT_FLAG_NONE );


		mDelete[] ptr;

	}

	return true;
}

static bool GetGATTServiceInfo( HANDLE handle , mBluetoothEnumerar::BluetoothLECatalogEntryBase& retentry )
{
	HRESULT hr;	//結果コード
	USHORT sz;	//バッファサイズ
	
	//サービスの一覧を取得する
	hr = BluetoothGATTGetServices( handle , 0 , nullptr , &sz , BLUETOOTH_GATT_FLAG_NONE );
	if( hr != HRESULT_FROM_WIN32( ERROR_MORE_DATA ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"BLEデバイスのサービスのエントリ数を取得できません" );
		return false;
	}

	using GattServices = std::vector< BTH_LE_GATT_SERVICE >;
	GattServices gatt_services;
	gatt_services.resize( sz );

	hr = BluetoothGATTGetServices( handle , sz , gatt_services.data() , &sz , BLUETOOTH_GATT_FLAG_NONE );
	if( hr != S_OK )
	{
		RaiseError( g_ErrorLogger , 0 , L"BLEデバイスのサービスを取得できません" );
		return false;
	}

	//各サービスの詳細情報を取得する
	for( GattServices::const_iterator gattitr = gatt_services.begin() ; gattitr != gatt_services.end() ; gattitr++ )
	{
		if( !GetGATTServiceCharacteristic( handle , *gattitr , retentry ) )
		{

		}
	}
	return true;
}

//void mBluetoothEnumerar::Test( void )
//{
//	mDeviceEnumerar dev( GUID_BLUETOOTHLE_DEVICE_INTERFACE , false );
//	mDeviceEnumerar::DeviceCatalog info;
//	if( !dev.GetDeviceCatalog( info ) )
//	{
//		return;
//	}
//
//	for( mDeviceEnumerar::DeviceCatalog::const_iterator infoitr = info.begin() ; infoitr != info.end() ; infoitr++ )
//	{
//		HANDLE handle = CreateFileW( infoitr->DevicePath.c_str() , GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
//		if( handle == INVALID_HANDLE_VALUE )
//		{
//			continue;
//		}
//
//		BluetoothLECatalogEntryBase entry;
//		GetGATTServiceInfo( handle , entry );
//		CloseHandle( handle );
//	}
//	
//	BLUETOOTH_DEVICE_INFO btinfo;
//	ZeroMemory( &btinfo , sizeof( btinfo ) );
//	btinfo.dwSize = sizeof( btinfo );
//	HANDLE handle = 0;
//	DWORD rc = 0;
//	if( info.size() )
//	{
//		handle = CreateFileW( info[0].DevicePath.c_str() , GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
//		rc = BluetoothGetDeviceInfo( handle , &btinfo );
//
//
//
//	}
//
//}
//


