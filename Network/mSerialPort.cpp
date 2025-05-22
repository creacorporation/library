//----------------------------------------------------------------------------
// シリアルポートハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSerialPort.h"
#include <General/mErrorLogger.h>
#include <General/mFileUtility.h>

mSerialPort::mSerialPort()
{
}

mSerialPort::~mSerialPort()
{
	Abort();
	return;
}

bool mSerialPort::Open( const Option& opt )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"二重にポートを開こうとしています" );
		return false;
	}

	//引数をWinSDKの定義値に置き換える
	DWORD access = 0;
	access |= ( opt.Fileinfo.AccessRead ) ? ( GENERIC_READ ) : ( 0 );
	access |= ( opt.Fileinfo.AccessWrite ) ? ( GENERIC_WRITE ) : ( 0 );

	DWORD share = 0;
	share |= ( opt.Fileinfo.ShareRead ) ? ( FILE_SHARE_READ ) : ( 0 );
	share |= ( opt.Fileinfo.ShareWrite ) ? ( FILE_SHARE_WRITE ) : ( 0 );

	DWORD create_dispo;
	switch( opt.Fileinfo.Mode )
	{
	case CreateMode::CreateNew:
		create_dispo = CREATE_NEW;
		break;
	case CreateMode::CreateAlways:
		create_dispo = CREATE_ALWAYS;
		break;
	case CreateMode::OpenExisting:
		create_dispo = OPEN_EXISTING;
		break;
	case CreateMode::OpenAlways:
		create_dispo = OPEN_ALWAYS;
		break;
	default:
		return false;
	}

	//ファイルを開く
	MyHandle = CreateFileW( opt.Fileinfo.Path.c_str() , access , share , 0 , create_dispo , FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH , 0 );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		//開けなかった
		RaiseError( g_ErrorLogger , 0 , L"ポートを開くことができませんでした" , opt.Fileinfo.Path );
		goto errorend;
	}

	//COMポートの設定を行う
	if( !ComPortSetting( MyHandle , opt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ポートの初期設定が失敗しました" , opt.Fileinfo.Path );
		goto errorend;
	}

	//キャッシュを設定する
	MyWriteCacheHead.reset( mNew BYTE[ opt.WritePacketSize ] );

	//ファイルを開けたので、設定をストック
	MyOption = opt;
	return true;

errorend:
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	return false;
}

INT mSerialPort::Read( void )
{
	BYTE result = 0;
	DWORD readsize = 0;
	if( !ReadFile( MyHandle , &result , 1 , &readsize , nullptr ) )
	{
		return EOF;
	}
	if( readsize == 0 )
	{
		return EOF;
	}
	return result;
}

bool mSerialPort::Write( INT data )
{
	//クローズ済み？
	if( MyIsClosed )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートはすでに閉じられています" );
		return false;
	}

	//キャッシュの残りがある？
	if( MyWriteCacheRemain == 0 )
	{
		if( !FlushCache() )
		{
			return false;
		}
	}

	//キャッシュに書込んで
	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	//キャッシュが満タンだったら自動送信
	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}

	return true;
}

bool mSerialPort::FlushCache( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	//書込むデータがない場合はそのまま戻る
	if( MyWriteCacheWritten == 0 )
	{
		//データなしは正常終了扱い
		return true;
	}

	DWORD bytes_to_write = MyWriteCacheWritten;
	MyWriteCacheWritten = 0;
	MyWriteCacheRemain = MyOption.WritePacketSize;

	DWORD written = 0;
	if( !WriteFile( MyHandle , MyWriteCacheHead.get() , bytes_to_write , &written , nullptr ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"書き込み処理が失敗しました" );
		return false;
	}
	return true;
}

bool mSerialPort::Abort( void )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	return true;
}

WString mSerialPort::GetPortName( void )const
{
	WString result;
	if( !mFileUtility::SplitPath( MyOption.Fileinfo.Path , nullptr , nullptr , &result , nullptr ) )
	{
		return L"";
	}
	return result;
}
