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
#include "mDeviceEnumerarBase.h"
#include "General/mErrorLogger.h"

#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")


mDeviceEnumerarBase::mDeviceEnumerarBase( const WString& ClassName , bool IsSetupClass )
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyIsSetupClass = IsSetupClass;

	DWORD guid_size = 0;
	if( !SetupDiClassGuidsFromNameW( ClassName.c_str() , &MyClassGuid , 1 , &guid_size ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"デバイスクラスの文字列からGUIDへの変換が失敗しました" );
		MyClassGuid = { 0 };
	}
	return;
}

mDeviceEnumerarBase::mDeviceEnumerarBase( const GUID& ClassGuid , bool IsSetupClass )
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyClassGuid = ClassGuid;
	MyIsSetupClass = IsSetupClass;
	return;
}

mDeviceEnumerarBase::~mDeviceEnumerarBase()
{
	FreeDevHandle();
	return;
}

void mDeviceEnumerarBase::FreeDevHandle( void )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		SetupDiDestroyDeviceInfoList( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	return;
}

//is_setup_class : true = guid はセットアップクラスのもの
//                 false = guid はインターフェイスクラスのもの
static bool OpenHandle( HDEVINFO &io_handle , const GUID& guid , bool is_setup_class )
{
	//前のハンドル削除
	if( io_handle != INVALID_HANDLE_VALUE )
	{
		SetupDiDestroyDeviceInfoList( io_handle );
		io_handle = INVALID_HANDLE_VALUE;
	}

	//新しいハンドル作成
	DWORD flags = DIGCF_PRESENT | DIGCF_PROFILE;
	if( !is_setup_class )
	{
		flags |= DIGCF_DEVICEINTERFACE;
	}
	io_handle = SetupDiGetClassDevsW( &guid , 0 , 0 , flags );
	return io_handle != INVALID_HANDLE_VALUE;
}

bool mDeviceEnumerarBase::BuildDevInfoData_Interface( void )
{
	//既存情報の破棄
	MyDevInfoData.clear();

	//ハンドルの確保
	if( !OpenHandle( MyHandle , MyClassGuid , MyIsSetupClass ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハンドルを取得できません" );
		return false;
	}

	//情報取得
	DWORD index = 0;
	while( 1 )
	{
		SP_DEVICE_INTERFACE_DATA devinterface;
		ZeroMemory( &devinterface , sizeof( devinterface ) );
		devinterface.cbSize = sizeof( devinterface );
		if( !SetupDiEnumDeviceInterfaces( MyHandle , nullptr , &MyClassGuid , index , &devinterface ) )
		{
			DWORD ec = GetLastError();
			if( ec == ERROR_NO_MORE_ITEMS )
			{
				break;
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"デバイス列挙時にエラーが発生しました" );
				return false;
			}
		}

		DWORD reqsize = 0;
		if( !SetupDiGetDeviceInterfaceDetailW( MyHandle , &devinterface , nullptr , 0 , &reqsize , nullptr ) )
		{
			if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
			{
				RaiseError( g_ErrorLogger , 0 , L"デバイスインターフェイス情報取得時にエラーが発生しました" );
				return false;
			}
		}

		SP_DEVINFO_DATA info_data;
		ZeroMemory( &info_data , sizeof( info_data ) );
		info_data.cbSize = sizeof(info_data);

		PSP_DEVICE_INTERFACE_DETAIL_DATA devdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)mNew BYTE[ reqsize ];
		ZeroMemory( devdetail , sizeof( devdetail ) );
		devdetail->cbSize = sizeof( devdetail );

		SetupDiGetDeviceInterfaceDetailW( MyHandle , &devinterface , devdetail , reqsize , &reqsize , &info_data );

		//取得できたのでエントリに追加
		DevInfoDataEntry entry;
		entry.DevInfo = info_data;
		entry.DevicePath = devdetail->DevicePath;
		MyDevInfoData.push_back( entry );

		mDelete[] devdetail;
		index++;
	}
	return true;
}

