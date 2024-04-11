//----------------------------------------------------------------------------
// Felica Light-S �J�[�h�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSCFELICA_H_INCLUDED
#define MSCFELICA_H_INCLUDED

#include "mSCBase.h"
#include <General/mBinary.h>
#include <Crypto/mSCFelicaDefinitions.h>
#include <Crypto/mSCFelicaMac.h>

class mSCFelica : public mSCBase
{
public:
	mSCFelica();
	virtual ~mSCFelica();

	///-----------------------------------------
	/// �ꎟ���s
	///-----------------------------------------

	//�P�����s�f�[�^
	struct IssuanceParam
	{
		//ID�u���b�N(82H)�ɑ��݂��郆�[�U�[���C�ӂɗ��p�ł���l
		//���u�ʉ��J�[�h���v�𗘗p����ꍇ�́A���̒l�͌������̃V�[�h�̈ꕔ�ɂȂ�܂��B
		BYTE Id[ 6 ];

		//�J�[�h���o�[�W����(86H)�̒l�B
		//�J�[�h���Ƀo�[�W�������������邽�߂̒l�ł����A�����I�ɂ̓��[�U�[���C�ӂɗ��p�\�B
		uint16_t KeyVersion;

		//�J�[�h��CK(87H)
		//�E�P�U�o�C�g�̃f�[�^���w�肵���Ƃ��́A���̃o�C�i�����J�[�h���Ƃ��܂�
		//  �i�w�肵�������̂��̂��J�[�h�ɏ����݂܂��j
		//�E�Q�S�o�C�g�̃f�[�^���w�肵���Ƃ��́A���̃o�C�i�����ʉ��}�X�^�[���Ƃ��܂�
		//  �i�w�肵������ID�u���b�N����\�j�[�W���̕��@�Ő������������J�[�h�ɏ����݂܂��j
		//�E�҂�����P�U�o�C�g�܂��́A�Q�S�o�C�g�̃f�[�^��ێ����Ă����ԂłȂ��ƃG���[�ɂȂ�܂�
		mSecureBinary Key;

		//�J�[�h���̍X�V�������邩(�^�̏ꍇ����)
		//�E�X�V���ɂ���ƁA�ꎟ���s�ŃV�X�e���u���b�N�����������֎~�ɂ������MAC���������݂ŃJ�[�h�����X�V�ł��܂�
		//�E�X�V�s�ɂ���ƁA�ꎟ���s�ŃV�X�e���u���b�N�����������֎~�ɂ�����̓J�[�h�����X�V�ł��Ȃ��Ȃ�܂�
		//�E�V�X�e���u���b�N�����������֎~�ɂ���܂ł́A���̐ݒ�Ɋւ�炸�J�[�h�������������\�ł�
		bool EnableKeyChange;

		//STATE�u���b�N�̔F�؂�v����ꍇ�͐^
		//�E�v����ɊO���F��(���ݔF��)�������ꍇ�͐^�B���Ȃ��ꍇ�͋U�B
		//�E�ꎟ���s���_�ł́A�ꎟ���s�`�񎟔��s�܂ł̊ԂɔF�؂��g���������ǂ����Őݒ肷��΂悭�A
		//�@�ŏI�I�ɂ́A�񎟔��s���Ƀ��[�U�[�u���b�N�̃p�[�~�b�V�����̐ݒ莟��Œl�����܂�܂��B
		bool EnableStateMacReq;

		IssuanceParam()
		{
			ZeroMemory( Id , sizeof( Id ) );
			Key.assign( 16 , 0x00 );
			KeyVersion = 0;
			EnableKeyChange = true;
			EnableStateMacReq = false;
		}
	};

	//�ꎟ���s���s���܂�
	// lock : �V�X�e���u���b�N�����b�N���A�ȍ~�̏����������֎~���܂�
	//    ���b�N���s����WCNT�̒l(MAC�t���������݂Ɏg�p)�����Z�b�g����܂��B���������܂ł͂킸���ȉ񐔂���(MAC���ł�)�������݂��ł��܂���B
	// param : �ݒ肷��ꎟ���s�̃f�[�^
	// ret : �������^
	bool ExecFirstIssuance( bool lock , const IssuanceParam& param );

	///-----------------------------------------
	/// �񎟔��s
	///-----------------------------------------

	//���[�U�[�u���b�N�̖��O
	using UserBlock = mSCFelicaDefinitions::UserBlock;

	//�ǂݎ��̃p�[�~�b�V����
	using ReadPermission = mSCFelicaDefinitions::ReadPermission;

