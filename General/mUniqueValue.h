//----------------------------------------------------------------------------
// ウインドウ管理（ID払い出し）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
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

	//ユニークな値を取得する
	//ret : ユニークな値
	//メモリリークしてしまうので、取得したユニークな値が不要となった時点で
	//Return()により値の所有権を返却して下さい。
	T Get( void )
	{
		//万一、払い出せる値が存在しない場合は、どうにもならないので例外
		if( MyValueSet.size() == ~(T)0 )
		{
			throw EXCEPTION( 0 , L"unique value full" );
		}

		//払いだそうとしている値が使用中でないか確認し、
		//使っていない値を見つけるまでループ
		while( MyValueSet.count( MyNextValue ) )
		{
			MyNextValue++;
		}

		//結果返却
		UINT result = MyNextValue;
		MyNextValue++;	//次に備えて1足しておく

		MyValueSet.insert( result );	//使用中に登録
		return result;
	}

	//不要となった値を返す
	//val : 返却する値
	void Return( T val )
	{
		if( MyValueSet.count( val ) )
		{
			MyValueSet.erase( val );
		}
	}

	//全部まとめて消す
	void Clear( void )
	{
		MyValueSet.clear();
	}

private:

	mUniqueValue( const mUniqueValue& src ) = delete;
	mUniqueValue& operator=( const mUniqueValue& src ) = delete;

	//払出済みの値の一覧
	typedef std::unordered_set< T > ValueSet;
	ValueSet MyValueSet;

	//次に払い出す予定の値
	T MyNextValue;
};


#endif //MUNIQUEVALUE_H_INCLUDED

