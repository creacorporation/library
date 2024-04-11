//----------------------------------------------------------------------------
// �X�g���[�~���O�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------


#ifndef MFILEREADSTREAMBASE_H_INCLUDED
#define MFILEREADSTREAMBASE_H_INCLUDED

#include "mStandard.h"
#include "mFile.h"
#include "mTCHAR.h"
#include <deque>
#include <memory>

class mFileReadStreamBase
{
public:
	mFileReadStreamBase();
	virtual ~mFileReadStreamBase();

	//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
	//ret : �ǂݎ��������
	//��EOF�̏ꍇ�A���̃��[�h�X�g���[���������A�N�Z�X�̂��̂��ǂ����ňӖ����Ⴂ�܂�
	// �����A�N�Z�X�̏ꍇ   : �t�@�C���̏I�[�i����ȏ�̃f�[�^�͂Ȃ��j
	// �񓯊��A�N�Z�X�̏ꍇ : ���ݓǂݎ���f�[�^���Ȃ��i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
	virtual INT Read( void ) = 0;

	//ANSI�łP�����ǂݍ��݂܂�
	//retChar : �ǂݎ��������
	//ret : �^�̏ꍇ : EOF�ɓ��B���Ă��Ȃ�
	//      �U�œ����A�N�Z�X�̏ꍇ   : �t�@�C���̏I�[�i����ȏ�̃f�[�^�͂Ȃ��j
	//      �U�Ŕ񓯊��A�N�Z�X�̏ꍇ : ���ݓǂݎ���f�[�^���Ȃ��i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
	bool ReadSingle( char& retChar );

	//UNICODE(LE)�łP�����ǂݍ��݂܂�
	//retWchar : �ǂݎ��������
	//ret : �^�̏ꍇ : EOF�ɓ��B���Ă��Ȃ�
	//      �U�œ����A�N�Z�X�̏ꍇ   : �t�@�C���̏I�[�i����ȏ�̃f�[�^�͂Ȃ��j
	//      �U�Ŕ񓯊��A�N�Z�X�̏ꍇ : ���ݓǂݎ���f�[�^���Ȃ��i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
	bool ReadSingle( wchar_t& retWchar );

	//�ǂݎ�莞�̃G���R�[�h�w��
	enum Encode
	{
		ENCODE_ASIS,		//�ǂݎ��t�@�C���̃G���R�[�h�ɂ��čl�����Ȃ��i���邪�܂܂ɓǂݍ��ށj
		ENCODE_SHIFT_JIS,	//�ǂݎ��t�@�C���̃G���R�[�h��SHIFT JIS�ł���
		ENCODE_UTF16,		//�ǂݎ��t�@�C���̃G���R�[�h��UTF16(BOM�Ȃ�)�ł���
	};

	//1�s�ǂݎ��E�w��T�C�Y�ǂݎ�莸�s���̓���w��
	enum OnLineReadError
	{
		//�ǂ߂Ȃ����������͔j������
		// ���ʁ�
		// �EReadLine:�󂪕Ԃ�
		// �EReadBinary:���g�͕ۏ؂���Ȃ�
		// �G���[�����܂łɓǂ�ł����f�[�^���ǂݎ��ς݂ɂȂ�
		// �֐��̖߂�l�����s
		LINEREADERR_DISCARD,
		
		//���ǂݎ���Ԃɖ߂�
		// ���ʁ�
		// �EReadLine:�󂪕Ԃ�
		// �EReadBinary:���g�͕ۏ؂���Ȃ�
		// �G���[�����܂łɓǂ�ł����f�[�^�����Ǐ�Ԃɖ߂�
		// �֐��̖߂�l�����s
		LINEREADERR_UNREAD,

		//�ǂ߂��ʒu�܂ł����ʂƂ���
		// ���ʁ�
		// �EReadLine:�G���[�����܂łɓǂ�ł����f�[�^���Ԃ�
		// �EReadBinary:�G���[�����܂łɓǂ�ł����f�[�^���Ԃ�
		// �G���[�����܂łɓǂ�ł����f�[�^���ǂݎ��ς݂ɂȂ�
		// �֐��̖߂�l������
		LINEREADERR_TRUNCATE,
	};

	//�ǂݎ�莞�̃t�@�C���̃G���R�[�h�����ł��邩���w�肵�܂��B
	//�@��F SetEncode( ENCODE_SHIFT_JIS )	����t�@�C���̃G���R�[�h��SHIFT JIS�Ǝw��
	//     �@��
	//       WString str
	//       ReadLine( str ) ����t�@�C����SHIFT JIS�œǂݎ���āAUTF16�ɕϊ�����str�Ɋi�[
	// encode : �t�@�C���̃G���R�[�h
	// ret : �������^
	// �����̊֐����Ăяo���Ȃ��i�G���R�[�h���w��j�ꍇ�́AENCODE_ASIS�ɂȂ�܂��B
	bool SetEncode( Encode encode );

