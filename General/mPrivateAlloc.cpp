//----------------------------------------------------------------------------
// �������A���P�[�^
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mPrivateAlloc.h"

mPrivateAlloc::mPrivateAlloc()
{
	;
}

mPrivateAlloc::~mPrivateAlloc()
{
	void* ptr;
	while( MyAllocInfo.size() )
	{
		ptr = MyAllocInfo.back();
		MyAllocInfo.pop_back();
		VirtualFree( ptr , 0 , MEM_RELEASE );
	}
}

void* mPrivateAlloc::Alloc( const mPrivateAlloc::AllocInfo& info )
{

	DWORD protect;
	if( info.Executable && info.Writable )
	{
		protect = PAGE_EXECUTE_READWRITE;
	}
	else if( info.Executable && !info.Writable )
	{
		protect = PAGE_EXECUTE_READ;
	}
	else if( !info.Executable && info.Writable )
	{
		protect = PAGE_READWRITE;
	}
	else
	{
		protect = PAGE_READONLY;
	}

	void* result = VirtualAlloc( 0 , info.Size , MEM_COMMIT , protect );
	if( result == nullptr )
	{
		return nullptr;
	}
	
	MyAllocInfo.push_back( result );
	return result;
}

