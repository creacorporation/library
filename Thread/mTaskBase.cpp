//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mTaskBase.h"

mTaskBase::mTaskBase() : MyTaskId( "" )
{
	MyTaskStatus = TaskStatus::STATUS_NOTSTARTED;
	return;
}

mTaskBase::mTaskBase( const AString& TaskId ) : MyTaskId( TaskId )
{
	MyTaskStatus = TaskStatus::STATUS_NOTSTARTED;
	return;
}

mTaskBase::~mTaskBase()
{
	return;
}

mTaskBase::TaskStatus mTaskBase::GetTaskStatus( void )const
{
	return MyTaskStatus;
}

void mTaskBase::CancelFunction( const Ticket& task )
{
	return;
}
