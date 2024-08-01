//----------------------------------------------------------------------------
// ハッシュ処理クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

//参考URL
// https://www.ipa.go.jp/security/rfc/RFC2104JA.html

#include "mHmac.h"

//warning C4200: 非標準の拡張機能が使用されています: 構造体または共用体中にサイズが 0 の配列があります。
#pragma warning(disable : 4200)  

mHmac::mHmac()
{
	MyCryptKey = 0;
	ZeroMemory( &MyHmacInfo, sizeof( MyHmacInfo ) );
}

mHmac::~mHmac()
{
	ReleaseHashObject();
}

void mHmac::ReleaseHashObject( void )
{
	if( MyCryptKey )
	{
		//すでにキーがある場合は破棄する
		CryptDestroyKey( MyCryptKey );
		MyCryptKey = 0;
	}
}

bool mHmac::Init( mHash::HashAlgorithm alg , const BYTE* key , DWORD keylen , bool openssl )
{
	//HMACキーインポート用の構造体
	struct HashKey{
		BLOBHEADER Header;
		DWORD Length;
		BYTE Key[];
	};
	HashKey* hmac_key = nullptr;
	DWORD hmac_key_size = 0;

	//HMACオブジェクト初期化
	if( !MyHashObject.Init( alg ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"HMACキーの初期化に失敗しました" );
		goto errorend;
	}

	//HMACキー設定
	if( openssl )
	{
		//OpenSSLと互換のモード
		//※RFC2202を素直に実装したもの
		if( keylen < MyHashObject.GetBlockSize() )
		{
			//キーの長さがブロック長より長い場合はハッシュを取る
			if( !MyHashObject.Hash( key , keylen ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"HMACキーの生成が失敗しました" );
				return false;
			}
			hmac_key_size = sizeof( HashKey ) + MyHashObject.GetResultLen();
			hmac_key = (HashKey*)malloc( hmac_key_size );
			hmac_key->Header.bType = PLAINTEXTKEYBLOB;
			hmac_key->Header.bVersion = CUR_BLOB_VERSION;
			hmac_key->Header.reserved = 0;
			hmac_key->Header.aiKeyAlg = CALG_RC2;
			hmac_key->Length = MyHashObject.GetResultLen();
			MyHashObject.GetResult( hmac_key->Key , hmac_key->Length );
		}
		else
		{
			//ブロック長より短い場合はそのまま使う
			hmac_key_size = sizeof( HashKey ) + keylen;
			hmac_key = (HashKey*)malloc( hmac_key_size );
			hmac_key->Header.bType = PLAINTEXTKEYBLOB;
			hmac_key->Header.bVersion = CUR_BLOB_VERSION;
			hmac_key->Header.reserved = 0;
			hmac_key->Header.aiKeyAlg = CALG_RC2;
			hmac_key->Length = keylen;
			MoveMemory( hmac_key->Key , key , keylen );
		}

		//キーオブジェクトの生成
		ReleaseHashObject();
		if( !CryptImportKey( MyHashObject.MyCryptProvider , (BYTE*)hmac_key , hmac_key_size , 0 , CRYPT_IPSEC_HMAC_KEY , &MyCryptKey ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"HMACキーオブジェクトを生成できませんでした" );
			goto errorend;
		}
		SecureZeroMemory( &hmac_key , hmac_key_size );
	}
	else
	{
		//キーをスクランブルするモード
		//※マイクロソフトのHMAC生成サンプルと同じ結果を返すもの
		//　https://msdn.microsoft.com/ja-jp/library/windows/desktop/aa382379(v=vs.85).aspx
		if( !CryptDeriveKey( MyHashObject.MyCryptProvider, CALG_RC4, MyHashObject.MyCryptHash, 0, &MyCryptKey ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"HMACキー用のハッシュを取り出せません" );
			goto errorend;
		}
	}

	//オブジェクト初期化
	if( !Reset() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトの初期化に失敗しました" );
		goto errorend;
	}

	free( hmac_key );
	return true;

errorend:
	free( hmac_key );
	hmac_key = nullptr;

	ReleaseHashObject();
	MyHashObject.ReleaseHashObject();
	return false;

}

bool mHmac::Reset( void )
{
	//オブジェクト初期化
	MyHashObject.ReleaseHashObject();
	if( !CryptCreateHash( MyHashObject.MyCryptProvider , CALG_HMAC , MyCryptKey , 0 , &MyHashObject.MyCryptHash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトの初期化に失敗しました" );
		return false;
	}

	//ハッシュアルゴリズム取得
	ALG_ID alg_id;
	DWORD datalen = sizeof( alg_id );
	if( !CryptGetHashParam( MyHashObject.MyCryptHash , HP_ALGID , (BYTE*)&alg_id , &datalen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュアルゴリズムを取得できません" );
		return false;
	}

	//HMACのための情報構造体を設定
	MyHmacInfo.HashAlgid = alg_id;
	MyHmacInfo.cbInnerString = 0;	//デフォルトを使用(0x36 * 64)
	MyHmacInfo.cbOuterString = 0;	//デフォルトを使用(0x5C * 64)
	MyHmacInfo.pbInnerString = 0;
	MyHmacInfo.pbOuterString = 0;
	if( !CryptSetHashParam( MyHashObject.MyCryptHash, HP_HMAC_INFO, (BYTE*)&MyHmacInfo, 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"HMAC情報構造体を設定できません" );
		return false;
	}

	return true;
}

bool mHmac::Hash( const BYTE* data , DWORD len )
{
	return MyHashObject.Hash( data , len );
}

DWORD mHmac::GetResultLen( void )const
{
	return MyHashObject.GetResultLen();
}

bool mHmac::GetResult( HmacData& retResult , DWORD& retLen )const
{
	return MyHashObject.GetResult( retResult , retLen );
}

bool mHmac::GetResult( BYTE* retresult , DWORD len )const
{
	return MyHashObject.GetResult( retresult , len );
}

bool mHmac::GetResult( AString& retResult )const
{
	return MyHashObject.GetResult( retResult );
}
