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
#include <General/mErrorLogger.h>
#include <General/mFileUtility.h>

mASyncSerialPort::mASyncSerialPort()
{
	MyWTP = nullptr;
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

	Abort();
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
	MyHandle = CreateFileW( opt.Fileinfo.Path.c_str() , access , share , 0 , create_dispo , FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH , 0 );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		//開けなかった
		RaiseError( g_ErrorLogger , 0 , L"ポートを開くことができませんでした" , opt.Fileinfo.Path );
		goto errorend;
	}

	//COMポートの設定を行う
	if( !ComPortSetting( MyHandle , opt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ポートの初期設定が失敗しました" , opt.Fileinfo.Path );
		goto errorend;
	}

	//ワーカースレッドプールに登録する
	if( !wtp.Attach( MyHandle , CompleteRoutine ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ワーカースレッドプールに登録できませんでした" );
		goto errorend;
	}
	MyWTP = &wtp;

	//コールバック用のトークンを初期化
	MyNotifyEventToken.reset( mNew int( 0 ) );

	//ファイルを開けたので、通知方法をストック
	MyOption = opt;
	MyNotifyOption = notifier;

	//早速読み込みバッファを積む
	if( !PrepareReadBuffer( MyOption.ReadPacketCount ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"読み込み用のバッファを準備できませんでした" );
		return false;	//バッファを積めてないだけでポートは開けているのでエラーにはするが、ハンドルの破棄はしない
	}
	return true;

errorend:
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	return false;
}

bool mASyncSerialPort::PrepareReadBuffer( DWORD count )
{
	//ハンドルが開いているか確認
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	//クリティカルセクション
	mCriticalSectionTicket critical( MyCritical );

	if( MyIsEOF )
	{
		//EOF設定済み
		return true;
	}

	//０だったらデフォルトにする
	if( count == 0 )
	{
		count = MyOption.ReadPacketCount;
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
		if( ReadFile( MyHandle , entry->Buffer , MyOption.ReadPacketSize , &readsize , &entry->Ov ) )
		{
			return true;
		}
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

	mASyncSerialPort* me = entry->Parent;
	if( !me )
	{
		//親が消滅している場合はそっと削除しておく
		SetLastError( ec );
		RaiseAssert( g_ErrorLogger , 0 , L"親オブジェクトが消滅しています" , entry->Type );

		mDelete[] entry->Buffer;
		mDelete entry;
		return;
	}

	//キューを完了状態にする
	if( !entry->Completed )
	{
		entry->Completed = true;
		entry->ErrorCode = ec;
		entry->BytesTransfered = len;
	}

	std::weak_ptr<NotifyEventToken::element_type> token_ptr = me->MyNotifyEventToken;
	NotifyEventToken token = token_ptr.lock();
	if( !token || me->MyIsEOF )
	{
		return;
	}

	switch( entry->Type )
	{
	case QueueType::READ_QUEUE_ENTRY:
		me->ReadCompleteRoutine( ec , len , ov );
		break;
	case QueueType::WRITE_QUEUE_ENTRY:
		me->WriteCompleteRoutine( ec , len , ov );
		break;
	default:
		break;
	}
	return;
}

//読み取り時の完了ルーチン
void mASyncSerialPort::ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	//キューの先頭ではない場合はコールバックを呼ばない
	//※NOTIFY_CALLBACK_PARALLELのときは、先頭か否かに関係なくコールバックを呼ぶ
	bool complete_callback;
	if( MyNotifyOption.OnRead.Mode != NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( MyCritical );
		if( MyReadQueue.empty() )
		{
			complete_callback = false;
		}
		else if( 2 < MyNotifyEventToken.use_count() )
		{
			complete_callback = ( MyReadQueue.front() == entry );
		}
		else
		{
			complete_callback = true;
		}
	}
	else
	{
		complete_callback = true;
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
				AsyncEvent( *this , MyNotifyOption.OnError , opt );
			}
		}
	}
	else
	{
		if( complete_callback )
		{
			//完了イベントをコール
			NotifyFunctionOpt opt;
			AsyncEvent( *this , MyNotifyOption.OnRead , opt );
		}
	}
	return;
}


