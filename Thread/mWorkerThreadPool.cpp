//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mWorkerThreadPool.h"
#include "General/mBinaryUtil.h"
#include "General/mErrorLogger.h"
#include <vector>

mWorkerThreadPool::mWorkerThreadPool()
{
	//IO完了ポートを作成する
	MyIoPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE , 0 , 0 , 0 );
	if( MyIoPort == NULL )
	{
		MyIoPort = INVALID_HANDLE_VALUE;
		RaiseError( g_ErrorLogger , 0 , L"IO完了ポートを作成できませんでした" );
	}

	//タスクの数を初期化
	MyQueuedTaskCount = 0;

	return;
}

mWorkerThreadPool::~mWorkerThreadPool()
{
	//スレッド破棄
	for( ThreadPool::iterator itr = MyThreadPool.begin() ; itr != MyThreadPool.end() ; itr++ )
	{
		itr->Terminate();
	}

	//IO完了ポート破棄
	if( MyIoPort != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyIoPort );
		MyIoPort = INVALID_HANDLE_VALUE;
	}

	return;
}

static int GetCreateThreadCount( int request )
{
	int thread_count;

	if( 1 <= request )
	{
		thread_count = request;
	}
	else
	{
		//必要バッファサイズを得る
		DWORD bufflen= 0;
		GetLogicalProcessorInformation( NULL, &bufflen );
		if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"プロセッサの情報取得を行うバッファを確保できません" );
			return 0;
		}
		
		using ProcInfo = std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION>;
		ProcInfo proc_info;
		proc_info.resize( bufflen / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) );

		//情報取得
		if( !GetLogicalProcessorInformation( proc_info.data() , &bufflen ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"プロセッサの情報を取得できません" );
			return 0;
		}

		//物理コアを検索
		int physical_core = 0;
		int logical_core = 0;
		for( ProcInfo::const_iterator itr = proc_info.begin() ; itr != proc_info.end() ; itr++ )
		{
			//これ物理コア？
			if( itr->Relationship == RelationProcessorCore )
			{
				//ProcessorMaskの1になっているビットが、その物理コアが担当している
				//論理コアのマスクになる
				physical_core++;
				logical_core += BitCount( itr->ProcessorMask );
			}
		}

		//設定値に応じてスレッド数を決定
		switch( request )
		{
		case mWorkerThreadPool::THREAD_LOGICAL_CORE_MINUS1:
			thread_count = ( 2 <= logical_core ) ? ( logical_core - 1 ) : ( 0 );
			break;
		case mWorkerThreadPool::THREAD_LOGICAL_CORE:
			thread_count = ( 1 <= logical_core ) ? ( logical_core - 0 ) : ( 0 );
			break;
		case mWorkerThreadPool::THREAD_PHYSICAL_CORE_MINUS1:
			thread_count = ( 2 <= physical_core ) ? ( physical_core - 1 ) : ( 0 );
			break;
		case mWorkerThreadPool::THREAD_PHYSICAL_CORE:
			thread_count = ( 1 <= physical_core ) ? ( physical_core - 0 ) : ( 0 );
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"スレッド数指定フラグが不正です" , request );
			return 0;
		}

		//スレッド数チェック
		if( thread_count == 0 )
		{
			//コアが全く見つからなかった場合は1にする
			RaiseError( g_ErrorLogger , 0 , L"必要なスレッド数を取得できませんでした" );
		}
	}
	return thread_count;
}


