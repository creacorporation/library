//----------------------------------------------------------------------------
// シリアルポートハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MASYNCSERIALPORT_H_INCLUDED
#define MASYNCSERIALPORT_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"

#include <memory>

/*
使い方

int main( int argc , char** argv )
{
	mASyncSerialPort SerialPort;
	mWorkerThreadPool ThreadPool;

	//---------------
	//準備
	//---------------

	//スレッドプール作成
	//・このクラスはスレッドプールと連携して動くので必須
	ThreadPool.Begin( 1 );

	//COMポート
	{
		mASyncSerialPort::Option opt;				//←各種設定
		mASyncSerialPort::NotifyOption notifier;	//←イベント時にコールバック等が必要なら

		opt.Fileinfo.Path = LR"(\\.\COM5)";
		SerialPort.Open( ThreadPool , opt , notifier );
	}

	//---------------
	//メインループ
	//---------------

	while( 1 )
	{
		//読み書き関連のイベントが発生すると、設定した内容に応じて
		//・コールバック関数が呼ばれる
		//・イベントオブジェクトがシグナル状態になる
		//・ウインドウメッセージがポストされる
		//といったことが起きるので、これをトリガにしてアプリを動作させる

		//イベントが発生したら、以下のような感じでデータを読む
		INT read_one_byte = SerialPort.read();	//1文字単位

		AString read_as_string;
		SerialPort.ReadLine( read_as_string );	//1行単位

		//※1行単位で読むとき、通信エラー時に無限に待ち続けないように注意
		//  一定時間受信がないときは1文字ごとに読んでみるとかして対策
	}

	//---------------
	//クリーンアップ
	//---------------

	//EOFを設定(シリアルポートは終端がないので明示的に設定しないと永久に読めてしまう)
	SerialPort.SetEOF();

	//受信済みのデータを全て処理する(IsEOFがtrueになるまで)
	//ここでは、単に受信済みのデータを全部捨てている
	while( !SerialPort.IsEOF() )
	{
		//Read()がEOFを返しても、「現時点の最後まで読んだ」だけかもしれない(今後データが届くかもしれない)
		//IsEOFがtrueの場合は、以降データが届くことはない

		while( SerialPort.Read() != EOF )
		{
		}
		Sleep( 100 );
	}

	//スレッドプールのタスクがはけるのを待つ
	//このサンプルではタスクを追加していないので、すぐに抜ける
	while( ThreadPool.GetTaskCount() )
	{
		Sleep( 100 );
	}

	//スレッドプール終了
	ThreadPool.End();

	return 0;
}

*/

namespace Definitions_ASyncSerialPort
{
	enum ParityType
	{
		PARITYTYPE_NOPARITY ,	//パリティなし
		PARITYTYPE_EVEN ,		//偶数パリティ
		PARITYTYPE_ODD ,		//奇数パリティ
	};

	enum StopBitType
	{
		STOPBIT_ONE ,			//1ビット
		STOPBIT_ONEFIVE ,		//1.5ビット
		STOPBIT_TWO ,			//2ビット
	};

	//エラーが発生したタイミング
	enum ErrorAction
	{
		ERROR_ON_CONNECT,	//接続処理中のエラー
		ERROR_ON_READ,		//読み込み中のエラー
		ERROR_ON_WRITE,		//書き込み中のエラー
	};

	//フローコントロール(DTR-DSR)
	enum class DTRFlowControlMode
	{
		ALWAYS_OFF,					//自分のDTRから常にオフを送信する
		ALWAYS_ON,					//自分のDTRから常にオンを送信する
		HANDSHAKE					//DTR-DSRのハンドシェイクを行う
	};

	//フローコントロール(RTS-CTS)
	enum class RTSFlowControlMode
	{
		ALWAYS_OFF,					//自分のRTSから常にオフを送信する
		ALWAYS_ON,					//自分のRTSから常にオンを送信する
		HANDSHAKE,					//RTS-CTSのハンドシェイクを行う(送信バッファにデータが50%たまるとオン/25%以下でオフ)
		TOGGLE,						//RTS-CTSのハンドシェイクを行う(送信バッファに1バイトでもデータがあればオン/空でオフ)
	};

};

