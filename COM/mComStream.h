//----------------------------------------------------------------------------
// COMへのストリーミング書き込み操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MCOMSTREAM_H_INCLUDED
#define MCOMSTREAM_H_INCLUDED

#include "mStandard.h"
#include "General/mFileReadStreamBase.h"
#include "General/mFileWriteStreamBase.h"
#include "General/mTCHAR.h"
#include <objidl.h>

//COMのラッパー
class mComStream : public mFileReadStreamBase , public mFileWriteStreamBase
{
public:
	//インターフェイスを取得
	virtual IStream* Get( void )const = 0;

	//インターフェイスを取得
	virtual operator IStream*( void )const = 0;

};


#endif
