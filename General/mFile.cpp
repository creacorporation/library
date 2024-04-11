//----------------------------------------------------------------------------
// ファイル管理
// Copyright (C) 2005,2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MFILE_CPP_COMPILING
#include "mfile.h"
#include "mErrorLogger.h"
#include "mFileUtility.h"

mFile::mFile()
{
	
	MyHandle = INVALID_HANDLE_VALUE;
	return;

}

mFile::~mFile()
{
	Close();
}

bool mFile::Close( void )
{
	bool result = true;
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		result = ( CloseHandle( MyHandle ) );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	MyPath.clear();

	return result;
}



bool mFile::Open( const mFile::Option& opt )
{
	//二重に開けないようにする
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		//二重にファイルをオープンしようとしている
		RaiseError( g_ErrorLogger , 0 , L"Already opened : " + opt.Path );
		return false;
	}

	//読み書きモード
	DWORD open_mode = 0;
	open_mode |= ( opt.AccessRead ) ? ( GENERIC_READ ) : ( 0 );
	open_mode |= ( opt.AccessWrite ) ? ( GENERIC_WRITE ) : ( 0 );

	//共有モード
	DWORD share_mode = 0;
	share_mode |= ( opt.ShareRead ) ? ( FILE_SHARE_READ ) : ( 0 );
	share_mode |= ( opt.ShareWrite ) ? ( FILE_SHARE_WRITE ) : ( 0 );

	//作成モード
	DWORD create_mode = 0;
	switch( opt.Mode )
	{
	case CreateMode::CreateNew:			//新しくファイルを作成。すでにある場合はエラー
		create_mode = CREATE_NEW;
		break;
	case CreateMode::CreateAlways:		//新しくファイルを作成。すでにある場合は上書き（中身を捨てる）
		create_mode = CREATE_ALWAYS;
		break;
	case CreateMode::OpenExisting:		//すでにあるファイルを開く。ない場合はエラー
		create_mode = OPEN_EXISTING;
		break;
	case CreateMode::OpenAlways:		//すでにあるファイルを開く。ない場合は新しいファイルを開く
		create_mode = OPEN_ALWAYS;
		break;
	case CreateMode::TruncateExisting:	//すでにあるファイルを開いて中身を捨てる。ない場合はエラー。
		create_mode = TRUNCATE_EXISTING;
		break;
	case CreateMode::CreateWithDirectory:
		create_mode = CREATE_ALWAYS;
		if( !mFileUtility::CreateMiddleDirectory( opt.Path ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"中間のディレクトリを作成できません" + opt.Path );
			return false;
		}
		break;
	default:
		//不正なモードを指定している
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)opt.Mode , L"Invalid creation disposition" );
		break;
	}

	//諸々の設定ができたから、ファイルを開きます
	MyHandle = CreateFileW( opt.Path.c_str() , open_mode , share_mode , nullptr , create_mode , FILE_ATTRIBUTE_NORMAL , nullptr );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		//残念。ファイルを開くのに失敗しました。
		RaiseError( g_ErrorLogger , 0 , L"CreateFileW failed : " + opt.Path );
		return false;
	}
	MyPath = opt.Path;
	return true;

}

WString mFile::GetPath( bool fullpath )const
{
	//フルパス不要ならそのまま返して終わり
	if( !fullpath )
	{
		return MyPath;
	}

	//バッファの確保
	DWORD reqsize = GetFullPathNameW( MyPath.c_str() , 0 , nullptr , nullptr );
	if( reqsize == 0 )
	{
		return MyPath;
	}
	std::unique_ptr< wchar_t > buffer( mNew wchar_t[ reqsize ] );

	//取得
	DWORD usedsize = GetFullPathNameW( MyPath.c_str() , reqsize , buffer.get() , nullptr );
	if( reqsize != usedsize )
	{
		return MyPath;
	}

	return WString( buffer.get() );
}

