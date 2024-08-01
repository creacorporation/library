//----------------------------------------------------------------------------
// 乱数基底クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MRAND_CPP_COMPILING
#include "mRand.h"
#include "../General/mErrorLogger.h"

mRand::mRand()
{
	MyCryptProv = 0;
}

mRand::~mRand()
{
	FreeProvider();
}

//プロバイダの初期化
bool mRand::InitProvider( void )
{
	//暗号化プロバイダは初期化ずみ？
	if( MyCryptProv )
	{
		//既に初期化済み
		return true;
	}
	//暗号化プロバイダの初期化
	if( !CryptAcquireContext( &MyCryptProv , nullptr , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダの初期化に失敗しました" );
		return false;
	}
	return true;
}

//プロバイダの解放
bool mRand::FreeProvider( void )
{
	if( MyCryptProv )
	{
		CryptReleaseContext( MyCryptProv , 0 );
		MyCryptProv = 0;
	}
	return true;
}

//UINT形式、32ビットの乱数
UINT  mRand::RandInt32( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );
	return val;
}

//正の数限定、31ビットの乱数
INT   mRand::RandUInt31( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );
	return (INT)( val >> 1 );
}

//閉区間[0,1]の実数(0以上1以下)
DOUBLE mRand::RandDouble1( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );

	return (DOUBLE)val * ( 1.0 / 4294967295.0 ); 
}

//半開区間[0,1)の実数(0以上1未満)
DOUBLE mRand::RandDouble2( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );

	return (DOUBLE)val * ( 1.0 / 4294967296.0 ); 
}

//開区間(0,1)の実数(0より大きく1より小さい)
DOUBLE mRand::RandDouble3( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );

	return ( (DOUBLE)val + 0.5 ) * ( 1.0 / 4294967296.0 ); 
}

//閉区間[0,1]の実数(0以上1以下)
DOUBLE mRand::RandClose( void )
{
	return RandDouble1();
}

//半開区間[0,1)の実数(0以上1未満)
DOUBLE mRand::RandSemiOpen( void )
{
	return RandDouble2();
}

//開区間(0,1)の実数(0より大きく1より小さい)
DOUBLE mRand::RandOpen( void )
{
	return RandDouble3();
}

//渡したバッファを乱数値で埋める
void mRand::RandFill( BYTE* buffer , DWORD bufferlen )
{
	if( !buffer )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが指定されていません" );
		return;
	}
	if( !InitProvider() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダが初期化されていません" );
		SecureZeroMemory( buffer , bufferlen );
		return;
	}
	if( !CryptGenRandom( MyCryptProv , bufferlen , buffer ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ処理結果を取得できません" );
		return;
	}
	return;
}

//指定した2値間に均等分布する値
UINT mRand::RandBetween( UINT val1 , UINT val2 )
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

