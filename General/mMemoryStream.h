//----------------------------------------------------------------------------
// メモリへのストリーミング書き込み・読み込み操作
// Copyright (C) 2025 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MMEMORYSTREAM_H_INCLUDED
#define MMEMORYSTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileReadStreamBase.h"
#include "mFileWriteStreamBase.h"
#include "mTCHAR.h"
#include <deque>

class mMemoryStream : public mFileReadStreamBase , public mFileWriteStreamBase
{
public:

	mMemoryStream( DWORD buffersize = 4096 );
	virtual ~mMemoryStream();

	//ファイルを閉じます
	virtual bool Close( void );

	//１文字書き込み
	virtual bool Write( INT data );

	//１文字（１バイト）読み込みます
	virtual INT Read( void );

	//開いているかどうか
	virtual bool IsOpen( void )const;

	//EOFに達しているかを調べます
	virtual bool IsEOF( void )const;

	//キャッシュを書き込み
	virtual bool FlushCache( void );

private:

	mMemoryStream( const mMemoryStream& source ) = delete;
	void operator=( const mMemoryStream& source ) = delete;

	//確保するバッファのサイズ
	const DWORD MAX_BUFFER_SIZE;

	//バッファ
	using BufferArray = std::deque<std::unique_ptr<BYTE[]>>;
	BufferArray MyBufferArray;

};


#endif
