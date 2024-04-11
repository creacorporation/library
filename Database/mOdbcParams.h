//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCPARAMS_H_INCLUDED
#define MODBCPARAMS_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "../General/mBinary.h"
#include "../General/mDateTime.h"
#include "../General/mException.h"

#include "mOdbc.h"

#include <vector>
#include <unordered_map>
#include <typeinfo>

#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")

class mOdbcParamsEntry
{
public:

	mOdbcParamsEntry();
	virtual ~mOdbcParamsEntry();
	mOdbcParamsEntry( const mOdbcParamsEntry& src );
	const mOdbcParamsEntry& operator=( const mOdbcParamsEntry& src );

	//���̃p�����[�^�[���k���ł���ΐ^
	bool IsNull( void ) const;

	//���̃p�����[�^�[���k���ɂ���
	void SetNull( void );

	//���̃p�����[�^�[�̎�ނ𓾂�
	mOdbc::ParameterType GetParameterType( void )const;

	//[Setter]64�r�b�g�l���Z�b�g���܂�
	void Set( int64_t dt );
	//[Setter]32�r�b�g�l���Z�b�g���܂�
	void Set( int32_t dt );
	//[Setter]16�r�b�g�l���Z�b�g���܂�
	void Set( int16_t dt );
	//[Setter]8�r�b�g�l���Z�b�g���܂�
	void Set( int8_t dt );
	//[Setter]�P���x���������_�l���Z�b�g���܂�
	void Set( float dt );
	//[Setter]�{���x���������_�l���Z�b�g���܂�
	void Set( double dt );
	//[Setter]ANSI/SJIS��������Z�b�g���܂�
	void Set( const AString& dt );
	//[Setter]Unicode��������Z�b�g���܂�
	void Set( const WString& dt );
	//[Setter]�o�C�i������Z�b�g���܂�
	void Set( const mBinary& dt );
	//[Setter]���t���Z�b�g���܂�
	void Set( const mDateTime::Date& dt );
	//[Setter]�������Z�b�g���܂�
	void Set( const mDateTime::Time& dt );
	//[Setter]�^�C���X�^���v���Z�b�g���܂�
	void Set( const mDateTime::Timestamp& dt );


	//�l���Ƃ�R���X�g���N�^
	template< class T >
	mOdbcParamsEntry( T value ) : mOdbcParamsEntry()
	{
		Set( value );
	}

	//[Getter]64�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , int64_t& retDt )const;
	//[Getter]32�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , int32_t& retDt )const;
	//[Getter]16�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , int16_t& retDt )const;
	//[Getter]8�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , int8_t& retDt )const;
	//[Getter]�P���x���������_�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , float& retDt )const;
	//[Getter]�{���x���������_�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , double& retDt )const;
	//[Getter]ANSI/SJIS��������擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , AString& retDt )const;
	//[Getter]Unicode��������擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , WString& retDt )const;
	//[Getter]�o�C�i������擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , mBinary& dt )const;
	//[Getter]���t���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , mDateTime::Date& dt )const;
	//[Getter]�������擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , mDateTime::Time& dt )const;
	//[Getter]�^�C���X�^���v���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , mDateTime::Timestamp& dt )const;

	//[Getter]64�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , uint64_t& retDt )const;
	//[Getter]32�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , uint32_t& retDt )const;
	//[Getter]16�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , uint16_t& retDt )const;
	//[Getter]8�r�b�g�l���擾���܂�
	//retDt : ���ʂ̊i�[��
	//retresult : null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	void Get( bool& retresult , uint8_t& retDt )const;

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//retValid : �L���Ȓl���擾�����ꍇ�͐^
	//			 null�̏ꍇ�A�ێ����Ă���l�̌^���قȂ邽�ߕϊ��o���Ȃ��ꍇ�͋U���Ԃ�܂�
	//ret : �ێ����Ă���l�B�G���[�̏ꍇ�̒l�͕s��B
	template< class T > T Get( bool& retValid )const
	{
		T Dummy;
		Get( retValid , Dummy );
		return Dummy;
	}

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//retValue : �ێ����Ă���l
	//ret : �L���Ȓl���擾�����ꍇ�͐^�B�G���[�̏ꍇretValue�̒l�͕s��B
	template< class T > bool Query( T& retValue )const
	{
		bool result = false;
		Get( result , retValue );
		return result;
	}

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//ret : �ێ����Ă���l
	//���s����ƁA�e���v���[�g�̑��p�����[�^�Ŏw�肵���^�̗�O�𓊂���B��O�N���X��mExceptionBase����h���������̂ł��邱�ƁB
	// �Q�l using err = class ErrorClass : mExceptionBase{};
	template< class T , class ExceptionIfFailed > T Get( void )const
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );

		bool result;
		T buffer = Get< T >( result );

		if( result )
		{
			return buffer;
		}

		throw ExceptionIfFailed();
	}

	//�ێ����Ă���l���e���v���[�g�Ŏw�肵���^�ŕԂ��܂�
	//defvalue : �G���[�܂���null�̏ꍇ�ɕԂ��l
	//ret : �ێ����Ă���l�i�G���[�̏ꍇ��defvalue�̒l�j
	template< class T > T Get( const T& defvalue )const
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
	//retValue : �ێ����Ă���l
	//ret : �L���Ȓl���擾�����ꍇ�͐^�B�G���[�̏ꍇretValue�̒l�͕s��B
	template< class T > bool Query( T& retValue , const T& defvalue )const
	{
		bool result;
		Get( result , retValue );
		if( !result )
		{
			retValue = defvalue;
		}
		return result;
	}

