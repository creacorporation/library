//----------------------------------------------------------------------------
// ウインドウ管理（グローバルなウインドウ関数）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGLOBALWINDOWFUNC_CPP_COMPILING
#include "mGlobalWindowFunc.h"
#include "General/mErrorLogger.h"

//静的なメンバ変数の定義
mGlobalWindowFunc::HandleObjMap mGlobalWindowFunc::MyHandleObjMap;

LRESULT __stdcall mGlobalWindowFunc::MessageProcedure( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam )
{
	//hwndをキーに検索して、見つかったオブジェクトにメッセージを処理させる。
	//見つからなかった場合はデフォルト君に処理させる。

	HandleObjMap::iterator itr = MyHandleObjMap.find( hwnd );
	if( itr == MyHandleObjMap.end() )
	{
		//不明なオブジェクト
		return DefWindowProcW( hwnd , msg , wparam , lparam );
	}
	else
	{
		//該当オブジェクトあり
		return itr->second->WindowProcedure( msg , wparam , lparam );
	}
}

bool mGlobalWindowFunc::Attach( const mGlobalWindowFunc::AttachAccessPermission& , HWND hwnd , mWindow* win )
{
	//すでに登録済み？
	if( MyHandleObjMap.count( hwnd ) )
	{
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)hwnd , L"Duplicate attach" );
		return false;
	}
	//登録
	MyHandleObjMap.insert( HandleObjMap::value_type( hwnd , win ) );
	return true;
}

mWindow* mGlobalWindowFunc::Query( const mGlobalWindowFunc::AttachAccessPermission& , HWND hwnd )
{
	HandleObjMap::iterator itr = MyHandleObjMap.find( hwnd );
	if( itr == MyHandleObjMap.end() )
	{
		//不明なオブジェクト
		return nullptr;
	}
	else
	{
		//該当オブジェクトあり
		return itr->second;
	}
}

bool mGlobalWindowFunc::Detach( const mGlobalWindowFunc::DetachAccessPermission& , HWND hwnd , mWindow* win )
{
	//登録ある？
	HandleObjMap::iterator itr = MyHandleObjMap.find( hwnd );
	if( itr == MyHandleObjMap.end() )
	{
		//登録なし
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)hwnd , L"Object not found" );
		return false;
	}
	if( ( itr->first != hwnd ) || ( itr->second != win ) )
	{
		//登録されているものと、削除されるものが異なる場合。
		//この場合でも、この関数を返すとwinが指しているアドレスが解放される可能性が大なので、
		//エラー終了せずに、このまま削除してしまう。画面は崩れるかもしれないが。
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)hwnd , L"HWND/mWindow mismatch" );
	}

	//削除
	MyHandleObjMap.erase( hwnd );
	return true;
}

