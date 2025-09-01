//----------------------------------------------------------------------------
// スマートカードリーダーハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCREADER_H_INCLUDED
#define MSCREADER_H_INCLUDED

#include <mStandard.h>
#include <General/mNotifyOption.h>
#include <General/mTCHAR.h>
#include <General/mBinary.h>
#include <Thread/mThread.h>

#include <winscard.h>
#pragma comment (lib, "winscard.lib")

//動作確認しているリーダー
//・ACR1251CL-NTTCom
//・RC-S380
//※RC-S380を利用している場合、設定アプリで「FeliCaアプリケーションを優先」のチェックを外さないと動かないかもしれない
//  公式のドライバをインストールするとコントロールパネルに追加される「NFCポート／パソリ」のアイコンから設定できる

namespace Definitions_SCReader
{
	//ATRから認識されたカードの種類
	enum CardKind
	{
		//Mifare Classic 1K
		MIFARE_CLASSIC_1K,
		//Felica(Felica Standard/Light/LightSの判別は接続してみないと分からない)
		FELICA,
		//Mifare Ultralight (NTAGもこれ)
		MIFARE_ULTRALIGHT,
		NTAG = MIFARE_ULTRALIGHT,
		//不明・その他
		UNKNOWN
	};

	//カードを認識した場合のイベントです
	struct OnCardPresent
	{
		CardKind Kind;
		DWORD Status;
		mBinary Atr;
	};

	//カードが取り外された場合のイベントです
	struct OnCardRemoved
	{
		DWORD Status;
		mBinary Atr;
	};

	//リーダーが取り外されたと思われる場合のイベントです
	struct OnReaderGone
	{
		DWORD Status;
	};

};

//スマートカードリーダーのハンドラ。以下の機能があります。
//・スマートカードサブシステムが利用可能であるか判定し、利用可能になるまで待機する
//・システムが認識しているスマートカードリーダーの一覧を作成する
//・スマートカードリーダー上にカードが置かれた・外された、リーダーそのものが取り外されたことを検出する
class mSCReader
{
public:
	mSCReader();
	virtual ~mSCReader();

	//カードの種類
	using CardKind = Definitions_SCReader::CardKind;

	//通知時のパラメータ
	union NotifyFunctionOptPtr
	{
		//カードを認識した場合のイベントです
		Definitions_SCReader::OnCardPresent* OnCardPresent;
		//カードが取り外された場合のイベントです
		Definitions_SCReader::OnCardRemoved* OnCardRemoved;
		//リーダーが取り外されたと思われる場合のイベントです
		Definitions_SCReader::OnReaderGone* OnReaderGone;
	};

	//通知設定
	using NotifyFunction = void(*)( mSCReader& sc , DWORD_PTR parameter , NotifyFunctionOptPtr* opt );
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		//カードを認識した場合のイベントです
		NotifierInfo OnCardPresent;
		//カードが取り外された場合のイベントです
		NotifierInfo OnCardRemoved;
		//リーダーが取り外されたと思われる場合のイベントです
		NotifierInfo OnReaderGone;
	};

	//スマートカードサービスの起動を待機する
	// waittime : ミリ秒単位の待機時間
	//             INFINITEで起動まで永久待機
	//             0で現在の状態を確認して直ちに戻る
	// ret : 時間内にサービスが起動したら真
	bool WaitForServiceAvailable( DWORD waittime = 0 );

	//接続されているカードリーダーの一覧を取得する
	// retReader : 接続されているリーダーの一覧
	// ret : 成功時真
	bool GetCardReaderList( WStringVector& retReader )const;

	//カードリーダーの監視を開始する
	// readername : 監視するカードリーダーの名前
	// opt : 通知オプション
	// ret : 成功時真
	bool StartWatch( const WString& readername , const NotifyOption& opt );

	//カードリーダーの監視を終了する
	// ret : 成功時真
	bool EndWatch( void );

	//監視中のカードリーダーの名前を取得する
	const WString& GetCurrentCardReaderName( void )const;

private:
	mSCReader( const mSCReader& source );
	const mSCReader& operator=( const mSCReader& source ) = delete;

protected:

	//スマートカードサブシステムが起動しているかどうかをチェックするためのイベントオブジェクト
	HANDLE MyServiceAvailableSignal;

	//スマートカードサブシステムのハンドル
	mutable SCARDCONTEXT MySCardContext;

	//監視スレッドのハンドル
	mThread* MyThread;

	//監視中のカードリーダーの名前
	WString MyCardReaderName;

	//コンテキストを開く
	bool OpenContext( void )const;


};


#ifdef SAMPLE_CODE

#include "mStandard.h"
#pragma comment (lib, "winscard.lib")
#pragma comment (lib, "scarddlg.lib")
#include <Crypto/mSCReader.h>


//スマートカードがセットされたときのコールバック
void OnPresent( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	return;
}

//スマートカードがリーダーから取り外されたときのコールバック
void OnRemove( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	return;
}

//リーダーが取り外されたと思われる場合のコールバック
//※エラー状態なので、再びリーダーが取り付けられても次のコールバックはありません。監視を再起動する必要があります。
void OnReaderGone( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	return;
}

int main( int argc , char** argv )
{
	//オブジェクトを生成し、スマートカードサービスの起動を制限時間無制限で待機する
	mSCReader reader;
	reader.WaitForServiceAvailable( INFINITE );

	//ＰＣに接続されているカードリーダーの一覧を取得する
	WStringVector cardreader;
	reader.GetCardReaderList( cardreader );

	//取得した一覧の先頭（要素０）のリーダーについて、
	//スマートカードのセット・アンセット、リーダーの取り外しを検出する。
	mSCReader::NotifyOption notifyopt;
	notifyopt.OnCardPresent.Mode = mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
	notifyopt.OnCardPresent.Notifier.CallbackFunction = OnPresent;
	notifyopt.OnCardRemoved.Mode = mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
	notifyopt.OnCardRemoved.Notifier.CallbackFunction = OnRemove;
	notifyopt.OnReaderGone.Mode = mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
	notifyopt.OnReaderGone.Notifier.CallbackFunction = OnReaderGone;

	//監視開始
	reader.StartWatch( cardreader[ 0 ] , notifyopt );

	//待機。これはサンプルなので単なるウエイト。
	while( 1 )
	{
		Sleep( 1000 );
	}

	//クリーンアップ
	reader.EndWatch();
	return 0;
}
#endif

#endif

