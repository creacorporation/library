//----------------------------------------------------------------------------
// RSA暗号化クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mRSAVerifier.h"
#include "../General/mErrorLogger.h"

mRSAVerifier::mRSAVerifier()
{
}

mRSAVerifier::~mRSAVerifier()
{

}

bool mRSAVerifier::DupeHCRYPTHASH( const mHash& data , HCRYPTHASH& retHash )const
{
	mHash::HashData hash_value;
	DWORD hash_len;

	//渡されたハッシュの値を取得し、テンポラリのハッシュオブジェクトを作成する
	//※渡されたハッシュオブジェクトと、このオブジェクトはハッシュプロバイダのインスタンスが違うので、
	//　いったんエクスポートしないとエラーになる
	if( !data.GetResult( hash_value , hash_len ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ値を取得できません" );
		return false;
	}
	if( !CryptCreateHash( MyCryptProv , data.HashAlgorithm2AlgId( data.MyHashAlgorithm ) , 0 , 0 , &retHash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトを生成できませんでした" );
		return false;
	}
	if( !CryptSetHashParam( retHash , HP_HASHVAL , hash_value.get() , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ値を設定できませんでした" );
		return false;
	}

	return true;
}

bool mRSAVerifier::Sign( const mHash& data , SignBuffer& retSign , DWORD& retWritten )const
{
	bool result = false;
	HCRYPTHASH hash = 0;

	//渡されたハッシュの値を取得し、テンポラリのハッシュオブジェクトを作成する
	//※渡されたハッシュオブジェクトと、このオブジェクトはハッシュプロバイダのインスタンスが違うので、
	//　いったんエクスポートしないとエラーになる
	if( !DupeHCRYPTHASH( data , hash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトを複製できませんでした" );
		goto errorend;
	}

	//バッファの確保
	retWritten = 0;
	if( !CryptSignHash( hash , AT_KEYEXCHANGE , nullptr , 0 , nullptr , &retWritten ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"署名格納に必要なバッファサイズを算出できません" );
		goto errorend;
	}
	retSign.reset( mNew BYTE[ retWritten ] );

	//署名の作成
	if( !CryptSignHash( hash , AT_KEYEXCHANGE , nullptr , 0 , retSign.get() , &retWritten ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"署名に失敗しました" );
		goto errorend;
	}
	//成功
	result = true;
	CreateLogEntry( g_ErrorLogger , 0 , L"署名を行いました" );

errorend:
	CryptDestroyHash( hash );
	return false;

}

bool mRSAVerifier::Verify( const mHash& data , const BYTE* Sign , DWORD SignLen )const
{
	bool result = false;
	HCRYPTHASH hash = 0;

	//入力チェック
	if( !Sign )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"署名のバッファがヌルです" );
		goto errorend;
	}

	//渡されたハッシュの値を取得し、テンポラリのハッシュオブジェクトを作成する
	//※渡されたハッシュオブジェクトと、このオブジェクトはハッシュプロバイダのインスタンスが違うので、
	//　いったんエクスポートしないとエラーになる
	if( !DupeHCRYPTHASH( data , hash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトを複製できませんでした" );
		goto errorend;
	}

	if( !CryptVerifySignature( hash , Sign , SignLen , MyCryptKeyPub , 0 , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"署名の検証に失敗しました" );
		goto errorend;
	}

	//成功
	result = true;

errorend:
	CryptDestroyHash( hash );
	return false;


}

