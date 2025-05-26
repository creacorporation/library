//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mThread.h"
#include "General/mErrorLogger.h"

static void AsyncEvent( mThread& thread , const mThread::NotifyOption::NotifierInfo& info , unsigned int returncode )
{
	if( info.Mode == mThread::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mThread::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&thread , info.Parameter );
	}
	else if( info.Mode == mThread::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( thread , info.Parameter , returncode );
	}
	else if( info.Mode == mThread::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
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


unsigned int __stdcall ThreadBaseFunc( void* arg )
{
	//スレッドの開始エントリポイント
	mThread* thread_ptr = (mThread*)arg;

	//タスクの実行
	unsigned int result = thread_ptr->TaskFunction();

	//タスク終了のイベントコール
	AsyncEvent( *thread_ptr , thread_ptr->MyNotifyOption.OnFinish , result );

	//スレッド終了
	_endthreadex( result );
	return result;
}

mThread::mThread()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyThreadId = 0;
	MyTerminateSignal = INVALID_HANDLE_VALUE;
	MyArg = 0;
}

mThread::~mThread()
{
	Clear();
}

bool mThread::Begin( std::shared_ptr<void> arg , const NotifyOption* notifier )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"スレッドはすでに作成されています" , MyThreadId );
		return false;
	}

	//終了シグナルを生成する
	MyTerminateSignal = CreateEvent( 0 , true , false , 0 );
	MyArg = arg;

	//終了コールバック
	if( notifier )
	{
		MyNotifyOption = *notifier;
	}

	//スレッド開始
	MyHandle = (HANDLE)_beginthreadex( 0 , 0 , ThreadBaseFunc , this , CREATE_SUSPENDED , &MyThreadId );
	if( MyHandle == (HANDLE)-1 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"スレッドを開始できませんでした" );
		return false;
	}
	return true;
}

bool mThread::Begin( const NotifyOption* notifier )
{
	std::shared_ptr<void> arg;
	return Begin( arg , notifier );
}

bool mThread::SetAffinityMask( DWORD_PTR mask )
{
	return SetThreadAffinityMask( MyHandle , mask );
}

bool mThread::Resume( DWORD* retPrevCount )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"存在しないスレッドをレジュームしようとしました" );
		return false;
	}
	DWORD count = ResumeThread( MyHandle );
	if( retPrevCount )
	{
		*retPrevCount = count;
	}
	return count != (DWORD)-1;
}

bool mThread::Suspend( DWORD* retPrevCount )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"存在しないスレッドをサスペンドしようとしました" );
		return false;
	}
	DWORD count = SuspendThread( MyHandle );
	if( retPrevCount )
	{
		*retPrevCount = count;
	}
	return count != (DWORD)-1;
}

bool mThread::Clear()
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( (HANDLE)MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
		MyThreadId = 0;
	}

	if( MyTerminateSignal != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyTerminateSignal );
		MyTerminateSignal = INVALID_HANDLE_VALUE;
	}
	return true;
}

bool mThread::End( void )
{
	//自分自身を終了しようとしていないかチェックする
	if( GetCurrentThreadId() == MyThreadId )
	{
		RaiseError( g_ErrorLogger , 0 , L"スレッドが自分自身の終了を待機しようとしました" , MyThreadId );
		return false;
	}

	//終了シグナルをセットする
	if( !FinishRequest() )
	{
		return false;
	}

	//終了まで待機する
	while( 1 )
	{
		//sync=trueの場合、スレッドハンドルがシグナル状態（スレッドの終了を示す）になるまで待機
		//とりあえず200ms待ってみる
		DWORD wait_result = WaitForSingleObject( (HANDLE)MyHandle , 200 );

		if( wait_result == WAIT_OBJECT_0 )
		{
			//シグナル状態になった場合、正常終了
			break;
		}
		else if( wait_result == WAIT_TIMEOUT )
		{
			//タイムアウトの場合、サスペンドしていないか確認する
			DWORD suspend_count;
			if( !Suspend( &suspend_count ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"スレッドの状態を取得できません" );
				return false;
			}
			if( suspend_count )
			{
				//サスペンド中
				RaiseError( g_ErrorLogger , 0 , L"スレッドのサスペンド中に同期モードで終了しようとしました" );
				return false;
			}
			if( !Resume( &suspend_count ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"スレッドの状態を取得できません" );
				return false;
			}
		}
		else
		{
			//WAIT_OBJECT_0以外の場合、なにか事故ってスレッドが消滅している
			RaiseError( g_ErrorLogger , 0 , L"終了対象のスレッドはすでに存在しません" );
			Clear();
			return false;
		}
	}
	Clear();
	return true;
}

bool mThread::WaitForFinish( DWORD max_wait )
{
	DWORD wait_result = WaitForSingleObject( (HANDLE)MyHandle , max_wait );

	if( wait_result == WAIT_OBJECT_0 )
	{
		//シグナル状態になった場合、正常終了
		return true;
	}
	return false;
}

bool mThread::FinishRequest( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"存在しないスレッドを終了しようとしました" );
		return false;
	}

	//終了シグナルをセットする
	if( !SetEvent( MyTerminateSignal ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"終了シグナルのセットが失敗しました" );
		return false;
	}
	return true;
}

threadsafe bool mThread::IsFinishRequested( void )const
{
	DWORD wait_result = WaitForSingleObject( (HANDLE)MyTerminateSignal , 0 );

	if( wait_result == WAIT_OBJECT_0 )
	{
		//シグナル状態になった場合、終了要求を受けている
		return true;
	}
	return false;
}


bool mThread::Terminate( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"存在しないスレッドを強制終了しようとしました" );
		return false;
	}

	#pragma warning( push )
	#pragma warning( disable : 6258 )
	if( !TerminateThread( MyHandle , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"スレッドの強制終了が失敗しました" );
		return false;
	}
	#pragma warning( pop )

	Clear();
	return true;
}

bool mThread::IsValid( void )const
{
	return MyHandle != INVALID_HANDLE_VALUE;
}


//スレッドIDを得る
threadsafe unsigned int mThread::GetThreadId( void )const
{
	return MyThreadId;
}

