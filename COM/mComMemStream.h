//----------------------------------------------------------------------------
// COMへのストリーミング書き込み操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MCOMMEMSTREAM_H_INCLUDED
#define MCOMMEMSTREAM_H_INCLUDED

#include "mStandard.h"
#include "General/mFileReadStreamBase.h"
#include "General/mFileWriteStreamBase.h"
#include "General/mTCHAR.h"
#include <objidl.h>

//COMのラッパー
class mComMemStream : public mFileReadStreamBase , public mFileWriteStreamBase
{
public:

	mComMemStream( DWORD buffersize = 4096 );
	virtual ~mComMemStream();

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

	//指定の位置にポインタを移動します
	//指定するのは、ファイルの先頭からの位置になります。
	bool SetPointer( ULONGLONG pos );

	//ファイルポインタを前後に移動
	//distance : 移動距離
	//ret : 成功時true
	//指定位置がEOFを超える場合もエラーになりません。
	bool MovePointer( LONGLONG distance );

	//ファイルポインタをファイルの末尾に移動
	bool SetPointerToEnd( void );

	//ファイルポインタをファイルの先頭に移動
	bool SetPointerToBegin( void );

	//インターフェイスを取得
	IStream* Get( void )const;

	//インターフェイスを取得
	operator IStream*( void )const;

private:

	mComMemStream( const mComMemStream& source ) = delete;
	void operator=( const mComMemStream& source ) = delete;

protected:

	//一回のシステムコールで読み取るサイズ
	const DWORD MAX_BUFFER_SIZE;

	//書込みキャッシュを破棄
	void ResetWriteCache( void );

	IStream* MyStream;
};

#endif