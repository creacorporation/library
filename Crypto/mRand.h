//----------------------------------------------------------------------------
// �������N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

//�������C�u����
//�EWindowsAPI�𗘗p���ė����𓾂܂�
//�ECryptGenRandom()���g�p
//�E�Í��Ɏg�p�\
//�E��Ԃ̃Z�[�u�͕s��

#ifndef MRAND_H_INCLUDED
#define MRAND_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>

class mRand
{
public:
	mRand();
	virtual ~mRand();

	//UINT�`���A32�r�b�g�̗���
	UINT  RandInt32( void );

	//���̐�����A31�r�b�g�̗���
	INT   RandUInt31( void );

	//���[0,1]�̎���(0�ȏ�1�ȉ�)
	DOUBLE RandDouble1( void );

	//���J���[0,1)�̎���(0�ȏ�1����)
	DOUBLE RandDouble2( void );

	//�J���(0,1)�̎���(0���傫��1��菬����)
	DOUBLE RandDouble3( void );

	//���[0,1]�̎���(0�ȏ�1�ȉ�)
	DOUBLE RandClose( void );

	//���J���[0,1)�̎���(0�ȏ�1����)
	DOUBLE RandSemiOpen( void );

	//�J���(0,1)�̎���(0���傫��1��菬����)
	DOUBLE RandOpen( void );

	//�n�����o�b�t�@�𗐐��l�Ŗ��߂�
	//buffer : �����f�[�^�̊i�[��
	//bufferlen : �i�[����o�C�g��
	void RandFill( BYTE* buffer , DWORD bufferlen );

	//�w�肵��2�l�Ԃɋϓ����z����l
	// ret : val1�`val2�̒l�ɕ��z����l(val1�Aval2�Ƃ��o�����܂�)
	UINT RandBetween( UINT val1 , UINT val2 );

private:
	mRand( const mRand& source );
	const mRand& operator=( const mRand& source ) = delete;

protected:

	//���������p�v���o�C�_
	HCRYPTPROV MyCryptProv;

	//�v���o�C�_�̏�����
	bool InitProvider( void );

	//�v���o�C�_�̉��
	bool FreeProvider( void );

};

#endif