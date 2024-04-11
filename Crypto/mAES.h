//----------------------------------------------------------------------------
// AES暗号化クラス
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09～
//----------------------------------------------------------------------------


#ifndef MAES_H_INCLUDED
#define MAES_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>
#include "../General/mTCHAR.h"

//AES専用暗号化クラス
//・簡単仕様
//・AES256bit専用
//・鍵は256ビットのバイナリかパスワードテキスト
//・パスワードテキストで入力した場合は、SHA256(パスワード + salt )で得られるバイナリを鍵とする。

//※メモ
//PKCS#5パディングとは？
//パディングするバイト数の値でパディングする。
//16バイトブロックで12バイト使っているなら、4バイトパディングするので、
//その4バイトを「0x04」でパディングする。
//ブロック長ぴったりなら、次の1ブロックを丸ごとパディングする。

class mAES
{
public:
	mAES();
	virtual ~mAES();

	//初期化
	//key : 暗号化キー(３２バイトのバイナリ)
	//ret : 成功時真
	bool Init( const BYTE* key );

	//初期化
	// key : 暗号化キー(文字列)
	// salt : 暗号化キーに含めるソルト
	// saltsize : ソルトのバイト数
	//ret : 成功時真
	bool Init( const SecureAString& key , const BYTE* salt = nullptr , DWORD saltsize = 0 );

	//暗号化キーを解放し、初期状態に戻します
	void Deinit( void );

	//IVをセットする
	// iv は16バイトのバイナリ
	//ret : 成功時真
	bool SetIV( const BYTE* iv );

	//暗号化キーがセットされているかを返す
	//ret : セットされているとき真
	bool IsInitialized( void )const;

private:
	mAES( const mAES& source );
	const mAES& operator=( const mAES& source ) = delete;

protected:

	HCRYPTPROV	MyCryptProv;	//暗号化プロバイダのハンドル
	HCRYPTKEY	MyCryptKey;		//暗号化キーのハンドル

	//プロバイダの初期化
	//ret : 成功時真
	bool InitProvider( void );

	//初期化関数（内部用）
	// hash : キーをハッシュしたオブジェクト
	// ret : 成功時真
	bool Init( HCRYPTHASH hash );

};

#endif