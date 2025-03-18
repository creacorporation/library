//----------------------------------------------------------------------------
// タイマーコントロール
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mTimer.h"
#include "../General/mErrorLogger.h"
#include "../General/mCriticalSectionContainer.h"


static HANDLE g_TimerQueue = nullptr;
static DWORD  g_TimerQueueCount = 0;

mTimer::mTimer()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyCount = 0;

	{
		mCriticalSectionTicket critical( g_CriticalSection );
		if( g_TimerQueueCount == 0 )
		{
			g_TimerQueue = CreateTimerQueue();
		}
		g_TimerQueueCount++;
	}

}

mTimer::~mTimer()
{
	Stop();
	{
		mCriticalSectionTicket critical( g_CriticalSection );
		g_TimerQueueCount--;
		if( g_TimerQueueCount == 0 )
		{
			DeleteTimerQueue( g_TimerQueue );
			g_TimerQueue = 0;
		}
	}
}

bool mTimer::Setup( const NotificationOption& opt )
{
	SetCount( opt.Count );

	switch( opt.Method )
	{
	case NotificationOption::NotificationMethod::METHOD_NONE:
		MyOption.reset( mNew Option_None( *(Option_None*)&opt ) );
		break;
	case NotificationOption::NotificationMethod::METHOD_WINDOW_MESSAGE:
		MyOption.reset( mNew Option_WindowMessage( *(Option_WindowMessage*)&opt ) );
		break;
	case NotificationOption::NotificationMethod::METHOD_SIGNAL_OBJECT:
		MyOption.reset( mNew Option_SignalObject( *(Option_SignalObject*)&opt ) );
		break;
	case NotificationOption::NotificationMethod::METHOD_CALLBACK_FUNCTION:
		MyOption.reset( mNew Option_CallbackFunction( *(Option_CallbackFunction*)&opt ) );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"イベント通知方法の指定が誤っています" , (int)opt.Method );
		MyOption.reset( mNew Option_None( *(Option_None*)&opt ) );
		break;
	}

	if( opt.StartImmediate )
	{
		return Start();
	}
	return true;
}

bool mTimer::Restart( void )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		return true;
	}
	if( GetCount() != 0 )
	{
		if( !CreateTimerQueueTimer( &MyHandle , g_TimerQueue , TimerRoutine , this , MyOption->Interval , 0 , WT_EXECUTEONLYONCE ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"タイマーを開始できません" );
			return false;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"タイマーの残り実行回数は0です" );
		return false;
	}
	return true;
}

bool mTimer::Start( void )
{
	SetCount();
	return Restart();
}


bool mTimer::Stop( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return true;
	}

	HANDLE handle_to_kill = MyHandle;
	MyHandle = INVALID_HANDLE_VALUE;

	if( !DeleteTimerQueueTimer( g_TimerQueue , handle_to_kill , NULL ) )
	{
		if( GetLastError() != ERROR_IO_PENDING )
		{
			RaiseError( g_ErrorLogger , 0 , L"タイマーを停止できません" );
			return false;
		}
	}
	return true;
}

bool mTimer::SetCount( int newval )
{
	InterlockedExchange( &MyCount , newval );
	return true;
}

bool mTimer::SetCount( void )
{
	return SetCount( MyOption->Count );
}

int mTimer::GetCount( void )const
{
	return MyCount;
}

VOID CALLBACK mTimer::TimerRoutine( PVOID param , BOOLEAN istimer )
{
	//対象オブジェクト検索
	mTimer* parent = (mTimer*)param;

	//残り回数減算
	long Value = parent->MyCount;
	if( Value < 0 )
	{
		;	//無限繰り返しなので減算なし
	}
	else if( Value == 0 )
	{
		//0になっているので無し
		parent->Stop();
		return;
	}
	else
	{
		//有限回数の繰り返しなので回数の更新
		do
		{
			if( InterlockedCompareExchange( &parent->MyCount , Value - 1 , Value ) == Value )
			{
				Value--;
				if( Value == 0 )
				{
					//0回になったのでタイマを止める
					parent->Stop();
				}
				break;
			}
			Value = parent->MyCount;
		}while( 1 );
	}

	//キューの先頭の場合は完了イベントをコール
	switch( parent->MyOption->Method )
	{
	case NotificationOption::NotificationMethod::METHOD_NONE:
		break;
	case NotificationOption::NotificationMethod::METHOD_WINDOW_MESSAGE:
	{
		//ウインドウメッセージの場合
		// wparam : 呼出し元mASyncNamedPipeへのポインタ
		// lparam : Parameterの値
		Option_WindowMessage* op = (Option_WindowMessage*)( parent->MyOption.get() );
		PostMessageW( op->Sendto , op->OnTimer , (WPARAM)parent , op->Parameter );
		break;
	}
	case NotificationOption::NotificationMethod::METHOD_CALLBACK_FUNCTION:
	{
		//コールバック関数の場合
		Option_CallbackFunction* op = (Option_CallbackFunction*)( parent->MyOption.get() );
		if( op->OnTimer )
		{
			op->OnTimer( *parent , op->Parameter , Value );
		}
		break;
	}
	case NotificationOption::NotificationMethod::METHOD_SIGNAL_OBJECT:
	{
		//シグナルオブジェクトの場合
		Option_SignalObject* op = (Option_SignalObject*)( parent->MyOption.get() );
		if( op->OnTimer != INVALID_HANDLE_VALUE )
		{
			SetEvent( op->OnTimer );
		}
		break;
	}
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , (int)parent->MyOption->Method );
		break;
	}

	//新しいタイマーを作る
	if( parent->MyHandle != INVALID_HANDLE_VALUE && 0 != Value )
	{
		parent->Stop();
		parent->Start();
	}

}



