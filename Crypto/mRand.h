//----------------------------------------------------------------------------
// 乱数基底クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

//乱数ライブラリ
//・WindowsAPIを利用して乱数を得ます
//・CryptGenRandom()を使用
//・暗号に使用可能
//・状態のセーブは不可

#ifndef MRAND_H_INCLUDED
#define MRAND_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>

class mRand
{
public:
	mRand();
	virtual ~mRand();

	//UINT形式、32ビットの乱数
	UINT  RandInt32( void );

	//正の数限定、31ビットの乱数
	INT   RandUInt31( void );

	//閉区間[0,1]の実数(0以上1以下)
	DOUBLE RandDouble1( void );

	//半開区間[0,1)の実数(0以上1未満)
	DOUBLE RandDouble2( void );

	//開区間(0,1)の実数(0より大きく1より小さい)
	DOUBLE RandDouble3( void );

	//閉区間[0,1]の実数(0以上1以下)
	DOUBLE RandClose( void );

	//半開区間[0,1)の実数(0以上1未満)
	DOUBLE RandSemiOpen( void );

	//開区間(0,1)の実数(0より大きく1より小さい)
	DOUBLE RandOpen( void );

	//渡したバッファを乱数値で埋める
	//buffer : 乱数データの格納先
	//bufferlen : 格納するバイト数
	void RandFill( BYTE* buffer , DWORD bufferlen );

	//指定した2値間に均等分布する値
	// ret : val1～val2の値に分布する値(val1、val2とも出現します)
	UINT RandBetween( UINT val1 , UINT val2 );

private:
	mRand( const mRand& source );
	const mRand& operator=( const mRand& source ) = delete;

protected:

	//乱数生成用プロバイダ
	HCRYPTPROV MyCryptProv;

	//プロバイダの初期化
	bool InitProvider( void );

	//プロバイダの解放
	bool FreeProvider( void );

};

#endif