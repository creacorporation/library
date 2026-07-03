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
#include "mASyncTcpListener.h"
#include <memory>
#include <ws2tcpip.h>

class mASyncTcpSocket : public mFileReadStreamBase , public mFileWriteStreamBase
{
public:
	mASyncTcpSocket();
	virtual ~mASyncTcpSocket();

	//アドレス情報
	using Version = mASyncTcpListener::Version;
	using AddressInfoEntry = mASyncTcpListener::AddressInfoEntry;
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
			const AddressInfoEntry* Local;
			const AddressInfoEntry* Remote;
		}OnConnect;

		struct OnReadOpt
		{
		}OnRead;

		struct OnWriteOpt
		{
		}OnWrite;

		struct OnFinOpt
		{
		}OnFin;

		struct OnDisconnectOpt
		{
		}OnDisconnect;

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
	using NotifyFunction = int(*)( mASyncTcpSocket& sock , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
	class NotifyOption : public mNotifyOption< NotifyFunction , Definitions_NotifyOption::IONotifyMode >
	{
	public:
		//コールバックの場合、渡されたInfoのインデックスを返すと、そのインデックスのIPに接続します。
		//コールバックを使わない場合、アドレス解決したとき一番最初に見つかったアドレスに接続します。
		NotifierInfo OnAddressLookup;
		//接続完了時のコールバック
		NotifierInfo OnConnect;
		//データ到着コールバック
		NotifierInfo OnRead;
		//データ送信完了コールバック
		NotifierInfo OnWrite;
		//FIN受信コールバック
		NotifierInfo OnFin;
		//閉鎖完了コールバック
		NotifierInfo OnDisconnect;
		//エラー発生コールバック
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
	bool Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , const WString& address , uint16_t port );

	//指定のリスナーに着信している相手と接続する
	// listener : リスンしているソケット
	// opt : 通知オプション
	//※このメソッドでは、OnConnectのコールバックは発生しません
	bool Connect( mASyncTcpListener& listener , const ConnectionOption& opt , const NotifyOption& notifier );

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//EOFは現在読み取れるデータがないことを示します
	//（時間が経てば再度読み取れるかもしれない）
	virtual INT Read( void ) override;

	//１文字書き込み
	virtual bool Write( INT data ) override;

	//キャッシュを書き込み
	//これを呼ばないと実際の送信は発生しません
	virtual bool FlushCache( void ) override;

	//読み込み側の経路が閉じているかを返します
	virtual bool IsEOF( void )const override;

	//書き込み側の経路が開いているかを返します
	virtual bool IsOpen( void )const override;

	//書き込み側の経路を閉じます
	//※TCP接続的なクローズではない。TCP的にはFINを送る処理。
	virtual bool Close( void ) override;

	//即座に接続を閉じます
	// disconnect_notify_request : 真にすると、OnDisconnectのnotifyを発生させます
	bool Abort( bool disconnect_notify_request );

	//読み込み用の内部バッファを確保します
	//臨時にバッファが必要になるときに使用します
	// count : 内部バッファが指定した数未満であれば、その数になるように内部バッファを新たに作成します
	bool PrepareReadBuffer( DWORD count );

private:

	mASyncTcpSocket( const mASyncTcpSocket& src ) = delete;
	const mASyncTcpSocket& operator=( const mASyncTcpSocket& src ) = delete;

protected:
	
	//ソケット
	SOCKET MySocket = INVALID_SOCKET;

	//設定値
	ConnectionOption MyOption;

	//通知設定値
	NotifyOption MyNotifyOption;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCritical;

	//関連付けられているワーカースレッドプールへのポインタ
	mWorkerThreadPool* MyWTP = nullptr;

	//Notify呼び出し中のイベント数
	using NotifyEventToken = std::shared_ptr<int>;
	//Notify呼び出し中のイベント数
	NotifyEventToken MyNotifyEventToken;

	//送信側のシャットダウン状態
	enum class SendShutdownState
	{
		Open,
		CloseRequest,
		Closed,
	};
	//送信側のシャットダウン状態
	SendShutdownState MySendShutdownState = SendShutdownState::Open;

	//受信側のシャットダウン状態
	enum class RecvShutdownState
	{
		Open,
		CallbackRequest,
		Closed,
	};
	//送信側のシャットダウン状態
	RecvShutdownState MyRecvShutdownState = RecvShutdownState::Open;

	//送受信ともにシャットダウンが完成したかをチェックする
	void DisconnectCheck( void );

protected:

	//処理内容フラグ
	enum class QueueType
	{
		INVALID_ENTRY,
		WRITE_QUEUE_ENTRY,
		READ_QUEUE_ENTRY,
		CONNECT_QUEUE_ENTRY,
		NAME_RESOLV_QUEUE_ENTRY,
		FIN_CALLBACK_ENTRY,
		DISCONNECT_CALLBACK_ENTRY,
	};

	//非同期用データのベース
	class ASyncDataBase
	{
	public:
		const QueueType Type;

		//親オブジェクトへのポインタ
		//ただし、非同期操作の完了時点で親オブジェクトが破棄されている場合はヌルポインタ
		mASyncTcpSocket* Parent = nullptr;

		//非同期用のOVERLAPPED構造体（Windowsに渡す用）
		WSAOVERLAPPED Ov = { 0 };

	protected:
		ASyncDataBase( QueueType type ) : Type( type ){}
	};

	//接続データ
	class ConnectData : public ASyncDataBase
	{
	public:
		AddressInfoEntry Remote;
	public:
		ConnectData() : ASyncDataBase( QueueType::CONNECT_QUEUE_ENTRY ){}
	};

	//接続データ
	ConnectData* MyConnectData = nullptr;

	//キュー
	class BufferQueueEntry : public ASyncDataBase
	{
	public:
		//実データ用バッファ
		WSABUF Buffer = { 0 , nullptr };

		//フラグ
		DWORD Flags = 0;

		//完了済みならばtrue(IO完了時に設定)
		bool Completed = false;

		//完了時のエラーコード(IO完了時に設定)
		DWORD ErrorCode = 0;

		//読み取りサイズ
		DWORD BytesTransfered = 0;
	protected:
		BufferQueueEntry( QueueType type ) : ASyncDataBase( type ){}
	};
	
	//ライトバッファ
	class WriteQueueEntry : public BufferQueueEntry
	{
	public:
		WriteQueueEntry() : BufferQueueEntry( QueueType::WRITE_QUEUE_ENTRY ){}
	};
	//ライトバッファ
	using WriteQueue = std::deque<WriteQueueEntry*>;
	//ライトバッファ
	WriteQueue MyWriteQueue;

	//リードバッファ
	class ReadQueueEntry : public BufferQueueEntry
	{
	public:
		ReadQueueEntry() : BufferQueueEntry( QueueType::READ_QUEUE_ENTRY ){}
	};
	//リードバッファ
	using ReadQueue = std::deque<ReadQueueEntry*>;
	//リードバッファ
	ReadQueue MyReadQueue;

	//名前解決用
	class NameResolveData : public ASyncDataBase
	{
	public:
		//実データ用バッファ
		ADDRINFOEXW* Info = nullptr;

		//接続先アドレス（アドレス解決前）
		WString Address;

		//接続先ポート番号
		uint16_t Port = 0;
	public:
		NameResolveData() : ASyncDataBase( QueueType::NAME_RESOLV_QUEUE_ENTRY ){}
	};
	NameResolveData* MyNameResolveData = nullptr;

	//FIN受信データ
	class FinCallbackData : public ASyncDataBase
	{
	public:
		FinCallbackData() : ASyncDataBase( QueueType::FIN_CALLBACK_ENTRY ){}
	};
	FinCallbackData* MyFinCallbackData = nullptr;

	//クローズ受信データ
	class DisconnectCallbackData : public ASyncDataBase
	{
	public:
		DisconnectCallbackData() : ASyncDataBase( QueueType::DISCONNECT_CALLBACK_ENTRY ){}
	};
	DisconnectCallbackData* MyCloseCallbackData = nullptr;

protected:

	//Finコールバックを呼び出すタスクをポストする
	void PostFinCallbackTask( void );

	//切断コールバックを呼び出すタスクをポストする
	void PostDisconnectCallbackTask( void );

	//完了ルーチン
	static void CompleteRoutine( DWORD ec , DWORD len , LPWSAOVERLAPPED ov );

	//接続完了時の完了ルーチン
	static void AddressLookupCompleteRoutine( DWORD ec , DWORD len , LPWSAOVERLAPPED ov );

	//FIN受信/切断時のコールバック
	static bool GenericCallbackRoutine( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

	//接続完了時の完了ルーチン
	void AddressLookupRoutine( DWORD ec , NameResolveData& entry );

	//接続完了時の完了ルーチン
	void ConnectCompleteRoutine( DWORD ec , ConnectData& entry );

	//受信完了時の完了ルーチン
	void ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//送信完了時の完了ルーチン
	void WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//Fin受信時の完了ルーチン
	void FinCompleteRoutine( DWORD ec );

	//切断完了時の完了ルーチン
	static void DisconnectCompleteRoutine( mASyncTcpSocket& obj , DWORD ec );

};



#endif //MASYNCSERIALPORT_H_INCLUDED



