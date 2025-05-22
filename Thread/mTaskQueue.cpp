//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
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
	return;
}

mTaskQueue::~mTaskQueue()
{
	return;
}

mWorkerThreadPool& mTaskQueue::GetThreadPool( void ) const
{
	return MyWorkerThreadPool;
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
	//タスクは未使用か？
	if( task->MyTaskStatus != mTaskBase::TaskStatus::STATUS_NOTSTARTED )
	{
		RaiseError( g_ErrorLogger , 0 , L"タスクが再利用されています" );
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
			MyTicketQueue.push_front( task );
		}
		else
		{
			//そうでなければ末尾に追加
			MyTicketQueue.push_back( task );
		}

		//状態を更新
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
	}

	if( !AddTask() )
	{
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_ABORTED;
		AsyncEvent( task->Notifier.OnAbort , *this , task , true );
		return false;
	}
	return true;
}

threadsafe bool mTaskQueue::AddTask( void )
{
	if( !MyWorkerThreadPool.AddTask( TaskRoutine , 0 , (DWORD_PTR)this , (DWORD_PTR)this ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"完了ポートへのタスク登録が失敗しました" );
		return false;
	}
	return true;
}

bool mTaskQueue::AddTaskBlocking( bool high , mTaskBase::Ticket& task , uint32_t timeout )
{
	bool result = false;

	//ポインタチェック
	if( task == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"タスクのポインタがヌルです" );
		goto finish;
	}
	//タスクは未使用か？
	if( task->MyTaskStatus != mTaskBase::TaskStatus::STATUS_NOTSTARTED )
	{
		RaiseError( g_ErrorLogger , 0 , L"タスクが再利用されています" );
		goto finish;
	}
	//メンバースレッドのチェック
	if( MyWorkerThreadPool.IsPoolMember() )
	{
		RaiseError( g_ErrorLogger , 0 , L"スレッドプールのメンバースレッドからブロッキングタスクは追加できません" );
		goto finish;
	}
	//完了オブジェクトの確認
	if( task->MyCompleteObject == 0 )
	{
		task->MyCompleteObject = CreateEventA( nullptr , true , false , nullptr );
		if( task->MyCompleteObject == 0 )
		{
			RaiseError( g_ErrorLogger , 0 , L"完了オブジェクトの生成が失敗" );
			goto finish;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"完了オブジェクトがすでに登録されている" );
		goto finish;
	}
	//タスクの登録
	if( !AddTask( high , task , false ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"タスクの登録が失敗" );
		goto finish;
	}
	//完了の待機
	switch( WaitForSingleObject( task->MyCompleteObject , timeout ) )
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_TIMEOUT:
		CreateLogEntry( g_ErrorLogger , 0 , L"タスクの待機がタイムアウトしました" );
		SetLastError( ADDTASKBLOCKING_TIMEOUT );
		goto finish;
	default:
		RaiseError( g_ErrorLogger , 0 , L"タスクの完了待機が失敗" );
		goto finish;
	}
	result = true;

finish:
	//完了オブジェクト破棄
	if( task->MyCompleteObject )
	{
		CloseHandle( task->MyCompleteObject );
		task->MyCompleteObject = 0;
	}
	return result;
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
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , (int)info.Mode );
	}
}

DWORD mTaskQueue::GetActiveTaskIdCount( const AString& id )const
{
	mCriticalSectionTicket critical( MyCriticalSection );
	DWORD result = 0;
	for( TaskInformationMap::const_iterator itr = MyTaskInformationMap.begin() ; itr != MyTaskInformationMap.end() ; itr++ )
	{
		if( itr->first.find( id ) == 0 )
		{
			result += itr->second;
		}
	}
	return result;
}

DWORD mTaskQueue::GetTaskIdCount( const AString& id )const
{
	DWORD result = 0;
	mCriticalSectionTicket critical( MyCriticalSection );
	for( TicketQueue::const_iterator itr = MyTicketQueue.begin() ; itr != MyTicketQueue.end() ; itr++ )
	{
		if( (*itr)->MyTaskId.find( id ) == 0 )
		{
			result++;
		}
	}
	return result;
}

DWORD mTaskQueue::CancelTask( void )
{
	TicketQueue todelete;
	{
		/*CRITICALSECTION*/
		mCriticalSectionTicket critical( MyCriticalSection );
		for( TicketQueue::iterator itr = MyTicketQueue.begin() ; itr != MyTicketQueue.end() ; )
		{
			switch( (*itr)->MyTaskStatus )
			{
			case mTaskBase::TaskStatus::STATUS_QUEUED:
			case mTaskBase::TaskStatus::STATUS_SUSPENDED:
				todelete.push_back( std::move( *itr ) );
				itr = MyTicketQueue.erase( itr );
				continue;
			default:
				itr++;
				break;
			}
		}
	}

	for( TicketQueue::iterator itr = todelete.begin() ; itr != todelete.end() ; itr++ )
	{
		(*itr)->MyTaskStatus = mTaskBase::TaskStatus::STATUS_CANCELED;
		(*itr)->CancelFunction( *itr );
		AsyncEvent( (*itr)->Notifier.OnCancel , *this , (*itr) , true );

		//完了オブジェクトが存在していればシグナル状態に
		if( (*itr)->MyCompleteObject )
		{
			SetEvent( (*itr)->MyCompleteObject );
		}
	}

	return DWORD( todelete.size() );
}

