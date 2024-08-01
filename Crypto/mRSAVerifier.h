//----------------------------------------------------------------------------
// RSA暗号化クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MRSAVERIFIER_H_INCLUDED
#define MRSAVERIFIER_H_INCLUDED

#include "../mStandard.h"
#include "mRSA.h"
#include "mHash.h"
#include <memory>

class mRSAVerifier : public mRSA
{
public:
	mRSAVerifier();
	virtual ~mRSAVerifier();

	//署名書き込み用バッファ
	typedef std::unique_ptr<BYTE> SignBuffer;

	//署名を行う
	//必要な鍵：秘密鍵
	//data : 署名を行うハッシュ値
	//retSign : 署名を格納するバッファ
	//retWritten : 書き込まれたバイト数
	//ret : 成功時真
	bool Sign( const mHash& data , SignBuffer& retSign , DWORD& retWritten )const;

	//検証を行う
	//→正当に署名されているかを判定します
	//必要な鍵：公開鍵
	//data : 署名されているデータのハッシュ
	//Sign : 署名
	//SignLen : 署名の長さ
	//ret : 検証成功時真
	bool Verify( const mHash& data , const BYTE* Sign , DWORD SignLen )const;

private:
	mRSAVerifier( const mRSAVerifier& source );
	const mRSAVerifier& operator=( const mRSAVerifier& source ) = delete;

protected:

	//渡されたmHashオブジェクトと同一ハッシュ値をもつハンドルを作る
	//作成したハンドルのプロバイダは、このインスタンスのプロバイダを使う
	//data : コピー元のオブジェクト
	//retHash : コピーの格納先
	//ret : 成功時真
	bool DupeHCRYPTHASH( const mHash& data , HCRYPTHASH& retHash )const;
};

#ifdef SAMPLE_CODE
//使い方
int main( int argc , char** argv )
{
	InitializeLibrary();

	//＜署名＞
	//・データのハッシュ値を求める
	BYTE Data1[] = "Secret Data";
	mHash hash;
	hash.Init( mHash::HashAlgorithm::SHA256 );
	hash.Hash( Data1 , sizeof( Data1 ) );

	//・署名用のオブジェクトを作り、新しい鍵ペアをつくる
	//・キーコンテナの名前を指定しない場合は、署名に使えない
	mRSAVerifier sign;
	sign.Init( L"Test Program" , false );
	sign.GenerateNewKey( mRSA::KEYLENGTH::KEYLEN_2048BIT );

	//・公開鍵をエクスポート（署名検証時に使う）
	DWORD pubkey_size;
	mRSA::KeyBinary pubkey;
	sign.ExportKey( mRSA::KEYTYPE::KEY_PUBLIC , pubkey , pubkey_size );

	//・署名する
	mRSAVerifier::SignBuffer signbuffer;
	DWORD signlen;
	sign.Sign( hash , signbuffer , signlen );

	//＜検証＞
	//・データのハッシュ値を求める
	hash.Init( mHash::HashAlgorithm::SHA256 );
	hash.Hash( Data1 , sizeof( Data1 ) );

	//・検証時はキーコンテナを作る必要はない
	//・検証側には秘密鍵はない前提なので、ここでは公開鍵のみインポートする
	//・Verify()で検証する
	mRSAVerifier verify;
	verify.Init();
	verify.ImportKey( pubkey.get() , pubkey_size );
	verify.Verify( hash , signbuffer.get() , signlen );

	return 0;
}
#endif

#endif