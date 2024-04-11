//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCENVIRONMENT_H_INCLUDED
#define MODBCENVIRONMENT_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "mOdbc.h"
#include "mOdbcSqlState.h"
#include "mOdbcConnection.h"

#include <deque>
#include <sql.h>
#include <sqlext.h>
#include <memory>

#pragma comment(lib, "odbc32.lib")

//ODBC�̊��n���h�����v���Z�X���ŋ��p���邽�߂̃N���X�ł�
//�����n���h���͂P�v���Z�X�P�����ł�
//�E�ŏ��ɎQ�Ƃ��ꂽ�Ƃ��ɁAODBC3.0�̊��n���h���𐶐����A�ȍ~������g���܂킵�܂��B
//�E���̃N���X�̃C���X�^���X���Ȃ��Ȃ����ꍇ�A���n���h����j�����܂�

class mOdbcEnvironment final
{
public:

	mOdbcEnvironment();
	virtual ~mOdbcEnvironment();

	//�f�[�^�x�[�X�ɐڑ�����Ƃ��̈������i�[����\����
	struct ConnectInfo
	{
		WString DataSource;		//�f�[�^�x�[�X�\�[�X�̖��O
		WString User;			//���[�U�[��
		WString Password;		//�p�X���[�h
	};

	//�f�[�^�x�[�X�ɐڑ�����
	//info : �ڑ�����
	//retConn : �ڑ��������ʓ���ꂽ�n���h��
	//ret : �ڑ��������^
	bool NewConnect( const ConnectInfo& info , mOdbcConnection& retConn );

	//���n���h������������Ă��邩��Ԃ�
	operator bool() const;

private:

	mOdbcEnvironment( const mOdbcEnvironment& source );			//�R�s�[�֎~�N���X
	void operator=( const mOdbcEnvironment& source ) = delete;	//�R�s�[�֎~�N���X

private:

	//ODBC���n���h��
	static HENV MyHenv;

	//�Q�ƃJ�E���g
	static DWORD MyRefCount;

	//���n���h���̊m��
	bool CreateHandle( void );

	//���n���h���̉��
	void DisposeHandle( void );

	//SQLRETURN�^�̌��ʃR�[�h������I�����ǂ����𔻒肷��
	//�܂��ASQL�X�e�[�g�����g�̃����o��ݒ肵�܂�
	//rc : ���ʃR�[�h
	//ret : ���ʃR�[�h���������������̂ł���ΐ^
	bool SQL_RESULT_CHECK( SQLRETURN rc );

};

//�O���[�o���I�u�W�F�N�g
#ifndef MODBCENVIRONMENT_CPP_COMPILING
extern mOdbcEnvironment g_OdbcEnvironment;
#else
mOdbcEnvironment g_OdbcEnvironment;
#endif

#endif
