//----------------------------------------------------------------------------
// AES�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09�`
//----------------------------------------------------------------------------

#define MAES_CPP_COMPILE
#include "mAES.h"
#include "../General/mErrorLogger.h"

//�Q�l�ɂȂ邩������Ȃ�URL
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

//�Í����L�[��������A������Ԃɖ߂��܂�
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

	//�v���o�C�_������
	if( !InitProvider() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���v���o�C�_������������Ă��܂���" );
		result = false;
		goto end;
	}

	//�p�X���[�h����n�b�V���𐶐�
	if( !CryptCreateHash( MyCryptProv , CALG_SHA_256 , 0 , 0 , &hash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�𐶐��ł��܂���" );
		result = false;
		goto end;
	}

	//�L�[�̃T�C�Y���`�F�b�N�B���������邱�Ƃ͂Ȃ��Ǝv�����B
	if( key.size() > MAXDWORD )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�L�[���������܂�" );
		result = false;
		goto end;
	}

	//�L�[�̃n�b�V���𐶐�
	if( !CryptHashData( hash , (const BYTE*)key.c_str() , (DWORD)key.size() , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�X���[�h�̃n�b�V���Ɏ��s���܂���" );
		result = false;
		goto end;
	}

	//salt�̃n�b�V���𐶐��B�w�肪�Ȃ��ꍇ�͌Œ�l�̃n�b�V�����g���B
	if( salt == nullptr || saltsize == 0 )
	{
		if( !CryptHashData( hash , SHA_HashSalt , sizeof( SHA_HashSalt ) , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�p�X���[�h�̃n�b�V���Ɏ��s���܂���" );
			result = false;
			goto end;
		}
	}
	else
	{
		if( !CryptHashData( hash , salt , saltsize , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�p�X���[�h�̃n�b�V���Ɏ��s���܂���" );
			result = false;
			goto end;
		}
	}

	//�n�b�V���I�u�W�F�N�g���ł����̂ŁA�Í����n���h���𐶐�����
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

	//�v���o�C�_������
	if( !InitProvider() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v���o�C�_������������Ă��܂���" );
		result = false;
		goto end;
	}

	//�n�b�V���I�u�W�F�N�g�̐���
	if( !CryptCreateHash( MyCryptProv , CALG_SHA_256 , 0 , 0 , &hash_sha ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�𐶐��ł��܂���ł���" );
		result = false;
		goto end;
	}

	if( !CryptSetHashParam( hash_sha , HP_HASHVAL , key , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�X���[�h�̃n�b�V���Ɏ��s���܂���" );
		result = false;
		goto end;
	}

	//�n�b�V���I�u�W�F�N�g���ł����̂ŁA�Í����n���h���𐶐�����
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

	//�����i�[�����n�b�V���I�u�W�F�N�g���ł����̂ŁA
	//AES���𐶐�����B
	if( !CryptDeriveKey( MyCryptProv , CALG_AES_256 , hash , ( 256 << 16 ) , &MyCryptKey ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"AES���̐����Ɏ��s���܂���" );
		result = false;
		goto end;
	}

	//�p�f�B���O���[�h�̐ݒ�(PKCS5)
	DWORD padding = PKCS5_PADDING;
	if( !CryptSetKeyParam( MyCryptKey , KP_PADDING , (BYTE*)&padding , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�f�B���O�̐ݒ�Ɏ��s���܂���" );
		result = false;
		goto end;
	}

	//�������x�N�^�̐ݒ�(�������x�N�^��0�Ȃ̂ŁASetIV�Ŏ����ŃZ�b�g���邱��)
	BYTE init_vector[16]={ 0 };
	if( !SetIV( init_vector ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"IV�̐ݒ�Ɏ��s���܂���" );
		result = false;
		goto end;
	}

	//CBC���[�h�ɐݒ�
	DWORD block_mode = CRYPT_MODE_CBC;
	if( !CryptSetKeyParam ( MyCryptKey , KP_MODE , (BYTE*)&block_mode , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í������[�h�̐ݒ�Ɏ��s���܂���" );
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

//IV���Z�b�g����
// iv ��16�o�C�g�̃o�C�i��
bool mAES::SetIV( const BYTE* iv )
{
	if( !CryptSetKeyParam( MyCryptKey , KP_IV , iv , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í������[�h�̐ݒ�Ɏ��s���܂���" );
		return false;
	}
	return true;
}

bool mAES::InitProvider( void )
{
	//��d�������̋֎~
	if( MyCryptProv )
	{
		//����������
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����I�u�W�F�N�g�͏������ς݂ł�" );
		return false;
	}

	//�v���o�C�_�̏�����
	if( !CryptAcquireContext( &MyCryptProv , 0 , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v���o�C�_�̏��������ł��܂���ł���" );
		return false;
	}
	return true;

}

