//----------------------------------------------------------------------------
// ウインドウ管理（1つのオブジェクト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MWINDOW_CPP_COMPILING
#include "mWindow.h"
#include "mWindowCollection.h"
#include "mGlobalWindowFunc.h"
#include "General/mErrorLogger.h"


mWindow::mWindow()
{
	MyChild = nullptr;
	MyMenu = nullptr;
	MyWindowClass = L"";
	MyIsWindowClassOriginal = false;
	MyHwnd = 0;
	MyParent = 0;
	MyRoot = 0;
	MyDefWndproc = nullptr;
}

mWindow::~mWindow()
{
	//子ウインドウを破棄する
	mDelete MyChild;
	MyChild = nullptr;
	mDelete MyMenu;
	MyMenu = nullptr;

	//グローバルなウインドウファンクションから自分を登録解除する
	if( !mGlobalWindowFunc::Detach( mGlobalWindowFunc::DetachAccessPermission() , GetMyHwnd() , this ) )
	{
		//削除できない。何でだ。
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)GetMyHwnd() , MyWindowClass );
	}

	//自分で登録したウインドウクラスである場合はウインドウクラスの削除を試みる
	if( MyIsWindowClassOriginal )
	{
		//同じウインドウクラスを使っているウインドウが他にもあるかもしれないので
		//エラーになっても不問にする。
		UnregisterClassW( MyWindowClass.c_str() , GetModuleHandleW( 0 ) );
	}
}

LRESULT mWindow::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( MyDefWndproc != nullptr )
	{
		//ウインドウプロシージャが置き換えられている場合は、置き換え前のに処理させる
		return CallWindowProcW( MyDefWndproc , GetMyHwnd() , msg , wparam , lparam );
	}
	else
	{
		//ウインドウプロシージャが置き換えられていない場合は、デフォルト君に任せる。
		return DefWindowProcW( GetMyHwnd() , msg , wparam , lparam );
	}
}

bool mWindow::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;
}

//ウインドウ生成を完了した場合にコールされる
bool mWindow::OnCreate( const void* opt )
{
	return true;
}

//ウインドウが破棄されようとしているときコールされる
void mWindow::OnDestroy( void )
{
	return;
}

//自分自身のウインドウハンドルを取得する
HWND mWindow::GetMyHwnd( void )const
{
	return MyHwnd;
}

//親ウインドウのハンドルを取得する
HWND mWindow::GetMyParent( void )const
{
	return MyParent;
}

//最上位ウインドウのハンドルを取得する
HWND mWindow::GetMyRoot( void )const
{
	return MyRoot;
}

//ウインドウの位置をセット
bool mWindow::SetWindowPosition( const WindowPosition& newpos )
{
	MyPosition = newpos;
	return true;
}

void mWindow::MoveWindowPosition( const RECT& pos )
{
	::SetWindowPos( GetMyHwnd() , nullptr , pos.left , pos.top , pos.right-pos.left , pos.bottom-pos.top , 0 );
}

//ウインドウの可視/不可視を変更する
bool mWindow::SetVisible( bool newstate )
{
	int cmd = ( newstate ) ? ( SW_SHOW ) : ( SW_HIDE );
	::ShowWindow( GetMyHwnd() , cmd );
	return true;
}

//ウインドウの有効/無効を変更する
bool mWindow::SetEnable( bool newstate )
{
	::EnableWindow( GetMyHwnd() , newstate );
	return true;
}

//ウインドウの位置をスクリーン座標で得る
RECT mWindow::GetRect( void ) const
{
	RECT rect;
	if( !::GetWindowRect( GetMyHwnd() , &rect ) )
	{
		rect.left = 0;
		rect.right = 0;
		rect.top = 0;
		rect.bottom = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetRect failed" );
	}
	return rect;
}

//ウインドウのサイズを得る
SIZE mWindow::GetSize( void ) const
{
	RECT rect;
	SIZE size;
	if( !::GetWindowRect( GetMyHwnd() , &rect ) )
	{
		size.cx = 0;
		size.cy = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetSize failed" );
	}
	else
	{
		size.cx = rect.right - rect.left;
		size.cy = rect.bottom - rect.top;
	}
	return size;
}

//このウインドウのクライアント座標をスクリーン座標に変換する
POINT mWindow::Client2Screen( const POINT& client_pos ) const
{
	POINT point = client_pos;
	if( !::ClientToScreen( GetMyHwnd() , &point ) )
	{
		point.x = 0;
		point.y = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetRect failed" );
	}
	return point;
}

//このウインドウのクライアント座標をスクリーン座標に変換する
POINT mWindow::Client2Screen( INT x , INT y ) const
{
	POINT point;
	point.x = x;
	point.y = y;
	return Client2Screen( point );
}

//スクリーン座標をこのウインドウのクライアント座標に変換する
POINT mWindow::Screen2Client( const POINT& client_pos ) const
{
	POINT point = client_pos;
	if( !::ScreenToClient( GetMyHwnd() , &point ) )
	{
		point.x = 0;
		point.y = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetRect failed" );
	}
	return point;
}

//スクリーン座標をこのウインドウのクライアント座標に変換する
POINT mWindow::Screen2Client( INT x , INT y ) const
{
	POINT point;
	point.x = x;
	point.y = y;
	return Screen2Client( point );
}

bool mWindow::SetFocus( void )const
{
	return ::SetFocus( GetMyHwnd() );
}

LRESULT mWindow::MessageSend( UINT msg , WPARAM wparam , LPARAM lparam )const
{
	return SendMessageW( GetMyHwnd() , msg , wparam , lparam );		 
}


LRESULT mWindow::MessagePost( UINT msg , WPARAM wparam , LPARAM lparam )const
{
	return PostMessageW( GetMyHwnd() , msg , wparam , lparam );		 
}
