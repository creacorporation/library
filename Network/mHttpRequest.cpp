//----------------------------------------------------------------------------
// HTTP�A�N�Z�X
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019-2020 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mHttpRequest.h"
#include "General/mErrorLogger.h"

mHttpRequest::mHttpRequest()
{
	MyRequest = 0;
	MyRequestStatus = RequestStatus::REQUEST_INITIALIZED;
	MyIsResponseHeaderAvailable = false;
}

mHttpRequest::~mHttpRequest()
{
	//���M�L���[���̂ĂāA�n���h�������
	{
		mCriticalSectionTicket ticket( MyCritical );
		MyRequestStatus = RequestStatus::REQUEST_FINISHED;	//�O�̂��߃X�e�[�^�X�������ɂ���

		//���M�L���[�j��
		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			mDelete[] itr->Buffer;
		}
		MyWriteQueue.clear();

		//�n���h���j��
		if( MyRequest )
		{
			WinHttpCloseHandle( MyRequest );
			MyRequest = 0;
		}
	}

	//WinHTTP�̃A�N�e�B�u�ȃL���[���J����̂�҂�
	DWORD timer = 0;
	while( 1 )
	{
		SleepEx( timer , true );
		{
			mCriticalSectionTicket ticket( MyCritical );
			if( MyWriteActiveQueue.empty() && MyReadActiveQueue.empty() )
			{
				break;
			}
		}
		if( timer < 500 )
		{
			timer += 20;
		}
	}

	//WinHTTP����L���[���J�����̂Ŏ̂Ă�
	{
		for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
		{
			mDelete[] itr->Buffer;
		}
		MyReadQueue.clear();
	}
	return;
}

bool mHttpRequest::SetHeader( const WString& header )
{
	if( MyRequestStatus != RequestStatus::REQUEST_PREEXEC )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݃w�b�_��ǉ����邱�Ƃ͂ł��܂���" );
		return false;
	}

	//�e�w�b�_��ǉ�
	if( !WinHttpAddRequestHeaders( MyRequest , header.c_str() , (DWORD)header.size() , WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"HTTP�w�b�_�̒ǉ������s���܂���" );
		return false;
	}
	return true;
}

