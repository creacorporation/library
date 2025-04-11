//----------------------------------------------------------------------------
// HTTPアクセス
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019-2020 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
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
	//送信キューを捨てて、ハンドルを閉じる
	{
		mCriticalSectionTicket ticket( MyCritical );
		MyRequestStatus = RequestStatus::REQUEST_FINISHED;	//念のためステータスを完了にする

		//送信キュー破棄
		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			mDelete[] itr->Buffer;
		}
		MyWriteQueue.clear();

		//ハンドル破棄
		if( MyRequest )
		{
			WinHttpCloseHandle( MyRequest );
			MyRequest = 0;
		}
	}

	//WinHTTPのアクティブなキューが捌けるのを待つ
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

	//WinHTTPからキューが捌けたので捨てる
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
		RaiseError( g_ErrorLogger , 0 , L"現在ヘッダを追加することはできません" );
		return false;
	}

	//各ヘッダを追加
	if( !WinHttpAddRequestHeaders( MyRequest , header.c_str() , (DWORD)header.size() , WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"HTTPヘッダの追加が失敗しました" );
		return false;
	}
	return true;
}

//１文字書き込み
bool mHttpRequest::Write( INT data )
{
	if( !IsWritable() )
	{
		RaiseError( g_ErrorLogger , 0 , L"書き込みできない状態です" );
		return false;
	}

	//もしキャッシュがないようなら作成
	if( MyWriteCacheHead.get() == nullptr )
	{
		MyWriteCacheHead.reset( mNew BYTE[ MyOption.SendPacketSize ] );
		MyWriteCacheRemain = MyOption.SendPacketSize;
		MyWriteCacheWritten = 0;
	}

	//キャッシュの残りがある？
	if( MyWriteCacheRemain == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"書き込みキャッシュの残量がありません" );
		return false;
	}

	//キャッシュに書込んで
	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	//キャッシュが満タンだったら自動送信
	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}
	return true;
}

