//----------------------------------------------------------------------------
// RSA暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mRSA.h"
#include "../General/mErrorLogger.h"

mRSA::mRSA()
{
	MyCryptProv = 0;
	MyCryptKey = 0;
	MyCryptKeyPub = 0;
}

mRSA::~mRSA()
{
	Clear();
	if( MyCryptProv )
	{
		CryptReleaseContext( MyCryptProv , 0 );
		MyCryptProv = 0;
	}
}

bool mRSA::Init( void )
{
	//RSAを使用するが、CRYPT_VERIFYCONTEXTを使用して差し支えない。
	//クライアントOSにおいては、CRYPT_VERIFYCONTEXTを使用した場合は一時メモリに鍵が保管される。
	//そしてハンドルの解放と同時に削除される。

	//＜MSDNのCRYPT_VERIFYCONTEXTの説明より抜粋＞
	//The application has no access to the persisted private keys of public/private key pairs. 
	//When this flag is set, temporary public/private key pairs can be created, but they are not persisted.
	//訳：アプリケーションは、鍵ペアの秘密鍵にアクセスすることはありません。
	//    このフラグがセットされた場合は、テンポラリな鍵ペアが作成されますが、保存されません。
	//http://msdn.microsoft.com/en-us/library/aa379886(v=vs.85).aspx より。

	if( MyCryptProv )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダはすでに初期化されています" );
		return false;
	}

	if( !CryptAcquireContext( &MyCryptProv , nullptr , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダの初期化に失敗しました" );
		return false;
	}
	return true;
}

bool mRSA::Init( const WString& container_name , bool machine_keyset )
{
	if( MyCryptProv )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダはすでに初期化されています" );
		return false;
	}

	DWORD flag = ( machine_keyset ) ? ( CRYPT_MACHINE_KEYSET ) : ( 0 );
	if( !CryptAcquireContext( &MyCryptProv , container_name.c_str() , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , flag ) )
	{
		flag |= CRYPT_NEWKEYSET;
		if( !CryptAcquireContext( &MyCryptProv , container_name.c_str() , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , flag ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダを初期化できませんでした" );
			return false;
		}
		else
		{
			CreateLogEntry( g_ErrorLogger , 0 , L"新しいキーコンテナを作成しました" , container_name );
		}
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"キーコンテナをロードしました" , container_name );
	}
	return true;
}

bool mRSA::DestroyKeyContainer( const WString& container_name , bool machine_keyset )
{
	HCRYPTPROV dummy_prov = 0;
	DWORD flag = CRYPT_DELETEKEYSET;
	flag |= ( machine_keyset ) ? ( CRYPT_MACHINE_KEYSET ) : ( 0 );

	if( !CryptAcquireContext( &dummy_prov , container_name.c_str() , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , flag ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"キーコンテナの削除ができませんでした" , container_name );
		return false;
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"キーコンテナを削除しました" , container_name );
		return true;
	}
}

bool mRSA::Clear( void )
{
	if( MyCryptKeyPub )
	{
		CryptDestroyKey( MyCryptKeyPub );
		MyCryptKeyPub = 0;
	}
	if( MyCryptKey )
	{
		CryptDestroyKey( MyCryptKey );
		MyCryptKey = 0;
	}
	return true;
}

bool mRSA::GenerateNewKey( KEYLENGTH len )
{
	USHORT bit;
	switch( len )
	{
	case KEYLENGTH::KEYLEN_2048BIT:
		bit = 2048;
		break;
	case KEYLENGTH::KEYLEN_4096BIT:
		bit = 4096;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"RSA鍵長の指定が不正です" );
		return false;
	}

	if( MyCryptProv == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダが初期化されていません" );
		return false;
	}
	if( !Clear() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"既存のハンドル解放に失敗しました" );
		return false;
	}

	if( !CryptGenKey( MyCryptProv , AT_KEYEXCHANGE , ( bit << 16 ) | CRYPT_EXPORTABLE , &MyCryptKey ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化鍵の生成が失敗しました" );
		return false;
	}
	return true;
}

