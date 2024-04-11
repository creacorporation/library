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
	//�������̔j��
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
		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.FriendlyName );	//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���
		result &= GetProperty( i , SPDRP_HARDWAREID , entry.HardwareId );		//�R���g���[���p�l���́u�n�[�h�E�G�AID�v�Ɠ���
		entry.DevicePath = MyDevInfoData[ i ].DevicePath;

		if( !result )
		{
			RaiseError( g_ErrorLogger , 0 , L"���擾�����s���܂���" );
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




