//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCDESCRIPTION_H_INCLUDED
#define MODBCDESCRIPTION_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"

#include "mOdbc.h"

#include <vector>
#include <unordered_map>

#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")

struct mOdbcDescriptionEntry
{
	//�T�C�Y
	DWORD_PTR Size;
	//�T�C�Y
	DWORD_PTR Digit;
	//�k�������e����ꍇ�^
	bool Nullable;
	//�Œ蒷�ł���ꍇ�F�^
	//�ϒ��ł���ꍇ�F�U
	bool Fixed;
	//���^ODBC�^
	DWORD MetaType;
	//�����f�[�^�^
	mOdbc::ParameterType ParamType;
	//�����Ή����Ă��邩
	bool Available;
	//�p�����[�^�A���ʃZ�b�g�̗�ԍ�
	INT Index;
};

//�p�����[�^�N�G���̃p�����[�^����ێ�����
//�p�����[�^�̍������珇�ɔz��Ɋi�[����
typedef std::vector<mOdbcDescriptionEntry> mOdbcParameterDescription;

//�N�G���ɂ���ē���ꂽ���ʃZ�b�g�̏���ێ�����
//���F��̖��O
//�E�F��̏��
typedef std::unordered_map<WString,mOdbcDescriptionEntry> mOdbcResultDescription;

#endif
