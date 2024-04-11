//----------------------------------------------------------------------------
// アトミックな値
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MATOMICVALUE_H_INCLUDED
#define MATOMICVALUE_H_INCLUDED

/*
●用途

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

	//代入演算子
	T operator=( T val )
	{		
		PVOID tmp = (PVOID)val;
		InterlockedExchangePointer( (volatile PVOID*)&MyValue , tmp );
		return MyValue.val;
	}

	//キャスト演算子
	operator T()const
	{
		return MyValue.val;
	}

	//前置インクリメント
	T operator++()
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedIncrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedIncrement( (volatile LONG*)&MyValue );
#endif
		return (T)result;
	}

	//後置インクリメント
	T operator++(int)
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedIncrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedIncrement( (volatile LONG*)&MyValue );
#endif
		return (T)( result - 1 );
	}

	//前置デクリメント
	T operator--()
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedDecrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedDecrement( (volatile LONG*)&MyValue );
#endif
		return (T)result;
	}

	//後置デクリメント
	T operator--(int)
	{
#ifdef _WIN64
		LONG64 result = (T)InterlockedDecrement64( (volatile LONG64*)&MyValue );
#else
		LONG result = (T)InterlockedDecrement( (volatile LONG*)&MyValue );
#endif
		return (T)( result + 1 );
	}

	//アトミックに値を比較して、一致していれば交換する
	//if_val : もしこの値だったら、
	//set_val : この値に交換する
	//ret : 交換前の値
	T CompareExchange( T if_val , T set_val )
	{
		T result;
		PVOID tmp_if = (PVOID)if_val;
		PVOID tmp_set = (PVOID)set_val;
		result = (T)InterlockedCompareExchangePointer( (volatile PVOID*)&MyValue , set_val , if_val );
		return result;
	}

protected:

	//デフォルトでは、
	//・32ビットプロセッサでは4バイト境界にアライメント
	//・64ビットプロセッサでは8バイト境界にアライメント
	//されているハズ。

	//アトミックに操作したい値。
	//BYTEとかで使ってもよいように、ULONG_PTR型とのunionにしてある。
	//最初からULONG_PTRでメンバ作って、都度キャストした値を返してもいいけど、
	//enumで作ったときに、デバッガで見るとラベル名が分かってちょっと便利。
	union Value
	{
		ULONG_PTR ptr;	//ダミー。必要サイズ確保＆初期化用。
		T val;			//アトミックに操作する値
	};
	
	Value MyValue;

};


#endif

