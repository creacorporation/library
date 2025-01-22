//----------------------------------------------------------------------------
// AES暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09〜
//----------------------------------------------------------------------------

#define MAES_CPP_COMPILE
#include "mAES.h"
#include "../General/mErrorLogger.h"

//参考になるかもしれないURL
// http://www.trustss.co.jp/smnEncrypt010.html
// http://www.trustss.co.jp/cng/1000.html
// http://blogs.msdn.com/b/japan_platform_sdkwindows_sdk_support_team_blog/archive/2012/03/01/api-cryptography-api.aspx

static const BYTE SHA_HashSalt[] = {
	0x82, 0xBD, 0x16, 0x28, 0xCE, 0x36, 0x36, 0x3F, 0xFF, 0x48, 0xE9, 0xDE, 0xE2, 0x8F, 0x5C, 0x0A,
	0x21, 0x3A, 0x1D, 0xB9, 0x55, 0x70, 0x86, 0x29, 0x87, 0x7C, 0x47, 0xE0, 0x60, 0x41, 0x4C, 0xB2,
	0x09, 0xFB, 0x9C, 0xC1, 0xFD, 0x55, 0x91, 0x50, 0xEA, 0x56, 0x3B, 0x37, 0x46, 0x17, 0x5A, 0xA7,
	0x6E, 0xC7, 0xA4, 0x25, 0x67, 0x99, 0x3A, 0xAE, 0x65, 0xCC, 0x4F, 0xE2, 0x52, 0xDC, 0x3C, 0xBD
};

mAES::mAES()
{
	MyCryptProv = 0;
	MyCryptKey = 0;
}

mAES::~mAES()
{
	Deinit();
}

//暗号化キーを解放し、初期状態に戻します
void mAES::Deinit( void )
{
	if( MyCryptKey )
	{
		CryptDestroyKey( MyCryptKey );
		MyCryptKey = 0;
	}
	if( MyCryptProv )
	{
		CryptReleaseContext( MyCryptProv , 0 );
		MyCryptProv = 0;
	}
}

bool mAES::Init( const SecureAString& key , const BYTE* salt , DWORD saltsize )
{
	BOOL result = true;
	HCRYPTHASH hash = 0;

	DWORD tmpkeylen = 0;
	BYTE* tmpkey = nullptr;

	//プロバイダ初期化
	if( !InitProvider() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュプロバイダが初期化されていません" );
		result = false;
		goto end;
	}

	//パスワードからハッシュを生成
	if( !CryptCreateHash( MyCryptProv , CALG_SHA_256 , 0 , 0 , &hash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトを生成できません" );
		result = false;
		goto end;
	}

	//キーのサイズをチェック。引っかかることはないと思うが。
	if( key.size() > MAXDWORD )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"キーが長すぎます" );
		result = false;
		goto end;
	}

	//キーのハッシュを生成
	if( !CryptHashData( hash , (const BYTE*)key.c_str() , (DWORD)key.size() , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"パスワードのハッシュに失敗しました" );
		result = false;
		goto end;
	}

	//saltのハッシュを生成。指定がない場合は固定値のハッシュを使う。
	if( salt == nullptr || saltsize == 0 )
	{
		if( !CryptHashData( hash , SHA_HashSalt , sizeof( SHA_HashSalt ) , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"パスワードのハッシュに失敗しました" );
			result = false;
			goto end;
		}
	}
	else
	{
		if( !CryptHashData( hash , salt , saltsize , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"パスワードのハッシュに失敗しました" );
			result = false;
			goto end;
		}
	}

	//ハッシュオブジェクトができたので、暗号化ハンドルを生成する
	result = Init( hash );

end:
	if( hash )
	{
		CryptDestroyHash( hash );
	}
	return result;
}

bool mAES::Init( const BYTE* key )
{
	BOOL result = true;
	HCRYPTHASH hash_sha = 0;

	//プロバイダ初期化
	if( !InitProvider() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プロバイダが初期化されていません" );
		result = false;
		goto end;
	}

	//ハッシュオブジェクトの生成
	if( !CryptCreateHash( MyCryptProv , CALG_SHA_256 , 0 , 0 , &hash_sha ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトを生成できませんでした" );
		result = false;
		goto end;
	}

	if( !CryptSetHashParam( hash_sha , HP_HASHVAL , key , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"パスワードのハッシュに失敗しました" );
		result = false;
		goto end;
	}

	//ハッシュオブジェクトができたので、暗号化ハンドルを生成する
	result = Init( hash_sha );

end:
	if( hash_sha )
	{
		CryptDestroyHash( hash_sha );
	}
	return result;
}

bool mAES::Init( HCRYPTHASH hash )
{
	BOOL result = true;

	//鍵を格納したハッシュオブジェクトができたので、
	//AES鍵を生成する。
	if( !CryptDeriveKey( MyCryptProv , CALG_AES_256 , hash , ( 256 << 16 ) , &MyCryptKey ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"AES鍵の生成に失敗しました" );
		result = false;
		goto end;
	}

	//パディングモードの設定(PKCS5)
	DWORD padding;
	padding = PKCS5_PADDING;
	if( !CryptSetKeyParam( MyCryptKey , KP_PADDING , (BYTE*)&padding , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"パディングの設定に失敗しました" );
		result = false;
		goto end;
	}

	//初期化ベクタの設定(初期化ベクタは0なので、SetIVで自分でセットすること)
	BYTE init_vector[16];
	ZeroMemory( init_vector , sizeof( init_vector ) );
	if( !SetIV( init_vector ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"IVの設定に失敗しました" );
		result = false;
		goto end;
	}

	//CBCモードに設定
	DWORD block_mode;
	block_mode = CRYPT_MODE_CBC;
	if( !CryptSetKeyParam ( MyCryptKey , KP_MODE , (BYTE*)&block_mode , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化モードの設定に失敗しました" );
		result = false;
		goto end;
	}

end:
	return result;
}

bool mAES::IsInitialized( void )const
{
	return MyCryptKey != 0;
}

//IVをセットする
// iv は16バイトのバイナリ
bool mAES::SetIV( const BYTE* iv )
{
	if( !CryptSetKeyParam( MyCryptKey , KP_IV , iv , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化モードの設定に失敗しました" );
		return false;
	}
	return true;
}

bool mAES::InitProvider( void )
{
	//二重初期化の禁止
	if( MyCryptProv )
	{
		//初期化ずみ
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化オブジェクトは初期化済みです" );
		return false;
	}

	//プロバイダの初期化
	if( !CryptAcquireContext( &MyCryptProv , 0 , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プロバイダの初期化ができませんでした" );
		return false;
	}
	return true;

}

