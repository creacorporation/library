//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
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

	//タスクの追加
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task );

	//タスクの追加（ブロッキング）
	//追加したタスクが完了するまで戻らない
	//ワーカースレッドのメンバースレッドから呼び出すことはできない
	//※パフォーマンスを下げるので、乱用しないこと
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	threadsafe bool AddTaskBlocking( bool high , mTaskBase::Ticket& task );

	//最終タスクの追加
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	//この関数でタスクを追加すると、以降のAddTask(),Seal()は全て失敗します。
	threadsafe bool Seal( bool high , mTaskBase::Ticket& task );

	//タスク終了
	//この関数を呼ぶと、以降のAddTask(),Seal()は全て失敗します。
	threadsafe bool Seal( void );

	//実行待ちキューにあるタスクを全て取り消す
	//すでに実行が開始されたタスクは、取り消されない。
	//取り消されたタスクの数が返る
	threadsafe DWORD CancelTask( void );

	//アイドル状態であるか得る
	//・キューに積まれたタスクがない
	//・タスクを実行しているスレッドがない
	threadsafe bool IsIdle( void )const;

	//指定したタスクIDをもつタスクがいくつあるかを返す
	// id : 数えたいタスクID
	// ret : 指定したタスクIDの数
	threadsafe DWORD GetTaskIdCount( const AString& id )const;

	//ワーカースレッドプールのハンドルを返す
	threadsafe mWorkerThreadPool& GetThreadPool( void )const;

private:

	mTaskQueue();
	mTaskQueue( const mTaskQueue& src );
	const mTaskQueue& operator=( const mTaskQueue& src ) = delete;

protected:

	//関連付けられているワーカースレッドプール
	mWorkerThreadPool& MyWorkerThreadPool;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCriticalSection;

	//封印済み？
	bool MyIsSealed;

	//タスクIDごとの情報
	struct TaskInformation
	{
		DWORD Count;		//現在このタスクIDに属しているタスクの数(参照カウント)
		DWORD Executing;	//現在このタスクIDで実行中のタスクの数
		TaskInformation()
		{
			Count = 0;
			Executing = 0;
		}
	};
	using TaskInformationMap = std::unordered_map< AString , TaskInformation >;
	TaskInformationMap MyTaskInformationMap;

	//タスクIDの参照カウントをインクリメントする
	threadsafe void TaskInformationIncrement( const AString& id );

	//タスクIDの参照カウントをデクリメントする
	threadsafe void TaskInformationDecrement( const AString& id );

	//クリティカル指定のタスクを実行しているかどうか
	bool MyIsCritical;

	//タスクキューのエントリ
	using TicketQueue = std::deque< mTaskBase::Ticket >;
	TicketQueue MyWaiting;		//実行待ちキュー

	//実行中のタスクの数
	DWORD MyActiveTask;

	//タスクの追加
	//このクラスのインスタンスのデストラクタが実行開始以後は、このコールは実行されず失敗する。
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task , bool isFinal );

	//タスクの処理ルーチン
	static bool TaskRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

};

#endif //MTASKQUEUE_H_INCLUDED

