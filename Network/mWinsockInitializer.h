//----------------------------------------------------------------------------
// Winsockユーティリティ
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MWINSOCKINITIALIZER_H_INCLUDED
#define MWINSOCKINITIALIZER_H_INCLUDED

#include <WinSock2.h>
#include "mStandard.h"

class mWinsockInitializer
{
public:
	mWinsockInitializer();
	virtual ~mWinsockInitializer();

	//Winsockがイニシャライズされているかどうか
	bool IsInitialized( void )const;

	//Winsockがイニシャライズされているかどうか
	operator bool() const;

private:

	mWinsockInitializer( const mWinsockInitializer& src ) = delete;
	const mWinsockInitializer& operator=( const mWinsockInitializer& src ) = delete;

	bool MyIsInitialized;
};

#endif
