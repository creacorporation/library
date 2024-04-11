//----------------------------------------------------------------------------
// �X�g���[�~���O�t�@�C���������ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

/*
���p�r
�X�g���[�~���O�I�Ƀt�@�C�����������݂܂��B

�g�����F
void TestFunction( void )
{
	//�t�@�C�����J���Ƃ��̏��
	//�������I�ɃL���b�V���������Ă��邽�߁A
	//  ������̏������݃A�N�Z�X������ƌ��ʂ����������Ȃ�܂��B
	mFile::Option opt;
	opt.Path = L"d:\\test.dat";	//�t�@�C����
	opt.AccessWrite = true;		//�������݃A�N�Z�X�͕K�{
	opt.ShareWrite = false;		//������̏������݃A�N�Z�X���֎~

	//�t�@�C�����J��
	mFileReadStream fp;
	fp.Open( opt );

	fp.Write( L'h' );
	fp.Write( L'o' );
	fp.Write( L'g' );
	fp.Write( L'e' );

	fp.WriteString( L"fuga" );

	WString str = L"piyo\r\n";
	fp.WriteString( str );
}
*/

#ifndef MFILEWRITESTREAM_H_INCLUDED
#define MFILEWRITESTREAM_H_INCLUDED

#include "mStandard.h"
#include "mFileWriteStreamBase.h"
#include "mFile.h"
#include "mTCHAR.h"

//�X�g���[�~���O�t�@�C���ǂݍ��ݑ���

class mFileWriteStream : public mFileWriteStreamBase
{
public:
	mFileWriteStream();
	virtual ~mFileWriteStream();

	//�t�@�C�����J���Ƃ��̏��
	typedef mFile::CreateMode CreateMode;
	typedef mFile::Option Option;

	//���̃V�X�e���R�[���œǂݎ��t�@�C���T�C�Y�B
	static const DWORD MAX_BUFFER_SIZE = 65536;

	//�t�@�C�����J���܂��B
	// opt : �t�@�C�����J���Ƃ��̃I�v�V����
	//       �������݃X�g���[���̂��߁Aopt.AccessRead = true�Ƃ��邱��
	virtual bool Open( const mFile::Option& opt );

	//�t�@�C������܂�
	virtual bool Close( void );

	//�P������������
	virtual bool Write( INT data );

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

	//���݂̏������݈ʒu�𓾂܂�
	ULONGLONG GetPointer( void )const;

	//�t�@�C�����J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const;

	//�J���Ă���p�X��Ԃ��܂�
	// fullpath : true�̏ꍇ�A�J���Ă���p�X���t���p�X�ɕϊ����悤�Ƃ��܂�
	//            false�̏ꍇ�Atrue�ł��ϊ��Ɏ��s�����ꍇ�́A�t�@�C���I�[�v�����ɓn�����p�X�����̂܂ܕԂ�܂�
	WString GetPath( bool fullpath )const;

	//�t�@�C���̃T�C�Y���擾
	bool GetFileSize( ULONGLONG& retSize )const;

	//�t�@�C���̃T�C�Y���擾
	// high �͕s�v�ȏꍇ�k���ł��B������high�ɓ���ׂ����ʂ�1�ȏ�̎��G���[�ɂȂ�B
	bool GetFileSize( DWORD* high , DWORD& low )const;

	//�t�@�C���̃T�C�Y���擾
	// �G���[�̏ꍇ�͂O
	ULONGLONG GetFileSize( void )const;

	//���݂̈ʒu��EOF�ɂ��܂�
	bool SetEof( void );

private:
	mFileWriteStream( const mFileWriteStream& source ) = delete;
	void operator=( const mFileWriteStream& source ) = delete;

protected:

	//�L���b�V������������
	virtual bool FlushCache( void );

	//�L���b�V����j��
	void ResetCache( void );

	mFile MyHandle;			//�t�@�C���̃n���h��

};

#endif