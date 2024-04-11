//----------------------------------------------------------------------------
// �X�g���[�~���O�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------


#ifndef MFILEREADSTREAM_H_INCLUDED
#define MFILEREADSTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileReadStreamBase.h"

/*
���p�r
�X�g���[�~���O�I�Ƀt�@�C����ǂݎ��܂��B

�g�����F
void TestFunction( void )
{
	//�t�@�C�����J���Ƃ��̏��
	//�������I�ɃL���b�V���������Ă��邽�߁A
	//  ������̏������݃A�N�Z�X������ƌ��ʂ����������Ȃ�܂��B
	mFile::Option opt;
	opt.Path = L"d:\\test.dat";	//�t�@�C����
	opt.AccessRead = true;		//�ǂݎ��X�g���[���̂��߁A�ǂݎ��A�N�Z�X�͕K�{
	opt.ShareWrite = false;		//������̏������݃A�N�Z�X���֎~

	//�t�@�C�����J��
	mFileReadStream fp;
	fp.Open( opt );

	//1�s���t�@�C�����e�L�X�g�Ƃ��ēǂݎ��܂�
	while( !fp.IsEOF() )
	{
		//str��1�s���ǂݎ��܂�
		WString str;
		if( !fp.ReadLine( str ) )
		{
			//�񓯊��̏ꍇ�AReadLine�ARead����EOF�̏ꍇ�́A
			//�t�@�C���̏I�[�ł���ꍇ�̑��ɁA�ǂݍ��ݒ��ł���ꍇ������܂�
			//���̂��߁A���߂�IsEOF���g���Ĕ��肷��K�v������܂��B
			::Sleep( 100 );
			continue;
		}

		//str�̓��e���o�͂��܂�
		printf( "%s" , str.c_str() );
	}
}
*/


class mFileReadStream : public mFileReadStreamBase
{
public:
	mFileReadStream();
	virtual ~mFileReadStream();

	//�t�@�C�����J���Ƃ��̏��
	typedef mFile::CreateMode CreateMode;
	typedef mFile::Option Option;

	//���̃V�X�e���R�[���œǂݎ��t�@�C���T�C�Y�B
	//Windows�ł�65536�o�C�g�P�ʂœǂݎ��ƌ������悢�炵���̂ŁA���̒l�ɂ��Ă���B
	static const DWORD MAX_BUFFER_SIZE = 65536;

	//�t�@�C�����J���܂��B
	// opt : �t�@�C�����J���Ƃ��̃I�v�V����
	//       ���[�h�X�g���[���̂��߁Aopt.AccessRead = true�Ƃ��邱��
	bool Open( const mFile::Option& opt );

	//�t�@�C�����J���܂�
	// filename : �J���t�@�C��
	// ���ǂݎ��A�N�Z�X�A�������݋��L�s�ŃI�[�v�����܂�
	bool Open( const WString& filename );

	//�t�@�C������܂�
	bool Close( void );

	//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
	//ret : �ǂݎ��������
	//��EOF�̏ꍇ�A���̃��[�h�X�g���[���������A�N�Z�X�̂��̂��ǂ����ňӖ����Ⴂ�܂�
	// �����A�N�Z�X�̏ꍇ   : �t�@�C���̏I�[�i����ȏ�̃f�[�^�͂Ȃ��j
	// �񓯊��A�N�Z�X�̏ꍇ : ���ݓǂݎ���f�[�^���Ȃ��i���Ԃ��o�Ă΍ēx�ǂݎ���j
	//���񓯊��A�N�Z�X�͖�����
	virtual INT Read( void );

	//�w��̈ʒu����ǂݎn�߂܂�
	//�w�肷��̂́A�t�@�C���̐擪����̈ʒu�ɂȂ�܂��B
	bool SetPointer( ULONGLONG newpos );

	//EOF�ɒB���Ă��邩�𒲂ׂ܂�
	//�������A�N�Z�X�E�񓯊��A�N�Z�X�ǂ���ł����Ă��A���̊֐���true�̏ꍇ�A
	//�@�t�@�C���̏I�[�ɒB���Ă��܂��B
	//���񓯊��A�N�Z�X�͖�����
	virtual bool IsEOF( void )const;

	//�t�@�C�����J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const;

	//�J���Ă���p�X��Ԃ��܂�
	// fullpath : true�̏ꍇ�A�J���Ă���p�X���t���p�X�ɕϊ����悤�Ƃ��܂�
	//            false�̏ꍇ�Atrue�ł��ϊ��Ɏ��s�����ꍇ�́A�t�@�C���I�[�v�����ɓn�����p�X�����̂܂ܕԂ�܂�
	WString GetPath( bool fullpath )const;

	//�t�@�C���̃T�C�Y���擾
	bool GetFileSize( ULONGLONG& retSize )const;

protected:

	mFileReadStream( const mFileReadStream& source ) = delete;
	void operator=( const mFileReadStream& source ) = delete;

	//�L���b�V�����N���A����
	virtual void InvalidateCache( void );

	//���̃u���b�N��ǂݍ���
	virtual bool ReadNextBlock( void );

	mFile MyHandle;			//�t�@�C���̃n���h��


};

#endif

