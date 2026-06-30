//----------------------------------------------------------------------------
// TCPハンドラ
// Copyright (C) 2026 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MASYNCTCPLISNTENER_CPP_COMPILING
#include "mASyncTcpListener.h"
#include "mWinsockInitializer.h"
#include "../General/mErrorLogger.h"

mASyncTcpListener::mASyncTcpListener()
{
}

mASyncTcpListener::~mASyncTcpListener()
{
	Close();
	return;
}



void AsyncEvent( mASyncTcpListener& pipe , const mASyncTcpListener::NotifyOption::NotifierInfo& info , const mASyncTcpListener::NotifyFunctionOpt& opt )
{
	if( info.Mode == mASyncTcpListener::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mASyncTcpListener::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&pipe , info.Parameter );
	}
	else if( info.Mode == mASyncTcpListener::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mASyncTcpListener::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
				if( !mASyncTcpListener::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mASyncTcpListener::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
	}
	else if( info.Mode == mASyncTcpListener::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
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
	return;
}


VOID CALLBACK mASyncTcpListener::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	AcceptData* entry = CONTAINING_RECORD( ov ,  AcceptData , Ov );
	if( ov == nullptr || entry == nullptr )
	{
		return;
	}

	mASyncTcpListener* me = entry->Parent;
	if( !me )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseError( g_ErrorLogger , 0 , L"TCP" L"親オブジェクトが消滅しています" );
		return;
	}

	//キューを完了状態にする
	{
		mCriticalSectionTicket Ticket( me->MyCritical );
		if( entry->Status == AcceptDataState::Listening )
		{
			entry->Status = AcceptDataState::WaitForAccept;
		}
	}

	std::weak_ptr<NotifyEventToken::element_type> token_ptr = me->MyNotifyEventToken;
	NotifyEventToken token = token_ptr.lock();
	if( !token )
	{
		return;
	}

	me->ConnectCompleteRoutine( ec , len , ov );
	return;
}


//ソケットを閉じます
bool mASyncTcpListener::Close( void )
{
	//新たにtokenを作れないようにする
	NotifyEventToken token = MyNotifyEventToken;
	{
		mCriticalSectionTicket Ticket( MyCritical );
		if( MyAcceptData.Status == AcceptDataState::Listening )
		{
			CancelIoEx( reinterpret_cast<HANDLE>( MyAcceptData.Socket ) , &(MyAcceptData.Ov) );
		}
		MyNotifyEventToken.reset();
	}

	//スレッドプール内からの呼び出しかどうかで目標スレッド数を決める
	long check_thread_count = ( MyWTP->IsPoolMember() ) ? ( 2 ) : ( 1 );

	//未処理のキュー破棄
	DWORD wait_time = 0;
	while( 1 )
	{
		if( token.use_count() <= check_thread_count )
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
	if( MySocket != INVALID_SOCKET )
	{
		closesocket( MySocket );
		MySocket = INVALID_SOCKET;
	}
	return true;
}

int mASyncTcpListener::GetAddressFamily( Version ver )const
{
	switch( ver )
	{
	case Version::IPv4:
		return AF_INET;
		break;
	case Version::IPv6:
		return AF_INET6;
		break;
	default:
		return AF_INET;
		break;
	};
}


bool mASyncTcpListener::Open( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier )
{
	//二重に開こうとしている？
	if( MyWTP )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"TCP" , L"ソケットを二重に開こうとしています %s" , opt.Address.c_str() );
		return false;
	}

	//コールバック用のあれこれを初期化
	MyWTP = &wtp;
	MyNotifyEventToken.reset( mNew int( 0 ) );
	MyOption = opt;
	MyNotifyOption = notifier;

	struct MYADDRINFO
	{
		~MYADDRINFO()
		{
			FreeAddrInfoExW( AddrInfo );
		}
		ADDRINFOEXW* AddrInfo = nullptr;
	};

	//リスンソケットの生成
	{
		MYADDRINFO serv_addr;
		ADDRINFOEXW addr = {0};
		addr.ai_family = GetAddressFamily( opt.Ver );

		INT rc = GetAddrInfoExW( opt.Address.c_str() , nullptr , NS_DNS , nullptr , &addr , &serv_addr.AddrInfo , nullptr , nullptr , nullptr , nullptr );
		if( rc != NO_ERROR )
		{
			//アドレス取得失敗
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"オープンするアドレスを取得できません" );
			Close();
			return false;
		}

		//ソケットの生成
		MySocket = WSASocketW( serv_addr.AddrInfo->ai_family , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
		if( MySocket == INVALID_SOCKET )
		{
			//選択範囲外なのでエラーにする
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
			Close();
			return false;
		}

		//ワーカースレッドプールに登録する
		if( !MyWTP->Attach( reinterpret_cast<HANDLE>( MySocket ) , CompleteRoutine ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ワーカースレッドプールに登録できませんでした" );
			return false;
		}

		//バインド
		switch( serv_addr.AddrInfo->ai_family )
		{
		case AF_INET6:
			reinterpret_cast<sockaddr_in6*>( serv_addr.AddrInfo->ai_addr )->sin6_port = htons( opt.Port );
			break;
		case AF_INET:
		default:
			reinterpret_cast<sockaddr_in*>( serv_addr.AddrInfo->ai_addr )->sin_port = htons( opt.Port );
			break;
		}
		if( bind( MySocket , serv_addr.AddrInfo->ai_addr , serv_addr.AddrInfo->ai_addrlen ) == SOCKET_ERROR )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"バインドが失敗しました" );
			Close();
			return false;
		}

		//リスンさせる
		if( listen( MySocket , opt.Backlog ) == SOCKET_ERROR )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"リスンの開始が失敗しました" );
			Close();
			return false;
		}

		//着信用ソケットを準備
		if( !PrepareAcceptSocket() )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
			Close();
			return false;
		}
	}
	return true;
}

