//----------------------------------------------------------------------------
// クリティカルセクション管理
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MCRITICALSECTIONCONTAINER_CPP_COMPILING
#include "mCriticalSectionContainer.h"


mCriticalSectionContainer::mCriticalSectionContainer()
{

	InitializeCriticalSection( &MyCriticalSection );

}

mCriticalSectionContainer::~mCriticalSectionContainer()
{

	DeleteCriticalSection( &MyCriticalSection );

}

DWORD mCriticalSectionContainer::SetSpinCount( DWORD Count )
{

	return SetCriticalSectionSpinCount( &MyCriticalSection , Count );

}

void mCriticalSectionContainer::Enter( void )
{

	EnterCriticalSection( &MyCriticalSection );

}

void mCriticalSectionContainer::Leave( void )
{

	LeaveCriticalSection( &MyCriticalSection );

}

bool mCriticalSectionContainer::TryEnter( void )
{

	return TryEnterCriticalSection( &MyCriticalSection ) != FALSE;

}


mCriticalSectionTicket::mCriticalSectionTicket(mCriticalSectionContainer &critical_section)
	: MySection( critical_section )
{
	MySection.Enter();
	return;
}

mCriticalSectionTicket::~mCriticalSectionTicket()
{
	MySection.Leave();
	return;
}