DWORD mRSA::GetKeySize( mRSA::KEYTYPE type )
{
	DWORD size = 0;
	if( !GetKey( type , nullptr , size ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化鍵が設定されていません" , type );
		return 0;
	}
	return size;
}

//鍵バイナリを得る
bool mRSA::ExportKey( mRSA::KEYTYPE type , KeyBinary& retKey , DWORD& retWritten )
{
	retWritten = GetKeySize( type );
	retKey.reset( mNew BYTE[ retWritten ] );

	return GetKey( type , retKey.get() , retWritten );
}

bool mRSA::ImportKey( const BYTE* data , DWORD datalen )
{
	//鍵のフォーマットは、先頭はPUBLICKEYSTRUCなので、
	//そこからフォーマットを判別する。

	//http://msdn.microsoft.com/en-us/library/aa375601(v=vs.85).aspx
	//http://msdn.microsoft.com/en-us/library/aa387453(v=vs.85).aspx

	if( data == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが未指定です" );
		return false;
	}

	const PUBLICKEYSTRUC* header = (const PUBLICKEYSTRUC*)data;
	if( header->bType == PRIVATEKEYBLOB )
	{
		Clear();
		return ImportKeyInternal( &MyCryptKey , datalen , data );
	}
	else if( ( header->bType == PUBLICKEYBLOB ) || ( header->bType == PUBLICKEYBLOBEX ) )
	{
		Clear();
		return ImportKeyInternal( &MyCryptKeyPub , datalen , data );
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"データに鍵が含まれていません" );
		return false;
	}
}

bool mRSA::GetKey( mRSA::KEYTYPE type , BYTE* buffer , DWORD& size )
{
	if( MyCryptProv == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダが初期化されていません" );
		return false;
	}

	//値変換 (KEYTYPE → CryptExportKeyのblobtype)
	DWORD blobtype;
	switch( type )
	{
	case KEYTYPE::KEY_PRIVATE:
		blobtype = PRIVATEKEYBLOB;
		break;
	case KEYTYPE::KEY_PUBLIC:
		blobtype = PUBLICKEYBLOB;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"不正な鍵の種類です" , type );
		return false;
	}

	//値取得
	if( CryptExportKey( MyCryptKey , 0 , blobtype , 0 , buffer , &size ) )
	{
		return true;
	}
	if( CryptExportKey( MyCryptKeyPub , 0 , blobtype , 0 , buffer , &size ) )
	{
		return true;
	}
	RaiseAssert( g_ErrorLogger , 0 , L"鍵のエクスポートに失敗しました" );
	return false;

}

bool mRSA::ExtractPublicKey( void )
{
	BOOL result = true;
	DWORD keylen = 0;
	KeyBinary binkey;

	if( MyCryptKeyPub != 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"公開鍵がすでに設定されています" );
		goto end;
	}

	//鍵の長さを取得して、そのサイズのバッファを作る
	keylen = GetKeySize( KEYTYPE::KEY_PUBLIC );
	if( keylen == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"公開鍵の長さを取得できませんでした" );
		result = false;
		goto end;
	}
	binkey.reset( mNew BYTE[ keylen ] );

	//作ったバッファに鍵を取得
	if( !ExportKey( KEYTYPE::KEY_PUBLIC , binkey , keylen ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"公開鍵の保存に失敗しました" );
		result = false;
		goto end;
	}

	//取得した公開鍵をそのままインポートして公開鍵だけのハンドルを作る
	if( !ImportKeyInternal( &MyCryptKeyPub , keylen , binkey.get() ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"公開鍵のインポートに失敗しました" );
		result = false;
		goto end;
	}
end:
	if( keylen )
	{
		SecureZeroMemory( binkey.get() , keylen );
	}
	return result;

}

bool mRSA::ImportKeyInternal( HCRYPTKEY* key , DWORD len , const BYTE* buffer )
{
	if( !CryptImportKey( MyCryptProv , buffer , len , 0 , 0 , key ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"鍵のインポートに失敗しました" );
		return false;
	}
	return true;
}


