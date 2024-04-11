//----------------------------------------------------------------------------
// Variant�^�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MVARIANT_H_INCLUDED
#define MVARIANT_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

#include <objbase.h>
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")


class mVariant
{
public:
	mVariant();
	virtual ~mVariant();

	mVariant( const _variant_t& src );
	mVariant( const mVariant& src );
	const mVariant& operator=( const _variant_t& src );
	const mVariant& operator=( const mVariant& src );

	bool IsEmpty( void )const;
	void SetEmpty( void );

	const mVariant& operator=( char val );
	const mVariant& operator=( short val );
	const mVariant& operator=( int val );
	const mVariant& operator=( long val );
	const mVariant& operator=( __int64 val );
	const mVariant& operator=( unsigned char val );
	const mVariant& operator=( unsigned short val );
	const mVariant& operator=( unsigned int val );
	const mVariant& operator=( unsigned long val );
	const mVariant& operator=( unsigned __int64 val );
	const mVariant& operator=( float val );
	const mVariant& operator=( double val );
	const mVariant& operator=( const WString& val );
	const mVariant& operator=( const AString& val );

	void Set( char val );
	void Set( short val );
	void Set( int val );
	void Set( long val );
	void Set( __int64 val );
	void Set( unsigned char val );
	void Set( unsigned short val );
	void Set( unsigned int val );
	void Set( unsigned long val );
	void Set( unsigned __int64 val );
	void Set( float val );
	void Set( double val );
	void Set( const WString& val );
	void Set( const AString& val );

	void Get( bool& retresult , char& retval )const;
	void Get( bool& retresult , short& retval )const;
	void Get( bool& retresult , int& retval )const;
	void Get( bool& retresult , long& retval )const;
	void Get( bool& retresult , __int64& retval )const;
	void Get( bool& retresult , unsigned char& retval )const;
	void Get( bool& retresult , unsigned short& retval )const;
	void Get( bool& retresult , unsigned int& retval )const;
	void Get( bool& retresult , unsigned long& retval )const;
	void Get( bool& retresult , unsigned __int64& retval )const;
	void Get( bool& retresult , float& retval )const;
	void Get( bool& retresult , double& retval )const;
	void Get( bool& retresult , WString& retval )const;
	void Get( bool& retresult , AString& retval )const;

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//retValid : �L���Ȓl���擾�����ꍇ�͐^
	//			 �ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	//ret : �ێ����Ă���l
	template< class T > T Get( bool& retValid )const
	{
		T Dummy;
		Get( retValid , Dummy );
		return Dummy;
	}

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//ret : �ێ����Ă���l
	//���ӁF�G���[�̏ꍇ�͕s��l�ƂȂ�܂�
	template< class T > T Get( void )const
	{
		bool Dummy;
		return Get< T >( Dummy );
	}

	WString Get( const wchar_t* defvalue )const;
	AString Get( const char* defvalue )const;

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//defvalue : �G���[�̏ꍇ�ɕԂ��l
	//ret : �ێ����Ă���l�i�G���[�̏ꍇ��defvalue�̒l�j
	template< class T > T Get( T defvalue )const
	{
		T value;
		bool result;
		Get( result , value );

		if( result )
		{
			return value;
		}
		return defvalue;
	}

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//ret : �ێ����Ă���l
	//���s����ƁA�e���v���[�g�̑��p�����[�^�Ŏw�肵���^�̗�O�𓊂���B��O�N���X��mExceptionBase����h���������̂ł��邱�ƁB
	// �Q�l using ExceptionIfFailed = class ErrorClass : mExceptionBase{};
	template< class T , class ExceptionIfFailed > T Get( void )const
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		bool result;
		T buffer;
		Get( result , buffer );

		if( result )
		{
			return buffer;
		}
		throw ExceptionIfFailed();
	}

private:
	_variant_t MyOriginal;
	mutable _variant_t MyConverted;

};



#endif