//�P������������
bool mHttpRequest::Write( INT data )
{
	if( !IsWritable() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�������݂ł��Ȃ���Ԃł�" );
		return false;
	}

	//�����L���b�V�����Ȃ��悤�Ȃ�쐬
	if( MyWriteCacheHead.get() == nullptr )
	{
		MyWriteCacheHead.reset( mNew BYTE[ MyOption.SendPacketSize ] );
		MyWriteCacheRemain = MyOption.SendPacketSize;
		MyWriteCacheWritten = 0;
	}

	//�L���b�V���̎c�肪����H
	if( MyWriteCacheRemain == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�������݃L���b�V���̎c�ʂ�����܂���" );
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
bool mHttpRequest::FlushCache( void )
{
	if( MyWriteCacheHead )
	{
		mCriticalSectionTicket ticket( MyCritical );

		BufferQueueEntry entry;
		entry.Buffer = MyWriteCacheHead.release();
		entry.Status = BufferQueueStatus::BUFFERQUEUE_CREATED;
		entry.Size = MyWriteCacheWritten;
		entry.BytesTransfered = 0;
		entry.ErrorCode = 0;
		MyWriteQueue.push_back( std::move( entry ) );

		MyWriteCacheRemain = 0;
		MyWriteCacheWritten = 0;	

		return ExecWriteQueue();	//�����N���e�B�J���Z�N�V�������Ŏ��s
	}
	else
	{
		return ExecWriteQueue();	//�����N���e�B�J���Z�N�V�����O�Ŏ��s
	}
}

bool mHttpRequest::ExecWriteQueue( void )
{
	bool error_detected = false;
	if( MyRequestStatus == RequestStatus::REQUEST_PREEXEC )
	{
		return true;
	}
	else
	{
		//�N���e�B�J���Z�N�V����
		mCriticalSectionTicket ticket( MyCritical );
		while( !MyWriteQueue.empty() )
		{
			BufferQueue::iterator itr = MyWriteQueue.begin();

			//�L���[���̖����M���������M����
			if( WinHttpWriteData( MyRequest , itr->Buffer , itr->Size , 0 ) )
			{
				//�E�ʐM�͊J�n���������Ŋ������Ă���Ƃ͌���Ȃ�
				//�E�i�s����Ԃɂ���
				//�E��ق�WinHttp����̃R�[���o�b�N������
				itr->Status = BufferQueueStatus::BUFFERQUEUE_PROCEEDING;
			}
			else
			{
				//���s
				//�E�G���[��Ԃɂ���
				//�EWinHttp����̃R�[���o�b�N�͂Ȃ�����A����R�[���o�b�N���Ăяo��
				itr->ErrorCode = GetLastError();
				itr->Status = BufferQueueStatus::BUFFERQUEUE_ERROR;

				RaiseAssert( g_ErrorLogger , 0 , L"���M�������J�n���܂���ł���" );
				return false;
			}
			MyWriteActiveQueue.push_back( std::move( MyWriteQueue.front() ) );
			MyWriteQueue.pop_front();
		}
	}
	return true;
}

static void AsyncEvent( mHttpRequest& req , const mHttpRequest::NotifyOption::NotifierInfo& info , const mHttpRequest::NotifyFunctionOpt& addval )
{
	if( info.Mode == mHttpRequest::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mHttpRequest::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&req , info.Parameter );
	}
	else if( info.Mode == mHttpRequest::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mHttpRequest::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( req , info.Parameter , addval );
				if( !mHttpRequest::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mHttpRequest::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( req , info.Parameter , addval );
	}
	else if( info.Mode == mHttpRequest::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
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

bool mHttpRequest::CompleteWriteQueue( DWORD written_size )
{
	{
		mCriticalSectionTicket ticket( MyCritical );

		while( written_size )
		{
			if( MyWriteActiveQueue.empty() )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�������݃L���[���Ȃ��̂Ɋ����ʒm����M���܂���" );
				return false;
			}

			//�]���ς݃T�C�Y���X�V
			BufferQueueEntry* entry = &MyWriteActiveQueue.front();
			DWORD space = entry->Size - entry->BytesTransfered;
			if( written_size <= space )
			{
				//���݂̃L���[�͈̔͂Ŋ������Ă���ꍇ
				entry->BytesTransfered += written_size;
				written_size = 0;
			}
			else
			{
				//���̃L���[�ɂ܂����Ŋ������Ă���ꍇ
				entry->BytesTransfered = entry->Size;
				written_size -= space;					 
			}

			//�������Ă���΃L���[����폜����
			if( entry->BytesTransfered == entry->Size )
			{
				entry->Status = BufferQueueStatus::BUFFERQUEUE_COMPLETED;
				mDelete[] entry->Buffer;
				MyWriteActiveQueue.pop_front();
			}
		}
	}
	return true;
}

bool mHttpRequest::IsWritable( void )const
{
	if( ( MyRequestStatus == RequestStatus::REQUEST_PREEXEC ) ||
		( MyRequestStatus == RequestStatus::REQUEST_POSTEXEC ) )
	{
		return true;
	}
	return false;
}

//���N�G�X�g�̑��M
bool mHttpRequest::Execute( DWORD AdditionalDataSize )
{
	if( MyRequestStatus != RequestStatus::REQUEST_PREEXEC )
	{
		RaiseError( g_ErrorLogger , 0 , L"��d�Ƀ��N�G�X�g�𑗐M���悤�Ƃ��܂���" );
		return false;
	}

	//�f�[�^�T�C�Y�̎Z��
	DWORD TotalPostLen = AdditionalDataSize;
	for( BufferQueue::const_iterator itr = MyWriteActiveQueue.begin() ; itr != MyWriteActiveQueue.end() ; itr++ )
	{
		TotalPostLen += itr->Size;
	}
	if( 0 < TotalPostLen )
	{
		switch( MyOption.Verb )
		{
		case RequestVerb::VERB_POST:
		case RequestVerb::VERB_PUT:
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"���b�Z�[�W�{�f�B�Ƀf�[�^���܂܂�Ă��܂�" );
			return false;
		}
	}
	if( AdditionalDataSize )
	{
		MyRequestStatus = RequestStatus::REQUEST_POSTEXEC;
	}
	else
	{
		MyRequestStatus = RequestStatus::REQUEST_CLOSED;
	}

	//���N�G�X�g���M
	if( !WinHttpSendRequest( MyRequest , WINHTTP_NO_ADDITIONAL_HEADERS , 0 , WINHTTP_NO_REQUEST_DATA , 0 , TotalPostLen , (DWORD_PTR)this ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���N�G�X�g�̑��M�����s���܂���" );
		return false;
	}

	//���߂Ă���f�[�^�𗬂�
	if( !FlushCache() )
	{
		RaiseError( g_ErrorLogger , 0 , L"���N�G�X�g�̑��M���ɃL���b�V���𑗏o�ł��܂���ł���" );
		return false;
	}
	return true;
}

bool mHttpRequest::CompleteReadQueue( BYTE* buffer , DWORD bytes_received )
{
	mCriticalSectionTicket ticket( MyCritical );

	//��M���̃o�b�t�@�ꗗ����A���������o�b�t�@��T�������_
	auto CompleteQueueEntry = [this]( BYTE* buffer , DWORD bytes_received ) -> bool
	{
		for( BufferQueue::iterator itr = MyReadActiveQueue.begin() ; itr != MyReadActiveQueue.end() ; itr++ )
		{
			if( itr->Buffer == buffer )
			{
				itr->Status = BufferQueueStatus::BUFFERQUEUE_COMPLETED;
				itr->BytesTransfered = bytes_received;
				return true;
			}
		}
		return false;
	};

	//���������o�b�t�@����肵�A�X�e�[�^�X������������
	if( !CompleteQueueEntry( buffer , bytes_received ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ǂݎ�芮�������o�b�t�@����������܂���" );
	}

	//�擪���珇�ɃX�L�������A�������Ă���΃��[�U�[�Ɉ����n���\�ȃf�[�^������
	//��x�ł��������Ă��Ȃ��L���[�ɓ���������A����ȍ~�̃L���[�͂܂���M���Ȃ̂Ń��[�U�[�ɂ͓n���Ȃ�

	bool rc = false;	//���[�U�[�Ɉ����n���\�ȃf�[�^�͂��������H
	while( !MyReadActiveQueue.empty() )
	{
		BufferQueueEntry* front = &MyReadActiveQueue.front();
		if( front->Status == BufferQueueStatus::BUFFERQUEUE_COMPLETED )
		{
			//�������Ă���
			if( front->BytesTransfered )
			{
				//�L���ȃf�[�^���������o�b�t�@
				MyReadQueue.push_back( *front );
				MyReadActiveQueue.pop_front();
				rc = true;
			}
			else
			{
				//�����ǂݎ�炸���������o�b�t�@�͎̂Ă�
				mDelete[] front->Buffer;
				MyReadActiveQueue.pop_front();
			}
		}
		else
		{
			//�������Ă��Ȃ��L���[�����������甲����
			break;
		}
	}
	return rc;
}

bool mHttpRequest::StartReceiveResponse( void )
{
	mCriticalSectionTicket ticket( MyCritical );

	if( !WinHttpReceiveResponse( MyRequest , nullptr ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���X�|���X�̎�M�����s���܂���" );
		return false;
	}

	MyRequestStatus = RequestStatus::REQUEST_RECEIVING;
	if( !PrepareReadBuffer( MyOption.RecievePacketMaxActive ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ǂݎ��p�̃o�b�t�@�̏��������s���܂���" );
		return false;
	}

	return true;
}

bool mHttpRequest::PrepareReadBuffer( DWORD count )
{
	mCriticalSectionTicket ticket( MyCritical );

	//�T�C�Y�����ł�MAX�Ȃ�ΐV���ɐς܂Ȃ�
	if( count <= MyReadActiveQueue.size() )
	{
		return true;
	}
	//�ǂݎ�蒆�łȂ��Ȃ�ΐV���ɐς܂Ȃ�
	if( MyRequestStatus != RequestStatus::REQUEST_RECEIVING )
	{
		return true;
	}
	//MyIsEOF���Z�b�g����Ă�����V���ɐς܂Ȃ�
	//��mHttpRequest�ɂ����ẮAMyIsEOF�̓G���[�����ɂ��ʐM���ł��؂�ꂽ�ꍇ�ɃZ�b�g����
	if( MyIsEOF )
	{
		return true;
	}

	//�T�C�Y�ɕs�����镪�����ς�
	DWORD create_queue_count = count - (DWORD)MyReadActiveQueue.size();
	for( DWORD i = 0 ; i < create_queue_count ; i++ )
	{
		//��M�o�b�t�@�𐶐�����
		BufferQueueEntry entry;
		entry.Buffer = new BYTE[ MyOption.RecievePacketSize ];
		entry.Size = MyOption.RecievePacketSize;
		entry.BytesTransfered = 0;
		entry.ErrorCode = 0;
		entry.Status = BufferQueueStatus::BUFFERQUEUE_PROCEEDING;
		MyReadActiveQueue.push_back( std::move( entry ) );

		if( !WinHttpReadData( MyRequest , MyReadActiveQueue.back().Buffer , MyReadActiveQueue.back().Size , 0 ) )
		{
			mDelete[] MyReadActiveQueue.back().Buffer;
			MyReadActiveQueue.pop_back();

			if( GetLastError() == ERROR_WINHTTP_INCORRECT_HANDLE_STATE )
			{
				//�o�b�t�@�ς݉߂��Ǝv���邽�߃G���[�ɂ͂��Ȃ�
				return true;
			}
			//���̑��̓G���[
			RaiseError( g_ErrorLogger , 0 , L"�ǂݎ��p�̃o�b�t�@�̏��������s���܂���" );
			return false;
		}
	}
	return true;
}

void __stdcall mHttpRequest::WinhttpStatusCallback(
	IN HINTERNET hInternet ,
	IN DWORD_PTR dwContext ,
	IN DWORD dwInternetStatus ,
	IN LPVOID lpvStatusInformation ,
	IN DWORD dwStatusInformationLength
)
{	
	mHttpRequest* obj = (mHttpRequest*)dwContext;

	if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE )
	{
		//�������݂���������
		//�����鏑�����݃o�b�t�@�̑��M�����������Ƃ����Ӗ�
		DWORD bytes_written = *(DWORD*)lpvStatusInformation;
		obj->CompleteWriteQueue( bytes_written );

		NotifyFunctionOpt opt;
		opt.OnSend.BytesSent = bytes_written;
		AsyncEvent( *obj , obj->MyNotifyOption.OnSend , opt );
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE )
	{
		//���M��������������
		//�����鏑�����݃o�b�t�@�̑��M�����������Ƃ����Ӗ��ł͂Ȃ��AHTTP���N�G�X�g�S�̂̑��M�����������Ƃ����Ӗ�
		obj->StartReceiveResponse();
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE )
	{
		//�f�[�^��ǂݎ��\�ł���
		DWORD bytes_received = *(DWORD*)lpvStatusInformation;
		obj->PrepareReadBuffer( obj->MyOption.RecievePacketMaxActive );
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_READ_COMPLETE )
	{
		//�f�[�^��ǂݏI�����
		DWORD bytes_received = dwStatusInformationLength;
		if( bytes_received == 0 )
		{
			obj->MyRequestStatus = RequestStatus::REQUEST_COMPLETED;
			obj->MyIsEOF = true;
		}
		if( obj->CompleteReadQueue( (BYTE*)lpvStatusInformation , bytes_received ) )
		{
			NotifyFunctionOpt opt;
			opt.OnReceive.BytesReceived = bytes_received;
			AsyncEvent( *obj , obj->MyNotifyOption.OnReceive , opt );
		}
		if( bytes_received != 0 )
		{
			obj->PrepareReadBuffer( obj->MyOption.RecievePacketMaxActive );
		}
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE )
	{
		//HTTP�w�b�_���ǂݎ��\�ɂȂ���
		obj->MyIsResponseHeaderAvailable = true;

		NotifyFunctionOpt opt;
		AsyncEvent( *obj , obj->MyNotifyOption.OnHeaderAvailable , opt );
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_SECURE_FAILURE )
	{
		RaiseError( g_ErrorLogger , 0 , L"SSL�G���[���������܂���" , dwInternetStatus );

		DWORD flags = *(DWORD*)lpvStatusInformation;

		NotifyFunctionOpt opt;
		opt.OnSslError.IsCertRevocationFailed = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED );	//�ؖ����̃`�F�b�N���ł��Ȃ�����
		opt.OnSslError.IsInvalidCert = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CERT );				//�ؖ������j�Z���m
		opt.OnSslError.IsCertRevoked = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REVOKED );				//�ؖ������������Ă�
		opt.OnSslError.IsInvalidCA = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CA );					//�I���I���ؖ���
		opt.OnSslError.IsInvalidCommonName = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_CN_INVALID );		//�ؖ�����URL����v���ĂȂ�
		opt.OnSslError.IsInvalidDate = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_DATE_INVALID );			//�ؖ����������؂�
		opt.OnSslError.IsChannelError = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_SECURITY_CHANNEL_ERROR );	//���̑��悭�킩��Ȃ��G���[

		AsyncEvent( *obj , obj->MyNotifyOption.OnSslError , opt );
		obj->MyIsEOF = true;	//mHttpRequest�ɂ����ẮAMyIsEOF�̓G���[�����ɂ��ʐM���ł��؂�ꂽ�ꍇ�ɃZ�b�g����
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR )
	{
		RaiseError( g_ErrorLogger , 0 , L"HTTP���N�G�X�g�G���[���������܂���" , dwInternetStatus );
		WINHTTP_ASYNC_RESULT* result = (WINHTTP_ASYNC_RESULT*)lpvStatusInformation;

		NotifyFunctionOpt opt;
		opt.OnError.Api = result->dwResult;
		opt.OnError.ErrorCode = result->dwError;

		AsyncEvent( *obj , obj->MyNotifyOption.OnError , opt );
		obj->MyIsEOF = true;	//mHttpRequest�ɂ����ẮAMyIsEOF�̓G���[�����ɂ��ʐM���ł��؂�ꂽ�ꍇ�ɃZ�b�g����
	}
	else
	{
		//���̑�
		RaiseError( g_ErrorLogger , 0 , L"HTTP�A�N�Z�X���ɕs���ȃG���[���������܂���" , dwInternetStatus );
	}
	return;
}