void mASyncSerialPort::WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov )
{
	BufferQueueEntry* entry = CONTAINING_RECORD( ov ,  BufferQueueEntry , Ov );

	BufferQueue remove_queue;	//削除予定のキュー
	size_t queue_size = 0;		//削除後のキューサイズ
	{
		//このブロックはクリティカルセクション
		mCriticalSectionTicket critical( MyCritical );

		//キューの先頭からスキャンし、完了済みのパケットを順次削除
		while( !MyWriteQueue.empty() )
		{
			if( MyWriteQueue.front()->Completed )
			{
				remove_queue.push_back( std::move( MyWriteQueue.front() ) );
				MyWriteQueue.pop_front();
			}
			else
			{
				break;
			}
		}
		queue_size = MyWriteQueue.size();
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
		AsyncEvent( *this , MyNotifyOption.OnError , opt );
	}
	else if( queue_size < MyOption.WritePacketNotifyCount )
	{
		//キューのエントリ数が減ったからイベントをコール
		NotifyFunctionOpt opt;
		AsyncEvent( *this , MyNotifyOption.OnWrite , opt );
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

//EOFをセットしているか調べる
bool mASyncSerialPort::IsEOF( void )const
{
	return MyIsEOF;
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
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		return false;
	}

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

	//ハンドルが有効であればIOキャンセル
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		for( BufferQueue::iterator itr = MyWriteQueue.begin() ; itr != MyWriteQueue.end() ; itr++ )
		{
			if( !(*itr)->Completed )
			{
				CancelIoEx( MyHandle , &(*itr)->Ov );
			}
		}
	}
	return true;
}

void mASyncSerialPort::ClearReadBuffer( void )
{
	mCriticalSectionTicket critical( MyCritical );
	MyReadQueue.clear();
	mFileReadStreamBase::ClearBuffer();
}

//現在未完了の通信(送受信とも)を全て破棄し、接続を閉じます
bool mASyncSerialPort::Abort( void )
{
	//書き込み終了しキューをキャンセル
	Close();
	Cancel();
	//読み込み終了してキューをキャンセル
	SetEOF();

	//新たにtokenを作れないようにする
	NotifyEventToken token = MyNotifyEventToken;
	MyNotifyEventToken.reset();

	//スレッドプール内からの呼び出しかどうかで目標スレッド数を決める
	long check_thread_count = ( MyWTP->IsPoolMember() ) ? ( 2 ) : ( 1 );

	//未処理のキュー破棄
	DWORD wait_time = 0;
	while( 1 )
	{
		bool empty = true;

		if( token.use_count() <= check_thread_count )
		{
			mCriticalSectionTicket critical( MyCritical );

			auto QueueClear = []( BufferQueue& queue )->void
			{
				for( BufferQueue::iterator itr = queue.begin() ; itr != queue.end() ; )
				{
					if( (*itr)->Completed )
					{
						mDelete (*itr)->Buffer;
						mDelete (*itr);
						itr = queue.erase( itr );
						continue;
					}
					itr++;
				}
			};
			QueueClear( MyWriteQueue );
			QueueClear( MyReadQueue );

			MyReadCacheRemain = 0;
			MyReadCacheCurrent = 0;
			MyReadCacheHead.reset();

			empty = MyWriteQueue.empty() && MyReadQueue.empty();
		}
		else
		{
			empty = false;
		}

		if( empty && ( token.use_count() <= check_thread_count ) )
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
	CloseHandle( MyHandle );
	MyHandle = INVALID_HANDLE_VALUE;
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

WString mASyncSerialPort::GetPortName( void )const
{
	WString result;
	if( !mFileUtility::SplitPath( MyOption.Fileinfo.Path , nullptr , nullptr , &result , nullptr ) )
	{
		return L"";
	}
	return result;
}
