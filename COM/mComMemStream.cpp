//----------------------------------------------------------------------------
// COMへのストリーミング書き込み操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MCOMMEMSTREAM_CPP_COMPILING
#include "mComMemStream.h"
#include "General/mErrorLogger.h"
#include <shlwapi.h>

mComMemStream::mComMemStream( DWORD buffersize )
	: MAX_BUFFER_SIZE( buffersize )
{
	ResetWriteCache();
	MyStream = SHCreateMemStream( nullptr , 0 );
}

mComMemStream::~mComMemStream()
{
	MyStream->Release();
}

bool mComMemStream::Close( void )
{
	return false;
}

bool mComMemStream::Write( INT data )
{

	//キャッシュの残りがある？
	if( MyWriteCacheRemain == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"書き込みキャッシュの残量がありません" );
		return false;
	}

	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}
	return true;
}

bool mComMemStream::IsOpen( void )const
{
	if( !MyStream )
	{
		return false;
	}
	return true;
}

INT mComMemStream::Read( void )
{
	if( !IsOpen() )
	{
		return EOF;
	}

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
		if( MyReadCacheHead.get() == nullptr )
		{
			MyReadCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
		}

		HRESULT hr = MyStream->Read( MyReadCacheHead.get() , MAX_BUFFER_SIZE , &MyReadCacheRemain );
		if( SUCCEEDED( hr ) )
		{
			MyReadCacheCurrent = 0;
			if( MyReadCacheRemain == 0 )
			{
				MyIsEOF = true;
				return EOF;
			}
		}
		else
		{
			MyReadCacheCurrent = 0;
			MyReadCacheRemain = 0;
			MyIsEOF = true;
			return EOF;
		}
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

bool mComMemStream::IsEOF( void )const
{
	return MyIsEOF;
}

bool mComMemStream::FlushCache( void )
{
	//書き込むものがないときはそのまま戻る
	if( MyWriteCacheWritten == 0 )
	{
		return true;
	}

	//ファイルの書き込みを行う
	DWORD written;
	bool result = true;
	HRESULT hr = MyStream->Write( MyWriteCacheHead.get() , MyWriteCacheWritten , &written );

	if( !SUCCEEDED( hr ) || ( written != MyWriteCacheWritten  ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Cannot write" );
		result = false;
	}

	//バッファをリセット
	ResetWriteCache();

	return result;
}

void mComMemStream::ResetWriteCache( void )
{
	//もしキャッシュがないようなら作成
	if( MyWriteCacheHead.get() == nullptr )
	{
		MyWriteCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
		if( !MyWriteCacheHead )
		{
			MyWriteCacheRemain = 0;
			MyWriteCacheWritten = 0;
			return;
		}
	}
	MyWriteCacheRemain = MAX_BUFFER_SIZE;
	MyWriteCacheWritten = 0;
}


IStream* mComMemStream::Get( void )const
{
	return MyStream;
}

mComMemStream::operator IStream*( void )const
{
	return Get();
}

bool mComMemStream::SetPointer( ULONGLONG pos )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = pos;

	FlushCache();
	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_SET , nullptr );
	MyIsEOF = false;

	return SUCCEEDED( hr );
}

bool mComMemStream::MovePointer( LONGLONG distance )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = distance;

	FlushCache();
	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_CUR , nullptr );
	MyIsEOF = false;

	return SUCCEEDED( hr );
}

bool mComMemStream::SetPointerToEnd( void )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = 0;

	FlushCache();
	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_END , nullptr );
	MyIsEOF = false;

	return SUCCEEDED( hr );
}

bool mComMemStream::SetPointerToBegin( void )
{
	return SetPointer( 0 );
}

