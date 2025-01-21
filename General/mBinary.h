//----------------------------------------------------------------------------
// バイナリ管理
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
	バイナリデータの制御を行います

使い方：

*/


#ifndef MBINARY_H_INCLUDED
#define MBINARY_H_INCLUDED

#include "mStandard.h"
#include "mSecureAllocator.h"
#include <vector>

template< class T >
class mBinaryTemplate final : public std::vector< BYTE , T > 
{
public:

	mBinaryTemplate()
	{
		return;
	}

	~mBinaryTemplate()
	{
		return;
	}

	mBinaryTemplate( const mBinaryTemplate& source ) : std::vector< BYTE , T >( source )
	{
		return;
	}

	void operator=( const mBinaryTemplate& source )
	{
		std::vector< BYTE , T >::operator=( source );
	}

	template< class U >
	void append( const mBinaryTemplate<U>& dt )
	{
		__super::insert( __super::end() , dt.begin() , dt.end() );
	}

	static const size_t npos = -1;

	mBinaryTemplate subdata( size_t pos = 0 , size_t n = npos )const
	{
		mBinaryTemplate dt;
		n = ( n == npos ) ? ( __super::size() ) : ( pos + n );
		dt.reserve( n - pos );
		for( ; pos < n ; pos++ )
		{
			dt.push_back( __super::data()[ pos ] );
		}
		return std::move( dt );
	}

	void secure_erase( void )
	{
		SecureZeroMemory( __super::data() , __super::capacity() );
		__super::clear();
	}

	void copyfrom( const void* p , size_t sz )
	{
		__super::reserve( sz );
		MoveMemory( __super::data() , p , sz );
	}

private:

};

using mBinary = mBinaryTemplate< std::allocator< BYTE > >;
using mSecureBinary = mBinaryTemplate< mSecureAllocator< BYTE > >;

#endif
