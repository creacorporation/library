//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iID�����o���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
*/

#ifndef MUNIQUEVALUE_H_INCLUDED
#define MUNIQUEVALUE_H_INCLUDED

#include "mStandard.h"
#include "General/mException.h"
#include <unordered_set>

template< class T > class mUniqueValue
{
public:
	mUniqueValue()
	{
		MyNextValue = (T)( 1 << ( sizeof( T ) * 8 - 1 ) );
	}
	virtual ~mUniqueValue() = default;

	//���j�[�N�Ȓl���擾����
	//ret : ���j�[�N�Ȓl
	//���������[�N���Ă��܂��̂ŁA�擾�������j�[�N�Ȓl���s�v�ƂȂ������_��
	//Return()�ɂ��l�̏��L����ԋp���ĉ������B
	T Get( void )
	{
		//����A�����o����l�����݂��Ȃ��ꍇ�́A�ǂ��ɂ��Ȃ�Ȃ��̂ŗ�O
		if( MyValueSet.size() == ~(T)0 )
		{
			throw EXCEPTION( 0 , L"unique value full" );
		}

		//�����������Ƃ��Ă���l���g�p���łȂ����m�F���A
		//�g���Ă��Ȃ��l��������܂Ń��[�v
		while( MyValueSet.count( MyNextValue ) )
		{
			MyNextValue++;
		}

		//���ʕԋp
		UINT result = MyNextValue;
		MyNextValue++;	//���ɔ�����1�����Ă���

		MyValueSet.insert( result );	//�g�p���ɓo�^
		return result;
	}

	//�s�v�ƂȂ����l��Ԃ�
	//val : �ԋp����l
	void Return( T val )
	{
		if( MyValueSet.count( val ) )
		{
			MyValueSet.erase( val );
		}
	}

	//�S���܂Ƃ߂ď���
	void Clear( void )
	{
		MyValueSet.clear();
	}

private:

	mUniqueValue( const mUniqueValue& src ) = delete;
	mUniqueValue& operator=( const mUniqueValue& src ) = delete;

	//���o�ς݂̒l�̈ꗗ
	typedef std::unordered_set< T > ValueSet;
	ValueSet MyValueSet;

	//���ɕ����o���\��̒l
	T MyNextValue;
};


#endif //MUNIQUEVALUE_H_INCLUDED

