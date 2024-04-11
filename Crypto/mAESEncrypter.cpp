//----------------------------------------------------------------------------
// AES�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09�`
//----------------------------------------------------------------------------


#include "mAESEncrypter.h"
#include <math.h>
#include "../General/mErrorLogger.h"

mAESEncrypter::mAESEncrypter()
{

}

mAESEncrypter::~mAESEncrypter()
{

}

bool mAESEncrypter::Encrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retEncryptData , DWORD& retWritten )const
{
	//�o�b�t�@�`�F�b�N
	if( !data )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�����w��ł�" );
		return false; 
	}

	//�K�v�o�b�t�@���̎Z�o
	retWritten = datalen;
	DWORD bufflen = datalen;

	if( !CryptEncrypt( MyCryptKey , 0 , isfinal , 0 , nullptr , &bufflen , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����Ɏ��s���܂���" );
		return false;
	}
	//�o�b�t�@�̊m��
	retEncryptData.reset( mNew BYTE[ bufflen ] );
	MoveMemory( retEncryptData.get() , data , datalen );

	//�Í�������
	if( !CryptEncrypt( MyCryptKey , 0 , isfinal , 0 , retEncryptData.get() , &retWritten , bufflen ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����Ɏ��s���܂���" );
		return false;
	}
	return true;
}


bool mAESEncrypter::CreateRandomHeader( BYTE* data , DWORD bufflen , DWORD& retwritten )
{
	if( !data )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�����w��ł�" );
		return false;
	}
	if( bufflen == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@����0�ł�" );
		return false;
	}
	if( bufflen < 32 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@���Z�����܂�" );
	}

	//���o�C�g�������_���f�[�^�����H
	DWORD factor = 0;
	CryptGenRandom( MyCryptProv , sizeof( DWORD ) , (BYTE*)&factor );
	double rnd = (double)factor * ( 1.0 / 4294967296.0 );

	//17������菭�Ȃ���ΑS��
	//17�����ȏ�Ȃ�Ώ��Ȃ��Ă�16���������_���f�[�^�ɂ���B
	DWORD len;
	if( bufflen < 17 )
	{
		len = bufflen;
	}
	else
	{
		DWORD rndm = bufflen - 16;
		len = (DWORD)( rnd * rndm ) + 16 + 1;
	}

	//�����_���o�C�g�𐶐�
	CryptGenRandom( MyCryptProv , len , data );

	//�������������_���o�C�g�̍ŏI�o�C�g�ȊO��0����������0����Ȃ��l�Ŗ��߂�
	for( DWORD i = 0 ; i < len - 1 ; i++ )
	{
		if( data[ i ] == 0 )
		{
			BYTE dt = 0;
			do
			{
				CryptGenRandom( MyCryptProv , 1 , &dt );
			}while( dt == 0 );
			data[ i ] = dt;
		}
	}

	//�ŏI�o�C�g���O�ɂ���
	data[ len - 1 ] = 0;

	//���߂��o�C�g�����Z�b�g
	retwritten = len;

	return true;
}
