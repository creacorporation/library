//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCQUERY_H_INCLUDED
#define MODBCQUERY_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"

#include "mOdbc.h"
#include "mOdbcSqlState.h"
#include "mOdbcParams.h"
#include "mOdbcDescription.h"
#include "mOdbcConnectionHandle.h"

#include <vector>
#include <memory>
#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")

class mOdbcQuery final
{
public:

	mOdbcQuery();
	virtual ~mOdbcQuery();

	//�N�G�����J�n����
	//���T�[�o�[�ɂ��A�p�����[�^���T�u�N�G����inner/outer join�Ȃǂ̃p�����[�^�ɂȂ��Ă���Ǝ��s���邱�Ƃ�����܂��B
	//  �΍��mOdbc.h�ɋL�ڂ̃R�����g���Q�l�ɂ��Ă�������
	// query : �N�G���̕�����
	// ret : �������^
	bool Prepare( const WString& query );

	//�N�G�����J�n����B�p�����[�^�̒�`�͎蓮�ōs���B
	//���p�����[�^���T�u�N�G����inner/outer join�Ȃǂ̃p�����[�^�ɂȂ��Ă���ꍇ�A
	//  ���邢��DB�T�[�o�[���Ή����Ȃ��ꍇ�A�p�����[�^�̐ݒ�͎蓮�ōs���K�v������܂��B
	// query : �N�G���̕�����
	// desc : �蓮��`����p�����[�^���i���̃p�����[�^�̓��e�`�F�b�N�͍s���܂���B���������̂Ƃ��Ĉ����܂��B�j
	// ret : �������^
	//���T�u�N�G����innner join���܂ރN�G���̎d��(�ȒP�o�[�W�����B�������Adesc��S�Ď蓮�Őݒ肵�Ă�OK)
	//(1)�{�Ԃ̃N�G���ƃp�����[�^�����������A�T�u�N�G�������܂܂Ȃ��_�~�[�̃N�G�������
	//(2)Prepare()�Ń_�~�[�̃N�G�����Z�b�g(mOdbcParameterDescription���\�z�����)
	//(3)Prepare()��desc��GetParameterDescription()�œ����Q�Ƃ��Z�b�g���āA�{�Ԃ̃N�G�����Z�b�g����
	bool Prepare( const WString& query , const mOdbcParameterDescription& desc );

	//�p�����[�^�N�G�����s���Ƃ��́A�p�����[�^�����擾����
	// ret : �p�����[�^�̏��
	//���p�����[�^���Ȃ��ꍇ�����s�͂��܂���(�Ԃ��ꂽ�Q�Ƃ̃N���X�ɃG���g�����Ȃ���ԂɂȂ�܂�)
	const mOdbcParameterDescription& GetParameterDescription( void )const;

	//�N�G�������s����
	//���p�����[�^�N�G���ŁA�p�����[�^���Ȃ��ꍇ
	// ret : �������^
	bool Execute( void );

	//�N�G�������s����
	//���p�����[�^�N�G���ŁA�p�����[�^���g�p����ꍇ
	// params : �p�����[�^�N�G���̃p�����[�^
	// ret : �������^
	bool Execute( const mOdbcQueryParams& params );

	//�N�G�������s����
	//������SQL�����s����ꍇ
	// query : �N�G���̕�����
	// ret : �������^
	bool Execute( const WString& query );

	//SQL���s���ʂ̗�Ɋւ�������擾����
	// ret : �p�����[�^�̏��
	//�����s���ʂ��Ȃ��ꍇ�����s�͂��܂���(�Ԃ��ꂽ�Q�Ƃ̃N���X�ɃG���g�����Ȃ���ԂɂȂ�܂�)
	const mOdbcResultDescription& GetResultDescription( void )const;

	//�t�F�b�`��������
	enum FetchResult
	{
		FETCH_SUCCEEDED,	//����
		FETCH_NOMOREDATA,	//�����F�����t�F�b�`����f�[�^���Ȃ����߉����ǂݎ���Ă��Ȃ�
		FETCH_TRUNCATED,	//�����F�������A�ꕔ�̃f�[�^�̓o�b�t�@�s���̂��ߐ؂�̂Ă����Ă���
		FETCH_UNKNOWNTYPE,	//�G���[�F�f�[�^�^���s���i�G���[�����ʒu�܂ł����ǂݎ���܂���j
	};

	//���ʂ��擾����
	// retResult : ���ʂ̊i�[��
	// �E���ʃZ�b�g�Ɋ܂܂��S�Ă̗񂪃Z�b�g����܂�
	// �E���ł�retResult�ɓ����Ă���f�[�^�͔j������܂�
	// �E�v��Ȃ���܂Ŏ擾���Ȃ��Ă�����ł������N�G���̂ق��𒼂��Ă�������
	// ret �������^
	FetchResult Fetch( mOdbcResultParam& retResult );

