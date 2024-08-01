//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MWORKERTHREAD_H_INCLUDED
#define MWORKERTHREAD_H_INCLUDED

#include "mStandard.h"
#include "mThread.h"

class mWorkerThreadPool;

class mWorkerThread : public mThread
{
public:
	mWorkerThread( mWorkerThreadPool& pool );
	virtual ~mWorkerThread();

private:

	mWorkerThread( const mWorkerThread& src ) = delete;
	const mWorkerThread& operator=( const mWorkerThread& src ) = delete;

	//スレッドで実行する処理
	virtual unsigned int TaskFunction( void );

protected:

	//このワーカースレッドが関連付けられているプール
	mWorkerThreadPool& MyParent;

};

#endif

