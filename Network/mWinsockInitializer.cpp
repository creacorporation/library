//----------------------------------------------------------------------------
// Winsockユーティリティ
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mWinsockInitializer.h"
#include <General/mErrorLogger.h>
#include <General/mCriticalSectionContainer.h>

static ULONG g_WinsockInitializeCount = 0;
static WSADATA g_WsaData = { 0 };

mWinsockInitializer::mWinsockInitializer()
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
		if( !SetExtendFunctionPointer() )
		{
			RaiseError( g_ErrorLogger , 0 , L"拡張関数の取得が失敗しました" );
		}
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsockが初期化されました" );
	}

	g_WinsockInitializeCount++;
	MyIsInitialized = true;
}

mWinsockInitializer::~mWinsockInitializer()
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

bool mWinsockInitializer::IsInitialized( void )const
{
	return MyIsInitialized;
}

mWinsockInitializer::operator bool() const
{
	return MyIsInitialized;
}

bool mWinsockInitializer::SetExtendFunctionPointer( void )
{
	//ダミーソケットの作成
	SOCKET socket = WSASocket( AF_INET , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
	if( socket == INVALID_SOCKET )
	{
		return false;
	}

	//拡張関数のアドレス取得
	bool result = true;
	DWORD resultsize = 0;
	GUID guid;
	
	guid = WSAID_CONNECTEX;
	result &= ( WSAIoctl( socket , SIO_GET_EXTENSION_FUNCTION_POINTER , &guid , sizeof( guid ) , &MyConnextEx , sizeof( MyConnextEx ) , &resultsize , nullptr , nullptr ) == 0 );
	guid = WSAID_ACCEPTEX;
	result &= ( WSAIoctl( socket , SIO_GET_EXTENSION_FUNCTION_POINTER , &guid , sizeof( guid ) , &MyAcceptEx , sizeof( MyAcceptEx ) , &resultsize , nullptr , nullptr ) == 0 );
	guid = WSAID_TRANSMITFILE;
	result &= ( WSAIoctl( socket , SIO_GET_EXTENSION_FUNCTION_POINTER , &guid , sizeof( guid ) , &MyTransmitFile , sizeof( MyTransmitFile ) , &resultsize , nullptr , nullptr ) == 0 );

	//ソケット廃棄
	closesocket( socket );

	return result;
}

bool mWinsockInitializer::ConnextEx(
	SOCKET s,
	const struct sockaddr FAR *name,
	int namelen,
	PVOID lpSendBuffer,
	DWORD dwSendDataLength,
	LPDWORD lpdwBytesSent,
	LPOVERLAPPED lpOverlapped
)const
{
	if( !MyConnextEx )
	{
		return false;
	}
	return MyConnextEx( s , name , namelen , lpSendBuffer , dwSendDataLength , lpdwBytesSent , lpOverlapped );
}

bool mWinsockInitializer::AcceptEx(
	SOCKET sListenSocket,
	SOCKET sAcceptSocket,
	PVOID lpOutputBuffer,
	DWORD dwReceiveDataLength,
	DWORD dwLocalAddressLength,
	DWORD dwRemoteAddressLength,
	LPDWORD lpdwBytesReceived,
	LPOVERLAPPED lpOverlapped
)const
{
	if( !MyAcceptEx )
	{
		return false;
	}
	return MyAcceptEx( sListenSocket , sAcceptSocket , lpOutputBuffer , dwReceiveDataLength , dwLocalAddressLength , dwRemoteAddressLength , lpdwBytesReceived , lpOverlapped );
}

bool mWinsockInitializer::TransmitFile(
	SOCKET hSocket,
	HANDLE hFile,
	DWORD nNumberOfBytesToWrite,
	DWORD nNumberOfBytesPerSend,
	LPOVERLAPPED lpOverlapped,
	LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
	DWORD dwReserved
)const
{
	if( !MyTransmitFile )
	{
		return false;
	}
	return MyTransmitFile( hSocket , hFile , nNumberOfBytesToWrite , nNumberOfBytesPerSend , lpOverlapped , lpTransmitBuffers , dwReserved );
}
