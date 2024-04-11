//----------------------------------------------------------------------------
// TCPハンドラ
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MTCPCONNECTION_CPP_COMPILING
#include "mTcpConnection.h"
#include "../General/mErrorLogger.h"

#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Normaliz.lib" )

LONG mTcpConnection::MySocketCount = 0;
WSAData mTcpConnection::MyWSAData = { 0 };

mTcpConnection::mTcpConnection()
{
	MySocket = INVALID_SOCKET;
	MyIsConnected = false;
	MyConnectData = nullptr;
	MyNotifyEventToken.reset( New int( 0 ) );
}

mTcpConnection::~mTcpConnection()
{
	{
		//完了関数からこのオブジェクトが呼び出されないようにする
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

/*
	//ハンドル削除
	if( MySocket != INVALID_SOCKET )
	{
		CloseHandle( MySocket );
	}
*/
	return;
}


//ポートを開く
bool mTcpConnection::Attach( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier )
{

	//ワーカースレッドプールに登録する
/*
	if( !wtp.Attach( MySocket , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドプールに登録できませんでした" );
		return false;
	}
*/
	//ファイルを開けたので、通知方法をストック
	MyOption = opt;
	MyNotifyOption = notifier;

	return true;
}

bool mTcpConnection::PrepareReadBuffer( DWORD count )
{
	//クリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	if( MyIsEOF )
	{
		//EOF設定済み
		return true;
	}

	while( MyReadQueue.size() < count )
	{
		BufferQueueEntry* entry = New BufferQueueEntry;
		entry->Parent = this;
		entry->Type = QueueType::READ_QUEUE_ENTRY;
		entry->Buffer = New BYTE[ MyOption.ReadPacketSize ];
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
//		ReadFile( MySocket , entry->Buffer , MyOption.ReadPacketSize , &readsize , &entry->Ov );
		switch( GetLastError() )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"読み込みの非同期操作が開始しませんでした" );
			MyReadQueue.pop_back();
			Delete[] entry->Buffer;
			Delete entry;
			return false;
		}
	}

	return true;
}

static void AsyncEvent( mTcpConnection& pipe , const mTcpConnection::NotifyOption::NotifierInfo& info , const mTcpConnection::NotifyFunctionOpt& opt )
{
	if( info.Mode == mTcpConnection::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mTcpConnection::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&pipe , info.Parameter );
	}
	else if( info.Mode == mTcpConnection::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mTcpConnection::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
				if( !mTcpConnection::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mTcpConnection::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
	}
	else if( info.Mode == mTcpConnection::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
	{
		if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
		{
			SetEvent( info.Notifier.Handle );
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , info.Mode );
	}
}


VOID CALLBACK mTcpConnection::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{

	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	if( ov == nullptr || entry == nullptr )
	{
		return;
	}

	if( !entry->Parent )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"親オブジェクトが消滅しています" , entry->Type );

		Delete[] entry->Buffer;
		Delete entry;
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

//接続完了時の完了ルーチン
VOID CALLBACK mTcpConnection::ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	//キューを完了状態にする
	entry->Completed = true;
	entry->ErrorCode = ec;
	entry->Parent->MyIsConnected = true;

	//非同期操作が失敗している場合は記録する
	if( ec != ERROR_SUCCESS )
	{
		switch( ec )
		{
		case ERROR_OPERATION_ABORTED:
			break;
		default:
			{
				SetLastError( ec );
				RaiseError( g_ErrorLogger , 0 , L"非同期接続操作が失敗しました" );

				NotifyFunctionOpt opt;
				opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_CONNECT;
				opt.OnError.ErrorCode = ec;
				AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		//完了イベントをコール
		NotifyFunctionOpt opt;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnConnect , opt );

		//読み取りバッファを補充
		if( !entry->Parent->PrepareReadBuffer( entry->Parent->MyOption.ReadPacketCount ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
		}
	}

	entry->Parent->MyConnectData = nullptr;
	Delete entry;
}

//読み取り時の完了ルーチン
VOID CALLBACK mTcpConnection::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	bool complete_callback = true;

	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		if( !entry->Completed )
		{
			entry->Completed = true;
			entry->ErrorCode = ec;
			entry->BytesTransfered = len;
		}

		//キューの先頭ではない場合はコールバックを呼ばない
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
				RaiseAssert( g_ErrorLogger , 0 , L"非同期読み込み操作が失敗しました" );

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
			//キューの先頭の場合は完了イベントをコール
			NotifyFunctionOpt opt;
			AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnRead , opt );
		}
	}
	return;
}

