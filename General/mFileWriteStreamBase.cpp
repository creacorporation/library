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
#include "mFileWriteStreamBase.h"
#include "General/mErrorLogger.h"

mFileWriteStreamBase::mFileWriteStreamBase()
{
	MyWriteCacheRemain = 0;
	MyWriteCacheWritten = 0;
}

mFileWriteStreamBase::~mFileWriteStreamBase()
{
}


bool mFileWriteStreamBase::WriteString( const AString& line )	//ANSI文字列を書き込む
{

	for( DWORD i = 0 ; i < line.size() ; i++ )
	{
		if( !Write( line.at( i ) ) )
		{
			WString error_str = AString2WString( line );
			RaiseAssert( g_ErrorLogger , i , L"Write ANSI failed : " + error_str );
			return false;
		}
	}
	return true;
}


bool mFileWriteStreamBase::WriteString( const WString& line )	//UNICODE文字列を書き込む
{

	for( DWORD i = 0 ; i < line.size() ; i++ )
	{
		wchar_t dt =  line.at( i );

		if( !Write( ( dt >> 0 ) & 0x000000FF ) )
		{
			RaiseAssert( g_ErrorLogger , i * 2 + 0 , L"Write UNICODE failed : " + line );
			return false;
		}
		if( !Write( ( dt >> 8 ) & 0x000000FF ) )
		{
			RaiseAssert( g_ErrorLogger , i * 2 + 1 , L"Write UNICODE failed : " + line );
			return false;
		}
	}
	return true;

}

bool mFileWriteStreamBase::WriteString( const char* line )	//ANSI文字列を書き込む
{
	if( !line )
	{
		return false;
	}
	return WriteString( AString( line ) );
}

bool mFileWriteStreamBase::WriteString( const wchar_t* line )	//UNICODE文字列を書き込む
{
	if( line == 0 )
	{
		return true;
	}
	return WriteString( WString( line ) );
}

//指定サイズを書込みます
bool mFileWriteStreamBase::WriteBinary( _In_bytecount_(size) const BYTE* buffer , size_t size )
{
	for( size_t i = 0 ; i < size ; i++ )
	{
		if( !Write( *buffer ) )
		{
			RaiseAssert( g_ErrorLogger , i , L"write binary failed" );
			return false;
		}
		buffer++;
	}
	return true;
}

//指定ストリームから読み取ってそのまま書き込みます
//※指定バイト書き込むか、読み込み元ストリームがEOFになるまで書き込みます
//fp : 書き込み元
//sz : 書き込むバイト数
//retWritten : 書き込んだサイズ(不要ならnullptrで可)
//ret : 成功時true
bool mFileWriteStreamBase::WriteStream( mFileReadStreamBase& fp , size_t sz , size_t* retWritten )
{
	std::unique_ptr<BYTE[]> buffer( mNew BYTE[ 65536 ] );

	while( !fp.IsEOF() && sz )
	{
		size_t readsize = ( sz < sizeof( buffer ) ) ? ( sz ) : ( sizeof( buffer ) );
		if( !fp.ReadBinary( buffer.get() , readsize , &readsize ) )
		{
			if( readsize == 0 )
			{
				continue;
			}
		}
		if( !WriteBinary( buffer.get() , readsize ) )
		{
			return false;
		}
		sz -= readsize;
		if( retWritten )
		{
			*retWritten += readsize;
		}
	}
	return true;
}

//指定ストリームから読み取ってそのまま書き込みます
//※読み込み元ストリームがEOFになるまで書き込みます
//fp : 書き込み元
//retWritten : 書き込んだサイズ(不要ならnullptrで可)
//ret : 成功時true
bool mFileWriteStreamBase::WriteStream( mFileReadStreamBase& fp , size_t* retWritten )
{
	std::unique_ptr<BYTE[]> buffer( mNew BYTE[ 65536 ] );

	while( !fp.IsEOF() )
	{
		size_t readsize;
		if( !fp.ReadBinary( buffer.get() , sizeof( buffer ) , &readsize ) )
		{
			if( readsize == 0 )
			{
				continue;
			}
		}
		if( !WriteBinary( buffer.get() , readsize ) )
		{
			return false;
		}
		if( retWritten )
		{
			*retWritten += readsize;
		}
	}
	return true;
}

