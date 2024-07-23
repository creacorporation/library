//----------------------------------------------------------------------------
// COM�ւ̃X�g���[�~���O�������ݑ���
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
#include "General/mFileReadStreamBase.h"
#include "General/mFileWriteStreamBase.h"
#include "General/mTCHAR.h"
#include <objidl.h>

//COM�̃��b�p�[
class mComMemStream : public mFileReadStreamBase , public mFileWriteStreamBase
{
public:

	mComMemStream( DWORD buffersize = 4096 );
	virtual ~mComMemStream();

	//�t�@�C������܂�
	virtual bool Close( void );

	//�P������������
	virtual bool Write( INT data );

	//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
	virtual INT Read( void );

	//�J���Ă��邩�ǂ���
	virtual bool IsOpen( void )const;

	//EOF�ɒB���Ă��邩�𒲂ׂ܂�
	virtual bool IsEOF( void )const;

	//�L���b�V������������
	virtual bool FlushCache( void );

	//�w��̈ʒu�Ƀ|�C���^���ړ����܂�
	//�w�肷��̂́A�t�@�C���̐擪����̈ʒu�ɂȂ�܂��B
	bool SetPointer( ULONGLONG pos );

	//�t�@�C���|�C���^��O��Ɉړ�
	//distance : �ړ�����
	//ret : ������true
	//�w��ʒu��EOF�𒴂���ꍇ���G���[�ɂȂ�܂���B
	bool MovePointer( LONGLONG distance );

	//�t�@�C���|�C���^���t�@�C���̖����Ɉړ�
	bool SetPointerToEnd( void );

	//�t�@�C���|�C���^���t�@�C���̐擪�Ɉړ�
	bool SetPointerToBegin( void );

	//�C���^�[�t�F�C�X���擾
	IStream* Get( void )const;

	//�C���^�[�t�F�C�X���擾
	operator IStream*( void )const;

private:

	mComMemStream( const mComMemStream& source ) = delete;
	void operator=( const mComMemStream& source ) = delete;

protected:

	//���̃V�X�e���R�[���œǂݎ��T�C�Y
	const DWORD MAX_BUFFER_SIZE;

	//�����݃L���b�V����j��
	void ResetWriteCache( void );

	IStream* MyStream;
};

#endif