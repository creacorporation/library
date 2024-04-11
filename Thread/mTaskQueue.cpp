//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
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
		RaiseError( g_ErrorLogger , 0 , L"タスクのポインタがヌルです" );
		return false;
	}

	{ /*CRITICALSECTION*/
		mCriticalSectionTicket critical( MyCriticalSection );

		//破棄中・最終タスク後は失敗させる
		if( MyIsSealed )
		{
			RaiseError( g_ErrorLogger , 0 , L"最終タスクがすでに登録済みです" );
			return false;
		}

		//最終タスク？
		if( isFinal )
		{
			MyIsSealed = true;
		}

		//キューに追加
		if( high )
		{
			//優先なら先頭に追加
			MyWaiting.push_front( task );
		}
		else
		{
			//そうでなければ末尾に追加
			MyWaiting.push_back( task );
		}

		//状態を更新
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
		TaskInformationIncrement( task->GetTaskId() );
	}

	if( !MyWorkerThreadPool.AddTask( TaskRoutine , high , (DWORD_PTR)this ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"完了ポートへのタスク登録が失敗しました" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , info.Mode );
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
		RaiseAssert( g_ErrorLogger , 0 , "タスクIDが登録されていません" , id );
		return;
	}

	if( itr->second.Count == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , "タスクの参照カウントがマイナスになりました" , id );
	}
	else
	{
		itr->second.Count--;
	}

	//参照カウントがゼロになった場合の処理
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

	//次のタスクを取得
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

		//ステータスを実行中に
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_INPROGRESS;
	}

	//タスクの実行
	mTaskBase::TaskFunctionResult result = task->TaskFunction( task );

	if( result == mTaskBase::TaskFunctionResult::RESULT_INPROGRESS )
	{
		//タスクは一時中断したので再スケジュールする
		AsyncEvent( task->Notifier.OnSuspend , *queue , task , true );

		bool addtask_result;
		{
			//再登録
			//一時中断したのだから、プライオリティに関係なく一番後ろにくっつける
			mCriticalSectionTicket critical( queue->MyCriticalSection );

			if( queue->MyWorkerThreadPool.AddTask( TaskRoutine , Param1 , (DWORD_PTR)queue ) )
			{
				//ステータスを実行待ちに
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
			RaiseError( g_ErrorLogger , 0 , L"完了ポートへのタスク登録が失敗しました" );
			AsyncEvent( task->Notifier.OnAbort , *queue , task , true );
		}
	}
	else
	{
		//タスクが完了した
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
			//アクティブなタスクの数を減算
			mCriticalSectionTicket critical( queue->MyCriticalSection );
			queue->MyActiveTask--;
			queue->TaskInformationDecrement( task->GetTaskId() );
		}
	}
	return;
}

