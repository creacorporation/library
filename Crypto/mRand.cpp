//----------------------------------------------------------------------------
// �������N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MRAND_CPP_COMPILING
#include "mRand.h"
#include "../General/mErrorLogger.h"

mRand::mRand()
{
	MyCryptProv = 0;
}

mRand::~mRand()
{
	FreeProvider();
}

//�v���o�C�_�̏�����
bool mRand::InitProvider( void )
{
	//�Í����v���o�C�_�͏��������݁H
	if( MyCryptProv )
	{
		//���ɏ������ς�
		return true;
	}
	//�Í����v���o�C�_�̏�����
	if( !CryptAcquireContext( &MyCryptProv , nullptr , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_�̏������Ɏ��s���܂���" );
		return false;
	}
	return true;
}

//�v���o�C�_�̉��
bool mRand::FreeProvider( void )
{
	if( MyCryptProv )
	{
		CryptReleaseContext( MyCryptProv , 0 );
		MyCryptProv = 0;
	}
	return true;
}

//UINT�`���A32�r�b�g�̗���
UINT  mRand::RandInt32( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );
	return val;
}

//���̐�����A31�r�b�g�̗���
INT   mRand::RandUInt31( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );
	return (INT)( val >> 1 );
}

//���[0,1]�̎���(0�ȏ�1�ȉ�)
DOUBLE mRand::RandDouble1( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );

	return (DOUBLE)val * ( 1.0 / 4294967295.0 ); 
}

//���J���[0,1)�̎���(0�ȏ�1����)
DOUBLE mRand::RandDouble2( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );

	return (DOUBLE)val * ( 1.0 / 4294967296.0 ); 
}

//�J���(0,1)�̎���(0���傫��1��菬����)
DOUBLE mRand::RandDouble3( void )
{
	UINT val;
	RandFill( (BYTE*)&val , sizeof( val ) );

	return ( (DOUBLE)val + 0.5 ) * ( 1.0 / 4294967296.0 ); 
}

//���[0,1]�̎���(0�ȏ�1�ȉ�)
DOUBLE mRand::RandClose( void )
{
	return RandDouble1();
}

//���J���[0,1)�̎���(0�ȏ�1����)
DOUBLE mRand::RandSemiOpen( void )
{
	return RandDouble2();
}

//�J���(0,1)�̎���(0���傫��1��菬����)
DOUBLE mRand::RandOpen( void )
{
	return RandDouble3();
}

//�n�����o�b�t�@�𗐐��l�Ŗ��߂�
void mRand::RandFill( BYTE* buffer , DWORD bufferlen )
{
	if( !buffer )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@���w�肳��Ă��܂���" );
		return;
	}
	if( !InitProvider() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_������������Ă��܂���" );
		SecureZeroMemory( buffer , bufferlen );
		return;
	}
	if( !CryptGenRandom( MyCryptProv , bufferlen , buffer ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�n�b�V���������ʂ��擾�ł��܂���" );
		return;
	}
	return;
}

//�w�肵��2�l�Ԃɋϓ����z����l
UINT mRand::RandBetween( UINT val1 , UINT val2 )
{
	if( val1 == val2 )
	{
		return val1;
	}
	else if( val1 == 0 && val2 == UINT_MAX )
	{
		return RandInt32();
	}

	UINT diff = ( val1 < val2 ) ? ( val2 - val1 + 1 ) : ( val1 - val2 + 1 );
	return UINT( RandSemiOpen() * diff );
}

