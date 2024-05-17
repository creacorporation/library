//----------------------------------------------------------------------------
// Winsock���[�e�B���e�B
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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

	//Winsock���C�j�V�����C�Y����Ă��邩�ǂ���
	bool IsInitialized( void )const;

	//Winsock���C�j�V�����C�Y����Ă��邩�ǂ���
	operator bool() const;

private:

	mWinsockInitializer( const mWinsockInitializer& src ) = delete;
	const mWinsockInitializer& operator=( const mWinsockInitializer& src ) = delete;

	bool MyIsInitialized;
};

#endif
