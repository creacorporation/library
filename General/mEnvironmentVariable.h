//----------------------------------------------------------------------------
// ���ϐ�
// Copyright (C) 2023- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------
#ifndef MENVIRONMENTVARIABLE_H_INCLUDED
#define MENVIRONMENTVARIABLE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mException.h"

namespace mEnvironmentVariable
{

	//���ϐ����擾���܂�
	// key : �擾������ϐ�
	// retResult : ���ʂ̏����ݐ�
	// def : �G���[���̒l
	// ret : �������^
	bool GetVariable( const WString& key , WString& retResult , const WString& def );

	//���ϐ����擾���܂�
	// key : �擾������ϐ�
	// def : �G���[���̒l
	// ret : ���ϐ��̒l�A�G���[�̏ꍇ��def�̒l
	inline WString GetVariable( const WString& key , const WString& def )
	{
		WString result;
		mEnvironmentVariable::GetVariable( key , result , def );
		return result;
	}

	//���ϐ����擾���܂��B�G���[�̏ꍇ�͗�O���������܂��B
	// key : �擾������ϐ�
	// ret : ���ϐ��̒l
	template< class ExceptionIfFailed >
	inline WString GetVariable( const WString& key )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result;
		if( !mEnvironmentVariable::GetVariable( key , result , L"" ) )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}


	//���ϐ���ݒ肵�܂��B�G���[�̏ꍇ�͗�O���������܂��B
	// key : �ݒ肷����ϐ�
	// val : �ݒ肷��l
	// ret : �^
	template< class ExceptionIfFailed = bool >
	bool SetVariable( const WString& key , const WString& val )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		if( !mEnvironmentVariable::SetVariable<bool>( key , val ) )
		{
			throw ExceptionIfFailed();
		}
		return true;
	}

	//���ϐ���ݒ肵�܂��B
	// key : �ݒ肷����ϐ�
	// val : �ݒ肷��l
	// ret : �������^
	template<>
	bool SetVariable< bool >( const WString& key , const WString& val );


}


#endif
