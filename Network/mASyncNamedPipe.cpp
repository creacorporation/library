//----------------------------------------------------------------------------
// �p�C�v�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MASYNCNAMEDPIPE_CPP_COMPILING
#include "mASyncNamedPipe.h"
#include "../General/mErrorLogger.h"

mASyncNamedPipe::mASyncNamedPipe()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyIsConnected = false;
	MyConnectData = nullptr;
	MyNotifyEventToken.reset( mNew int( 0 ) );
}

mASyncNamedPipe::~mASyncNamedPipe()
{
	{
		//�����֐����炱�̃I�u�W�F�N�g���Ăяo����Ȃ��悤�ɂ���
		mCriticalSectionTicket critical( MyCritical );

		if( MyConnectData )
		{	
			MyConnectData->Parent = nullptr;
		}
		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
		for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
	}

	//�n���h���폜
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
	}
	return;
}


bool mASyncNamedPipe::Create( mWorkerThreadPool& wtp , const CreateOption& createopt , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename )
{
	//�ڑ����`�F�b�N
	DWORD max_conn = createopt.MaxConn;
	if( max_conn == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�C�v�ւ̓����ڑ�����0�ɂ͂ł��܂���" );
		return false;
	}
	else if( PIPE_UNLIMITED_INSTANCES < max_conn )
	{
		max_conn = PIPE_UNLIMITED_INSTANCES;
	}

	//�p�C�v���̍\�z
	WString full_pipe_name;
	sprintf( full_pipe_name ,  LR"(\\%s\pipe\%s)" , servername.c_str() , pipename.c_str() );

	//�p�C�v�̐���
	DWORD open_mode = 0;
	open_mode |= PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED;
	DWORD pipe_mode = 0;
	pipe_mode |= PIPE_TYPE_BYTE;
	pipe_mode |= ( createopt.RemoteAccess ) ? ( PIPE_ACCEPT_REMOTE_CLIENTS ) : ( PIPE_REJECT_REMOTE_CLIENTS );

	MyHandle = CreateNamedPipeW( full_pipe_name.c_str() , open_mode , pipe_mode , max_conn , opt.ReadPacketSize , opt.WritePacketSize , createopt.Timeout , nullptr );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"�p�C�v���쐬�ł��܂���ł���" , full_pipe_name );
		return false;
	}

	//IO�����|�[�g�ɓo�^����
	if( !Attach( wtp , opt , notifier ) )
	{
		return false;
	}

	//�ڑ���t�J�n

	MyConnectData = mNew BufferQueueEntry;
	MyConnectData->Parent = this;
	MyConnectData->Type = QueueType::CONNECT_QUEUE_ENTRY;
	MyConnectData->Buffer = nullptr;
	MyConnectData->Ov.hEvent = 0;
	MyConnectData->Ov.Internal = 0;
	MyConnectData->Ov.InternalHigh = 0;
	MyConnectData->Ov.Offset = 0;
	MyConnectData->Ov.OffsetHigh = 0;
	MyConnectData->Completed = false;
	MyConnectData->ErrorCode = 0;
	MyConnectData->BytesTransfered = 0;

	if( !ConnectNamedPipe( MyHandle , &MyConnectData->Ov ) )
	{
		DWORD ec = GetLastError();
		if( ec == ERROR_PIPE_CONNECTED )
		{
			//���łɐڑ�����Ă���̂Ŏ���R�[���o�b�N���Ăяo��
			CompleteRoutine( ERROR_SUCCESS , 0 , &MyConnectData->Ov );
		}
		else if( ec != ERROR_IO_PENDING )
		{
			RaiseError( g_ErrorLogger , 0 , L"�p�C�v�ւ̑҂��󂯃L���[���Z�b�g�ł��܂���" );
			return false;
		}
	}
	return true;
}

