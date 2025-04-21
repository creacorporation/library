//----------------------------------------------------------------------------
// シリアルポートハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mASyncSerialPort.h"
#include "../General/mErrorLogger.h"

static bool ComPortSetting( HANDLE handle , const mASyncSerialPort::Option& setting );

mASyncSerialPort::mASyncSerialPort()
{
	MyHandle = INVALID_HANDLE_VALUE;
	MyNotifyEventToken.reset( mNew int( 0 ) );
}

mASyncSerialPort::~mASyncSerialPort()
{
	{
		//完了関数からこのオブジェクトが呼び出されないようにする
		mCriticalSectionTicket critical( MyCritical );

		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
		for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
		{
			(*itr)->Parent = nullptr;
		}
	}

	//ハンドル廃棄
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}

	return;
}

//シリアルポートを開く
bool mASyncSerialPort::Open( mWorkerThreadPool& wtp , const Option& opt , const NotifyOption& notifier )
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"二重にポートを開こうとしています" );
		return false;
	}
	//引数をWinSDKの定義値に置き換える
	DWORD access = 0;
	access |= ( opt.Fileinfo.AccessRead ) ? ( GENERIC_READ ) : ( 0 );
	access |= ( opt.Fileinfo.AccessWrite ) ? ( GENERIC_WRITE ) : ( 0 );

	DWORD share = 0;
	share |= ( opt.Fileinfo.ShareRead ) ? ( FILE_SHARE_READ ) : ( 0 );
	share |= ( opt.Fileinfo.ShareWrite ) ? ( FILE_SHARE_WRITE ) : ( 0 );

	DWORD create_dispo;
	switch( opt.Fileinfo.Mode )
	{
	case CreateMode::CreateNew:
		create_dispo = CREATE_NEW;
		break;
	case CreateMode::CreateAlways:
		create_dispo = CREATE_ALWAYS;
		break;
	case CreateMode::OpenExisting:
		create_dispo = OPEN_EXISTING;
		break;
	case CreateMode::OpenAlways:
		create_dispo = OPEN_ALWAYS;
		break;
	default:
		return false;
	}

	//ファイルを開く
	MyHandle = CreateFileW( opt.Fileinfo.Path.c_str() , access , share , 0 , create_dispo , FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL , 0 );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		//開けなかった
		RaiseError( g_ErrorLogger , 0 , L"ポートを開くことができませんでした" , opt.Fileinfo.Path );
		return false;
	}

	//COMポートの設定を行う
	if( !ComPortSetting( MyHandle , opt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ポートの初期設定が失敗しました" , opt.Fileinfo.Path );
		return false;
	}

	//ワーカースレッドプールに登録する
	if( !wtp.Attach( MyHandle , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドプールに登録できませんでした" );
		return false;
	}

	//ファイルを開けたので、通知方法をストック
	MyOption = opt;
	MyNotifyOption = notifier;

	//早速読み込みバッファを積む
	if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
		return false;
	}
	return true;
}

bool mASyncSerialPort::PrepareReadBuffer( DWORD count )
{
	//クリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	if( MyIsEOF )
	{
		//EOF設定済み
		return true;
	}

	while( MyReadQueue.size() < count )
	{
		BufferQueueEntry* entry = mNew BufferQueueEntry;
		entry->Parent = this;
		entry->Type = QueueType::READ_QUEUE_ENTRY;
		entry->Buffer = mNew BYTE[ MyOption.ReadPacketSize ];
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
		ReadFile( MyHandle , entry->Buffer , MyOption.ReadPacketSize , &readsize , &entry->Ov );
		switch( GetLastError() )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"読み込みの非同期操作が開始しませんでした" );
			MyReadQueue.pop_back();
			mDelete[] entry->Buffer;
			mDelete entry;
			return false;
		}
	}
	return true;
}

static void AsyncEvent( mASyncSerialPort& pipe , const mASyncSerialPort::NotifyOption::NotifierInfo& info , const mASyncSerialPort::NotifyFunctionOpt& opt )
{
	if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&pipe , info.Parameter );
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mASyncSerialPort::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
				if( !mASyncSerialPort::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( pipe , info.Parameter , opt );
	}
	else if( info.Mode == mASyncSerialPort::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
	{
		if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
		{
			SetEvent( info.Notifier.Handle );
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , (int)info.Mode );
	}
}

