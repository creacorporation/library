//----------------------------------------------------------------------------
// 非同期操作完了通知オブジェクト
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MNOTIFYOPTION_H_INCLUDED
#define MNOTIFYOPTION_H_INCLUDED

#include "mStandard.h"

namespace Definitions_NotifyOption
{
	//通知モード
	enum NotifyMode
	{
		NOTIFY_NONE ,
		NOTIFY_WINDOWMESSAGE ,
		NOTIFY_SIGNAL ,
		NOTIFY_CALLBACK ,
		NOTIFY_CALLBACK_PARALLEL ,
	};
};


template< typename fn > class mNotifyOption
{
public:
	using NotifyMode = Definitions_NotifyOption::NotifyMode;

	//ウインドウメッセージで通知する場合のパラメータ
	struct NotifyMessage
	{
		HWND Hwnd;		//通知先のウインドウのハンドル
		UINT Message;	//通知するメッセージID
	};

	//通知情報
	struct NotifierInfo
	{
		//イベント発生時の通知方法
		//ここに指定した値に応じて、Notifierに呼び出し先等の設定をしてください
		NotifyMode Mode;

		//通知方法がウインドウメッセージまたはコールバック関数の時に使われるユーザー定義の値
		//※シグナルオブジェクトで通知する場合は使用されません
		DWORD_PTR Parameter;

		//イベント発生時の通知内容
		//Modeに指定した内容と矛盾しないようにしてください
		union Notifiers
		{
			fn CallbackFunction;				//コールバック関数で通知する場合
			NotifyMessage Message;				//ウインドウメッセージで通知する場合
			HANDLE Handle;						//シグナルオブジェクトで通知する場合
		}Notifier;

		NotifierInfo()
		{
			Mode = NotifyMode::NOTIFY_NONE;
			Parameter = 0;
			Notifier.CallbackFunction = nullptr;
			MyNotifyEventCounter = 0;
		};

	private:
		friend static bool mNotifyOption<fn>::EnterNotifyEvent( const NotifierInfo& info );
		friend static bool mNotifyOption<fn>::LeaveNotifyEvent( const NotifierInfo& info );

		mutable volatile LONG MyNotifyEventCounter;

	};

	static bool EnterNotifyEvent( const NotifierInfo& info )
	{
		LONG val = InterlockedIncrement( &info.MyNotifyEventCounter );
		if( val == 1 )
		{
			return true;
		}
		return false;
	}

	static bool LeaveNotifyEvent( const NotifierInfo& info )
	{
		LONG val = InterlockedExchange( &info.MyNotifyEventCounter , 0 );
		if( val == 1 )
		{
			return false;
		}
		return true;
	}

	static bool IsCallback( NotifyMode mode )
	{
		return ( mode == NotifyMode::NOTIFY_CALLBACK ) || ( mode == NotifyMode::NOTIFY_CALLBACK_PARALLEL );
	}
};



#endif
