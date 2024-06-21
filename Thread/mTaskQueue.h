//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MTASKQUEUE_H_INCLUDED
#define MTASKQUEUE_H_INCLUDED

#include "mStandard.h"
#include "General/mCriticalSectionContainer.h"
#include "Thread/mTaskBase.h"
#include "Thread/mWorkerThreadPool.h"
#include <deque>
#include <unordered_map>

class mTaskQueue
{
public:

	mTaskQueue( mWorkerThreadPool& wtp );
	virtual ~mTaskQueue();

	//�^�X�N�̒ǉ�
	// high : ���̃^�X�N�ɗD�悵�ď�������
	// task : ��������^�X�N(mTaskBase���p�������N���X)
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task );

	//�^�X�N�̒ǉ�
	// high : ���̃^�X�N�ɗD�悵�ď�������
	// task : ��������^�X�N(mTaskBase���p�������N���X)
	// key : �����L�[�����^�X�N�𓯂����[�J�[�X���b�h�v�[�����łȂ�ׂ��������s���Ȃ��悤�ɂ���
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task , DWORD_PTR key );

	//�^�X�N�̒ǉ��i�u���b�L���O�j
	//�ǉ������^�X�N����������܂Ŗ߂�Ȃ�
	//���[�J�[�X���b�h�̃����o�[�X���b�h����Ăяo�����Ƃ͂ł��Ȃ�
	//���p�t�H�[�}���X��������̂ŁA���p���Ȃ�����
	// high : ���̃^�X�N�ɗD�悵�ď�������
	// task : ��������^�X�N(mTaskBase���p�������N���X)
	threadsafe bool AddTaskBlocking( bool high , mTaskBase::Ticket& task );

	//�^�X�N�̒ǉ��i�u���b�L���O�j
	//�ǉ������^�X�N����������܂Ŗ߂�Ȃ�
	//���[�J�[�X���b�h�̃����o�[�X���b�h����Ăяo�����Ƃ͂ł��Ȃ�
	//���p�t�H�[�}���X��������̂ŁA���p���Ȃ�����
	// high : ���̃^�X�N�ɗD�悵�ď�������
	// task : ��������^�X�N(mTaskBase���p�������N���X)
	// key : �����L�[�����^�X�N�𓯂����[�J�[�X���b�h�v�[�����łȂ�ׂ��������s���Ȃ��悤�ɂ���
	threadsafe bool AddTaskBlocking( bool high , mTaskBase::Ticket& task , DWORD_PTR key );

	//�ŏI�^�X�N�̒ǉ�
	// high : ���̃^�X�N�ɗD�悵�ď�������
	// task : ��������^�X�N(mTaskBase���p�������N���X)
	//���̊֐��Ń^�X�N��ǉ�����ƁA�ȍ~��AddTask(),Seal()�͑S�Ď��s���܂��B
	threadsafe bool Seal( bool high , mTaskBase::Ticket& task );

	//�ŏI�^�X�N�̒ǉ�
	// high : ���̃^�X�N�ɗD�悵�ď�������
	// task : ��������^�X�N(mTaskBase���p�������N���X)
	// key : �����L�[�����^�X�N�𓯂����[�J�[�X���b�h�v�[�����łȂ�ׂ��������s���Ȃ��悤�ɂ���
	//���̊֐��Ń^�X�N��ǉ�����ƁA�ȍ~��AddTask(),Seal()�͑S�Ď��s���܂��B
	threadsafe bool Seal( bool high , mTaskBase::Ticket& task , DWORD_PTR key );

	//�^�X�N���I��
	//���̊֐����ĂԂƁA�ȍ~��AddTask(),Seal()�͑S�Ď��s���܂��B
	threadsafe bool Seal( void );

	//���s�҂��L���[�ɂ���^�X�N��S�Ď�����
	//���łɎ��s���J�n���ꂽ�^�X�N�́A��������Ȃ��B
	//�������ꂽ�^�X�N�̐����Ԃ�
	threadsafe DWORD CancelTask( void );

	//�A�C�h����Ԃł��邩����
	//�E�L���[�ɐς܂ꂽ�^�X�N���Ȃ�
	//�E�^�X�N�����s���Ă���X���b�h���Ȃ�
	threadsafe bool IsIdle( void )const;

	//�w�肵���^�X�NID�����^�X�N���������邩��Ԃ�
	// id : ���������^�X�NID
	// ret : �w�肵���^�X�NID�̐�
	threadsafe DWORD GetTaskIdCount( const AString& id )const;

	//���[�J�[�X���b�h�v�[���̃n���h����Ԃ�
	threadsafe mWorkerThreadPool& GetThreadPool( void )const;

private:

	mTaskQueue();
	mTaskQueue( const mTaskQueue& src );
	const mTaskQueue& operator=( const mTaskQueue& src );

protected:

	//�֘A�t�����Ă��郏�[�J�[�X���b�h�v�[��
	mWorkerThreadPool& MyWorkerThreadPool;

	//�N���e�B�J���Z�N�V����
	mutable mCriticalSectionContainer MyCriticalSection;

	//����ς݁H
	bool MyIsSealed;

	//�^�X�NID���Ƃ̏��
	struct TaskInformation
	{
		DWORD Count;		//���݂��̃^�X�NID�ɑ����Ă���^�X�N�̐�(�Q�ƃJ�E���g)
		DWORD Executing;	//���݂��̃^�X�NID�Ŏ��s���̃^�X�N�̐�
		TaskInformation()
		{
			Count = 0;
			Executing = 0;
		}
	};
	using TaskInformationMap = std::unordered_map< AString , TaskInformation >;
	TaskInformationMap MyTaskInformationMap;

	//�^�X�NID�̎Q�ƃJ�E���g���C���N�������g����
	threadsafe void TaskInformationIncrement( const AString& id );

	//�^�X�NID�̎Q�ƃJ�E���g���f�N�������g����
	threadsafe void TaskInformationDecrement( const AString& id );

	//�N���e�B�J���w��̃^�X�N�����s���Ă��邩�ǂ���
	bool MyIsCritical;

	//�^�X�N�L���[�̃G���g��
	using TicketQueue = std::deque< mTaskBase::Ticket >;
	TicketQueue MyWaiting;		//���s�҂��L���[

	//���s���̃^�X�N�̐�
	DWORD MyActiveTask;

	//�^�X�N�̒ǉ�
	//���̃N���X�̃C���X�^���X�̃f�X�g���N�^�����s�J�n�Ȍ�́A���̃R�[���͎��s���ꂸ���s����B
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task , bool isFinal , DWORD_PTR key );

	//�^�X�N�̏������[�`��
	static void TaskRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

};

#endif //MTASKQUEUE_H_INCLUDED

