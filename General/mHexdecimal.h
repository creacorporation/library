//----------------------------------------------------------------------------
// 16�i�I�u�W�F�N�g
// Copyright (C) 2021 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
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
