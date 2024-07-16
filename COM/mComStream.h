//----------------------------------------------------------------------------
// COM�ւ̃X�g���[�~���O�������ݑ���
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MCOMSTREAM_H_INCLUDED
#define MCOMSTREAM_H_INCLUDED

#include "mStandard.h"
#include "General/mFileReadStreamBase.h"
#include "General/mFileWriteStreamBase.h"
#include "General/mTCHAR.h"
#include <objidl.h>

//COM�̃��b�p�[
class mComStream : public mFileReadStreamBase , public mFileWriteStreamBase
{
};


#endif
