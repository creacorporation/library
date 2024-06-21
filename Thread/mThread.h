//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MTHREAD_H_INCLUDED
#define MTHREAD_H_INCLUDED

#include "mStandard.h"
#include <process.h>
#include <memory>

//�X���b�h�̃x�[�X�N���X

//HOW TO USE
//1.���̃N���X���p�����ATaskFunction()�ɕʃX���b�h�Ŏ��s����������������
//2.Begin()�Ƀ��[�U��`�̈�����n��
//3.TaskFunction()���ʃX���b�h������s�����
//4.TaskFunction()���Ԃ�ƃX���b�h���I������
//5.�X���b�h���I������O�ɃI�u�W�F�N�g�̃C���X�^���X���폜����ƌ��ʂ��s��ɂȂ�

class mThread
{
private:

	friend unsigned int __stdcall ThreadBaseFunc( void* );

public:
	mThread();
	virtual ~mThread();

	//�X���b�h���J�n����
	//arg : �X���b�h�ɓn������
	//�X���b�h�̓T�X�y���h��ԂŊJ�n���܂��B
	//ret : �������^
	bool Begin( std::shared_ptr<void> arg );

	//�X���b�h���J�n����
	//�X���b�h�̓T�X�y���h��ԂŊJ�n���܂��B
	//ret : �������^
	bool Begin( void );

	//�X���b�h�ɑ΂���v���Z�b�T�R�A�̐e�a�����Z�b�g����
	// mask : �A�t�B�j�e�B�}�X�N
	bool SetAffinityMask( DWORD_PTR mask );

	//��~���̃X���b�h���ĊJ����
	//retPrevCount : �w�肷��Ǝ��s�O�̃T�X�y���h�J�E���g�̒l��Ԃ��܂�
	//ret : �������^
	threadsafe bool Resume( DWORD* retPrevCount = nullptr );

	//���s���̃X���b�h���~����
	//retPrevCount : �w�肷��Ǝ��s�O�̃T�X�y���h�J�E���g�̒l��Ԃ��܂�
	//ret : �������^
	threadsafe bool Suspend( DWORD* retPrevCount = nullptr );

	//�X���b�h���I������
	//ret : �������^
	//�E�X���b�h���I������܂Ő����Ԃ��܂���
	//�E�X���b�h���T�X�y���h���Ă���Ƃ��ɂ��̊֐����ĂԂƎ��s���܂�
	//�E�Ăяo�������g�̃X���b�h���I�����邱�Ƃ͂ł��܂���
	threadsafe bool End( void );

	//�X���b�h�ɏI������悤�w������
	//�E�w�����邾���ŁA���ۂɏI������܂łɂ̓^�C�����O������
	//�E�Ăяo�������g�̃X���b�h�ɑ΂��ČĂяo�����Ƃ��ł��܂�
	//ret : �������^
	threadsafe bool FinishRequest( void );

	//�X���b�h��(������)�I������
	//ret : �������^
	bool Terminate( void );

	//�X���b�h�����s�����ۂ��𒲂ׂ�
	threadsafe bool IsValid( void )const;

	//�X���b�hID�𓾂�
	threadsafe unsigned int GetThreadId( void )const;

private:

	mThread( const mThread& src );
	const mThread& operator=( const mThread& src );

	//�X���b�h�Ŏ��s���鏈��
	virtual unsigned int TaskFunction() = 0;

	//�ϐ�������
	bool Clear( void );

protected:

	//�X���b�h�̃n���h��
	HANDLE MyHandle;

	//�X���b�hID
	unsigned int MyThreadId;

	//�X���b�h�̏I���V�O�i��
	//FinishRequest���Ă΂��ƃV�O�i����ԂɂȂ�
	HANDLE MyTerminateSignal;

	//Begin()�œn���ꂽ����
	std::shared_ptr<void> MyArg;

};





#endif //MTHREAD_H_INCLUDED