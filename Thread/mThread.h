//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MTHREAD_H_INCLUDED
#define MTHREAD_H_INCLUDED

#include "mStandard.h"
#include <process.h>
#include <memory>

//スレッドのベースクラス

//HOW TO USE
//1.このクラスを継承し、TaskFunction()に別スレッドで実行したい処理を書く
//2.Begin()にユーザ定義の引数を渡す
//3.TaskFunction()が別スレッドから実行される
//4.TaskFunction()が返るとスレッドが終了する
//5.スレッドが終了する前にオブジェクトのインスタンスを削除すると結果が不定になる

class mThread
{
private:

	friend unsigned int __stdcall ThreadBaseFunc( void* );

public:
	mThread();
	virtual ~mThread();

	//スレッドを開始する
	//arg : スレッドに渡す引数
	//スレッドはサスペンド状態で開始します。
	//ret : 成功時真
	bool Begin( std::shared_ptr<void> arg );

	//スレッドを開始する
	//スレッドはサスペンド状態で開始します。
	//ret : 成功時真
	bool Begin( void );

	//スレッドに対するプロセッサコアの親和性をセットする
	// mask : アフィニティマスク
	bool SetAffinityMask( DWORD_PTR mask );

	//停止中のスレッドを再開する
	//retPrevCount : 指定すると実行前のサスペンドカウントの値を返します
	//ret : 成功時真
	threadsafe bool Resume( DWORD* retPrevCount = nullptr );

	//実行中のスレッドを停止する
	//retPrevCount : 指定すると実行前のサスペンドカウントの値を返します
	//ret : 成功時真
	threadsafe bool Suspend( DWORD* retPrevCount = nullptr );

	//スレッドを終了する
	//ret : 成功時真
	//・スレッドが終了するまで制御を返しません
	//・スレッドがサスペンドしているときにこの関数を呼ぶと失敗します
	//・呼び出し元自身のスレッドを終了することはできません
	threadsafe bool End( void );

	//スレッドに終了するよう指示する
	//・指示するだけで、実際に終了するまでにはタイムラグがある
	//・呼び出し元自身のスレッドに対して呼び出すこともできます
	//ret : 成功時真
	threadsafe bool FinishRequest( void );

	//スレッドを(強引に)終了する
	//ret : 成功時真
	bool Terminate( void );

	//スレッドが実行中か否かを調べる
	threadsafe bool IsValid( void )const;

	//スレッドIDを得る
	threadsafe unsigned int GetThreadId( void )const;

private:

	mThread( const mThread& src );
	const mThread& operator=( const mThread& src );

	//スレッドで実行する処理
	virtual unsigned int TaskFunction() = 0;

	//変数初期化
	bool Clear( void );

protected:

	//スレッドのハンドル
	HANDLE MyHandle;

	//スレッドID
	unsigned int MyThreadId;

	//スレッドの終了シグナル
	//FinishRequestが呼ばれるとシグナル状態になる
	HANDLE MyTerminateSignal;

	//Begin()で渡された引数
	std::shared_ptr<void> MyArg;

};





#endif //MTHREAD_H_INCLUDED