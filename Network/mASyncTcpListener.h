//----------------------------------------------------------------------------
// TCPハンドラ
// Copyright (C) 2026 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MASYNCTCPLISTENER_H_INCLUDED
#define MASYNCTCPLISTENER_H_INCLUDED

#include "mStandard.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"
#include <memory>
#include <set>
#include <ws2tcpip.h>



class mASyncTcpListener
{
public:
	mASyncTcpListener();
	virtual ~mASyncTcpListener();

	//IPアドレスバージョン
	enum class Version
	{
		IPv4,
		IPv6,
	};

	//通知データ
	union NotifyFunctionOpt
	{
		struct OnConnectOpt
		{
		}OnConnect;

		struct OnErrorOpt
		{
			DWORD ErrorCode;
		}OnError;
	};

	//通知設定
	using NotifyFunction = void(*)( mASyncTcpListener& listener , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
	class NotifyOption : public mNotifyOption< NotifyFunction , Definitions_NotifyOption::IONotifyMode >
	{
	public:
		NotifierInfo OnConnect;
		NotifierInfo OnError;
	};

	//オプション構造体
	struct ConnectionOption
	{
	public:
		//IPバージョン
		Version Ver = Version::IPv4;
		//アドレス
		WString Address;
		//ポート番号
		uint16_t Port = 10000;
		//バックログの数
		DWORD Backlog = 10;
	};

	//ポートを開く
	// wtp : 登録先のワーカースレッドプール
	// notifier : 通知オプション
	bool Open( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier );

	//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
	bool Close( void );

	//アドレス情報
	struct AddressInfoEntry
	{
		AddressInfoEntry();
		Version Version;
		union Address
		{
			sockaddr_in v4;
			sockaddr_in6 v6;
		};
		Address Address;
	};

	//接続してきたソケットの取得
	class TcpSocketInterface
	{
	private:
		friend class mASyncTcpSocket;
		TcpSocketInterface( mASyncTcpListener& listener );
		mASyncTcpListener& Object;
	protected:
		bool GetNewSocket( SOCKET& retNewSocket , AddressInfoEntry& retLocal , AddressInfoEntry& retRemote );
	};

private:

	mASyncTcpListener( const mASyncTcpListener& src ) = delete;
	const mASyncTcpListener& operator=( const mASyncTcpListener& src ) = delete;

protected:

	bool PrepareAcceptSocket( void );

	//パイプのハンドル
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
	NotifyEventToken MyNotifyEventToken;

	//現在の状態
	enum class AcceptDataState
	{
		Empty,				//未使用
		Listening,			//外部からの接続待ち
		WaitForAccept,		//プログラムからソケットを取得されるの待ち
	};

	//バッファサイズ定義
	static constexpr uint32_t ReceiveDataLength = 0;
	static constexpr uint32_t LocalAddressLength = sizeof(sockaddr_storage) + 16;
	static constexpr uint32_t RemoteAddressLength = sizeof(sockaddr_storage) + 16;

	//接続データ
	struct AcceptData
	{
		//親オブジェクトへのポインタ
		//ただし、非同期操作の完了時点で親オブジェクトが破棄されている場合はヌルポインタ
		mASyncTcpListener* Parent = nullptr;

		//処理対象ソケット
		SOCKET Socket = INVALID_SOCKET;

		//データバッファ
		BYTE Buffer[ ReceiveDataLength + LocalAddressLength + RemoteAddressLength ];

		//非同期用のOVERLAPPED構造体（Windowsに渡す用）
		OVERLAPPED Ov;

		//完了済みならばtrue(IO完了時に設定)
		AcceptDataState Status = AcceptDataState::Empty;

		//完了時のエラーコード(IO完了時に設定)
		DWORD ErrorCode = 0;

		//完了時の処理済みバイト数(IO完了時に設定)
		DWORD BytesTransfered = 0;
	};

	//接続データ
	AcceptData MyAcceptData;

protected:

	//完了ルーチン
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//接続完了時の完了ルーチン
	void ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//バージョンのenum→AF_INETx
	int GetAddressFamily( Version ver )const;

};



#endif //MASYNCSERIALPORT_H_INCLUDED