VOID CALLBACK mASyncSerialPort::CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );
	if( ov == nullptr || entry == nullptr )
	{
		return;
	}

	if( !entry->Parent )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"親オブジェクトが消滅しています" , entry->Type );

		mDelete[] entry->Buffer;
		mDelete entry;
		return;
	}

	NotifyEventToken token( entry->Parent->MyNotifyEventToken );

	switch( entry->Type )
	{
	case QueueType::READ_QUEUE_ENTRY:
		ReadCompleteRoutine( ec , len , ov );
		break;
	case QueueType::WRITE_QUEUE_ENTRY:
		WriteCompleteRoutine( ec , len , ov );
		break;
	default:
		break;
	}
	return;
}

//読み取り時の完了ルーチン
VOID CALLBACK mASyncSerialPort::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	bool complete_callback = true;
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		if( !entry->Completed )
		{
			entry->Completed = true;
			entry->ErrorCode = ec;
			entry->BytesTransfered = len;
		}

		//キューの先頭ではない場合はコールバックを呼ばない
		//※NOTIFY_CALLBACK_PARALLELのときは、先頭か否かに関係なくコールバックを呼ぶ
		if( entry->Parent->MyNotifyOption.OnRead.Mode != NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
		{
			if( entry->Parent->MyReadQueue.empty() || entry->Parent->MyReadQueue.front() != entry )
			{
				complete_callback = false;
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
				opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_READ;
				opt.OnError.ErrorCode = ec;
				AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		if( complete_callback )
		{
			//完了イベントをコール
			NotifyFunctionOpt opt;
			AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnRead , opt );
		}
	}
	return;
}


VOID CALLBACK mASyncSerialPort::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	BufferQueue remove_queue;	//削除予定のキュー
	size_t queue_size = 0;		//削除後のキューサイズ
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( entry->Parent->MyCritical );

		//キューを完了状態にする
		if( !entry->Completed )
		{
			entry->Completed = true;
			entry->ErrorCode = ec;
			entry->BytesTransfered = len;
		}

		//キューの先頭からスキャンし、完了済みのパケットを順次削除
		while( !entry->Parent->MyWriteQueue.empty() )
		{
			if( entry->Parent->MyWriteQueue.front()->Completed )
			{
				remove_queue.push_back( std::move( entry->Parent->MyWriteQueue.front() ) );
				entry->Parent->MyWriteQueue.pop_front();
			}
			else
			{
				break;
			}
		}
		queue_size = entry->Parent->MyWriteQueue.size();
	}

	//イベント呼び出し
	if( ec != ERROR_SUCCESS )
	{
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"非同期書き込み操作が失敗しました" );

		//エラーが起きているからイベントをコール
		NotifyFunctionOpt opt;
		opt.OnError.Action = NotifyFunctionOpt::OnErrorOpt::ErrorAction::ERROR_ON_WRITE;
		opt.OnError.ErrorCode = ec;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnError , opt );
	}
	else if( queue_size < entry->Parent->MyOption.WritePacketNotifyCount )
	{
		//キューのエントリ数が減ったからイベントをコール
		NotifyFunctionOpt opt;
		AsyncEvent( *entry->Parent , entry->Parent->MyNotifyOption.OnWrite , opt );
	}

	//ポインタの削除を行う
	for( BufferQueue::iterator itr = remove_queue.begin() ; itr != remove_queue.end() ; itr++ )
	{
		mDelete (*itr)->Buffer;
		mDelete (*itr);
	}
	return;
}

//１文字（１バイト）読み込みます
INT mASyncSerialPort::Read( void )
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
				MyReadCacheHead.reset( entry->Buffer );
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

//EOFに達しているかを調べます
bool mASyncSerialPort::IsEOF( void )const
{
	if( !MyIsEOF )
	{
		return false;
	}
	else if( MyReadCacheRemain )
	{
		return false;
	}
	else
	{
		//ここだけクリティカルセクション
		mCriticalSectionTicket critical( MyCritical );
		return MyReadQueue.empty() && ( MyNotifyEventToken.use_count() == 1 );
	}
}

