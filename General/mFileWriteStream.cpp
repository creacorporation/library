//----------------------------------------------------------------------------
// ストリーミングファイル書き込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MFILEWRITESTREAM_CPP_COMPILING
#include "mFileWriteStream.h"
#include "General/mErrorLogger.h"

mFileWriteStream::mFileWriteStream()
{
	MyWriteCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
}

mFileWriteStream::~mFileWriteStream()
{
	mFileWriteStream::FlushCache();
	MyHandle.Close();
	MyWriteCacheHead.reset();
}

bool mFileWriteStream::FlushCache( void )
{
	//書き込むものがないときはそのまま戻る
	if( MyWriteCacheWritten == 0 )
	{
		return true;
	}

	//ファイルの書き込みを行う
	DWORD written;
	bool result = true;
	if( !MyHandle.Write( MyWriteCacheHead.get() , MyWriteCacheWritten , written ) || ( written != MyWriteCacheWritten  ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Cannot write" );
		result = false;
	}

	//バッファをリセット
	mFileWriteStream::ResetCache();

	return result;
}

void mFileWriteStream::ResetCache( void )
{
	MyWriteCacheRemain = MAX_BUFFER_SIZE;
	MyWriteCacheWritten = 0;
}

bool mFileWriteStream::Open( const mFile::Option& opt )
{

	//ファイルをオープン
	//※二重に開こうとするとこのメソッドは失敗する
	if( !MyHandle.Open( opt ) )
	{
		return false;
	}

	//ファイルを開くことが出来た場合は、ファイル名をコピーして、キャッシュを無効化
	ResetCache();

	return true;
}

bool mFileWriteStream::Close( void )
{
	if( !FlushCache() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"FlushCache failed" );
	}
	return MyHandle.Close();
}

bool mFileWriteStream::Write( INT data )
{
	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}
	return true;
}

bool mFileWriteStream::SetPointer( ULONGLONG pos )
{
	FlushCache();
	if( !MyHandle.SetPointer( pos ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetPointer failed" );
		return false;
	}
	return true;
}

bool mFileWriteStream::MovePointer( LONGLONG distance )
{
	FlushCache();
	return MyHandle.MovePointer( distance );
}

//ファイルポインタをファイルの末尾に移動
bool mFileWriteStream::SetPointerToEnd( void )
{
	FlushCache();
	return MyHandle.SetPointerToEnd();
}

//ファイルポインタをファイルの先頭に移動
bool mFileWriteStream::SetPointerToBegin( void )
{
	FlushCache();
	return MyHandle.SetPointerToBegin();
}

bool mFileWriteStream::IsOpen( void )const
{
	return MyHandle.IsOpen();
}

ULONGLONG mFileWriteStream::GetPointer( void )const
{
	ULONGLONG result = 0;
	if( MyHandle.GetPosition( result ) )
	{
		result += MyWriteCacheWritten;
	}
	return result;
}

WString mFileWriteStream::GetPath( bool fullpath )const
{
	return MyHandle.GetPath( fullpath );
}

bool mFileWriteStream::GetFileSize( ULONGLONG& retSize )const
{
	retSize = 0;
	if( MyHandle.GetFileSize( retSize ) )
	{
		retSize += MyWriteCacheWritten;
		return true;
	}
	return false;
}

bool mFileWriteStream::GetFileSize( DWORD* high , DWORD& low )const
{
	ULONGLONG tmp;
	if( !GetFileSize( tmp ) )
	{
		return 0;
	}
	LARGE_INTEGER li;
	li.QuadPart = (LONGLONG)tmp;

	if( high )
	{
		*high = li.HighPart;
	}
	else if( li.HighPart )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ファイルサイズが4GBを超えています" );
		return false;
	}
	low = li.LowPart;
	return true;
}

ULONGLONG mFileWriteStream::GetFileSize( void )const
{
	ULONGLONG tmp;
	if( !GetFileSize( tmp ) )
	{
		return 0;
	}
	return tmp;
}

bool mFileWriteStream::SetEof( void )
{
	FlushCache();
	return MyHandle.SetEof();
}
