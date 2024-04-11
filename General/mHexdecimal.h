//----------------------------------------------------------------------------
// 16進オブジェクト
// Copyright (C) 2021 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------


#ifndef MHEXDECIMAL_H_INCLUDED
#define MHEXDECIMAL_H_INCLUDED

#include <mStandard.h>

class mHexdecimal
{
public:
	mHexdecimal()
	{
		Hex = 0;
	}
	mHexdecimal( DWORD v )
	{
		Hex = v;
	}
	mHexdecimal( const mHexdecimal& src )
	{
		Hex = src.Hex;
	}
	const mHexdecimal& operator=( const mHexdecimal& src )
	{
		Hex = src.Hex;
		return *this;
	}
	operator DWORD() const
	{
		return Hex;
	}
	DWORD Hex;
};

#endif
