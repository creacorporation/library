//----------------------------------------------------------------------------
// RSA暗号化クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MRSACIPHER_H_INCLUDED
#define MRSACIPHER_H_INCLUDED

#include "../mStandard.h"
#include "MRSA.h"
#include <memory>
class mRSACipher : public mRSA
{
public:
	mRSACipher();
	virtual ~mRSACipher();

	//暗号化済みデータの格納先
	typedef std::unique_ptr<BYTE> EncryptData;

	//暗号化する
	//type : 暗号化に使う鍵(公開鍵 or 秘密鍵)
	//data : 暗号化するデータ
	//       ※(RSAbit数÷8)-11 バイトがデータ長の限界。
	//       2048bitRSA の場合 245バイト
	//       4096bitRSA の場合 501バイト
	//datalen : dataの長さ
	//retEncrypted : 暗号化したデータの格納先
	//retwritten : 暗号化の結果、retEncryptedに書き込まれたサイズ。
	bool Encrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retEncrypted , DWORD& retWritten )const;

	//復号する
	//type : 復号に使う鍵(公開鍵 or 秘密鍵)
	//data : 復号するデータ
	//datalen : dataの長さ(1024bitRSAなら128バイト、2048bitRSAなら256バイトになる筈)
	//bufflen : バッファのサイズ
	//retwritten : 復号の結果、dataに書き込まれたサイズ。
	bool Decrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retDecrypted , DWORD& retWritten )const;

private:

	mRSACipher( const mRSACipher& source );
	const mRSACipher& operator=( const mRSACipher& source ) = delete;


};

#endif
