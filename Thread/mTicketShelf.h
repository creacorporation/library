//----------------------------------------------------------------------------
// タスクハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MTICKETSHELF_H_INCLUDED
#define MTICKETSHELF_H_INCLUDED

#include "mStandard.h"
#include "General/mCriticalSectionContainer.h"
#include "General/mException.h"
#include "mTaskBase.h"

#include <map>

class mTicketShelf
{
public:
	mTicketShelf();
	virtual ~mTicketShelf();

	DWORD_PTR Deposit( mTaskBase::Ticket& ticket );
	mTaskBase::Ticket Removal( DWORD_PTR key );

private:

	mTicketShelf( const mTicketShelf& src ) = delete;
	const mTicketShelf& operator=( const mTicketShelf& src ) = delete;

protected:

	mCriticalSectionContainer MyCritical;

	using TicketMap = std::map< DWORD_PTR , mTaskBase::Ticket >;
	TicketMap MyTicketMap;

};


#endif //MTICKETSHELF_H_INCLUDED

