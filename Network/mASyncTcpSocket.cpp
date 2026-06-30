//----------------------------------------------------------------------------
// TCPハンドラ
// Copyright (C) 2026 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MASYNCTCPSOCKET_CPP_COMPILING
#include "mASyncTcpSocket.h"
#include "mWinsockInitializer.h"
#include "../General/mErrorLogger.h"

mASyncTcpSocket::mASyncTcpSocket()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyIsConnected = false;
	MyConnectData = nullptr;
	MyWTP = nullptr;
}

mASyncTcpSocket::~mASyncTcpSocket()
{
	{
		//完了関数からこのオブジェクトが呼び出されないようにする
		mCriticalSectionTicket critical( MyCritical );

		if( MyConnectData )
		{	
			MyConnectData->Entry.Parent = nullptr;
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

	Abort();
	return;
}

//ポートを開く
mASyncTcpSocket::mASyncTcpSocket( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier )
{
	//ワーカースレッドプールに登録する
	if( !wtp.Attach( MyHandle , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ワーカースレッドプールに登録できませんでした" );
		return;
	}
	MyWTP = &wtp;

	//コールバック用のトークンを初期化
	MyNotifyEventToken.reset( mNew int( 0 ) );

	//ファイルを開けたので、通知方法をストック
	MyOption = opt;
	MyNotifyOption = notifier;

	return;
}

bool mASyncTcpSocket::PrepareReadBuffer( DWORD count )
{
	//ハンドルが開いているか確認
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	//クリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	if( MyIsEOF )
	{
		//EOF設定済み
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
		if( ReadFile( MyHandle , entry->Buffer , MyOption.ReadPacketSize , &readsize , &entry->Ov ) )
		{
			return true;
		}

		switch( GetLastError() )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"読み込みの非同期操作が開始しませんでした" );
			MyReadQueue.pop_back();
			mDelete[] entry->Buffer;
			mDelete entry;
			return false;
		}
	}

	return true;
}

int AsyncEvent( mASyncTcpSocket& pipe , const mASyncTcpSocket::NotifyOption::NotifierInfo& info , const mASyncTcpSocket::NotifyFunctionOpt& opt )
{
	int result = 0;
	if( info.Mode == mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&pipe , info.Parameter );
	}
	else if( info.Mode == mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mASyncTcpSocket::NotifyOption::EnterNotifyEvent( info ) )
			{
				result = info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
				if( !mASyncTcpSocket::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		result = info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
	}
	else if( info.Mode == mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
	{
		if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
		{
			SetEvent( info.Notifier.Handle );
		}
	}
	else
	{
		RaiseAssertF( g_ErrorLogger , 0 , L"TCP" , L"非同期操作の完了通知方法が不正です %d" , (int)info.Mode );
	}
	return result;
}


VOID CALLBACK mASyncTcpSocket::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{

	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	if( ov == nullptr || entry == nullptr )
	{
		return;
	}

	mASyncTcpSocket* me = entry->Parent;
	if( !me )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseErrorF( g_ErrorLogger , 0 , L"親オブジェクトが消滅しています" , L"Type=%d" , (int)entry->Type );
		mDelete[] entry->Buffer;
		mDelete entry;
		return;
	}

	//キューを完了状態にする
	if( !entry->Completed )
	{
		entry->Completed = true;
	}

	std::weak_ptr<NotifyEventToken::element_type> token_ptr = me->MyNotifyEventToken;
	NotifyEventToken token = token_ptr.lock();
	if( !token || me->MyIsEOF )
	{
		return;
	}

	switch( entry->Type )
	{
	case QueueType::ADDRESS_LOOKUP_ENTRY:
		me->AddressLookupRoutine( ec , len , ov );
		break;
	case QueueType::CONNECT_QUEUE_ENTRY:
		me->ConnectCompleteRoutine( ec , len , ov );
		break;
	case QueueType::READ_QUEUE_ENTRY:
		me->ReadCompleteRoutine( ec , len , ov );
		break;
	case QueueType::WRITE_QUEUE_ENTRY:
		me->WriteCompleteRoutine( ec , len , ov );
		break;
	default:
		break;
	}
	return;
}

//読み取り時の完了ルーチン
void mASyncTcpSocket::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	bool complete_callback;

	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		entry->ErrorCode = ec;
		entry->BytesTransfered = len;

		//キューの先頭ではない場合はコールバックを呼ばない
		//※NOTIFY_CALLBACK_PARALLELのときは、先頭か否かに関係なくコールバックを呼ぶ
		if( MyNotifyOption.OnRead.Mode != NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
		{
			if( MyReadQueue.empty() )
			{
				complete_callback = false;
			}
			else if( 2 < MyNotifyEventToken.use_count() )
			{
				complete_callback = ( MyReadQueue.front() == entry );
			}
			else
			{
				complete_callback = true;
			}
		}
		else
		{
			complete_callback = true;
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
				opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::Read;
				opt.OnError.ErrorCode = ec;
				AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		if( complete_callback )
		{
			//完了イベントをコール
			NotifyFunctionOpt opt;
			AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnRead , opt );
		}
	}
	return;
}