//�|�[�g���J��
bool mASyncNamedPipe::Attach( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier )
{

	//���[�J�[�X���b�h�v�[���ɓo�^����
	if( !wtp.Attach( MyHandle , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�v�[���ɓo�^�ł��܂���ł���" );
		return false;
	}

	//�t�@�C�����J�����̂ŁA�ʒm���@���X�g�b�N
	MyOption = opt;
	MyNotifyOption = notifier;

	return true;
}

bool mASyncNamedPipe::PrepareReadBuffer( DWORD count )
{
	//�N���e�B�J���Z�N�V����
	mCriticalSectionTicket critical( MyCritical );

	if( MyIsEOF )
	{
		//EOF�ݒ�ς�
		return true;
	}

	while( MyReadQueue.size() < count )
	{
		BufferQueueEntry* entry = mNew BufferQueueEntry;
		entry->Parent = this;
		entry->Type = QueueType::READ_QUEUE_ENTRY;
		entry->Buffer = mNew BYTE[ MyOption.ReadPacketSize ];
		entry->Ov.hEvent = 0;
		entry->Ov.Internal = 0;
		entry->Ov.InternalHigh = 0;
		entry->Ov.Offset = 0;
		entry->Ov.OffsetHigh = 0;
		entry->Completed = false;
		entry->ErrorCode = 0;
		entry->BytesTransfered = 0;
		MyReadQueue.push_back( entry );

		DWORD readsize = 0;
		ReadFile( MyHandle , entry->Buffer , MyOption.ReadPacketSize , &readsize , &entry->Ov );
		switch( GetLastError() )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"�ǂݍ��݂̔񓯊����삪�J�n���܂���ł���" );
			MyReadQueue.pop_back();
			mDelete[] entry->Buffer;
			mDelete entry;
			return false;
		}
	}

	return true;
}

