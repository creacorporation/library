//----------------------------------------------------------------------------
//�����Z���k�E�c�C�X�^���������N���X
// Copyright (C) 2005 Fingerling. All rights reserved.
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// ��2018�N6���̃o�[�W�������AC++11��random�ˑ��ɂȂ�܂����B
//----------------------------------------------------------------------------

#ifndef MMTWISTER_H_INCLUDED
#define MMTWISTER_H_INCLUDED

#include <random>

//�������C�u����
//�E�����Z���k�c�C�X�^�^�������𗘗p���ė����𓾂܂�
//�E�Í��ɂ͂��̂܂܂ł͎g�p�s��
//�E��Ԃ̃Z�[�u��

#include "mStandard.h"
#include <memory>

class mMTwister
{
public:

	mMTwister();

	//�����n���������
	bool Initialize( void );

	//����������̓�����Ԃ̎擾
	typedef std::unique_ptr<BYTE> RandState;

	//�������l��ŗ����n���������
	//seed : ������ԃf�[�^
	//len : ������ԃf�[�^�̒���
	//ret : �������^
	bool Initialize( const RandState& seed , DWORD len );

	//�����̌��݂̓�����Ԃ��擾����
	//retState : �擾��������
	//retLen : ������ԂƂ��Ċi�[���ꂽ�o�C�g��
	//ret : �������^
	bool GetState( RandState& retState , DWORD& retLen )const;

	//UINT�`���A32�r�b�g�̗���
	UINT RandInt32( void );

	//���̐�����A31�r�b�g�̗���
	INT RandUInt31( void );

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

	mMTwister( const mMTwister& src );
	const mMTwister& operator=( const mMTwister& src ) = delete;

protected:

	typedef std::mt19937 RandSource;
	RandSource MyRandSource;


};


#endif