void mASyncTcpSocket::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	BufferQueue remove_queue;	//削除予定のキュー
	size_t queue_size = 0;		//削除後のキューサイズ
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		entry->ErrorCode = ec;
		entry->BytesTransfered = len;

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
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::Write;
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
		mDelete (*itr)->Buffer;
		mDelete (*itr);
	}
	return;
}

//１文字（１バイト）読み込みます
INT mASyncTcpSocket::Read( void )
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
				mDelete entry;

			}while( MyReadCacheRemain == 0 );

			//読み取りバッファを補充
			if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
			}
		}

		result = MyReadCacheHead[ MyReadCacheCurrent ];
		MyReadCacheCurrent++;
		MyReadCacheRemain--;

	}while( ProcLFIgnore( result ) );
	return result;
}

//EOFをセットしているか調べる
bool mASyncTcpSocket::IsEOF( void )const
{
	return MyIsEOF;
}

//書き込み側の経路を閉じます
bool mASyncTcpSocket::Close( void )
{
	MyIsClosed = true;
	FlushCache();
	return true;
}

//読み込み側の経路を閉じます
bool mASyncTcpSocket::SetEOF( void )
{
	//ここだけクリティカルセクション
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

//１文字書き込み
bool mASyncTcpSocket::Write( INT data )
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
		MyWriteCacheHead.reset( mNew BYTE[ MyOption.WritePacketSize ] );
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
bool mASyncTcpSocket::FlushCache( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return false;
	}
	
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
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"書き込み待ちパケットの数が上限に到達しました" );
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

	//書き込みに失敗しているのでこのキューを削除する
	//※データは損失している
	mDelete[] entry->Buffer;
	mDelete entry;
	RaiseError( g_ErrorLogger , 0 , L"TCP" , L"書き込みの非同期操作が開始しませんでした" );
	return false;
}


//送信未完了のデータがあるかを返します
DWORD mASyncTcpSocket::IsWriting( void )const
{
	//このブロックはクリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	return (DWORD)MyWriteQueue.size();
}

//接続しているか否かを返します
bool mASyncTcpSocket::IsConnected( void )const
{
	if( this == nullptr )
	{
		return false;
	}
	return MyIsConnected;
}

//送信未完了のデータを破棄します
bool mASyncTcpSocket::Cancel( void )
{
	mCriticalSectionTicket critical( MyCritical );
	MyWriteCacheHead.reset();
	MyWriteCacheWritten = 0;
	MyWriteCacheRemain = 0;

	//ハンドルが有効であればIOキャンセル
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			if( !(*itr)->Completed )
			{
				CancelIoEx( MyHandle , &(*itr)->Ov );
			}
		}
	}
	return true;
}

