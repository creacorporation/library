//----------------------------------------------------------------------------
// AES暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09～
//----------------------------------------------------------------------------


#ifndef MAESDECRYPTER_H_INCLUDED
#define MAESDECRYPTER_H_INCLUDED

#include "../mStandard.h"
#include "mAES.h"
#include <memory>

//暗号化解除クラス
class mAESDecrypter : public mAES
{
public:
	mAESDecrypter();
	virtual ~mAESDecrypter();

	//暗号化データの格納先
	typedef std::unique_ptr<BYTE> EncryptData;

	//暗号化を解除します
	// isfinal : 最後のデータである場合に指定します(trueの場合、パディングの処理を行います)
	// data : 暗号化されているデータへのポインタ。暗号化解除したデータで上書きされます。
	// datalen : dataのサイズ
	// retData : 復号後のデータ格納先
	// retWritten : 暗号化解除後のサイズ。isfinal=trueの場合、入力と出力のサイズが異なることがあります。
	//              サイズが異なる場合は、必ず暗号文より平文のほうが小さくなります。
	bool Decrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retData , DWORD& retWritten )const;

private:
	mAESDecrypter( const mAESDecrypter& source );
	const mAESDecrypter& operator=( const mAESDecrypter& source ) = delete;

};

#endif