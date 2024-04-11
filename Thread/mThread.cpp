//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mThread.h"
#include "General/mErrorLogger.h"

unsigned int __stdcall ThreadBaseFunc( void* arg )
{
	//�X���b�h�̊J�n�G���g���|�C���g
	mThread* thread_ptr = (mThread*)arg;
	unsigned result = thread_ptr->TaskFunction();

	_endthreadex( result );
	return result;
}

mThread::mThread()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyThreadId = 0;
	MyTerminateSignal = INVALID_HANDLE_VALUE;
	MyArg = 0;
}

mThread::~mThread()
{
	Clear();
}

bool mThread::Begin( std::shared_ptr<void> arg )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�X���b�h�͂��łɍ쐬����Ă��܂�" , MyThreadId );
		return false;
	}

	//�I���V�O�i���𐶐�����
	MyTerminateSignal = CreateEvent( 0 , true , false , 0 );
	MyArg = arg;

	//�X���b�h�J�n
	MyHandle = (HANDLE)_beginthreadex( 0 , 0 , ThreadBaseFunc , this , CREATE_SUSPENDED , &MyThreadId );
	if( MyHandle == (HANDLE)-1 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�X���b�h���J�n�ł��܂���ł���" );
		return false;
	}
	return true;
}

bool mThread::Begin( void )
{
	std::shared_ptr<void> arg;
	return Begin( arg );
}

bool mThread::SetAffinityMask( DWORD_PTR mask )
{
	return SetThreadAffinityMask( MyHandle , mask );
}

bool mThread::Resume( DWORD* retPrevCount )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���݂��Ȃ��X���b�h�����W���[�����悤�Ƃ��܂���" );
		return false;
	}
	DWORD count = ResumeThread( MyHandle );
	if( retPrevCount )
	{
		*retPrevCount = count;
	}
	return count != (DWORD)-1;
}

bool mThread::Suspend( DWORD* retPrevCount )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���݂��Ȃ��X���b�h���T�X�y���h���悤�Ƃ��܂���" );
		return false;
	}
	DWORD count = SuspendThread( MyHandle );
	if( retPrevCount )
	{
		*retPrevCount = count;
	}
	return count != (DWORD)-1;
}

bool mThread::Clear()
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( (HANDLE)MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
		MyThreadId = 0;
	}

	if( MyTerminateSignal != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyTerminateSignal );
		MyTerminateSignal = INVALID_HANDLE_VALUE;
	}
	return true;
}

bool mThread::End( void )
{
	//�I���V�O�i�����Z�b�g����
	if( !FinishRequest() )
	{
		return false;
	}

	//�������g���I�����悤�Ƃ��Ă��Ȃ����`�F�b�N����
	if( GetCurrentThreadId() == MyThreadId )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X���b�h���������g�̏I����ҋ@���悤�Ƃ��܂���" , MyThreadId );
		return false;
	}

	//�I���܂őҋ@����
	while( 1 )
	{
		//sync=true�̏ꍇ�A�X���b�h�n���h�����V�O�i����ԁi�X���b�h�̏I���������j�ɂȂ�܂őҋ@
		//�Ƃ肠����200ms�҂��Ă݂�
		DWORD wait_result = WaitForSingleObject( (HANDLE)MyHandle , 200 );

		if( wait_result == WAIT_OBJECT_0 )
		{
			//�V�O�i����ԂɂȂ����ꍇ�A����I��
			break;
		}
		else if( wait_result == WAIT_TIMEOUT )
		{
			//�^�C���A�E�g�̏ꍇ�A�T�X�y���h���Ă��Ȃ����m�F����
			DWORD suspend_count;
			if( !Suspend( &suspend_count ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"�X���b�h�̏�Ԃ��擾�ł��܂���" );
				return false;
			}
			if( suspend_count )
			{
				//�T�X�y���h��
				RaiseError( g_ErrorLogger , 0 , L"�X���b�h�̃T�X�y���h���ɓ������[�h�ŏI�����悤�Ƃ��܂���" );
				return false;
			}
			if( !Resume( &suspend_count ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"�X���b�h�̏�Ԃ��擾�ł��܂���" );
				return false;
			}
		}
		else
		{
			//WAIT_OBJECT_0�ȊO�̏ꍇ�A�Ȃɂ����̂��ăX���b�h�����ł��Ă���
			RaiseError( g_ErrorLogger , 0 , L"�I���Ώۂ̃X���b�h�͂��łɑ��݂��܂���" );
			Clear();
			return false;
		}
	}
	Clear();
	return true;
}

bool mThread::FinishRequest( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���݂��Ȃ��X���b�h���I�����悤�Ƃ��܂���" );
		return false;
	}

	//�I���V�O�i�����Z�b�g����
	if( !SetEvent( MyTerminateSignal ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�I���V�O�i���̃Z�b�g�����s���܂���" );
		return false;
	}
	return true;
}


bool mThread::Terminate( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���݂��Ȃ��X���b�h�������I�����悤�Ƃ��܂���" );
		return false;
	}

	if( !TerminateThread( MyHandle , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�X���b�h�̋����I�������s���܂���" );
		return false;
	}

	Clear();
	return true;
}

bool mThread::IsValid( void )const
{
	return MyHandle != INVALID_HANDLE_VALUE;
}


