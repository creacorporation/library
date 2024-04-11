//----------------------------------------------------------------------------
// RSA�Í����N���X
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

//�Í�������
bool mRSACipher::Encrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retEncrypted , DWORD& retWritten )const
{
	if( data == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�����w��ł�" );
		return false;
	}

	if( type == KEYTYPE::KEY_PRIVATE )
	{
		//�Í�����̃f�[�^�������߂�
		retWritten = datalen;
		DWORD bufflen = datalen;

		if( !CryptEncrypt( MyCryptKey , 0 , true , 0 , nullptr , &bufflen , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Í�����̃f�[�^�����擾�ł��܂���" );
			return false;
		}
		//�Í����O�̃f�[�^���R�s�[����
		retEncrypted.reset( mNew BYTE[ bufflen ] );
		MoveMemory( retEncrypted.get() , data , datalen );

		//�Í������s��
		if( !CryptEncrypt( MyCryptKey , 0 , true , 0 , retEncrypted.get() , &retWritten , bufflen ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Í����Ɏ��s���܂���" );
			return false;
		}
	}
	else if( type == KEYTYPE::KEY_PUBLIC )
	{
		//���J���̐ݒ�
		if( !const_cast<mRSACipher*>(this)->ExtractPublicKey() )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"���J���̐ݒ肪���s���܂���" );
			return false;
		}
		//�Í�����̃f�[�^�������߂�
		retWritten = datalen;
		DWORD bufflen = datalen;

		if( !CryptEncrypt( MyCryptKeyPub , 0 , true , 0 , nullptr , &bufflen , 0 ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Í�����̃f�[�^�����擾�ł��܂���" );
			return false;
		}
		//�Í����O�̃f�[�^���R�s�[����
		retEncrypted.reset( mNew BYTE[ bufflen ] );
		MoveMemory( retEncrypted.get() , data , datalen );

		//�Í������s��
		if( !CryptEncrypt( MyCryptKeyPub , 0 , true , 0 , retEncrypted.get() , &retWritten , bufflen ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Í����Ɏ��s���܂���" );
			return false;
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�s���ȃJ�M�̎�ނł�" , type );
		return false;
	}
	return true;
}

bool mRSACipher::Decrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retDecrypted , DWORD& retWritten )const
{
	if( data == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�����w��ł�" );
		return false;
	}

	if( type == KEYTYPE::KEY_PRIVATE )
	{
		//�����O�̃f�[�^���R�s�[����
		retDecrypted.reset( mNew BYTE[ datalen ] );
		MoveMemory( retDecrypted.get() , data , datalen );

		//�������s��
		retWritten = datalen;
		if( !CryptDecrypt( MyCryptKey , 0 , true , 0 , retDecrypted.get() , &retWritten ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�����Ɏ��s���܂���" );
			return false;
		}
	}
	else if( type == KEYTYPE::KEY_PUBLIC )
	{
		//���J���̐ݒ�
		if( !const_cast<mRSACipher*>(this)->ExtractPublicKey() )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"���J���̐ݒ肪���s���܂���" );
			return false;
		}
		//�����O�̃f�[�^���R�s�[����
		retDecrypted.reset( mNew BYTE[ datalen ] );
		MoveMemory( retDecrypted.get() , data , datalen );

		//�������s��
		retWritten = datalen;
		if( !CryptDecrypt( MyCryptKeyPub , 0 , true , 0 , retDecrypted.get() , &retWritten ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�����Ɏ��s���܂���" );
			return false;
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�s���ȃJ�M�̎�ނł�" , type );
		return false;
	}
	return true;
}


