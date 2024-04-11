//----------------------------------------------------------------------------
//メルセンヌ・ツイスタ乱数発生クラス
// Copyright (C) 2005 Fingerling. All rights reserved.
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// ※2018年6月のバージョンより、C++11のrandom依存になりました。
//----------------------------------------------------------------------------

#ifndef MMTWISTER_H_INCLUDED
#define MMTWISTER_H_INCLUDED

#include <random>

//乱数ライブラリ
//・メルセンヌツイスタ疑似乱数を利用して乱数を得ます
//・暗号にはそのままでは使用不可
//・状態のセーブ可

#include "mStandard.h"
#include <memory>

class mMTwister
{
public:

	mMTwister();

	//乱数系列を初期化
	bool Initialize( void );

	//乱数生成器の内部状態の取得
	typedef std::unique_ptr<BYTE> RandState;

	//長い数値列で乱数系列を初期化
	//seed : 内部状態データ
	//len : 内部状態データの長さ
	//ret : 成功時真
	bool Initialize( const RandState& seed , DWORD len );

	//乱数の現在の内部状態を取得する
	//retState : 取得した結果
	//retLen : 内部状態として格納されたバイト数
	//ret : 成功時真
	bool GetState( RandState& retState , DWORD& retLen )const;

	//UINT形式、32ビットの乱数
	UINT RandInt32( void );

	//正の数限定、31ビットの乱数
	INT RandUInt31( void );

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

	mMTwister( const mMTwister& src );
	const mMTwister& operator=( const mMTwister& src ) = delete;

protected:

	typedef std::mt19937 RandSource;
	RandSource MyRandSource;


};


#endif