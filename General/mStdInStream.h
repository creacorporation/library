//----------------------------------------------------------------------------
// �W�����͓ǂݍ��ݑ���
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------


#ifndef MSTDINSTREAM_H_INCLUDED
#define MSTDINSTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileReadStreamBase.h"

/*
���p�r
�X�g���[�~���O�I�ɕW�����͂�ǂݎ��܂��B
*/

class mStdInStream : public mFileReadStreamBase
{
public:
	mStdInStream();
	virtual ~mStdInStream();

	//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
	//ret : �ǂݎ��������
	//��EOF�̏ꍇ�A���ݓǂݎ���f�[�^���Ȃ����Ƃ������܂�
	//�i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
	//���X�g���[�������S�ɏI�����Ă��邩��m��ɂ�IsEOF���g���܂�
	virtual INT Read( void );

	//EOF�ɒB���Ă��邩�𒲂ׂ܂�
	virtual bool IsEOF( void )const;

	//�t�@�C�����J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const;

protected:

	mStdInStream( const mStdInStream& source ) = delete;
	void operator=( const mStdInStream& source ) = delete;

};

#endif