private:
	friend class mOdbcQuery;

	//[Setter]���t���Z�b�g���܂�
	void Set( const SQL_DATE_STRUCT& dt );
	//[Setter]�������Z�b�g���܂�
	void Set( const SQL_TIME_STRUCT& dt );
	//[Setter]�^�C���X�^���v���Z�b�g���܂�
	void Set( const SQL_TIMESTAMP_STRUCT& dt );

protected:

	//�p�����[�^�[�̎�ނ����߂�(RTTI����)
	mOdbc::ParameterType MyParameterType;

	//���̃p�����[�^�[���k���ł���ΐ^
	bool MyIsNull;

	//���̃p�����[�^�̎��f�[�^
	union DataEntry
	{
		int64_t deInt64;
		int32_t deInt32;
		int16_t deInt16;
		int8_t deInt8;
		float deFloat;
		double deDouble;
		::AString deAString;
		::WString deWString;
		mBinary deBinary;
		SQL_DATE_STRUCT deDate;
		SQL_TIME_STRUCT deTime;
		SQL_TIMESTAMP_STRUCT deTimestamp;

		DataEntry()
		{
		}
		~DataEntry()
		{
		}
	};
	DataEntry MyDataEntry;

};

//�N�G���̃p�����[�^�ݒ�
typedef std::vector< mOdbcParamsEntry > mOdbcQueryParams;

//���ʃZ�b�g�̎擾�p
// ���@�F�@���ʂ̗�
// �E�@�F�@�擾����
typedef std::unordered_map< WString , mOdbcParamsEntry > mOdbcResultParam;

//�N�G�����ʂ���w��̗���擾����B
// param : �N�G������
// key : �ǂݎ���̖��O
// def : �f�t�H���g�̒l
// ret : ���������ꍇ�A���̗�̒l�B
//       �w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�̓f�t�H���g�̒l
template< class T >
T ReadOdbcResult( const mOdbcResultParam& param , const WString key , const T& def )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		return def;
	}
	return itr->second.Get<T>( def );
}

//�N�G�����ʂ���w��̗���擾����B
// param : �N�G������
// key : �ǂݎ���̖��O
// retValue : ���������ꍇ�A���̗�̒l�B
//			  �w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�̓f�t�H���g�̒l
// def : �f�t�H���g�̒l
// ret : ���������ꍇ�͐^�B�w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�͋U�B
template< class T >
bool QueryOdbcResult( const mOdbcResultParam& param , const WString key , T& retValue , const T& def )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		retValue = def;
		return false;
	}
	return itr->second.Query<T>( retValue , def );
}

//�N�G�����ʂ���w��̗���擾����B
//���s����ƁA�e���v���[�g�̑��p�����[�^�Ŏw�肵���^�̗�O�𓊂���B��O�N���X��mExceptionBase����h���������̂ł��邱�ƁB
// ���Q�l�� using err = class ErrorClass : mExceptionBase{};
// param : �N�G������
// key : �ǂݎ���̖��O
// ret : �ǂݎ������̒l
template< class T , class ExceptionIfFailed >
T ReadOdbcResult( const mOdbcResultParam& param , const WString key )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );

	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		throw ExceptionIfFailed();
	}
	return itr->second.Get<T,ExceptionIfFailed>();
}

//�N�G�����ʂ���w��̗���擾����B
// param : �N�G������
// key : �ǂݎ���̖��O
// retValid : ���������ꍇ�͐^�B�w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�͋U�B
// ret : ���������ꍇ�A���̗�̒l�B
//       �w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�͕s��B
template< class T >
T ReadOdbcResult( const mOdbcResultParam& param , const WString key , bool& retValid )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		return T();
	}
	return itr->second.Get<T>( retValid );
}

//�N�G�����ʂ���w��̗���擾����B
// param : �N�G������
// key : �ǂݎ���̖��O
// retValue : ���������ꍇ�A���̗�̒l�B
//			  �w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�͕s��B
// ret : ���������ꍇ�͐^�B�w��̗񂪑��݂��Ȃ��ꍇ�A�܂��͓ǂݎ��Ȃ��ꍇ�͋U�B
template< class T >
bool QueryOdbcResult( const mOdbcResultParam& param , const WString key , T& retValue )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		return false;
	}
	return itr->second.Query<T>( retValue );
}

#endif
