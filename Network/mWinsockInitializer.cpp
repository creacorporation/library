//----------------------------------------------------------------------------
// Winsockユーティリティ
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mWinSockInitializer.h"
#include <General/mErrorLogger.h>
#include <General/mCriticalSectionContainer.h>

static ULONG g_WinsockInitializeCount = 0;
static WSADATA g_WsaData = { 0 };

mWinSockInitializer::mWinSockInitializer()
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );
	MyIsInitialized = false;

	if( g_WinsockInitializeCount == 0 )
	{
		//Winsockの初期化を行う
		int err = WSAStartup( MAKEWORD( 2 , 2 ) , &g_WsaData );
		if( err )
		{
			//初期化失敗
			RaiseErrorF( g_ErrorLogger , 0 , L"Winsockの初期化が失敗しました" , L"%d" , err );
			return;
		}
		if( LOBYTE( g_WsaData.wVersion ) != 2 || HIBYTE( g_WsaData.wVersion ) != 2 )
		{
			//初期化失敗
			RaiseErrorF( g_ErrorLogger , 0 , L"Winsockのバージョンが違います" , L"%d" , g_WsaData.wVersion );
			WSACleanup();
			return;
		}
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsockが初期化されました" );
	}

	g_WinsockInitializeCount++;
	MyIsInitialized = true;
}

mWinSockInitializer::~mWinSockInitializer()
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );
	MyIsInitialized = false;

	if( g_WinsockInitializeCount == 0 )
	{
		return;
	}
	g_WinsockInitializeCount--;
	if( g_WinsockInitializeCount == 0 )
	{
		WSACleanup();
		ZeroMemory( &g_WsaData , sizeof( g_WsaData ) );
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsockがアンロードされました" );
	}
}

bool mWinSockInitializer::IsInitialized( void )const
{
	return MyIsInitialized;
}

mWinSockInitializer::operator bool() const
{
	return MyIsInitialized;
}