bool mFile::Read(
	void*	Buffer ,							//読み取ったデータを格納するバッファ
	ULONGLONG	ReadSize ,						//読み取るバイト数
	ULONGLONG& retReadSize )					//読み取ったバイト数
{
	retReadSize = 0;
	BYTE* tmp_buffer = (BYTE*)Buffer;

	while( ReadSize )
	{
		//まあ生きている間には無いとは思うんだけど、将来的に32bitを超えるサイズを
		//一気に読み込もうとした場合に備えて、32bit単位に区切ってアクセスする。
		DWORD tmp_read_size = ( MAXDWORD < ReadSize ) ? ( MAXDWORD ) : ( (DWORD)ReadSize );
		DWORD tmp_stored_size = 0;
		
		bool result = Read( Buffer , tmp_read_size , tmp_stored_size );
		retReadSize += tmp_stored_size;
		if( !result || tmp_read_size != tmp_stored_size )
		{
			return result;
		}
		ReadSize -= tmp_stored_size;
		tmp_buffer += tmp_stored_size;
	}
	return true;
}

bool mFile::Read(
	void*	Buffer ,							//読み取ったデータを格納するバッファ
	DWORD	ReadSize ,							//読み取るバイト数
	DWORD&	retReadSize )						//読み取ったバイト数
{
	//不正入力のチェック
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"" );
		return false;
	}

	//読み取り処理
	if( !ReadFile( MyHandle , Buffer , ReadSize , &retReadSize , nullptr ) )
	{
		//ファイルの読み取りができなかった
		RaiseError( g_ErrorLogger , ReadSize , L"ReadFile failed" );
		return false;
	}
	return true;
}


bool mFile::Write(
	void*	Buffer ,							//書き込むデータを格納してあるバッファ
	ULONGLONG	WriteSize ,						//書き込むバイト数
	ULONGLONG& retWriteSize )					//書き込んだバイト数
{
	retWriteSize = 0;
	BYTE* tmp_buffer = (BYTE*)Buffer;

	while( WriteSize )
	{
		//32bit幅を超えるサイズを一気に書き込もうとしている場合、
		//WriteFileがDWORDになってるので、32bit単位で区切って出力を行う。
		//でもその前にメモリが足りなくなって終了だと思う。
		DWORD tmp_write_size = ( MAXDWORD < WriteSize ) ? ( MAXDWORD ) : ( (DWORD)WriteSize );
		DWORD tmp_stored_size = 0;

		bool result = Write( Buffer , tmp_write_size , tmp_stored_size );
		retWriteSize += tmp_stored_size;
		if( !result || tmp_write_size != tmp_stored_size )
		{
			return result;
		}
		WriteSize -= tmp_stored_size;
		tmp_buffer += tmp_stored_size;
	}
	return true;

}

bool mFile::Write(
	void*	Buffer ,							//書き込むデータを格納してあるバッファ
	DWORD	WriteSize ,							//書き込むバイト数
	DWORD&  retWriteSize )						//書き込んだバイト数
{
	//不正入力のチェック
	if( MyHandle == INVALID_HANDLE_VALUE || Buffer == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"" );
		return false;
	}
	//書き込み処理
	if( !WriteFile( MyHandle , Buffer , WriteSize , &retWriteSize , nullptr ) )
	{
		RaiseError( g_ErrorLogger , WriteSize , L"WriteFile failed" );
		return false;
	}
	return true;

}

