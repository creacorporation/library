//----------------------------------------------------------------------------
// ハッシュ処理クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MHMAC_H_INCLUDED
#define MHMAC_H_INCLUDED

#include "mStandard.h"
#include "mHash.h"
#include "../General/mErrorLogger.h"

/*
<使い方>

int main( int argc , const char** argv )
{
	//key =           "Jefe"
	//key_len =       4
	//data =          "what do ya want for nothing?"
	//data_len =      28
	//digest =        0xeffcdf6ae5eb2fa2d27416d5f184df9c259a7c79

	mHmac hmac;
	BYTE Data1[] = { 'J' , 'e' , 'f' , 'e' };
	BYTE Data2[] = {
		'w' , 'h' , 'a' , 't' , ' ' , 'd' , 'o' , ' ' , 'y' , 'a' ,
		' ' , 'w' , 'a' , 'n' , 't' , ' ' , 'f' , 'o' , 'r' , ' ' ,
		'n' , 'o' , 't' , 'h' , 'i' , 'n' , 'g' , '?'
	};

	hmac.Init( mHash::HashAlgorithm::SHA1 , Data1 , sizeof( Data1 ) );
	hmac.Hash( Data2 , sizeof( Data2 ) );

	AString digest1;
	hmac.GetResult( digest1 );
	return 0;
}
*/

class mHmac
{
public:

	mHmac();
	virtual ~mHmac();

	//初期化
	//algo : 使用したいハッシュアルゴリズム
	//key : 暗号化キー
	//keylen : keyのバイト数
	//openssl : 真：OpenSSLと同じHMACを生成します。
	//			偽：キーを一旦RC4でスクランブルしてからHMACを生成します(WindowsAPIのサンプルと同じ)。
	//ret : 成功時真
	bool Init( mHash::HashAlgorithm algo , const BYTE* key , DWORD keylen , bool openssl = true );

	//リセット
	//オブジェクトをリセットする。暗号化キーはリセットされない。
	//Initで初期化→HMAC計算→Reset→次のHMAC計算…のようにオブジェクトを再利用できる
	bool Reset( void );

	//ハッシュを実行
	//大きなデータの場合はコマ切れにして呼び出してよい。
	// len : データのバイト数
	// data : データへのポインタ
	// ret : 成功時真
	bool Hash( const BYTE* data , DWORD len );

	//結果のサイズを得る
	//ret : 結果のバイト数(エラーの場合0)
	DWORD GetResultLen( void )const;

	//ハッシュ値バイナリの格納先
	typedef std::unique_ptr<BYTE> HmacData;

	//結果を得る
	//結果格納先のバッファは自動的に確保され、スマートポインタに設定されます。
	//retResult : 結果の格納先
	//retLen : 結果のバイト数
	//ret:成功時真
	bool GetResult( HmacData& retResult , DWORD& retLen )const;

	//結果を得る
	//結果格納先のバッファは、呼び出し元が確保し、また、使用後は解放する必要があります。
	//len:結果格納先のバイト数
	//retResult:結果格納先
	//ret:成功時真
	bool GetResult( BYTE* retResult , DWORD len )const;

	//結果を得る
	//retResult : ハッシュ値の16進文字列の格納先
	//ret : 成功時真
	bool GetResult( AString& retResult )const;

private:

	mHmac( const mHmac& src );
	const mHmac& operator=( const mHmac& src ) = delete;

protected:

	mHash MyHashObject;

	HCRYPTKEY MyCryptKey;	//HMAC生成用のキー
	HMAC_INFO MyHmacInfo;	//HMAC生成用の情報（パディング等）

	void ReleaseHashObject( void );
};


#endif 