void AsyncEvent( mASyncNamedPipe& pipe , const mASyncNamedPipe::NotifyOption::NotifierInfo& info , const mASyncNamedPipe::NotifyFunctionOpt& opt )
{
	if( info.Mode == mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&pipe , info.Parameter );
	}
	else if( info.Mode == mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mASyncNamedPipe::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
				if( !mASyncNamedPipe::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
	}
	else if( info.Mode == mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
	{
		if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
		{
			SetEvent( info.Notifier.Handle );
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�񓯊�����̊����ʒm���@���s���ł�" , info.Mode );
	}
}


VOID CALLBACK mASyncNamedPipe::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{

	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	if( ov == nullptr || entry == nullptr )
	{
		return;
	}

	if( !entry->Parent )
	{
		//�e�����ł��Ă���ꍇ�͂����ƍ폜���Ă���
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"�e�I�u�W�F�N�g�����ł��Ă��܂�" , entry->Type );

		mDelete[] entry->Buffer;
		mDelete entry;
		return;
	}

	NotifyEventToken token( entry->Parent->MyNotifyEventToken );

	switch( entry->Type )
	{
	case QueueType::CONNECT_QUEUE_ENTRY:
		ConnectCompleteRoutine( ec , len , ov );
		break;
	case QueueType::READ_QUEUE_ENTRY:
		ReadCompleteRoutine( ec , len , ov );
		break;
	case QueueType::WRITE_QUEUE_ENTRY:
		WriteCompleteRoutine( ec , len , ov );
		break;
	default:
		break;
	}
	return;
}

//�ڑ��������̊������[�`��
VOID CALLBACK mASyncNamedPipe::ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	//�L���[��������Ԃɂ���
	entry->Completed = true;
	entry->ErrorCode = ec;
	entry->Parent->MyIsConnected = true;

	//�񓯊����삪���s���Ă���ꍇ�͋L�^����
	if( ec != ERROR_SUCCESS )
	{
		switch( ec )
		{
		case ERROR_OPERATION_ABORTED:
			break;
		default:
			{
				SetLastError( ec );
				RaiseError( g_ErrorLogger , 0 , L"�񓯊��ڑ����삪���s���܂���" );

				NotifyFunctionOpt opt;
				opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_CONNECT;
				opt.OnError.ErrorCode = ec;
				AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		//�����C�x���g���R�[��
		NotifyFunctionOpt opt;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnConnect , opt );

		//�ǂݎ��o�b�t�@���[
		if( !entry->Parent->PrepareReadBuffer( entry->Parent->MyOption.ReadPacketCount ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�ǂݍ��ݗp�̃o�b�t�@�������ł��܂���ł���" );
		}
	}

	entry->Parent->MyConnectData = nullptr;
	mDelete entry;
}

//�ǂݎ�莞�̊������[�`��
VOID CALLBACK mASyncNamedPipe::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	bool complete_callback = true;

	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	{
		//���̃u���b�N�̓N���e�B�J���Z�N�V����
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//�L���[��������Ԃɂ���
		if( !entry->Completed )
		{
			entry->Completed = true;
			entry->ErrorCode = ec;
			entry->BytesTransfered = len;
		}

		//�L���[�̐擪�ł͂Ȃ��ꍇ�̓R�[���o�b�N���Ă΂Ȃ�
		if( entry->Parent->MyReadQueue.empty() || entry->Parent->MyReadQueue.front() != entry )
		{
			complete_callback = false;
		}
	}

	if( ec != ERROR_SUCCESS )
	{
		switch( ec )
		{
		case ERROR_OPERATION_ABORTED:
			break;
		default:
			{
				SetLastError( ec );
				RaiseAssert( g_ErrorLogger , 0 , L"�񓯊��ǂݍ��ݑ��삪���s���܂���" );

				NotifyFunctionOpt opt;
				opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_READ;
				opt.OnError.ErrorCode = ec;
				AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		if( complete_callback )
		{
			//�L���[�̐擪�̏ꍇ�͊����C�x���g���R�[��
			NotifyFunctionOpt opt;
			AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnRead , opt );
		}
	}
	return;
}

VOID CALLBACK mASyncNamedPipe::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	BufferQueue remove_queue;	//�폜�\��̃L���[
	size_t queue_size = 0;		//�폜��̃L���[�T�C�Y
	{
		//���̃u���b�N�̓N���e�B�J���Z�N�V����
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//�L���[��������Ԃɂ���
		if( !entry->Completed )
		{
			entry->Completed = true;
			entry->ErrorCode = ec;
			entry->BytesTransfered = len;
		}

		//�L���[�̐擪����X�L�������A�����ς݂̃p�P�b�g�������폜
		while( !entry->Parent->MyWriteQueue.empty() )
		{
			if( entry->Parent->MyWriteQueue.front()->Completed )
			{
				remove_queue.push_back( std::move( entry->Parent->MyWriteQueue.front() ) );
				entry->Parent->MyWriteQueue.pop_front();
			}
			else
			{
				break;
			}
		}
		queue_size = entry->Parent->MyWriteQueue.size();
	}

	//�C�x���g�Ăяo��
	if( ec != ERROR_SUCCESS )
	{
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"�񓯊��������ݑ��삪���s���܂���" );

		//�G���[���N���Ă��邩��C�x���g���R�[��
		NotifyFunctionOpt opt;
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_WRITE;
		opt.OnError.ErrorCode = ec;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
	}
	else if( queue_size < entry->Parent->MyOption.WritePacketNotifyCount )
	{
		//�L���[�̃G���g����������������C�x���g���R�[��
		NotifyFunctionOpt opt;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnWrite , opt );
	}

	//�|�C���^�̍폜���s��
	for( BufferQueue::iterator itr = remove_queue.begin() ; itr != remove_queue.end() ; itr++ )
	{
		mDelete (*itr)->Buffer;
		mDelete (*itr);
	}
	return;
}

