//----------------------------------------------------------------------------
// AES暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09〜
//----------------------------------------------------------------------------

#ifndef MAESENCRYPTER_H_INCLUDED
#define MAESENCRYPTER_H_INCLUDED

#include "../mStandard.h"
#include "mAES.h"
#include <memory>

class mAESEncrypter : public mAES
{
public:
	mAESEncrypter();
	virtual ~mAESEncrypter();

	//bufflenバイト以下のランダムなサイズの文字列を付加する
	//生成される文字列の最後の文字は\0であり、それより前に\0は含まない。
	// data : ランダムなバイト列を書き込むポインタ
	// bufflen : dataのサイズ(32以上を推奨)
	// retwritten : 実際に書き込まれたバイト数
	bool CreateRandomHeader( BYTE* data , DWORD bufflen , DWORD& retwritten );

	//暗号化データの格納先
	typedef std::unique_ptr<BYTE> EncryptData;

	//暗号化します
	//※isfinal=trueの場合、出力のサイズが入力より増えることがあります。
	// isfinal : 最後のデータである場合に指定します(trueの場合、パディングの処理を行います)
	// data : 暗号化したいデータへのポインタ
	// datalen : dataのサイズ(平文のデータサイズ)
	// retEncryptData : 暗号化データの書き込み先
	// retwritten : 暗号化後のサイズ。
	bool Encrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retEncryptData , DWORD& retWritten )const;

private:
	mAESEncrypter( const mAESEncrypter& source );
	const mAESEncrypter& operator=( const mAESEncrypter& source ) = delete;


};

#endif