bool mWorkerThreadPool::Begin( int threads , int min_threads , int max_threads )
{
	if( !MyThreadPool.empty() )
	{
		RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドプールを二重に作成しようとしました" );
		return false;
	}

	//パラメータチェック
	if( ( 0 <= min_threads ) &&
		( 0 <= max_threads ) &&
		( max_threads < min_threads ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"最小スレッド数と最大スレッド数の指定が不正です" );
		return false;
	}

	//作成するスレッドの数を求める
	int req_threads = GetCreateThreadCount( threads );

	//スレッド数を最大値・最小値で補正
	if( ( 0 < min_threads ) &&	( req_threads < min_threads ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"最小スレッド数の指定によりスレッド数が補正されました" );
		req_threads = min_threads;
	}
	else
	{
		//min_threadsが無指定(0以下)の場合でも、算出スレッド数が0以下であれば1に補正する
		if( req_threads <= 0 )
		{
			CreateLogEntry( g_ErrorLogger , 0 , L"自動設定したスレッド数が0であったため、1に補正します" );
			req_threads = 1;
		}
	}
	if( ( 0 < max_threads ) &&	( max_threads < req_threads ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"最大スレッド数の指定によりスレッド数が補正されました" );
		req_threads = max_threads;
	}

	//スレッド作成
	for( int i = 0 ; i < req_threads ; i++ )
	{
		MyThreadPool.emplace_back( *this );
		if( !MyThreadPool[ i ].Begin( 0 ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドの開始が失敗しました" , i );
		}
	}
	
	//スレッド起動
	for( int i = 0 ; i < req_threads ; i++ )
	{
		if( !MyThreadPool[ i ].Resume() )
		{
			RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドのレジュームが失敗しました" , i );
		}
	}
	return true;
}

static VOID CALLBACK DummyCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	return;
}

bool mWorkerThreadPool::End( void )
{
	//終了要求
	for( ThreadPool::iterator itr = MyThreadPool.begin() ; itr != MyThreadPool.end() ; itr++ )
	{
		itr->FinishRequest();
	}

	//寝ているスレッドがいるかもしれないのでダミーの完了パケットを投げる
	for( size_t i = 0 ; i < MyThreadPool.size() ; i++ )
	{
		PostQueuedCompletionStatus( MyIoPort , 0 , (ULONG_PTR)DummyCompleteRoutine , 0 );
	}

	//停止待ち
	for( ThreadPool::iterator itr = MyThreadPool.begin() ; itr != MyThreadPool.end() ; itr++ )
	{
		if( !itr->End() )
		{
			RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドの停止に失敗しました" );
			return false;
		}
	}

	//リソース廃棄
	MyThreadPool.clear();

	return true;
}

bool mWorkerThreadPool::Attach( HANDLE handle , LPOVERLAPPED_COMPLETION_ROUTINE callback )
{
	if( handle == INVALID_HANDLE_VALUE || handle == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"無効なハンドルが指定されました" );
		return false;
	}
	if( callback == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"無効な完了関数が指定されました" );
		return false;
	}

	if( !CreateIoCompletionPort( handle , MyIoPort , (ULONG_PTR)callback , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"IO完了ポートへの関連付けが失敗しました" );
		return false;
	}
	return true;
}

HANDLE mWorkerThreadPool::GetHandle( void )const
{
	return MyIoPort;
}

//完了ルーチン
VOID CALLBACK mWorkerThreadPool::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	if( ov == nullptr )
	{
		return;
	}

	//タスク実行
	TaskInfoEntry* entry = (TaskInfoEntry*)ov;
	if( entry->TaskFunction )
	{
		entry->TaskFunction( entry->Parent , entry->Param1 , entry->Param2 );
	}

	//タスクのデータを破棄
	//タスクカウントを参照するため、Parentの参照だけコピーを取っておく
	mWorkerThreadPool& Parent( entry->Parent );
	mDelete entry;
	{
		mCriticalSectionTicket critical( Parent.MyCriticalSection );	
		Parent.MyQueuedTaskCount--;
	}
}

//タスクの追加
bool mWorkerThreadPool::AddTask( CallbackFunction callback , DWORD Param1, DWORD_PTR Param2 )
{
	if( callback == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"実行するタスクが設定されていません" );
		return false;
	}

	mCriticalSectionTicket critical( MyCriticalSection );

	TaskInfoEntry* entry = mNew TaskInfoEntry( *this );
	entry->Param1 = Param1;
	entry->Param2 = Param2;
	entry->TaskFunction = callback;

	if( !PostQueuedCompletionStatus( MyIoPort , 0 , (ULONG_PTR)CompleteRoutine , (LPOVERLAPPED)entry ) )
	{
		mDelete entry;
		RaiseError( g_ErrorLogger , 0 , L"タスクを追加できません" );
		return false;
	}
	MyQueuedTaskCount++;
	return true;
}

//現在保持している未完了タスクの数を取得する
DWORD mWorkerThreadPool::GetTaskCount( void )const
{
	return MyQueuedTaskCount;
}

//スレッドの数を得る
DWORD mWorkerThreadPool::GetThreadCount( void )const
{
	return (DWORD)MyThreadPool.size();
}
