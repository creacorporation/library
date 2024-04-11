//----------------------------------------------------------------------------
// ���[�J�[�X���b�h���^�X�N�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MWORKERTHREAD_H_INCLUDED
#define MWORKERTHREAD_H_INCLUDED

#include "mStandard.h"
#include "mThread.h"

class mWorkerThreadPool;

class mWorkerThread : public mThread
{
public:
	mWorkerThread( mWorkerThreadPool& pool );
	virtual ~mWorkerThread();

private:

	mWorkerThread( const mWorkerThread& src ) = delete;
	const mWorkerThread& operator=( const mWorkerThread& src ) = delete;

	//�X���b�h�Ŏ��s���鏈��
	virtual unsigned int TaskFunction( void );

protected:

	//���̃��[�J�[�X���b�h���֘A�t�����Ă���v�[��
	mWorkerThreadPool& MyParent;

};

#endif

