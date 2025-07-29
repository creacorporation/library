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
#include <unordered_map>

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
	enum class TaskStartCheckResult
	{
		//このタスクをスタートする。
		Start,
		//このタスクをスタートしない。後続のタスクもスタートさせない。
		Lock,
		//このタスクをスタートしない。後続のタスクにスタートできるものがあればスタートさせる。
		Postpone,
	};

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
		//【未完了】タスクの休眠中である
		STATUS_SUSPENDED,
		//【未完了】タスクの休眠中だったが目覚めようとしている
		STATUS_WAKINGUP,
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
	// queue タスクが登録されていたキュー
	// ticket 完了したタスク
	// parameter 登録時に設定したユーザ定義のパラメータ
	// result タスクが成功したかどうか(TaskFunctionがRESULT_FINISH_SUCCEED=trueを返したか、RESULT_FINISH_FAILED=falseを返したか)
	using CallbackFunction = void(*)( class mTaskQueue& queue , Ticket& ticket , DWORD_PTR parameter , bool result );

	//タスク実行の順位指定
	enum class ScheduleType
	{
		//特に指定なし
		Normal,
		//他のタスク動作中は開始せず、実行中は他のタスクを開始させない
		Critical,
		//キュー内で同一IDのタスクが実行中であれば、タスクを開始しない（後続のタスクは実行されない）
		IdLock,
		//キュー内で同一IDのタスクが実行中であれば、後続の開始可能なタスクを実行する
		//このタスクの実行は後回しになる
		IdPostpone,
		//コールバック関数(mTaskBase::TaskStartCheck)を呼び出して決める
		Callback
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

	//完了判定用シグナルオブジェクト(内部使用)
	//※mTaskQueueが勝手に書き換えるので、内部からは触らないこと
	//・ブロッキングするタスクで使う
	HANDLE MyCompleteObject;

	//親オブジェクト
	mTaskQueue* MyParent;

	//生成時刻(タイムアウト判定用)
	const DWORD MyCreateTime;

protected:

	mTaskBase();

	// TaskId : タスクの名称
	mTaskBase( const AString& TaskId , ScheduleType ScheduleType = ScheduleType::Normal , bool Dedicate = false );

	//タスクID
	const AString MyTaskId;

	//タスク実行順制御
	const ScheduleType MyScheduleType;

	//専用スレッドにするかどうか
	const bool MyDedicated;

	//タスクIDごとの情報
	// 左：タスクID
	// 右：現在実行中のタスクのうち、そのタスクIDの数
	using TaskInformationMap = std::unordered_map< AString , uint32_t >;

	//タスクの開始チェック結果
	using TaskStartCheckResult = Definitions_TaskBase::TaskStartCheckResult;

	//タスクの開始チェック
	virtual TaskStartCheckResult TaskStartCheck( const TaskInformationMap& taskmap )const;

	//タスクの実行結果
	using TaskFunctionResult = Definitions_TaskBase::TaskFunctionResult;

	//タスクが実行されるとき呼び出します
	// ret : タスクの実行結果を示す値
	virtual TaskFunctionResult TaskFunction( const Ticket& task ) = 0;

	//タスクがキャンセルされるとき呼び出します
	virtual void CancelFunction( const Ticket& task );

	//タスクが寝ている場合に起こす
	bool Wakeup( void );
};



#endif //MWORKERTHREAD_H_INCLUDED

