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
	friend class mTaskBase;

public:

	mTaskQueue( mWorkerThreadPool& wtp );
	virtual ~mTaskQueue();

	//タスクの追加
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task );

	static const uint32_t ADDTASKBLOCKING_TIMEOUT = 0x20000000ul;

	//タスクの追加（ブロッキング）
	//追加したタスクが完了するまで戻らない
	//・ワーカースレッドのメンバースレッドから呼び出すことはできない
	//・パフォーマンスを下げるので、乱用しないこと
	//・タイムアウトしたとき、偽を返しGetLastErrorがADDTASKBLOCKING_TIMEOUTになります。
	//・タイムアウトした時点でタスクが開始していた場合、そのタスクは引続き実行されます。
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	// timeout : タイムアウト(ms)
	threadsafe bool AddTaskBlocking( bool high , mTaskBase::Ticket& task , uint32_t timeout = INFINITE );

	//最終タスクの追加
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	//この関数でタスクを追加すると、以降のAddTask(),Seal()は全て失敗します。
	threadsafe bool Seal( bool high , mTaskBase::Ticket& task );

	//最終タスクの追加（ブロッキング）
	//追加したタスクが完了するまで戻らない
	//この関数でタスクを追加すると、以降のAddTask(),Seal()は全て失敗します。
	//・ワーカースレッドのメンバースレッドから呼び出すことはできない
	//・パフォーマンスを下げるので、乱用しないこと
	//・タイムアウトしたとき、偽を返しGetLastErrorがADDTASKBLOCKING_TIMEOUTになります。
	//・タイムアウトした時点でタスクが開始していた場合、そのタスクは引続き実行されます。
	// high : 他のタスクに優先して処理する
	// task : 処理するタスク(mTaskBaseを継承したクラス)
	// timeout : タイムアウト(ms)
	threadsafe bool SealBlocking( bool high , mTaskBase::Ticket& task , uint32_t timeout = INFINITE );

	//タスク終了
	//この関数を呼ぶと、以降のAddTask(),Seal()は全て失敗します。
	threadsafe bool Seal( void );

	//実行待ちキューにあるタスクを全て取り消す
	//・実行中のタスクは取り消されない。
	//・サスペンドしているタスクは取り消される。
	//取り消されたタスクの数が返る
	threadsafe DWORD CancelTask( void );

	//アイドル状態であるか得る
	//・キューに積まれたタスクがない
	//・タスクを実行しているスレッドがない
	threadsafe bool IsIdle( void )const;

	//指定したタスクIDをもつタスクがいくつあるかを返す
	//・存在しているもの全てを数える
	//・前方一致です。"TEST"で検索すると、"TEST"も"TEST3"もカウントされます。
	// id : 数えたいタスクID
	// ret : 指定したタスクIDの数
	threadsafe DWORD GetTaskIdCount( const AString& id )const;

	//指定したタスクIDをもつタスクがいくつ実行中であるかを返す
	//・実行中のもののみを数える。キューにあるが未実行のものは数えない。
	//・前方一致です。"TEST"で検索すると、"TEST"も"TEST3"もカウントされます。
	// id : 数えたいタスクID
	// ret : 指定したタスクIDの数
	threadsafe DWORD GetActiveTaskIdCount( const AString& id )const;

	//ワーカースレッドプールのハンドルを返す
	threadsafe mWorkerThreadPool& GetThreadPool( void )const;

private:

	mTaskQueue();
	mTaskQueue( const mTaskQueue& src );
	const mTaskQueue& operator=( const mTaskQueue& src ) = delete;

	//ワーカースレッドプールに呼び出しを依頼
	threadsafe bool AddTask( void );

	//指定したタスクが寝ていたら起こす
	threadsafe bool Wakeup( mTaskBase& p );

protected:

	//関連付けられているワーカースレッドプール
	mWorkerThreadPool& MyWorkerThreadPool;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCriticalSection;

	//封印済み？
	bool MyIsSealed;

	//タスクIDごとの情報
	mTaskBase::TaskInformationMap MyTaskInformationMap;

	//タスクIDの参照カウントをインクリメントする
	threadsafe void TaskInformationIncrement( const AString& id );

	//タスクIDの参照カウントをデクリメントする
	threadsafe void TaskInformationDecrement( const AString& id );

	//タスクキューのエントリ
	using TicketQueue = std::deque< mTaskBase::Ticket >;
	TicketQueue MyTicketQueue;		//実行キュー

	//タスクの追加
	//このクラスのインスタンスのデストラクタが実行開始以後は、このコールは実行されず失敗する。
	threadsafe bool AddTask( bool high , mTaskBase::Ticket& task , bool isFinal );

	//タスクの追加
	threadsafe bool AddTaskBlocking( bool high , mTaskBase::Ticket& task , uint32_t timeout , bool isFinal );


	//タスクの処理ルーチン
	static bool TaskRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

};

#endif //MTASKQUEUE_H_INCLUDED

