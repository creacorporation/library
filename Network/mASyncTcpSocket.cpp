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
}

mASyncTcpSocket::~mASyncTcpSocket()
{
	Abort( false );
	return;
}

bool mASyncTcpSocket::PrepareReadBuffer( DWORD count )
{
	//ハンドルが開いているか確認
	if( MySocket == INVALID_SOCKET )
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
		ReadQueueEntry* entry = mNew ReadQueueEntry;
		entry->Parent = this;
		entry->Buffer.buf = mNew CHAR[ MyOption.ReadPacketSize ];
		entry->Buffer.len = MyOption.ReadPacketSize;
		entry->Flags = 0;
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
		if( WSARecv( MySocket , &entry->Buffer , 1 , nullptr , &entry->Flags , &entry->Ov , nullptr ) == ERROR_SUCCESS )
		{
			//即時成功は０
			return true;
		}

		int ec = WSAGetLastError();
		switch( ec )
		{
		case WSA_IO_PENDING:
			break;
		case WSAESHUTDOWN:
			if( !MyIsEOF )
			{
				MyIsEOF = true;
				MyRecvShutdownState = RecvShutdownState::CallbackRequest;
				PostFinCallbackTask();
			}
			break;
		default:
			RaiseError( g_ErrorLogger , ec , L"読み込みの非同期操作が開始しませんでした" );
			MyReadQueue.pop_back();
			mDelete[] entry->Buffer.buf;
			mDelete entry;
			return false;
		}
	}
	return true;
}

//Finコールバックを呼び出すタスクをポストする
void mASyncTcpSocket::PostFinCallbackTask( void )
{
	MyFinCallbackData = mNew FinCallbackData;
	MyFinCallbackData->Parent = this;
	MyWTP->AddTask( GenericCallbackRoutine , 0 , reinterpret_cast<DWORD_PTR>( &MyFinCallbackData->Ov ) );
}

//クローズコールバックを呼び出すタスクをポストする
void mASyncTcpSocket::PostDisconnectCallbackTask( void )
{
	MyCloseCallbackData = mNew DisconnectCallbackData;
	MyCloseCallbackData->Parent = this;
	MyWTP->AddTask( GenericCallbackRoutine , 0 , reinterpret_cast<DWORD_PTR>( &MyCloseCallbackData->Ov ) );
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

bool mASyncTcpSocket::GenericCallbackRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 )
{
	CompleteRoutine( 0 , 0 , reinterpret_cast<LPWSAOVERLAPPED>( Param2 ) );
	return false;
}

