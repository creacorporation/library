//----------------------------------------------------------------------------
// ウインドウ管理（子ウインドウ管理）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MWINDOWCOLLECTION_CPP_COMPILING
#include "mWindowCollection.h"
#include "mGlobalWindowFunc.h"
#include "General/mErrorLogger.h"

mWindowCollection::mWindowCollection( mWindow* parent )
	: MyParent( parent )
{
}

mWindowCollection::~mWindowCollection()
{
	//子オブジェクトを全部破棄する
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		itr->second->OnDestroy();
		mDelete itr->second;
	}
}

mWindow* mWindowCollection::AddControlInternal( mWindowFactory factory , const WString& id , const void* opt )
{
	//使用不能なIDでないかを確認
	//すでに存在するIDではないか、IDが空文字列ではないかを確認する
	if( id == L"" || MyIdMap.count( id ) )
	{
		//使用不能なIDだった
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"IDが使用不可能です" + id );
		return nullptr;
	}

	//インスタンスのハンドルを取得
	HINSTANCE instance = GetModuleHandleW( 0 );

	//ウインドウのインスタンスを作成する
	mWindow* win = factory( opt );
	if( win == nullptr )
	{
		//ファクトリメソッドが失敗した
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"ファクトリメソッドが失敗しました" );
		return nullptr;
	}

	//ウインドウクラスの登録
	mWindow::WindowClassSetting wndclass;
	bool need_wndcls_register = win->WindowClassSettingCallback( wndclass , opt );
	if( need_wndcls_register )
	{
		//登録をする場合
		WNDCLASSEXW wc;
		wc.cbSize = sizeof( WNDCLASSEXW );				//構造体のサイズ
		wc.style = wndclass.Style;						//スタイル。
		wc.lpfnWndProc = mGlobalWindowFunc::MessageProcedure;	//ウインドウプロシージャ
		wc.cbClsExtra = 0;								//普通0
		wc.cbWndExtra = 0;								//普通0
		wc.hInstance = instance;						//インスタンスハンドル
		wc.hIcon = wndclass.Icon;						//ウインドウのアイコン。後で変えれる。
		wc.hCursor = wndclass.Cursor;					//カーソル。後で変えれる。
		wc.hbrBackground = wndclass.Background;			//バックグラウンドのブラシ。後で変えれる。
		wc.lpszMenuName = nullptr;						//メニューの名前。後で(ry
		wc.lpszClassName = wndclass.ClassName.c_str();	//クラス名
		wc.hIconSm = wndclass.IconSmall;				//小さい時のアイコン。後で(ry

		ATOM atom = ::RegisterClassExW( &wc );
		if( atom == 0 )
		{
			//登録失敗したが、その理由は？
			DWORD err_code = GetLastError();
			//すでに登録済みでした→問題なし
			//その他なんかのエラーでした→エラー
			if( err_code != ERROR_CLASS_ALREADY_EXISTS )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"ウインドウクラスの登録ができません" );
				mDelete win;
				return nullptr;
			}
		}
	}

	//ウインドウクラスの登録ができたから、次はウインドウの生成を行う
	mWindow::CreateWindowSetting wndsetting;
	wndsetting.ClassName = wndclass.ClassName;
	bool window_create = win->CreateWindowCallback( wndsetting , opt );

	//親オブジェクトが存在するかどうかでWS_CHILDをセットしたり解除したりする
	if( MyParent == nullptr )
	{
		wndsetting.Style &= ~WS_CHILD;	//親ウインドウはない
	}
	else
	{
		wndsetting.Style |= WS_CHILD;	//親ウインドウあり
	}

	if( window_create )
	{
		//ウインドウ生成
		HWND hwnd = ::CreateWindowExW(
			wndsetting.ExStyle,				//拡張スタイル
			wndsetting.ClassName.c_str(),	//登録されているクラス名
			wndsetting.WindowName.c_str(),	//キャプション
			wndsetting.Style ,				//スタイル
			wndsetting.x ,					//X座標
			wndsetting.y ,					//Y座標
			wndsetting.Width ,				//幅
			wndsetting.Height ,				//高さ
			( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyHwnd ),		//親ウィンドウ
			nullptr ,						//メニューハンドル
			instance ,						//インスタンスのハンドル
			0 );							//ウィンドウ作成データ(MDI) or 任意のデータ

		//できあがったウインドウのプライベートメンバを書いてやる
		win->MyIsWindowClassOriginal = need_wndcls_register;
		win->MyWindowClass = wndsetting.ClassName;
		win->MyParent = ( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyHwnd );
		win->MyRoot = ( MyParent == nullptr ) ? ( hwnd ) : ( MyParent->MyRoot );
		win->MyHwnd = hwnd;
		win->MyDefWndproc = nullptr;

		//ところで、CreateWindow成功してたんだっけ？
		if( hwnd == nullptr )
		{
			//失敗しているので破棄する
			RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"ウインドウを開けません" + wndsetting.WindowName );
			mDelete win;
			return nullptr;
		}

		//サブクラス化する？
		if( wndsetting.ProcedureChange )
		{
			//サブクラス化する。
			if( need_wndcls_register )
			{
				//ウインドウクラスを自作してる場合、最初からそうなってるんで
				//別にサブクラス化しなくても、ちゃんとメッセージ受信できるよ！
				;
			}
			else
			{
				SetLastError( 0 );
				win->MyDefWndproc = (WNDPROC)::SetWindowLongPtrW( hwnd , GWLP_WNDPROC , (LONG_PTR)mGlobalWindowFunc::MessageProcedure );
				if( win->MyDefWndproc == nullptr && GetLastError() != 0 )
				{
					//置き換えに失敗
					RaiseAssert( g_ErrorLogger , 0 , L"サブクラス化が失敗しました" );
					mDelete win;
					return nullptr;
				}
			}
		}

		//マップに登録(1)　～グローバルウインドウ関数～
		if( !mGlobalWindowFunc::Attach( mGlobalWindowFunc::AttachAccessPermission() , hwnd , win ) )
		{
			//登録できない
			RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"グローバルウインドウ関数に登録できませんでした" + wndsetting.WindowName );
			mDelete win;
			return nullptr;
		}

		//マップに登録(2)　～IDマップ～
		MyIdMap.insert( IdMap::value_type( id , win ) );
		MyHwndMap.insert( HwndMap::value_type( hwnd , id ) );
	}
	else
	{
		win->MyIsWindowClassOriginal = need_wndcls_register;
		win->MyWindowClass = wndsetting.ClassName;
		win->MyParent = ( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyHwnd );
		win->MyRoot = ( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyRoot );
		win->MyHwnd = 0;
		win->MyDefWndproc = nullptr;
		MyIdMap.insert( IdMap::value_type( id , win ) );
	}

	//作成完了なのでコールバックを呼び出す
	if( !win->OnCreate( opt ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"OnCreateが失敗しました" );
		mDelete win;
		return nullptr;
	}

	return win;
}