//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
INT mHttpRequest::Read( void )
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
				PrepareReadBuffer( MyOption.RecievePacketMaxActive );
				return EOF;
			}

			//�ǂݍ��݃L���[�̐擪�����o��
			BufferQueueEntry entry = MyReadQueue.front();
			MyReadQueue.pop_front();

			//�ǂݎ��L���b�V���ɃZ�b�g
			MyReadCacheHead.reset( entry.Buffer );
			MyReadCacheCurrent = 0;
			MyReadCacheRemain = entry.BytesTransfered;

		} while( MyReadCacheRemain == 0 );
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

//EOF�ɒB���Ă��邩�𒲂ׂ܂�
bool mHttpRequest::IsEOF( void )const
{
	if( ( MyRequestStatus != RequestStatus::REQUEST_COMPLETED ) &&
		( !MyIsEOF ) )	//mHttpRequest�ɂ����ẮAMyIsEOF�̓G���[�����ɂ��ʐM���ł��؂�ꂽ�ꍇ�ɃZ�b�g����
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
		return MyReadQueue.empty();
	}
}

mHttpRequest::operator bool() const
{
	return MyRequest;
}

//�������݂̊���
bool mHttpRequest::Close( void )
{
	RaiseAssert( g_ErrorLogger , 0 , L"���̃��\�b�h�͎g�p���܂���" );
	return false;
}

