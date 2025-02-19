//----------------------------------------------------------------------------
// メモリへのストリーミング書き込み・読み込み操作
// Copyright (C) 2025 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MMEMORYSTREAM_CPP_COMPILING
#include "mMemoryStream.h"
#include "mErrorLogger.h"

mMemoryStream::mMemoryStream( DWORD buffersize )
	: MAX_BUFFER_SIZE( buffersize )
{
	MyWriteCacheRemain = 0;
	MyWriteCacheWritten = 0;
	MyIsEOF = true;
}

mMemoryStream::~mMemoryStream()
{
}

bool mMemoryStream::Close( void )
{
	return false;
}

bool mMemoryStream::Write( INT data )
{

	//キャッシュの残りがある？
	if( MyWriteCacheRemain == 0 )
	{
		if( MyWriteCacheHead )
		{
			MyBufferArray.push_back( std::move( MyWriteCacheHead ) );
		}
		MyWriteCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
		MyWriteCacheRemain = MAX_BUFFER_SIZE;
		MyWriteCacheWritten = 0;
		MyIsEOF = false;
	}

	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;
	return true;
}

bool mMemoryStream::IsOpen( void )const
{
	return true;
}

INT mMemoryStream::Read( void )
{
	//UnReadされた文字がある場合はソレを返す
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	//キャッシュの残量があればキャッシュを読み込む
	//キャッシュの残量がないならキューから取得する
	if( MyReadCacheRemain == 0 )
	{
		//読み取りキャッシュにセット
		if( !MyBufferArray.empty() )
		{
			//空じゃない場合はそこの先頭を取り出す
			MyReadCacheHead = std::move( MyBufferArray.front() );
			MyBufferArray.pop_front();

			MyReadCacheCurrent = 0;
			MyReadCacheRemain = MAX_BUFFER_SIZE;
		}
		else if( MyWriteCacheWritten )
		{
			//空の場合で、書込みキャッシュがある場合はそれを取る
			MyReadCacheHead = std::move( MyWriteCacheHead );
			MyWriteCacheHead.reset();

			MyReadCacheCurrent = 0;
			MyReadCacheRemain = MyWriteCacheRemain;

			MyWriteCacheWritten = 0;
			MyWriteCacheRemain = 0;
		}
		else
		{
			//どちらもない場合はEOF
			MyIsEOF = true;
			return EOF;
		}
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

bool mMemoryStream::IsEOF( void )const
{
	return MyIsEOF;
}

bool mMemoryStream::FlushCache( void )
{
	return true;
}

