//----------------------------------------------------------------------------
// ウインドウ管理（グローバルなウインドウ関数）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
用途：
グローバルなメッセージプロシージャ。

HWNDとmWindowの組を登録
　↓
全てのウインドウメッセージは一旦このクラスに届く
　↓
適切なインスタンスのmWindow::WindowProcedureが呼び出される
　↓
mWindow::WindowProcedureでメッセージを処理

参考：
CreateMessageExで呼び出される順番
●成功してるとき
(1)WM_GETMINMAXINFO
(2)WM_NCCREATE
(3)WM_NCCALCSIZE
(4)WM_CREATE
●失敗しているとき
(1)WM_GETMINMAXINFO
(2)WM_NCCREATE
(3)WM_NCDESTROY
*/

#ifndef MGLOBALWINDOWFUNC_H_INCLUDED
#define MGLOBALWINDOWFUNC_H_INCLUDED

#include "mStandard.h"
#include "mWindow.h"
#include <unordered_map>

class mGlobalWindowFunc
{
public:

	//メッセージプロシージャ
	//ウインドウメッセージを受け取り、hwndの値を基に受け取るべくオブジェクトを調べ
	//そのオブジェクトのコールバックを呼び出します。
	//該当のオブジェクトが存在しない場合は、DefWindowProcW()に処理させます。
	//hwnd : 宛先ウインドウハンドル
	//msg : ウインドウメッセージ
	//wparam : WPARAMの値（メッセージにより意味は異なる）
	//lparam : LPARAMの値（メッセージにより意味は異なる）
	//※キホン、Windowsからコールバックするための関数です。
	//　ユーザアプリから直接呼び出すとSendMessage()を実行したみたいな結果になるはず。
	static LRESULT __stdcall MessageProcedure( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam );

public:
	//------------------------------------
	//　以下内部処理用
	//------------------------------------

	//アクセス権設定用オブジェクト
	//このクラスのコンストラクタを呼び出せる者にのみアクセス許可を与える。
	class AttachAccessPermission
	{
	private:
		friend class mWindowCollection;		//アクセス許可するクラス
		AttachAccessPermission()
		{
			return;
		}
	};

	//ウインドウハンドル(hwnd)と、そのハンドル宛てのウインドウメッセージを受け取る
	//オブジェクト(win)の組を登録します。この関数は、mWindowCollectionからのみアクセスを許可します。
	//hwnd : 登録するウインドウハンドル
	//win : hwndに関連づけるオブジェクト
	//ret : 正常に登録できた場合true
	static bool Attach( const AttachAccessPermission& perm , HWND hwnd , mWindow* win );

	//指定するウインドウハンドルに関連づけられているオブジェクトを取得します。
	//この関数は、mWindowCollectionからのみアクセスを許可します。
	//hwnd : 検索するウインドウハンドル
	//ret : 関連づけられているオブジェクト。存在しない場合はnullptr
	static mWindow* Query( const AttachAccessPermission& perm , HWND hwnd );

	//アクセス権設定用オブジェクト
	//このクラスのコンストラクタを呼び出せる者にのみアクセス許可を与える。
	class DetachAccessPermission
	{
	private:
		friend class mWindow;	//アクセス許可するクラス
		DetachAccessPermission()
		{
			return;
		}
	};

	//指定されたウインドウハンドルとオブジェクトの組を登録解除する。
	//この関数は、mWindowからのみアクセスを許可します。
	//hwnd : 登録解除するウインドウハンドル
	//win : hwndに関連づけられているオブジェクト
	//ret : 指定された組が存在し、登録解除できた場合はtrue。
	static bool Detach( const DetachAccessPermission& perm , HWND hwnd , mWindow* win );

private:

	//HWND-mWindowのマップ
	typedef std::unordered_map<HWND,mWindow*> HandleObjMap;
	static HandleObjMap MyHandleObjMap;

};



#endif	//MGLOBALWINDOWFUNC_H_INCLUDED