bool mWindowCollection::RemoveControl( const WString& id )
{
	//削除対象を検索
	IdMap::iterator itr = MyIdMap.find( id );
	if( itr == MyIdMap.end() )
	{
		//無いんですけど？
		RaiseError( g_ErrorLogger , 0 , L"ID not found : " + id );
		return false;
	}

	HWND del_hwnd = itr->second->GetMyHwnd();

	//削除処理
	itr->second->OnDestroy();
	mDelete itr->second;
	MyIdMap.erase( id );

	if( MyHwndMap.count( del_hwnd ) == 0 )
	{
		//無いんですけど？
		RaiseError( g_ErrorLogger , 0 , L"Associated hwnd is not found : " + id );
		return false;
	}
	MyHwndMap.erase( del_hwnd );

	return true;
}

WString mWindowCollection::QueryId( HWND hwnd )const
{
	HwndMap::const_iterator itr = MyHwndMap.find( hwnd );
	if( itr == MyHwndMap.end() )
	{
		return L"";
	}
	return itr->second;
}

mWindow* mWindowCollection::Query( const WString& id )const
{
	IdMap::const_iterator itr = MyIdMap.find( id );
	if( itr == MyIdMap.end() )
	{
		return nullptr;
	}
	return itr->second;
}

bool mWindowCollection::AdjustSize( const RECT& world )
{
	//子オブジェクト全部にサイズ変更させる
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		//変更後の位置を求める
		RECT abspos;
		CalcAbsolutePosition( itr->second->MyPosition , abspos , world );

		//位置変更
		itr->second->MoveWindowPosition( abspos );
	}
	return true;
}

bool mWindowCollection::AdjustSize( HWND hwnd )
{
	RECT rect;
	if( !::GetClientRect( hwnd , &rect ) )
	{
		return false;
	}
	return AdjustSize( rect );
}


bool mWindowCollection::CalcAbsolutePosition( const mWindow::WindowPosition& srcpos , RECT& retPos , const RECT& world )
{
	INT width = world.right - world.left;	//幅
	INT height = world.bottom - world.top;	//高さ

	retPos.left = (LONG)( srcpos.left.rate * width ) + srcpos.left.offset;			//左端の座標
	retPos.right = (LONG)( srcpos.right.rate * width ) + srcpos.right.offset;		//右端の座標
	retPos.top = (LONG)( srcpos.top.rate * height ) + srcpos.top.offset;			//上端の座標
	retPos.bottom = (LONG)( srcpos.bottom.rate * height ) + srcpos.bottom.offset;	//下端の座標
	return true;
}

bool mWindowCollection::ReflectMessage( UINT msg , WPARAM wparam , LPARAM lparam )
{
	//子オブジェクト全部にメッセージを送信する
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		itr->second->WindowProcedure( msg , wparam , lparam );
	}
	return true;
}