VOID CALLBACK mASyncTcpSocket::CompleteRoutine( DWORD ec , DWORD len , LPWSAOVERLAPPED ov )
{
	if( ov == nullptr )
	{
		return;
	}
	ASyncDataBase* baseptr = CONTAINING_RECORD( ov ,  ASyncDataBase , Ov );
	mASyncTcpSocket* me = baseptr->Parent;

	if( !me )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"親オブジェクトが消滅しています Type=%d" , (int)baseptr->Type );

		if( baseptr->Type == QueueType::CONNECT_QUEUE_ENTRY )
		{
			ConnectData* entry = CONTAINING_RECORD( ov ,  ConnectData , Ov );
			mDelete entry;
		}
		else if( baseptr->Type == QueueType::READ_QUEUE_ENTRY )
		{
			ReadQueueEntry* entry = CONTAINING_RECORD( ov ,  ReadQueueEntry , Ov );
			mDelete[] entry->Buffer.buf;
			mDelete entry;
		}
		else if( baseptr->Type == QueueType::WRITE_QUEUE_ENTRY )
		{
			WriteQueueEntry* entry = CONTAINING_RECORD( ov ,  WriteQueueEntry , Ov );
			mDelete[] entry->Buffer.buf;
			mDelete entry;
		}
		else if( baseptr->Type == QueueType::FIN_CALLBACK_ENTRY )
		{
			FinCallbackData* entry = CONTAINING_RECORD( ov ,  FinCallbackData , Ov );
			mDelete entry;
		}
		else if( baseptr->Type == QueueType::DISCONNECT_CALLBACK_ENTRY )
		{
			DisconnectCallbackData* entry = CONTAINING_RECORD( ov ,  DisconnectCallbackData , Ov );
			mDelete entry;
		}
		else
		{
			;
		}
		return;
	}

	//コールバックの実行権利を取る
	std::weak_ptr<NotifyEventToken::element_type> token_ptr = me->MyNotifyEventToken;
	NotifyEventToken token = token_ptr.lock();

	//コールバックの呼び出し
	if( baseptr->Type == QueueType::READ_QUEUE_ENTRY )
	{
		ReadQueueEntry* entry = CONTAINING_RECORD( ov ,  ReadQueueEntry , Ov );
		entry->Completed = true;
		if( token )
		{
			me->ReadCompleteRoutine( ec , len , ov );
		}
	}
	else if( baseptr->Type == QueueType::WRITE_QUEUE_ENTRY )
	{
		WriteQueueEntry* entry = CONTAINING_RECORD( ov ,  WriteQueueEntry , Ov );
		entry->Completed = true;
		if( token )
		{
			me->WriteCompleteRoutine( ec , len , ov );
		}
	}
	else if( baseptr->Type == QueueType::CONNECT_QUEUE_ENTRY )
	{
		ConnectData* entry = CONTAINING_RECORD( ov ,  ConnectData , Ov );
		if( token )
		{
			me->ConnectCompleteRoutine( ec , *entry );
			me->MyConnectData = nullptr;
		}
		mDelete entry;
	}
	else if( baseptr->Type == QueueType::FIN_CALLBACK_ENTRY )
	{
		FinCallbackData* entry = CONTAINING_RECORD( ov ,  FinCallbackData , Ov );
		mDelete entry;
		if( token )
		{
			me->FinCompleteRoutine( ec );
		}
	}
	else if( baseptr->Type == QueueType::DISCONNECT_CALLBACK_ENTRY )
	{
		DisconnectCallbackData* entry = CONTAINING_RECORD( ov ,  DisconnectCallbackData , Ov );
		mDelete entry;
		if( token )
		{
			DisconnectCompleteRoutine( *me , ec );
		}
	}
	else
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"不正なキュータイプです Type=%d" , (int)baseptr->Type );
	}
	return;
}

void mASyncTcpSocket::AddressLookupCompleteRoutine( DWORD ec , DWORD len , LPWSAOVERLAPPED ov )
{
	ASyncDataBase* baseptr = CONTAINING_RECORD( ov ,  ASyncDataBase , Ov );
	if( ov == nullptr || baseptr->Type != QueueType::NAME_RESOLV_QUEUE_ENTRY )
	{
		//想定と違う
		return;
	}

	NameResolveData* entry = CONTAINING_RECORD( ov ,  NameResolveData , Ov );
	mASyncTcpSocket* me = entry->Parent;
	if( !me )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"親オブジェクトが消滅しています" );
		FreeAddrInfoExW( entry->Info );
		mDelete entry;
		return;
	}

	std::weak_ptr<NotifyEventToken::element_type> token_ptr = me->MyNotifyEventToken;
	NotifyEventToken token = token_ptr.lock();
	if( token )
	{
		me->AddressLookupRoutine( ec , *entry );
	}

	me->MyNameResolveData = nullptr;
	FreeAddrInfoExW( entry->Info );
	mDelete entry;
	return;
}

