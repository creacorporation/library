//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mWorkerThreadPool.h"
#include "General/mBinaryUtil.h"
#include "General/mErrorLogger.h"
#include <vector>

mWorkerThreadPool::mWorkerThreadPool()
{
	//IO�����|�[�g���쐬����
	MyIoPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE , 0 , 0 , 0 );
	if( MyIoPort == NULL )
	{
		MyIoPort = INVALID_HANDLE_VALUE;
		RaiseError( g_ErrorLogger , 0 , L"IO�����|�[�g���쐬�ł��܂���ł���" );
	}

	//�^�X�N�̐���������
	MyQueuedTaskCount = 0;

	return;
}

mWorkerThreadPool::~mWorkerThreadPool()
{
	//�X���b�h�j��
	for( ThreadPool::iterator itr = MyThreadPool.begin() ; itr != MyThreadPool.end() ; itr++ )
	{
		itr->Terminate();
	}

	//IO�����|�[�g�j��
	if( MyIoPort != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyIoPort );
		MyIoPort = INVALID_HANDLE_VALUE;
	}

	return;
}

static int GetCreateThreadCount( int request )
{
	int thread_count;

	if( 1 <= request )
	{
		thread_count = request;
	}
	else
	{
		//�K�v�o�b�t�@�T�C�Y�𓾂�
		DWORD bufflen= 0;
		GetLogicalProcessorInformation( NULL, &bufflen );
		if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�v���Z�b�T�̏��擾���s���o�b�t�@���m�ۂł��܂���" );
			return 0;
		}
		
		using ProcInfo = std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION>;
		ProcInfo proc_info;
		proc_info.resize( bufflen / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) );

		//���擾
		if( !GetLogicalProcessorInformation( proc_info.data() , &bufflen ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�v���Z�b�T�̏����擾�ł��܂���" );
			return 0;
		}

		//�����R�A������
		int physical_core = 0;
		int logical_core = 0;
		for( ProcInfo::const_iterator itr = proc_info.begin() ; itr != proc_info.end() ; itr++ )
		{
			//���ꕨ���R�A�H
			if( itr->Relationship == RelationProcessorCore )
			{
				//ProcessorMask��1�ɂȂ��Ă���r�b�g���A���̕����R�A���S�����Ă���
				//�_���R�A�̃}�X�N�ɂȂ�
				physical_core++;
				logical_core += BitCount( itr->ProcessorMask );
			}
		}

		//�ݒ�l�ɉ����ăX���b�h��������
		switch( request )
		{
		case mWorkerThreadPool::THREAD_LOGICAL_CORE_MINUS1:
			thread_count = ( 2 <= logical_core ) ? ( logical_core - 1 ) : ( 0 );
			break;
		case mWorkerThreadPool::THREAD_LOGICAL_CORE:
			thread_count = ( 1 <= logical_core ) ? ( logical_core - 0 ) : ( 0 );
			break;
		case mWorkerThreadPool::THREAD_PHYSICAL_CORE_MINUS1:
			thread_count = ( 2 <= physical_core ) ? ( physical_core - 1 ) : ( 0 );
			break;
		case mWorkerThreadPool::THREAD_PHYSICAL_CORE:
			thread_count = ( 1 <= physical_core ) ? ( physical_core - 0 ) : ( 0 );
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"�X���b�h���w��t���O���s���ł�" , request );
			return 0;
		}

		//�X���b�h���`�F�b�N
		if( thread_count == 0 )
		{
			//�R�A���S��������Ȃ������ꍇ��1�ɂ���
			RaiseError( g_ErrorLogger , 0 , L"�K�v�ȃX���b�h�����擾�ł��܂���ł���" );
		}
	}
	return thread_count;
}


