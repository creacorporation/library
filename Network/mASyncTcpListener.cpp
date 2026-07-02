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
		WString listen_addr = ( opt.Address != L"" ) ? ( opt.Address ) : ( L"::" );

		//アドレス→IP変換
		if( GetAddrInfoExW( listen_addr.c_str() , nullptr , NS_DNS , nullptr , nullptr , &serv_addr.AddrInfo , nullptr , nullptr , nullptr , nullptr ) != 0 )
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
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
			Close();
			return false;
		}

		//デュアルスタックの有効化
		if( opt.Address == L"" )
		{
			int optval = 0; //0=デュアルスタック有効
			if( setsockopt( MySocket , IPPROTO_IPV6 , IPV6_V6ONLY , (char*)&optval , sizeof( optval ) ) != 0 )
			{
				RaiseError( g_ErrorLogger , 0 , L"TCP" , L"デュアルスタックの設定が失敗しました" );
				Close();
				return false;
			}
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
		if( bind( MySocket , serv_addr.AddrInfo->ai_addr , static_cast<int>( serv_addr.AddrInfo->ai_addrlen ) ) == SOCKET_ERROR )
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
		if( MyAcceptData.Status == AcceptDataState::Listening )
		{
			return true;
		}
	}

	//ソケットの生成
	MyAcceptData.Parent = this;
	MyAcceptData.Ov = {0};
	MyAcceptData.Status = AcceptDataState::Listening;
	MyAcceptData.ErrorCode = 0;
	MyAcceptData.BytesTransfered = 0;
	int address_family;
	{
		sockaddr_storage in;
		int len = sizeof( in );
		if( getsockname( MySocket , reinterpret_cast<sockaddr*>( &in ) , &len ) == 0 )
		{
			address_family = in.ss_family;
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"リスンソケットのIPバージョンが不正です" );
			MyAcceptData.Status = AcceptDataState::Empty;
			return false;
		}
	}

	MyAcceptData.Socket = WSASocketW( address_family , SOCK_STREAM , IPPROTO_TCP , nullptr , 0 , WSA_FLAG_OVERLAPPED );
	if( MyAcceptData.Socket == INVALID_SOCKET )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケットの生成が失敗しました" );
		MyAcceptData.Status = AcceptDataState::Empty;
		return false;
	}
	ZeroMemory( MyAcceptData.Buffer , sizeof( MyAcceptData.Buffer ) );

	//接続
	if( !mWinsockInitializer::Get().AcceptEx( MySocket , MyAcceptData.Socket , MyAcceptData.Buffer , ReceiveDataLength , LocalAddressLength , RemoteAddressLength , &MyAcceptData.BytesTransfered , &MyAcceptData.Ov ) )
	{
		//即時完了以外
		int ec = WSAGetLastError();
		SetLastError( (DWORD)ec );
		switch( ec )
		{
		case ERROR_IO_PENDING:	//接続中
			break;
		case WSAECONNRESET:		//コネクションリセット
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続がリセットされました" );
			MyAcceptData.Status = AcceptDataState::Empty;
			return false;
		default:				//その他
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続エラー" );
			MyAcceptData.Status = AcceptDataState::Empty;
			return false;
		}
	}
	return true;
}

//接続完了時の完了ルーチン
void mASyncTcpListener::ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	AcceptData* entry = CONTAINING_RECORD( ov ,  AcceptData , Ov );

	//キューを完了状態にする
	{
		mCriticalSectionTicket Ticket( MyCritical );
		entry->ErrorCode = ec;
		if( entry->Status == AcceptDataState::Listening )
		{
			entry->Status = AcceptDataState::WaitForAccept;
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"接続完了コールバックが想定外のタイミングでコールされました" );
		}
	}

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
		auto ReadSockAddr = []( const sockaddr* addr , AddressInfoEntry& entry )->void
		{
			switch( addr->sa_family )
			{
			case AF_INET6:
				entry = AddressInfoEntry( *reinterpret_cast<const sockaddr_in6*>( addr ) );
				break;
			case AF_INET:
			default:
				entry = AddressInfoEntry( *reinterpret_cast<const sockaddr_in*>( addr ) );
				break;
			}
		};

		if( setsockopt( MyAcceptData.Socket , SOL_SOCKET , SO_UPDATE_ACCEPT_CONTEXT , reinterpret_cast<char*>( &MySocket ) , sizeof( MySocket ) ) != 0 )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"ソケット設定エラー" );
		}

		sockaddr* localaddr = nullptr;
		int localaddr_len = 0;
		sockaddr* remoteaddr = nullptr;
		int remoteaddr_len = 0;

		mWinsockInitializer::Get().GetAcceptExSockaddrs( MyAcceptData.Buffer , ReceiveDataLength , LocalAddressLength , RemoteAddressLength , &localaddr , &localaddr_len , &remoteaddr , &remoteaddr_len );

		AddressInfoEntry Local;
		AddressInfoEntry Remote;
		if( !localaddr || !remoteaddr )
		{
			RaiseError( g_ErrorLogger , 0 , L"TCP" , L"アドレス取得エラー" );
		}
		else
		{
			ReadSockAddr( localaddr , Local );
			ReadSockAddr( remoteaddr , Remote );
		}

		//完了イベントをコール
		NotifyFunctionOpt opt;
		opt.OnConnect.Local = &Local;
		opt.OnConnect.Remote = &Remote;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnConnect , opt );
	}
	return;
}

