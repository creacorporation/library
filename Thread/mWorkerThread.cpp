//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------


#include "mWorkerThread.h"
#include "mWorkerThreadPool.h"
#include "General/mErrorLogger.h"

mWorkerThread::mWorkerThread( mWorkerThreadPool& pool )
	: MyParent( pool )
{
}

mWorkerThread::~mWorkerThread()
{
}

unsigned int mWorkerThread::TaskFunction( void )
{
	HANDLE handle = MyParent.GetHandle();

	while( 1 )
	{
		DWORD bytes = 0;	//�ǂݎ�����o�C�g��
		ULONG_PTR key = 0;	//�L�[�i�����֐������Ă���j
		LPOVERLAPPED ov;

		SetLastError( 0 );
		if( !GetQueuedCompletionStatus( handle , &bytes , &key , &ov , INFINITE ) )
		{
			if( ov == nullptr )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"IO�����|�[�g�̑҂��󂯂����s���܂���" , (ULONG_PTR)handle );
				return 2;
			}
		}
		LPOVERLAPPED_COMPLETION_ROUTINE CompletionRoutine = (LPOVERLAPPED_COMPLETION_ROUTINE)key;

		if( CompletionRoutine )
		{
			CompletionRoutine( GetLastError() , bytes , ov );
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"IO�����L�[���w�肳��Ă��܂���" , (ULONG_PTR)handle );
		}


		DWORD signaled = WaitForSingleObject( MyTerminateSignal , 0 );
		switch( signaled )
		{
		case WAIT_OBJECT_0 + 0:
			//�I���V�O�i��
			return 0;
		case WAIT_TIMEOUT:
			break;
		default:
			RaiseAssert( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�̏I���V�O�i�����ُ�ł�" );
			return 1;
		}
	}
}
