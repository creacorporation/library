//----------------------------------------------------------------------------
// 基本ヘッダ
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MSTANDARD_CPP_COMPILING
#include "mStandard.h"
#include "mRevision.h"
#include <locale.h>

void InitializeLibrary( void )
{
#ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	setlocale( LC_ALL , "ja-JP" );

	return;
}


void DeinitializeLibrary( void )
{
}

#if defined( NDEBUG ) && LIBRARY_REVISION_MODIFIED
#pragma message( "=============== W A R N I N G ===============" )
#pragma message( "You're building release version without committing." )
#pragma message( "Be sure to commit modification before building release version." )
#pragma message( "=============================================" )
#endif


