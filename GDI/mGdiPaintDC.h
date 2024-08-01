//----------------------------------------------------------------------------
// ウインドウ管理（再描画用デバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
再描画用のデバイスコンテキストです。
このクラスでは、BeginPaint()を呼び出してWindowsからデバイスコンテキストを取得します。
適宜再描画を行い、全部終ったらインスタンスを破棄すると、デストラクタがEndPaintを呼び出します。
*/

#ifndef MGDIPAINTDC_H_INCLUDED
#define MGDIPAINTDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"

class mGdiPaintDC : public mGdiDC
{
public:

	//コンストラクタ
	//hwnd : デバイスコンテキストを取得するのに使用するウインドウのハンドル。
	//       指定したウインドウのクライアント領域のデバイスコンテキストを取得します。
	mGdiPaintDC( HWND hwnd );

	virtual ~mGdiPaintDC();

	//再描画領域を取得する
	//retArea : 再描画が必要な領域
	//ret : 成功時true
	bool GetRedrawArea( RECT& retArea )const;

private:
	mGdiPaintDC();
	mGdiPaintDC( const mGdiPaintDC& src ) = delete;
	mGdiPaintDC& operator=( const mGdiPaintDC& src ) = delete;

protected:

	//MyHdcを取得するときに使用したウインドウのハンドル
	//※コンストラクタで指定したもの
	HWND MyHwnd;	

	//再描画範囲
	PAINTSTRUCT MyPaintStruct;

};

#endif //MGDIPAINTDC_H_INCLUDED

