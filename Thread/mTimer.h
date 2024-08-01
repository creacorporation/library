//----------------------------------------------------------------------------
// タイマーコントロール
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MTIMER_H_INCLUDED
#define MTIMER_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"

class mTimer
{
public:
	mTimer();
	virtual ~mTimer();


	//オプション構造体
	struct NotificationOption
	{
	public:
		//通知方法
		enum NotificationMethod
		{
			METHOD_NONE,				//何もしない
			METHOD_WINDOW_MESSAGE,		//ウインドウメッセージ
			METHOD_CALLBACK_FUNCTION,	//コールバック関数
			METHOD_SIGNAL_OBJECT,		//シグナルオブジェクト
		};
		const NotificationMethod Method;

		DWORD Interval;			//動作間隔(ms単位)
		int Count;				//残り回数(負の数の場合無限)
		bool StartImmediate;	//真ならすぐ開始する

	protected:
		NotificationOption() = delete;
		NotificationOption( NotificationMethod method ) : Method( method )
		{
			Interval = 1000;
			Count = -1;
			StartImmediate = true;
		}
	};

	//何も通知しない場合のオプション
	struct Option_None : public NotificationOption
	{
		Option_None() : NotificationOption( NotificationMethod::METHOD_NONE )
		{
		}
	};
	//ウインドウメッセージを使用して通知する場合のオプション
	struct Option_WindowMessage : public NotificationOption
	{
		// wparam : 呼出し元mTimerへのポインタ
		// lparam : Parameterの値

		//メッセージを送るウインドウ
		HWND Sendto;
		//接続完了時に投げるウインドウメッセージ番号
		UINT OnTimer;
		//LPARAMに渡す値(任意)
		LPARAM Parameter;

		Option_WindowMessage() : NotificationOption( NotificationMethod::METHOD_WINDOW_MESSAGE )
		{
			Sendto = 0;
			OnTimer = WM_TIMER;
			Parameter = 0;
		}
	};

	//データ受信時にコールバック関数を呼び出して通知する場合のオプション
	struct Option_CallbackFunction : public NotificationOption
	{
		using CallbackFunction = void(*)( mTimer& timer , DWORD_PTR parameter , int count );

		CallbackFunction OnTimer;
		DWORD_PTR Parameter;

		Option_CallbackFunction() : NotificationOption( NotificationMethod::METHOD_CALLBACK_FUNCTION )
		{
			OnTimer = nullptr;
			Parameter = 0;
		}
	};

	//データ受信時にシグナルオブジェクトを使用して通知する場合のオプション
	struct Option_SignalObject : public NotificationOption
	{
		HANDLE OnTimer;		//タイマ発動時にシグナル状態にするオブジェクト

		Option_SignalObject() : NotificationOption( NotificationMethod::METHOD_SIGNAL_OBJECT )
		{
			OnTimer = INVALID_HANDLE_VALUE;
		}
	};

	//タイマーのセットアップ
	// opt : 設定値
	bool Setup( const NotificationOption& opt );

	//タイマーを再開始する
	//・残り回数をリセットしません
	//・残り回数がゼロだと開始しません
	bool Restart( void );

	//タイマーをリセットして開始する
	//・残り回数をセットアップしたときの値にリセットしてから開始します
	bool Start( void );

	//タイマーを停止する
	bool Stop( void );

	//タイマー呼び出しを行う残り回数をセットする
	// newval : 残り回数(負の数＝無限）
	//・現在の回数に対して設定する
	//・セットアップしたときの設定を上書きするものではない
	bool SetCount( int newval );

	//タイマー呼び出しをセットアップしたときの値にリセットする
	bool SetCount( void );

	//現在の残り回数を取得する
	int GetCount( void )const;

private:

	mTimer( const mTimer& src ) = delete;
	mTimer& operator=( const mTimer& src ) = delete;

	//設定値
	std::unique_ptr< NotificationOption > MyOption;

	//完了ルーチン
	static VOID CALLBACK TimerRoutine( PVOID param , BOOLEAN istimer );

	//残り呼び出し回数
	volatile long MyCount;

	//タイマーキュー
	HANDLE MyHandle;


};

#endif
