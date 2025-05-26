//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MWORKERTHREADPOOL_H_INCLUDED
#define MWORKERTHREADPOOL_H_INCLUDED

#include "mStandard.h"

#include "General/mCriticalSectionContainer.h"
#include "mWorkerThread.h"
#include "mTimer.h"
#include <deque>
#include <list>

class mWorkerThreadPool
{
public:
	mWorkerThreadPool();
	virtual ~mWorkerThreadPool();

	//論理コアの数より1個少ない数だけスレッドを作成する(論理コアが1個だけなら1個起動する)
	static const int THREAD_LOGICAL_CORE_MINUS1 = 0;
	//論理コアの数と同じ数だけスレッドを作成する
	static const int THREAD_LOGICAL_CORE = -1;
	//物理コアの数より1個少ない数だけスレッドを作成する(物理コアが1個だけなら1個起動する)
	static const int THREAD_PHYSICAL_CORE_MINUS1 = -2;
	//物理コアの数と同じ数だけスレッドを作成する
	static const int THREAD_PHYSICAL_CORE = -3;

	//ワーカースレッドの生成
	// threads : 生成するワーカースレッドの数
	// 　正の整数ならば、指定した数のワーカースレッドを作成する。
	// 　0以下の値ならば、上記定数の条件でワーカースレッドを作成する。
	// min_threads : 手動または自動設定したスレッド数が、この数を下回る場合は、この数に補正する
	//		0以下の場合は補正は行わない
	// max_threads : 手動または自動設定したスレッド数が、この数を上回る場合は、この数に補正する
	//		0以下の場合は補正は行わない
	// ret : 成功時真
	//・min_threadsとmax_threadsをともに指定する場合、大小関係がおかしいとエラーになる
	//・threadに0以下の値を指定して、自動設定したスレッド数が0以下になった場合、min_threadが無指定だったら1に補正する
	bool Begin( int threads = THREAD_PHYSICAL_CORE_MINUS1 , int min_threads = 0 , int max_threads = 0 );

	//ワーカースレッドの終了
	//正しい順番で終了しないと、残りのタスクが処理されないため、
	//メモリ(リソース)リークになります。
	//手順1:関連付けたファイルハンドル等があれば、それらの読み書きを全て終了する
	//      ・ハンドルを破棄するところまでやること
	//      ・この手順をサボってもクラッシュはしないがデータが欠落する可能性あり
	//手順2:GetTaskCount()が0になるまで待機する
	//      ・AddTaskで追加したタスクを全て完了させること
	//      ・終了手順開始後にタスクを追加する場合は特に注意
	//手順3:End()を呼び出す
	threadsafe bool End( void );

	//IO完了ポートと、ハンドルを関連付ける
	// handle : FILE_FLAG_OVERLAPPEDフラグをつけて開かれたファイルなどのハンドル
	// callback : IO完了時に呼び出すコールバック関数
	bool Attach( HANDLE handle , LPOVERLAPPED_COMPLETION_ROUTINE callback );

