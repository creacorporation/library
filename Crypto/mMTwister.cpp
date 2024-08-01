//----------------------------------------------------------------------------
//メルセンヌ・ツイスタ乱数発生クラス
// Copyright (C) 2005 Fingerling. All rights reserved.
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#include "mMTwister.h"
#include "mRand.h"
#include "General/mErrorLogger.h"
#include <array>

mMTwister::mMTwister()
{
	return;
}

bool mMTwister::Initialize( void )
{
	std::array< std::seed_seq::result_type , RandSource::state_size	> seed_data;	//シードの値

	//シードの初期化
	mRand rnd;
	rnd.RandFill( reinterpret_cast<BYTE*>( seed_data.data() ) , DWORD( sizeof( seed_data[ 0 ] ) * seed_data.size() ) );

	//作ったシードを適用
	std::seed_seq seq( seed_data.begin() , seed_data.end() );
	MyRandSource.seed( seq );

	//適当に読み飛ばしを行う
	MyRandSource.discard( ( ( timeGetTime() >> 3 ) & 0x0000FFFFUL ) + 10000 );

	return true;
}

UINT mMTwister::RandInt32( void )
{

	static_assert( std::is_same< RandSource::result_type , UINT >::value , "RNG result type mismatch" );
	return MyRandSource();

}

INT mMTwister::RandUInt31( void )
{

	ULONG y = RandInt32();
	return (LONG)( y >> 1);

}

// # a≦x≦bを満たす実数xの集合を閉区間といい、[a,b]で表す
// # 0以上1以下。
DOUBLE mMTwister::RandDouble1( void )
{

	ULONG y = RandInt32();

    return (DOUBLE)y * ( 1.0 / 4294967295.0 ); 
}

DOUBLE mMTwister::RandClose( void )
{

	return RandDouble1();

}

// # 0以上1未満。
DOUBLE mMTwister::RandDouble2( void )
{

	ULONG y = RandInt32();

    return (DOUBLE)y * ( 1.0 / 4294967296.0 ); 
}

DOUBLE mMTwister::RandSemiOpen( void )
{

	return RandDouble2();

}

// # 0より大きく1より小さい。
DOUBLE mMTwister::RandDouble3( void )
{

	ULONG y = RandInt32();

    return ( (DOUBLE)y + 0.5 ) * ( 1.0 / 4294967296.0 ); 
}

DOUBLE mMTwister::RandOpen( void )
{

	return RandDouble3();

}

//渡したバッファを乱数値で埋める
void mMTwister::RandFill( BYTE* buffer , DWORD bufferlen )
{
	while( ( 0 < bufferlen ) && ( (ULONG_PTR)buffer & 0x03 ) )
	{
		*buffer = ( RandInt32() ) & 0xFFu;
		buffer++;
		bufferlen--;
	}
	while( 3 < bufferlen )
	{
		*(UINT*)buffer = RandInt32();
		buffer += 4;
		bufferlen -= 4;
	}
	while( 0 < bufferlen )
	{
		*buffer = ( RandInt32() ) & 0xFFu;
		buffer++;
		bufferlen--;
	}
	return;
}

//指定した2値間に均等分布する値
UINT mMTwister::RandBetween( UINT val1 , UINT val2 )
{
	if( val1 == val2 )
	{
		return val1;
	}
	else if( val1 == 0 && val2 == UINT_MAX )
	{
		return RandInt32();
	}

	UINT diff = ( val1 < val2 ) ? ( val2 - val1 + 1 ) : ( val1 - val2 + 1 );
	return UINT( RandSemiOpen() * diff );
}

