//----------------------------------------------------------------------------
// Mifare Classic 1K カードハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCMIFARE_H_INCLUDED
#define MSCMIFARE_H_INCLUDED

#include "mSCBase.h"
#include <array>
#include <General/mBinary.h>

class mSCMifare : public mSCBase
{
public:
	mSCMifare();
	virtual ~mSCMifare();

	//パーミッション設定
	enum Permission
	{
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA   ×    ×   KeyA  KeyA  KeyA  KeyA/B  KeyA/B  KeyA/B  KeyA/B
		Permission0,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA   ×    ×   ×    KeyA  ×    KeyA/B  ×      ×      ×
		Permission1,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B ×    ×   KeyB  ×    KeyB  KeyA/B  KeyB    ×      ×
		Permission2,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B ×    ×   ×    ×    ×    KeyA/B  KeyB    KeyB    KeyA/B
		Permission3,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA   KeyA  ×   KeyA  KeyA  KeyA  KeyA/B  ×      ×      KeyA/B
		Permission4,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B KeyB  ×   KeyB  ×    KeyB  KeyB    KeyB    ×      ×
		Permission5,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B KeyB  ×   ×    ×    ×    KeyB    ×      ×      ×
		Permission6,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B ×    ×   ×    ×    ×    ×      ×      ×      ×
		Permission7,
	};

	using Key = std::array<BYTE,6>;
	struct Trailer
	{
		//セクタの鍵Ａです
		//・キーＡを読み出しすことはできないので、読み出し時は必ず00Hになります。
		Key key_a;
		//セクタの鍵Ｂです
		Key key_b;
		//セクタ内の先頭データブロック(3個のうち1個目)に対するパーミッションです
		Permission permission0;
		//セクタ内の真ん中のデータブロック(3個のうち2個目)に対するパーミッションです
		Permission permission1;
		//セクタ内の後ろのデータブロック(3個のうち3個目)に対するパーミッションです
		Permission permission2;
		//トレイラーブロックそのものに対するパーミッションです
		Permission permission_trailer;
		//トレイラーブロックの9バイト目にあるユーザーデータです
		uint8_t userdata;
	};

	//認証キーをセットする
	// key : セットするキー
	// ret : 成功時真
	bool LoadAuthenticationKeys( Key key )const;

	//セットされた認証キーを用いて認証処理を行う
	// sector : 認証を行うセクタ
	//  セクタ０：ブロック０〜３（トレーラーブロックは３）
	//  セクタ１：ブロック４〜７（トレーラーブロックは７）
	//  セクタ２：ブロック８〜１１（トレーラーブロックは１１）
	//　　（中略）
	//  セクタ１５：ブロック６０〜６３（トレーラーブロックは６３）
	// key_a : 真の場合keyAとして認証する。偽の場合keyBとして認証する。
	// ret : 成功時真
	bool ExecAuthentication( DWORD sector , bool key_a )const;

	//１セクタ(48バイト)を読み取る
	//・LoadAuthenticationKeys、ExecAuthenticationを用いて、該当セクタの読み取りが可能な認証を済ませておく必要がある
	// sector : 読み取るセクタ
	// retData : 読み取ったデータ
	// ret : 成功時真
	bool ReadSector( DWORD sector , mBinary& retData )const;

	//指定セクタのトレイラーを取得する
	// sector : 読み取るセクタ
	// retTrailer : 読み取ったデータ
	// ret : 成功時真
	bool ReadTrailer( DWORD sector , Trailer& retTrailer )const;

	//１セクタ(48バイト)を書込む
	//・LoadAuthenticationKeys、ExecAuthenticationを用いて、該当セクタの書き込みが可能な認証を済ませておく必要がある
	// sector : 書込むセクタ
	// data : 書込むデータ
	//  ・48バイトに満たない場合は足りない分00Hを補う
	//  ・48バイトを超える場合は、48バイトを超えた分は切り捨てる
	// ret : 成功時真
	bool WriteSector( DWORD sector , const mBinary& data )const;

	//指定セクタのトレイラーを書込む
	// sector : 書込むセクタ
	// trailer : 書込むデータ
	// ret : 成功時真
	bool WriteTrailer( DWORD sector , const Trailer& trailer )const;

protected:

	//接続時のカード個別の処理
	// ret : 処理成功時真
	virtual bool OnConnectCallback( void );

private:
	mSCMifare( const mSCMifare& source );
	const mSCMifare& operator=( const mSCMifare& source ) = delete;

	//送信データに対して、成否しか返してこないコマンドの送受信処理をまとめたヘルパー関数
	// dt : 送信するデータ
	// ret : 処理が成功した場合真(WinAPI、カード側処理両方成功して真)
	bool DefaultCommunication( const TransmitData& dt )const;

};

#ifdef SAMPLE_CODE

#include "mStandard.h"
#pragma comment (lib, "winscard.lib")
#pragma comment (lib, "scarddlg.lib")
#include <Crypto/mSCReader.h>
#include <Crypto/mSCMifare.h>

//スマートカードがセットされたときのコールバック
void OnPresent( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	//セットされたスマートカードがMifare1Kの場合
	if( opt->OnCardPresent->Kind == Definitions_SCReader::CardKind::MIFARE_CLASSIC_1K )
	{
		//カードと接続する
		mSCMifare card;
		if( !card.Connect( sc.GetCurrentCardReaderName() ) )
		{
			return;
		}
		//認証キーをセット
		//※ここでは、セクタ０、１ともにデフォルト（全てFF）がセットされていると仮定
		card.LoadAuthenticationKeys( mSCMifare::Key( {0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu} ) );

		//※セクタ０を読み取って、キーを書き換えるテスト
		//セクタ０の認証実施(以降エラーチェック省略)
		if( !card.ExecAuthentication( 0 , true ) )
		{
			//認証失敗
			return;
		}
		//セクタ０のデータを読み取り
		mBinary data;
		card.ReadSector( 0 , data );
		//セクタ０のトレイラーを読み取り
		mSCMifare::Trailer trailer;
		card.ReadTrailer( 0 , trailer );
		//セクタ０のキーＡを別のものに書き換える
		//trailer.key_a = mSCMifare::Key( {0x12u,0x34u,0x56u,0x78u,0x9Au,0xBCu} );
		//card.WriteTrailer( 0 , trailer );

		//※セクタ１のデータを更新するテスト
		//セクタ１を認証
		card.ExecAuthentication( 1 , true );
		//セクタ１のデータを読み取り
		card.ReadSector( 1 , data );
		//セクタ１の１バイト目を更新
		data[ 0 ]++;
		//セクタ１を書込み
		card.WriteSector( 1 , data );
	}
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

	//監視開始
	reader.StartWatch( cardreader[ 0 ] , notifyopt );

	//待機。これはサンプルなので単なるウエイト。
	DWORD flag = 1;
	while( flag )
	{
		Sleep( 1000 );
	}

	//クリーンアップ
	reader.EndWatch();
	return 0;
}


#endif

#endif


