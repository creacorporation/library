//----------------------------------------------------------------------------
// COM�I�u�W�F�N�g�p���[�e�B���e�B�֐��Q
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#include "mComObject.h"
#include "General/mErrorLogger.h"

mComObject::mComObject()
{
	HRESULT result = CoInitializeEx( 0 , COINIT_APARTMENTTHREADED );
	if( result == S_OK )
	{
		//�V���ɏ��������s�����ꍇ
		CreateLogEntry( g_ErrorLogger , 0 , L"COM�����������܂���" );
		MyIsInitialized = true;
		CoInitializeSecurity(NULL, -1, NULL, NULL,
								RPC_C_AUTHN_LEVEL_DEFAULT,
								RPC_C_IMP_LEVEL_IMPERSONATE,
								NULL, EOAC_NONE, NULL);
	}
	else if( result == S_FALSE )
	{
		//���łɏ������ς݂������ꍇ
		MyIsInitialized = true;
	}
	else
	{
		//�G���[�̏ꍇ
		RaiseError( g_ErrorLogger , 0 , L"COM�̏����������s���܂���" , result );
		MyIsInitialized = false;
	}
}

mComObject::~mComObject()
{
	if( MyIsInitialized )
	{
		CoUninitialize();
	}
}


