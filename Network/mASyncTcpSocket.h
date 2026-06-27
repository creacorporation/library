//----------------------------------------------------------------------------
// TCPハンドラ
// Copyright (C) 2026 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MASYNCTCPSOCKET_H_INCLUDED
#define MASYNCTCPSOCKET_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"
#include <memory>
#include <ws2tcpip.h>

/*
使い方

#include "mStandard.h"
#include "Network/mASyncTcpSocket.h"

mWorkerThreadPool ThreadPool;
mASyncTcpSocket Pipe;

void ReadCallback_Server( mASyncTcpSocket& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	WString str;
	pipe.ReadLine( str );
	wchar_printf( "Recieved(Server):%s\n" , str.c_str() );

	pipe.WriteString( L"Pong" );
	pipe.FlushCache();
	return;
}
void ReadCallback_Client( mASyncTcpSocket& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	WString str;
	pipe.ReadLine( str );

	wchar_printf( "Recieved(Client):%s\n" , str.c_str() );
	return;
}
void ConnectCallback( mASyncTcpSocket& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	pipe.SetEncode( mFileReadStreamBase::Encode::ENCODE_UTF16 );
	return;
}

int main( int argc , char** argv )
{

	//---------------
	//準備
	//---------------
	//スレッドプール作成
	//・このクラスはスレッドプールと連携して動くので必須
	ThreadPool.Begin( 1 );

	//---------------
	//サーバー側
	//---------------
	{
		//接続まちパイプ生成時の設定
		mASyncTcpSocket::CreateOption createopt;
		createopt.MaxConn = 1;
		createopt.Timeout = 50;
		createopt.RemoteAccess = false;

		//パイプ接続時の設定
		mASyncTcpSocket::ConnectionOption connopt;

		//通知関連の設定
		mASyncTcpSocket::NotifyOption notifyopt;
		notifyopt.OnConnect.Mode = mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnConnect.Notifier.CallbackFunction = ConnectCallback;
		notifyopt.OnConnect.Parameter = parameter;
		notifyopt.OnRead.Mode = mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnRead.Notifier.CallbackFunction = ReadCallback;
		notifyopt.OnRead.Parameter = parameter;

		Pipe.Create( ThreadPool , createopt , connopt , notifyopt , L"." , "crea\\test)" );
	}

	//---------------
	//クライアント側
	//---------------
	mASyncTcpSocket client;
	{
		//パイプ接続時の設定
		mASyncTcpSocket::ConnectionOption opt;

		//通知関連の設定
		mASyncTcpSocket::NotifyOption notifyopt;
		notifyopt.OnConnect.Mode = mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnConnect.Notifier.CallbackFunction = ConnectCallback;
		notifyopt.OnConnect.Parameter = parameter;
		notifyopt.OnRead.Mode = mASyncTcpSocket::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnRead.Notifier.CallbackFunction = ReadCallback;
		notifyopt.OnRead.Parameter = parameter;

		client.Connect( ThreadPool , opt , notifyopt , L"." , "crea\\test)" );
	}

	for( int i = 0 ; i < 3 ; i++ )
	{

		SleepEx( 1000 , true );
		if( Pipe.IsConnected() )
		{
			bool valid = true;
			valid &= client.WriteString( L"Ping" );
			valid &= client.FlushCache();
			if( !valid )
			{
				//パイプが死んでる
				wchar_printf( "pipe died" );
				break;
			}
		}
	}

	//例なのでサーバー側だけ処理。本番ではクライアントも同様に処理する。
	Pipe.Close();		//書き込み終了
	Pipe.SetEOF();		//読み込み終了
	//未処理の読み込みキュー破棄
	while( !Pipe.IsEOF() )
	{
		SleepEx( 100 , true );
		while( Pipe.Read() != EOF )
		{
		}
	}
	//未処理の書き込みキュー破棄
	Pipe.Cancel();

	//スレッドプール終了
	ThreadPool.End();

	return 0;
}


*/

class mPipeReadStream : public mFileReadStreamBase
{
public:
	//読み取り側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const override
	{
		return !IsEOF();
	}
};

class mPipeWriteStream : public mFileWriteStreamBase
{
public:
	mPipeWriteStream()
	{
		MyIsClosed = false;
	}

