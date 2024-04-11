//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MWORKERTHREADPOOL_H_INCLUDED
#define MWORKERTHREADPOOL_H_INCLUDED

#include "mStandard.h"

#include "General/mCriticalSectionContainer.h"
#include "mWorkerThread.h"
#include <deque>

class mWorkerThreadPool
{
public:
	mWorkerThreadPool();
	virtual ~mWorkerThreadPool();

	//�_���R�A�̐����1���Ȃ��������X���b�h���쐬����
	static const int THREAD_LOGICAL_CORE_MINUS1 = 0;
	//�_���R�A�̐��Ɠ����������X���b�h���쐬����
	static const int THREAD_LOGICAL_CORE = -1;
	//�����R�A�̐����1���Ȃ��������X���b�h���쐬����
	static const int THREAD_PHYSICAL_CORE_MINUS1 = -2;
	//�����R�A�̐��Ɠ����������X���b�h���쐬����
	static const int THREAD_PHYSICAL_CORE = -3;

	//���[�J�[�X���b�h�̐���
	// threads : �������郏�[�J�[�X���b�h�̐�
	// �@���̐����Ȃ�΁A�w�肵�����̃��[�J�[�X���b�h���쐬����B
	// �@0�ȉ��̒l�Ȃ�΁A��L�萔�̏����Ń��[�J�[�X���b�h���쐬����B
	// min_threads : �蓮�܂��͎����ݒ肵���X���b�h�����A���̐��������ꍇ�́A���̐��ɕ␳����
	//		0�ȉ��̏ꍇ�͕␳�͍s��Ȃ�
	// max_threads : �蓮�܂��͎����ݒ肵���X���b�h�����A���̐�������ꍇ�́A���̐��ɕ␳����
	//		0�ȉ��̏ꍇ�͕␳�͍s��Ȃ�
	// ret : �������^
	//�Emin_threads��max_threads���Ƃ��Ɏw�肷��ꍇ�A�召�֌W�����������ƃG���[�ɂȂ�
	//�Ethread��0�ȉ��̒l���w�肵�āA�����ݒ肵���X���b�h����0�ȉ��ɂȂ����ꍇ�Amin_thread�����w�肾������1�ɕ␳����
	bool Begin( int threads , int min_threads = 0 , int max_threads = 0 );

	//���[�J�[�X���b�h�̏I��
	//���������ԂŏI�����Ȃ��ƁA�c��̃^�X�N����������Ȃ����߁A
	//������(���\�[�X)���[�N�ɂȂ�܂��B
	//�菇1:�֘A�t�����t�@�C���n���h����������΁A�����̓ǂݏ�����S�ďI������
	//      �E�n���h����j������Ƃ���܂ł�邱��
	//      �E���̎菇���T�{���Ă��N���b�V���͂��Ȃ����f�[�^����������\������
	//�菇2:GetTaskCount()��0�ɂȂ�܂őҋ@����
	//      �EAddTask�Œǉ������^�X�N��S�Ċ��������邱��
	//      �E�I���菇�J�n��Ƀ^�X�N��ǉ�����ꍇ�͓��ɒ���
	//�菇3:End()���Ăяo��
	threadsafe bool End( void );

	//IO�����|�[�g�ƁA�n���h�����֘A�t����
	// handle : FILE_FLAG_OVERLAPPED�t���O�����ĊJ���ꂽ�t�@�C���Ȃǂ̃n���h��
	// callback : IO�������ɌĂяo���R�[���o�b�N�֐�
	bool Attach( HANDLE handle , LPOVERLAPPED_COMPLETION_ROUTINE callback );

	//�^�X�N�����p�R�[���o�b�N�֐��̒�`
	// pool : �R�[���o�b�N�֐����Ăяo�������[�J�[�X���b�h�v�[��
	// Param1 : AddTask���ɓn�����p�����[�^
	// Param2 : AddTask���ɓn�����p�����[�^
	using CallbackFunction = void(*)( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

	//�^�X�N�̒ǉ�
	// callback : �X���b�h�v�[������Ăяo���R�[���o�b�N�֐�
	// Param1 : �R�[���o�b�N�֐��ɓn�����l�i�C�ӂ̒l��OK�j
	// Param2 : �R�[���o�b�N�֐��ɓn�����l�i�C�ӂ̒l��OK�j
	threadsafe bool AddTask( CallbackFunction callback , DWORD Param1, DWORD_PTR Param2 );

	//���ݕێ����Ă��関�����^�X�N�̐����擾����
	//��Attach�Ŋ֘A�t����IO�̊����͊܂܂Ȃ�
	threadsafe DWORD GetTaskCount( void )const;

	//�X���b�h�̐��𓾂�
	DWORD GetThreadCount( void )const;

private:
	mWorkerThreadPool( const mWorkerThreadPool& src );
	const mWorkerThreadPool& operator=( const mWorkerThreadPool& src );

private:

	//�X���b�h�{��
	using ThreadPool = std::deque<mWorkerThread>;
	ThreadPool MyThreadPool;

	//�N���e�B�J���Z�N�V����
	mutable mCriticalSectionContainer MyCriticalSection;

	//���ݕێ����Ă���^�X�N�̐�
	DWORD MyQueuedTaskCount;

	//IO�����|�[�g
	HANDLE MyIoPort;

	//�^�X�N���
	struct TaskInfoEntry
	{
		mWorkerThreadPool& Parent;
		CallbackFunction TaskFunction;
		DWORD Param1;
		DWORD_PTR Param2;

		TaskInfoEntry( mWorkerThreadPool& parent ) : Parent( parent )
		{
			TaskFunction = nullptr;
			Param1 = 0;
			Param2 = 0;
		}
	};

protected:
	friend class mWorkerThread;		//�A�N�Z�X������N���X

	//IO�|�[�g�̃n���h�����擾����
	HANDLE GetHandle( void )const;

private:
	//�������[�`�� �^�X�N�p
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

};



#endif


