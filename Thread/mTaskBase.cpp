//----------------------------------------------------------------------------
// ワーカースレッド＆タスクハンドラ
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mTaskBase.h"
#include <General/mCriticalSectionContainer.h>
#include "mTaskQueue.h"

mTaskBase::mTaskBase() : 
	MyTaskId( "" ) ,
	MyScheduleType( ScheduleType::Normal ),
	MyCreateTime( timeGetTime() ),
	MyDedicated( false )
{
	MyTaskStatus = TaskStatus::STATUS_NOTSTARTED;
	MyCompleteObject = 0;
	MyParent = nullptr;
	return;
}

mTaskBase::mTaskBase( const AString& TaskId , ScheduleType ScType , bool Dedicate ) : 
	MyTaskId( TaskId ) ,
	MyScheduleType( ScType ),
	MyCreateTime( timeGetTime() ),
	MyDedicated( Dedicate )
{
	MyTaskStatus = TaskStatus::STATUS_NOTSTARTED;
	MyCompleteObject = 0;
	MyParent = nullptr;
	return;
}

mTaskBase::~mTaskBase()
{
	if( MyCompleteObject != 0 )
	{
		CloseHandle( MyCompleteObject );
		MyCompleteObject = 0;
	}
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

bool mTaskBase::Wakeup( void )
{
	if( !MyParent )
	{
		return false;
	}
	return MyParent->Wakeup( *this );
}


