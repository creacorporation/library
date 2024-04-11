//----------------------------------------------------------------------------
// �A�g�~�b�N�Ȓl
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MATOMICVALUE_H_INCLUDED
#define MATOMICVALUE_H_INCLUDED

/*
���p�r

*/

#include "mStandard.h"

template< class T > class mAtomicValue
{
public:

#ifdef _WIN64
	static_assert( sizeof( T ) <= 8 , "sizoef(typename) too big" );
#else
	static_assert( sizeof( T ) <= 4 , "sizoef(typename) too big" );
#endif

	mAtomicValue()
	{
		MyValue.ptr = 0;
	}
	mAtomicValue( T val )
	{
		MyValue.ptr = 0;
		MyValue.val = val;
	}
	virtual ~mAtomicValue()
	{
	}
	mAtomicValue( const mAtomicValue& source )
	{
		MyValue.ptr = source.MyValue.ptr;
	}

	//������Z�q
	T operator=( T val )
	{		
		PVOID tmp = (PVOID)val;
		InterlockedExchangePointer( (volatile PVOID*)&MyValue , tmp );
		return MyValue.val;
	}

	//�L���X�g���Z�q
	operator T()const
	{
		return MyValue.val;
	}

	//�O�u�C���N�������g
	T operator++()
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedIncrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedIncrement( (volatile LONG*)&MyValue );
#endif
		return (T)result;
	}

	//��u�C���N�������g
	T operator++(int)
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedIncrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedIncrement( (volatile LONG*)&MyValue );
#endif
		return (T)( result - 1 );
	}

	//�O�u�f�N�������g
	T operator--()
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedDecrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedDecrement( (volatile LONG*)&MyValue );
#endif
		return (T)result;
	}

	//��u�f�N�������g
	T operator--(int)
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedDecrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedDecrement( (volatile LONG*)&MyValue );
#endif
		return (T)( result + 1 );
	}

	//�A�g�~�b�N�ɒl���r���āA��v���Ă���Ό�������
	//if_val : �������̒l��������A
	//set_val : ���̒l�Ɍ�������
	//ret : �����O�̒l
	T CompareExchange( T if_val , T set_val )
	{
		T result;
		PVOID tmp_if = (PVOID)if_val;
		PVOID tmp_set = (PVOID)set_val;
		result = (T)InterlockedCompareExchangePointer( (volatile PVOID*)&MyValue , set_val , if_val );
		return result;
	}

protected:

	//�f�t�H���g�ł́A
	//�E32�r�b�g�v���Z�b�T�ł�4�o�C�g���E�ɃA���C�����g
	//�E64�r�b�g�v���Z�b�T�ł�8�o�C�g���E�ɃA���C�����g
	//����Ă���n�Y�B

	//�A�g�~�b�N�ɑ��삵�����l�B
	//BYTE�Ƃ��Ŏg���Ă��悢�悤�ɁAULONG_PTR�^�Ƃ�union�ɂ��Ă���B
	//�ŏ�����ULONG_PTR�Ń����o����āA�s�x�L���X�g�����l��Ԃ��Ă��������ǁA
	//enum�ō�����Ƃ��ɁA�f�o�b�K�Ō���ƃ��x�������������Ă�����ƕ֗��B
	union Value
	{
		ULONG_PTR ptr;	//�_�~�[�B�K�v�T�C�Y�m�ہ��������p�B
		T val;			//�A�g�~�b�N�ɑ��삷��l
	};
	
	Value MyValue;

};


#endif