VOID CALLBACK mTcpConnection::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	BufferQueue remove_queue;	//削除予定のキュー
	size_t queue_size = 0;		//削除後のキューサイズ
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		if( !entry->Completed )
		{
			entry->Completed = true;
			entry->ErrorCode = ec;
			entry->BytesTransfered = len;
		}

		//キューの先頭からスキャンし、完了済みのパケットを順次削除
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

	//イベント呼び出し
	if( ec != ERROR_SUCCESS )
	{
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"非同期書き込み操作が失敗しました" );

		//エラーが起きているからイベントをコール
		NotifyFunctionOpt opt;
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_WRITE;
		opt.OnError.ErrorCode = ec;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
	}
	else if( queue_size < entry->Parent->MyOption.WritePacketNotifyCount )
	{
		//キューのエントリ数が減ったからイベントをコール
		NotifyFunctionOpt opt;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnWrite , opt );
	}

	//ポインタの削除を行う
	for( BufferQueue::iterator itr = remove_queue.begin() ; itr != remove_queue.end() ; itr++ )
	{
		Delete (*itr)->Buffer;
		Delete (*itr);
	}
	return;
}

//１文字（１バイト）読み込みます
INT mTcpConnection::Read( void )
{
	//UnReadされた文字がある場合はソレを返す
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

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
				PrepareReadBuffer( MyOption.ReadPacketCount );
				return EOF;
			}

			//読み込みキューの先頭がIO未完了ならEOF
			if( !MyReadQueue.front()->Completed )
			{
				return EOF;
			}

			//読み込みキューの先頭を取り出す
			BufferQueueEntry* entry = MyReadQueue.front();
			MyReadQueue.pop_front();

			//読み取りキャッシュにセット
			MyReadCacheHead.reset( entry->Buffer );
			MyReadCacheCurrent = 0;
			MyReadCacheRemain = entry->BytesTransfered;
			Delete entry;

		}while( MyReadCacheRemain == 0 );

		//読み取りバッファを補充
		if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
		}
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

//EOFに達しているかを調べます
bool mTcpConnection::IsEOF( void )const
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
		//ここだけクリティカルセクション
		mCriticalSectionTicket critical( MyCritical );
		return MyReadQueue.empty() && ( MyNotifyEventToken.use_count() == 1 );
	}
}

//書き込み側の経路を閉じます
bool mTcpConnection::Close( void )
{
	MyIsClosed = true;
	FlushCache();
	return true;
}

//読み込み側の経路を閉じます
bool mTcpConnection::SetEOF( void )
{
	//ここだけクリティカルセクション
	mCriticalSectionTicket critical( MyCritical );
	MyIsEOF = true;

	for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
	{
		if( !(*itr)->Completed )
		{
//			CancelIoEx( MySocket , &(*itr)->Ov );
		}
	}

	return true;
}

