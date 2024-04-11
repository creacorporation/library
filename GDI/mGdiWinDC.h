//----------------------------------------------------------------------------
// ウインドウ管理（ウインドウ描画用デバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
ウインドウ描画用のデバイスコンテキストです。
このクラスでは、GetDC()を呼び出してWindowsからデバイスコンテキストを取得します。
GetDCで取得できるのはWindows内でグローバルなデバイスコンテキストなので、
描画が終わったらさっさと破棄しないといけないらしいです。
⇒よって、このクラスのインスタンスも描画が終わったら破棄してください。

よくわからんけど、
Get～で始まるAPI関数で取れたデバイスコンテキスト⇒さっさと破棄しろ
Create～始まるAPI関数で取れたデバイスコンテキスト⇒アプリ終了までグローバル変数とかに持っていてもOK
ってことらしい。
*/

#ifndef MGDIWINDC_H_INCLUDED
#define MGDIWINDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"

class mGdiWinDC : public mGdiDC
{
public:

	//コンストラクタ
	//hwnd : デバイスコンテキストを取得するのに使用するウインドウのハンドル。
	//       指定したウインドウのクライアント領域のデバイスコンテキストを取得します。
	mGdiWinDC( HWND hwnd );

	virtual ~mGdiWinDC();

private:
	mGdiWinDC();
	mGdiWinDC( const mGdiWinDC& src ) = delete;
	mGdiWinDC& operator=( const mGdiWinDC& src ) = delete;

protected:

	HWND MyHwnd;	//MyHdcを取得するときに使用したウインドウのハンドル
					//※コンストラクタで指定したもの

};

#endif //MGDIWINDC_H_INCLUDED