	//�������݂̃p�[�~�b�V����
	using WritePermission = mSCFelicaDefinitions::WritePermission;

	//���[�U�[�u���b�N�̃p�[�~�b�V����
	struct Permission
	{
		//�ǂݍ��݃p�[�e�B�V����
		//0�`13�����[�U�[�u���b�N��S_PAD0�`S_PAD13�ɑΉ����A14��REG�ɑΉ����܂��B
		ReadPermission Read[ 15 ];
		//�������݃p�[�e�B�V����
		//0�`13�����[�U�[�u���b�N��S_PAD0�`S_PAD13�ɑΉ����A14��REG�ɑΉ����܂��B
		WritePermission Write[ 15 ];
	};

	//�񎟔��s���s���܂�
	// lock : �p�[�~�b�V���������b�N���A�ȍ~�̐ݒ�ύX���֎~���܂�
	// param : �ݒ肷��p�[�~�b�V����
	// ret : �������^
	bool ExecSecondIssuance( bool lock , const Permission& param );

	///-----------------------------------------
	/// �F��
	///-----------------------------------------

	//�����F�؂܂��́A�O���i���݁j�F�؂��s���܂�
	// key : �J�[�h���̃o�C�i��(�҂�����16or24�o�C�g�̃f�[�^��ێ����Ă��邱��)
	//  16�o�C�g�̎����̃o�C�i�����J�[�h���A24�o�C�g�̂Ƃ��ʉ��J�[�h���̃}�X�^�[���Ƃ��ď������܂�
	// int_auth_only : �^�̏ꍇ�A�����F�؂̂ݍs���܂�
	//                    �����F�؁����[�_�[���J�[�h��^���Ȃ���(���������������Ă���)�ƔF�߂Ă�����
	//                 �U�̏ꍇ�A���ݔF�؂��s���܂�(�����F�؁A�O���F�؂̗������s���܂�)
	//                    ���ݔF�؁����[�_�[�ƃJ�[�h�����݂��ɐ^���Ȃ���(���������������Ă���)�ƔF�߂Ă�����
	// ret : �������^
	bool ExecAuthentication( const mSecureBinary& key , bool int_auth_only );

	//�F�؏��
	using AuthStatus = mSCFelicaDefinitions::AuthStatus;

	//���ݔF�؂��s���Ă��邩��Ԃ��܂�
	// ret : ���݂̔F�؏��
	AuthStatus GetAuthStatus( void )const;

	//�w��̃o�C�i�������ۂɃJ�[�h�Ƀn�b�V�������A���̌��ʂ𓾂܂�
	//�E�J�[�h�����F�؏�Ԃ̂Ƃ������g�p�ł��܂���
	//�E�Ԃ����l�́Ain���`�������W�Ƃ��AID���擾�����Ƃ���MAC�̒l�ł�
	// in : �n�b�V������f�[�^
	//      16�o�C�g�ɖ����Ȃ��ꍇ�͑���Ȃ����͌���00h�𖄂߁A16�o�C�g�𒴂���Ƃ��͐擪��16�o�C�g���g�p���܂��B
	// retHash : ���ʊi�[��
	// ret : �������^
	bool CalcHash( const mBinary& in , mBinary& rethash );

	//�J�[�h�����X�V����
	//�E���炩���ߊO���F�؁i���ݔF�؁j���������Ă���K�v������܂�
	//�E�ꎟ���s���ɁA�J�[�h���̍X�V��������Ă���K�v������܂�
	// key : �J�[�h���̃o�C�i��(�҂�����16or24�o�C�g�̃f�[�^��ێ����Ă��邱��)
	//  16�o�C�g�̎����̃o�C�i�����J�[�h���A24�o�C�g�̂Ƃ��ʉ��J�[�h���̃}�X�^�[���Ƃ��ď������܂�
	// keyver : �V�������o�[�W����
	// ret : �������^
	bool UpdateKey( const mSecureBinary& key , uint16_t keyver );

	///-----------------------------------------
	/// �ݒ���擾
	///-----------------------------------------

	//ID�u���b�N(82H)�ɑ��݂��郆�[�U�[���C�ӂɗ��p�ł���l���擾���܂�
	//���u�ʉ��J�[�h���v�𗘗p����ꍇ�́A���̒l�͌������̃V�[�h�̈ꕔ�ɂȂ�܂��B
	// retVal : �擾�����l
	// macauth : �^�̏ꍇ�AMAC_A�ɂ��F�؂��s���܂�
	//   ���^�̏ꍇ�A���炩���ߓ����F�؂܂��́A�O���i���݁j�F�؂��������Ă���K�v������܂�
	// ret : �������^
	bool GetUserID( mBinary& retVal , bool macauth = false )const;

