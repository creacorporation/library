//----------------------------------------------------------------------------
// AES暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09～
//----------------------------------------------------------------------------


#include "mAESEncrypter.h"
#include <math.h>
#include "../General/mErrorLogger.h"

mAESEncrypter::mAESEncrypter()
{

}

mAESEncrypter::~mAESEncrypter()
{

}

bool mAESEncrypter::Encrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retEncryptData , DWORD& retWritten )const
{
	//バッファチェック
	if( !data )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが未指定です" );
		return false; 
	}

	//必要バッファ長の算出
	retWritten = datalen;
	DWORD bufflen = datalen;

	if( !CryptEncrypt( MyCryptKey , 0 , isfinal , 0 , nullptr , &bufflen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化に失敗しました" );
		return false;
	}
	//バッファの確保
	retEncryptData.reset( mNew BYTE[ bufflen ] );
	MoveMemory( retEncryptData.get() , data , datalen );

	//暗号化処理
	if( !CryptEncrypt( MyCryptKey , 0 , isfinal , 0 , retEncryptData.get() , &retWritten , bufflen ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化に失敗しました" );
		return false;
	}
	return true;
}


bool mAESEncrypter::CreateRandomHeader( BYTE* data , DWORD bufflen , DWORD& retwritten )
{
	if( !data )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが未指定です" );
		return false;
	}
	if( bufflen == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファ長が0です" );
		return false;
	}
	if( bufflen < 32 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが短すぎます" );
	}

	//何バイト分ランダムデータを作る？
	DWORD factor = 0;
	CryptGenRandom( MyCryptProv , sizeof( DWORD ) , (BYTE*)&factor );
	double rnd = (double)factor * ( 1.0 / 4294967296.0 );

	//17文字より少なければ全部
	//17文字以上ならば少なくても16文字ランダムデータにする。
	DWORD len;
	if( bufflen < 17 )
	{
		len = bufflen;
	}
	else
	{
		DWORD rndm = bufflen - 16;
		len = (DWORD)( rnd * rndm ) + 16 + 1;
	}

	//ランダムバイトを生成
	CryptGenRandom( MyCryptProv , len , data );

	//生成したランダムバイトの最終バイト以外で0があったら0じゃない値で埋める
	for( DWORD i = 0 ; i < len - 1 ; i++ )
	{
		if( data[ i ] == 0 )
		{
			BYTE dt = 0;
			do
			{
				CryptGenRandom( MyCryptProv , 1 , &dt );
			}while( dt == 0 );
			data[ i ] = dt;
		}
	}

	//最終バイトを０にする
	data[ len - 1 ] = 0;

	//埋めたバイト数をセット
	retwritten = len;

	return true;
}