//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
bool mASyncTcpSocket::Abort( void )
{
	if( !IsConnected() && MyConnectData )
	{
		CancelIoEx( MyHandle , &(MyConnectData->Entry.Ov) );
	}

	//書き込み終了しキューをキャンセル
	Close();
	Cancel();
	//読み込み終了してキューをキャンセル
	SetEOF();

	//新たにtokenを作れないようにする
	NotifyEventToken token = MyNotifyEventToken;
	MyNotifyEventToken.reset();

	//スレッドプール内からの呼び出しかどうかで目標スレッド数を決める
	long check_thread_count = ( MyWTP->IsPoolMember() ) ? ( 2 ) : ( 1 );

	//未処理のキュー破棄
	DWORD wait_time = 0;
	while( 1 )
	{
		bool empty = true;

		if( token.use_count() <= check_thread_count )
		{
			mCriticalSectionTicket critical( MyCritical );
			auto QueueClear = []( BufferQueue& queue )->void
			{
				for( BufferQueue::iterator itr = queue.begin() ; itr != queue.end() ; )
				{
					if( (*itr)->Completed )
					{
						mDelete (*itr)->Buffer;
						mDelete (*itr);
						itr = queue.erase( itr );
						continue;
					}
					itr++;
				}
			};
			QueueClear( MyWriteQueue );
			QueueClear( MyReadQueue );

			if( MyConnectData )
			{
				MyConnectData->ClearEntry();
				if( MyConnectData->Entry.Completed )
				{
					MyConnectData.reset();
				}
			}

			MyReadCacheRemain = 0;
			MyReadCacheCurrent = 0;
			MyReadCacheHead.reset();

			empty = MyWriteQueue.empty() && MyReadQueue.empty() && !MyConnectData;
		}
		else
		{
			empty = false;
		}

		if( empty && ( token.use_count() <= check_thread_count ) )
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

	//ハンドル廃棄
	CloseHandle( MyHandle );
	MyHandle = INVALID_HANDLE_VALUE;

	if( MySocket != INVALID_SOCKET )
	{
		closesocket( MySocket );
		MySocket = INVALID_SOCKET;
	}
	return true;
}

void mASyncTcpSocket::ConnectData::ClearEntry( void )
{
	if( Entry.Buffer )
	{
		FreeAddrInfoExW( reinterpret_cast<ADDRINFOEXW*>( Entry.Buffer ) );
		Entry.Buffer = nullptr;
	}
}

bool mASyncTcpSocket::Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , const WString& address , uint16_t port )
{
	//二重に開こうとしている？
	if( MyWTP )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"ソケットを二重に開こうとしています %s" , address.c_str() );
		return false;
	}

	//コールバック用のあれこれを初期化
	MyWTP = &wtp;
	MyNotifyEventToken.reset( mNew int( 0 ) );
	MyOption = opt;
	MyNotifyOption = notifier;

	//名前解決を行う
	{
		//タイムアウト
		timeval Timeval = { long( opt.DnsTimeout / 1000 ) , ( opt.DnsTimeout % 1000 ) * 1000 };

		//接続用オーバーラップ構造体
		MyConnectData.reset( mNew ConnectData );
		MyConnectData->Entry.Parent = this;
		MyConnectData->Entry.Type = QueueType::ADDRESS_LOOKUP_ENTRY;
		MyConnectData->Entry.Buffer = nullptr;
		MyConnectData->Entry.Ov.hEvent = 0;
		MyConnectData->Entry.Ov.Internal = 0;
		MyConnectData->Entry.Ov.InternalHigh = 0;
		MyConnectData->Entry.Ov.Offset = 0;
		MyConnectData->Entry.Ov.OffsetHigh = 0;
		MyConnectData->Entry.Completed = false;
		MyConnectData->Entry.ErrorCode = 0;
		MyConnectData->Entry.BytesTransfered = 0;
		MyConnectData->Address = address;
		MyConnectData->Port = port;

		ADDRINFOEXW addr = {0};
		addr.ai_family = AF_UNSPEC;
		/*
		switch( ver )
		{
		case Version::IPv4:
			addr.ai_family = AF_INET;
			break;
		case Version::IPv6:
			addr.ai_family = AF_INET6;
			break;
		case Version::Unspecified:
		default:
			addr.ai_family = AF_UNSPEC;
			break;
		};
		*/

		INT result = GetAddrInfoExW( address.c_str() , nullptr , NS_DNS , nullptr , &addr , reinterpret_cast<ADDRINFOEXW**>( &MyConnectData->Entry.Buffer ) , &Timeval , &MyConnectData->Entry.Ov , CompleteRoutine , nullptr );
		if( result != WSA_IO_PENDING )
		{
			//非同期操作が開始しなかった場合直接コールバックを呼び出す
			//※成功か失敗かごちゃまぜになるので非同期操作にならなかった場合一律に呼び出す
			AddressLookupRoutine( result , 0 , &MyConnectData->Entry.Ov );
		}
	}
	return true;
}

