//----------------------------------------------------------------------------
// ウインドウ管理（再描画用デバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIPAINTDC_CPP_COMPILING
#include "mGdiPaintDC.h"
#include "General/mErrorLogger.h"

mGdiPaintDC::mGdiPaintDC( HWND hwnd )
{
	MyHdc = ::BeginPaint( hwnd , &MyPaintStruct );
	if( MyHdc != nullptr )
	{
		MyHwnd = hwnd;
	}
	else
	{
		MyHwnd = nullptr;
		ZeroMemory( &MyPaintStruct , sizeof( MyPaintStruct ) );
		RaiseAssert( g_ErrorLogger , 0 , L"BeginPaint failed" );
	}
}

mGdiPaintDC::~mGdiPaintDC()
{
	//選択されているGDIオブジェクトを全部元に戻す
	//※これをやらないとリソースリークするかも
	ResetSelectedObject();

	//ハンドルの解放をして終了
	::EndPaint( MyHwnd , &MyPaintStruct );
}

bool mGdiPaintDC::GetRedrawArea( RECT& retArea )const
{
	retArea = MyPaintStruct.rcPaint;
	return true;
}
