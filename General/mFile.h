//----------------------------------------------------------------------------
// ファイル管理
// Copyright (C) 2005,2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
ファイルをいじります。

使い方：
void TestFunction( void )
{
	//ファイルを開くときの情報
	mFile::Option opt;	
	opt.Path = L"d:\\test.dat";	//ファイル名
	opt.AccessRead = true;		//読み取りアクセスをする
	opt.ShareWrite = false;		//ヨソからの書き込みアクセスを禁止

	//ファイルを開く
	mFile fp;
	fp.Open( opt );

	//開いたファイルからdataに値を読み取ります
	DWORD data;		//読み取り先
	DWORD size;		//読み取ったサイズ
	fp.Read( &data , sizeof( data ) , &size );
}
*/


#ifndef MFILE_H_INCLUDED
#define MFILE_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include <General/mBinary.h>
#include <General/mDateTime.h>

class mFile
{
public:

	mFile();
	virtual ~mFile();

	//ファイルを開くときのモード
	enum class CreateMode
	{
		CreateNew ,				//新しくファイルを作成。すでにある場合はエラー
		CreateAlways ,			//新しくファイルを作成。すでにある場合は上書き（中身を捨てる）
		OpenExisting ,			//すでにあるファイルを開く。ない場合はエラー
		OpenAlways ,			//すでにあるファイルを開く。ない場合は新しいファイルを開く
		TruncateExisting ,		//すでにあるファイルを開いて中身を捨てる。ない場合はエラー。
		CreateWithDirectory,	//新しくファイルを作成。ディレクトリがない場合はディレクトリも作成する。すでにある場合は上書き（中身を捨てる）。
	};

	struct Option
	{
		WString Path;		//開くファイルのパス
		bool ShareWrite;	//trueの場合他からの書き込みアクセスを認める
		bool ShareRead;		//trueの場合他からの読み込みアクセスを認める
		bool AccessWrite;	//書き込みアクセスを行う
		bool AccessRead;	//読み取りアクセスを行う
		CreateMode Mode;	//ファイルを開くときのモード
		Option()
		{
			ShareWrite = false;
			ShareRead = false;
			AccessWrite = true;
			AccessRead = true;
			Mode = CreateMode::OpenAlways;
		}
	};

	//ファイルを開く
	bool Open( const Option& opt );

	//ファイルを閉じる
	//デストラクタで閉じるようになっているから別に呼ばなくて可。
	//ファイルを閉じて別のファイルを開きたいとき用。
	bool Close( void );

	//ファイルから読み込み
	// Buffer : 読み取ったデータを格納するバッファ
	// ReadSize : 読み取りたいサイズ
	// retReadSize : 実際に読み取ったサイズ
	bool Read(
		void*	Buffer ,							//読み取ったデータを格納するバッファ
		ULONGLONG  ReadSize ,						//読み取るバイト数
		ULONGLONG& retReadSize );					//読み取ったバイト数
	
	bool Read(
		void*	Buffer ,							//読み取ったデータを格納するバッファ
		DWORD	ReadSize ,							//読み取るバイト数
		DWORD&	retReadSize );						//読み取ったバイト数

	//ファイルに書き込み
	// Buffer : 読み取ったデータを格納するバッファ
	// WriteSize : 読み取りたいサイズ
	// retWriteSize : 実際に読み取ったサイズ
	bool Write(
		void*	Buffer,								//書き込むデータを格納してあるバッファ
		ULONGLONG  WriteSize,						//書き込むバイト数
		ULONGLONG& retWriteSize );					//書き込んだバイト数

	bool Write(
		void*  Buffer ,								//書き込むデータを格納してあるバッファ
		DWORD  WriteSize ,							//書き込むバイト数
		DWORD& retWriteSize );						//書き込んだバイト数
													
	//ファイルポインタを指定位置に移動
	//newpos : 新しい位置（先頭からのバイト数）
	//ret : 成功時true
	//指定位置がEOFを超える場合もエラーになりません。
	bool SetPointer( ULONGLONG newpos );

	//ファイルポインタを前後に移動
	//distance : 移動距離
	//ret : 成功時true
	//指定位置がEOFを超える場合もエラーになりません。
	bool MovePointer( LONGLONG distance );

	//ファイルポインタをファイルの末尾に移動
	bool SetPointerToEnd( void );

	//ファイルポインタをファイルの先頭に移動
	bool SetPointerToBegin( void );

	//ファイルのバッファをフラッシュする
	bool FlushBuffer( void );

	//ファイルのサイズを取得
	bool GetFileSize( ULONGLONG& retSize )const;

	//ファイルのサイズを取得
	// high は不要な場合ヌルでも可。ただしhighに入るべき結果が1以上の時エラーになる。
	bool GetFileSize( DWORD* high , DWORD& low )const;

	//ファイルのサイズを取得
	// エラーの場合は０
	ULONGLONG GetFileSize( void )const;

	//現在のファイルポインタ位置の取得
	bool GetPosition( ULONGLONG& retPos )const;

	//ファイルが開いているかを判定します
	//開いている場合は真が返ります
	bool IsOpen( void )const;

	//開いているパスを返します
	// fullpath : trueの場合、開いているパスをフルパスに変換しようとします
	//            falseの場合、trueでも変換に失敗した場合は、ファイルオープン時に渡したパスがそのまま返ります
	WString GetPath( bool fullpath )const;

	//現在のファイルポインタの位置をEOFに指定します
	bool SetEof( void );

	//コントロールコードの送信
	// code : コントロールコード
	// in : 入力データ。何も渡さないならnullptr
	// in : 出力データ。何も受け取らないならnullptr
	bool ExecIoControl( DWORD code , const mBinary* in , mBinary* retResult );

	//ファイル時刻を取得します
	// retCreationTime : 作成時刻
	// retLastAccessTime : 最終アクセス時刻
	// retLastWriteTime : 最終書き込み時刻
	bool GetFileTime(
		mDateTime::Timestamp* retCreationTime,
		mDateTime::Timestamp* retLastAccessTime,
		mDateTime::Timestamp* retLastWriteTime
	)const;

private:

	//コピー禁止
	mFile( const mFile& source ) = delete;
	void operator=( const mFile& source ) = delete;

protected:

	HANDLE MyHandle;
	WString MyPath;

};

#endif