//----------------------------------------------------------------------------
// ハッシュ処理クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MHASH_H_INCLUDED
#define MHASH_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>
#include <General/mErrorLogger.h>
#include <General/mFileReadStream.h>
#include <memory>

/*
使用法
(1)Init()で初期化します
(2)Hash()でハッシュを算出したいデータを与えます
(3)GetResultLen()でハッシュ値のバイト数を調べ、結果格納先のバッファを確保します
(4)GetResult()でハッシュ値を取得します
(5)引き続きハッシュを求めたい場合は、(1)から繰り返してください。
　オブジェクトは再利用可能です。
*/

class mHash
{
public:

	mHash();
	virtual ~mHash();

	//対応するハッシュアルゴリズム
	enum HashAlgorithm
	{
		MD5,		//MD5   128bit
		SHA1,		//SHA1   160bit
		SHA256,		//SHA2(SHA256)   256bit
		SHA512,		//SHA512   512bit
	};

	//初期化
	//algo : 使用したいハッシュアルゴリズム
	//ret : 成功時真
	bool Init( HashAlgorithm algo );

	//ハッシュを実行
	//大きなデータの場合はコマ切れにして呼び出してよい。
	// len : データのバイト数
	// data : データへのポインタ
	// ret : 成功時真
	bool Hash( const BYTE* data , DWORD len );

	//ハッシュを実行
	// filename : ハッシュを取得するファイル名
	//　・読み取り可能である必要があります.
	//　・EOFになるまで制御が返ってきません。パイプの名前とかを渡さないでください。
	// ret : 成功時真
	bool Hash( const WString& filename );

	//ハッシュを実行
	// opt : ファイル開くときのオプション
	//　・読み取り可能である必要があります.
	//　・EOFになるまで制御が返ってきません。パイプの名前とかを渡さないでください。
	// ret : 成功時真
	bool Hash( mFile::Option opt );

	//ハッシュを実行
	// fp : ハッシュを取得するストリームオブジェクト
	//　・渡したときの読み取り位置から最後までをハッシュします
	//　・EOFになるまで制御が返ってきません
	// ret : 成功時真
	bool Hash( mFileReadStream& fp );

	//ハッシュを実行
	// fp : ハッシュを取得するストリームオブジェクト
	//　・渡したときの読み取り位置からlenで指定した長さ、または最後までをハッシュします
	//　・EOFになるまで制御が返ってきません
	// len : ハッシュする長さ
	// ret : 成功時真
	bool Hash( mFileReadStream& fp , uint32_t len );

	//結果のサイズを得る
	//ret : 結果のバイト数(エラーの場合0)
	DWORD GetResultLen( void )const;

	//ハッシュ関数のブロックサイズを得る
	//ret : ブロックサイズ(エラーの場合0)
	DWORD GetBlockSize( void )const;

	//ハッシュ値バイナリの格納先
	typedef std::unique_ptr<BYTE> HashData;

	//結果を得る
	//結果格納先のバッファは自動的に確保され、スマートポインタに設定されます。
	//retResult:結果格納先
	//retLen:結果のバイト数
	//ret:成功時真
	bool GetResult( HashData& retResult , DWORD& retLen )const;

	//結果を得る
	//結果格納先のバッファは、呼び出し元が確保し、また、使用後は解放する必要があります。
	//retResult:結果格納先
	//len:結果格納先のバイト数
	//ret:成功時真
	bool GetResult( BYTE* retResult , DWORD len )const;

	//結果を得る
	//retResult : ハッシュ値の16進文字列の格納先
	//ret : 成功時真
	bool GetResult( AString& retResult )const;

	struct HashResult
	{
		HashData Data;
		DWORD Len;
	};

	//結果を得る
	//retResult : ハッシュ値のバイナリ列の格納先
	//ret : 成功時真
	bool GetResult( HashResult& retResult )const;

private:

	mHash( const mHash& src );
	const mHash& operator=( const mHash& src ) = delete;

	friend class mHmac;
	friend class mRSAVerifier;

protected:

	HashAlgorithm MyHashAlgorithm;
	HCRYPTPROV MyCryptProvider;
	HCRYPTHASH MyCryptHash;

	//ハッシュオブジェクトを破棄する
	void ReleaseHashObject( void );

	//ハッシュアルゴリズムのIDを取得する
	ALG_ID HashAlgorithm2AlgId( HashAlgorithm alg )const;

};


#endif 