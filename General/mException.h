//----------------------------------------------------------------------------
// ��O�N���X
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
��O�������ɓ�����I�u�W�F�N�g�̒�`�ł��B
EXCEPTION�}�N�����g�p���ē�����̂��������߂ł��B

��F
	throw EXCEPTION( 0x80007AFF , L"�G���[�ł�" );
*/

#ifndef MEXCEPTION_H_INCLUDED
#define MEXCEPTION_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#pragma warning( disable : 4290 )

class mExceptionBase
{
public:
	mExceptionBase()noexcept;
	virtual ~mExceptionBase()noexcept;
	mExceptionBase( const mExceptionBase& source )noexcept;
	mExceptionBase& operator=( const mExceptionBase& source )noexcept;

	DWORD GetErrorCode( void )const noexcept;
protected:
	//��O�G���[�������̃G���[�R�[�h(GetLastError�̒l)
	DWORD MyLastError;
};

class mException : public mExceptionBase
{
public:

	//��O�I�u�W�F�N�g�𐶐����܂�
	//path : ��O�𔭐������t�@�C����
	//line : ��O�𔭐������s��
	//code : ��O�������̃G���[�R�[�h
	//explain : ��O�ɑ΂������
	//�������I�ɗ�O�̔��������O�ɋL�^���܂�
	mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , const WString& explain2 = L"" )noexcept;

	//��O�I�u�W�F�N�g�𐶐����܂�
	//path : ��O�𔭐������t�@�C����
	//line : ��O�𔭐������s��
	//code : ��O�������̃G���[�R�[�h
	//explain : ��O�ɑ΂������
	//�������I�ɗ�O�̔��������O�ɋL�^���܂�
	mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , DWORD_PTR val )noexcept;

	//��̗�O�I�u�W�F�N�g�𐶐����܂�
	//�������I�ɗ�O�̔��������O�ɋL�^���܂���
	mException() noexcept;

	virtual ~mException()noexcept;
	mException( const mException& source )noexcept;
	mException& operator=( const mException& source )noexcept;

public:

	//��O�𔭐������t�@�C�������擾���܂�
	//ret : ��O�������̃t�@�C����
	WString GetPath( void )const noexcept;

	//��O�𔭐������R�[�h�s�𓾂܂�
	//ret : ��O�������̃R�[�h�s
	DWORD GetLine( void )const noexcept;

	//��O�𔭐������G���[�R�[�h�𓾂܂�
	//ret : ��O�������̃G���[�R�[�h
	ULONG_PTR GetCode( void )const noexcept;

	//��O�ɑ΂�������𓾂܂�
	//ret : ��O�������̐�����
	WString GetExplain( void )const noexcept;

	//��O�ɑ΂���g�������𓾂܂�
	//ret : ��O�������̐�����
	WString GetExplainOption( void )const noexcept;

protected:

	//��O�������̃t�@�C����
	WString MyPath;
	//��O�������̃R�[�h�s
	DWORD MyLine;
	//��O�������̃G���[�R�[�h
	ULONG_PTR MyCode;
	//��O�������̐�����
	WString MyExplain1;
	WString MyExplain2;

};

//��O�I�u�W�F�N�g�𐶐����܂�
//code : ��O�������̃G���[�R�[�h
//explain : ��O�ɑ΂������
#define EXCEPTION(error_code,...) mException( mCURRENT_FILE , __LINE__ , error_code , __VA_ARGS__ )

//�`�F�b�N�Ɏ��s�����ꍇ�̓e���v���[�g�����Ŏw�肵����O�𓊂���
//��O�N���X��mExceptionBase����h���������̂ł��邱�ƁB
// ���Q�l�� using err = class ErrorClass : mExceptionBase{};
//          Check<err>( SomeFunction() );
template< class ExceptionIfFailed >
inline void Check( bool v )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
	if( !v ) throw ExceptionIfFailed();
}

#endif //MEXCEPTION_H_INCLUDED 