//１文字書き込み
bool mTcpConnection::Write( INT data )
{
	//クローズ済み？
	if( MyIsClosed )
	{
		//書き込み済みだからエラー終了
		RaiseError( g_ErrorLogger , 0 , L"パイプはすでに閉じられています" );
		return false;
	}

	//もしキャッシュがないようなら作成
	if( MyWriteCacheHead.get() == nullptr )
	{
		MyWriteCacheHead.reset( New BYTE[ MyOption.WritePacketSize ] );
		MyWriteCacheRemain = MyOption.WritePacketSize;
		MyWriteCacheWritten = 0;
	}

	//キャッシュの残りがある？
	if( MyWriteCacheRemain == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"書き込みキャッシュの残量がありません" );
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
//これを呼ばないと実際の送信は発生しません
bool mTcpConnection::FlushCache( void )
{
	BufferQueueEntry* entry = nullptr;
	{
		//クリティカルセクション
		mCriticalSectionTicket critical( MyCritical );

		//書込むデータがない場合はそのまま戻る
		if( MyWriteCacheWritten == 0 || MyWriteCacheHead.get() == nullptr )
		{
			//データなしは正常終了扱い
			return true;
		}

		if( ( MyOption.WritePacketLimit ) &&
			( MyOption.WritePacketLimit < MyWriteQueue.size() ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"書き込み待ちパケットの数が上限に到達しました" );
			return false;
		}

		entry = New BufferQueueEntry;
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
//		if( WriteFile( MySocket , entry->Buffer , bytes_to_write , &written , &entry->Ov ) )
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

	//書き込みに失敗しているのでこのキューを削除する
	//※データは損失している
	Delete[] entry->Buffer;
	Delete entry;
	RaiseError( g_ErrorLogger , 0 , L"書き込みの非同期操作が開始しませんでした" );
	return false;
}


//送信未完了のデータがあるかを返します
DWORD mTcpConnection::IsWriting( void )const
{
	//このブロックはクリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	return (DWORD)MyWriteQueue.size();
}

//接続しているか否かを返します
bool mTcpConnection::IsConnected( void )const
{
	if( this == nullptr )
	{
		return false;
	}
	return MyIsConnected;
}

//送信未完了のデータを破棄します
bool mTcpConnection::Cancel( void )
{
	mCriticalSectionTicket critical( MyCritical );
	MyWriteCacheHead.reset();
	MyWriteCacheWritten = 0;
	MyWriteCacheRemain = 0;

	for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
	{
		if( !(*itr)->Completed )
		{
//			CancelIoEx( MySocket , &(*itr)->Ov );
		}
	}
	return true;
}

//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
bool mTcpConnection::Abort( void )
{
	if( !IsConnected() && MyConnectData )
	{
//		CancelIoEx( MySocket , &(MyConnectData->Ov) );
	}

	//書き込み終了しキューをキャンセル
	Close();
	Cancel();
	//読み込み終了してキューをキャンセル
	SetEOF();

	//未処理のキュー破棄
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
					Delete (*itr)->Buffer;
					Delete (*itr);
					itr = MyWriteQueue.erase( itr );
					continue;
				}
				itr++;
			}
			for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; )
			{
				if( (*itr)->Completed )
				{
					Delete (*itr)->Buffer;
					Delete (*itr);
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

//Winsockの初期化をする
bool mTcpConnection::SetupWinsock( void )
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );

	if( MySocketCount == 0 )
	{
		//Winsockの初期化を行う
		int err = WSAStartup( MAKEWORD( 2 , 2 ) , &MyWSAData );
		if( err )
		{
			//初期化失敗
			RaiseErrorF( g_ErrorLogger , 0 , L"Winsockの初期化が失敗しました" , L"%d" , err );
			return false;
		}
		if( LOBYTE( MyWSAData.wVersion ) != 2 || HIBYTE( MyWSAData.wVersion ) != 2 )
		{
			//初期化失敗
			RaiseErrorF( g_ErrorLogger , 0 , L"Winsockのバージョンが違います" , L"%d" , MyWSAData.wVersion );
			WSACleanup();
			return false;
		}
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsockが初期化されました" );
	}

	MySocketCount++;
	return true;
}

//Winsockのクリーンアップをする
bool mTcpConnection::CleanupWinsock( void )
{
	//クリティカルセクション
	mCriticalSectionTicket cs( g_CriticalSection );

	if( MySocketCount == 0 )
	{
		return true;
	}
	MySocketCount--;
	if( MySocketCount == 0 )
	{
		WSACleanup();
		ZeroMemory( &MyWSAData , sizeof( MyWSAData ) );
		CreateLogEntry( g_ErrorLogger , 0 , L"Winsockがアンロードされました" );
	}
	return true;
}

struct ADDRINFOW_deleter
{
	void operator()( ADDRINFOW** tofree )const
	{
		if( *tofree )
		{
			FreeAddrInfoW( *tofree );
		}
		delete tofree;
	}
};