//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
INT mASyncNamedPipe::Read( void )
{
	//UnRead���ꂽ����������ꍇ�̓\����Ԃ�
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	//�L���b�V���̎c�ʂ�����΃L���b�V����ǂݍ���
	//�L���b�V���̎c�ʂ��Ȃ��Ȃ�L���[����擾����
	if( MyReadCacheRemain == 0 )
	{
		//���̃u���b�N�̓N���e�B�J���Z�N�V����
		mCriticalSectionTicket critical( MyCritical );
		do
		{
			//�ǂݍ��݃L���[���Ȃ��ꍇ��EOF
			if( MyReadQueue.empty() )
			{
				//�ǂݎ��o�b�t�@���[
				PrepareReadBuffer( MyOption.ReadPacketCount );
				return EOF;
			}

			//�ǂݍ��݃L���[�̐擪��IO�������Ȃ�EOF
			if( !MyReadQueue.front()->Completed )
			{
				return EOF;
			}

			//�ǂݍ��݃L���[�̐擪�����o��
			BufferQueueEntry* entry = MyReadQueue.front();
			MyReadQueue.pop_front();

			//�ǂݎ��L���b�V���ɃZ�b�g
			MyReadCacheHead.reset( entry->Buffer );
			MyReadCacheCurrent = 0;
			MyReadCacheRemain = entry->BytesTransfered;
			mDelete entry;

		}while( MyReadCacheRemain == 0 );

		//�ǂݎ��o�b�t�@���[
		if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�ǂݍ��ݗp�̃o�b�t�@�������ł��܂���ł���" );
		}
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

//EOF�ɒB���Ă��邩�𒲂ׂ܂�
bool mASyncNamedPipe::IsEOF( void )const
{
	if( !MyIsEOF )
	{
		return false;
	}
	else if( MyReadCacheRemain )
	{
		return false;
	}
	else
	{
		//���������N���e�B�J���Z�N�V����
		mCriticalSectionTicket critical( MyCritical );
		return MyReadQueue.empty() && ( MyNotifyEventToken.use_count() == 1 );
	}
}

//�������ݑ��̌o�H����܂�
bool mASyncNamedPipe::Close( void )
{
	MyIsClosed = true;
	FlushCache();
	return true;
}

//�ǂݍ��ݑ��̌o�H����܂�
bool mASyncNamedPipe::SetEOF( void )
{
	//���������N���e�B�J���Z�N�V����
	mCriticalSectionTicket critical( MyCritical );
	MyIsEOF = true;

	for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
	{
		if( !(*itr)->Completed )
		{
			CancelIoEx( MyHandle , &(*itr)->Ov );
		}
	}

	return true;
}

//�P������������
bool mASyncNamedPipe::Write( INT data )
{
	//�N���[�Y�ς݁H
	if( MyIsClosed )
	{
		//�������ݍς݂�����G���[�I��
		RaiseError( g_ErrorLogger , 0 , L"�p�C�v�͂��łɕ����Ă��܂�" );
		return false;
	}

	//�����L���b�V�����Ȃ��悤�Ȃ�쐬
	if( MyWriteCacheHead.get() == nullptr )
	{
		MyWriteCacheHead.reset( mNew BYTE[ MyOption.WritePacketSize ] );
		MyWriteCacheRemain = MyOption.WritePacketSize;
		MyWriteCacheWritten = 0;
	}

	//�L���b�V���̎c�肪����H
	if( MyWriteCacheRemain == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�������݃L���b�V���̎c�ʂ�����܂���" );
	}

	//�L���b�V���ɏ������
	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	//�L���b�V�������^���������玩�����M
	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}

	return true;
}

//�L���b�V������������
//������Ă΂Ȃ��Ǝ��ۂ̑��M�͔������܂���
bool mASyncNamedPipe::FlushCache( void )
{
	BufferQueueEntry* entry = nullptr;
	{
		//�N���e�B�J���Z�N�V����
		mCriticalSectionTicket critical( MyCritical );

		//�����ރf�[�^���Ȃ��ꍇ�͂��̂܂ܖ߂�
		if( MyWriteCacheWritten == 0 || MyWriteCacheHead.get() == nullptr )
		{
			//�f�[�^�Ȃ��͐���I������
			return true;
		}

		if( ( MyOption.WritePacketLimit ) &&
			( MyOption.WritePacketLimit < MyWriteQueue.size() ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������ݑ҂��p�P�b�g�̐�������ɓ��B���܂���" );
			return false;
		}

		entry = mNew BufferQueueEntry;
		entry->Parent = this;
		entry->Type = QueueType::WRITE_QUEUE_ENTRY;
		entry->Buffer = MyWriteCacheHead.release();
		entry->Ov.hEvent = 0;
		entry->Ov.Internal = 0;
		entry->Ov.InternalHigh = 0;
		entry->Ov.Offset = 0;
		entry->Ov.OffsetHigh = 0;
		entry->Completed = false;
		entry->ErrorCode = 0;
		entry->BytesTransfered = 0;

		DWORD bytes_to_write = MyWriteCacheWritten;
		MyWriteQueue.push_back( entry );
		MyWriteCacheWritten = 0;
		MyWriteCacheRemain = 0;

		DWORD written = 0;
		if( WriteFile( MyHandle , entry->Buffer , bytes_to_write , &written , &entry->Ov ) )
		{
			return true;
		}

		switch( GetLastError() )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			return true;
		default:
			MyWriteQueue.pop_back();
			break;
		}
	}

	//�������݂Ɏ��s���Ă���̂ł��̃L���[���폜����
	//���f�[�^�͑������Ă���
	mDelete[] entry->Buffer;
	mDelete entry;
	RaiseError( g_ErrorLogger , 0 , L"�������݂̔񓯊����삪�J�n���܂���ł���" );
	return false;
}