	//���o�[�W�������擾���܂�
	// retVal : �擾�����l
	// macauth : �^�̏ꍇ�AMAC_A�ɂ��F�؂��s���܂�
	//   ���^�̏ꍇ�A���炩���ߓ����F�؂܂��́A�O���i���݁j�F�؂��������Ă���K�v������܂�
	// ret : �������^
	bool GetKeyVersion( uint16_t& retVal , bool macauth = false )const;

	//�J�[�h������ROM���j�����Ă��Ȃ�����Ԃ��܂�
	//�ECRC�ɂ��s�������������̌��،��ʂ�Ԃ��܂��B����̓J�[�h�N�����ɂP�񂾂��s����`�F�b�N�ł��B
	// macauth : �^�̏ꍇ�AMAC_A�ɂ��F�؂��s���܂�
	//   ���^�̏ꍇ�A���炩���ߓ����F�؂܂��́A�O���i���݁j�F�؂��������Ă���K�v������܂�
	// ret : �`�F�b�N���i�̏ꍇ�^�A�`�F�b�N�s���i�̏ꍇ�U
	bool GetChecksumResult( bool macauth = false )const;

	///-----------------------------------------
	/// �ǂݏ����֘A
	///-----------------------------------------

	//�ǂݏ������s���u���b�N�̃G���g��
	using DataBlockEntry = mSCFelicaDefinitions::DataBlockEntry;
	//�ǂݏ������s���u���b�N�̃f�[�^
	using DataBlock = mSCFelicaDefinitions::DataBlock;
	//�ǂݎ����s�������u���b�N�̔ԍ�
	using ReadRequestBlock = mSCFelicaDefinitions::ReadRequestBlock;
	//���[�U�[�u���b�N�̖��O
	using UserBlock = mSCFelicaDefinitions::UserBlock;
	//REG�f�[�^
	using RegData = mSCFelicaDefinitions::RegData;

	//�f�[�^�̓ǂݎ��
	//�E���[�U�[�u���b�N�̂ݓǂݎ��ł��܂�
	//  �u���b�N�ԍ�S_PAD0(00h)�`REG(0Eh)�͈̔́B
	//�E�P�x�ɒʐM�ł���T�C�Y�𒴂��ēǂݎ��Ώۂ��w�肵���ꍇ�́A������ɕ������Ă�����s���܂�
	//�E�����u���b�N���Q��ȏ�ǂݎ�낤�Ƃ���̂͋֎~�ł�
	// request : �ǂݎ�肽���u���b�N�ԍ��̈ꗗ
	// retResponse : �ǂݎ�茋��
	//   �������Arequest�Ɏw�肵���u���b�N�̕��я��ɓǂݎ�����f�[�^���i�[����邱�Ƃ�ۏ؂��܂�
	// macauth : �^�̏ꍇ�AMAC_A�ɂ��F�؂��s���܂�
	//   ���^�̏ꍇ�A���炩���ߓ����F�؂܂��́A�O���i���݁j�F�؂��������Ă���K�v������܂�
	// ret : �������^
	bool Read( const ReadRequestBlock& request , DataBlock& retResponse , bool macauth = false )const;

	//�f�[�^�̏�������
	//�E���[�U�[�u���b�N�̂ݏ������݂ł��܂�
	//  �u���b�N�ԍ�S_PAD0(00h)�`REG(0Eh)�͈̔́Benum UserBlock�Ŗ��̂��`���Ă���܂��B
	//�E�����̏������ݑΏۂ��w�肵���ꍇ�́A�P�u���b�N���������ď������݂��s���܂��B
	//  ���̂Ƃ��A�����ݏ������s���鏇�Ԃ́Adata�̔z��̕��я��ƂȂ�܂��B
	//�E�����u���b�N���Q��ȏ㏑�������Ƃ���̂͋֎~�ł�
	// data : �����ރf�[�^
	// macauth : �^�̏ꍇ�AMAC_A�ɂ��F�؂��s���܂�
	//   ���^�̏ꍇ�A���炩���ߓ����F�؂܂��́A�O���i���݁j�F�؂��������Ă���K�v������܂�
	// ret : �������^
	bool Write( const DataBlock& data , bool macauth = false )const;

	//DataBlockEntry��RegData�ɕϊ�
	//�ERead���\�b�h�Ŏ擾����DataBlock��REG(���Z)�u���b�N�Ƃ��ēǂݎ��܂�
	// src : �ǂݎ��f�[�^�u���b�N(�u���b�N�ԍ���REG���W�X�^�̂��̂łȂ��Ă��ǂݎ��܂�)
	// ret : �ǂݎ�茋��
	RegData DataBlockEntryToRegData( const DataBlockEntry& src );

