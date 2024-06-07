//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MTASKBASE_H_INCLUDED
#define MTASKBASE_H_INCLUDED

#include "mStandard.h"
#include "General/mNotifyOption.h"
#include "General/mTCHAR.h"
#include <memory>

//スレッドプールで処理させるタスクの基底クラス

//HOW TO USE
//1.このクラスを継承します
//  TaskFunction()に、スレッドプールで処理したい処理を実装します
//  CancelFunction()に、一度はスレッドプールに登録した後、キャンセルされた場合の処理を実装します
//　その他メンバ変数として、スレッドプールで処理するデータ等を追加します
//2.そのインスタンスをmTaskQueue::AddTask()またはmTaskQueue::Seal()に渡してスレッドプールのキューに登録します
//3.タスクの処理順が来ると、TaskFunction()が呼び出されます
namespace Definitions_TaskBase
{
	enum TaskFunctionResult
	{
		//タスクの実行は完了し、結果は成功であった
		RESULT_FINISH_SUCCEED,
		//タスクの実行は完了し、結果は失敗であった
		RESULT_FINISH_FAILED,
		//タスクの実行中である
		RESULT_INPROGRESS,
	};

	enum TaskStatus
	{
		//【未完了】タスクは実行されていない
		STATUS_NOTSTARTED ,
		//【完了】タスクの実行は完了し、結果は成功であった
		STATUS_FINISH_SUCCEED ,
		//【完了】タスクの実行は完了し、結果は失敗であった
		STATUS_FINISH_FAILED ,
		//【未完了】タスクは実行待ちキューの中に入っている
		STATUS_QUEUED ,
		//【未完了】タスクの実行中である
		STATUS_INPROGRESS ,
		//【完了】タスクはキャンセルされた
		STATUS_CANCELED,
		//【完了】タスクは内部エラーにより実行できない状態になっている
		STATUS_ABORTED ,

	};
};

class mTaskBase
{
	friend class mTaskQueue;

public:
	virtual ~mTaskBase();

	using Ticket = std::shared_ptr< mTaskBase >;

	//タスク完了時に呼び出すコールバック関数の宣言
	using CallbackFunction = void(*)( class mTaskQueue& queue , Ticket& ticket , DWORD_PTR parameter , bool result );

	//タスク実行の順位指定
	enum ScheduleType
	{
		//特に指定なし
		Normal,
		//他のタスク動作中は開始せず、動作中は他のタスクを開始させない
		Critical,
		//キュー内で同一IDのタスクが実行中であれば、タスクを開始しない
		IdLock,
		//キュー内で同一IDのタスクが実行中であれば、後続の開始可能なタスクを実行する
		IdPostpone
	};

	//タスク完了時の通知オプション
	class NotifyOption : public mNotifyOption< CallbackFunction >
	{
	public:
		//タスクが完了した場合
		NotifierInfo OnComplete;
		//タスクが中断した場合
		NotifierInfo OnSuspend;
		//タスクがキャンセルされた場合
		NotifierInfo OnCancel;
		//タスクが(システム的な理由で)続行不能になった場合
		NotifierInfo OnAbort;
	};

	//タスク完了時の通知オプション
	NotifyOption Notifier;

	//現在のタスクの状態
	using TaskStatus = Definitions_TaskBase::TaskStatus;

	//現在のタスクの状況を得る
	TaskStatus GetTaskStatus( void )const;

	//タスクIDを得る
	inline const AString& GetTaskId( void )const noexcept
	{
		return MyTaskId;
	}

private:

	mTaskBase( const mTaskBase& src );
	const mTaskBase& operator=( const mTaskBase& src );

	//現在のタスクの状態
	//※mTaskQueueが勝手に書き換えるので、内部からは触らないこと
	TaskStatus MyTaskStatus;

protected:

	mTaskBase();

	// TaskId : タスクの名称
	mTaskBase( const AString& TaskId , ScheduleType ScheduleType = ScheduleType::Normal );

	//タスクID
	const AString MyTaskId;

	//タスク実行順制御
	const ScheduleType MyScheduleType;

	//タスクの実行結果
	using TaskFunctionResult = Definitions_TaskBase::TaskFunctionResult;

	//タスクが実行されるとき呼び出します
	// ret : タスクの実行結果を示す値
	virtual TaskFunctionResult TaskFunction( const Ticket& task ) = 0;

	//タスクがキャンセルされるとき呼び出します
	virtual void CancelFunction( const Ticket& task );

};



#endif //MWORKERTHREAD_H_INCLUDED