bool mFile::SetPointer( ULONGLONG newpos )
{
	//不正入力のチェック
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	//ファイルポインタの移動(SetPointerではファイルの先頭から)
	LARGE_INTEGER pos;
	pos.QuadPart = (LONGLONG)( newpos & 0x7FFF'FFFF'FFFF'FFFFULL );	//APIの引数には符号があるので消す
	if( !SetFilePointerEx( MyHandle , pos , nullptr , FILE_BEGIN ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
		return false;
	}
	if( newpos & 0x8000'0000'0000'0000ULL )
	{
		//指定位置がファイルの先頭から8エクサバイト以上の場合（当分はあり得ない）。
		bool result = true;
		pos.QuadPart = (LONGLONG)( 0x7FFF'FFFF'FFFF'FFFFULL );
		result &= SetFilePointerEx( MyHandle , pos , nullptr , FILE_CURRENT ) != FALSE;
		pos.QuadPart = (LONGLONG)( 0x0000'0000'0000'0001ULL );
		result &= SetFilePointerEx( MyHandle , pos , nullptr , FILE_CURRENT ) != FALSE;
		if( !result )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
			return false;
		}
	}
	return true;
}

bool mFile::MovePointer( LONGLONG distance )
{
	//不正入力のチェック
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	LARGE_INTEGER pos;
	pos.QuadPart = distance;
	if( !SetFilePointerEx( MyHandle , pos , nullptr , FILE_CURRENT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
		return false;
	}
	return true;

}

bool mFile::SetPointerToEnd( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	if( !SetFilePointer( MyHandle , 0 , nullptr , FILE_END ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointer (FILE_END) failed" );
		return false;
	}
	return true;
}

bool mFile::SetPointerToBegin( void )
{

	return SetPointer( 0 );

}

bool mFile::FlushBuffer( void )
{

	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	if( !FlushFileBuffers( MyHandle ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"FlushFileBuffers failed" );
		return false;
	}
	return true;

}

bool mFile::GetFileSize( ULONGLONG& retSize )const
{
	retSize = 0;
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	//ファイルサイズの取得
	LARGE_INTEGER result;
	if( !GetFileSizeEx( MyHandle , &result ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"GetFileSizeEx failed" );
		return false;
	}
	retSize = (ULONGLONG)( result.QuadPart );
	return true;
}

bool mFile::GetFileSize( DWORD* high , DWORD& low )const
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	//ファイルサイズの取得
	LARGE_INTEGER result;
	if( !GetFileSizeEx( MyHandle , &result ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"GetFileSizeEx failed" );
		return false;
	}
	if( high )
	{
		*high = result.HighPart;
	}
	else if( result.HighPart )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ファイルサイズが4GBを超えています" );
		return false;
	}
	low = result.LowPart;
	return true;
}

ULONGLONG mFile::GetFileSize( void )const
{
	ULONGLONG tmp;
	if( !GetFileSize( tmp ) )
	{
		return 0;
	}
	return tmp;
}

bool mFile::GetPosition( ULONGLONG& retPos )const
{
	retPos = 0;
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	LARGE_INTEGER dummy_pos;
	dummy_pos.QuadPart = 0;
	LARGE_INTEGER result;
	if( !SetFilePointerEx( MyHandle , dummy_pos , &result , FILE_CURRENT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
		return false;
	}

	retPos = (ULONGLONG)( result.QuadPart );
	return true;

}

bool mFile::IsOpen( void )const
{
	return MyHandle != INVALID_HANDLE_VALUE;
}

bool mFile::SetEof( void )
{
	return SetEndOfFile( MyHandle );
}

bool mFile::ExecIoControl( DWORD code , const mBinary* in , mBinary* retResult )
{
	mBinary in_dummy;
	mBinary out_dummy;
	mBinary* in_ptr;
	mBinary* out_ptr;

	in_ptr = ( in ) ? ( const_cast< mBinary* >( in ) ) : ( &in_dummy );
	out_ptr = ( retResult ) ? ( retResult ) : ( &out_dummy );

	//必要サイズの調査
	DWORD response_size = 0;
	while( !DeviceIoControl( MyHandle , code , in_ptr->data() , in_ptr->size() , out_ptr->data() , out_ptr->size() , &response_size , nullptr ) )
	{
		DWORD errcode = GetLastError();
		if( ( errcode == ERROR_INSUFFICIENT_BUFFER ) ||
			( errcode == ERROR_MORE_DATA ) )
		{
			if( !retResult )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"結果が返されているが受け取るようになっていない" );
				return false;
			}
			out_ptr->resize( out_ptr->capacity() * 2 + 32 );
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"DeviceIoControlが失敗" );
			return false;
		}
	}
	out_ptr->resize( response_size );
	return true;
}