bool mHttpRequest::IsHeaderAvailable( void )const
{
	return MyIsResponseHeaderAvailable;
}

DWORD mHttpRequest::GetStatusCode( void )const
{
	//�L���ȃ��N�G�X�g�H
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"�܂��w�b�_����M���Ă��܂���" );
		return 0;
	}

	//�X�e�[�^�X�R�[�h���擾
	DWORD code = 0;
	DWORD size = sizeof( DWORD );
	if( !WinHttpQueryHeaders( MyRequest , WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER , WINHTTP_HEADER_NAME_BY_INDEX , &code , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//�G���[�����̏ꍇ
		RaiseError( g_ErrorLogger , 0 , L"�X�e�[�^�X�R�[�h�̎�M���ɃG���[���������܂���" );
		return 0;
	}
	return code;
}

WString mHttpRequest::QueryHeaderDirect( HeaderFlag flag , const WString& errstr )const
{
	WString retstr;
	if( QueryHeader( flag , retstr ) )
	{
		return retstr;
	}
	return errstr;
}

bool mHttpRequest::QueryHeader( HeaderFlag flag , WString& retHeader )const
{
	retHeader = L"";

	//�w�b�_��M�ς݃`�F�b�N
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"�܂��w�b�_����M���Ă��܂���" );
		return false;
	}

	//�K�v�ȃT�C�Y�𓾂�
	DWORD size = 0;
	WinHttpQueryHeaders( MyRequest , flag , WINHTTP_HEADER_NAME_BY_INDEX , 0 , &size , WINHTTP_NO_HEADER_INDEX );
	if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
	{
		RaiseError( g_ErrorLogger , 0 , L"�w��̃w�b�_�͎擾�ł��܂���" );
		return false;
	}

	//���̃T�C�Y�̃o�b�t�@���m�ۂ��āA���ʎ擾
	WCHAR* buff = mNew WCHAR[ size / sizeof( WCHAR ) ];
	if( !WinHttpQueryHeaders( MyRequest , flag , WINHTTP_HEADER_NAME_BY_INDEX , buff , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�w�b�_�̎擾�����s���܂���" );
	}
	else
	{
		//�擾�������ʂ𕶎���ɕϊ�
		retHeader = buff;
	}

	mDelete[] buff;
	return true;
}

