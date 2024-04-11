//----------------------------------------------------------------------------
// RSA暗号化クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mRSACipher.h"
#include "../General/mErrorLogger.h"

mRSACipher::mRSACipher()
{

}

mRSACipher::~mRSACipher()
{

}

//暗号化する
bool mRSACipher::Encrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retEncrypted , DWORD& retWritten )const
{
	if( data == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが未指定です" );
		return false;
	}

	if( type == KEYTYPE::KEY_PRIVATE )
	{
		//暗号化後のデータ長を求める
		retWritten = datalen;
		DWORD bufflen = datalen;

		if( !CryptEncrypt( MyCryptKey , 0 , true , 0 , nullptr , &bufflen , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"暗号化後のデータ長を取得できません" );
			return false;
		}
		//暗号化前のデータをコピーする
		retEncrypted.reset( mNew BYTE[ bufflen ] );
		MoveMemory( retEncrypted.get() , data , datalen );

		//暗号化を行う
		if( !CryptEncrypt( MyCryptKey , 0 , true , 0 , retEncrypted.get() , &retWritten , bufflen ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"暗号化に失敗しました" );
			return false;
		}
	}
	else if( type == KEYTYPE::KEY_PUBLIC )
	{
		//公開鍵の設定
		if( !const_cast<mRSACipher*>(this)->ExtractPublicKey() )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"公開鍵の設定が失敗しました" );
			return false;
		}
		//暗号化後のデータ長を求める
		retWritten = datalen;
		DWORD bufflen = datalen;

		if( !CryptEncrypt( MyCryptKeyPub , 0 , true , 0 , nullptr , &bufflen , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"暗号化後のデータ長を取得できません" );
			return false;
		}
		//暗号化前のデータをコピーする
		retEncrypted.reset( mNew BYTE[ bufflen ] );
		MoveMemory( retEncrypted.get() , data , datalen );

		//暗号化を行う
		if( !CryptEncrypt( MyCryptKeyPub , 0 , true , 0 , retEncrypted.get() , &retWritten , bufflen ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"暗号化に失敗しました" );
			return false;
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"不正なカギの種類です" , type );
		return false;
	}
	return true;
}

bool mRSACipher::Decrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retDecrypted , DWORD& retWritten )const
{
	if( data == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが未指定です" );
		return false;
	}

	if( type == KEYTYPE::KEY_PRIVATE )
	{
		//復号前のデータをコピーする
		retDecrypted.reset( mNew BYTE[ datalen ] );
		MoveMemory( retDecrypted.get() , data , datalen );

		//復号を行う
		retWritten = datalen;
		if( !CryptDecrypt( MyCryptKey , 0 , true , 0 , retDecrypted.get() , &retWritten ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"復号に失敗しました" );
			return false;
		}
	}
	else if( type == KEYTYPE::KEY_PUBLIC )
	{
		//公開鍵の設定
		if( !const_cast<mRSACipher*>(this)->ExtractPublicKey() )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"公開鍵の設定が失敗しました" );
			return false;
		}
		//復号前のデータをコピーする
		retDecrypted.reset( mNew BYTE[ datalen ] );
		MoveMemory( retDecrypted.get() , data , datalen );

		//復号を行う
		retWritten = datalen;
		if( !CryptDecrypt( MyCryptKeyPub , 0 , true , 0 , retDecrypted.get() , &retWritten ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"復号に失敗しました" );
			return false;
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"不正なカギの種類です" , type );
		return false;
	}
	return true;
}


