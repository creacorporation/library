//----------------------------------------------------------------------------
// �n�b�V�������N���X
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
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���A���S���Y���̎w�肪�s���ł�" , alg );
	}
	return 0;
}

bool mHash::Init( HashAlgorithm alg )
{
	if( !MyCryptProvider )
	{
		if( !CryptAcquireContext( &MyCryptProvider , NULL , NULL , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_�̏������Ɏ��s���܂���" );
			MyCryptProvider = 0;
			return false;
		}
	}

	//�I�u�W�F�N�g������
	ReleaseHashObject();

	MyHashAlgorithm = alg;
	ALG_ID algorithm_id = HashAlgorithm2AlgId( MyHashAlgorithm );

	if( !CryptCreateHash( MyCryptProvider , algorithm_id , 0 , 0 , &MyCryptHash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�̏������Ɏ��s���܂���" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V�������Ɏ��s���܂���" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���T�C�Y���擾�ł��܂���" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"�u���b�N�T�C�Y���擾�ł��܂���" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���������ʂ��擾�ł��܂���" );
		return false;
	}
	return true;
}

bool mHash::GetResult( BYTE* retResult , DWORD len )const
{
	if( !retResult )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@���w�肳��Ă��܂���" );
		return false;
	}
	if( len < GetResultLen() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�̗ʂ��s�\���ł�" );
		return false;
	}

	if( !CryptGetHashParam( MyCryptHash , HP_HASHVAL , retResult , &len , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���������ʂ��擾�ł��܂���" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���������ʂ��擾�ł��܂���" );
		return false;
	}

	//������`���Ƀt�H�[�}�b�g����
	return Binary2String( retResult, hash.get(), hashlen );
}

bool mHash::Hash( const WString& filename )
{
	//�t�@�C���J��
	mFileReadStream fp;
	if( !fp.Open( filename ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�t�@�C�����J�����Ƃ��ł��܂���" , filename );
		return false;
	}
	return Hash( fp );
}

bool mHash::Hash( mFile::Option opt )
{
	//�t�@�C���J��
	mFileReadStream fp;
	if( !fp.Open( opt ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�t�@�C�����J�����Ƃ��ł��܂���" , opt.Path );
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

