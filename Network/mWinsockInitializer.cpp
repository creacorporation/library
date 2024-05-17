//----------------------------------------------------------------------------
// Winsock���[�e�B���e�B
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mWinSockInitializer.h"
#include <General/mErrorLogger.h>
#include <General/mCriticalSectionContainer.h>

static ULONG g_WinsockInitializeCount = 0;
static WSADATA g_WsaData = { 0 };

mWinSockInitializer::mWinSockInitializer()
{
	//�N���e�B�J���Z�N�V����
	mCriticalSectionTicket cs( g_CriticalSection );
	MyIsInitialized = false;

	if( g_WinsockInitializeCount == 0 )
	{
		//Winsock�̏��������s��
		int err = WSAStartup( MAKEWORD( 2 , 2 ) , &g_WsaData );
		if( err )
		{
			//���������s
			RaiseErrorF( g_ErrorLogger , 0 , L"Winsock�̏����������s���܂���" , L"%d" , err );
			return;
		}
		if( LOBYTE( g_WsaData.wVersion ) != 2 || HIBYTE( g_WsaData.wVersion ) != 2 )
		{
			//���������s
			RaiseErrorF( g_ErrorLogger , 0 , L"Winsock�̃o�[�W�������Ⴂ�܂�" , L"%d" , g_WsaData.wVersion );
			WSACleanup();
			return;
		}
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsock������������܂���" );
	}

	g_WinsockInitializeCount++;
	MyIsInitialized = true;
}

mWinSockInitializer::~mWinSockInitializer()
{
	//�N���e�B�J���Z�N�V����
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
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsock���A�����[�h����܂���" );
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
