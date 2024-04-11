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

/*
●用途
ストリーミング的にファイルを書き込みます。

使い方：
void TestFunction( void )
{
	//ファイルを開くときの情報
	//※内部的にキャッシュをもっているため、
	//  他からの書き込みアクセスがあると結果がおかしくなります。
	mFile::Option opt;
	opt.Path = L"d:\\test.dat";	//ファイル名
	opt.AccessWrite = true;		//書き込みアクセスは必須
	opt.ShareWrite = false;		//他からの書き込みアクセスを禁止

	//ファイルを開く
	mFileReadStream fp;
	fp.Open( opt );

	fp.Write( L'h' );
	fp.Write( L'o' );
	fp.Write( L'g' );
	fp.Write( L'e' );

	fp.WriteString( L"fuga" );

	WString str = L"piyo\r\n";
	fp.WriteString( str );
}
*/

#ifndef MFILEWRITESTREAM_H_INCLUDED
#define MFILEWRITESTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileWriteStreamBase.h"
#include "mFile.h"
#include "mTCHAR.h"

//ストリーミングファイル読み込み操作

class mFileWriteStream : public mFileWriteStreamBase
{
public:
	mFileWriteStream();
	virtual ~mFileWriteStream();

	//ファイルを開くときの情報
	typedef mFile::CreateMode CreateMode;
	typedef mFile::Option Option;

	//一回のシステムコールで読み取るファイルサイズ。
	static const DWORD MAX_BUFFER_SIZE = 65536;

	//ファイルを開きます。
	// opt : ファイルを開くときのオプション
	//       書き込みストリームのため、opt.AccessRead = trueとすること
	virtual bool Open( const mFile::Option& opt );

	//ファイルを閉じます
	virtual bool Close( void );

	//１文字書き込み
	virtual bool Write( INT data );

	//指定の位置にポインタを移動します
	//指定するのは、ファイルの先頭からの位置になります。
	bool SetPointer( ULONGLONG pos );

	//ファイルポインタを前後に移動
	//distance : 移動距離
	//ret : 成功時true
	//指定位置がEOFを超える場合もエラーになりません。
	bool MovePointer( LONGLONG distance );

	//ファイルポインタをファイルの末尾に移動
	bool SetPointerToEnd( void );

	//ファイルポインタをファイルの先頭に移動
	bool SetPointerToBegin( void );

	//現在の書き込み位置を得ます
	ULONGLONG GetPointer( void )const;

	//ファイルが開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const;

	//開いているパスを返します
	// fullpath : trueの場合、開いているパスをフルパスに変換しようとします
	//            falseの場合、trueでも変換に失敗した場合は、ファイルオープン時に渡したパスがそのまま返ります
	WString GetPath( bool fullpath )const;

	//ファイルのサイズを取得
	bool GetFileSize( ULONGLONG& retSize )const;

	//ファイルのサイズを取得
	// high は不要な場合ヌルでも可。ただしhighに入るべき結果が1以上の時エラーになる。
	bool GetFileSize( DWORD* high , DWORD& low )const;

	//ファイルのサイズを取得
	// エラーの場合は０
	ULONGLONG GetFileSize( void )const;

	//現在の位置をEOFにします
	bool SetEof( void );

private:
	mFileWriteStream( const mFileWriteStream& source ) = delete;
	void operator=( const mFileWriteStream& source ) = delete;

protected:

	//キャッシュを書き込み
	virtual bool FlushCache( void );

	//キャッシュを破棄
	void ResetCache( void );

	mFile MyHandle;			//ファイルのハンドル

};

#endif