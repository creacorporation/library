//----------------------------------------------------------------------------
// ウインドウ管理（GDIリソース）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDIリソースのラッパーです
*/


#ifndef MGDIREHANDLE_H_INCLUDED
#define MGDIREHANDLE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mGdiHandle
{
public:
	/*
	※ファクトリメソッド作成例
	　以下コードを継承したクラスにコピペし、「mNew mGdiHandle」の部分を継承したクラス名で置き換えて下さい。
	static mGdiHandle* Factory( const Option* opt )throw( )
	{
		return mNew mGdiHandle;
	}
	*/

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const = 0;

private:
	mGdiHandle( const mGdiHandle& src ) = delete;
	mGdiHandle& operator=( const mGdiHandle& src ) = delete;

	friend class mGdiResource;

protected:
	mGdiHandle();
	virtual ~mGdiHandle();

};

#endif	//MGDIREHANDLE_H_INCLUDED

