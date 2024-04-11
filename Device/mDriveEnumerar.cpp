//----------------------------------------------------------------------------
// �h���C�u�񋓃N���X
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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
		RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X�ꗗ���쐬�ł��܂���" );
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		//��{�I�ȏ����ڍs
		bool result = true;
		CatalogEntry entry;

		entry.index = DWORD( i );
		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.FriendlyName );	//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���
		result &= GetProperty( i , SPDRP_HARDWAREID , entry.HardwareId );		//�R���g���[���p�l���́u�n�[�h�E�G�AID�v�Ɠ���
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
			RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X�p�X���J�����Ƃ��ł��܂���" );
			return false;
		}

		mBinary out_data;
		if( !fp.ExecIoControl( IOCTL_STORAGE_GET_DEVICE_NUMBER_EX , nullptr , &out_data ) )
		{
			//�擾�ł��Ȃ��ꍇ
			entry.DeviceType = 0;
			entry.DeviceNumber = (DWORD)-1;
			entry.PartitionNumber = (DWORD)-1;
			entry.DeviceGuid = { 0 };
			entry.IsDeviceGuidFromHW = false;
			entry.Drive = 0;
		}
		else
		{
			//�擾�ł����ꍇ
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
		//�A���C�ɒǉ�
		MyCatalog.push_back( std::move( entry ) );
	}

	if( !ScanDriveLetter() )
	{
		return false;
	}
	return true;
}

//�h���C�u���^�[�̃X�L����
bool mDriveEnumerar::ScanDriveLetter( void )
{
	DWORD drives = GetLogicalDrives();
	for( wchar_t i = L'A' ; i <= L'Z' ; i++ )
	{
		if( drives & 0x00000001u )
		{
			//���̃h���C�u�͑���
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



