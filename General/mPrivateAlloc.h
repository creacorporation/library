//----------------------------------------------------------------------------
// �������A���P�[�^
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MPRIVATEALLOC_H_INCLUDED
#define MPRIVATEALLOC_H_INCLUDED

#include "mStandard.h"
#include <deque>

//VirtualAlloc�����b�v��������

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