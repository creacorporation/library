//----------------------------------------------------------------------------
// �f�o�C�X�񋓃N���X
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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
		RaiseAssert( g_ErrorLogger , 0 , L"�f�o�C�X�N���X�̕����񂩂�GUID�ւ̕ϊ������s���܂���" );
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

//is_setup_class : true = guid �̓Z�b�g�A�b�v�N���X�̂���
//                 false = guid �̓C���^�[�t�F�C�X�N���X�̂���
static bool OpenHandle( HDEVINFO &io_handle , const GUID& guid , bool is_setup_class )
{
	//�O�̃n���h���폜
	if( io_handle != INVALID_HANDLE_VALUE )
	{
		SetupDiDestroyDeviceInfoList( io_handle );
		io_handle = INVALID_HANDLE_VALUE;
	}

	//�V�����n���h���쐬
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
	//�������̔j��
	MyDevInfoData.clear();

	//�n���h���̊m��
	if( !OpenHandle( MyHandle , MyClassGuid , MyIsSetupClass ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n���h�����擾�ł��܂���" );
		return false;
	}

	//���擾
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
				RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X�񋓎��ɃG���[���������܂���" );
				return false;
			}
		}

		DWORD reqsize = 0;
		if( !SetupDiGetDeviceInterfaceDetailW( MyHandle , &devinterface , nullptr , 0 , &reqsize , nullptr ) )
		{
			if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
			{
				RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X�C���^�[�t�F�C�X���擾���ɃG���[���������܂���" );
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

		//�擾�ł����̂ŃG���g���ɒǉ�
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
	//�������̔j��
	MyDevInfoData.clear();

	//�n���h���̊m��
	if( !OpenHandle( MyHandle ,  MyClassGuid , MyIsSetupClass ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n���h�����擾�ł��܂���" );
		return false;
	}

	//���擾
	DWORD index = 0;
	while( 1 )
	{
		SP_DEVINFO_DATA info_data;
		ZeroMemory( &info_data , sizeof( info_data ) );
		info_data.cbSize = sizeof(SP_DEVINFO_DATA);

		//SP_DEVINFO_DATA�Ȃ���̂��擾
		if( !SetupDiEnumDeviceInfo( MyHandle , index , &info_data ) )
		{
			DWORD ec = GetLastError();
			if( ec == ERROR_NO_MORE_ITEMS )
			{
				break;
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X���擾���ɃG���[���������܂���" );
				return false;
			}
		}

		//�擾�ł����̂ŃG���g���ɒǉ�
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
	//�n���h���̊m��
	if( !OpenHandle( MyHandle , MyClassGuid , MyIsSetupClass ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n���h�����擾�ł��܂���" );
		return false;
	}

	if( reload )
	{
		MyDevInfoData.clear();
	}

	//������񂪂Ȃ���΍\�z
	if( MyDevInfoData.size() == 0 )
	{
		bool ret = false;
		if( MyIsSetupClass )
		{
			if( !BuildDevInfoData_Setup() )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�����f�[�^(Setup)���\�z�ł��܂���ł���" );
				return false;
			}
		}
		else
		{
			if( !BuildDevInfoData_Interface() )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�����f�[�^(Interface)���\�z�ł��܂���ł���" );
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
			RaiseError( g_ErrorLogger , 0 , L"�K�v�ȃo�b�t�@�T�C�Y�𔻒�ł��܂���" , prop_id );
			return false;
		}
	}
	std::unique_ptr<BYTE> dt( mNew BYTE[ required_size ] );
	if( !SetupDiGetDeviceRegistryPropertyW( MyHandle , &const_cast<SP_DEVINFO_DATA&>( info ) , prop_id , nullptr , dt.get() , required_size , &required_size ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�f�[�^���擾�ł��܂���" , prop_id );
		return false;
	}
	retProp = (wchar_t*)dt.get();
	return true;
}

bool mDeviceEnumerarBase::GetProperty( DWORD index , DWORD prop_id , WString& retProp)const
{
	if( MyDevInfoData.size() <= index )
	{
		RaiseError( g_ErrorLogger , 0 , L"�C���f�b�N�X���͈͊O�ł�" , index );
		return false;
	}
	return GetProperty( MyDevInfoData[ index ].DevInfo , prop_id , retProp );
}

bool mDeviceEnumerarBase::GetDeviceRegistry( DWORD index , mDeviceRegistry& retReg )const
{
	retReg.Close();

	//�C���f�b�N�X�`�F�b�N
	if( MyDevInfoData.size() <= index )
	{
		RaiseError( g_ErrorLogger , 0 , L"�C���f�b�N�X���f�o�C�X�̑����𒴂��Ă��܂�" , index );
		return false;
	}

	//���W�X�g���L�[�̊l��
	if( !retReg.Open( MyHandle , MyDevInfoData[ index ].DevInfo ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���W�X�g���L�[���擾�ł��܂���" );
		return false;
	}
	return true;

}