	//タスク処理用コールバック関数の定義
	// pool : コールバック関数を呼び出したワーカースレッドプール
	// Param1 : AddTask時に渡したパラメータ
	// Param2 : AddTask時に渡したパラメータ
	// ret : 真：タスクは引続き実行中とされ、後ほど再度コールバックが呼び出されます
	//       偽：タスクが完了したものとされ、削除されます
	using CallbackFunction = bool(*)( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

	//タスクの追加
	// callback : スレッドプールから呼び出すコールバック関数
	// Param1 : コールバック関数に渡される値（任意の値でOK）
	// Param2 : コールバック関数に渡される値（任意の値でOK）
	// LoadbalanceKey : なるべく異なるキーを持つタスクが同時実行されるようにする
	//  ・1つのワーカースレッドプールに複数のmTaskQueueが同居するとき、特定のmTaskQueueにスレッドが偏らないようにするためのもの
	//  ・LoadbalanceKeyが 同じ タスク間は、先入れ先出しでタスクが取り出される
	//    LoadbalanceKeyが異なるタスク間は、LoadbalanceKeyの値ごとにラウンドロビンでタスクが取り出される
	//  ・mTaskQueue由来   の   タスクを追加する場合→mTaskQueueのポインタを与えるのを推奨
	//    mTaskQueue由来ではないタスクを追加する場合→適当な値でよい
	//      ※ただし、LoadbalanceKeyが同じだと先入れ先出し、異なるとラウンドロビンなので、後から追加したタスクが先に取り出される場合がある
	//         例 [A]Key=1追加 → [B]Key=1追加 → [C]Key=2追加 → [D]Key=2追加 → [E]Key=3追加
	//             A→C→E→B→Dの順にタスクが取り出される
	threadsafe bool AddTask( CallbackFunction callback , DWORD Param1, DWORD_PTR Param2 , DWORD_PTR LoadbalanceKey = 0 );

	//現在保持している未完了タスクの数を取得する
	//※Attachで関連付けたIOの完了は含まない
	threadsafe DWORD GetTaskCount( void )const;

	//スレッドの数を得る
	DWORD GetThreadCount( void )const;

	//この関数を呼び出したスレッドがスレッドプールのメンバーか判定する
	bool IsPoolMember( void )const;

	//このメソッドをワーカースレッドプールに所属するスレッドから呼び出すと、どのスレッドは現在実行中のタスク専用になる
	//・ワーカースレッドプールは、専用化されるスレッドを補うために、新たに１つスレッドを起動する
	//・専用化されたスレッドがCallbackFunctionから制御を返すと、そのスレッドは終了される
	//　このとき、CallbackFunctionで真を返してもスレッドは終了し、後で呼び出されるときは別のスレッドからとなる
	//・非常に時間がかかるタスクを実行するときに呼び出してください
	bool DedicateThread( void );

private:
	mWorkerThreadPool( const mWorkerThreadPool& src );
	const mWorkerThreadPool& operator=( const mWorkerThreadPool& src ) = delete;

private:

	//スレッド本体
	using ThreadPool = std::list<mWorkerThread>;
	ThreadPool MyThreadPool;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCriticalSection;

	//終わったスレッドをパージするためのタイマー
	mTimer MyTimer;

	//IO完了ポート
	HANDLE MyIoPort;

	//TLSのインデックス
	static DWORD MyTlsIndex;

	//タスク情報
	struct TaskInfoEntry
	{
		CallbackFunction TaskFunction;
		DWORD Param1;
		DWORD_PTR Param2;

		TaskInfoEntry()
		{
			TaskFunction = nullptr;
			Param1 = 0;
			Param2 = 0;
		}
	};
	using TaskInfo = std::deque<TaskInfoEntry>;

	//タスクキー別タスク情報
	struct TaskArrayEntry
	{
		DWORD_PTR LoadbalanceKey;
		TaskInfo Task;
	};
	using TaskArray = std::list<TaskArrayEntry>;
	TaskArray MyTaskArray;

	//タスク情報の登録
	void RegisterTaskEntry( DWORD_PTR LoadbalanceKey , TaskInfoEntry&& entry );

	//現在動作中のタスクの数
	DWORD MyActiveTaskNum = 0;

	//スレッドを一つ起動する
	bool AddWorkerThread( void );

	//スレッドをパージする予約を行う
	bool ScheduleWorkerThreadPurge( void );

	//スレッドをパージするコールバック
	static void WorkerThreadPurgeCallback( mTimer& timer , DWORD_PTR parameter , int count );

protected:
	friend class mWorkerThread;		//アクセス許可するクラス

	//IOポートのハンドルを取得する
	HANDLE GetHandle( void )const;

private:
	//完了ルーチン タスク用
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

};



#endif