//キャッシュを書き込み
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

		return ExecWriteQueue();	//←※クリティカルセクション内で実行
	}
	else
	{
		return ExecWriteQueue();	//←※クリティカルセクション外で実行
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
		//クリティカルセクション
		mCriticalSectionTicket ticket( MyCritical );
		while( !MyWriteQueue.empty() )
		{
			BufferQueue::iterator itr = MyWriteQueue.begin();

			//キュー内の未送信を順次送信する
			if( WinHttpWriteData( MyRequest , itr->Buffer , itr->Size , 0 ) )
			{
				//・通信は開始しただけで完了しているとは限らない
				//・進行中状態にする
				//・後ほどWinHttpからのコールバックがある
				itr->Status = BufferQueueStatus::BUFFERQUEUE_PROCEEDING;
			}
			else
			{
				//失敗
				//・エラー状態にする
				//・WinHttpからのコールバックはないから、自らコールバックを呼び出す
				itr->ErrorCode = GetLastError();
				itr->Status = BufferQueueStatus::BUFFERQUEUE_ERROR;

				RaiseAssert( g_ErrorLogger , 0 , L"送信処理が開始しませんでした" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , (int)info.Mode );
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
				RaiseAssert( g_ErrorLogger , 0 , L"書き込みキューがないのに完了通知を受信しました" );
				return false;
			}

			//転送済みサイズを更新
			BufferQueueEntry* entry = &MyWriteActiveQueue.front();
			DWORD space = entry->Size - entry->BytesTransfered;
			if( written_size <= space )
			{
				//現在のキューの範囲で完了している場合
				entry->BytesTransfered += written_size;
				written_size = 0;
			}
			else
			{
				//次のキューにまたいで完了している場合
				entry->BytesTransfered = entry->Size;
				written_size -= space;					 
			}

			//完了していればキューから削除する
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

//リクエストの送信
bool mHttpRequest::Execute( DWORD AdditionalDataSize )
{
	if( MyRequestStatus != RequestStatus::REQUEST_PREEXEC )
	{
		RaiseError( g_ErrorLogger , 0 , L"二重にリクエストを送信しようとしました" );
		return false;
	}

	//データサイズの算定
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
			RaiseError( g_ErrorLogger , 0 , L"メッセージボディにデータが含まれています" );
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

	//リクエスト送信
	if( !WinHttpSendRequest( MyRequest , WINHTTP_NO_ADDITIONAL_HEADERS , 0 , WINHTTP_NO_REQUEST_DATA , 0 , TotalPostLen , (DWORD_PTR)this ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"リクエストの送信が失敗しました" );
		return false;
	}

	//貯めてあるデータを流す
	if( !FlushCache() )
	{
		RaiseError( g_ErrorLogger , 0 , L"リクエストの送信時にキャッシュを送出できませんでした" );
		return false;
	}
	return true;
}

bool mHttpRequest::CompleteReadQueue( BYTE* buffer , DWORD bytes_received )
{
	mCriticalSectionTicket ticket( MyCritical );

	//受信中のバッファ一覧から、完了したバッファを探すラムダ
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

	//完了したバッファを特定し、ステータスを書き換える
	if( !CompleteQueueEntry( buffer , bytes_received ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"読み取り完了したバッファが見当たりません" );
	}

	//先頭から順にスキャンし、完了していればユーザーに引き渡し可能なデータがある
	//一度でも完了していないキューに当たったら、それ以降のキューはまだ受信中なのでユーザーには渡せない

	bool rc = false;	//ユーザーに引き渡し可能なデータはあったか？
	while( !MyReadActiveQueue.empty() )
	{
		BufferQueueEntry* front = &MyReadActiveQueue.front();
		if( front->Status == BufferQueueStatus::BUFFERQUEUE_COMPLETED )
		{
			//完了している
			if( front->BytesTransfered )
			{
				//有効なデータを持ったバッファ
				MyReadQueue.push_back( *front );
				MyReadActiveQueue.pop_front();
				rc = true;
			}
			else
			{
				//何も読み取らず完了したバッファは捨てる
				mDelete[] front->Buffer;
				MyReadActiveQueue.pop_front();
			}
		}
		else
		{
			//完了していないキューを見つけたから抜ける
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
		RaiseError( g_ErrorLogger , 0 , L"レスポンスの受信が失敗しました" );
		return false;
	}

	MyRequestStatus = RequestStatus::REQUEST_RECEIVING;
	if( !PrepareReadBuffer( MyOption.RecievePacketMaxActive ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"読み取り用のバッファの準備が失敗しました" );
		return false;
	}

	return true;
}

bool mHttpRequest::PrepareReadBuffer( DWORD count )
{
	mCriticalSectionTicket ticket( MyCritical );

	//サイズがすでにMAXならば新たに積まない
	if( count <= MyReadActiveQueue.size() )
	{
		return true;
	}
	//読み取り中でないならば新たに積まない
	if( MyRequestStatus != RequestStatus::REQUEST_RECEIVING )
	{
		return true;
	}
	//MyIsEOFがセットされていたら新たに積まない
	//※mHttpRequestにおいては、MyIsEOFはエラー発生により通信が打ち切られた場合にセットする
	if( MyIsEOF )
	{
		return true;
	}

	//サイズに不足する分だけ積む
	DWORD create_queue_count = count - (DWORD)MyReadActiveQueue.size();
	for( DWORD i = 0 ; i < create_queue_count ; i++ )
	{
		//受信バッファを生成する
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
				//バッファ積み過ぎと思われるためエラーにはしない
				return true;
			}
			//その他はエラー
			RaiseError( g_ErrorLogger , 0 , L"読み取り用のバッファの準備が失敗しました" );
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
		//書き込みが完了した
		//※ある書き込みバッファの送信が完了したという意味
		DWORD bytes_written = *(DWORD*)lpvStatusInformation;
		obj->CompleteWriteQueue( bytes_written );

		NotifyFunctionOpt opt;
		opt.OnSend.BytesSent = bytes_written;
		AsyncEvent( *obj , obj->MyNotifyOption.OnSend , opt );
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE )
	{
		//送信処理が完了した
		//※ある書き込みバッファの送信が完了したという意味ではなく、HTTPリクエスト全体の送信が完了したという意味
		obj->StartReceiveResponse();
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE )
	{
		//データを読み取り可能である
		DWORD bytes_received = *(DWORD*)lpvStatusInformation;
		obj->PrepareReadBuffer( obj->MyOption.RecievePacketMaxActive );
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_READ_COMPLETE )
	{
		//データを読み終わった
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
		//HTTPヘッダが読み取り可能になった
		obj->MyIsResponseHeaderAvailable = true;

		NotifyFunctionOpt opt;
		AsyncEvent( *obj , obj->MyNotifyOption.OnHeaderAvailable , opt );
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_SECURE_FAILURE )
	{
		RaiseError( g_ErrorLogger , 0 , L"SSLエラーが発生しました" , dwInternetStatus );

		DWORD flags = *(DWORD*)lpvStatusInformation;

		NotifyFunctionOpt opt;
		opt.OnSslError.IsCertRevocationFailed = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REV_FAILED );	//証明書のチェックができなかった
		opt.OnSslError.IsInvalidCert = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CERT );				//証明書がニセモノ
		opt.OnSslError.IsCertRevoked = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_REVOKED );				//証明書が失効してる
		opt.OnSslError.IsInvalidCA = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_INVALID_CA );					//オレオレ証明書
		opt.OnSslError.IsInvalidCommonName = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_CN_INVALID );		//証明書とURLが一致してない
		opt.OnSslError.IsInvalidDate = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_CERT_DATE_INVALID );			//証明書が期限切れ
		opt.OnSslError.IsChannelError = ( flags & WINHTTP_CALLBACK_STATUS_FLAG_SECURITY_CHANNEL_ERROR );	//その他よくわからないエラー

		AsyncEvent( *obj , obj->MyNotifyOption.OnSslError , opt );
		obj->MyIsEOF = true;	//mHttpRequestにおいては、MyIsEOFはエラー発生により通信が打ち切られた場合にセットする
	}
	else if( dwInternetStatus == WINHTTP_CALLBACK_STATUS_REQUEST_ERROR )
	{
		RaiseError( g_ErrorLogger , 0 , L"HTTPリクエストエラーが発生しました" , dwInternetStatus );
		WINHTTP_ASYNC_RESULT* result = (WINHTTP_ASYNC_RESULT*)lpvStatusInformation;

		NotifyFunctionOpt opt;
		opt.OnError.Api = result->dwResult;
		opt.OnError.ErrorCode = result->dwError;

		AsyncEvent( *obj , obj->MyNotifyOption.OnError , opt );
		obj->MyIsEOF = true;	//mHttpRequestにおいては、MyIsEOFはエラー発生により通信が打ち切られた場合にセットする
	}
	else
	{
		//その他
		RaiseError( g_ErrorLogger , 0 , L"HTTPアクセス中に不明なエラーが発生しました" , dwInternetStatus );
	}
	return;
}

