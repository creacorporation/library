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

#ifndef MFILEWRITESTREAMBASE_H_INCLUDED
#define MFILEWRITESTREAMBASE_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "mFileReadStreamBase.h"
#include <memory>

//�X�g���[�~���O�t�@�C���ǂݍ��ݑ���

class mFileWriteStreamBase
{
public:
	mFileWriteStreamBase();
	virtual ~mFileWriteStreamBase();

	//�t�@�C������܂�
	virtual bool Close( void ) = 0;

	//�P������������
	virtual bool Write( INT data ) = 0;

	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	bool WriteString( const AString& line );	//ANSI���������������
	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	bool WriteString( const WString& line );	//UNICODE���������������
	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	bool WriteString( const char* line );		//ANSI���������������
	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	bool WriteString( const wchar_t* line );	//UNICODE���������������

	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	template <typename... args> bool WriteString( const AString& format , const args... va )
	{
		AString str;
		sprintf( str , format.c_str() , va... );
		return WriteString( str );
	}
	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	template <typename... args> bool WriteString( const WString& format , const args... va )
	{
		WString str;
		sprintf( str , format.c_str() , va... );
		return WriteString( str );
	}
	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	template <typename... args> bool WriteString( const char* format , const args... va )
	{
		AString str;
		if( format == nullptr )
		{
			return false;
		}
		sprintf( str , format , va... );
		return WriteString( str );
	}
	//�P�s�������݂܂��B
	//���s�R�[�h������ɕt�������肵�Ȃ��̂ŁA�������ޕ�����̕��ɉ��s���܂߂ĉ������B
	//�E������1�̏ꍇ�́A�����̕���������̂܂܏o�͂��܂�
	//�E������2�ȏ�̏ꍇ�́A1�ڂ������w�蕶����Ƃ��A2�ڈȍ~�t�H�[�}�b�g������������o�͂��܂�
	template <typename... args> bool WriteString( const wchar_t* format , const args... va )
	{
		WString str;
		if( format == nullptr )
		{
			return false;
		}
		sprintf( str , format , va... );
		return WriteString( str );
	}

	//�w��T�C�Y�������݂܂�
	//ret : ������true
	bool WriteBinary( const BYTE* buffer , size_t size );

	//�w��X�g���[������ǂݎ���Ă��̂܂܏������݂܂�
	//���w��o�C�g�������ނ��A�ǂݍ��݌��X�g���[����EOF�ɂȂ�܂ŏ������݂܂�
	//fp : �������݌�
	//sz : �������ރo�C�g��
	//retWritten : �������񂾃T�C�Y(�s�v�Ȃ�nullptr�ŉ�)
	//ret : ������true
	bool WriteStream( mFileReadStreamBase& fp , size_t sz , size_t* retWritten = nullptr );

	//�w��X�g���[������ǂݎ���Ă��̂܂܏������݂܂�
	//���ǂݍ��݌��X�g���[����EOF�ɂȂ�܂ŏ������݂܂�
	//fp : �������݌�
	//retWritten : �������񂾃T�C�Y(�s�v�Ȃ�nullptr�ŉ�)
	//ret : ������true
	bool WriteStream( mFileReadStreamBase& fp , size_t* retWritten = nullptr );


	//�t�@�C�����J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const = 0;

private:
	mFileWriteStreamBase( const mFileWriteStreamBase& source ) = delete;
	void operator=( const mFileWriteStreamBase& source ) = delete;

protected:

	//�L���b�V������������
	virtual bool FlushCache( void ) = 0;

	std::unique_ptr<BYTE[]> MyWriteCacheHead;		//�L���b�V�����Ă���f�[�^�̐擪�o�C�g
	DWORD MyWriteCacheRemain;						//���������݂̃L���b�V���̃T�C�Y
	DWORD MyWriteCacheWritten;						//�������ݍς݃L���b�V���̃T�C�Y

};

#endif	//MFILEWRITESTREAMBASE_H_INCLUDED