//読み取り時の完了ルーチン
void mASyncTcpSocket::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	bool complete_callback;

	ReadQueueEntry* entry = CONTAINING_RECORD( ov ,  ReadQueueEntry , Ov );
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		entry->ErrorCode = ec;
		entry->BytesTransfered = len;

		if( len == 0 )
		{
			//FIN受信時
			if( !MyIsEOF )
			{
				MyIsEOF = true;
				MyRecvShutdownState = RecvShutdownState::CallbackRequest;
				complete_callback = true;
			}
			else
			{
				complete_callback = false;
			}
		}
		else
		{
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
				AsyncEvent( *this , MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		if( complete_callback )
		{
			//完了イベントをコール
			NotifyFunctionOpt opt;
			if( len )
			{
				AsyncEvent( *this , MyNotifyOption.OnRead , opt );
			}
			else
			{
				PostFinCallbackTask();
			}
		}
	}
	return;
}

void mASyncTcpSocket::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	WriteQueueEntry* entry = CONTAINING_RECORD( ov ,  WriteQueueEntry , Ov );

	WriteQueue remove_queue;	//削除予定のキュー
	size_t queue_size = 0;		//削除後のキューサイズ
	SendShutdownState state;

	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( MyCritical );

		//キューを完了状態にする
		entry->ErrorCode = ec;
		entry->BytesTransfered = len;

		//キューの先頭からスキャンし、完了済みのパケットを順次削除
		while( !MyWriteQueue.empty() )
		{
			if( MyWriteQueue.front()->Completed )
			{
				remove_queue.push_back( std::move( MyWriteQueue.front() ) );
				MyWriteQueue.pop_front();
			}
			else
			{
				break;
			}
		}
		queue_size = MyWriteQueue.size();
		state = MySendShutdownState;
	}

	//ポインタの削除を行う
	for( WriteQueue::iterator itr = remove_queue.begin() ; itr != remove_queue.end() ; ++itr )
	{
		mDelete[] (*itr)->Buffer.buf;
		mDelete (*itr);
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
		AsyncEvent( *this , MyNotifyOption.OnError , opt );
	}
	
	if( state == SendShutdownState::Open )
	{
		if( queue_size < MyOption.WritePacketNotifyCount )
		{
			//キューのエントリ数が減ったからイベントをコール
			NotifyFunctionOpt opt;
			AsyncEvent( *this , MyNotifyOption.OnWrite , opt );
		}
	}
	else if( state == SendShutdownState::CloseRequest )
	{
		//シャットダウン
		if( queue_size == 0 )
		{
			if( shutdown( MySocket , SD_SEND ) != 0 )
			{
				SetLastError( WSAGetLastError() );
				RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットのシャットダウンが失敗" );
			}
			MySendShutdownState = SendShutdownState::Closed;
			DisconnectCheck();
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"TCP" , L"クローズ状態でパケットが完了しました" );
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
				MyReadCacheHead.reset( reinterpret_cast<BYTE*>( entry->Buffer.buf ) );
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
	//すでにクローズされているか確認
	if( !IsOpen() )
	{
		return true;
	}

	//クローズフラグを立て、キャッシュの送信をしてなおキューが空か確認
	{
		mCriticalSectionTicket critical( MyCritical );
		FlushCache();

		//キューが空ならシャットダウンをする。空でないならコールバック内でシャットダウンをする。
		if( MyWriteQueue.empty() )
		{
			if( shutdown( MySocket , SD_SEND ) != 0 )
			{
				SetLastError( WSAGetLastError() );
				RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットのシャットダウンが失敗" );
				return false;
			}
			MySendShutdownState = SendShutdownState::Closed;
			DisconnectCheck();
		}
		else
		{
			MySendShutdownState = SendShutdownState::CloseRequest;
		}
	}
	return true;
}

bool mASyncTcpSocket::IsOpen( void )const
{
	return MySendShutdownState == SendShutdownState::Open;
}

