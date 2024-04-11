//----------------------------------------------------------------------------
// COMオブジェクト用ユーティリティ関数群
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MCOMOBJECT_H_INCLUDED
#define MCOMOBJECT_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <objbase.h>
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")

class mComObject
{
public:
	mComObject();
	virtual ~mComObject();

private:
	mComObject( const mComObject& src );
	const mComObject& operator=( const mComObject& src );

	//このオブジェクトでイニシャライズ(CoInitialize)を行ったか？
	//true = イニシャライズを行った
	bool MyIsInitialized;
};

#endif

