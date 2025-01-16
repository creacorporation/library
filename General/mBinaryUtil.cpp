//----------------------------------------------------------------------------
// バイナリユーティリティ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#define MBINARYUTIL_CPP_COMPILING
#include "mBinaryUtil.h"
#include "General/mErrorLogger.h"


int BitCount( const void* p , size_t sz )
{
	static const char bit[ 256 ] =
	{
		0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
		4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
	};

	int result = 0;
	for( size_t i = 0 ; i < sz ; i++ )
	{
		result += bit[ ((const BYTE*)p)[ i ] ];
	}
	return result;

}


#if 0 

/*
popcntはSSE4.2(Core i)プロセッサからだよ。
Core2とかは非対応だからしょうがないね。
下みたいなアルゴリズムでこねくり回すより、テーブル引く方が速いらしいよ。
*/

//#include <intrin.h>

int BitCount( int v )
{
	return (int)BitCount( (unsigned int)v );

	//return __popcnt( v );
}

unsigned int BitCount( unsigned int v )
{
	v = v - ( ( v >> 1) & 0x55555555 );
	v = ( v & 0x33333333 ) + ( ( v >> 2 ) & 0x33333333 );
	v = ( v + ( v >> 4) ) & 0x0f0f0f0f;
	v = v + ( v >> 8 );
	v = v + ( v >> 16 );
	return v;

	//return __popcnt( v );
}

#endif