//書き込み側の経路を閉じます
bool mASyncSerialPort::Close( void )
{
	MyIsClosed = true;
	FlushCache();
	return true;
}

//読み込み側の経路を閉じます
bool mASyncSerialPort::SetEOF( void )
{
	//ここだけクリティカルセクション
	mCriticalSectionTicket critical( MyCritical );
	MyIsEOF = true;

	for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
	{
		if( !(*itr)->Completed )
		{
			CancelIoEx( MyHandle , &(*itr)->Ov );
		}
	}

	return true;
}

//１文字書き込み
bool mASyncSerialPort::Write( INT data )
{
	//クローズ済み？
	if( MyIsClosed )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートはすでに閉じられています" );
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
		return false;
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
bool mASyncSerialPort::FlushCache( void )
{
	BufferQueueEntry* entry = nullptr;
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
			RaiseError( g_ErrorLogger , 0 , L"書き込み待ちパケットの数が上限に到達しました" );
			return false;
		}

		entry = mNew BufferQueueEntry;
		entry->Parent = this;
		entry->Type = QueueType::WRITE_QUEUE_ENTRY;
		entry->Buffer = MyWriteCacheHead.release();
		entry->Ov.hEvent = 0;
		entry->Ov.Internal = 0;
		entry->Ov.InternalHigh = 0;
		entry->Ov.Offset = 0;
		entry->Ov.OffsetHigh = 0;
		entry->Completed = false;
		entry->ErrorCode = 0;
		entry->BytesTransfered = 0;

		DWORD bytes_to_write = MyWriteCacheWritten;
		MyWriteQueue.push_back( entry );
		MyWriteCacheWritten = 0;
		MyWriteCacheRemain = 0;

		DWORD written = 0;
		if( WriteFile( MyHandle , entry->Buffer , bytes_to_write , &written , &entry->Ov ) )
		{
			return true;
		}
		switch( GetLastError() )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			return true;
		default:
			break;
		}
	}

	//書き込みに失敗しているのでこのキューを削除する
	//※データは損失している
	mDelete[] entry->Buffer;
	mDelete entry;
	RaiseError( g_ErrorLogger , 0 , L"書き込みの非同期操作が開始しませんでした" );
	return false;
}

//送信未完了のデータがあるかを返します
DWORD mASyncSerialPort::IsWriting( void )const
{
	//このブロックはクリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	return (DWORD)MyWriteQueue.size();
}

//送信未完了のデータを破棄します
bool mASyncSerialPort::Cancel( void )
{
	mCriticalSectionTicket critical( MyCritical );
	MyWriteCacheHead.reset();
	MyWriteCacheWritten = 0;
	MyWriteCacheRemain = 0;

	for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
	{
		if( !(*itr)->Completed )
		{
			CancelIoEx( MyHandle , &(*itr)->Ov );
		}
	}
	return true;
}

//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
bool mASyncSerialPort::Abort( void )
{
	//書き込み終了しキューをキャンセル
	Close();
	Cancel();
	//読み込み終了してキューをキャンセル
	SetEOF();

	//未処理のキュー破棄
	DWORD wait_time = 0;
	while( 1 )
	{
		bool empty = true;

		if( MyNotifyEventToken.use_count() == 1 )
		{
			mCriticalSectionTicket critical( MyCritical );
			for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; )
			{
				if( (*itr)->Completed )
				{
					mDelete (*itr)->Buffer;
					mDelete (*itr);
					itr = MyWriteQueue.erase( itr );
					continue;
				}
				itr++;
			}
			for( BufferQueue::iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; )
			{
				if( (*itr)->Completed )
				{
					mDelete (*itr)->Buffer;
					mDelete (*itr);
					itr = MyReadQueue.erase( itr );
					continue;
				}
				itr++;
			}
			MyReadCacheRemain = 0;
			MyReadCacheCurrent = 0;
			MyReadCacheHead.reset();

			empty = MyWriteQueue.empty() && MyReadQueue.empty();
		}
		else
		{
			empty = false;
		}

		if( empty && ( MyNotifyEventToken.use_count() == 1 ) )
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
	return true;

}