void mTaskQueue::TaskInformationIncrement( const AString& id )
{
	mCriticalSectionTicket critical( MyCriticalSection );
	MyTaskInformationMap[ id ]++;
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

	if( itr->second == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , "タスクの参照カウントがマイナスになりました" , id );
	}
	else
	{
		itr->second--;
		if( itr->second == 0 )
		{
			MyTaskInformationMap.erase( itr );
		}
	}
	return;
}

bool mTaskQueue::IsIdle( void )const
{
	mCriticalSectionTicket critical( MyCriticalSection );
	return MyTicketQueue.empty();
}

bool mTaskQueue::TaskRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 )
{
	mTaskQueue* queue = (mTaskQueue*)Param2;

	//次のタスクを取得
	mTaskBase::Ticket task;
	{ /*CRITICALSECTION*/
		mCriticalSectionTicket critical( queue->MyCriticalSection );

		if( queue->MyTicketQueue.empty() )
		{
			return false;
		}

		for( TicketQueue::iterator itr = queue->MyTicketQueue.begin() ; itr != queue->MyTicketQueue.end() ; itr++ )
		{
			switch( (*itr)->MyScheduleType )
			{
			case mTaskBase::ScheduleType::Normal:
				break;
			case mTaskBase::ScheduleType::Critical:
				if( !queue->MyTaskInformationMap.empty() )
				{
					return false;
				}
				break;
			case mTaskBase::ScheduleType::IdLock:
				if( queue->MyTaskInformationMap.count( (*itr)->MyTaskId ) )
				{
					return false;
				}
				break;
			case mTaskBase::ScheduleType::IdPostpone:
				if( queue->MyTaskInformationMap.count( (*itr)->MyTaskId ) ) 
				{
					continue;
				}
				break;
			default:
				break;
			}
			if( (*itr)->MyTaskStatus != mTaskBase::TaskStatus::STATUS_QUEUED )
			{
				continue;
			}

			task = *itr;
			break;
		}
		if( task == nullptr )
		{
			return false;
		}
		//ステータスを実行中に
		queue->TaskInformationIncrement( task->MyTaskId );
		task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_INPROGRESS;
		task->MyParent = queue;
	}

	//自分の他にもう1スレッド同時実行されるようにタスクを追加
	queue->AddTask();

	//タスクの実行
	mTaskBase::TaskFunctionResult result = task->TaskFunction( task );

	//タスクの完了
	{ 
		if( result == mTaskBase::TaskFunctionResult::RESULT_INPROGRESS )
		{
			//※タスクは一時中断した
			/*CRITICALSECTION*/
			{
				mCriticalSectionTicket critical( queue->MyCriticalSection );
				queue->TaskInformationDecrement( task->MyTaskId );

				switch( task->MyTaskStatus )
				{
				case mTaskBase::TaskStatus::STATUS_INPROGRESS:
					task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_SUSPENDED;
					break;
				case mTaskBase::TaskStatus::STATUS_WAKINGUP:
					task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
					break;
				default:
					RaiseAssert( g_ErrorLogger , 0 , "タスクの状態が不正 %d" , (int)task->MyTaskStatus );
					task->MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
					break;
				}
			}
			AsyncEvent( task->Notifier.OnSuspend , *queue , task , true );
		}
		else
		{
			//※タスクが完了した
			/*CRITICALSECTION*/
			{
				mCriticalSectionTicket critical( queue->MyCriticalSection );
				queue->TaskInformationDecrement( task->MyTaskId );
				//親の情報を消す
				task->MyParent = nullptr;
				//キューから削除する
				for( TicketQueue::iterator itr = queue->MyTicketQueue.begin() ; itr != queue->MyTicketQueue.end() ; itr++ )
				{
					if( itr->get() == task.get() )
					{
						queue->MyTicketQueue.erase( itr );
						break;
					}
				}
			}
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
			//完了オブジェクトが存在していればシグナル状態に
			if( task->MyCompleteObject )
			{
				SetEvent( task->MyCompleteObject );
			}
		}
	}
	return true;
}

threadsafe bool mTaskQueue::Wakeup( mTaskBase& p )
{
	{
		mCriticalSectionTicket crit( MyCriticalSection );
		if( p.MyTaskStatus == mTaskBase::TaskStatus::STATUS_INPROGRESS )
		{
			p.MyTaskStatus = mTaskBase::TaskStatus::STATUS_WAKINGUP;
		}
		else
		{
			p.MyTaskStatus = mTaskBase::TaskStatus::STATUS_QUEUED;
		}
	}
	return AddTask();
}

