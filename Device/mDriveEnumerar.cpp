//----------------------------------------------------------------------------
// ドライブ列挙クラス
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mDriveEnumerar.h"
#include <General/mErrorLogger.h>
#include <General/mFile.h>
#include <General/mBinary.h>
#include <winioctl.h>

mDriveEnumerar::mDriveEnumerar()
	: mDeviceEnumerarBase( GUID_DEVINTERFACE_VOLUME , false )
{
}

mDriveEnumerar::~mDriveEnumerar()
{
}

bool mDriveEnumerar::Reload( void )
{
	MyCatalog.clear();

	if( !CreateCatalog() )
	{
		RaiseError( g_ErrorLogger , 0 , L"デバイス一覧を作成できません" );
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		//基本的な情報を移行
		bool result = true;
		CatalogEntry entry;

		entry.index = DWORD( i );
		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.FriendlyName );	//コントロールパネルの「フレンドリ名」と同じ
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//コントロールパネルの「デバイスの説明」と同じ
		result &= GetProperty( i , SPDRP_HARDWAREID , entry.HardwareId );		//コントロールパネルの「ハードウエアID」と同じ
		entry.DevicePath = ToLower( MyDevInfoData[ i ].DevicePath );

		mFile::Option op;
		op.Path = entry.DevicePath;
		op.AccessRead = false;
		op.AccessWrite = false;
		op.Mode = mFile::CreateMode::OpenExisting;
		op.ShareRead = true;
		op.ShareWrite = true;

		mFile fp;
		if( !fp.Open( op ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"デバイスパスを開くことができません" );
			return false;
		}

		mBinary out_data;
		if( !fp.ExecIoControl( IOCTL_STORAGE_GET_DEVICE_NUMBER_EX , nullptr , &out_data ) )
		{
			//取得できない場合
			entry.DeviceType = 0;
			entry.DeviceNumber = (DWORD)-1;
			entry.PartitionNumber = (DWORD)-1;
			entry.DeviceGuid = { 0 };
			entry.IsDeviceGuidFromHW = false;
			entry.Drive = 0;
		}
		else
		{
			//取得できた場合
			const STORAGE_DEVICE_NUMBER_EX* devnum = reinterpret_cast< const STORAGE_DEVICE_NUMBER_EX* >( out_data.data() );
			entry.DeviceType = devnum->DeviceType;
			entry.DeviceNumber = devnum->DeviceNumber;
			entry.PartitionNumber = devnum->PartitionNumber;
			entry.DeviceGuid = devnum->DeviceGuid;
			if( ( ( devnum->Flags & STORAGE_DEVICE_FLAGS_RANDOM_DEVICEGUID_REASON_NOHWID ) != 0 ) ||
				( ( devnum->Flags & STORAGE_DEVICE_FLAGS_RANDOM_DEVICEGUID_REASON_CONFLICT ) != 0 ) ||
				( ( devnum->Flags & STORAGE_DEVICE_FLAGS_PAGE_83_DEVICEGUID ) == 0 ) )
			{
				entry.IsDeviceGuidFromHW = false;
			}
			else
			{
				entry.IsDeviceGuidFromHW = true;
			}
			entry.Drive = 0;
		}

		{
			DWORD buffsize = 0;
			WString path = entry.DevicePath;
			DWORD ec = 0;
			path.append( L"\\" );
			if( !GetVolumeNameForVolumeMountPointW( path.c_str() , nullptr , 0 ) )
			{
				ec = GetLastError();
			}
			
			std::unique_ptr< wchar_t[] > buffer( mNew wchar_t[ 50 ] );
			GetVolumeNameForVolumeMountPointW( path.c_str() , buffer.get() , 50 );

			buffsize++;
		}
		//アレイに追加
		MyCatalog.push_back( std::move( entry ) );
	}

	if( !ScanDriveLetter() )
	{
		return false;
	}
	return true;
}

//ドライブレターのスキャン
bool mDriveEnumerar::ScanDriveLetter( void )
{
	DWORD drives = GetLogicalDrives();
	for( wchar_t i = L'A' ; i <= L'Z' ; i++ )
	{
		if( drives & 0x00000001u )
		{
			//このドライブは存在
			mFile::Option op;
			op.Path = makeprintf( LR"(\\.\%c:)" , i );
			op.AccessRead = false;
			op.AccessWrite = 0;
			op.Mode = mFile::CreateMode::OpenExisting;
			op.ShareRead = true;
			op.ShareWrite = true;

			mFile fp;
			if( fp.Open( op ) )
			{
				mBinary out_data;
				if( fp.ExecIoControl( IOCTL_STORAGE_GET_DEVICE_NUMBER_EX , nullptr , &out_data ) )
				{
					const STORAGE_DEVICE_NUMBER_EX* devnum = reinterpret_cast< const STORAGE_DEVICE_NUMBER_EX* >( out_data.data() );

					for( Catalog::iterator itr = MyCatalog.begin() ; itr != MyCatalog.end() ; itr++ )
					{
						if( ( itr->DeviceGuid == devnum->DeviceGuid ) &&
							( itr->PartitionNumber == devnum->PartitionNumber ) )
						{
							itr->Drive = i;
							break;
						}
					}
				}
			}
		}
		drives >>= 1;
	}
	return true;
}


bool mDriveEnumerar::GetCatalog( Catalog& retCatalog , bool reload )
{
	if( reload || MyCatalog.empty() )
	{
		if( !Reload() )
		{
			retCatalog.clear();
			return false;
		}
	}
	retCatalog = MyCatalog;
	return true;
}

const mDriveEnumerar::Catalog& mDriveEnumerar::GetCatalog( bool reload )
{
	if( reload || MyCatalog.empty() )
	{
		Reload();
	}
	return MyCatalog;
}

const mDriveEnumerar::CatalogEntry* mDriveEnumerar::GetFromDevicePath( const WString DevicePath )const
{
	WString tmppath = ToLower( DevicePath );
	for( Catalog::const_iterator itr = MyCatalog.begin() ; itr != MyCatalog.end() ; itr++ )
	{
		if( itr->DevicePath == tmppath )
		{
			return &(*itr);
		}
	}
	return nullptr;
}



