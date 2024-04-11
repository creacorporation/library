//----------------------------------------------------------------------------
// �f�o�C�X�񋓃N���X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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
		RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X�ꗗ���쐬�ł��܂���" );
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		//��{�I�ȏ����ڍs
		bool result = true;
		SerialPortCatalogEntry entry;

		entry.index = DWORD( i );
		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.FriendlyName );	//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���
		result &= GetProperty( i , SPDRP_HARDWAREID , entry.HardwareId );		//�R���g���[���p�l���́u�n�[�h�E�G�AID�v�Ɠ���
		entry.DevicePath = MyDevInfoData[ i ].DevicePath;

		//�|�[�g��������
		mDeviceRegistry reg;
		if( !GetDeviceRegistry( i , reg ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�V���A���|�[�g�̃��W�X�g���ɃA�N�Z�X�ł��܂���" , i );
			return false;
		}
		//���|�[�g�������Ȃ��ꍇ�ł��A�{���ɃA�T�C������Ă��Ȃ��̂�������Ȃ�����󔒂̂܂܂ɂ��Ă���
		entry.PortName = reg.GetString( L"PortName" , L"" );

		//�A���C�ɒǉ�
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
