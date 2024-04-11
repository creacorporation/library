//----------------------------------------------------------------------------
// �n�b�V�������N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

//�Q�lURL
// https://www.ipa.go.jp/security/rfc/RFC2104JA.html

#include "mHmac.h"

//warning C4200: ��W���̊g���@�\���g�p����Ă��܂�: �\���̂܂��͋��p�̒��ɃT�C�Y�� 0 �̔z�񂪂���܂��B
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
		//���łɃL�[������ꍇ�͔j������
		CryptDestroyKey( MyCryptKey );
		MyCryptKey = 0;
	}
}

bool mHmac::Init( mHash::HashAlgorithm alg , const BYTE* key , DWORD keylen , bool openssl )
{
	//HMAC�L�[�C���|�[�g�p�̍\����
	struct HashKey{
		BLOBHEADER Header;
		DWORD Length;
		BYTE Key[];
	};
	HashKey* hmac_key = nullptr;
	DWORD hmac_key_size = 0;

	//HMAC�I�u�W�F�N�g������
	if( !MyHashObject.Init( alg ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"HMAC�L�[�̏������Ɏ��s���܂���" );
		goto errorend;
	}

	//HMAC�L�[�ݒ�
	if( openssl )
	{
		//OpenSSL�ƌ݊��̃��[�h
		//��RFC2202��f���Ɏ�����������
		if( keylen < MyHashObject.GetBlockSize() )
		{
			//�L�[�̒������u���b�N����蒷���ꍇ�̓n�b�V�������
			if( !MyHashObject.Hash( key , keylen ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"HMAC�L�[�̐��������s���܂���" );
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
			//�u���b�N�����Z���ꍇ�͂��̂܂܎g��
			hmac_key_size = sizeof( HashKey ) + keylen;
			hmac_key = (HashKey*)malloc( hmac_key_size );
			hmac_key->Header.bType = PLAINTEXTKEYBLOB;
			hmac_key->Header.bVersion = CUR_BLOB_VERSION;
			hmac_key->Header.reserved = 0;
			hmac_key->Header.aiKeyAlg = CALG_RC2;
			hmac_key->Length = keylen;
			MoveMemory( hmac_key->Key , key , keylen );
		}

		//�L�[�I�u�W�F�N�g�̐���
		ReleaseHashObject();
		if( !CryptImportKey( MyHashObject.MyCryptProvider , (BYTE*)hmac_key , hmac_key_size , 0 , CRYPT_IPSEC_HMAC_KEY , &MyCryptKey ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"HMAC�L�[�I�u�W�F�N�g�𐶐��ł��܂���ł���" );
			goto errorend;
		}
		SecureZeroMemory( &hmac_key , hmac_key_size );
	}
	else
	{
		//�L�[���X�N�����u�����郂�[�h
		//���}�C�N���\�t�g��HMAC�����T���v���Ɠ������ʂ�Ԃ�����
		//�@https://msdn.microsoft.com/ja-jp/library/windows/desktop/aa382379(v=vs.85).aspx
		if( !CryptDeriveKey( MyHashObject.MyCryptProvider, CALG_RC4, MyHashObject.MyCryptHash, 0, &MyCryptKey ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"HMAC�L�[�p�̃n�b�V�������o���܂���" );
			goto errorend;
		}
	}

	//�I�u�W�F�N�g������
	if( !Reset() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�̏������Ɏ��s���܂���" );
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
	//�I�u�W�F�N�g������
	MyHashObject.ReleaseHashObject();
	if( !CryptCreateHash( MyHashObject.MyCryptProvider , CALG_HMAC , MyCryptKey , 0 , &MyHashObject.MyCryptHash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�̏������Ɏ��s���܂���" );
		return false;
	}

	//�n�b�V���A���S���Y���擾
	ALG_ID alg_id;
	DWORD datalen = sizeof( alg_id );
	if( !CryptGetHashParam( MyHashObject.MyCryptHash , HP_ALGID , (BYTE*)&alg_id , &datalen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���A���S���Y�����擾�ł��܂���" );
		return false;
	}

	//HMAC�̂��߂̏��\���̂�ݒ�
	MyHmacInfo.HashAlgid = alg_id;
	MyHmacInfo.cbInnerString = 0;	//�f�t�H���g���g�p(0x36 * 64)
	MyHmacInfo.cbOuterString = 0;	//�f�t�H���g���g�p(0x5C * 64)
	MyHmacInfo.pbInnerString = 0;
	MyHmacInfo.pbOuterString = 0;
	if( !CryptSetHashParam( MyHashObject.MyCryptHash, HP_HMAC_INFO, (BYTE*)&MyHmacInfo, 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"HMAC���\���̂�ݒ�ł��܂���" );
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
