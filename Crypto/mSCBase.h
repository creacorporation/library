//----------------------------------------------------------------------------
// スマートカードハンドラベース
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCBASE_H_INCLUDED
#define MSCBASE_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include <General/mBinary.h>

#include <winscard.h>
#pragma comment (lib, "winscard.lib")


namespace Definitions_SCBase
{
	//カードリーダーのメーカー
	enum ReaderMaker
	{
		READER_MAKER_GENERAL,	//不明（初期化時に指定するとリーダー名から推定）
		READER_MAKER_ACS,		//Advanced Card Systems(ACS12xx等)
		READER_MAKER_SONY,		//ソニー(PaSoRi等)
	};
};

class mSCBase
{
public:
	mSCBase();
	virtual ~mSCBase();

	using ReaderMaker = Definitions_SCBase::ReaderMaker;

	//スマートカードに接続する
	// reader : 接続先のスマートカードがつながっているリーダーの名前
	// maker : スマートカードリーダーのメーカー
	// ret : 成功時真
	bool Connect( const WString& reader , ReaderMaker maker = ReaderMaker::READER_MAKER_GENERAL );

	//カードのIDを取得する
	const mBinary& GetCardId( void )const;

private:

	mSCBase( const mSCBase& source );
	const mSCBase& operator=( const mSCBase& source ) = delete;

	//スマートカードサブシステムのハンドル
	SCARDCONTEXT MySCardContext;

	//スマートカードのハンドル
	SCARDHANDLE MySCard;

	//スマートカードとの通信プロトコル
	enum Protocol
	{
		T0,	//T=0プロトコル
		T1,	//T=1プロトコル
		Unknwon
	};

	//アクティブなプロトコル
	Protocol MyActiveProtocol;

	//カードリーダーのメーカー
	ReaderMaker MyReaderMaker;

protected:

	//カードID
	mBinary MyCardID;

	//接続時のカード個別の処理
	// ret : 処理成功時真
	virtual bool OnConnectCallback( void );

	struct TransmitData
	{
		uint8_t cla;	//インストラクションクラス
		uint8_t ins;	//インストラクションコード
		uint8_t p1;		//パラメータ１
		uint8_t p2;		//パラメータ２
		mBinary data;	//追加送信データ（インストラクションにより必要）
		TransmitData()
		{
			cla = 0xFFu;
			ins = 0;
			p1 = 0;
			p2 = 0;
		}
		void SetData( const BYTE* dt , DWORD size )
		{
			data.clear();
			for( DWORD i = 0 ; i < size ; i++ )
			{
				data.push_back( dt[ i ] );
			}
		}
		TransmitData( const TransmitData& src )
		{
			*this = src;
		}
		const TransmitData& operator=( const TransmitData& src )
		{
			cla = src.cla;
			ins = src.ins;
			p1 = src.p1;
			p2 = src.p2;
			data = src.data;
		}
	};

	struct TransmitDataLen
	{
		uint8_t cla;		//インストラクションクラス
		uint8_t ins;		//インストラクションコード
		uint8_t p1;			//パラメータ１
		uint8_t p2;			//パラメータ２
		uint8_t len;		//データ長
		TransmitDataLen()
		{
			cla = 0xFFu;
			ins = 0;
			p1 = 0;
			p2 = 0;
			len = 0;
		}
	};

	struct ResponseData
	{
		mBinary data;	//応答データ
	};

	//スマートカードと通信を行う
	// dt : 送信するデータ
	// retResponse : 受信したデータ
	// ret : 処理が成功した場合真
	//       関数の返値はあくまでWinAPIが成功したかどうかであり、スマートカード側で処理が成功したかはレスポンスで判断する。
	bool Communicate( const TransmitData& dt , ResponseData& retResponse )const;

	//スマートカードと通信を行う
	//※追加送信データはないが、データ長に意味がある場合に使用する
	// dt : 送信するデータ
	// retResponse : 受信したデータ
	// ret : 処理が成功した場合真
	//       関数の返値はあくまでWinAPIが成功したかどうかであり、スマートカード側で処理が成功したかはレスポンスで判断する。
	bool Communicate( const TransmitDataLen& dt , ResponseData& retResponse )const;

	//スマートカードリーダーと通信を行う
	// controlcode : コントロールコード
	// senddata : 送信するデータ
	// retResponse : 受信したデータ(不要ならnull可)
	// ret : 処理が成功した場合真
	//       関数の返値はあくまでWinAPIが成功したかどうかであり、スマートカードリーダー側で処理が成功したかはレスポンスで判断する。
	bool Control( DWORD controlcode , const mBinary& senddata , mBinary* retResponse = nullptr )const;

	//スマートカードリーダーのメーカーを取得する
	// ret : 取得したメーカー
	//初期化時にユーザーが指定したか、リーダー名から推定したメーカーが返ります
	//不明な場合はREADER_MAKER_GENERALが返ることがあります
	ReaderMaker QueryMaker( void )const;

	//スマートカードリーダーの種類に対応した直接通信コマンドを設定する
	// retPacket : コマンドの設定先
	void SetDirectCommand( TransmitData& retPacket ) const;

	//カード側と直接通信をするためのオブジェクト(Transparent Exchange)
	class TransparentSession
	{
	public:
		TransparentSession( const mSCBase& base );
		~TransparentSession();

		struct TransarentResponse
		{
			//エラーコード
			//形式：0x00AABBCC(AA=エラー位置,BB=SW1,CC=SW2)
			//・C0タグの内容がそのまま入る。
			//・エラーなしは0x00009000。
			uint32_t ErrorCode;

			//エラー内容(エラーコードをenum化したもの)
			enum class ErrorDescriptionCode
			{
				NoError,
				InformationNotAvailable,
				NoInformation,
				ExcecutionStopped,
				NotSupported,
				UnexpectedLength,
				UnexpectedValue,
				IFDNoResponse,
				ICCNoResponse,
				NoPreciseDiagnosis,
				Unknown,
			};
			ErrorDescriptionCode ErrorDescription;

			//CRCが正常なら真
			bool IsCrcOK = false;

			//コリジョンが発生しなかったら真
			bool IsCollisionOK = false;

			//パリティが正常なら真
			bool IsParityOK = false;

			//フレーミングエラーがなければ真
			bool IsFramingOK = false;

			//最終バイトの何ビット目まで有効か（０は全ビット有効）
			uint8_t ResponseBitFraming = (uint8_t)0;

			//コリジョンが発生している場合のコリジョン位置
			uint8_t CollisionPos = 0;

		};

		bool Communicate( const mBinary& in , mBinary& retout , TransarentResponse* retresponse = nullptr )const;

	private:
		TransparentSession( const TransparentSession& source ) = delete;
		const TransparentSession& operator=( const TransparentSession& source ) = delete;
		const mSCBase& MyCard;
		bool MyIsValid = false;

	protected:

	};
};


#endif


