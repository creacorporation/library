//----------------------------------------------------------------------------
// RSA暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MRSA_H_INCLUDED
#define MRSA_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>
#include <memory>
#include "../General/mTCHAR.h"

//RSA鍵管理クラス
//＜できること＞
//・鍵ペアの管理をすることができます
//・キーコンテナを使わず、テンポラリなカギを作ることができます(この場合電子署名は不可)
//・新しい鍵を作ることができます
//・作成した鍵をエクスポートできます
//・鍵をインポートできます
//＜注意＞
//・暗号化を行うには、派生クラスのmRSACipherを使います
//・電子署名を行うには、派生クラスのmRSAVerifierを使います

//参考になるかもしれないURL
// http://eternalwindows.jp/crypto/csp/csp00.html
// http://msdn.microsoft.com/en-us/library/aa380252(v=vs.85).aspx

class mRSA
{
public:
	mRSA();
	virtual ~mRSA();

	//鍵の種別(公開鍵/秘密鍵)
	enum KEYTYPE
	{
		KEY_PUBLIC,			//公開鍵
		KEY_PRIVATE,		//秘密鍵
	};

	//鍵のビット長
	enum KEYLENGTH
	{
		KEYLEN_2048BIT,		//2048bit
		KEYLEN_4096BIT,		//4096bit
	};

	//初期化
	//この初期化メソッドを使用した場合、
	//・鍵は自分でエクスポートしない限り保存されない
	//・署名には使えない
	//len : 鍵のビット数
	bool Init( void );

	//初期化
	//この初期化メソッドを使用した場合、
	//・指定した名前のキーコンテナが作成される
	//・すでに同名のキーコンテナが存在する場合、そのキーコンテナのキーをロードする
	//len : 鍵のビット数
	//container_name : キーコンテナの名前
	//machine_keyset : 真の場合、キーコンテナをコンピュータに関連付ける（サービスプログラム用）
	//				   偽の場合、キーコンテナをユーザに関連付ける（一般アプリ用）
	bool Init( const WString& container_name , bool machine_keyset );

	//キーコンテナを破棄する
	//container_name : 削除したいキーコンテナの名前
	//                 何も指定しない場合、デフォルトのキーコンテナを指定したことになる
	//machine_keyset : 真の場合、キーコンテナをコンピュータに関連付ける（サービスプログラム用）
	//				   偽の場合、キーコンテナをユーザに関連付ける（一般アプリ用）
	//※指定したキーコンテナに含まれるキーはすべて削除されるので注意
	static bool DestroyKeyContainer( const WString& container_name , bool machine_keyset );

	//新しい鍵ペアを作る
	//現在の鍵は破棄する
	//※ハンドルを破棄するだけで、コンテナを破棄するわけではない
	bool GenerateNewKey( KEYLENGTH len );

	//鍵を破棄する
	//※ハンドルを破棄するだけで、コンテナを破棄するわけではない
	bool Clear( void );

	//鍵バイナリのサイズを得る
	//RSAのビット長ではなく、GetKeyBinaryで結果の格納に必要なバッファのサイズが返る。
	//type : 取得したい鍵(公開鍵 or 秘密鍵)
	DWORD GetKeySize( KEYTYPE type );

	//鍵バイナリ
	typedef std::unique_ptr<BYTE> KeyBinary;

	//鍵バイナリを得る
	//結果はPRIVATEKEYBLOBまたは、PUBLICKEYBLOBフォーマットのバイナリ。
	//type : 取得したい鍵(公開鍵 or 秘密鍵)
	//buffer : 格納先のバッファ
	//len : バッファのサイズ
	//retWritten : 書き込まれたバイト数
	bool ExportKey( KEYTYPE type , KeyBinary& retKey , DWORD& retWritten );

	//鍵をバイナリからインポートする
	//data : 鍵。PRIVATEKEYBLOBまたは、PUBLICKEYBLOBフォーマットのバイナリ。
	//datalen : dataのバイト数
	//成功時真
	bool ImportKey( const BYTE* data , DWORD datalen );

private:
	mRSA( const mRSA& source );
	const mRSA& operator=( const mRSA& source ) = delete;

protected:

	//暗号化プロバイダのハンドル
	HCRYPTPROV	MyCryptProv;		//
	HCRYPTKEY	MyCryptKey;			//公開鍵＋秘密鍵
	HCRYPTKEY	MyCryptKeyPub;		//公開鍵のみ

	//鍵バイナリを得る
	bool GetKey( KEYTYPE type , BYTE* buffer , DWORD& size );

	//MyCryptKeyから公開鍵だけを抽出してMyCryptKeyPubにセットする
	bool ExtractPublicKey( void );

	//鍵バイナリのインポート
	bool ImportKeyInternal( HCRYPTKEY* key , DWORD len , const BYTE* buffer );

};

#ifdef SAMPLE_CODE

int main( int argc , char** argv )
{
	InitializeLibrary();

	//・新しい鍵ペアをつくる
	//※キーコンテナの名前を指定しない場合は、署名に使えない
	mRSA key;
	key.Init( L"Test Program" , false );
	key.GenerateNewKey( mRSA::KEYLENGTH::KEYLEN_2048BIT );

	//・公開鍵をエクスポート
	DWORD pubkey_size;
	mRSA::KeyBinary pubkey;
	key.ExportKey( mRSA::KEYTYPE::KEY_PUBLIC , pubkey , pubkey_size );

	//・秘密鍵をエクスポート
	DWORD privkey_size;
	mRSA::KeyBinary privkey;
	key.ExportKey( mRSA::KEYTYPE::KEY_PRIVATE , privkey , privkey_size );

	//エクスポートした公開鍵を別のオブジェクトにインポートする
	mRSA other_key;
	other_key.Init();
	other_key.ImportKey( pubkey.get() , pubkey_size );

	return 0;
}

#endif

#endif