DWORD mHttpRequest::QueryHeaderDirect( HeaderFlag flag , DWORD errval )const
{
	DWORD retval;
	if( QueryHeader( flag , retval ) )
	{
		return retval;
	}
	return errval;
}

bool mHttpRequest::QueryHeader( HeaderFlag flag , DWORD& retHeader )const
{
	retHeader = 0;

	//�w�b�_��M�ς݃`�F�b�N
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"�܂��w�b�_����M���Ă��܂���" );
		return false;
	}

	//�R�[�h���擾
	DWORD size = sizeof( DWORD );
	if( !WinHttpQueryHeaders( MyRequest , ( flag ) | WINHTTP_QUERY_FLAG_NUMBER , WINHTTP_HEADER_NAME_BY_INDEX , &retHeader , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//�G���[�����̏ꍇ
		RaiseError( g_ErrorLogger , 0 , L"�w�b�_�̎擾�����s���܂���" );
		return false;
	}
	return true;
}

uint64_t mHttpRequest::QueryHeaderDirect( HeaderFlag flag , uint64_t errval )const
{
	uint64_t retval;
	if( QueryHeader( flag , retval ) )
	{
		return retval;
	}
	return errval;
}

bool mHttpRequest::QueryHeader( HeaderFlag flag , uint64_t& retHeader )const
{
	retHeader = 0;

	//�w�b�_��M�ς݃`�F�b�N
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"�܂��w�b�_����M���Ă��܂���" );
		return false;
	}

	//�R�[�h���擾
	DWORD size = sizeof( uint64_t );
	if( !WinHttpQueryHeaders( MyRequest , ( flag ) | WINHTTP_QUERY_FLAG_NUMBER64 , WINHTTP_HEADER_NAME_BY_INDEX , &retHeader , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//�G���[�����̏ꍇ
		RaiseError( g_ErrorLogger , 0 , L"�w�b�_�̎擾�����s���܂���" );
		return false;
	}
	return true;
}