static bool ComPortSetting( HANDLE handle , const mASyncSerialPort::Option& setting )
{
	DCB dcb;

	//現在の設定を取得
	if( !GetCommState( handle , &dcb ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートの現在の設定を取得できませんでした" );
		return false;
	}

	//設定の更新

	//パリティ
	switch( setting.Parity )
	{
	case mASyncSerialPort::ParityType::PARITYTYPE_NOPARITY:
		dcb.Parity = NOPARITY;
		dcb.fParity = false;
		break;
	case mASyncSerialPort::ParityType::PARITYTYPE_EVEN:
		dcb.Parity = EVENPARITY;
		dcb.fParity = true;
		break;
	case mASyncSerialPort::ParityType::PARITYTYPE_ODD:
		dcb.Parity = ODDPARITY;
		dcb.fParity = true;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パリティ設定が不正です" , setting.Parity );
		return false;
	}
	//ストップビット
	switch( setting.StopBit )
	{
	case mASyncSerialPort::StopBitType::STOPBIT_ONE:
		dcb.StopBits = ONESTOPBIT;
		break;
	case mASyncSerialPort::StopBitType::STOPBIT_ONEFIVE:
		dcb.StopBits = ONE5STOPBITS;
		break;
	case mASyncSerialPort::StopBitType::STOPBIT_TWO:
		dcb.StopBits = TWOSTOPBITS;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"ストップビット設定が不正です" , setting.StopBit );
		return false;
	}
	//ボーレート
	dcb.BaudRate = setting.BaudRate;

	//バイトサイズ
	if( 0xffu < setting.ByteSize )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バイトサイズ設定が不正です" , setting.ByteSize );
		return false;
	}
	dcb.ByteSize = (BYTE)setting.ByteSize;

	//フローコントロール(DTR)
	switch( setting.DTRFlowControl )
	{
	case mASyncSerialPort::DTRFlowControlMode::ALWAYS_OFF:
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		break;
	case mASyncSerialPort::DTRFlowControlMode::ALWAYS_ON:
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		break;
	case mASyncSerialPort::DTRFlowControlMode::HANDSHAKE:
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"DTR設定が不正です" , (int)setting.DTRFlowControl );
		return false;
	}

	//フローコントロール(DSR)
	dcb.fOutxDsrFlow = setting.MonitorDSR;

	//フローコントロール(RTS)
	switch( setting.RTSFlowControl )
	{
	case mASyncSerialPort::RTSFlowControlMode::ALWAYS_OFF:
		dcb.fRtsControl = DTR_CONTROL_DISABLE;
		break;
	case mASyncSerialPort::RTSFlowControlMode::ALWAYS_ON:
		dcb.fRtsControl = DTR_CONTROL_ENABLE;
		break;
	case mASyncSerialPort::RTSFlowControlMode::HANDSHAKE:
		dcb.fRtsControl = DTR_CONTROL_HANDSHAKE;
		break;
	case mASyncSerialPort::RTSFlowControlMode::TOGGLE:
		dcb.fRtsControl = RTS_CONTROL_TOGGLE;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"RTS設定が不正です" , (int)setting.RTSFlowControl );
		return false;
	}

	//フローコントロール(CTS)
	dcb.fOutxCtsFlow = setting.MonitorCTS;

	//設定の適用
	if( !SetCommState( handle , &dcb ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートの設定を更新できませんでした" );
	}


	COMMTIMEOUTS timeout;
	GetCommTimeouts( handle , &timeout );
	timeout.ReadIntervalTimeout = setting.ReadBufferTimeout;
	timeout.ReadTotalTimeoutConstant = 0;
	timeout.ReadTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = 0;
	timeout.WriteTotalTimeoutMultiplier = 0;
	if( !SetCommTimeouts( handle , &timeout ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートのタイムアウト設定を更新できませんでした" );
	}

	return true;
}

uint32_t mASyncSerialPort::GetReadableSize( void )const
{
	uint32_t result = 0;

	result += (uint32_t)MyUnReadBuffer.size();
	result += MyReadCacheRemain;
	{
		mCriticalSectionTicket ticket( MyCritical );
		for( BufferQueue::const_iterator itr = MyReadQueue.begin() ; itr != MyReadQueue.end() ; itr++ )
		{
			if( !(*itr)->Completed )
			{
				break;
			}
			result += (*itr)->BytesTransfered;
		}
	}
	return result;
}


