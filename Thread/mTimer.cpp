//----------------------------------------------------------------------------
// �^�C�}�[�R���g���[��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mTimer.h"
#include "../General/mErrorLogger.h"
#include "../General/mCriticalSectionContainer.h"


static HANDLE g_TimerQueue = nullptr;
static DWORD  g_TimerQueueCount = 0;

mTimer::mTimer()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyCount = 0;

	{
		mCriticalSectionTicket critical( g_CriticalSection );
		if( g_TimerQueueCount == 0 )
		{
			g_TimerQueue = CreateTimerQueue();
		}
		g_TimerQueueCount++;
	}

}

mTimer::~mTimer()
{
	Stop();
	{
		mCriticalSectionTicket critical( g_CriticalSection );
		g_TimerQueueCount--;
		if( g_TimerQueueCount == 0 )
		{
			DeleteTimerQueue( g_TimerQueue );
			g_TimerQueue = 0;
		}
	}
}

bool mTimer::Setup( const NotificationOption& opt )
{
	SetCount( opt.Count );

	switch( opt.Method )
	{
	case NotificationOption::METHOD_NONE:
		MyOption.reset( mNew Option_None( *(Option_None*)&opt ) );
		break;
	case NotificationOption::METHOD_WINDOW_MESSAGE:
		MyOption.reset( mNew Option_WindowMessage( *(Option_WindowMessage*)&opt ) );
		break;
	case NotificationOption::METHOD_SIGNAL_OBJECT:
		MyOption.reset( mNew Option_SignalObject( *(Option_SignalObject*)&opt ) );
		break;
	case NotificationOption::METHOD_CALLBACK_FUNCTION:
		MyOption.reset( mNew Option_CallbackFunction( *(Option_CallbackFunction*)&opt ) );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�C�x���g�ʒm���@�̎w�肪����Ă��܂�" , opt.Method );
		MyOption.reset( mNew Option_None( *(Option_None*)&opt ) );
		break;
	}

	if( opt.StartImmediate )
	{
		return Start();
	}
	return true;
}

bool mTimer::Restart( void )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		return true;
	}
	if( GetCount() != 0 )
	{
		if( !CreateTimerQueueTimer( &MyHandle , g_TimerQueue , TimerRoutine , this , MyOption->Interval , 0 , WT_EXECUTEONLYONCE ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�^�C�}�[���J�n�ł��܂���" );
			return false;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"�^�C�}�[�̎c����s�񐔂�0�ł�" );
		return false;
	}
	return true;
}

bool mTimer::Start( void )
{
	SetCount();
	return Restart();
}


bool mTimer::Stop( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return true;
	}

	HANDLE handle_to_kill = MyHandle;
	MyHandle = INVALID_HANDLE_VALUE;

	if( !DeleteTimerQueueTimer( g_TimerQueue , handle_to_kill , NULL ) )
	{
		if( GetLastError() != ERROR_IO_PENDING )
		{
			RaiseError( g_ErrorLogger , 0 , L"�^�C�}�[���~�ł��܂���" );
			return false;
		}
	}
	return true;
}

bool mTimer::SetCount( int newval )
{
	InterlockedExchange( &MyCount , newval );
	return true;
}

bool mTimer::SetCount( void )
{
	return SetCount( MyOption->Count );
}

int mTimer::GetCount( void )const
{
	return MyCount;
}

VOID CALLBACK mTimer::TimerRoutine( PVOID param , BOOLEAN istimer )
{
	//�ΏۃI�u�W�F�N�g����
	mTimer* parent = (mTimer*)param;

	//�c��񐔌��Z
	long Value = parent->MyCount;
	if( Value < 0 )
	{
		;	//�����J��Ԃ��Ȃ̂Ō��Z�Ȃ�
	}
	else if( Value == 0 )
	{
		//0�ɂȂ��Ă���̂Ŗ���
		parent->Stop();
		return;
	}
	else
	{
		//�L���񐔂̌J��Ԃ��Ȃ̂ŉ񐔂̍X�V
		do
		{
			if( InterlockedCompareExchange( &parent->MyCount , Value - 1 , Value ) == Value )
			{
				Value--;
				if( Value == 0 )
				{
					//0��ɂȂ����̂Ń^�C�}���~�߂�
					parent->Stop();
				}
				break;
			}
			Value = parent->MyCount;
		}while( 1 );
	}

	//�L���[�̐擪�̏ꍇ�͊����C�x���g���R�[��
	switch( parent->MyOption->Method )
	{
	case NotificationOption::METHOD_NONE:
		break;
	case NotificationOption::METHOD_WINDOW_MESSAGE:
	{
		//�E�C���h�E���b�Z�[�W�̏ꍇ
		// wparam : �ďo����mASyncNamedPipe�ւ̃|�C���^
		// lparam : Parameter�̒l
		Option_WindowMessage* op = (Option_WindowMessage*)( parent->MyOption.get() );
		PostMessageW( op->Sendto , op->OnTimer , (WPARAM)parent , op->Parameter );
		break;
	}
	case NotificationOption::METHOD_CALLBACK_FUNCTION:
	{
		//�R�[���o�b�N�֐��̏ꍇ
		Option_CallbackFunction* op = (Option_CallbackFunction*)( parent->MyOption.get() );
		if( op->OnTimer )
		{
			op->OnTimer( *parent , op->Parameter , Value );
		}
		break;
	}
	case NotificationOption::METHOD_SIGNAL_OBJECT:
	{
		//�V�O�i���I�u�W�F�N�g�̏ꍇ
		Option_SignalObject* op = (Option_SignalObject*)( parent->MyOption.get() );
		if( op->OnTimer != INVALID_HANDLE_VALUE )
		{
			SetEvent( op->OnTimer );
		}
	}
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�񓯊�����̊����ʒm���@���s���ł�" , parent->MyOption->Method );
		break;
	}

	//�V�����^�C�}�[�����
	if( parent->MyHandle != INVALID_HANDLE_VALUE && 0 != Value )
	{
		parent->Stop();
		parent->Start();
	}

}



