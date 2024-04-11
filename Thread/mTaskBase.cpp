//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mTaskBase.h"

mTaskBase::mTaskBase() : MyTaskId( "" )
{
	MyTaskStatus = TaskStatus::STATUS_NOTSTARTED;
	return;
}

mTaskBase::mTaskBase( const AString& TaskId ) : MyTaskId( TaskId )
{
	MyTaskStatus = TaskStatus::STATUS_NOTSTARTED;
	return;
}

mTaskBase::~mTaskBase()
{
	return;
}

mTaskBase::TaskStatus mTaskBase::GetTaskStatus( void )const
{
	return MyTaskStatus;
}

void mTaskBase::CancelFunction( const Ticket& task )
{
	return;
}
