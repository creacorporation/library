//----------------------------------------------------------------------------
// XML����
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MCOMMEMSTREAM_H_INCLUDED
#define MCOMMEMSTREAM_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mComFileStream.h"
#include <deque>

class mComXmlLight
{
public:

	mComXmlLight();
	virtual ~mComXmlLight();

	struct Attribute
	{
		WString Name;
		WString Value;
	};

	struct Element
	{
		WString Prefix;
		WString Name;
		
		WStringDeque ChildName;
	};


private:

	mComXmlLight( const mComXmlLight& source ) = delete;
	void operator=( const mComXmlLight& source ) = delete;

protected:


};

#endif