	//������Ԃł̃o�b�t�@�̍ő�l
	//��̍ő咷�������ꍇ(����VARCHAR�̂悤�ȗ�)�A���̃T�C�Y�̃o�b�t�@�����m�ۂ��Ȃ�
	//���t�F�b�`�p�o�b�t�@�T�C�Y�������������ꍇ�́A�t�F�b�`�����f�[�^���r���Ő؂�Ă��܂��܂��B
	//  ���̏ꍇ�A�����f�[�^���Ď擾���邱�Ƃ͂ł��Ȃ��̂ŁA�S�̂��擾����ɂ͍ēx�N�G�������蒼����������܂���B
	const DWORD MAX_FETCH_BUFFER_SIZE = 8192;

	//�w�肳�ꂽ��̃t�F�b�`�p�o�b�t�@�T�C�Y��ύX����
	//Execute���s�������_�ł́A�t�F�b�`�p�o�b�t�@�͍ő�ł�MAX_FETCH_BUFFER_SIZE�����m�ۂ���Ă��Ȃ�
	//������傫���T�C�Y�̗�ƂȂ�ꍇ�́A���炩���߂��̊֐����Ă�ŁA�o�b�t�@�̃T�C�Y���w�肵�܂��B
	//���t�F�b�`�p�o�b�t�@�T�C�Y�������������ꍇ�́A�t�F�b�`�����f�[�^���r���Ő؂�Ă��܂��܂��B
	//  ���̏ꍇ�A�����f�[�^���Ď擾���邱�Ƃ͂ł��Ȃ��̂ŁA�S�̂��擾����ɂ͍ēx�N�G�������蒼����������܂���B
	// colname : �ݒ�Ώۂ̗�̖��O
	// buffsize : �m�ۂ���o�C�g��
	// ret : �������^
	bool ResizeFetchBuffer( const WString colname , size_t buffsize );

	//���Ƀt�F�b�`����f�[�^�����݂��邩���`�F�b�N����
	// ret : �܂��t�F�b�`���Ă��Ȃ��f�[�^������ΐ^
	bool IsDataExist( void )const;

	//�����N�G�����p�����[�^��ύX���čĎ��s�ł���悤��������
	//�E�ǂݎ�蒆�̌��ʂ͔j�����܂�
	//�E���̃R�[�������s��A�ēxExecute()���Ăяo�����Ƃ��ł��܂�
	// Prepare()��Execute()��Recycle()��Execute()��Recycle()��Execute()��...
	bool Recycle( void );

private:

	mOdbcQuery( const mOdbcQuery& source );					//�R�s�[�֎~�N���X
	void operator=( const mOdbcQuery& source ) = delete;	//�R�s�[�֎~�N���X

	friend class mOdbcConnection;

protected:

	//mOdbcConnection�����琶���𔻒肷�邽�߂̃`�P�b�g
	std::shared_ptr< ULONG_PTR > MyDogtag;

	//�X�e�[�g�����g�n���h��
	HSTMT MyStmt;

	//�p�����[�^�N�G���Ɋւ�����
	mOdbcParameterDescription MyParameterDescription;

	//SQLBindParameter��StrLen_or_IndPtr�ɓn���o�b�t�@�̃A���C
	std::vector< SQLLEN > MyParameterLenArray;

	//���ʃZ�b�g�Ɋւ�����
	mOdbcResultDescription MyResultDescription;

	//�t�F�b�`�����f�[�^���ꎞ�i�[����o�b�t�@
	struct FetchBufferEntry
	{
		SQLLEN size;				//ptr�Ɋm�ۂ���Ă���o�C�g��
		SQLLEN value;				//�t�F�b�`�����T�C�Y�E�܂��̓k�����ǂ���������(ODBC�ɓn���p)
		std::unique_ptr<BYTE> ptr;	//�t�F�b�`���̊i�[��(ODBC�ɓn���p)
	};
	typedef std::vector<FetchBufferEntry> FetchBuffer;
	FetchBuffer MyFetchBuffer;

	//�܂��t�F�b�`���Ă��Ȃ��f�[�^������H
	//�t�F�b�`���Ă��Ȃ��f�[�^������ΐ^
	bool MyIsDataExist;

	//SQLRETURN�^�̌��ʃR�[�h������I�����ǂ����𔻒肷��
	//�ǉ���񂪂���ꍇ�́ASQL�X�e�[�g�����g�̃����o(MyOdbcSqlState)���X�V����
	//rc : ���ʃR�[�h
	//ret : ���ʃR�[�h���������������̂ł���ΐ^
	bool SQL_RESULT_CHECK( SQLRETURN rc );

	//�w�肵��mOdbcResultDescription�̃G���g���Ƀt�B�b�g����悤�ɁAMyFetchBuffer���\�z����B
	//�܂��A���ʃZ�b�g�Ƀo�b�t�@���o�C���h����
	// desc : �\�z���ɂ��錋�ʃZ�b�g�̏��
	// ret : �������^
	bool CreateFetchBuffer( const mOdbcResultDescription& desc );

	//desc�Ɏw�肵�������g���āAMyFetchBuffer�Ƀo�b�t�@����ݒ肷��B
	//�������A�m�ۂ���o�C�g����size�̎w��ɏ]���B
	bool SetFetchBuffer( const mOdbcDescriptionEntry& desc , SQLLEN size );

	//�p�����[�^�̌^��C�^�ɕϊ�����
	SQLSMALLINT ParameterType2CType( mOdbc::ParameterType type )const;
};


#endif
