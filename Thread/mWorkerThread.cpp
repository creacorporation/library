//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------


#include "mWorkerThread.h"
#include "mWorkerThreadPool.h"
#include "General/mErrorLogger.h"

mWorkerThread::mWorkerThread( mWorkerThreadPool& pool )
	: MyParent( pool )
{
}

mWorkerThread::~mWorkerThread()
{
}

unsigned int mWorkerThread::TaskFunction( void )
{
	HANDLE handle = MyParent.GetHandle();

	while( 1 )
	{
		DWORD bytes = 0;	//読み取ったバイト数
		ULONG_PTR key = 0;	//キー（完了関数を入れている）
		LPOVERLAPPED ov;

		SetLastError( 0 );
		if( !GetQueuedCompletionStatus( handle , &bytes , &key , &ov , INFINITE ) )
		{
			if( ov == nullptr )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"IO完了ポートの待ち受けが失敗しました" , (ULONG_PTR)handle );
				return 2;
			}
		}
		LPOVERLAPPED_COMPLETION_ROUTINE CompletionRoutine = (LPOVERLAPPED_COMPLETION_ROUTINE)key;

		if( CompletionRoutine )
		{
			CompletionRoutine( GetLastError() , bytes , ov );
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"IO完了キーが指定されていません" , (ULONG_PTR)handle );
		}


		DWORD signaled = WaitForSingleObject( MyTerminateSignal , 0 );
		switch( signaled )
		{
		case WAIT_OBJECT_0 + 0:
			//終了シグナル
			return 0;
		case WAIT_TIMEOUT:
			break;
		default:
			RaiseAssert( g_ErrorLogger , 0 , L"ワーカースレッドの終了シグナルが異常です" );
			return 1;
		}
	}
}
