//----------------------------------------------------------------------------
// �X�}�[�g�J�[�h�n���h���x�[�X
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSCBASE_H_INCLUDED
#define MSCBASE_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include <General/mBinary.h>

#include <winscard.h>
#pragma comment (lib, "winscard.lib")


namespace Definitions_SCBase
{
	//�J�[�h���[�_�[�̃��[�J�[
	enum ReaderMaker
	{
		READER_MAKER_GENERAL,	//�s���i���������Ɏw�肷��ƃ��[�_�[�����琄��j
		READER_MAKER_ACS,		//Advanced Card Systems(ACS12xx��)
		READER_MAKER_SONY,		//�\�j�[(PaSoRi��)
	};
};

class mSCBase
{
public:
	mSCBase();
	virtual ~mSCBase();

	using ReaderMaker = Definitions_SCBase::ReaderMaker;

	//�X�}�[�g�J�[�h�ɐڑ�����
	// reader : �ڑ���̃X�}�[�g�J�[�h���Ȃ����Ă��郊�[�_�[�̖��O
	// maker : �X�}�[�g�J�[�h���[�_�[�̃��[�J�[
	// ret : �������^
	bool Connect( const WString& reader , ReaderMaker maker = ReaderMaker::READER_MAKER_GENERAL );

	//�J�[�h��ID���擾����
	const mBinary& GetCardId( void )const;

private:

	mSCBase( const mSCBase& source );
	const mSCBase& operator=( const mSCBase& source ) = delete;

	//�X�}�[�g�J�[�h�T�u�V�X�e���̃n���h��
	SCARDCONTEXT MySCardContext;

	//�X�}�[�g�J�[�h�̃n���h��
	SCARDHANDLE MySCard;

	//�X�}�[�g�J�[�h�Ƃ̒ʐM�v���g�R��
	enum Protocol
	{
		T0,	//T=0�v���g�R��
		T1,	//T=1�v���g�R��
		Unknwon
	};

	//�A�N�e�B�u�ȃv���g�R��
	Protocol MyActiveProtocol;

	//�J�[�h���[�_�[�̃��[�J�[
	ReaderMaker MyReaderMaker;

protected:

	//�J�[�hID
	mBinary MyCardID;

	//�ڑ����̃J�[�h�ʂ̏���
	// ret : �����������^
	virtual bool OnConnectCallback( void );

	struct TransmitData
	{
		uint8_t cla;	//�C���X�g���N�V�����N���X
		uint8_t ins;	//�C���X�g���N�V�����R�[�h
		uint8_t p1;		//�p�����[�^�P
		uint8_t p2;		//�p�����[�^�Q
		mBinary data;	//�ǉ����M�f�[�^�i�C���X�g���N�V�����ɂ��K�v�j
		TransmitData()
		{
			cla = 0xFFu;
			ins = 0;
			p1 = 0;
			p2 = 0;
		}
		void SetData( const BYTE* dt , DWORD size )
		{
			data.clear();
			for( DWORD i = 0 ; i < size ; i++ )
			{
				data.push_back( dt[ i ] );
			}
		}
		TransmitData( const TransmitData& src )
		{
			*this = src;
		}
		const TransmitData& operator=( const TransmitData& src )
		{
			cla = src.cla;
			ins = src.ins;
			p1 = src.p1;
			p2 = src.p2;
			data = src.data;
		}
	};

	struct TransmitDataLen
	{
		uint8_t cla;	//�C���X�g���N�V�����N���X
		uint8_t ins;	//�C���X�g���N�V�����R�[�h
		uint8_t p1;		//�p�����[�^�P
		uint8_t p2;		//�p�����[�^�Q
		uint8_t len;	//�f�[�^��
		TransmitDataLen()
		{
			cla = 0xFFu;
			ins = 0;
			p1 = 0;
			p2 = 0;
			len = 0;
		}
	};

	struct ResponseData
	{
		mBinary data;	//�����f�[�^
	};

	//�X�}�[�g�J�[�h�ƒʐM���s��
	// dt : ���M����f�[�^
	// retResponse : ��M�����f�[�^
	// ret : ���������������ꍇ�^
	//       �֐��̕Ԓl�͂����܂�WinAPI�������������ǂ����ł���A�X�}�[�g�J�[�h���ŏ����������������̓��X�|���X�Ŕ��f����B
	bool Communicate( const TransmitData& dt , ResponseData& retResponse )const;

	//�X�}�[�g�J�[�h�ƒʐM���s��
	//���ǉ����M�f�[�^�͂Ȃ����A�f�[�^���ɈӖ�������ꍇ�Ɏg�p����
	// dt : ���M����f�[�^
	// retResponse : ��M�����f�[�^
	// ret : ���������������ꍇ�^
	//       �֐��̕Ԓl�͂����܂�WinAPI�������������ǂ����ł���A�X�}�[�g�J�[�h���ŏ����������������̓��X�|���X�Ŕ��f����B
	bool Communicate( const TransmitDataLen& dt , ResponseData& retResponse )const;

	//�X�}�[�g�J�[�h���[�_�[�ƒʐM���s��
	// controlcode : �R���g���[���R�[�h
	// senddata : ���M����f�[�^
	// retResponse : ��M�����f�[�^(�s�v�Ȃ�null��)
	// ret : ���������������ꍇ�^
	//       �֐��̕Ԓl�͂����܂�WinAPI�������������ǂ����ł���A�X�}�[�g�J�[�h���[�_�[���ŏ����������������̓��X�|���X�Ŕ��f����B
	bool Control( DWORD controlcode , const mBinary& senddata , mBinary* retResponse = nullptr )const;

	//�X�}�[�g�J�[�h���[�_�[�̃��[�J�[���擾����
	// ret : �擾�������[�J�[
	//���������Ƀ��[�U�[���w�肵�����A���[�_�[�����琄�肵�����[�J�[���Ԃ�܂�
	//�s���ȏꍇ��READER_MAKER_GENERAL���Ԃ邱�Ƃ�����܂�
	ReaderMaker QueryMaker( void )const;

	//�X�}�[�g�J�[�h���[�_�[�̎�ނɑΉ��������ڒʐM�R�}���h��ݒ肷��
	// retPacket : �R�}���h�̐ݒ��
	void SetDirectCommand( TransmitData& retPacket ) const;

	//�X�}�[�g�J�[�h���[�_�[�̎�ނɑΉ��������ڒʐM�R�}���h��ݒ肷��
	// retPacket : �R�}���h�̐ݒ��
	void SetDirectCommand( TransmitDataLen& retPacket ) const;

};


#endif