//１文字書き込み
bool mASyncTcpSocket::Write( INT data )
{
	//クローズ済み？
	if( !IsOpen() )
	{
		//書き込み済みだからエラー終了（あふれるのでログは出さない）
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
	if( MySocket == INVALID_SOCKET )
	{
		return false;
	}
	
	WriteQueueEntry* entry = nullptr;
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

		entry = mNew WriteQueueEntry;
		entry->Parent = this;
		entry->Buffer.buf = reinterpret_cast<CHAR*>( MyWriteCacheHead.release() );
		entry->Buffer.len = MyWriteCacheWritten;
		entry->Flags = 0;
		entry->Ov.hEvent = 0;
		entry->Ov.Internal = 0;
		entry->Ov.InternalHigh = 0;
		entry->Ov.Offset = 0;
		entry->Ov.OffsetHigh = 0;
		entry->Completed = false;
		entry->ErrorCode = 0;
		entry->BytesTransfered = 0;

		MyWriteQueue.push_back( entry );
		MyWriteCacheWritten = 0;
		MyWriteCacheRemain = 0;

		DWORD written = 0;
		if( WSASend( MySocket , &entry->Buffer , 1 , nullptr , entry->Flags , &entry->Ov , nullptr ) == ERROR_SUCCESS )
		{
			return true;
		}

		int ec = WSAGetLastError();
		switch( ec )
		{
		case WSA_IO_PENDING:
			return true;
		default:
			RaiseError( g_ErrorLogger , ec , L"TCP" , L"送信処理が失敗しました" );
			MyWriteQueue.pop_back();
			break;
		}
	}

	//書き込みに失敗しているのでこのキューを削除する
	//※データは損失している
	mDelete[] entry->Buffer.buf;
	mDelete entry;
	RaiseError( g_ErrorLogger , 0 , L"TCP" , L"書き込みの非同期操作が開始しませんでした" );
	return false;
}

bool mASyncTcpSocket::Abort( bool disconnect_notify_request )
{
	//キューの破棄
	if( MySocket != INVALID_SOCKET )
	{
		//完了関数からこのオブジェクトが呼び出されないようにする
		mCriticalSectionTicket critical( MyCritical );

		if( MyNameResolveData )
		{	
			MyNameResolveData->Parent = nullptr;
		}
		for( WriteQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
		for( ReadQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
		if( MyFinCallbackData )
		{
			MyFinCallbackData->Parent = nullptr;
		}
		if( MyCloseCallbackData )
		{
			MyCloseCallbackData->Parent = nullptr;
		}

		//ハンドル廃棄
		closesocket( MySocket );
		MySocket = INVALID_SOCKET;
		if( disconnect_notify_request )
		{
			PostDisconnectCallbackTask();
		}
	}
	return true;
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
		MyNameResolveData = mNew NameResolveData();
		MyNameResolveData->Parent = this;
		MyNameResolveData->Info = nullptr;
		MyNameResolveData->Ov.hEvent = 0;
		MyNameResolveData->Ov.Internal = 0;
		MyNameResolveData->Ov.InternalHigh = 0;
		MyNameResolveData->Ov.Offset = 0;
		MyNameResolveData->Ov.OffsetHigh = 0;
		MyNameResolveData->Address = address;
		MyNameResolveData->Port = port;

		ADDRINFOEXW addr = {0};
		addr.ai_family = AF_UNSPEC;

		INT result = GetAddrInfoExW( address.c_str() , nullptr , NS_DNS , nullptr , &addr , &MyNameResolveData->Info , &Timeval , &MyNameResolveData->Ov , AddressLookupCompleteRoutine , nullptr );
		if( result != WSA_IO_PENDING )
		{
			//非同期操作が開始しなかった場合直接コールバックを呼び出す
			//※成功か失敗かごちゃまぜになるので非同期操作にならなかった場合一律に呼び出す
			AddressLookupRoutine( result , *MyNameResolveData );

			FreeAddrInfoExW( MyNameResolveData->Info );
			mDelete MyNameResolveData;
			MyNameResolveData = nullptr;
		}
	}
	return true;
}

bool mASyncTcpSocket::Connect( mASyncTcpListener& listener , const ConnectionOption& opt , const NotifyOption& notifier )
{
	//二重に開こうとしている？
	if( MyWTP )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"ソケットを二重に開こうとしています" );
		return false;
	}

	//リスナーオブジェクトのリファレンス
	mASyncTcpListener::TcpSocketInterface ref( listener );

	//ソケットを取得
	if( !ref.GetNewSocket( MySocket ) )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"受信待機が失敗しました" );
	}
	if( MySocket == INVALID_SOCKET )
	{
		return false;
	}

	//コールバック用のあれこれを初期化
	MyWTP = ref.GetWTP();
	if( !MyWTP )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"リスナーのWTPは無効です" );
		return false;
	}
	MyNotifyEventToken.reset( mNew int( 0 ) );
	MyOption = opt;
	MyNotifyOption = notifier;

	//ワーカースレッドプールに登録する
	if( !MyWTP->Attach( reinterpret_cast<HANDLE>( MySocket ) , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ワーカースレッドプールに登録できませんでした" );
		return false;
	}
	
	return PrepareReadBuffer( MyOption.ReadPacketCount );
}