//名前解決時の完了ルーチン
void mASyncTcpSocket::AddressLookupRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	//完了イベント(エラー)をコール
	auto CallErrorEvent = []( BufferQueueEntry& entry , DWORD errorcode )->void
	{
		NotifyFunctionOpt opt;
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::AddressLookup;
		opt.OnError.ErrorCode = errorcode;
		AsyncEvent( *entry.Parent , entry.Parent->MyNotifyOption.OnError , opt );
		return;
	};

	//オーバーラップ構造体の確認
	ConnectData* entry = CONTAINING_RECORD( ov , ConnectData , Entry.Ov );
	if( !MyConnectData || entry != MyConnectData.get() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"TCP" , L"オーバーラップ構造体のアドレスが想定と異なります" );
		CallErrorEvent( entry->Entry , ec );
		return;
	}

	//キューを完了状態にする
	entry->Entry.ErrorCode = ec;
	entry->Entry.BytesTransfered = len;

	//結果のパースをする
	const ADDRINFOEXW* info = reinterpret_cast<const ADDRINFOEXW*>( entry->Entry.Buffer );
	AddressInfo addrinfo;
	while( info )
	{
		AddressInfoEntry entry;
		switch( info->ai_family )
		{
		case AF_INET:
		{
			entry.Version = Version::IPv4;
			entry.Address.v4 = *reinterpret_cast<const sockaddr_in *>( info->ai_addr );
			addrinfo.push_back( std::move( entry ) );
			break;
		}
		case AF_INET6:
		{
			entry.Version = Version::IPv6;
			entry.Address.v6 = *reinterpret_cast<const sockaddr_in6 *>( info->ai_addr );
			addrinfo.push_back( std::move( entry ) );
			break;
		}
		default:
			RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"不明なアドレスファミリ%d" , info->ai_family );
			break;
		}
		info = info->ai_next;
	}

	//コールバック
	NotifyFunctionOpt opt;
	opt.OnAddressLookup.Info = &addrinfo;
	int callback_result = AsyncEvent( *entry->Entry.Parent , entry->Entry.Parent->MyNotifyOption.OnAddressLookup , opt );

	//コールバックの結果確認
	if( callback_result < 0 || addrinfo.size() <= callback_result )
	{
		//選択範囲外なのでエラーにする
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"コールバックが接続先を選択しませんでした" );
		CallErrorEvent( entry->Entry , ec );
		return;
	}

	//ソケットの生成
	{
		int af;
		switch( addrinfo[ callback_result ].Version )
		{
		case Version::IPv4:
			af = AF_INET;
			break;
		case Version::IPv6:
			af = AF_INET6;
			break;
		default:
			RaiseAssert( g_ErrorLogger , 0 , L"TCP" , L"TCPバージョンが想定と異なります" );
			CallErrorEvent( entry->Entry , ec );
			return;
		}
		//ソケットの生成
		MySocket = WSASocketW( af , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
		if( MySocket == INVALID_SOCKET )
		{
			//選択範囲外なのでエラーにする
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
			CallErrorEvent( entry->Entry , WSAGetLastError() );
			return;
		}

		//バインド
		if( addrinfo[ callback_result ].Version == Version::IPv4 )
		{
			sockaddr_in addr = { 0 };
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = INADDR_ANY;
			addr.sin_port = 0;
			if( bind( MySocket , (sockaddr*)&addr , sizeof( addr ) ) == SOCKET_ERROR )
			{
				RaiseError( g_ErrorLogger , 0 , L"TCP" , L"バインドが失敗しました" );
				CallErrorEvent( entry->Entry , WSAGetLastError() );
				return;
			}
		}
		if( addrinfo[ callback_result ].Version == Version::IPv6 )
		{
			sockaddr_in6 addr = { 0 };
			addr.sin6_family = AF_INET6;
			addr.sin6_addr = in6addr_any;
			addr.sin6_port = 0;
			if( bind( MySocket , (sockaddr*)&addr , sizeof( addr ) ) == SOCKET_ERROR )
			{
				RaiseError( g_ErrorLogger , 0 , L"TCP" , L"バインドが失敗しました" );
				CallErrorEvent( entry->Entry , WSAGetLastError() );
				return;
			}
		}

		//ワーカースレッドプールに登録する
		if( !MyWTP->Attach( reinterpret_cast<HANDLE>( MySocket ) , CompleteRoutine ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ワーカースレッドプールに登録できませんでした" );
			CallErrorEvent( entry->Entry , WSAGetLastError() );
			return;
		}
	}
	{
		//接続
		MyConnectData->Entry.Parent = this;
		MyConnectData->Entry.Type = QueueType::CONNECT_QUEUE_ENTRY;
		MyConnectData->Entry.Buffer = nullptr;
		MyConnectData->Entry.Ov.hEvent = 0;
		MyConnectData->Entry.Ov.Internal = 0;
		MyConnectData->Entry.Ov.InternalHigh = 0;
		MyConnectData->Entry.Ov.Offset = 0;
		MyConnectData->Entry.Ov.OffsetHigh = 0;
		MyConnectData->Entry.Completed = false;
		MyConnectData->Entry.ErrorCode = 0;
		MyConnectData->Entry.BytesTransfered = 0;

		if( addrinfo[ callback_result ].Version == Version::IPv4 )
		{
			sockaddr_in addr = { 0 };
			addr.sin_family = AF_INET;
			addr.sin_addr = addrinfo[ callback_result ].Address.v4.sin_addr;
			addr.sin_port = htons( MyConnectData->Port );
			if( !mWinsockInitializer::Get().ConnextEx( MySocket, reinterpret_cast<const sockaddr*>( &addr ) , (int)sizeof( addr ) , nullptr , 0 , nullptr , &MyConnectData->Entry.Ov ) )
			{
				if( WSAGetLastError() != ERROR_IO_PENDING )
				{
					RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続に失敗しました" );
					CallErrorEvent( entry->Entry , WSAGetLastError() );
					return;
				}
			}
		}
		if( addrinfo[ callback_result ].Version == Version::IPv6 )
		{
			sockaddr_in6 addr = { 0 };
			addr.sin6_family = AF_INET6;
			addr.sin6_addr = addrinfo[ callback_result ].Address.v6.sin6_addr;
			addr.sin6_port = htons( MyConnectData->Port );
			if( !mWinsockInitializer::Get().ConnextEx( MySocket, reinterpret_cast<const sockaddr*>( &addr ) , (int)sizeof( addr ) , nullptr , 0 , nullptr , &MyConnectData->Entry.Ov ) )
			{
				if( WSAGetLastError() != ERROR_IO_PENDING )
				{
					RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続に失敗しました" );
					CallErrorEvent( entry->Entry , WSAGetLastError() );
					return;
				}
			}
		}

		//接続が始まったらアドレス解決の結果は不要なので廃棄
		MyConnectData->ClearEntry();
	}
	return;
}

//接続完了時の完了ルーチン
void mASyncTcpSocket::ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	//キューを完了状態にする
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
			opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::Connect;
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

	entry->Parent->MyConnectData.reset();
}