//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2005/08/19�`
//----------------------------------------------------------------------------

#ifndef MODBCCONNECTION_H_INCLUDED
#define MODBCCONNECTION_H_INCLUDED

/*
���t�@�����X�F
	�E���Ɓ[�b�{�{�Ԃ낮
	Windows���ɂ�C�����C++��ODBC���g���ɂ�
	http://sato-si.at.webry.info/200503/article_11.html
	�EODBC API Reference
	http://msdn.microsoft.com/en-us/library/ms714562.aspx
	�E�΂��΂̂n�c�a�b������
	http://www.amy.hi-ho.ne.jp/jbaba/index.htm
	�EInside ODBC
	  ISBN4-7561-1617-5
	
*/

#include "mstandard.h"
#include "mOdbc.h"
#include "mOdbcQuery.h"
#include "mOdbcSqlState.h"
#include "mOdbcConnectionHandle.h"

#include "../General/mTCHAR.h"
#include <sql.h>
#include <sqlext.h>
#include <deque>
#include <memory>
#pragma comment(lib, "odbc32.lib")

class mOdbcConnection
{

public:
	mOdbcConnection();
	virtual ~mOdbcConnection();

	//�V���ȃN�G���[���쐬����
	//retQuery : �V���ɍ쐬�����N�G���[�̃n���h��
	//ret : �ڑ��������^
	bool NewQuery( mOdbcQuery& retQuery );

	//�����R�~�b�g���[�h���g�p���邩
	// enable : �^�������R�~�b�g���[�h���g�p����(����)
	//          �U���g�����U�N�V�������s��
	//�g�����U�N�V�������s���ꍇ�́ACommit�ARollback�̑��삪�K�v�ɂȂ�܂�
	bool SetAutoCommit( bool enable );

	//�R�~�b�g����
	// ret : �������^
	bool Commit( void );

	//���[���o�b�N����
	// ret : �������^
	bool Rollback( void );

	//�������̃N�G�������݂��邩
	// ret : ���݂���ꍇtrue
	bool IsActive( void ) const noexcept;

private:

	mOdbcConnection( const mOdbcConnection& source );			//�R�s�[�֎~�N���X
	void operator=( const mOdbcConnection& source ) = delete;	//�R�s�[�֎~�N���X

	friend class mOdbcEnvironment;

protected:

	//ODBC�ڑ����̃n���h��
	mOdbcConnectionHandle MyDBCHandle;

	//���̐ڑ��Ɋ֘A�t���Ă���N�G���̃`�P�b�g�^
	using QueryTicket = std::weak_ptr<ULONG_PTR>;

	//���̐ڑ��Ɋ֘A�t���Ă���N�G���̃`�P�b�g�ꗗ
	using Queries = std::deque< QueryTicket >;

	//���̐ڑ��Ɋ֘A�t���Ă���N�G���̃`�P�b�g�ꗗ
	mutable Queries MyQueries;

	//SQLRETURN�^�̌��ʃR�[�h������I�����ǂ����𔻒肷��
	//�ǉ���񂪂���ꍇ�́ASQL�X�e�[�g�����g�̃����o(MyOdbcSqlState)���X�V����
	//rc : ���ʃR�[�h
	//ret : ���ʃR�[�h���������������̂ł���ΐ^
	bool SQL_RESULT_CHECK( SQLRETURN rc );

};

#endif