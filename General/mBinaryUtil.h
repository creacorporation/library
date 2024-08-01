//----------------------------------------------------------------------------
// バイナリユーティリティ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MBINARYUTILITY_H_INCLUDED
#define MBINARYUTILITY_H_INCLUDED

#include "mStandard.h"

int BitCount( const void* p , size_t sz );

template< class T > int BitCount( const T& v )
{
	return BitCount( &v , sizeof( v ) );
};



#endif