void mTcpConnection::ConnectTask( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 )
{
	mTcpConnection* p = (mTcpConnection* const)Param2;

	auto DnsResolveErrocCallback = [p]( DWORD errcode )->void
	{
		NotifyFunctionOpt opt;
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_DNSRESOLVE;
		opt.OnError.ErrorCode = errcode;
		AsyncEvent( *p , p->MyNotifyOption.OnError , opt );
	};

	//1.ヒント構造体
	ADDRINFOW hints;
    ZeroMemory( &hints, sizeof( hints ) );
	switch( p->MyOption.IPv6 )
	{
	case ConnectionOption::IPv6Support::IPv6_Auto:
		hints.ai_flags = AI_V4MAPPED;
	    hints.ai_family = AF_INET6;
		break;
	case ConnectionOption::IPv6Support::IPv6_Required:
	    hints.ai_family = AF_INET6;
		break;
	case ConnectionOption::IPv6Support::IPv4_Required:
	    hints.ai_family = AF_INET;
		break;
	default:
		RaiseErrorF( g_ErrorLogger , 0 , L"IPv6の要求が不正です" , L"%d" , p->MyOption.IPv6 );
		DnsResolveErrocCallback( 0 );
		return;
	}
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	//2.クエリ
	std::unique_ptr<ADDRINFOW*,ADDRINFOW_deleter> addrinfo( new ADDRINFOW* );
	int result = GetAddrInfoW( p->MyOption.HostName.c_str() , p->MyOption.Port.c_str() , &hints , addrinfo.get() );
	if( result != 0 )
	{
		//エラーが起きているからイベントをコール
		RaiseErrorF( g_ErrorLogger , 0 , L"ホスト名の取得に失敗しました" , L"%s,%d" , p->MyOption.HostName.c_str() , result );
		DnsResolveErrocCallback( result );
		return;
	}

	//3.ソケット生成
	int af;
	switch( (*addrinfo)->ai_family )
	{
	case AF_INET:	//IPv4
	case AF_INET6:	//IPv6
		af = (*addrinfo)->ai_family;
		break;
	default:
		break;
	}

	p->MySocket = WSASocketW( AF_UNSPEC , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );


	//4.接続
	DWORD local_address_length = sizeof( SOCKADDR );
	SOCKADDR local_address;
	DWORD remote_address_length = sizeof( SOCKADDR );
	SOCKADDR remote_address;
	TIMEVAL timeval;

	timeval.tv_sec = p->MyOption.ConnectTimeout / 1000;
	timeval.tv_usec = ( p->MyOption.ConnectTimeout % 1000 ) * 1000;

	if( !WSAConnectByNameW(
		p->MySocket,
		const_cast<LPWSTR>( p->MyOption.HostName.c_str() ),
		const_cast<LPWSTR>( p->MyOption.Port.c_str() ),
		&local_address_length,
		&local_address,
		&remote_address_length,
		&remote_address,
		&timeval,
		nullptr )
		)
	{
		DWORD ec = WSAGetLastError();
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"接続に失敗しました" );
		return;
	}



	//WTPに登録
	if( !p->Attach( pool , p->MyOption , p->MyNotifyOption ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ワーカースレッドプールに登録できませんでした" );
		return;
	}


//	FreeAddrInfoW( addrinfo );
	return;
}


bool mTcpConnection::Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier )
{
	//二重に開こうとしている？
	if( MySocket != INVALID_SOCKET )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"接続を二重に開こうとしています" , L"%s:%d" , opt.HostName.c_str() , opt.Port );
		return false;
	}

	//設定のコピーをとる
	MyOption = opt;
	MyNotifyOption = notifier;

	//Winsockの初期化をする
	SetupWinsock();

	//アドレスの名前解決
	return wtp.AddTask( ConnectTask , 0 , (DWORD_PTR)this );
}

/*
//既存の名前付きパイプに接続する
bool mTcpConnection::Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename )
{
	//二重に開こうとしている？
	if( MySocket != INVALID_SOCKET )
	{
		RaiseError( g_ErrorLogger , 0 , L"パイプを二重に開こうとしています" , pipename );
		return false;
	}

	//パイプ名の構築
	WString full_pipe_name;
	sprintf( full_pipe_name ,  LR"(\\%s\pipe\%s)" , servername.c_str() , pipename.c_str() );

	//ハンドルの取得
	MySocket = CreateFileW( full_pipe_name.c_str() , GENERIC_READ | GENERIC_WRITE , 0 , nullptr , OPEN_EXISTING , FILE_FLAG_OVERLAPPED , nullptr );
	if( MySocket == INVALID_SOCKET )
	{
		RaiseError( g_ErrorLogger , 0 , L"ハンドルを開けませんでした" , full_pipe_name );
		return false;
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"パイプに接続しました" , full_pipe_name );
	}

	//登録
	if( !Attach( wtp , opt , notifier ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ワーカースレッドプールに登録できませんでした" );
		return false;
	}

	//読み取りバッファを補充
	if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
	}
	return true;

}
*/