bool mASyncTcpListener::PrepareAcceptSocket( void )
{
	//未処理
	{
		mCriticalSectionTicket Ticket( MyCritical );
		if( MyAcceptData.Status != AcceptDataState::Empty )
		{
			return true;
		}
	}

	//ソケットの生成
	MyAcceptData.Parent = this;
	MyAcceptData.Ov = {0};
	MyAcceptData.Status = AcceptDataState::Empty;
	MyAcceptData.ErrorCode = 0;
	MyAcceptData.BytesTransfered = 0;
	int address_family = GetAddressFamily( MyOption.Ver );
	MyAcceptData.Socket = WSASocketW( address_family , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
	if( MyAcceptData.Socket == INVALID_SOCKET )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
		return false;
	}
	ZeroMemory( MyAcceptData.Buffer , sizeof( MyAcceptData.Buffer ) );

	//ワーカースレッドプールに登録する
	if( !MyWTP->Attach( reinterpret_cast<HANDLE>( MyAcceptData.Socket ) , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ワーカースレッドプールに登録できませんでした" );
		return false;
	}

	//接続
	if( mWinsockInitializer::Get().AcceptEx( MySocket , MyAcceptData.Socket , MyAcceptData.Buffer , 0 , sizeof( sockaddr_storage ) + 16 , sizeof( sockaddr_storage ) + 16 , &MyAcceptData.BytesTransfered , &MyAcceptData.Ov ) )
	{
		//即時完了
	}
	else
	{
		//即時完了以外
		int ec = WSAGetLastError();
		SetLastError( (DWORD)ec );
		switch( ec )
		{
		case ERROR_IO_PENDING:	//接続中
			return true;
		case WSAECONNRESET:		//コネクションリセット
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続がリセットされました" );
			break;
		default:				//その他
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続エラー" );
			break;
		}
	}
	return false;
}

//接続完了時の完了ルーチン
void mASyncTcpListener::ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	AcceptData* entry = CONTAINING_RECORD( ov ,  AcceptData , Ov );

	//キューを完了状態にする
	entry->ErrorCode = ec;

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
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"非同期接続操作が失敗しました" );

			NotifyFunctionOpt opt;
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
	}
	return;
}