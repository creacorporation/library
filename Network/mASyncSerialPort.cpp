//----------------------------------------------------------------------------
// �V���A���|�[�g�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mASyncSerialPort.h"
#include "../General/mErrorLogger.h"

static bool ComPortSetting( HANDLE handle , const mASyncSerialPort::Option& setting );

mASyncSerialPort::mASyncSerialPort()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyNotifyEventToken.reset( mNew int( 0 ) );
}

mASyncSerialPort::~mASyncSerialPort()
{
	{
		//�����֐����炱�̃I�u�W�F�N�g���Ăяo����Ȃ��悤�ɂ���
		mCriticalSectionTicket critical( MyCritical );

		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
		for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
	}

	//�n���h���p��
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}

	return;
}

//�V���A���|�[�g���J��
bool mASyncSerialPort::Open( mWorkerThreadPool& wtp , const Option& opt , const NotifyOption& notifier )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"��d�Ƀ|�[�g���J�����Ƃ��Ă��܂�" );
		return false;
	}
	//������WinSDK�̒�`�l�ɒu��������
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

	//�t�@�C�����J��
	MyHandle = CreateFileW( opt.Fileinfo.Path.c_str() , access , share , 0 , create_dispo , FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL , 0 );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		//�J���Ȃ�����
		RaiseError( g_ErrorLogger , 0 , L"�|�[�g���J�����Ƃ��ł��܂���ł���" , opt.Fileinfo.Path );
		return false;
	}

	//COM�|�[�g�̐ݒ���s��
	if( !ComPortSetting( MyHandle , opt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�|�[�g�̏����ݒ肪���s���܂���" , opt.Fileinfo.Path );
		return false;
	}

	//���[�J�[�X���b�h�v�[���ɓo�^����
	if( !wtp.Attach( MyHandle , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���[�J�[�X���b�h�v�[���ɓo�^�ł��܂���ł���" );
		return false;
	}

	//�t�@�C�����J�����̂ŁA�ʒm���@���X�g�b�N
	MyOption = opt;
	MyNotifyOption = notifier;

	//�����ǂݍ��݃o�b�t�@��ς�
	if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�ǂݍ��ݗp�̃o�b�t�@�������ł��܂���ł���" );
		return false;
	}
	return true;
}

bool mASyncSerialPort::PrepareReadBuffer( DWORD count )
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

static void AsyncEvent( mASyncSerialPort& pipe , const mASyncSerialPort::NotifyOption::NotifierInfo& info , const mASyncSerialPort::NotifyFunctionOpt& opt )
{
	if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&pipe , info.Parameter );
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mASyncSerialPort::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
				if( !mASyncSerialPort::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
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

VOID CALLBACK mASyncSerialPort::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
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

//�ǂݎ�莞�̊������[�`��
VOID CALLBACK mASyncSerialPort::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
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


VOID CALLBACK mASyncSerialPort::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
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
INT mASyncSerialPort::Read( void )
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
bool mASyncSerialPort::IsEOF( void )const
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
bool mASyncSerialPort::Close( void )
{
	MyIsClosed = true;
	FlushCache();
	return true;
}

//�ǂݍ��ݑ��̌o�H����܂�
bool mASyncSerialPort::SetEOF( void )
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
bool mASyncSerialPort::Write( INT data )
{
	//�N���[�Y�ς݁H
	if( MyIsClosed )
	{
		RaiseError( g_ErrorLogger , 0 , L"COM�|�[�g�͂��łɕ����Ă��܂�" );
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
		return false;
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
bool mASyncSerialPort::FlushCache( void )
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
DWORD mASyncSerialPort::IsWriting( void )const
{
	//���̃u���b�N�̓N���e�B�J���Z�N�V����
	mCriticalSectionTicket critical( MyCritical );

	return (DWORD)MyWriteQueue.size();
}

//���M�������̃f�[�^��j�����܂�
bool mASyncSerialPort::Cancel( void )
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
bool mASyncSerialPort::Abort( void )
{
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

static bool ComPortSetting( HANDLE handle , const mASyncSerialPort::Option& setting )
{
	DCB dcb;

	//���݂̐ݒ���擾
	if( !GetCommState( handle , &dcb ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COM�|�[�g�̌��݂̐ݒ���擾�ł��܂���ł���" );
		return false;
	}

	//�ݒ�̍X�V

	//�p���e�B
	switch( setting.Parity )
	{
	case mASyncSerialPort::ParityType::PARITYTYPE_NOPARITY:
		dcb.Parity = NOPARITY;
		dcb.fParity = false;
		break;
	case mASyncSerialPort::ParityType::PARITYTYPE_EVEN:
		dcb.Parity = EVENPARITY;
		dcb.fParity = true;
		break;
	case mASyncSerialPort::ParityType::PARITYTYPE_ODD:
		dcb.Parity = ODDPARITY;
		dcb.fParity = true;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�p���e�B�ݒ肪�s���ł�" , setting.Parity );
		return false;
	}
	//�X�g�b�v�r�b�g
	switch( setting.StopBit )
	{
	case mASyncSerialPort::StopBitType::STOPBIT_ONE:
		dcb.StopBits = ONESTOPBIT;
		break;
	case mASyncSerialPort::StopBitType::STOPBIT_ONEFIVE:
		dcb.StopBits = ONE5STOPBITS;
		break;
	case mASyncSerialPort::StopBitType::STOPBIT_TWO:
		dcb.StopBits = TWOSTOPBITS;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�X�g�b�v�r�b�g�ݒ肪�s���ł�" , setting.StopBit );
		return false;
	}
	//�{�[���[�g
	dcb.BaudRate = setting.BaudRate;

	//�o�C�g�T�C�Y
	if( 0xffu < setting.ByteSize )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�C�g�T�C�Y�ݒ肪�s���ł�" , setting.ByteSize );
		return false;
	}
	dcb.ByteSize = (BYTE)setting.ByteSize;

	//�ݒ�̓K�p
	if( !SetCommState( handle , &dcb ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COM�|�[�g�̐ݒ���X�V�ł��܂���ł���" );
	}


	COMMTIMEOUTS timeout;
	GetCommTimeouts( handle , &timeout );
	timeout.ReadIntervalTimeout = setting.ReadBufferTimeout;
	timeout.ReadTotalTimeoutConstant = 0;
	timeout.ReadTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = 0;
	timeout.WriteTotalTimeoutMultiplier = 0;
	if( !SetCommTimeouts( handle , &timeout ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COM�|�[�g�̃^�C���A�E�g�ݒ���X�V�ł��܂���ł���" );
	}

	return true;
}
