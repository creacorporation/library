//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2005 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2005/08/19�`
//----------------------------------------------------------------------------

#ifndef MODBC_H_INCLDUED
#define MODBC_H_INCLDUED

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

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "../General/mErrorLogger.h"

#include <sql.h>
#include <sqlext.h>
#include <memory>
#pragma comment(lib, "odbc32.lib")

namespace mOdbc
{
	//�p�����[�^�̎��
	enum ParameterType
	{
		Int64,			//��������64�r�b�g�����^(�����Ȃ��͂���܂���)
		Int32,			//��������32�r�b�g�����^(�����Ȃ��͂���܂���)
		Int16,			//��������16�r�b�g�����^(�����Ȃ��͂���܂���)
		Int8,			//��������8�r�b�g�����^(�����Ȃ��͂���܂���)
		Float,			//�P���x���������_
		Double,			//�{���x���������_
		AString,		//ANSI������
		WString,		//UNICODE������
		Binary,			//�o�C�i��
		Date,			//���t
		Time,			//����
		Timestamp,		//�^�C���X�^���v
	};

}

//�g�p���@
#if 0
int main( int argc , char** argv )
{
	InitializeLibrary();

	//���ڑ�

	//ODBC�ڑ��̂��߂̏����Z�b�g���܂�
	mOdbcEnvironment::ConnectInfo info;
	info.DataSource = L"LOCALDB";	//�f�[�^�\�[�X
	info.User = L"";				//���[�U�[�� ��MS SQL Server�̏ꍇ�󗓂ɂ����Windows�F�؂ɂȂ�
	info.Password = L"";			//�p�X���[�h

	//�f�[�^�x�[�X�ւ̐ڑ����m�����܂�
	mOdbcConnection db;
	g_OdbcEnvironment.NewConnect( info , db );

	//[�I�v�V����]
	//�K�v�Ȃ�A�g�����U�N�V�����̐ݒ�����܂��B
	//db.SetAutoCommit( false );

	//�N�G���̂��߂̃I�u�W�F�N�g���쐬���܂�
	mOdbcQuery query;
	db.NewQuery( query );

	//���N�G��

	//�N�G�����s���܂�
	query.Execute( L"use TESTDB" );
	query.Execute( L"select * from Name" );

	//[�I�v�V����]
	//����������̃��R�[�h������ꍇ�́A�o�b�t�@���Ċm�ۂ���K�v������܂��B
	//�o�b�t�@�ɓ��肫��Ȃ��ꍇ�́A�f�[�^�͐؂�l�߂��Ă��܂��܂��B
	//����̃o�b�t�@���́AmOdbcQuery::MAX_FETCH_BUFFER_SIZE�ɒ�`����܂��B
	//query.ResizeFetchBuffer( L"UserName" , 200000 );

	//[�I�v�V����]
	//�N�G���������ʂɊւ�������擾���܂��B
	//�����̃I�u�W�F�N�g�̏ڍׂ́A�f�o�b�K�Ŕ`���Ă݂Ă��������B
	const mOdbcResultDescription& resultdesc = query.GetResultDescription();
	const mOdbcDescriptionEntry& descentry1 = resultdesc.at( L"Name" );
	const mOdbcDescriptionEntry& descentry2 = resultdesc.at( L"Id" );
	const mOdbcDescriptionEntry& descentry3 = resultdesc.at( L"Score" );

	//�s���t�F�b�`���܂��B
	mOdbcResultParam result;
	query.Fetch( result );
	printf( "%d(%ws) : %d\n" , result[ L"Id" ].Get<int32_t>() , result[ L"Name" ].Get<WString>().c_str() , result[ L"Score" ].Get<int32_t>() );
	query.Fetch( result );
	printf( "%d(%ws) : %d\n" , result[ L"Id" ].Get<int32_t>() , result[ L"Name" ].Get<WString>().c_str() , result[ L"Score" ].Get<int32_t>() );


	//���s�̒ǉ�
	query.Prepare( L"insert into Name ( Id , Name , Score ) values ( ? , ? , ? )" );

	//[�I�v�V����]
	//�p�����[�^�̏����擾
	const mOdbcParameterDescription& desc = query.GetParameterDescription();

	//���T�u�N�G����join���g�������ꍇ�A���^�f�[�^�̎��o�������܂������Ȃ����Ƃ�����
	//�@���̂Ƃ��́A��������_�~�[�̃N�G���Ń��^�f�[�^�����o���Ă���A�{�Ԃ̃N�G�����s���B�ȉ��T���v���B
	//  (1)�p�����[�^�����擾���邽�߂̃_�~�[�N�G���B�{�Ԃ̃N�G����?�̏o�����͓����ɂ��邱�ƁB
	//  query.Prepare(
	//  	L"select * from TableA ,TableB"
	//  	L" where TableA.Date = ? and TableB.Class = ?" );
	//  
	//  //(2)�{���s�������N�G��
	//  query.Prepare(
	//  	L"select * from TableB"
	//  	L" left outer join TableA on TableB.Id = TableA.Id and TableA.Date = ?"
	//  	L" where TableB.Class = ?"
	//  	, query.GetParameterDescription() ); //����������ƑO�̃_�~�[�N�G���̃��^�f�[�^�����̂܂܎g�p����

	//�ǉ�����p�����[�^�̐ݒ�
	mOdbcQueryParams params;
	params.push_back( (int32_t) 300 );	//1�Ԗ�(Id)
	params.push_back( L"Added User" );	//2�Ԗ�(Name)
	params.push_back( (int32_t) 301 );	//3�Ԗ�(Score)

	//�ǉ����s
	query.Execute( params );
	return 0;
}
#endif


#endif