class mSerialReadStream : public mFileReadStreamBase
{
public:
	//読み取り側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mSerialWriteStream : public mFileWriteStreamBase
{
public:
	mSerialWriteStream()
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
	mSerialWriteStream( const mSerialWriteStream& src ) = delete;
	const mSerialWriteStream& operator=( const mSerialWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

//COMポートハンドルのラッパー
class mASyncSerialPort : public mSerialReadStream , public mSerialWriteStream
{
public:
	mASyncSerialPort();
	virtual ~mASyncSerialPort();

	//エラー発生時の理由コード
	enum ErrorCode
	{
	};

	//パリティの設定
	using ParityType = Definitions_ASyncSerialPort::ParityType;

	//ストップビット長
	using StopBitType = Definitions_ASyncSerialPort::StopBitType;

	//ファイル設定
	using FileOption = mFile::Option;

	//ファイルを開くときのモード
	using CreateMode = mFile::CreateMode;

	//フローコントロール
	using DTRFlowControlMode = Definitions_ASyncSerialPort::DTRFlowControlMode;

	//フローコントロール
	using RTSFlowControlMode = Definitions_ASyncSerialPort::RTSFlowControlMode;

	//オプション構造体
	struct Option
	{
		//-----------
		//送受信設定
		//-----------
		FileOption Fileinfo;
		DWORD BaudRate;			//ボーレート
		ParityType Parity;		//パリティ
		StopBitType StopBit;	//ストップビット
		DWORD ByteSize;			//1バイトが何ビットか。普通は8。

		//-----------
		//バッファリング設定
		//-----------
		DWORD ReadPacketSize;			//読み込みパケットのサイズ
		DWORD ReadPacketCount;			//読み込みパケットを確保する数
		DWORD WritePacketSize;			//書き込みパケットのサイズ
		DWORD WritePacketNotifyCount;	//書き込み待ちパケットがここで指定した数を下回った場合に通知する
		DWORD WritePacketLimit;			//書き込み待ちパケットの数の上限（超えると書き込みエラー）
		DWORD ReadBufferTimeout;		//任意のバイトの受信間隔(ミリ秒)がこの値を上回ったら受信通知を生成する

		//-----------
		//フローコントロール
		//-----------
		DTRFlowControlMode DTRFlowControl;	//DTR信号を制御するか
		bool MonitorDSR;					//相手から送られてくるDSRを利用するか(true=利用する/false=無視する)
		RTSFlowControlMode RTSFlowControl;	//RTS信号を制御するか
		bool MonitorCTS;					//相手から送られてくるCTSを利用するか(true=利用する/false=無視する)

		//-----------
		//初期値
		//-----------
		Option()
		{
			BaudRate = 9600;
			Parity = mASyncSerialPort::ParityType::PARITYTYPE_NOPARITY;
			StopBit = mASyncSerialPort::StopBitType::STOPBIT_ONE;
			ByteSize = 8;
			ReadPacketSize = 128;
			ReadPacketCount = 2;
			WritePacketSize = 128;
			WritePacketNotifyCount = 0;
			WritePacketLimit = 256;
			ReadBufferTimeout = 100;
			DTRFlowControl = DTRFlowControlMode::ALWAYS_ON;
			MonitorDSR = false;
			RTSFlowControl = RTSFlowControlMode::ALWAYS_ON;
			MonitorCTS = false;
		}
	};

	union NotifyFunctionOpt
	{
		struct OnReadOpt
		{
		}OnRead;

		struct OnWriteOpt
		{
		}OnWrite;

		struct OnErrorOpt
		{
			using ErrorAction = Definitions_ASyncSerialPort::ErrorAction;
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//コールバックで通知する場合のプロトタイプ
	//req : コールバックを発生させたオブジェクトの参照
	//parameter : オブジェクト生成時に渡した任意の値(NotifierInfo::Parameterの値)
	//opt : 拡張情報
	using NotifyFunction = void(*)( mASyncSerialPort& port , DWORD_PTR parameter , const NotifyFunctionOpt& opt );

	//通知設定
	class NotifyOption : public mNotifyOption< NotifyFunction , Definitions_NotifyOption::IONotifyMode >
	{
	public:
		NotifierInfo OnRead;
		NotifierInfo OnWrite;
		NotifierInfo OnError;
	};

	//シリアルポートを開く
	bool Open( mWorkerThreadPool& wtp , const Option& opt , const NotifyOption& notifier );

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

	//送信未完了のデータを破棄します
	bool Cancel( void );

	//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
	bool Abort( void );

	//読み込み側の経路を閉じます
	virtual bool SetEOF( void );

	//送信未完了のデータがあるかを返します
	// ret : 送信未完了のデータの数(キューのエントリ単位)
	DWORD IsWriting( void )const;

	//読み込み用の内部バッファを確保します
	//臨時にバッファが必要になるときに使用します
	// count : 内部バッファが指定した数未満であれば、その数になるように内部バッファを新たに作成します
	//         0であればインスタンス生成時のオプションの値にします
	bool PrepareReadBuffer( DWORD count = 0 );

	//現在読み取り可能なバイト数を返します
	uint32_t GetReadableSize( void )const;

	//現在受信済みのデータを破棄します
	void ClearReadBuffer( void );

private:

	mASyncSerialPort( const mASyncSerialPort& src ) = delete;
	const mASyncSerialPort& operator=( const mASyncSerialPort& src ) = delete;

protected:
	
	//COMポートのハンドル
	HANDLE MyHandle;

	//設定値
	Option MyOption;

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
		WRITE_QUEUE_ENTRY,
		READ_QUEUE_ENTRY
	};

	//キュー
	struct BufferQueueEntry
	{
		//親オブジェクトへのポインタ
		//ただし、非同期操作の完了時点で親オブジェクトが破棄されている場合はヌルポインタ
		mASyncSerialPort* Parent;

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

protected:

	//完了ルーチン
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//受信完了時の完了ルーチン
	static VOID CALLBACK ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//送信完了時の完了ルーチン
	static VOID CALLBACK WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

};



#endif //MASYNCSERIALPORT_H_INCLUDED



