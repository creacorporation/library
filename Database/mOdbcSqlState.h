//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCSQLSTATE_H_INCLUDED
#define MODBCSQLSTATE_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "../General/mErrorLogger.h"

#include <vector>
#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")


namespace mOdbcSqlState
{
	//�n���h���̎��
	enum HandleKind
	{
		Environment,	//���n���h��
		Connection,		//�ڑ��n���h��
		Statement,		//�X�e�[�g�����g�n���h��
		Descriotion,	//�f�B�X�N���v�^�n���h��
	};

	//�w�肵��ODBC�I�u�W�F�N�g����SQL�X�e�[�g�����g���擾����
	//handle : �X�e�[�g�����g���擾����n���h��
	//kind : handle�Ɏw�肵���n���h���̎��
	//ret : �������^
	bool AppendLog( SQLHANDLE handle , HandleKind kind , mErrorLogger* logger = &g_ErrorLogger );

};

#endif