	//DataBlockEntry��RegData�ɕϊ�
	//�ERead���\�b�h�Ŏ擾����DataBlock��REG(���Z)�u���b�N�Ƃ��ēǂݎ��܂�
	// src : �ǂݎ��f�[�^�u���b�N(�u���b�N�ԍ���REG���W�X�^�̂��̂łȂ��Ă��ǂݎ��܂�)
	// ret : �ǂݎ�茋��
	void DataBlockEntryToRegData( const DataBlockEntry& src , RegData& retRegData );

	//RegData��DataBlockEntry�ɕϊ�
	//�EREG(���Z)�u���b�N���������ݗp��DataBlockEntry�`���ɃG���R�[�h���܂�
	// src : ���Z�u���b�N
	// ret : �G���R�[�h����
	DataBlockEntry RegDataToDataBlockEntry( const RegData& src );

	//RegData��DataBlockEntry�ɕϊ�
	//�EREG(���Z)�u���b�N���������ݗp��DataBlockEntry�`���ɃG���R�[�h���܂�
	// src : ���Z�u���b�N
	// ret : �G���R�[�h����
	void RegDataToDataBlockEntry( const RegData& src , DataBlockEntry& retDataBlockEntry );

private:

	mSCFelica( const mSCFelica& source );
	const mSCFelica& operator=( const mSCFelica& source ) = delete;

protected:

	//�F�؃I�u�W�F�N�g
	mutable mSCFelicaMac MyFelicaMac;

	//�ڑ����̃J�[�h�ʂ̏���
	// ret : �����������^
	virtual bool OnConnectCallback( void );

	//�f�[�^�̓ǂݎ��
	// request : �ǂݎ�肽���u���b�N�ԍ��̈ꗗ(�ő�4�u���b�N)
	// retResponse : �ǂݎ�茋��
	//   �������Arequest�Ɏw�肵���u���b�N�̕��я��ɓǂݎ�����f�[�^���i�[����邱�Ƃ�ۏ؂��܂�
	// ret : �������^
	bool RawRead( const ReadRequestBlock& request , DataBlock& retResponse )const;

	//�f�[�^�̏�������
	// data : �����ރf�[�^
	//   �������Adata�Ɏw�肵���u���b�N�̕��я��ɏ������ݏ������s���܂�
	//   �e�G���g����Data��16�o�C�g�ɖ����Ȃ��ꍇ�́A����Ȃ�����00H��₢�܂��B16�o�C�g��葽���ꍇ�́A�擪��16�o�C�g�̂ݏ����݂܂��B
	// ret : �������^
	bool RawWrite( const DataBlock& data )const;

	//�f�[�^�̓ǂݎ��(MAC�F�ؗL��)
	//�����炩���ߓ����F�؂܂��́A�O���i���݁j�F�؂��������Ă���K�v������܂�
	// request : �ǂݎ�肽���u���b�N�ԍ��̈ꗗ(�ő�3�u���b�N)
	// retResponse : �ǂݎ�茋��
	//   �������Arequest�Ɏw�肵���u���b�N�̕��я��ɓǂݎ�����f�[�^���i�[����邱�Ƃ�ۏ؂��܂�
	// ret : �������^
	bool MacReadInternal( const ReadRequestBlock& request , DataBlock& retResponse )const;

	//�f�[�^�̏�������(MAC�F�ؗL��)
	//�����炩���ߊO���i���݁j�F�؂��������Ă���K�v������܂�
	// data : �����ރf�[�^
	// ret : �������^
	bool MacWriteInternal( const DataBlockEntry& data , DWORD wcnt )const;

	//WCNT�l(90h)���擾���܂�
	// ret : ������WCNT�l�B���s��0xFFFFFFFFu�B
	DWORD GetWCNT( void )const;

	//�����F�؂܂��́A�O���i���݁j�F�؂��s���܂�
	// key : �J�[�h���̃o�C�i��(�҂�����16�o�C�g�̃f�[�^��ێ����Ă��邱��)
	// int_auth_only : �^�̏ꍇ�A�����F�؂̂ݍs���܂�
	//                 �U�̏ꍇ�A���ݔF�؂��s���܂�(�����F�؁A�O���F�؂̗������s���܂�)
	// ret : �������^
	bool ExecAuthenticationInternal( const mSecureBinary& key , bool int_auth_only );

};

#endif