	//�P�s�ǂݎ��܂��B
	//ret : �ǂݎ����s�����Ƃ�true�B�����ǂݎ��Ȃ������Ƃ�false�B
	//�E���s�����́A\r�܂���\n�܂���\r\n�ł��B
	//�E�ǂݎ����������ɁA���s�R�[�h�͊܂܂�܂���B
	//�E�����G���R�[�h��SHIFT_JIS�Ƃ��ď�������܂��B
	bool ReadLine( AString& retResult , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//�P�s�ǂݎ��܂��B
	//ret : �ǂݎ����s�����Ƃ�true�B�����ǂݎ��Ȃ������Ƃ�false�B
	//�E���s�����́A\r�܂���\n�܂���\r\n�ł��B
	//�E�ǂݎ����������ɁA���s�R�[�h�͊܂܂�܂���B
	//�E�����G���R�[�h��UNICODE(BOM�Ȃ�16bit���g���G���f�B�A��)�Ƃ��ď�������܂��B
	bool ReadLine( WString& retResult , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//�w��T�C�Y��ǂݎ��܂�
	//retReadSize : ����I�����邩�A�G���[����������܂łɓǂݎ�����o�C�g���B�s�v�Ȃ�nullptr�ŉB
	//ret : �w��T�C�Y�ǂݎ�萬����true�A���s��false
	//�E�w��T�C�Y��ǂݎ��O��EOF�ɂȂ�Ǝ��s���܂�
	bool ReadBinary( BYTE* retResult , size_t ReadSize , size_t* retReadSize = nullptr , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//EOF�ɒB���Ă��邩�𒲂ׂ܂�
	//��true�̏ꍇ�A���̃��[�h�X�g���[���������A�N�Z�X�̂��̂��ǂ����ňӖ����Ⴂ�܂�
	// �����A�N�Z�X�̏ꍇ   : �t�@�C���̏I�[�i����ȏ�̃f�[�^�͂Ȃ��j
	// �񓯊��A�N�Z�X�̏ꍇ : ���ݓǂݎ���f�[�^���Ȃ��i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
	//�E�G���R�[�h��"ENCODE_UTF16"���w�肵�Ă���ꍇ�A�o�C�g�P�ʂ̒[�������݂���ƁA
	//  ���̍Ō�̃o�C�g�ɂ��Ă�EOF�ƌ��Ȃ��܂��B
	virtual bool IsEOF( void )const = 0;

	//�t�@�C�����J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const = 0;

protected:

	mFileReadStreamBase( const mFileReadStreamBase& source ) = delete;
	void operator=( const mFileReadStreamBase& source ) = delete;

protected:

	Encode MyEncode;		//�ǂݎ�莞�̃G���R�[�h

	//�ǂݍ��񂾂̂�����ώ~�߂��ƃo�b�t�@�ɉ����߂����ꍇ�ɁA
	//���ɓǂނƂ��܂Ŏ���Ă������߂̃o�b�t�@
	class UnReadBuffer
	{
	public:
		UnReadBuffer() = default;
		virtual ~UnReadBuffer() = default;

		//�^�̕������L���b�V���ɉ����Ԃ��܂�
		//val : �����Ԃ��f�[�^
		//�E���g���G���f�B�A���ƌ��Ȃ��܂�
		template<class T> void Unread( T val )
		{
			for( size_t i = 0 ; i < sizeof( T ) ; i++ )
			{
				size_t offset = ( sizeof( T ) - 1 - i ) * 8;
				MyBuffer.push_front( ( val >> offset ) & 0xFF );
			}
		}

		//�o�b�t�@����1������肾��
		//�o�b�t�@����Ȃ̂Ɏ��o�����Ƃ����EOF�ɂȂ�(���ӁI)
		//ret : �o�b�t�@������o�����f�[�^�B�o�b�t�@����̏ꍇEOF�B
		INT Read( void );

		//�o�b�t�@�͋󂩁H
		//ret : ��ł����true
		bool IsEmpty( void )const;

		//�o�b�t�@���N���A
		void Clear( void );

	private:
		UnReadBuffer( const UnReadBuffer& source ) = delete;
		void operator=( const UnReadBuffer& source ) = delete;

		//��x�ǂݎ�������̂́A�L���b�V���ɉ����Ԃ��ꂽ�f�[�^���i�[
		typedef std::deque<BYTE> Buffer;
		Buffer MyBuffer;
	};

	UnReadBuffer MyUnReadBuffer;

	std::unique_ptr<BYTE[]> MyReadCacheHead;	//�L���b�V�����Ă���f�[�^�̐擪�o�C�g
	DWORD MyReadCacheCurrent;					//����Read�ŕԂ��ʒu
	DWORD MyReadCacheRemain;					//���ǂ̃L���b�V���̃T�C�Y

	//EOF�ƂȂ������H
	bool MyIsEOF;

};

#endif //MFILEREADSTREAMBASE_H_INCLUDED

