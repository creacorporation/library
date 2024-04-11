//----------------------------------------------------------------------------
// �t�@�C���Ǘ�
// Copyright (C) 2005,2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�t�@�C����������܂��B

�g�����F
void TestFunction( void )
{
	//�t�@�C�����J���Ƃ��̏��
	mFile::Option opt;	
	opt.Path = L"d:\\test.dat";	//�t�@�C����
	opt.AccessRead = true;		//�ǂݎ��A�N�Z�X������
	opt.ShareWrite = false;		//���\����̏������݃A�N�Z�X���֎~

	//�t�@�C�����J��
	mFile fp;
	fp.Open( opt );

	//�J�����t�@�C������data�ɒl��ǂݎ��܂�
	DWORD data;		//�ǂݎ���
	DWORD size;		//�ǂݎ�����T�C�Y
	fp.Read( &data , sizeof( data ) , &size );
}
*/


#ifndef MFILE_H_INCLUDED
#define MFILE_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include <General/mBinary.h>

class mFile
{
public:

	mFile();
	virtual ~mFile();

	//�t�@�C�����J���Ƃ��̃��[�h
	enum CreateMode
	{
		CreateNew ,				//�V�����t�@�C�����쐬�B���łɂ���ꍇ�̓G���[
		CreateAlways ,			//�V�����t�@�C�����쐬�B���łɂ���ꍇ�͏㏑���i���g���̂Ă�j
		OpenExisting ,			//���łɂ���t�@�C�����J���B�Ȃ��ꍇ�̓G���[
		OpenAlways ,			//���łɂ���t�@�C�����J���B�Ȃ��ꍇ�͐V�����t�@�C�����J��
		TruncateExisting ,		//���łɂ���t�@�C�����J���Ē��g���̂Ă�B�Ȃ��ꍇ�̓G���[�B
		CreateWithDirectory,	//�V�����t�@�C�����쐬�B�f�B���N�g�����Ȃ��ꍇ�̓f�B���N�g�����쐬����B���łɂ���ꍇ�͏㏑���i���g���̂Ă�j�B
	};

	struct Option
	{
		WString Path;		//�J���t�@�C���̃p�X
		bool ShareWrite;	//true�̏ꍇ������̏������݃A�N�Z�X��F�߂�
		bool ShareRead;		//true�̏ꍇ������̓ǂݍ��݃A�N�Z�X��F�߂�
		bool AccessWrite;	//�������݃A�N�Z�X���s��
		bool AccessRead;	//�ǂݎ��A�N�Z�X���s��
		CreateMode Mode;	//�t�@�C�����J���Ƃ��̃��[�h
		Option()
		{
			ShareWrite = false;
			ShareRead = false;
			AccessWrite = true;
			AccessRead = true;
			Mode = CreateMode::OpenAlways;
		}
	};

	//�t�@�C�����J��
	bool Open( const Option& opt );

	//�t�@�C�������
	//�f�X�g���N�^�ŕ���悤�ɂȂ��Ă��邩��ʂɌĂ΂Ȃ��ĉB
	//�t�@�C������ĕʂ̃t�@�C�����J�������Ƃ��p�B
	bool Close( void );

	//�t�@�C������ǂݍ���
	// Buffer : �ǂݎ�����f�[�^���i�[����o�b�t�@
	// ReadSize : �ǂݎ�肽���T�C�Y
	// retReadSize : ���ۂɓǂݎ�����T�C�Y
	bool Read(
		void*	Buffer ,							//�ǂݎ�����f�[�^���i�[����o�b�t�@
		ULONGLONG  ReadSize ,						//�ǂݎ��o�C�g��
		ULONGLONG& retReadSize );					//�ǂݎ�����o�C�g��
	
	bool Read(
		void*	Buffer ,							//�ǂݎ�����f�[�^���i�[����o�b�t�@
		DWORD	ReadSize ,							//�ǂݎ��o�C�g��
		DWORD&	retReadSize );						//�ǂݎ�����o�C�g��

	//�t�@�C���ɏ�������
	// Buffer : �ǂݎ�����f�[�^���i�[����o�b�t�@
	// WriteSize : �ǂݎ�肽���T�C�Y
	// retWriteSize : ���ۂɓǂݎ�����T�C�Y
	bool Write(
		void*	Buffer,								//�������ރf�[�^���i�[���Ă���o�b�t�@
		ULONGLONG  WriteSize,						//�������ރo�C�g��
		ULONGLONG& retWriteSize );					//�������񂾃o�C�g��

	bool Write(
		void*  Buffer ,								//�������ރf�[�^���i�[���Ă���o�b�t�@
		DWORD  WriteSize ,							//�������ރo�C�g��
		DWORD& retWriteSize );						//�������񂾃o�C�g��
													
	//�t�@�C���|�C���^���w��ʒu�Ɉړ�
	//newpos : �V�����ʒu�i�擪����̃o�C�g���j
	//ret : ������true
	//�w��ʒu��EOF�𒴂���ꍇ���G���[�ɂȂ�܂���B
	bool SetPointer( ULONGLONG newpos );

	//�t�@�C���|�C���^��O��Ɉړ�
	//distance : �ړ�����
	//ret : ������true
	//�w��ʒu��EOF�𒴂���ꍇ���G���[�ɂȂ�܂���B
	bool MovePointer( LONGLONG distance );

	//�t�@�C���|�C���^���t�@�C���̖����Ɉړ�
	bool SetPointerToEnd( void );

	//�t�@�C���|�C���^���t�@�C���̐擪�Ɉړ�
	bool SetPointerToBegin( void );

	//�t�@�C���̃o�b�t�@���t���b�V������
	bool FlushBuffer( void );

	//�t�@�C���̃T�C�Y���擾
	bool GetFileSize( ULONGLONG& retSize )const;

	//�t�@�C���̃T�C�Y���擾
	// high �͕s�v�ȏꍇ�k���ł��B������high�ɓ���ׂ����ʂ�1�ȏ�̎��G���[�ɂȂ�B
	bool GetFileSize( DWORD* high , DWORD& low )const;

	//�t�@�C���̃T�C�Y���擾
	// �G���[�̏ꍇ�͂O
	ULONGLONG GetFileSize( void )const;

	//���݂̃t�@�C���|�C���^�ʒu�̎擾
	bool GetPosition( ULONGLONG& retPos )const;

	//�t�@�C�����J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	bool IsOpen( void )const;

	//�J���Ă���p�X��Ԃ��܂�
	// fullpath : true�̏ꍇ�A�J���Ă���p�X���t���p�X�ɕϊ����悤�Ƃ��܂�
	//            false�̏ꍇ�Atrue�ł��ϊ��Ɏ��s�����ꍇ�́A�t�@�C���I�[�v�����ɓn�����p�X�����̂܂ܕԂ�܂�
	WString GetPath( bool fullpath )const;

	//���݂̃t�@�C���|�C���^�̈ʒu��EOF�Ɏw�肵�܂�
	bool SetEof( void );

	//�R���g���[���R�[�h�̑��M
	// code : �R���g���[���R�[�h
	// in : ���̓f�[�^�B�����n���Ȃ��Ȃ�nullptr
	// in : �o�̓f�[�^�B�����󂯎��Ȃ��Ȃ�nullptr
	bool ExecIoControl( DWORD code , const mBinary* in , mBinary* retResult );

private:

	//�R�s�[�֎~
	mFile( const mFile& source ) = delete;
	void operator=( const mFile& source ) = delete;

protected:

	HANDLE MyHandle;
	WString MyPath;

};

#endif