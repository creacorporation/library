//----------------------------------------------------------------------------
// スマートカードリーダーハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSCReader.h"
#include <General/mErrorLogger.h>

mSCReader::mSCReader()
{
	MyServiceAvailableSignal = SCardAccessStartedEvent();
	MySCardContext = 0;
	MyThread = nullptr;
}

mSCReader::~mSCReader()
{
	//リーダーの監視中であればやめさせる
	EndWatch();
	//リソースの開放
	if( MyServiceAvailableSignal != NULL )
	{
		SCardReleaseStartedEvent();
		MyServiceAvailableSignal = NULL;
	}
	if( MySCardContext )
	{
		SCardReleaseContext( MySCardContext );
		MySCardContext = 0;
	}
}

bool mSCReader::WaitForServiceAvailable( DWORD waittime )
{
	if( MyServiceAvailableSignal == NULL )
	{
		return false;
	}
	if( WaitForSingleObject( MyServiceAvailableSignal , waittime ) == WAIT_OBJECT_0 )
	{
		return true;
	}
	return false;
}


bool mSCReader::OpenContext( void )const
{
	//初期化済みチェック
	if( MySCardContext )
	{
		return true;
	}

	//接続
	LONG result = SCardEstablishContext( SCARD_SCOPE_USER , nullptr , nullptr , &MySCardContext );
	switch( result )
	{
	case SCARD_S_SUCCESS:
		break;
	case SCARD_E_NO_SERVICE:
		RaiseError( g_ErrorLogger , result , L"スマートカードサービスが起動していません" );
		return false;
	default:
		RaiseError( g_ErrorLogger , result , L"スマートカードの初期化が失敗しました" );
		return false;
	}
	return true;
}

bool mSCReader::GetCardReaderList( WStringVector& retReader )const
{
	retReader.clear();

	//初期化済みチェック
	if( !OpenContext() )
	{
		return false;
	}

	//カードリーダーの一覧を取得する
	BYTE* buffer = nullptr;
	DWORD length = SCARD_AUTOALLOCATE;
	LONG result = SCardListReadersW( MySCardContext , nullptr , (LPWSTR)&buffer, &length);
	switch( result )
	{
	case SCARD_S_SUCCESS:
		break;
	case SCARD_E_NO_READERS_AVAILABLE:
		RaiseError( g_ErrorLogger , result , L"カードリーダーが接続されていません" );
		return false;
	case SCARD_E_READER_UNAVAILABLE:
		RaiseError( g_ErrorLogger , result , L"カードリーダーが利用できません" );
		return false;
	default:
		RaiseError( g_ErrorLogger , result , L"カードリーダーの名前を取得できません" );
		return false;
	}

	//ダブルヌルの文字列をパース
	ReadDoubleNullString( (const WCHAR*)buffer , retReader );
	SCardFreeMemory( MySCardContext , buffer );
	return true;
}

//監視スレッド
class SCReaderWatchThread : public mThread
{
public:
	SCReaderWatchThread( mSCReader& reader , SCARDCONTEXT context , const WString& readername , const mSCReader::NotifyOption& opt )
		: MyReader( reader )
		, MySCardContext( context )
		, MyReaderName( readername )
		, MyOption( opt )
	{
	}
	virtual ~SCReaderWatchThread()
	{
	}
	virtual unsigned int TaskFunction() override
	{
		DWORD waittime = 0;

		SCARD_READERSTATEW stat = { 0 };
		stat.szReader = MyReaderName.c_str();
		stat.dwCurrentState = SCARD_STATE_UNAWARE;

		while( 1 )
		{
			//終了要求のチェック
			DWORD code = WaitForSingleObject( MyTerminateSignal , 0 );
			switch( code )
			{
			case WAIT_OBJECT_0:	//終了要求あり
				return 0;
			case WAIT_TIMEOUT:	//終了要求なし
				break;
			default:			//エラー
				RaiseError( g_ErrorLogger , 0 , L"スレッド状態が異常です" );
				return GetLastError();
			}

			//スマートカードリーダーの状態をチェックする
			LONG result = SCardGetStatusChangeW( MySCardContext , waittime , &stat , 1 );
			switch( result )
			{
			case SCARD_S_SUCCESS:
				StatusChangeHandler( *this , stat );
				break;
			case SCARD_E_CANCELLED:
				break;
			default:
				RaiseError( g_ErrorLogger , result , L"スマートカードの監視が失敗しました" );
				return result;
			}

			stat.dwCurrentState = stat.dwEventState;
			waittime = INFINITE;
		}
	}

private:
	SCReaderWatchThread();
	SCReaderWatchThread( const SCReaderWatchThread& source );
	const SCReaderWatchThread& operator=( const SCReaderWatchThread& source ) = delete;

protected:
	mSCReader& MyReader;
	const SCARDCONTEXT MySCardContext;
	const WString MyReaderName;
	const mSCReader::NotifyOption MyOption;

private:

	using CardKind = Definitions_SCReader::CardKind;
	