bool mHttpRequest::QueryHeader( HeaderFlag flag , mDateTime::Timestamp& retHeader )const
{
	retHeader.Clear();

	//�w�b�_��M�ς݃`�F�b�N
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"�܂��w�b�_����M���Ă��܂���" );
		return false;
	}

	SYSTEMTIME systime;
	DWORD size = sizeof( SYSTEMTIME );
	ZeroMemory( &systime , sizeof( systime ) );

	if( !WinHttpQueryHeaders( MyRequest , ( flag ) | WINHTTP_QUERY_FLAG_SYSTEMTIME , WINHTTP_HEADER_NAME_BY_INDEX , &systime , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//�G���[�����̏ꍇ
		RaiseError( g_ErrorLogger , 0 , L"�w�b�_�̎擾�����s���܂���" );
		return false;
	}

	retHeader.Year = systime.wYear;
	retHeader.Month = systime.wMonth;
	retHeader.Day = systime.wDay;
	//retHeader.DayOfWeek = systime.wDayOfWeek;	�j���͌v�Z�ŏo���Ă��邽�ߕs�v
	retHeader.Hour = systime.wHour;
	retHeader.Minute = systime.wMinute;
	retHeader.Second = systime.wSecond;
	retHeader.Milliseconds = systime.wMilliseconds;
	return true;
}

