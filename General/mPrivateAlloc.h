//----------------------------------------------------------------------------
// メモリアロケータ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MPRIVATEALLOC_H_INCLUDED
#define MPRIVATEALLOC_H_INCLUDED

#include "mStandard.h"
#include <deque>

//VirtualAllocをラップしたもの

class mPrivateAlloc
{
public:

	mPrivateAlloc();
	virtual ~mPrivateAlloc();

	struct AllocInfo
	{
		DWORD Size;
		BOOL Writable;
		BOOL Executable;
	};
	void* Alloc( const AllocInfo& info );

private:

	mPrivateAlloc( const mPrivateAlloc& src );
	const mPrivateAlloc& operator=( const mPrivateAlloc& src );

protected:

	std::deque< void* > MyAllocInfo;
};


#endif