bool mWorkerThreadPool::Begin( int threads , int min_threads , int max_threads )
{
	if( !MyThreadPool.empty() )
	{
		RaiseError( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�v�[�����d�ɍ쐬���悤�Ƃ��܂���" );
		return false;
	}

	//�p�����[�^�`�F�b�N
	if( ( 0 <= min_threads ) &&
		( 0 <= max_threads ) &&
		( max_threads < min_threads ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ŏ��X���b�h���ƍő�X���b�h���̎w�肪�s���ł�" );
		return false;
	}

	//�쐬����X���b�h�̐������߂�
	int req_threads = GetCreateThreadCount( threads );

	//�X���b�h�����ő�l�E�ŏ��l�ŕ␳
	if( ( 0 < min_threads ) &&	( req_threads < min_threads ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�ŏ��X���b�h���̎w��ɂ��X���b�h�����␳����܂���" );
		req_threads = min_threads;
	}
	else
	{
		//min_threads�����w��(0�ȉ�)�̏ꍇ�ł��A�Z�o�X���b�h����0�ȉ��ł����1�ɕ␳����
		if( req_threads <= 0 )
		{
			CreateLogEntry( g_ErrorLogger , 0 , L"�����ݒ肵���X���b�h����0�ł��������߁A1�ɕ␳���܂�" );
			req_threads = 1;
		}
	}
	if( ( 0 < max_threads ) &&	( max_threads < req_threads ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�ő�X���b�h���̎w��ɂ��X���b�h�����␳����܂���" );
		req_threads = max_threads;
	}

	//�X���b�h�쐬
	for( int i = 0 ; i < req_threads ; i++ )
	{
		MyThreadPool.emplace_back( *this );
		if( !MyThreadPool[ i ].Begin( 0 ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�̊J�n�����s���܂���" , i );
		}
	}
	
	//�X���b�h�N��
	for( int i = 0 ; i < req_threads ; i++ )
	{
		if( !MyThreadPool[ i ].Resume() )
		{
			RaiseError( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�̃��W���[�������s���܂���" , i );
		}
	}
	return true;
}

static VOID CALLBACK DummyCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	return;
}

bool mWorkerThreadPool::End( void )
{
	//�I���v��
	for( ThreadPool::iterator itr = MyThreadPool.begin() ; itr != MyThreadPool.end() ; itr++ )
	{
		itr->FinishRequest();
	}

	//�Q�Ă���X���b�h�����邩������Ȃ��̂Ń_�~�[�̊����p�P�b�g�𓊂���
	for( size_t i = 0 ; i < MyThreadPool.size() ; i++ )
	{
		PostQueuedCompletionStatus( MyIoPort , 0 , (ULONG_PTR)DummyCompleteRoutine , 0 );
	}

	//��~�҂�
	for( ThreadPool::iterator itr = MyThreadPool.begin() ; itr != MyThreadPool.end() ; itr++ )
	{
		if( !itr->End() )
		{
			RaiseError( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�̒�~�Ɏ��s���܂���" );
			return false;
		}
	}

	//���\�[�X�p��
	MyThreadPool.clear();

	return true;
}

bool mWorkerThreadPool::Attach( HANDLE handle , LPOVERLAPPED_COMPLETION_ROUTINE callback )
{
	if( handle == INVALID_HANDLE_VALUE || handle == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����ȃn���h�����w�肳��܂���" );
		return false;
	}
	if( callback == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����Ȋ����֐����w�肳��܂���" );
		return false;
	}

	if( !CreateIoCompletionPort( handle , MyIoPort , (ULONG_PTR)callback , 0 ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"IO�����|�[�g�ւ̊֘A�t�������s���܂���" );
		return false;
	}
	return true;
}

HANDLE mWorkerThreadPool::GetHandle( void )const
{
	return MyIoPort;
}

//�������[�`��
VOID CALLBACK mWorkerThreadPool::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	if( ov == nullptr )
	{
		return;
	}

	//�^�X�N���s
	TaskInfoEntry* entry = (TaskInfoEntry*)ov;
	if( entry->TaskFunction )
	{
		entry->TaskFunction( entry->Parent , entry->Param1 , entry->Param2 );
	}

	//�^�X�N�̃f�[�^��j��
	//�^�X�N�J�E���g���Q�Ƃ��邽�߁AParent�̎Q�Ƃ����R�s�[������Ă���
	mWorkerThreadPool& Parent( entry->Parent );
	mDelete entry;
	{
		mCriticalSectionTicket critical( Parent.MyCriticalSection );	
		Parent.MyQueuedTaskCount--;
	}
}

//�^�X�N�̒ǉ�
bool mWorkerThreadPool::AddTask( CallbackFunction callback , DWORD Param1, DWORD_PTR Param2 )
{
	if( callback == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���s����^�X�N���ݒ肳��Ă��܂���" );
		return false;
	}

	mCriticalSectionTicket critical( MyCriticalSection );

	TaskInfoEntry* entry = mNew TaskInfoEntry( *this );
	entry->Param1 = Param1;
	entry->Param2 = Param2;
	entry->TaskFunction = callback;

	if( !PostQueuedCompletionStatus( MyIoPort , 0 , (ULONG_PTR)CompleteRoutine , (LPOVERLAPPED)entry ) )
	{
		mDelete entry;
		RaiseError( g_ErrorLogger , 0 , L"�^�X�N��ǉ��ł��܂���" );
		return false;
	}
	MyQueuedTaskCount++;
	return true;
}

//���ݕێ����Ă��関�����^�X�N�̐����擾����
DWORD mWorkerThreadPool::GetTaskCount( void )const
{
	return MyQueuedTaskCount;
}

//�X���b�h�̐��𓾂�
DWORD mWorkerThreadPool::GetThreadCount( void )const
{
	return (DWORD)MyThreadPool.size();
}
