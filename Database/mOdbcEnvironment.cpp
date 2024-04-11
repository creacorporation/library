//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MODBCENVIRONMENT_CPP_COMPILING
#include "mOdbcEnvironment.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mErrorLogger.h"

//�ÓI�ϐ��̎��Ԑ錾
HENV mOdbcEnvironment::MyHenv = 0;
DWORD mOdbcEnvironment::MyRefCount = 0;

mOdbcEnvironment::mOdbcEnvironment()
{
	//�N���e�B�J���Z�N�V����
	mCriticalSectionTicket cs( g_CriticalSection );
	MyRefCount++;
	return;
}

mOdbcEnvironment::~mOdbcEnvironment()
{
	//�N���e�B�J���Z�N�V����
	mCriticalSectionTicket cs( g_CriticalSection );

	//�Q�ƃJ�E���g���X�V���A�O�ɂȂ��Ă���΃n���h�����������
	MyRefCount--;
	if( MyRefCount )
	{
		return;	//�O����Ȃ��̂ŉ������Ȃ�
	}

	//�O�ɂȂ����̂ŁA�n���h�������
	DisposeHandle();
	return;
}

mOdbcEnvironment::operator bool() const
{
	return MyHenv != 0;
}

bool mOdbcEnvironment::NewConnect( const ConnectInfo& info , mOdbcConnection& retConn )
{
	//���n���h���̐���
	if( !CreateHandle() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���n���h���̐����G���[" );
		return false;
	}

	//�R�l�N�V�����n���h���̍쐬
	HDBC hdbc = 0;
	if( !SQL_RESULT_CHECK( SQLAllocHandle( SQL_HANDLE_DBC , MyHenv , &hdbc ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�R�l�N�V�����n���h���𐶐��ł��܂���" );
		return false;
	}
	retConn.MyDBCHandle = std::make_shared<mOdbcConnectionHandleContents>( hdbc );

	//ODBC�h���C�o�ɐڑ�
	if( !retConn.SQL_RESULT_CHECK( SQLConnectW(
		*retConn.MyDBCHandle.get(),
		const_cast<SQLWCHAR*>( info.DataSource.c_str() ),	//�f�[�^�\�[�X��
		(SQLSMALLINT)info.DataSource.size(),
		const_cast<SQLWCHAR*>( info.User.c_str() ),			//���[�U�[��
		(SQLSMALLINT)info.User.size(),
		const_cast<SQLWCHAR*>( info.Password.c_str() ),		//�p�X���[�h
		(SQLSMALLINT)info.Password.size()
	) ) )
	{
		//�G���[���͐ڑ��n���h�����ɋL�^����邽�߁A�I�u�W�F�N�g�̍쐬���̂͐�������
		RaiseError( g_ErrorLogger , 0 , L"�f�[�^�x�[�X�ɐڑ��ł��܂���ł���" , info.DataSource );
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�f�[�^�x�[�X�ɐڑ����܂���" , info.DataSource );
	}
	return true;
}

bool mOdbcEnvironment::CreateHandle( void )
{
	//�N���e�B�J���Z�N�V����
	mCriticalSectionTicket cs( g_CriticalSection );

	//���łɊ��n���h�������݂���ꍇ�͂����Ԃ�
	if( MyHenv )
	{
		return true;
	}

	//���݂��Ȃ��ꍇ�͐V���Ɋ��n���h�����m�ۂ���
	if( !SQL_SUCCEEDED( SQLAllocHandle( SQL_HANDLE_ENV , SQL_NULL_HANDLE , &MyHenv ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���n���h�����m�ۂł��܂���" );
		goto end;
	}

	//ODBC�o�[�W�����̒ʒm
	if( !SQL_SUCCEEDED( SQLSetEnvAttr( MyHenv , SQL_ATTR_ODBC_VERSION , (SQLPOINTER)SQL_OV_ODBC3_80 , SQL_IS_UINTEGER ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ODBC�o�[�W�����̐ݒ�G���[�ł�" );
		goto end;
	}

	CreateLogEntry( g_ErrorLogger , 0  , L"ODBC���n���h����V�K�ɍ쐬���܂���" );
	return true;

end:
	DisposeHandle();
	return 0;
}

void mOdbcEnvironment::DisposeHandle( void )
{
	if( MyHenv )
	{
		if( !SQL_SUCCEEDED( SQLFreeHandle( SQL_HANDLE_ENV , MyHenv ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"ODBC���n���h���̉���Ɏ��s���܂���" );
		}
		else
		{
			CreateLogEntry( g_ErrorLogger , 0  , L"ODBC���n���h�����������܂���" );
		}
		MyHenv = 0;
	}
	return;
}

bool mOdbcEnvironment::SQL_RESULT_CHECK( SQLRETURN rc )
{
	//�����ȊO�̏ꍇ�ɂ́A���炩�̒ǉ���񂪂���Ƃ݂āA�擾�����݂�B
	if( rc != SQL_SUCCESS )
	{
		mOdbcSqlState::AppendLog( MyHenv , mOdbcSqlState::HandleKind::Environment );
	}
	return SQL_SUCCEEDED( rc );
}
