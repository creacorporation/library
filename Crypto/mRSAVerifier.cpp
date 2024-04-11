//----------------------------------------------------------------------------
// RSA�Í����N���X
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

	//�n���ꂽ�n�b�V���̒l���擾���A�e���|�����̃n�b�V���I�u�W�F�N�g���쐬����
	//���n���ꂽ�n�b�V���I�u�W�F�N�g�ƁA���̃I�u�W�F�N�g�̓n�b�V���v���o�C�_�̃C���X�^���X���Ⴄ�̂ŁA
	//�@��������G�N�X�|�[�g���Ȃ��ƃG���[�ɂȂ�
	if( !data.GetResult( hash_value , hash_len ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���l���擾�ł��܂���" );
		return false;
	}
	if( !CryptCreateHash( MyCryptProv , data.HashAlgorithm2AlgId( data.MyHashAlgorithm ) , 0 , 0 , &retHash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�𐶐��ł��܂���ł���" );
		return false;
	}
	if( !CryptSetHashParam( retHash , HP_HASHVAL , hash_value.get() , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���l��ݒ�ł��܂���ł���" );
		return false;
	}

	return true;
}

bool mRSAVerifier::Sign( const mHash& data , SignBuffer& retSign , DWORD& retWritten )const
{
	bool result = false;
	HCRYPTHASH hash = 0;

	//�n���ꂽ�n�b�V���̒l���擾���A�e���|�����̃n�b�V���I�u�W�F�N�g���쐬����
	//���n���ꂽ�n�b�V���I�u�W�F�N�g�ƁA���̃I�u�W�F�N�g�̓n�b�V���v���o�C�_�̃C���X�^���X���Ⴄ�̂ŁA
	//�@��������G�N�X�|�[�g���Ȃ��ƃG���[�ɂȂ�
	if( !DupeHCRYPTHASH( data , hash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�𕡐��ł��܂���ł���" );
		goto errorend;
	}

	//�o�b�t�@�̊m��
	retWritten = 0;
	if( !CryptSignHash( hash , AT_KEYEXCHANGE , nullptr , 0 , nullptr , &retWritten ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����i�[�ɕK�v�ȃo�b�t�@�T�C�Y���Z�o�ł��܂���" );
		goto errorend;
	}
	retSign.reset( mNew BYTE[ retWritten ] );

	//�����̍쐬
	if( !CryptSignHash( hash , AT_KEYEXCHANGE , nullptr , 0 , retSign.get() , &retWritten ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����Ɏ��s���܂���" );
		goto errorend;
	}
	//����
	result = true;
	CreateLogEntry( g_ErrorLogger , 0 , L"�������s���܂���" );

errorend:
	CryptDestroyHash( hash );
	return false;

}

bool mRSAVerifier::Verify( const mHash& data , const BYTE* Sign , DWORD SignLen )const
{
	bool result = false;
	HCRYPTHASH hash = 0;

	//���̓`�F�b�N
	if( !Sign )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����̃o�b�t�@���k���ł�" );
		goto errorend;
	}

	//�n���ꂽ�n�b�V���̒l���擾���A�e���|�����̃n�b�V���I�u�W�F�N�g���쐬����
	//���n���ꂽ�n�b�V���I�u�W�F�N�g�ƁA���̃I�u�W�F�N�g�̓n�b�V���v���o�C�_�̃C���X�^���X���Ⴄ�̂ŁA
	//�@��������G�N�X�|�[�g���Ȃ��ƃG���[�ɂȂ�
	if( !DupeHCRYPTHASH( data , hash ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���I�u�W�F�N�g�𕡐��ł��܂���ł���" );
		goto errorend;
	}

	if( !CryptVerifySignature( hash , Sign , SignLen , MyCryptKeyPub , 0 , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����̌��؂Ɏ��s���܂���" );
		goto errorend;
	}

	//����
	result = true;

errorend:
	CryptDestroyHash( hash );
	return false;


}

