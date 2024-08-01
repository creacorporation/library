//----------------------------------------------------------------------------
// パイプハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MASYNCNAMEDPIPE_H_INCLUDED
#define MASYNCNAMEDPIPE_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"
#include <memory>

/*
使い方

#include "mStandard.h"
#include "Network/mASyncNamedPipe.h"

mWorkerThreadPool ThreadPool;
mASyncNamedPipe Pipe;

void ReadCallback_Server( mASyncNamedPipe& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	WString str;
	pipe.ReadLine( str );
	wchar_printf( "Recieved(Server):%s\n" , str.c_str() );

	pipe.WriteString( L"Pong" );
	pipe.FlushCache();
	return;
}
void ReadCallback_Client( mASyncNamedPipe& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	WString str;
	pipe.ReadLine( str );

	wchar_printf( "Recieved(Client):%s\n" , str.c_str() );
	return;
}
void ConnectCallback( mASyncNamedPipe& pipe , DWORD_PTR parameter , DWORD_PTR opt )
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
		mASyncNamedPipe::CreateOption createopt;
		createopt.MaxConn = 1;
		createopt.Timeout = 50;
		createopt.RemoteAccess = false;

		//パイプ接続時の設定
		mASyncNamedPipe::ConnectionOption connopt;

		//通知関連の設定
		mASyncNamedPipe::NotifyOption notifyopt;
		notifyopt.OnConnect.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnConnect.Notifier.CallbackFunction = ConnectCallback;
		notifyopt.OnConnect.Parameter = parameter;
		notifyopt.OnRead.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnRead.Notifier.CallbackFunction = ReadCallback;
		notifyopt.OnRead.Parameter = parameter;

		Pipe.Create( ThreadPool , createopt , connopt , notifyopt , L"." , "crea\\test)" );
	}

	//---------------
	//クライアント側
	//---------------
	mASyncNamedPipe client;
	{
		//パイプ接続時の設定
		mASyncNamedPipe::ConnectionOption opt;

		//通知関連の設定
		mASyncNamedPipe::NotifyOption notifyopt;
		notifyopt.OnConnect.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnConnect.Notifier.CallbackFunction = ConnectCallback;
		notifyopt.OnConnect.Parameter = parameter;
		notifyopt.OnRead.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
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

namespace Definitions_ASyncNamedPipe
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
		ERROR_ON_CONNECT,	//接続処理中のエラー
		ERROR_ON_READ,		//読み込み中のエラー
		ERROR_ON_WRITE,		//書き込み中のエラー
	};
};

class mPipeReadStream : public mFileReadStreamBase
{
public:
	//読み取り側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const
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
	virtual bool IsOpen( void )const
	{
		return !MyIsClosed;
	}

private:
	mPipeWriteStream( const mPipeWriteStream& src ) = delete;
	const mPipeWriteStream& operator=( const mPipeWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

class mASyncNamedPipe : public mPipeReadStream , public mPipeWriteStream
{
public:
	mASyncNamedPipe();
	virtual ~mASyncNamedPipe();

	//エラー発生時の理由コード
	enum ErrorCode
	{
	};

	//名前付きパイプ作成用のオプション構造体
	struct CreateOption
	{
		DWORD MaxConn;					//最大同時接続数
		bool RemoteAccess;				//リモートからのアクセスを許可するか
		DWORD Timeout;

		CreateOption()
		{
			MaxConn = 1;
			RemoteAccess = false;
			Timeout = 50;
		};
	};

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
			using ErrorAction = Definitions_ASyncNamedPipe::ErrorAction;
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//通知設定
	using NotifyFunction = void(*)( mASyncNamedPipe& pipe , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
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

		//-----------
		//送受信設定
		//-----------

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
			ReadPacketSize = 128;
			ReadPacketCount = 2;
			WritePacketSize = 128;
			WritePacketNotifyCount = 0;
			WritePacketLimit = 256;
			ReadBufferTimeout = 100;
		}
	};

	//既存の名前付きパイプに接続する
	// wtp : 登録先のワーカースレッドプール
	// opt : 通知オプション
	// pipename : 名前付きパイプの名前
	bool Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename );

	//パイプを新たに作成する
	// wtp : 登録先のワーカースレッドプール
	// opt : 通知オプション
	bool Create( mWorkerThreadPool& wtp , const CreateOption& createopt , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename );

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

	mASyncNamedPipe( const mASyncNamedPipe& src ) = delete;
	const mASyncNamedPipe& operator=( const mASyncNamedPipe& src ) = delete;

protected:
	
	//パイプのハンドル
	HANDLE MyHandle;

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
		mASyncNamedPipe* Parent;

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



#endif //MASYNCSERIALPORT_H_INCLUDED



