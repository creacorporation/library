//----------------------------------------------------------------------------
// Winsockユーティリティ
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MWINSOCKINITIALIZER_H_INCLUDED
#define MWINSOCKINITIALIZER_H_INCLUDED

#include <WinSock2.h>
#include <mswsock.h>
#include "mStandard.h"

class mWinsockInitializer
{
public:
	virtual ~mWinsockInitializer();

	static mWinsockInitializer& Get( void )
	{
		static mWinsockInitializer s_WinsockInitializer;
		return s_WinsockInitializer;
	}

	//Winsockがイニシャライズされているかどうか
	bool IsInitialized( void )const;

	//Winsockがイニシャライズされているかどうか
	operator bool() const;

	bool ConnextEx(
		SOCKET s,
		const struct sockaddr FAR *name,
		int namelen,
		PVOID lpSendBuffer,
		DWORD dwSendDataLength,
		LPDWORD lpdwBytesSent,
		LPOVERLAPPED lpOverlapped
	)const;

	bool AcceptEx(
		SOCKET sListenSocket,
		SOCKET sAcceptSocket,
		PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
		LPDWORD lpdwBytesReceived,
		LPOVERLAPPED lpOverlapped
	)const;

	bool TransmitFile(
		SOCKET hSocket,
		HANDLE hFile,
		DWORD nNumberOfBytesToWrite,
		DWORD nNumberOfBytesPerSend,
		LPOVERLAPPED lpOverlapped,
		LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
		DWORD dwReserved
	)const;

private:
	mWinsockInitializer();
	mWinsockInitializer( const mWinsockInitializer& src ) = delete;
	const mWinsockInitializer& operator=( const mWinsockInitializer& src ) = delete;

	bool SetExtendFunctionPointer( void );

	bool MyIsInitialized = false;
	LPFN_CONNECTEX MyConnextEx = nullptr;
	LPFN_ACCEPTEX MyAcceptEx = nullptr;
	LPFN_TRANSMITFILE MyTransmitFile = nullptr;
};

#endif
