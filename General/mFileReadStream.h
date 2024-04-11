//----------------------------------------------------------------------------
// ストリーミングファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------


#ifndef MFILEREADSTREAM_H_INCLUDED
#define MFILEREADSTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileReadStreamBase.h"

/*
●用途
ストリーミング的にファイルを読み取ります。

使い方：
void TestFunction( void )
{
	//ファイルを開くときの情報
	//※内部的にキャッシュをもっているため、
	//  他からの書き込みアクセスがあると結果がおかしくなります。
	mFile::Option opt;
	opt.Path = L"d:\\test.dat";	//ファイル名
	opt.AccessRead = true;		//読み取りストリームのため、読み取りアクセスは必須
	opt.ShareWrite = false;		//他からの書き込みアクセスを禁止

	//ファイルを開く
	mFileReadStream fp;
	fp.Open( opt );

	//1行ずつファイルをテキストとして読み取ります
	while( !fp.IsEOF() )
	{
		//strに1行分読み取ります
		WString str;
		if( !fp.ReadLine( str ) )
		{
			//非同期の場合、ReadLine、Read等がEOFの場合は、
			//ファイルの終端である場合の他に、読み込み中である場合もあります
			//そのため、改めてIsEOFを使って判定する必要があります。
			::Sleep( 100 );
			continue;
		}

		//strの内容を出力します
		printf( "%s" , str.c_str() );
	}
}
*/


class mFileReadStream : public mFileReadStreamBase
{
public:
	mFileReadStream();
	virtual ~mFileReadStream();

	//ファイルを開くときの情報
	typedef mFile::CreateMode CreateMode;
	typedef mFile::Option Option;

	//一回のシステムコールで読み取るファイルサイズ。
	//Windowsでは65536バイト単位で読み取ると効率がよいらしいので、この値にしている。
	static const DWORD MAX_BUFFER_SIZE = 65536;

	//ファイルを開きます。
	// opt : ファイルを開くときのオプション
	//       リードストリームのため、opt.AccessRead = trueとすること
	bool Open( const mFile::Option& opt );

	//ファイルを開きます
	// filename : 開くファイル
	// ※読み取りアクセス、書き込み共有不可でオープンします
	bool Open( const WString& filename );

	//ファイルを閉じます
	bool Close( void );

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//※EOFの場合、このリードストリームが同期アクセスのものかどうかで意味が違います
	// 同期アクセスの場合   : ファイルの終端（それ以上のデータはない）
	// 非同期アクセスの場合 : 現在読み取れるデータがない（時間が経てば再度読み取れる）
	//※非同期アクセスは未実装
	virtual INT Read( void );

	//指定の位置から読み始めます
	//指定するのは、ファイルの先頭からの位置になります。
	bool SetPointer( ULONGLONG newpos );

	//EOFに達しているかを調べます
	//※同期アクセス・非同期アクセスどちらであっても、この関数がtrueの場合、
	//　ファイルの終端に達しています。
	//※非同期アクセスは未実装
	virtual bool IsEOF( void )const;

	//ファイルが開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const;

	//開いているパスを返します
	// fullpath : trueの場合、開いているパスをフルパスに変換しようとします
	//            falseの場合、trueでも変換に失敗した場合は、ファイルオープン時に渡したパスがそのまま返ります
	WString GetPath( bool fullpath )const;

	//ファイルのサイズを取得
	bool GetFileSize( ULONGLONG& retSize )const;

protected:

	mFileReadStream( const mFileReadStream& source ) = delete;
	void operator=( const mFileReadStream& source ) = delete;

	//キャッシュをクリアする
	virtual void InvalidateCache( void );

	//次のブロックを読み込む
	virtual bool ReadNextBlock( void );

	mFile MyHandle;			//ファイルのハンドル


};

#endif