bool mDeviceEnumerarBase::BuildDevInfoData_Setup( void )
{
	//既存情報の破棄
	MyDevInfoData.clear();

	//ハンドルの確保
	if( !OpenHandle( MyHandle ,  MyClassGuid , MyIsSetupClass ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハンドルを取得できません" );
		return false;
	}

	//情報取得
	DWORD index = 0;
	while( 1 )
	{
		SP_DEVINFO_DATA info_data;
		ZeroMemory( &info_data , sizeof( info_data ) );
		info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		//SP_DEVINFO_DATAなるものを取得
		if( !SetupDiEnumDeviceInfo( MyHandle , index , &info_data ) )
		{
			DWORD ec = GetLastError();
			if( ec == ERROR_NO_MORE_ITEMS )
			{
				break;
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"デバイス情報取得時にエラーが発生しました" );
				return false;
			}
		}

		//取得できたのでエントリに追加
		DevInfoDataEntry entry;
		entry.DevInfo = info_data;
		entry.DevicePath = L"";
		MyDevInfoData.push_back( entry );
		index++;
	}
	return true;
}

bool mDeviceEnumerarBase::CreateCatalog( bool reload )
{
	//ハンドルの確保
	if( !OpenHandle( MyHandle , MyClassGuid , MyIsSetupClass ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハンドルを取得できません" );
		return false;
	}

	if( reload )
	{
		MyDevInfoData.clear();
	}

	//内部情報がなければ構築
	if( MyDevInfoData.size() == 0 )
	{
		bool ret = false;
		if( MyIsSetupClass )
		{
			if( !BuildDevInfoData_Setup() )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"内部データ(Setup)を構築できませんでした" );
				return false;
			}
		}
		else
		{
			if( !BuildDevInfoData_Interface() )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"内部データ(Interface)を構築できませんでした" );
				return false;
			}
		}
	}
	return true;
}

bool mDeviceEnumerarBase::GetProperty( const SP_DEVINFO_DATA& info , DWORD prop_id , WString& retProp)const
{
	DWORD required_size = 0;
	if( !SetupDiGetDeviceRegistryPropertyW( MyHandle , &const_cast<SP_DEVINFO_DATA&>( info ) , prop_id , nullptr , nullptr , required_size , &required_size ) )
	{
		if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
		{
			RaiseError( g_ErrorLogger , 0 , L"必要なバッファサイズを判定できません" , prop_id );
			return false;
		}
	}
	std::unique_ptr<BYTE> dt( mNew BYTE[ required_size ] );
	if( !SetupDiGetDeviceRegistryPropertyW( MyHandle , &const_cast<SP_DEVINFO_DATA&>( info ) , prop_id , nullptr , dt.get() , required_size , &required_size ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"データを取得できません" , prop_id );
		return false;
	}
	retProp = reinterpret_cast<wchar_t*>( dt.get() );
	return true;
}

bool mDeviceEnumerarBase::GetProperty( DWORD index , DWORD prop_id , WString& retProp)const
{
	if( MyDevInfoData.size() <= index )
	{
		RaiseError( g_ErrorLogger , 0 , L"インデックスが範囲外です" , index );
		return false;
	}
	return GetProperty( MyDevInfoData[ index ].DevInfo , prop_id , retProp );
}

bool mDeviceEnumerarBase::GetDeviceRegistry( DWORD index , mDeviceRegistry& retReg )const
{
	retReg.Close();

	//インデックスチェック
	if( MyDevInfoData.size() <= index )
	{
		RaiseError( g_ErrorLogger , 0 , L"インデックスがデバイスの総数を超えています" , index );
		return false;
	}

	//レジストリキーの獲得
	if( !retReg.Open( MyHandle , MyDevInfoData[ index ].DevInfo ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"レジストリキーを取得できません" );
		return false;
	}
	return true;

}