//１文字（１バイト）読み込みます
INT mHttpRequest::Read( void )
{
	//UnReadされた文字がある場合はソレを返す
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	INT result;
	do
	{
		//キャッシュの残量があればキャッシュを読み込む
		//キャッシュの残量がないならキューから取得する
		if( MyReadCacheRemain == 0 )
		{
			//このブロックはクリティカルセクション
			mCriticalSectionTicket critical( MyCritical );
			do
			{
				//読み込みキューがない場合はEOF
				if( MyReadQueue.empty() )
				{
					//読み取りバッファを補充
					PrepareReadBuffer( MyOption.RecievePacketMaxActive );
					return EOF;
				}

				//読み込みキューの先頭を取り出す
				BufferQueueEntry entry = MyReadQueue.front();
				MyReadQueue.pop_front();

				//読み取りキャッシュにセット
				MyReadCacheHead.reset( entry.Buffer );
				MyReadCacheCurrent = 0;
				MyReadCacheRemain = entry.BytesTransfered;

			} while( MyReadCacheRemain == 0 );
		}

		result = MyReadCacheHead[ MyReadCacheCurrent ];
		MyReadCacheCurrent++;
		MyReadCacheRemain--;

	}while( ProcLFIgnore( result ) );
	return result;
}

//EOFに達しているかを調べます
bool mHttpRequest::IsEOF( void )const
{
	if( ( MyRequestStatus != RequestStatus::REQUEST_COMPLETED ) &&
		( !MyIsEOF ) )	//mHttpRequestにおいては、MyIsEOFはエラー発生により通信が打ち切られた場合にセットする
	{
		return false;
	}
	else if( MyReadCacheRemain )
	{
		return false;
	}
	else
	{
		//ここだけクリティカルセクション
		mCriticalSectionTicket critical( MyCritical );
		return MyReadQueue.empty();
	}
}

mHttpRequest::operator bool() const
{
	return MyRequest;
}

//書き込みの完了
bool mHttpRequest::Close( void )
{
	RaiseAssert( g_ErrorLogger , 0 , L"このメソッドは使用しません" );
	return false;
}

bool mHttpRequest::IsHeaderAvailable( void )const
{
	return MyIsResponseHeaderAvailable;
}

