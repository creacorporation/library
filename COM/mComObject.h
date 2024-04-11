//----------------------------------------------------------------------------
// COM�I�u�W�F�N�g�p���[�e�B���e�B�֐��Q
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MCOMOBJECT_H_INCLUDED
#define MCOMOBJECT_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <objbase.h>
#include <comutil.h>
#pragma comment(lib, "comsupp.lib")

class mComObject
{
public:
	mComObject();
	virtual ~mComObject();

private:
	mComObject( const mComObject& src );
	const mComObject& operator=( const mComObject& src );

	//���̃I�u�W�F�N�g�ŃC�j�V�����C�Y(CoInitialize)���s�������H
	//true = �C�j�V�����C�Y���s����
	bool MyIsInitialized;
};

#endif

