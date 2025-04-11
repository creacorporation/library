//----------------------------------------------------------------------------
// ストリーミングファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MFILEREADSTREAM_CPP_COMPILING
#include "mFileReadStream.h"
#include "General/mErrorLogger.h"

mFileReadStream::mFileReadStream()
{
	MyReadCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
}

mFileReadStream::~mFileReadStream()
{
	MyHandle.Close();
}

bool mFileReadStream::Open( const mFile::Option& opt )
{
	//読み取りアクセスを指定しているかチェック
	if( !opt.AccessRead )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Insafficient access right : " + opt.Path );
		return false;
	}

	//ファイルをオープン
	//※二重に開こうとするとこのメソッドは失敗する
	if( !MyHandle.Open( opt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"Cannot open : " + opt.Path );
		return false;
	}

	//ファイルを開くことが出来た場合は、読み取りキャッシュ準備
	InvalidateCache();
	return true;
}

//ファイルを開きます
bool mFileReadStream::Open( const WString& filename )
{
	Option opt;
	opt.AccessRead = true;
	opt.AccessWrite = false;
	opt.Mode = CreateMode::OpenExisting;
	opt.Path = filename;
	opt.ShareRead = true;
	opt.ShareWrite = false;

	return Open( opt );
}

bool mFileReadStream::Close( void )
{
	//クローズ後は事故防止のためEOFしか読めないようにしておく
	MyIsEOF = true;
	return MyHandle.Close();
}

void mFileReadStream::InvalidateCache( void )
{
	//キャッシュを無効に。
	MyReadCacheRemain = 0;
	MyUnReadBuffer.Clear();
	return;
}

INT mFileReadStream::Read( void )
{
	//UnReadされた文字がある場合はソレを返す
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	//最後に読んだ結果もEOFなら今回もEOF
	if( MyIsEOF )
	{
		return EOF;
	}

	INT result;
	do
	{
		//キャッシュの残量が０なら、次を読む
		if( MyReadCacheRemain == 0 )
		{
			if( !ReadNextBlock() )
			{
				MyIsEOF = true;
				return EOF;
			}
		}

		//キャッシュの残量があるので、キャッシュから１文字返す
		result = MyReadCacheHead[ MyReadCacheCurrent ];
		MyReadCacheCurrent++;
		MyReadCacheRemain--;

	}while( ProcLFIgnore( result ) );

	return result;
}

bool mFileReadStream::ReadNextBlock( void )
{
	//ファイルの読み取り
	if( !MyHandle.Read( MyReadCacheHead.get() , MAX_BUFFER_SIZE , MyReadCacheRemain ) || MyReadCacheRemain == 0 )
	{
		//読めなかったらEOF
		//あるいは読み取ったサイズが０でもEOF
		return false;
	}

	//キャッシュのここまで読んだポインタを先頭にリセット
	MyReadCacheCurrent = 0;
	return true;
}

bool mFileReadStream::SetPointer( ULONGLONG position )
{
	InvalidateCache();
	if( MyHandle.SetPointer( position ) )
	{
		MyIsEOF = false;
		return true;
	}
	return false;
}

bool mFileReadStream::IsOpen( void )const
{
	return MyHandle.IsOpen();
}

//EOFに達しているかを調べます
bool mFileReadStream::IsEOF( void )const
{
	return MyIsEOF;
}

WString mFileReadStream::GetPath( bool fullpath )const
{
	return MyHandle.GetPath( fullpath );
}

//ファイルのサイズを取得
bool mFileReadStream::GetFileSize( ULONGLONG& retSize )const
{
	return MyHandle.GetFileSize( retSize );
}
