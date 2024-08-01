//----------------------------------------------------------------------------
// ストリーミングファイル書き込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MFILEWRITESTREAMBASE_H_INCLUDED
#define MFILEWRITESTREAMBASE_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "mFileReadStreamBase.h"
#include <memory>

//ストリーミングファイル読み込み操作

class mFileWriteStreamBase
{
public:
	mFileWriteStreamBase();
	virtual ~mFileWriteStreamBase();

	//ファイルを閉じます
	virtual bool Close( void ) = 0;

	//１文字書き込み
	virtual bool Write( INT data ) = 0;

	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	bool WriteString( const AString& line );	//ANSI文字列を書き込む
	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	bool WriteString( const WString& line );	//UNICODE文字列を書き込む
	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	bool WriteString( const char* line );		//ANSI文字列を書き込む
	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	bool WriteString( const wchar_t* line );	//UNICODE文字列を書き込む

	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	template <typename... args> bool WriteString( const AString& format , const args... va )
	{
		AString str;
		sprintf( str , format.c_str() , va... );
		return WriteString( str );
	}
	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	template <typename... args> bool WriteString( const WString& format , const args... va )
	{
		WString str;
		sprintf( str , format.c_str() , va... );
		return WriteString( str );
	}
	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	template <typename... args> bool WriteString( const char* format , const args... va )
	{
		AString str;
		if( format == nullptr )
		{
			return false;
		}
		sprintf( str , format , va... );
		return WriteString( str );
	}
	//１行書き込みます。
	//改行コードを勝手に付加したりしないので、書き込む文字列の方に改行を含めて下さい。
	//・引数が1個の場合は、引数の文字列をそのまま出力します
	//・引数が2個以上の場合は、1個目を書式指定文字列とし、2個目以降フォーマットした文字列を出力します
	template <typename... args> bool WriteString( const wchar_t* format , const args... va )
	{
		WString str;
		if( format == nullptr )
		{
			return false;
		}
		sprintf( str , format , va... );
		return WriteString( str );
	}

	//指定サイズを書込みます
	//ret : 成功時true
	bool WriteBinary( const BYTE* buffer , size_t size );

	//指定ストリームから読み取ってそのまま書き込みます
	//※指定バイト書き込むか、読み込み元ストリームがEOFになるまで書き込みます
	//fp : 書き込み元
	//sz : 書き込むバイト数
	//retWritten : 書き込んだサイズ(不要ならnullptrで可)
	//ret : 成功時true
	bool WriteStream( mFileReadStreamBase& fp , size_t sz , size_t* retWritten = nullptr );

	//指定ストリームから読み取ってそのまま書き込みます
	//※読み込み元ストリームがEOFになるまで書き込みます
	//fp : 書き込み元
	//retWritten : 書き込んだサイズ(不要ならnullptrで可)
	//ret : 成功時true
	bool WriteStream( mFileReadStreamBase& fp , size_t* retWritten = nullptr );


	//ファイルが開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const = 0;

private:
	mFileWriteStreamBase( const mFileWriteStreamBase& source ) = delete;
	void operator=( const mFileWriteStreamBase& source ) = delete;

protected:

	//キャッシュを書き込み
	virtual bool FlushCache( void ) = 0;

	std::unique_ptr<BYTE[]> MyWriteCacheHead;		//キャッシュしているデータの先頭バイト
	DWORD MyWriteCacheRemain;						//未書き込みのキャッシュのサイズ
	DWORD MyWriteCacheWritten;						//書き込み済みキャッシュのサイズ

};

#endif	//MFILEWRITESTREAMBASE_H_INCLUDED