	//書き込み側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const override
	{
		return !MyIsClosed;
	}

private:
	mPipeWriteStream( const mPipeWriteStream& src ) = delete;
	const mPipeWriteStream& operator=( const mPipeWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

class mASyncTcpSocket : public mPipeReadStream , public mPipeWriteStream
{
public:
	mASyncTcpSocket();
	virtual ~mASyncTcpSocket();

	//IPアドレスバージョン
	enum class Version
	{
		Unspecified,
		IPv4,
		IPv6,
	};

	//アドレス情報
	struct AddressInfoEntry
	{
		Version Version;
		union Address
		{
			sockaddr_in v4;
			sockaddr_in6 v6;
		};
		Address Address;
	};
	using AddressInfo = std::vector<AddressInfoEntry>;

	//通知データ
	union NotifyFunctionOpt
	{
		struct OnAddressLookupOpt
		{
			const AddressInfo* Info;
		}OnAddressLookup;

		struct OnConnectOpt
		{
		}OnConnect;

		struct OnReadOpt
		{
		}OnRead;

		struct OnWriteOpt
		{
		}OnWrite;

		struct OnErrorOpt
		{
			enum class ErrorAction
			{
				AddressLookup,	//アドレス解決中のエラー
				Connect,		//接続処理中のエラー
				Read,			//読み込み中のエラー
				Write,			//書き込み中のエラー
			};
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//通知設定
	using NotifyFunction = int(*)( mASyncTcpSocket& pipe , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
	class NotifyOption : public mNotifyOption< NotifyFunction , Definitions_NotifyOption::IONotifyMode >
	{
	public:
		NotifierInfo OnAddressLookup;
		NotifierInfo OnConnect;
		NotifierInfo OnRead;
		NotifierInfo OnWrite;
		NotifierInfo OnError;
	};

	//オプション構造体
	struct ConnectionOption
	{
	public:
		//-----------
		//送受信設定
		//-----------

		DWORD DnsTimeout;				//アドレス解決イムアウト(ミリ秒)
		DWORD ConnectTimeout;			//接続タイムアウト(ミリ秒)

		DWORD ReadPacketSize;			//読み込みパケットのサイズ
		DWORD ReadPacketCount;			//読み込みパケットを確保する数
		DWORD WritePacketSize;			//書き込みパケットのサイズ
		DWORD WritePacketNotifyCount;	//書き込み待ちパケットがここで指定した数を下回った場合に通知する
		DWORD WritePacketLimit;			//書き込み待ちパケットの数の上限（超えると書き込みエラー）
		DWORD ReadBufferTimeout;		//任意のバイトの受信間隔(ミリ秒)がこの値を上回ったら受信通知を生成する

		//-----------
		//初期値
		//-----------
		ConnectionOption()
		{
			DnsTimeout = 2000;
			ConnectTimeout = 5000;
			ReadPacketSize = 32768;
			ReadPacketCount = 8;
			WritePacketSize = 32768;
			WritePacketNotifyCount = 0;
			WritePacketLimit = 8;
			ReadBufferTimeout = 100;
		}
	};

	//指定のアドレスに接続する
	// wtp : 登録先のワーカースレッドプール
	// opt : 通知オプション
	// pipename : 名前付きパイプの名前
	bool Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , Version ver , const WString& address , uint16_t port );

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//EOFは現在読み取れるデータがないことを示します
	//（時間が経てば再度読み取れるかもしれない）
	virtual INT Read( void ) override;

	//EOFをセットしているかを調べます
	//・SetEOF()をコールするとtrueになります
	virtual bool IsEOF( void )const override;

	//１文字書き込み
	virtual bool Write( INT data ) override;

	//キャッシュを書き込み
	//これを呼ばないと実際の送信は発生しません
	virtual bool FlushCache( void ) override;

	//書き込み側の経路を閉じます
	virtual bool Close( void ) override;

	//読み込み側の経路を閉じます
	//・以降、新たな受信は行いません。
	//・その時点までに受信していたデータは通常通り読み取れます。
	virtual bool SetEOF( void ) ;

	//送信未完了のデータがあるかを返します
	// ret : 送信未完了のデータの数(キューのエントリ単位)
	DWORD IsWriting( void )const ;

	//送信未完了のデータを破棄します
	bool Cancel( void );

	//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
	bool Abort( void );

	//接続しているか否かを返します
	bool IsConnected( void )const;

	//読み込み用の内部バッファを確保します
	//臨時にバッファが必要になるときに使用します
	// count : 内部バッファが指定した数未満であれば、その数になるように内部バッファを新たに作成します
	bool PrepareReadBuffer( DWORD count );

private:

	mASyncTcpSocket( const mASyncTcpSocket& src ) = delete;
	const mASyncTcpSocket& operator=( const mASyncTcpSocket& src ) = delete;

protected:
	
	//パイプのハンドル
	HANDLE MyHandle;
	SOCKET MySocket = INVALID_SOCKET;

	//接続済みか？
	bool MyIsConnected;

	//設定値
	ConnectionOption MyOption;

	//通知設定値
	NotifyOption MyNotifyOption;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCritical;

	//関連付けられているワーカースレッドプールへのポインタ
	mWorkerThreadPool* MyWTP;

	//Notify呼び出し中のイベント数
	using NotifyEventToken = std::shared_ptr<int>;
	NotifyEventToken MyNotifyEventToken;

	//処理内容フラグ
	enum class QueueType
	{
		ADDRESS_LOOKUP_ENTRY,
		CONNECT_QUEUE_ENTRY,
		WRITE_QUEUE_ENTRY,
		READ_QUEUE_ENTRY
	};

	//キュー
	struct BufferQueueEntry
	{
		//親オブジェクトへのポインタ
		//ただし、非同期操作の完了時点で親オブジェクトが破棄されている場合はヌルポインタ
		mASyncTcpSocket* Parent;

		//接続、送信、受信のどのエントリか
		QueueType Type;

		//処理対象バッファ
		BYTE* Buffer;

		//非同期用のOVERLAPPED構造体（Windowsに渡す用）
		OVERLAPPED Ov;

		//完了済みならばtrue(IO完了時に設定)
		bool Completed;

		//完了時のエラーコード(IO完了時に設定)
		DWORD ErrorCode;

		//完了時の処理済みバイト数(IO完了時に設定)
		DWORD BytesTransfered;

	};

	using BufferQueue = std::deque<BufferQueueEntry*>;

	//ライトバッファ
	BufferQueue MyWriteQueue;

	//リードバッファ
	BufferQueue MyReadQueue;

	//接続用
	struct ConnectData
	{
		BufferQueueEntry Entry;
		WString Address;
		uint16_t Port;
	};
	std::unique_ptr<ConnectData> MyConnectData;

protected:

	//完了ルーチン
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//接続完了時の完了ルーチン
	void AddressLookupRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//接続完了時の完了ルーチン
	void ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//受信完了時の完了ルーチン
	void ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//送信完了時の完了ルーチン
	void WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//パイプを登録する
	// wtp : 登録先のワーカースレッドプール
	// opt : 通知オプション
	bool Attach( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier );


};



#endif //MASYNCSERIALPORT_H_INCLUDED



