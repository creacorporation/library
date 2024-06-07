//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MTASKBASE_H_INCLUDED
#define MTASKBASE_H_INCLUDED

#include "mStandard.h"
#include "General/mNotifyOption.h"
#include "General/mTCHAR.h"
#include <memory>

//�X���b�h�v�[���ŏ���������^�X�N�̊��N���X

//HOW TO USE
//1.���̃N���X���p�����܂�
//  TaskFunction()�ɁA�X���b�h�v�[���ŏ����������������������܂�
//  CancelFunction()�ɁA��x�̓X���b�h�v�[���ɓo�^������A�L�����Z�����ꂽ�ꍇ�̏������������܂�
//�@���̑������o�ϐ��Ƃ��āA�X���b�h�v�[���ŏ�������f�[�^����ǉ����܂�
//2.���̃C���X�^���X��mTaskQueue::AddTask()�܂���mTaskQueue::Seal()�ɓn���ăX���b�h�v�[���̃L���[�ɓo�^���܂�
//3.�^�X�N�̏�����������ƁATaskFunction()���Ăяo����܂�
namespace Definitions_TaskBase
{
	enum TaskFunctionResult
	{
		//�^�X�N�̎��s�͊������A���ʂ͐����ł�����
		RESULT_FINISH_SUCCEED,
		//�^�X�N�̎��s�͊������A���ʂ͎��s�ł�����
		RESULT_FINISH_FAILED,
		//�^�X�N�̎��s���ł���
		RESULT_INPROGRESS,
	};

	enum TaskStatus
	{
		//�y�������z�^�X�N�͎��s����Ă��Ȃ�
		STATUS_NOTSTARTED ,
		//�y�����z�^�X�N�̎��s�͊������A���ʂ͐����ł�����
		STATUS_FINISH_SUCCEED ,
		//�y�����z�^�X�N�̎��s�͊������A���ʂ͎��s�ł�����
		STATUS_FINISH_FAILED ,
		//�y�������z�^�X�N�͎��s�҂��L���[�̒��ɓ����Ă���
		STATUS_QUEUED ,
		//�y�������z�^�X�N�̎��s���ł���
		STATUS_INPROGRESS ,
		//�y�����z�^�X�N�̓L�����Z�����ꂽ
		STATUS_CANCELED,
		//�y�����z�^�X�N�͓����G���[�ɂ����s�ł��Ȃ���ԂɂȂ��Ă���
		STATUS_ABORTED ,

	};
};

class mTaskBase
{
	friend class mTaskQueue;

public:
	virtual ~mTaskBase();

	using Ticket = std::shared_ptr< mTaskBase >;

	//�^�X�N�������ɌĂяo���R�[���o�b�N�֐��̐錾
	using CallbackFunction = void(*)( class mTaskQueue& queue , Ticket& ticket , DWORD_PTR parameter , bool result );

	//�^�X�N���s�̏��ʎw��
	enum ScheduleType
	{
		//���Ɏw��Ȃ�
		Normal,
		//���̃^�X�N���쒆�͊J�n�����A���쒆�͑��̃^�X�N���J�n�����Ȃ�
		Critical,
		//�L���[���œ���ID�̃^�X�N�����s���ł���΁A�^�X�N���J�n���Ȃ�
		IdLock,
		//�L���[���œ���ID�̃^�X�N�����s���ł���΁A�㑱�̊J�n�\�ȃ^�X�N�����s����
		IdPostpone
	};

	//�^�X�N�������̒ʒm�I�v�V����
	class NotifyOption : public mNotifyOption< CallbackFunction >
	{
	public:
		//�^�X�N�����������ꍇ
		NotifierInfo OnComplete;
		//�^�X�N�����f�����ꍇ
		NotifierInfo OnSuspend;
		//�^�X�N���L�����Z�����ꂽ�ꍇ
		NotifierInfo OnCancel;
		//�^�X�N��(�V�X�e���I�ȗ��R��)���s�s�\�ɂȂ����ꍇ
		NotifierInfo OnAbort;
	};

	//�^�X�N�������̒ʒm�I�v�V����
	NotifyOption Notifier;

	//���݂̃^�X�N�̏��
	using TaskStatus = Definitions_TaskBase::TaskStatus;

	//���݂̃^�X�N�̏󋵂𓾂�
	TaskStatus GetTaskStatus( void )const;

	//�^�X�NID�𓾂�
	inline const AString& GetTaskId( void )const noexcept
	{
		return MyTaskId;
	}

private:

	mTaskBase( const mTaskBase& src );
	const mTaskBase& operator=( const mTaskBase& src );

	//���݂̃^�X�N�̏��
	//��mTaskQueue������ɏ���������̂ŁA��������͐G��Ȃ�����
	TaskStatus MyTaskStatus;

protected:

	mTaskBase();

	// TaskId : �^�X�N�̖���
	mTaskBase( const AString& TaskId , ScheduleType ScheduleType = ScheduleType::Normal );

	//�^�X�NID
	const AString MyTaskId;

	//�^�X�N���s������
	const ScheduleType MyScheduleType;

	//�^�X�N�̎��s����
	using TaskFunctionResult = Definitions_TaskBase::TaskFunctionResult;

	//�^�X�N�����s�����Ƃ��Ăяo���܂�
	// ret : �^�X�N�̎��s���ʂ������l
	virtual TaskFunctionResult TaskFunction( const Ticket& task ) = 0;

	//�^�X�N���L�����Z�������Ƃ��Ăяo���܂�
	virtual void CancelFunction( const Ticket& task );

};



#endif //MWORKERTHREAD_H_INCLUDED

