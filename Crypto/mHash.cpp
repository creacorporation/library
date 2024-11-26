//----------------------------------------------------------------------------
// ハッシュ処理クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mHash.h"

mHash::mHash()
{
	MyHashAlgorithm = HashAlgorithm::SHA256;
	MyCryptProvider = 0;
	MyCryptHash = 0;
}

mHash::~mHash()
{
	ReleaseHashObject();
	if( MyCryptProvider )
	{
		CryptReleaseContext( MyCryptProvider , 0 );
		MyCryptProvider = 0;
	}
}

void mHash::ReleaseHashObject( void )
{
	if( MyCryptHash )
	{
		CryptDestroyHash( MyCryptHash );
		MyCryptHash = 0;
	}
}

ALG_ID mHash::HashAlgorithm2AlgId( HashAlgorithm alg )const
{
	switch( alg )
	{
	case HashAlgorithm::MD5:
		return CALG_MD5;
	case HashAlgorithm::SHA1:
		return CALG_SHA1;
	case HashAlgorithm::SHA256:
		return CALG_SHA_256;
	case HashAlgorithm::SHA512:
		return CALG_SHA_512;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュアルゴリズムの指定が不正です" , (uint32_t)alg );
	}
	return 0;
}

bool mHash::Init( HashAlgorithm alg )
{
	if( !MyCryptProvider )
	{
		if( !CryptAcquireContext( &MyCryptProvider , NULL , NULL , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"暗号化プロバイダの初期化に失敗しました" );
			MyCryptProvider = 0;
			return false;
		}
	}

	//オブジェクト初期化
	ReleaseHashObject();

	MyHashAlgorithm = alg;
	ALG_ID algorithm_id = HashAlgorithm2AlgId( MyHashAlgorithm );

	if( !CryptCreateHash( MyCryptProvider , algorithm_id , 0 , 0 , &MyCryptHash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュオブジェクトの初期化に失敗しました" );
		goto errorend;
	}
	return true;

errorend:
	ReleaseHashObject();
	CryptReleaseContext( MyCryptProvider , 0 );
	MyCryptProvider = 0;
	return false;

}

bool mHash::Hash( const BYTE* data , DWORD len )
{
	if( !CryptHashData( MyCryptHash , data , len , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ処理に失敗しました" );
		return false;
	}
	return true;
}

DWORD mHash::GetResultLen( void )const
{
	DWORD retlen = 0;
	DWORD datalen = sizeof( DWORD );

	if( !CryptGetHashParam( MyCryptHash , HP_HASHSIZE , (BYTE*)&retlen , &datalen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュサイズを取得できません" );
		return 0;
	}
	return retlen;

}

DWORD mHash::GetBlockSize( void )const
{
	DWORD retlen = 0;
	DWORD datalen = sizeof( DWORD );

	if( !CryptGetKeyParam( MyCryptHash , KP_BLOCKLEN , (BYTE*)&retlen , &datalen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ブロックサイズを取得できません" );
		return 0;
	}
	return retlen;
}

bool mHash::GetResult( HashData& retResult , DWORD& retLen )const
{
	retLen = GetResultLen();
	retResult.reset( mNew BYTE[ retLen ] );

	if( !CryptGetHashParam( MyCryptHash , HP_HASHVAL , retResult.get() , &retLen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ処理結果を取得できません" );
		return false;
	}
	return true;
}

bool mHash::GetResult( BYTE* retResult , DWORD len )const
{
	if( !retResult )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファが指定されていません" );
		return false;
	}
	if( len < GetResultLen() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファの量が不十分です" );
		return false;
	}

	if( !CryptGetHashParam( MyCryptHash , HP_HASHVAL , retResult , &len , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ処理結果を取得できません" );
		return false;
	}
	return true;
}

bool mHash::GetResult( HashResult& retResult )const
{
	return GetResult( retResult.Data , retResult.Len );
}

bool mHash::GetResult( AString& retResult )const
{
	retResult.clear();

	HashData hash;
	DWORD hashlen;
	if( !GetResult( hash , hashlen ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ハッシュ処理結果を取得できません" );
		return false;
	}

	//文字列形式にフォーマットする
	return Binary2String( retResult, hash.get(), hashlen );
}

bool mHash::Hash( const WString& filename )
{
	//ファイル開く
	mFileReadStream fp;
	if( !fp.Open( filename ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ファイルを開くことができません" , filename );
		return false;
	}
	return Hash( fp );
}

bool mHash::Hash( mFile::Option opt )
{
	//ファイル開く
	mFileReadStream fp;
	if( !fp.Open( opt ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ファイルを開くことができません" , opt.Path );
		return false;
	}
	return Hash( fp );
}

bool mHash::Hash( mFileReadStream& fp )
{
	BYTE buff[ 4096 ];
	while( !fp.IsEOF() )
	{
		size_t readsize;
		if( !fp.ReadBinary( buff , sizeof( buff ) , &readsize , mFileReadStream::OnLineReadError::LINEREADERR_TRUNCATE ) )
		{
			if( readsize == 0 )
			{
				continue;
			}
		}
		if( !Hash( buff , readsize ) )
		{
			return false;
		}
	}
	return true;
}

bool mHash::Hash( mFileReadStream& fp , uint32_t len )
{
	BYTE buff[ 4096 ];
	size_t readsize;
	while( !fp.IsEOF() && len )
	{
		size_t reqsize = ( len < sizeof( buff ) ) ? ( len ) : ( sizeof( buff ) );
		if( !fp.ReadBinary( buff , reqsize , &readsize , mFileReadStream::OnLineReadError::LINEREADERR_TRUNCATE ) )
		{
			if( readsize == 0 )
			{
				continue;
			}
		}
		if( !Hash( buff , readsize ) )
		{
			return false;
		}
		len -= readsize;
	}
	return true;
}