DWORD mHttpRequest::GetStatusCode( void )const
{
	//有効なリクエスト？
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"まだヘッダを受信していません" );
		return 0;
	}

	//ステータスコードを取得
	DWORD code = 0;
	DWORD size = sizeof( DWORD );
	if( !WinHttpQueryHeaders( MyRequest , WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER , WINHTTP_HEADER_NAME_BY_INDEX , &code , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//エラー発生の場合
		RaiseError( g_ErrorLogger , 0 , L"ステータスコードの受信中にエラーが発生しました" );
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

	//ヘッダ受信済みチェック
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"まだヘッダを受信していません" );
		return false;
	}

	//必要なサイズを得る
	DWORD size = 0;
	WinHttpQueryHeaders( MyRequest , flag , WINHTTP_HEADER_NAME_BY_INDEX , 0 , &size , WINHTTP_NO_HEADER_INDEX );
	if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
	{
		RaiseError( g_ErrorLogger , 0 , L"指定のヘッダは取得できません" );
		return false;
	}

	//そのサイズのバッファを確保して、結果取得
	WCHAR* buff = mNew WCHAR[ size / sizeof( WCHAR ) ];
	if( !WinHttpQueryHeaders( MyRequest , flag , WINHTTP_HEADER_NAME_BY_INDEX , buff , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ヘッダの取得が失敗しました" );
	}
	else
	{
		//取得した結果を文字列に変換
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

	//ヘッダ受信済みチェック
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"まだヘッダを受信していません" );
		return false;
	}

	//コードを取得
	DWORD size = sizeof( DWORD );
	if( !WinHttpQueryHeaders( MyRequest , ( flag ) | WINHTTP_QUERY_FLAG_NUMBER , WINHTTP_HEADER_NAME_BY_INDEX , &retHeader , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//エラー発生の場合
		RaiseError( g_ErrorLogger , 0 , L"ヘッダの取得が失敗しました" );
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

	//ヘッダ受信済みチェック
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"まだヘッダを受信していません" );
		return false;
	}

	//コードを取得
	DWORD size = sizeof( uint64_t );
	if( !WinHttpQueryHeaders( MyRequest , ( flag ) | WINHTTP_QUERY_FLAG_NUMBER64 , WINHTTP_HEADER_NAME_BY_INDEX , &retHeader , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//エラー発生の場合
		RaiseError( g_ErrorLogger , 0 , L"ヘッダの取得が失敗しました" );
		return false;
	}
	return true;
}

bool mHttpRequest::QueryHeader( HeaderFlag flag , mDateTime::Timestamp& retHeader )const
{
	retHeader.Clear();

	//ヘッダ受信済みチェック
	if( !MyIsResponseHeaderAvailable )
	{
		RaiseError( g_ErrorLogger , 0 , L"まだヘッダを受信していません" );
		return false;
	}

	SYSTEMTIME systime;
	DWORD size = sizeof( SYSTEMTIME );
	ZeroMemory( &systime , sizeof( systime ) );

	if( !WinHttpQueryHeaders( MyRequest , ( flag ) | WINHTTP_QUERY_FLAG_SYSTEMTIME , WINHTTP_HEADER_NAME_BY_INDEX , &systime , &size , WINHTTP_NO_HEADER_INDEX ) )
	{
		//エラー発生の場合
		RaiseError( g_ErrorLogger , 0 , L"ヘッダの取得が失敗しました" );
		return false;
	}

	retHeader.Year = systime.wYear;
	retHeader.Month = systime.wMonth;
	retHeader.Day = systime.wDay;
	//retHeader.DayOfWeek = systime.wDayOfWeek;	曜日は計算で出しているため不要
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
		RaiseError( g_ErrorLogger , 0 , L"設定が失敗しました" );
		return false;
	}

	//----------------------------------------------
#ifdef WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE
	//証明書の検証ができなかった場合(Win10 Version2004以降)
	length = sizeof( opt.IgnoreCertRevocationFailed );
	if( !WinHttpSetOption( MyRequest , WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE , (LPVOID)&opt.IgnoreCertRevocationFailed , length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"設定が失敗しました" );
		return false;
	}
#endif

	//----------------------------------------------
	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_SECURITY_FLAGS , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の設定を取得できませんでした" );
		return false;
	}

	current &= ~( SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID );
	current |= ( opt.IgnoreInvalidCA ) ? ( SECURITY_FLAG_IGNORE_UNKNOWN_CA ) : ( 0 );
	current |= ( opt.IgnoreInvalidCommonName ) ? ( SECURITY_FLAG_IGNORE_CERT_CN_INVALID ) : ( 0 );
	current |= ( opt.IgnoreInvalidDate ) ? ( SECURITY_FLAG_IGNORE_CERT_DATE_INVALID ) : ( 0 );

	if( !::WinHttpSetOption( MyRequest , WINHTTP_OPTION_SECURITY_FLAGS , (LPVOID)&current , length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"設定が失敗しました" );
		return false;
	}
						
	return true;
}

DWORD mHttpRequest::GetEncryptionKeyLength( void )const
{
	DWORD length;
	DWORD current;

	//そもそも暗号化されているの？
	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_SECURITY_FLAGS , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の設定を取得できませんでした" );
		return 0;
	}
	if( ( current & SECURITY_FLAG_SECURE ) == 0 )
	{
		//暗号化されていないので0
		return 0;
	}

	//暗号化されているからビット数を得る
	length = sizeof( current );
	if( !::WinHttpQueryOption( MyRequest , WINHTTP_OPTION_SECURITY_KEY_BITNESS , (LPVOID)&current , (LPDWORD)&length ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の設定を取得できませんでした" );
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
		RaiseError( g_ErrorLogger , 0 , L"現在の設定を取得できませんでした" );
		return false;
	}
	return current == WINHTTP_PROTOCOL_FLAG_HTTP2;
}