//名前解決時の完了ルーチン
void mASyncTcpSocket::AddressLookupRoutine( DWORD ec , NameResolveData& entry )
{
	//完了イベント(エラー)をコール
	auto CallErrorEvent = [this]( DWORD errorcode )->void
	{
		NotifyFunctionOpt opt;
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::AddressLookup;
		opt.OnError.ErrorCode = errorcode;
		AsyncEvent( *this , MyNotifyOption.OnError , opt );
		return;
	};

	//結果のパースをする
	const ADDRINFOEXW* info = MyNameResolveData->Info;
	AddressInfo addrinfo;
	while( info )
	{
		AddressInfoEntry addrinfo_entry;
		switch( info->ai_family )
		{
		case AF_INET:
			addrinfo_entry = AddressInfoEntry( *reinterpret_cast<const sockaddr_in*>( info->ai_addr ) );
			addrinfo_entry.Port = entry.Port;
			addrinfo.push_back( addrinfo_entry );
			break;
		case AF_INET6:
			addrinfo_entry = AddressInfoEntry( *reinterpret_cast<const sockaddr_in6*>( info->ai_addr ) );
			addrinfo_entry.Port = entry.Port;
			addrinfo.push_back( addrinfo_entry );
			break;
		default:
			RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"不明なアドレスファミリ%d" , info->ai_family );
			break;
		}
		info = info->ai_next;
	}

	//コールバック
	NotifyFunctionOpt opt;
	opt.OnAddressLookup.Info = &addrinfo;
	int callback_result = AsyncEvent( *this , MyNotifyOption.OnAddressLookup , opt );

	//コールバックの結果確認
	if( callback_result < 0 || addrinfo.size() <= callback_result )
	{
		//選択範囲外なのでエラーにする
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"コールバックが接続先を選択しませんでした" );
		CallErrorEvent( ec );
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
			CallErrorEvent( ec );
			return;
		}
		//ソケットの生成
		MySocket = WSASocketW( af , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
		if( MySocket == INVALID_SOCKET )
		{
			//選択範囲外なのでエラーにする
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
			CallErrorEvent( WSAGetLastError() );
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
				CallErrorEvent( WSAGetLastError() );
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
				CallErrorEvent( WSAGetLastError() );
				return;
			}
		}

		//ワーカースレッドプールに登録する
		if( !MyWTP->Attach( reinterpret_cast<HANDLE>( MySocket ) , CompleteRoutine ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ワーカースレッドプールに登録できませんでした" );
			CallErrorEvent( WSAGetLastError() );
			return;
		}
	}
	{
		//接続
		MyConnectData = mNew ConnectData();
		MyConnectData->Parent = this;
		MyConnectData->Ov.hEvent = 0;
		MyConnectData->Ov.Internal = 0;
		MyConnectData->Ov.InternalHigh = 0;
		MyConnectData->Ov.Offset = 0;
		MyConnectData->Ov.OffsetHigh = 0;
		MyConnectData->Remote = addrinfo[ callback_result ];

		if( addrinfo[ callback_result ].Version == Version::IPv4 )
		{
			sockaddr_in addr = sockaddr_in( addrinfo[ callback_result ] );
			if( !mWinsockInitializer::Get().ConnextEx( MySocket, reinterpret_cast<const sockaddr*>( &addr ) , (int)sizeof( addr ) , nullptr , 0 , nullptr , &MyConnectData->Ov ) )
			{
				if( WSAGetLastError() != ERROR_IO_PENDING )
				{
					RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続に失敗しました" );
					CallErrorEvent( WSAGetLastError() );
					return;
				}
			}
		}
		if( addrinfo[ callback_result ].Version == Version::IPv6 )
		{
			sockaddr_in6 addr = sockaddr_in6( addrinfo[ callback_result ] );
			if( !mWinsockInitializer::Get().ConnextEx( MySocket, reinterpret_cast<const sockaddr*>( &addr ) , (int)sizeof( addr ) , nullptr , 0 , nullptr , &MyConnectData->Ov ) )
			{
				if( WSAGetLastError() != ERROR_IO_PENDING )
				{
					RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続に失敗しました" );
					CallErrorEvent( WSAGetLastError() );
					return;
				}
			}
		}
	}
	return;
}

