//----------------------------------------------------------------------------
// ウインドウ管理（メモリデバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
汎用のデバイスコンテキストです。
このクラスでは、CreateCompatibleDC()を呼び出してWindowsからデバイスコンテキストを取得します。
主にビットマップを保持するためのクラスです。
プライベートデバイスコンテキストとなるため、このクラスをグローバル変数に保持しても問題ありません。
*/

#ifndef MGDIMEMDC_H_INCLUDED
#define MGDIMEMDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"

class mGdiMemDC : public mGdiDC
{
public:

	//コンストラクタ
	//引数がない場合は、デスクトップと互換のデバイスコンテキストが作成されます。
	mGdiMemDC();

	//コンストラクタ
	//src : ココで指定したものと互換のデバイスコンテキストが作成されます。
	mGdiMemDC( const mGdiDC& src );

	//コンストラクタ
	//src : ココで指定したものと互換のデバイスコンテキストが作成されます。
	//      nullptrを指定するとデスクトップを指定したものと見なします
	mGdiMemDC( const mGdiDC* src );

	virtual ~mGdiMemDC();

private:
	mGdiMemDC( const mGdiMemDC& src ) = delete;
	mGdiMemDC& operator=( const mGdiMemDC& src ) = delete;

protected:


};

#endif //MGDIMEMDC_H_INCLUDED

