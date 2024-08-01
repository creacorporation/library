//----------------------------------------------------------------------------
// ウインドウ管理（ウインドウ描画用デバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIWINDC_CPP_COMPILING
#include "mGdiWinDC.h"
#include "General/mErrorLogger.h"

mGdiWinDC::mGdiWinDC( HWND hwnd )
{
	MyHdc = ::GetDC( hwnd );
	if( MyHdc != nullptr )
	{
		MyHwnd = hwnd;
	}
	else
	{
		MyHwnd = nullptr;
		RaiseAssert( g_ErrorLogger , 0 , L"GetDC failed" );
	}
}

mGdiWinDC::~mGdiWinDC()
{
	//選択されているGDIオブジェクトを全部元に戻す
	//※これをやらないとリソースリークするかも
	ResetSelectedObject();

	//ハンドルの解放をして終了
	::ReleaseDC( MyHwnd , MyHdc );
}


