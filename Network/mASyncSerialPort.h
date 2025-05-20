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
#include <General/mCriticalSectionContainer.h>
#include <General/mNotifyOption.h>
#include <Thread/mWorkerThreadPool.h>
#include "mSerialPortBase.h"
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

//COMポートハンドルのラッパー
class mASyncSerialPort : public mSerialPortBase , public mSerialReadStream , public mSerialWriteStream
{
public:
	mASyncSerialPort();
	virtual ~mASyncSerialPort();

	//エラー発生時の理由コード
	enum ErrorCode
	{
	};

	struct Option : public mSerialPortBase::Option
	{
		DWORD ReadPacketCount;			//読み込みパケットを確保する数
		DWORD WritePacketNotifyCount;	//書き込み待ちパケットがここで指定した数を下回った場合に通知する
		DWORD WritePacketLimit;			//書き込み待ちパケットの数の上限（超えると書き込みエラー）
		Option()
		{
			ReadPacketCount = 2;
			WritePacketNotifyCount = 0;
			WritePacketLimit = 256;
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
			using ErrorAction = Definitions_SerialPortBase::ErrorAction;
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
	virtual INT Read( void )override;

	//EOFをセットしているかを調べます
	//・SetEOF()をコールするとtrueになります
	virtual bool IsEOF( void )const override;

	//１文字書き込み
	virtual bool Write( INT data )override;

	//キャッシュを書き込み
	//これを呼ばないと実際の送信は発生しません
	virtual bool FlushCache( void )override;

	//書き込み側の経路を閉じます
	virtual bool Close( void )override;

	//送信未完了のデータを破棄します
	bool Cancel( void );

	//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
	bool Abort( void );

	//読み込み側の経路を閉じます
	//・以降、新たな受信は行いません。
	//・その時点までに受信していたデータは通常通り読み取れます。
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

	//通信対象のポートの名前を得ます
	virtual WString GetPortName( void )const override;

private:

	mASyncSerialPort( const mASyncSerialPort& src ) = delete;
	const mASyncSerialPort& operator=( const mASyncSerialPort& src ) = delete;

protected:
	
	//設定値
	Option MyOption;

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
	void ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//送信完了時の完了ルーチン
	void WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

};



#endif //MASYNCSERIALPORT_H_INCLUDED



