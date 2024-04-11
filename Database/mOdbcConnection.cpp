//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2005/08/19�`
//----------------------------------------------------------------------------

#define MODBCCONNECTION_CPP_COMPILING
#include "mOdbcConnection.h"
#include "mOdbcEnvironment.h"
#include "../General/mErrorLogger.h"

mOdbcConnection::mOdbcConnection()
{
	return;
}

mOdbcConnection::~mOdbcConnection()
{
	return;
}

bool mOdbcConnection::NewQuery( mOdbcQuery& retQuery )
{
	HSTMT stmt = 0;
	if( !SQL_RESULT_CHECK( SQLAllocHandle( SQL_HANDLE_STMT , *MyDBCHandle.get() , &stmt ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�X�e�[�g�����g�n���h���𐶐��ł��܂���" );
		return false;
	}

	//�쐬�����X�e�[�g�����g�n���h����ݒ�
	retQuery.MyStmt = stmt;

	//���ƂŃN�G���̊����𔻒f���邽�߂̔F������ǉ�
	retQuery.MyDogtag.reset( mNew ULONG_PTR( (ULONG_PTR)&retQuery ) );
	MyQueries.push_back( retQuery.MyDogtag );

	return true;
}

bool mOdbcConnection::IsActive( void ) const noexcept
{
	Queries::iterator itr = MyQueries.begin();
	while( itr != MyQueries.end() )
	{
		if( itr->expired() )
		{
			itr = MyQueries.erase( itr );
			continue;
		}
		itr++;
	}

	return !MyQueries.empty();
}


bool mOdbcConnection::SQL_RESULT_CHECK( SQLRETURN rc )
{
	//�����ȊO�̏ꍇ�ɂ́A���炩�̒ǉ���񂪂���Ƃ݂āA�擾�����݂�B
	if( rc != SQL_SUCCESS )
	{
		mOdbcSqlState::AppendLog( *MyDBCHandle.get() , mOdbcSqlState::HandleKind::Connection );
	}
	return SQL_SUCCEEDED( rc );
}

bool mOdbcConnection::SetAutoCommit( bool enable )
{
	ULONG mode = ( enable ) ? ( SQL_AUTOCOMMIT_ON ) : ( SQL_AUTOCOMMIT_OFF );
	if( !SQL_RESULT_CHECK( SQLSetConnectAttr( *MyDBCHandle , SQL_ATTR_AUTOCOMMIT, 0 , SQL_AUTOCOMMIT_OFF ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�I�[�g�R�~�b�g�̐ݒ肪���s���܂���" );
		return false;
	}

	CreateLogEntry( g_ErrorLogger , 0 , L"�I�[�g�R�~�b�g�̐ݒ肪�ύX����܂���" , enable );
	return true;
}

bool mOdbcConnection::Commit( void )
{
	if( !SQL_RESULT_CHECK( SQLEndTran( SQL_HANDLE_DBC , *MyDBCHandle , SQL_COMMIT ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�R�~�b�g�����s���܂���" );
		return false;
	}
	return true;
}

bool mOdbcConnection::Rollback( void )
{
	if( !SQL_RESULT_CHECK( SQLEndTran( SQL_HANDLE_DBC , *MyDBCHandle , SQL_ROLLBACK ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���[���o�b�N�����s���܂���" );
		return false;
	}
	return true;
}

