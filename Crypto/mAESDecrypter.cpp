//----------------------------------------------------------------------------
// AES暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09～
//----------------------------------------------------------------------------

#include "mAESDecrypter.h"
#include "../General/mErrorLogger.h"

mAESDecrypter::mAESDecrypter()
{

}

mAESDecrypter::~mAESDecrypter()
{

}

bool mAESDecrypter::Decrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retData , DWORD& retWritten )const
{
	//入力チェック
	if( !data )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが未指定です" );
		return false;
	}
	if( !MyCryptKey )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"鍵が設定されていません" );
		return false;
	}

	//バッファの確保
	retData.reset( mNew BYTE[ datalen ] );
	MoveMemory( retData.get() , data , datalen );

	//暗号化解除
	retWritten = datalen;
	if( !CryptDecrypt( MyCryptKey , 0 , isfinal , 0 , retData.get() , &retWritten ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"解読に失敗しました" );
		return false;
	}
	return true;
}