	//カードの種類を判別する
	static CardKind GetCardKind( const mBinary& atr )
	{
		//                                    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F   10   11   12   13
		static const BYTE AtrMifare1K[] = {0x3B,0x8F,0x80,0x01,0x80,0x4F,0x0C,0xA0,0x00,0x00,0x03,0x06,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x6A};
		static const BYTE AtrFelica[]   = {0x3B,0x8F,0x80,0x01,0x80,0x4F,0x0C,0xA0,0x00,0x00,0x03,0x06,0x11,0x00,0x3B,0x00,0x00,0x00,0x00,0x42};
		static const BYTE AtrMifareUL[] = {0x3B,0x8F,0x80,0x01,0x80,0x4F,0x0C,0xA0,0x00,0x00,0x03,0x06,0x03,0x00,0x3A,0x00,0x00,0x00,0x00,0x51};

		auto Compare = []( const mBinary& atr1 , const BYTE* atr2 , DWORD atrlen ) -> bool
		{
			if( atrlen != atr1.size() )
			{
				return false;
			}
			for( DWORD i = 0 ; i < atrlen ; i++ )
			{
				if( atr1.at( i ) != atr2[ i ] )
				{
					return false;
				}
			}
			return true;
		};

		//Mifare1Kか？
		if( Compare( atr , AtrMifare1K , sizeof( AtrMifare1K ) ) )
		{
			return CardKind::MIFARE_CLASSIC_1K;
		}
		//Felicaか？
		if( Compare( atr , AtrFelica , sizeof( AtrFelica ) ) )
		{
			return CardKind::FELICA;
		}
		//MifareUltraLightか？
		if( Compare( atr , AtrMifareUL , sizeof( AtrMifareUL ) ) )
		{
			return CardKind::MIFARE_ULTRALIGHT;
		}
		//その他のカード
		return CardKind::UNKNOWN;
	}

	static void StatusChangeHandler( SCReaderWatchThread& me , SCARD_READERSTATEW& stat )
	{
		auto AtrCopy = []( DWORD size , const BYTE* bindata , mBinary& retAtr ) -> void
		{
			retAtr.clear();
			for( DWORD i = 0 ; i < size ; i++ )
			{
				retAtr.push_back( *bindata );
				bindata++;
			}
		};

		if( ( stat.dwEventState & SCARD_STATE_PRESENT ) && !( stat.dwCurrentState & SCARD_STATE_PRESENT ) )
		{
			Definitions_SCReader::OnCardPresent info;
			info.Status = stat.dwEventState;
			AtrCopy( stat.cbAtr , stat.rgbAtr , info.Atr );
			info.Kind = GetCardKind( info.Atr );

			mSCReader::NotifyFunctionOptPtr ptr;
			ptr.OnCardPresent = &info;
			AsyncEvent( me.MyReader , me.MyOption.OnCardPresent , &ptr );
		}
		else if( !( stat.dwEventState & SCARD_STATE_PRESENT ) && ( stat.dwCurrentState & SCARD_STATE_PRESENT ) )
		{
			Definitions_SCReader::OnCardRemoved info;
			info.Status = stat.dwEventState;
			AtrCopy( stat.cbAtr , stat.rgbAtr , info.Atr );

			mSCReader::NotifyFunctionOptPtr ptr;
			ptr.OnCardRemoved = &info;
			AsyncEvent( me.MyReader , me.MyOption.OnCardRemoved , &ptr );
		}
		if( 
			( stat.dwEventState & SCARD_STATE_IGNORE ) ||
			( stat.dwEventState & SCARD_STATE_UNAVAILABLE ) )
		{
			Definitions_SCReader::OnReaderGone info;
			info.Status = stat.dwEventState;

			mSCReader::NotifyFunctionOptPtr ptr;
			ptr.OnReaderGone = &info;
			AsyncEvent( me.MyReader , me.MyOption.OnReaderGone , &ptr );
		}
		return;
	}

	//コールバック
	static void AsyncEvent( mSCReader& view , const mSCReader::NotifyOption::NotifierInfo& info , mSCReader::NotifyFunctionOptPtr* opt )
	{
		if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_NONE )
		{
			//do nothing
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
		{
			::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&view , info.Parameter );
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
		{
			info.Notifier.CallbackFunction( view , info.Parameter , opt );
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
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
		return;
	}
};

//カードリーダーの監視を開始する
bool mSCReader::StartWatch( const WString& readername , const NotifyOption& opt )
{
	//二重起動チェック
	if( MyThread )
	{
		RaiseError( g_ErrorLogger , 0 , L"すでにカードリーダーの監視中です" );
		return false;
	}

	//初期化済みチェック
	if( !OpenContext() )
	{
		return false;
	}

	//スレッドの起動
	MyThread = mNew SCReaderWatchThread( *this , MySCardContext , readername , opt );
	if( !MyThread->Begin() )
	{
		goto errorend;
	}
	if( !MyThread->Resume() )
	{
		goto errorend;
	}

	MyCardReaderName = readername;
	return true;

errorend:
	RaiseError( g_ErrorLogger , 0 , L"監視スレッドを起動できませんでした" );
	mDelete MyThread;
	MyThread = nullptr;
	MyCardReaderName = L"";
	return false;
}

//カードリーダーの監視を終了する
bool mSCReader::EndWatch( void )
{
	//起動チェック
	if( !MyThread )
	{
		return true;
	}

	//終了シグナルを送信
	if( !MyThread->FinishRequest() )
	{
		RaiseError( g_ErrorLogger , 0 , L"終了要求が失敗しました" );
		return false;
	}

	//待機中の変更通知をキャンセル
	//＃終了シグナルとの処理順番に注意
	LONG result = SCardCancel( MySCardContext );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードの待機をキャンセルできません" );
		return false;
	}

	//スレッドの終了待機
	if( !MyThread->End() )
	{
		RaiseError( g_ErrorLogger , 0 , L"スレッドの終了ができません" );
		return false;
	}

	mDelete MyThread;
	MyThread = nullptr;
	MyCardReaderName = L"";
	return true;
}

const WString& mSCReader::GetCurrentCardReaderName( void )const
{
	return MyCardReaderName;
}
