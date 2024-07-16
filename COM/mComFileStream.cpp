//----------------------------------------------------------------------------
// COMへのストリーミング書き込み操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MCOMFILESTREAM_CPP_COMPILING
#include "mComFileStream.h"
#include "General/mErrorLogger.h"
#include "General/mFileUtility.h"
#include <shlwapi.h>

mComFileStream::mComFileStream( DWORD buffersize )
	: MAX_BUFFER_SIZE( buffersize )
{
	ResetWriteCache();
	MyStream = nullptr;
}

mComFileStream::~mComFileStream()
{
	MyStream->Release();
}

bool mComFileStream::Close( void )
{
	return false;
}

bool mComFileStream::Write( INT data )
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

bool mComFileStream::IsOpen( void )const
{
	if( !MyStream )
	{
		return false;
	}
	return true;
}

INT mComFileStream::Read( void )
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
				return EOF;
			}
		}
		else
		{
			MyReadCacheCurrent = 0;
			MyReadCacheRemain = 0;
			return EOF;
		}
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

bool mComFileStream::IsEOF( void )const
{
	return !IsOpen();
}

bool mComFileStream::FlushCache( void )
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

void mComFileStream::ResetWriteCache( void )
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


IStream* mComFileStream::Get( void )const
{
	return MyStream;
}

mComFileStream::operator IStream*( void )const
{
	return Get();
}

bool mComFileStream::SetPointer( ULONGLONG pos )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = pos;

	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_SET , nullptr );
	return SUCCEEDED( hr );
}

bool mComFileStream::MovePointer( LONGLONG distance )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = distance;

	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_CUR , nullptr );
	return SUCCEEDED( hr );
}

bool mComFileStream::SetPointerToEnd( void )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = 0;

	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_END , nullptr );
	return SUCCEEDED( hr );
}

bool mComFileStream::SetPointerToBegin( void )
{
	return SetPointer( 0 );
}

//ファイルを開きます。
bool mComFileStream::Open( const mFile::Option& opt )
{
	if( IsOpen() )
	{
		return false;
	}

	DWORD mode = 0;
	BOOL create = FALSE;
	//アクセスモード
	if( opt.AccessWrite )
	{
		mode |= ( opt.AccessRead ) ? ( STGM_READWRITE ) : ( STGM_WRITE );
	}
	else
	{
		mode |= STGM_READ;
	}
	//共有
	if( opt.ShareRead && opt.ShareWrite )
	{
		mode |= STGM_SHARE_DENY_NONE;
	}
	else if( opt.ShareRead && !opt.ShareWrite )
	{
		mode |= STGM_SHARE_DENY_WRITE;
	}
	else if( !opt.ShareRead && opt.ShareWrite )
	{
		mode |= STGM_SHARE_DENY_READ;
	}
	else
	{
		mode |= STGM_SHARE_EXCLUSIVE;
	}
	//開き方
	switch( opt.Mode )
	{
	case mFile::CreateMode::CreateNew:				//新しくファイルを作成。すでにある場合はエラー
		mode |= STGM_FAILIFTHERE;
		create = TRUE;
		break;
	case mFile::CreateMode::CreateAlways:			//新しくファイルを作成。すでにある場合は上書き（中身を捨てる）
		mode |= STGM_CREATE;
		create = FALSE;
		break;
	case mFile::CreateMode::OpenExisting:			//すでにあるファイルを開く。ない場合はエラー
		mode |= STGM_FAILIFTHERE;
		create = FALSE;
		break;
	case mFile::CreateMode::OpenAlways:				//※すでにあるファイルを開く。ない場合は新しいファイルを開く
		if( PathFileExistsW( opt.Path.c_str() ) )
		{
			mode |= STGM_FAILIFTHERE;
			create = FALSE;
		}
		else
		{
			mode |= STGM_CREATE;
			create = FALSE;
		}
		break;
	case mFile::CreateMode::TruncateExisting:		//※すでにあるファイルを開いて中身を捨てる。ない場合はエラー。
		mode |= STGM_FAILIFTHERE;
		create = FALSE;
		break;
	case mFile::CreateMode::CreateWithDirectory:	//新しくファイルを作成。ディレクトリがない場合はディレクトリも作成する。すでにある場合は上書き（中身を捨てる）。
		if( !mFileUtility::CreateMiddleDirectory( opt.Path ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"中間のディレクトリを作成できません" + opt.Path );
			return false;
		}
		mode |= STGM_CREATE;
		create = FALSE;
		break;
	default:
		return false;
	}

	HRESULT hr = SHCreateStreamOnFileEx( opt.Path.c_str() , mode , 0 , create , nullptr , &MyStream );
	if( FAILED( hr ) )
	{
		SetLastError( hr );
		RaiseError( g_ErrorLogger , 0 , L"ファイルのオープンが失敗しました" + opt.Path );
		return false;
	}
	if( opt.Mode == mFile::CreateMode::TruncateExisting )
	{
		ULARGE_INTEGER li;
		li.QuadPart = 0;
		MyStream->SetSize( li );
	}
	return true;
}