mASyncTcpListener::TcpSocketInterface::TcpSocketInterface( mASyncTcpListener& listener )
	: Object( listener )
{
}

mWorkerThreadPool* mASyncTcpListener::TcpSocketInterface::GetWTP( void ) const
{
	return Object.MyWTP;
}

bool mASyncTcpListener::TcpSocketInterface::GetNewSocket( SOCKET& retNewSocket )
{
	mASyncTcpListener::AcceptDataState state;
	{
		mCriticalSectionTicket Ticket( Object.MyCritical );
		state = Object.MyAcceptData.Status;
	}

	if( state == mASyncTcpListener::AcceptDataState::Empty )
	{
		//Acceptしてない　→　返すソケット=無効、戻り値=Accept開始した結果
		retNewSocket = INVALID_SOCKET;
		return Object.PrepareAcceptSocket();
	}
	else if( state == mASyncTcpListener::AcceptDataState::Listening )
	{
		//Accept処理中　→　返すソケット=無効、戻り値=真
		retNewSocket = INVALID_SOCKET;
		return true;
	}
	else if( state == mASyncTcpListener::AcceptDataState::WaitForAccept )
	{
		//Accept完了　→　返すソケット=得られたソケット、戻り値=次のAccept開始した結果
		retNewSocket = Object.MyAcceptData.Socket;
		Object.MyAcceptData = AcceptData();
		return Object.PrepareAcceptSocket();
	}
	else
	{
		//それ以外(あり得ない)　→　返すソケット=無効、戻り値=偽
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"リスン状態エラー" );
		retNewSocket = INVALID_SOCKET;
		return false;
	}
}

mASyncTcpListener::AddressInfoEntry::AddressInfoEntry()
{
	Version = Version::IPv4;
	Port = 0;
	ZeroMemory( &Address , sizeof( Address ) );
}

mASyncTcpListener::AddressInfoEntry::AddressInfoEntry( const sockaddr_in& in )
{
	Version = Version::IPv4;
	Address.reserve( sizeof( sockaddr_in::sin_addr.S_un.S_un_b ) );
	Address.push_back( in.sin_addr.S_un.S_un_b.s_b1 );
	Address.push_back( in.sin_addr.S_un.S_un_b.s_b2 );
	Address.push_back( in.sin_addr.S_un.S_un_b.s_b3 );
	Address.push_back( in.sin_addr.S_un.S_un_b.s_b4 );
	Port = ntohs( in.sin_port );
}

mASyncTcpListener::AddressInfoEntry::AddressInfoEntry( const sockaddr_in6& in )
{
	Version = Version::IPv6;
	Address.reserve( sizeof( sockaddr_in6::sin6_addr.u.Byte ) );
	for( int i = 0 ; i < sizeof( sockaddr_in6::sin6_addr.u.Byte ) ; i++ )
	{
		Address.push_back( in.sin6_addr.u.Byte[ i ] );
	}
	Port = ntohs( in.sin6_port );
}

mASyncTcpListener::AddressInfoEntry::operator sockaddr_in() const
{
	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	if( Address.size() != sizeof( sockaddr_in::sin_addr.S_un.S_un_b ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"IPv4アドレスサイズエラー" );
	}
	else
	{
		addr.sin_addr.S_un.S_un_b.s_b1 = Address[ 0 ];
		addr.sin_addr.S_un.S_un_b.s_b2 = Address[ 1 ];
		addr.sin_addr.S_un.S_un_b.s_b3 = Address[ 2 ];
		addr.sin_addr.S_un.S_un_b.s_b4 = Address[ 3 ];
	}
	addr.sin_port = htons( Port );
	return addr;
}

mASyncTcpListener::AddressInfoEntry::operator sockaddr_in6() const
{
	sockaddr_in6 addr = { 0 };
	addr.sin6_family = AF_INET6;
	if( Address.size() != sizeof( sockaddr_in6::sin6_addr.u.Byte ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"TCP" , L"IPv6アドレスサイズエラー" );
	}
	else
	{
		for( int i = 0 ; i < sizeof( sockaddr_in6::sin6_addr.u.Byte ) ; i++ )
		{
			addr.sin6_addr.u.Byte[ i ] = Address[ i ];
		}
	}
	addr.sin6_port = htons( Port );
	return addr;
}