//���M�������̃f�[�^�����邩��Ԃ��܂�
DWORD mASyncNamedPipe::IsWriting( void )const
{
	//���̃u���b�N�̓N���e�B�J���Z�N�V����
	mCriticalSectionTicket critical( MyCritical );

	return (DWORD)MyWriteQueue.size();
}

//�ڑ����Ă��邩�ۂ���Ԃ��܂�
bool mASyncNamedPipe::IsConnected( void )const
{
	if( this == nullptr )
	{
		return false;
	}
	return MyIsConnected;
}

//���M�������̃f�[�^��j�����܂�
bool mASyncNamedPipe::Cancel( void )
{
	mCriticalSectionTicket critical( MyCritical );
	MyWriteCacheHead.reset();
	MyWriteCacheWritten = 0;
	MyWriteCacheRemain = 0;

	for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
	{
		if( !(*itr)->Completed )
		{
			CancelIoEx( MyHandle , &(*itr)->Ov );
		}
	}
	return true;
}

//���ݖ������̒ʐM(����M�Ƃ�)��S�Ĕj�����A�ڑ�����܂�
bool mASyncNamedPipe::Abort( void )
{
	if( !IsConnected() && MyConnectData )
	{
		CancelIoEx( MyHandle , &(MyConnectData->Ov) );
	}

	//�������ݏI�����L���[���L�����Z��
	Close();
	Cancel();
	//�ǂݍ��ݏI�����ăL���[���L�����Z��
	SetEOF();

	//�������̃L���[�j��
	DWORD wait_time = 0;
	while( 1 )
	{
		bool empty = true;

		if( MyNotifyEventToken.use_count() == 1 )
		{
			mCriticalSectionTicket critical( MyCritical );
			for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; )
			{
				if( (*itr)->Completed )
				{
					mDelete (*itr)->Buffer;
					mDelete (*itr);
					itr = MyWriteQueue.erase( itr );
					continue;
				}
				itr++;
			}
			for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; )
			{
				if( (*itr)->Completed )
				{
					mDelete (*itr)->Buffer;
					mDelete (*itr);
					itr = MyReadQueue.erase( itr );
					continue;
				}
				itr++;
			}
			MyReadCacheRemain = 0;
			MyReadCacheCurrent = 0;
			MyReadCacheHead.reset();

			empty = MyWriteQueue.empty() && MyReadQueue.empty();
		}
		else
		{
			empty = false;
		}

		if( empty && ( MyNotifyEventToken.use_count() == 1 ) )
		{
			break;
		}
		else
		{
			SleepEx( wait_time , true );
			if( wait_time < 200 )
			{
				wait_time += 10;
			}
		}
	}
	return true;
}


//�����̖��O�t���p�C�v�ɐڑ�����
bool mASyncNamedPipe::Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename )
{
	//��d�ɊJ�����Ƃ��Ă���H
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"�p�C�v���d�ɊJ�����Ƃ��Ă��܂�" , pipename );
		return false;
	}

	//�p�C�v���̍\�z
	WString full_pipe_name;
	sprintf( full_pipe_name ,  LR"(\\%s\pipe\%s)" , servername.c_str() , pipename.c_str() );

	//�n���h���̎擾
	MyHandle = CreateFileW( full_pipe_name.c_str() , GENERIC_READ | GENERIC_WRITE , 0 , nullptr , OPEN_EXISTING , FILE_FLAG_OVERLAPPED , nullptr );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"�n���h�����J���܂���ł���" , full_pipe_name );
		return false;
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�p�C�v�ɐڑ����܂���" , full_pipe_name );
	}

	//�o�^
	if( !Attach( wtp , opt , notifier ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�v�[���ɓo�^�ł��܂���ł���" );
		return false;
	}

	//�ǂݎ��o�b�t�@���[
	if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�ǂݍ��ݗp�̃o�b�t�@�������ł��܂���ł���" );
	}
	return true;

}
