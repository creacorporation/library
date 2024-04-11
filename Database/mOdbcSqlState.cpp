//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MODBCSQLSTATE_CPP_COMPILING
#include "mOdbcSqlState.h"
#include "../General/mErrorLogger.h"
#include "../General/mException.h"


bool mOdbcSqlState::AppendLog( SQLHANDLE handle , HandleKind kind , mErrorLogger* logger )
{
	SQLSMALLINT handletype;

	//�n���h���̎�ނ𔻒�
	switch( kind )
	{
	case HandleKind::Environment:	//���n���h��
		handletype = SQL_HANDLE_ENV;
		break;
	case HandleKind::Connection:	//�ڑ��n���h��
		handletype = SQL_HANDLE_DBC;
		break;
	case HandleKind::Statement:		//�X�e�[�g�����g�n���h��
		handletype = SQL_HANDLE_STMT;
		break;
	case HandleKind::Descriotion:	//�f�B�X�N���v�^�n���h��
		handletype = SQL_HANDLE_DESC;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�n���h���̎�ʂ��s���ł�" );
		return false;
	}
	
	//���R�[�h���̎擾
	SQLLEN records = 0;	//�i�[����Ă��郌�R�[�h��
	switch( SQLGetDiagFieldW( handletype , handle , 0 , SQL_DIAG_NUMBER , &records , 0 , 0 ) )
	{
	case SQL_SUCCESS:				//�擾����
	case SQL_SUCCESS_WITH_INFO:		//�����t���擾����
		break;

	case SQL_INVALID_HANDLE:		//�n���h��������
	case SQL_ERROR:					//���̑��G���[
		RaiseAssert( g_ErrorLogger , 0 , L"�f�f���R�[�h�����擾�ł��܂���" );
		return false;

	case SQL_NO_DATA:				//�f�[�^�o�^�Ȃ�
		return true;
	}

	//���R�[�h���e�̎擾
	SQLWCHAR SqlState[ 6 ];
	SQLWCHAR SqlMsg[ SQL_MAX_MESSAGE_LENGTH ];  
	SQLINTEGER NativeError = 0;
	SQLSMALLINT MsgLen = 0;

	for( SQLLEN i = 1 ; i <= records ; i++ )
	{
		switch( SQLGetDiagRecW( handletype , handle , (SQLSMALLINT)i , SqlState , &NativeError , SqlMsg , sizeof( SqlMsg ) , &MsgLen ) )
		{
		case SQL_SUCCESS:				//�擾����
		case SQL_SUCCESS_WITH_INFO:		//�����t���擾����
			{
				SetLastError( NativeError );
				RaiseError( g_ErrorLogger , 0 , SqlMsg , SqlState );
			}
			break;

		case SQL_INVALID_HANDLE:		//�n���h��������
		case SQL_ERROR:					//���̑��G���[
			RaiseAssert( g_ErrorLogger , 0 , L"�f�f���R�[�h���擾�ł��܂���" );
			return false;

		case SQL_NO_DATA:				//�f�[�^�o�^�Ȃ�
			break;
		}
	}
	return true;
}

