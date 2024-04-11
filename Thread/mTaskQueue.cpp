//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mTaskQueue.h"
#include "mTaskBase.h"
#include "General/mErrorLogger.h"

static void AsyncEvent( const mTaskBase::NotifyOption::NotifierInfo& info , class mTaskQueue& queue , mTaskBase::Ticket& ticket , bool result );

mTaskQueue::mTaskQueue( mWorkerThreadPool& wtp ) :
	MyWorkerThreadPool( wtp )
{
	MyIsSealed = false;
	MyActiveTask = 0;
	return;
}

mTaskQueue::~mTaskQueue()
{
	return;
}

bool mTaskQueue::Seal( bool high , mTaskBase::Ticket& task )
{
	return AddTask( high , task , true );
}

bool mTaskQueue::Seal( void )
{
	mCriticalSectionTicket critical( MyCriticalSection );
	MyIsSealed = true;
	return true;
}


bool mTaskQueue::AddTask( bool high , mTaskBase::Ticket& task )
{
	return AddTask( high , task , false );
}

bool mTaskQueue::AddTask( bool high , mTaskBase::Ticket& task , bool isFinal )
{
	if( task == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"�^�X�N�̃|�C���^���k���ł�" );
		return false;
	}

	{ /*CRITICALSECTION*/
		mCriticalSectionTicket critical( MyCriticalSection );

		//�j�����E�ŏI�^�X�N��͎��s������
		if( MyIsSealed )
		{
			RaiseError( g_ErrorLogger , 0 , L"�ŏI�^�X�N�����łɓo�^�ς݂ł�" );
			return false;
		}

		//�ŏI�^�X�N�H
		if( isFinal )
		{
			MyIsSealed = true;
		}

		//�L���[�ɒǉ�
		if( high )
		{
			//�D��Ȃ�擪�ɒǉ�
			MyWaiting.push_front( task );
		}
		else
		{
			//�����łȂ���Ζ����ɒǉ�
			MyWaiting.push_back( task );
		}

		//��Ԃ��X�V
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
		TaskInformationIncrement( task->GetTaskId() );
	}

	if( !MyWorkerThreadPool.AddTask( TaskRoutine , high , (DWORD_PTR)this ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�����|�[�g�ւ̃^�X�N�o�^�����s���܂���" );
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_ABORTED;
		AsyncEvent( task->Notifier.OnAbort , *this , task , true );
		return false;
	}
	return true;
}

static void AsyncEvent( const mTaskBase::NotifyOption::NotifierInfo& info , class mTaskQueue& queue , mTaskBase::Ticket& ticket , bool result )
{
	if( info.Mode == mTaskBase::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mTaskBase::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&queue , info.Parameter );
	}
	else if( info.Mode == mTaskBase::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mTaskBase::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( queue , ticket , info.Parameter , result );
				if( !mTaskBase::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mTaskBase::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( queue , ticket , info.Parameter , result );
	}
	else if( info.Mode == mTaskBase::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
	{
		if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
		{
			SetEvent( info.Notifier.Handle );
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�񓯊�����̊����ʒm���@���s���ł�" , info.Mode );
	}
}

DWORD mTaskQueue::GetTaskIdCount( const AString& id )const
{
	mCriticalSectionTicket critical( MyCriticalSection );
	TaskInformationMap::const_iterator itr = MyTaskInformationMap.find( id );
	if( itr == MyTaskInformationMap.end() )
	{
		return 0;
	}
	return itr->second.Count;
}

DWORD mTaskQueue::CancelTask( void )
{
	TicketQueue todelete;

	{ /*CRITICALSECTION*/
		mCriticalSectionTicket critical( MyCriticalSection );
		todelete.swap( MyWaiting );
	}

	for( TicketQueue::iterator itr = todelete.begin() ; itr != todelete.end() ; itr++ )
	{
		(*itr)->MyTaskStatus = mTaskBase::TaskStatus::STATUS_CANCELED;
		(*itr)->CancelFunction( *itr );
		AsyncEvent( (*itr)->Notifier.OnCancel , *this , (*itr) , true );
	}

	{ /*CRITICALSECTION*/
		mCriticalSectionTicket critical( MyCriticalSection );
		for( TicketQueue::iterator itr = todelete.begin() ; itr != todelete.end() ; itr++ )
		{
			TaskInformationDecrement( (*itr)->GetTaskId() );
		}
	}
	return DWORD( todelete.size() );
}

void mTaskQueue::TaskInformationIncrement( const AString& id )
{
	mCriticalSectionTicket critical( MyCriticalSection );
	MyTaskInformationMap[ id ].Count++;
}

void mTaskQueue::TaskInformationDecrement( const AString& id )
{
	mCriticalSectionTicket critical( MyCriticalSection );
	TaskInformationMap::iterator itr = MyTaskInformationMap.find( id );

	if( itr == MyTaskInformationMap.end() )
	{
		RaiseAssert( g_ErrorLogger , 0 , "�^�X�NID���o�^����Ă��܂���" , id );
		return;
	}

	if( itr->second.Count == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , "�^�X�N�̎Q�ƃJ�E���g���}�C�i�X�ɂȂ�܂���" , id );
	}
	else
	{
		itr->second.Count--;
	}

	//�Q�ƃJ�E���g���[���ɂȂ����ꍇ�̏���
	if( itr->second.Count == 0)
	{
		MyTaskInformationMap.erase( itr );
	}
	return;
}

bool mTaskQueue::IsIdle( void )const
{
	mCriticalSectionTicket critical( MyCriticalSection );
	if( MyWaiting.size() == 0 && MyActiveTask == 0 )
	{
		return true;
	}
	return false;
}

void mTaskQueue::TaskRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 )
{
	mTaskQueue* queue = (mTaskQueue*)Param2;

	//���̃^�X�N���擾
	mTaskBase::Ticket task;
	{ /*CRITICALSECTION*/
		mCriticalSectionTicket critical( queue->MyCriticalSection );

		do
		{
			if( queue->MyWaiting.empty() )
			{
				return;
			}
			task = std::move( queue->MyWaiting.front() );
			queue->MyWaiting.pop_front();
			queue->MyActiveTask++;
		}
		while( task == nullptr );

		//�X�e�[�^�X�����s����
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_INPROGRESS;
	}

	//�^�X�N�̎��s
	mTaskBase::TaskFunctionResult result = task->TaskFunction( task );

	if( result == mTaskBase::TaskFunctionResult::RESULT_INPROGRESS )
	{
		//�^�X�N�͈ꎞ���f�����̂ōăX�P�W���[������
		AsyncEvent( task->Notifier.OnSuspend , *queue , task , true );

		bool addtask_result;
		{
			//�ēo�^
			//�ꎞ���f�����̂�����A�v���C�I���e�B�Ɋ֌W�Ȃ���Ԍ��ɂ�������
			mCriticalSectionTicket critical( queue->MyCriticalSection );

			if( queue->MyWorkerThreadPool.AddTask( TaskRoutine , Param1 , (DWORD_PTR)queue ) )
			{
				//�X�e�[�^�X�����s�҂���
				addtask_result = true;
				task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
				queue->MyWaiting.push_back( std::move( task ) );
			}
			else
			{
				addtask_result = false;
				task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_ABORTED;
			}
			queue->MyActiveTask--;
		}

		if( !addtask_result )
		{
			RaiseError( g_ErrorLogger , 0 , L"�����|�[�g�ւ̃^�X�N�o�^�����s���܂���" );
			AsyncEvent( task->Notifier.OnAbort , *queue , task , true );
		}
	}
	else
	{
		//�^�X�N����������
		if( result == mTaskBase::TaskFunctionResult::RESULT_FINISH_SUCCEED )
		{
			task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_FINISH_SUCCEED;
			AsyncEvent( task->Notifier.OnComplete , *queue , task , true );
		}
		else
		{
			task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_FINISH_FAILED;
			AsyncEvent( task->Notifier.OnComplete , *queue , task , false );
		}
		{
			//�A�N�e�B�u�ȃ^�X�N�̐������Z
			mCriticalSectionTicket critical( queue->MyCriticalSection );
			queue->MyActiveTask--;
			queue->TaskInformationDecrement( task->GetTaskId() );
		}
	}
	return;
}

