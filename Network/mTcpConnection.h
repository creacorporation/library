//----------------------------------------------------------------------------
// TCPハンドラ
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MTCPCONNECTION_H_INCLUDED
#define MTCPCONNECTION_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"
#include <memory>

namespace Definitions_TcpConnection
{
	//通知モード
	enum NotifyMode
	{
		NOTIFY_NONE ,
		NOTIFY_WINDOWMESSAGE ,
		NOTIFY_SIGNAL ,
		NOTIFY_CALLBACK ,
	};

	//エラーが発生したタイミング
	enum ErrorAction
	{
		ERROR_ON_DNSRESOLVE,	//名前解決時のエラー
		ERROR_ON_CONNECT,		//接続処理中のエラー
		ERROR_ON_READ,			//読み込み中のエラー
		ERROR_ON_WRITE,			//書き込み中のエラー
	};
	//IPv6対応
	enum IPv6Support
	{
		IPv6_Required,			//IPv6を指定
		IPv6_Auto,				//IPv6が使えるときはIPv6
		IPv4_Required,			//IPv4を指定
	};
};

class mTcpReadStream : public mFileReadStreamBase
{
public:
	//読み取り側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mTcpWriteStream : public mFileWriteStreamBase
{
public:
	mTcpWriteStream()
	{
		MyIsClosed = false;
	}

	//書き込み側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const
	{
		return !MyIsClosed;
	}

private:
	mTcpWriteStream( const mTcpWriteStream& src ) = delete;
	const mTcpWriteStream& operator=( const mTcpWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

class mTcpConnection : public mTcpReadStream , public mTcpWriteStream
{
public:
	mTcpConnection();
	virtual ~mTcpConnection();

	//エラー発生時の理由コード
	enum ErrorCode
	{
	};

	//通知オプション
	union NotifyFunctionOpt
	{
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
			using ErrorAction = Definitions_TcpConnection::ErrorAction;
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//通知設定
	using NotifyFunction = void(*)( mTcpConnection& conn , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		NotifierInfo OnConnect;
		NotifierInfo OnRead;
		NotifierInfo OnWrite;
		NotifierInfo OnError;
	};

	//オプション構造体
	struct ConnectionOption
	{
	public:
		using IPv6Support = Definitions_TcpConnection::IPv6Support;

		//-----------
		//送受信設定
		//-----------
		IPv6Support IPv6;				//IPv6対応
		WString HostName;				//リモートのホスト名
		WString Port;					//ポート番号
		DWORD ConnectTimeout;			//接続時のタイムアウト（ミリ秒単位）

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
			IPv6 = IPv6Support::IPv6_Auto;
			HostName = L"";
			Port = L"";

			ReadPacketSize = 1500;
			ReadPacketCount = 128;
			WritePacketSize = 1500;
			WritePacketNotifyCount = 8;
			WritePacketLimit = 512;
			ReadBufferTimeout = 100;
		}
	};

	//指定のサーバーに接続する
	// wtp : 登録先のワーカースレッドプール
	// opt : 接続オプション
	// notifier : 通知オプション
	bool Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier );

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//EOFは現在読み取れるデータがないことを示します
	//（時間が経てば再度読み取れるかもしれない）
	virtual INT Read( void );

	//EOFに達しているかを調べます
	//・SetEOF()をコール後、その時点までに受信済みのデータを全て読み出すとtrueになります
	virtual bool IsEOF( void )const;

	//１文字書き込み
	virtual bool Write( INT data );

	//キャッシュを書き込み
	//これを呼ばないと実際の送信は発生しません
	virtual bool FlushCache( void );

	//書き込み側の経路を閉じます
	virtual bool Close( void );

	//読み込み側の経路を閉じます
	virtual bool SetEOF( void );

	//送信未完了のデータがあるかを返します
	// ret : 送信未完了のデータの数(キューのエントリ単位)
	DWORD IsWriting( void )const;

	//送信未完了のデータを破棄します
	bool Cancel( void );

	//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
	//接続待ちの場合は待機をキャンセルします
	bool Abort( void );

	//接続しているか否かを返します
	//※パイプの先に誰かつながっていれば真
	bool IsConnected( void )const;

	//読み込み用の内部バッファを確保します
	//臨時にバッファが必要になるときに使用します
	// count : 内部バッファが指定した数未満であれば、その数になるように内部バッファを新たに作成します
	bool PrepareReadBuffer( DWORD count );

private:

	mTcpConnection( const mTcpConnection& src ) = delete;
	const mTcpConnection& operator=( const mTcpConnection& src ) = delete;

	//Winsockの初期化をする
	static bool SetupWinsock( void );

	//Winsockのクリーンアップをする
	static bool CleanupWinsock( void );

	//接続処理
	// pool : 呼び出し元WTPの参照
	// param1 : 0
	// param2 : thisポインタ
	static void ConnectTask( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

protected:

	//使用中のインスタンスの数
	static LONG MySocketCount;

	//Winsockの実装詳細データ
	static WSAData MyWSAData;

	//接続のハンドル
	SOCKET MySocket;

	//接続済みか？
	bool MyIsConnected;

	//設定値
	ConnectionOption MyOption;

	//通知設定値
	NotifyOption MyNotifyOption;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCritical;

	//Notify呼び出し中のイベント数
	using NotifyEventToken = std::shared_ptr<int>;
	NotifyEventToken MyNotifyEventToken;

	//処理内容フラグ
	enum QueueType
	{
		CONNECT_QUEUE_ENTRY,
		WRITE_QUEUE_ENTRY,
		READ_QUEUE_ENTRY
	};

	//キュー
	struct BufferQueueEntry
	{
		//親オブジェクトへのポインタ
		//ただし、非同期操作の完了時点で親オブジェクトが破棄されている場合はヌルポインタ
		mTcpConnection* Parent;

		//送信キューのエントリか、受信キューのエントリか
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

	typedef std::deque<BufferQueueEntry*> BufferQueue;

	//ライトバッファ
	BufferQueue MyWriteQueue;

	//リードバッファ
	BufferQueue MyReadQueue;

	//接続用
	BufferQueueEntry* MyConnectData;

protected:

	//完了ルーチン
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//接続完了時の完了ルーチン
	static VOID CALLBACK ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//受信完了時の完了ルーチン
	static VOID CALLBACK ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//送信完了時の完了ルーチン
	static VOID CALLBACK WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//パイプを登録する
	// wtp : 登録先のワーカースレッドプール
	// opt : 通知オプション
	bool Attach( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier );


};

class mWinsockInitializer final : private mTcpConnection 
{
	mWinsockInitializer();
	virtual ~mWinsockInitializer();
private:
	mWinsockInitializer( const mWinsockInitializer& src ) = delete;
	const mWinsockInitializer& operator=( const mWinsockInitializer& src ) = delete;

};

#endif //MTCPCONNECTION_H_INCLUDED



