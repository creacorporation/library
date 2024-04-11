//----------------------------------------------------------------------------
// �N���e�B�J���Z�N�V�����Ǘ�
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�N���e�B�J���Z�N�V�����̊Ǘ������܂��B

�EmCriticalSectionContainer�N���X
�@��enter/leave�ŃN���e�B�J���Z�N�V�����ɏo���肵�܂�

 �EmCriticalSectionTicket�N���X
�@�����ʂ̃I�u�W�F�N�g�����A���̃I�u�W�F�N�g���������Ă���Ԃ����N���e�B�J���Z�N�V�����ɓ���܂�
*/

//���g����(1)
#ifdef SAMPLE_CODE
void HowToUse1( void )
{
	mCriticalSectionContainer cs;	//�N���e�B�J���Z�N�V���������
	cs.Enter();	//����������N���e�B�J���Z�N�V����
		//�Ȃɂ���������
		//�Ȃɂ���������
		//�Ȃɂ���������
		//�Ȃɂ���������
	cs.Leave(); //�������܂ŃN���e�B�J���Z�N�V����
}
#endif

//���g����(2)
#ifdef SAMPLE_CODE

//�O���[�o���ȃN���e�B�J���Z�N�V�����I�u�W�F�N�g�����ꍇ�́Alib�Z�N�V�����ɔz�u����B
//���R�F���̃O���[�o���ȃI�u�W�F�N�g���A�R���X�g���N�^�ŃN���e�B�J���Z�N�V�������g���ƁA
//�@�@�@���������ɂ���ẮA���������̃N���e�B�J���Z�N�V�����I�u�W�F�N�g���������Ă��܂��댯�����邽�߁B
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
mCriticalSectionContainer cs;	//�O���[�o���ȃN���e�B�J���Z�N�V����

void HowToUse2( void )
{
	while( 1 )	//�������̃��[�v�̓N���e�B�J���Z�N�V�����ł͂Ȃ�
	{
		//������������
		if( some_condition )
		{
			break;
		}
	}

	{
		mCriticalSectionTicket( cs );	//����������N���e�B�J���Z�N�V����
		for( DWORD i = 0 ; i < MAX_SIZE ; i++ )
		{
			//������������
			if( some_condition )
			{
				return;	//�������܂ŃN���e�B�J���Z�N�V����
			}
		}
	}	//�������܂ŃN���e�B�J���Z�N�V����
	return;
}
#endif

#ifndef MCRITICALSECTIONCONTAINER_H_INCLUDED
#define MCRITICALSECTIONCONTAINER_H_INCLUDED

#include "mStandard.h"

//1�̃N���e�B�J���Z�N�V�������Ǘ�����N���X
class mCriticalSectionContainer
{
public:
	mCriticalSectionContainer()throw();
	virtual ~mCriticalSectionContainer()throw();

	//�N���e�B�J���Z�N�V�����̃X�s���J�E���g��ݒ肵�܂�
	//Count : �X�s���J�E���g�̒l
	//ret : �ȑO�ɐݒ肳��Ă����X�s���J�E���g�̒l
	DWORD SetSpinCount( DWORD Count );

	//�N���e�B�J���Z�N�V�����ɓ���܂��B
	//�N���e�B�J���Z�N�V�����ɓ����܂Ő����Ԃ��܂���B
	void Enter( void );

	//�N���e�B�J���Z�N�V�������甲���܂��B
	void Leave( void );

	//�N���e�B�J���Z�N�V�����ɓ����Γ���܂��B����Ȃ������炻�̂܂ܕԂ�܂� ��....
	//ret : �N���e�B�J���Z�N�V�������m�ۂł����ꍇ�^
	//      �N���e�B�J���Z�N�V�������m�ۂł��Ȃ������ꍇ�U(���̃X���b�h�������Ă���ꍇ)
	bool TryEnter( void );

private:

	//�R�s�[�֎~
	mCriticalSectionContainer( const mCriticalSectionContainer& source ) = delete;
	const mCriticalSectionContainer& operator=( const mCriticalSectionContainer& source ) = delete;

protected:
	//�N���e�B�J���Z�N�V�����̎���
	CRITICAL_SECTION MyCriticalSection;

};

//�N���e�B�J���Z�N�V�����͈̔́��C���X�^���X�̃X�R�[�v�ɂ��邽�߂̃N���X
class mCriticalSectionTicket
{
public:
	//�R���X�g���N�^
	//�R���X�g���N�^���s���ɃN���e�B�J���Z�N�V�������m�ۂ���܂��B
	mCriticalSectionTicket( mCriticalSectionContainer& critical_section )throw();

	//�f�X�g���N�^
	//�f�X�g���N�^���s���ɃN���e�B�J���Z�N�V�������������܂��B
	virtual ~mCriticalSectionTicket()throw();

private:

	//�R�s�[�֎~
	mCriticalSectionTicket() = delete;
	mCriticalSectionTicket( const mCriticalSectionTicket& source ) = delete;
	const mCriticalSectionTicket& operator=( const mCriticalSectionTicket& source ) = delete;

	//�m�ۑΏۂ̃N���e�B�J���Z�N�V����
	mCriticalSectionContainer& MySection;
};

//�O���[�o���I�u�W�F�N�g
#ifndef MCRITICALSECTIONCONTAINER_CPP_COMPILING
extern mCriticalSectionContainer g_CriticalSection;
#else
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
mCriticalSectionContainer g_CriticalSection;
#endif

#endif //MCRITICALSECTIONCONTAINER_H_INCLUDED