bool mHttpRequest::SetSslIgnoreErrors( const SslIgnoreErrors opt )
{
	DWORD length;
	DWORD current;

	//----------------------------------------------
	length = sizeof( current );
	current = ( opt.DisableRevocation ) ? ( 0 ) : ( WINHTTP_ENABLE_SSL_REVOCATION );

	if( !::WinHttpSetOption( MyRequest , WINHTTP_OPTION_ENABLE_FEATURE , (LPVOID)&current , length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ݒ肪���s���܂���" );
		return false;
	}

	//----------------------------------------------
#ifdef WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE
	//�ؖ����̌��؂��ł��Ȃ������ꍇ(Win10 Version2004�ȍ~)
	length = sizeof( opt.IgnoreCertRevocationFailed );
	if( !WinHttpSetOption( MyRequest , WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE , (LPVOID)&opt.IgnoreCertRevocationFailed , length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ݒ肪���s���܂���" );
		return false;
	}
#endif

	//----------------------------------------------
	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_SECURITY_FLAGS , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̐ݒ���擾�ł��܂���ł���" );
		return false;
	}

	current &= ~( SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID );
	current |= ( opt.IgnoreInvalidCA ) ? ( SECURITY_FLAG_IGNORE_UNKNOWN_CA ) : ( 0 );
	current |= ( opt.IgnoreInvalidCommonName ) ? ( SECURITY_FLAG_IGNORE_CERT_CN_INVALID ) : ( 0 );
	current |= ( opt.IgnoreInvalidDate ) ? ( SECURITY_FLAG_IGNORE_CERT_DATE_INVALID ) : ( 0 );

	if( !::WinHttpSetOption( MyRequest , WINHTTP_OPTION_SECURITY_FLAGS , (LPVOID)&current , length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ݒ肪���s���܂���" );
		return false;
	}
						
	return true;
}

DWORD mHttpRequest::GetEncryptionKeyLength( void )const
{
	DWORD length;
	DWORD current;

	//���������Í�������Ă���́H
	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_SECURITY_FLAGS , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̐ݒ���擾�ł��܂���ł���" );
		return 0;
	}
	if( ( current & SECURITY_FLAG_SECURE ) == 0 )
	{
		//�Í�������Ă��Ȃ��̂�0
		return 0;
	}

	//�Í�������Ă��邩��r�b�g���𓾂�
	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_SECURITY_KEY_BITNESS , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̐ݒ���擾�ł��܂���ł���" );
		return 0;
	}
	return current;
}


bool mHttpRequest::IsHttp2( void )const
{
	DWORD length;
	DWORD current;

	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_HTTP_PROTOCOL_USED , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̐ݒ���擾�ł��܂���ł���" );
		return false;
	}
	return current == WINHTTP_PROTOCOL_FLAG_HTTP2;
}



