//----------------------------------------------------------------------------
// Felica Light-S カードハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCFELICAMAC_H_INCLUDED
#define MSCFELICAMAC_H_INCLUDED

#include <mStandard.h>
#include <General/mBinary.h>
#include <bcrypt.h>
#include <memory>
#include <Crypto/mSCFelicaDefinitions.h>

class mSCFelicaMac
{
public:
	mSCFelicaMac();
	virtual ~mSCFelicaMac();

	//暗号プロバイダの初期化
	// ret : 成功時真
	bool Initialize( void );

	//カード鍵の設定
	// key : カード鍵
	//　・ぴったり１６バイトであること
	// retChallenge : 生成したチャレンジ(これをFelicaLiteSのRC(80h)ブロックに送信する)
	// ret : 成功時真
	bool SetKey( const mSecureBinary& key , mBinary& retChallenge );

	//鍵を削除する
	void ClearKey( void );

	//鍵を保持しているか返す
	bool IsKeyExist( void )const;

	//既知の弱鍵でないかチェックします
	// key : 検証対象の鍵(CKに送り込むバイトオーダーで16バイト)
	//        +0         +7     +8         +15
	//       CK1[0],...,CK1[7],CK2[0],...,CK2[7]
	// ret : チェック合格の場合真。チェック不合格の場合(脆弱鍵の場合)偽
	static bool CheckIsWeakKey( const mSecureBinary& key );

	//読み書きを行うブロックのエントリ
	using DataBlockEntry = mSCFelicaDefinitions::DataBlockEntry;
	//読み書きを行うブロックのデータ
	using DataBlock = mSCFelicaDefinitions::DataBlock;

	//MAC_Aメッセージブロックを用いたメッセージの認証を行います
	//あらかじめSetKeyでカード鍵を設定し、取得したチャレンジをカードに送信した上でやりとりしたメッセージブロックが対象です。
	// data : 認証するメッセージ
	//　・最後のブロックはMAC_A(91h)であること
	//　・その他細々としたMAC_A認証の要件を満たしていること
	// ret : 認証が成功すれば真
	bool ValidateMacA( const DataBlock& data );

	//メッセージに対するMAC_Aメッセージブロックを生成する
	//あらかじめSetKeyでカード鍵を設定し、取得したチャレンジをカードに送信しておく必要があります。
	// data : 認証するメッセージ
	//　・このメッセージに対応するMAC_Aブロックを算出します
	// retMacA : 算出したMAC_Aブロック
	// wcnt : 現在のWCNTの値
	// ret : 生成に成功すれば真
	bool CreateMacA( const DataBlockEntry& data , DataBlockEntry& retMacA , DWORD wcnt );

	//個別化マスター鍵から個別化カード鍵を導きます
	// master : マスター鍵(ぴったり24バイトである必要があります)
	// id : カードのIDブロックのバイナリ(16バイト)
	// retcardkey : 導出したカード鍵
	static bool CalcDiversifiedKey( const mSecureBinary& master , const mBinary& id , mSecureBinary& retcardkey );

private:

	mSCFelicaMac( const mSCFelicaMac& source );
	const mSCFelicaMac& operator=( const mSCFelicaMac& source ) = delete;

protected:

	//暗号プロバイダのハンドル
	BCRYPT_ALG_HANDLE MyAlgHandle;

	//チャレンジの値
	mBinary MyChallenge;

	//暗号鍵のハンドル(リード用)
	BCRYPT_KEY_HANDLE MyRKeyHandle;

	//キーオブジェクト(リード用)
	std::unique_ptr<BYTE> MyRKeyObject;

	//暗号鍵のハンドル(ライト用)
	BCRYPT_KEY_HANDLE MyWKeyHandle;

	//キーオブジェクト(ライト用)
	std::unique_ptr<BYTE> MyWKeyObject;

	//セッション鍵の生成を行う
	// key : カード鍵のバイナリ
	// ret : 成功時真
	bool CreateSessionKey( const mSecureBinary& key );

};
	
#endif
