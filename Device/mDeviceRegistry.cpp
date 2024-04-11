//----------------------------------------------------------------------------
// �f�o�C�X�p���W�X�g���n���h��
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[�̃n���h�����擾�ł��܂���ł���" );
		return false;
	}
	return true;
}