//接続完了時の完了ルーチン
void mASyncTcpSocket::ConnectCompleteRoutine( DWORD ec , ConnectData& entry )
{
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
			AsyncEvent( *this , MyNotifyOption.OnError , opt );
		}
		}
	}
	else
	{
		if( setsockopt( MySocket , SOL_SOCKET , SO_UPDATE_CONNECT_CONTEXT , nullptr , 0 ) != 0 )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケット設定エラー" );
		}

		//完了イベントをコール
		AddressInfoEntry Local;
		{
			sockaddr_storage in;
			int len = sizeof( in );
			if( getsockname( MySocket , reinterpret_cast<sockaddr*>( &in ) , &len ) == 0 )
			{
				switch( in.ss_family )
				{
				case AF_INET:
					Local = AddressInfoEntry( *reinterpret_cast<const sockaddr_in*>( &in ) );
					break;
				case AF_INET6:
					Local = AddressInfoEntry( *reinterpret_cast<const sockaddr_in6*>( &in ) );
					break;
				default:
					break;
				}
			}
		}
		NotifyFunctionOpt opt;
		opt.OnConnect.Local = &Local;
		opt.OnConnect.Remote = &entry.Remote;
		AsyncEvent( *this , MyNotifyOption.OnConnect , opt );

		//読み取りバッファを補充
		if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
		}
	}
}

void mASyncTcpSocket::FinCompleteRoutine( DWORD ec )
{
	NotifyFunctionOpt opt;
	AsyncEvent( *this , MyNotifyOption.OnFin , opt );
	{
		mCriticalSectionTicket ticket( MyCritical );
		MyRecvShutdownState = RecvShutdownState::Closed;
		DisconnectCheck();
	}
}

void mASyncTcpSocket::DisconnectCompleteRoutine( mASyncTcpSocket& obj , DWORD ec )
{
	NotifyFunctionOpt opt;
	AsyncEvent( obj , obj.MyNotifyOption.OnDisconnect , opt );
}

void mASyncTcpSocket::DisconnectCheck( void )
{
	mCriticalSectionTicket ticket( MyCritical );
	if( MyRecvShutdownState == RecvShutdownState::Closed &&
		MySendShutdownState == SendShutdownState::Closed )
	{
		PostDisconnectCallbackTask();
	}
}
