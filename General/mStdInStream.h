//----------------------------------------------------------------------------
// 標準入力読み込み操作
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------


#ifndef MSTDINSTREAM_H_INCLUDED
#define MSTDINSTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileReadStreamBase.h"

/*
●用途
ストリーミング的に標準入力を読み取ります。
*/

class mStdInStream : public mFileReadStreamBase
{
public:
	mStdInStream();
	virtual ~mStdInStream();

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//※EOFの場合、現在読み取れるデータがないことを示します
	//（時間が経てば再度読み取れるかもしれない）
	//※ストリームが完全に終了しているかを知るにはIsEOFを使います
	virtual INT Read( void );

	//EOFに達しているかを調べます
	virtual bool IsEOF( void )const;

	//ファイルが開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const;

protected:

	mStdInStream( const mStdInStream& source ) = delete;
	void operator=( const mStdInStream& source ) = delete;

};

#endif

