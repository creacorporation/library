//----------------------------------------------------------------------------
// タスクハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mTicketShelf.h"

mTicketShelf::mTicketShelf()
{
}

mTicketShelf::~mTicketShelf()
{
}

DWORD_PTR mTicketShelf::Deposit( mTaskBase::Ticket& ticket )
{
	mCriticalSectionTicket crit( MyCritical );
	DWORD_PTR key = reinterpret_cast< DWORD_PTR >( ticket.get() );

	MyTicketMap[ key ] = ticket;
	return key;
}

mTaskBase::Ticket mTicketShelf::Removal( DWORD_PTR key )
{
	mCriticalSectionTicket crit( MyCritical );

	TicketMap::iterator itr = MyTicketMap.find( key );
	if( itr == MyTicketMap.end() )
	{
		return std::move( mTaskBase::Ticket() );
	}

	mTaskBase::Ticket ticket = std::move( itr->second );
	MyTicketMap.erase( itr );

	return std::move( ticket );
}

