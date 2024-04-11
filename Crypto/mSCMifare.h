//----------------------------------------------------------------------------
// Mifare Classic 1K �J�[�h�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSCMIFARE_H_INCLUDED
#define MSCMIFARE_H_INCLUDED

#include "mSCBase.h"
#include <array>
#include <General/mBinary.h>

class mSCMifare : public mSCBase
{
public:
	mSCMifare();
	virtual ~mSCMifare();

	//�p�[�~�b�V�����ݒ�
	enum Permission
	{
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA   �~    �~   KeyA  KeyA  KeyA  KeyA/B  KeyA/B  KeyA/B  KeyA/B
		Permission0,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA   �~    �~   �~    KeyA  �~    KeyA/B  �~      �~      �~
		Permission1,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B �~    �~   KeyB  �~    KeyB  KeyA/B  KeyB    �~      �~
		Permission2,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B �~    �~   �~    �~    �~    KeyA/B  KeyB    KeyB    KeyA/B
		Permission3,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA   KeyA  �~   KeyA  KeyA  KeyA  KeyA/B  �~      �~      KeyA/B
		Permission4,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B KeyB  �~   KeyB  �~    KeyB  KeyB    KeyB    �~      �~
		Permission5,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B KeyB  �~   �~    �~    �~    KeyB    �~      �~      �~
		Permission6,
		//AccessBits   KeyA-----  KeyB------  Data------------------------------
		//R      W     R    W     R     W     Read    Write   Inc     Dec,Tr/Res
		//KeyA/B �~    �~   �~    �~    �~    �~      �~      �~      �~
		Permission7,
	};

	using Key = std::array<BYTE,6>;
	struct Trailer
	{
		//�Z�N�^�̌��`�ł�
		//�E�L�[�`��ǂݏo�������Ƃ͂ł��Ȃ��̂ŁA�ǂݏo�����͕K��00H�ɂȂ�܂��B
		Key key_a;
		//�Z�N�^�̌��a�ł�
		Key key_b;
		//�Z�N�^���̐擪�f�[�^�u���b�N(3�̂���1��)�ɑ΂���p�[�~�b�V�����ł�
		Permission permission0;
		//�Z�N�^���̐^�񒆂̃f�[�^�u���b�N(3�̂���2��)�ɑ΂���p�[�~�b�V�����ł�
		Permission permission1;
		//�Z�N�^���̌��̃f�[�^�u���b�N(3�̂���3��)�ɑ΂���p�[�~�b�V�����ł�
		Permission permission2;
		//�g���C���[�u���b�N���̂��̂ɑ΂���p�[�~�b�V�����ł�
		Permission permission_trailer;
		//�g���C���[�u���b�N��9�o�C�g�ڂɂ��郆�[�U�[�f�[�^�ł�
		uint8_t userdata;
	};

	//�F�؃L�[���Z�b�g����
	// key : �Z�b�g����L�[
	// ret : �������^
	bool LoadAuthenticationKeys( Key key )const;

	//�Z�b�g���ꂽ�F�؃L�[��p���ĔF�؏������s��
	// sector : �F�؂��s���Z�N�^
	//  �Z�N�^�O�F�u���b�N�O�`�R�i�g���[���[�u���b�N�͂R�j
	//  �Z�N�^�P�F�u���b�N�S�`�V�i�g���[���[�u���b�N�͂V�j
	//  �Z�N�^�Q�F�u���b�N�W�`�P�P�i�g���[���[�u���b�N�͂P�P�j
	//�@�@�i�����j
	//  �Z�N�^�P�T�F�u���b�N�U�O�`�U�R�i�g���[���[�u���b�N�͂U�R�j
	// key_a : �^�̏ꍇkeyA�Ƃ��ĔF�؂���B�U�̏ꍇkeyB�Ƃ��ĔF�؂���B
	// ret : �������^
	bool ExecAuthentication( DWORD sector , bool key_a )const;

	//�P�Z�N�^(48�o�C�g)��ǂݎ��
	//�ELoadAuthenticationKeys�AExecAuthentication��p���āA�Y���Z�N�^�̓ǂݎ�肪�\�ȔF�؂��ς܂��Ă����K�v������
	// sector : �ǂݎ��Z�N�^
	// retData : �ǂݎ�����f�[�^
	// ret : �������^
	bool ReadSector( DWORD sector , mBinary& retData )const;

	//�w��Z�N�^�̃g���C���[���擾����
	// sector : �ǂݎ��Z�N�^
	// retTrailer : �ǂݎ�����f�[�^
	// ret : �������^
	bool ReadTrailer( DWORD sector , Trailer& retTrailer )const;

	//�P�Z�N�^(48�o�C�g)��������
	//�ELoadAuthenticationKeys�AExecAuthentication��p���āA�Y���Z�N�^�̏������݂��\�ȔF�؂��ς܂��Ă����K�v������
	// sector : �����ރZ�N�^
	// data : �����ރf�[�^
	//  �E48�o�C�g�ɖ����Ȃ��ꍇ�͑���Ȃ���00H��₤
	//  �E48�o�C�g�𒴂���ꍇ�́A48�o�C�g�𒴂������͐؂�̂Ă�
	// ret : �������^
	bool WriteSector( DWORD sector , const mBinary& data )const;

	//�w��Z�N�^�̃g���C���[��������
	// sector : �����ރZ�N�^
	// trailer : �����ރf�[�^
	// ret : �������^
	bool WriteTrailer( DWORD sector , const Trailer& trailer )const;

protected:

	//�ڑ����̃J�[�h�ʂ̏���
	// ret : �����������^
	virtual bool OnConnectCallback( void );

private:
	mSCMifare( const mSCMifare& source );
	const mSCMifare& operator=( const mSCMifare& source ) = delete;

	//���M�f�[�^�ɑ΂��āA���ۂ����Ԃ��Ă��Ȃ��R�}���h�̑���M�������܂Ƃ߂��w���p�[�֐�
	// dt : ���M����f�[�^
	// ret : ���������������ꍇ�^(WinAPI�A�J�[�h�����������������Đ^)
	bool DefaultCommunication( const TransmitData& dt )const;

};

#ifdef SAMPLE_CODE

#include "mStandard.h"
#pragma comment (lib, "winscard.lib")
#pragma comment (lib, "scarddlg.lib")
#include <Crypto/mSCReader.h>
#include <Crypto/mSCMifare.h>

//�X�}�[�g�J�[�h���Z�b�g���ꂽ�Ƃ��̃R�[���o�b�N
void OnPresent( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	//�Z�b�g���ꂽ�X�}�[�g�J�[�h��Mifare1K�̏ꍇ
	if( opt->OnCardPresent->Kind == Definitions_SCReader::CardKind::MIFARE_CLASSIC_1K )
	{
		//�J�[�h�Ɛڑ�����
		mSCMifare card;
		if( !card.Connect( sc.GetCurrentCardReaderName() ) )
		{
			return;
		}
		//�F�؃L�[���Z�b�g
		//�������ł́A�Z�N�^�O�A�P�Ƃ��Ƀf�t�H���g�i�S��FF�j���Z�b�g����Ă���Ɖ���
		card.LoadAuthenticationKeys( mSCMifare::Key( {0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu} ) );

		//���Z�N�^�O��ǂݎ���āA�L�[������������e�X�g
		//�Z�N�^�O�̔F�؎��{(�ȍ~�G���[�`�F�b�N�ȗ�)
		if( !card.ExecAuthentication( 0 , true ) )
		{
			//�F�؎��s
			return;
		}
		//�Z�N�^�O�̃f�[�^��ǂݎ��
		mBinary data;
		card.ReadSector( 0 , data );
		//�Z�N�^�O�̃g���C���[��ǂݎ��
		mSCMifare::Trailer trailer;
		card.ReadTrailer( 0 , trailer );
		//�Z�N�^�O�̃L�[�`��ʂ̂��̂ɏ���������
		//trailer.key_a = mSCMifare::Key( {0x12u,0x34u,0x56u,0x78u,0x9Au,0xBCu} );
		//card.WriteTrailer( 0 , trailer );

		//���Z�N�^�P�̃f�[�^���X�V����e�X�g
		//�Z�N�^�P��F��
		card.ExecAuthentication( 1 , true );
		//�Z�N�^�P�̃f�[�^��ǂݎ��
		card.ReadSector( 1 , data );
		//�Z�N�^�P�̂P�o�C�g�ڂ��X�V
		data[ 0 ]++;
		//�Z�N�^�P��������
		card.WriteSector( 1 , data );
	}
	return;
}

int main( int argc , char** argv )
{
	//�I�u�W�F�N�g�𐶐����A�X�}�[�g�J�[�h�T�[�r�X�̋N���𐧌����Ԗ������őҋ@����
	mSCReader reader;
	reader.WaitForServiceAvailable( INFINITE );

	//�o�b�ɐڑ�����Ă���J�[�h���[�_�[�̈ꗗ���擾����
	WStringVector cardreader;
	reader.GetCardReaderList( cardreader );

	//�擾�����ꗗ�̐擪�i�v�f�O�j�̃��[�_�[�ɂ��āA
	//�X�}�[�g�J�[�h�̃Z�b�g�E�A���Z�b�g�A���[�_�[�̎��O�������o����B
	mSCReader::NotifyOption notifyopt;
	notifyopt.OnCardPresent.Mode = mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
	notifyopt.OnCardPresent.Notifier.CallbackFunction = OnPresent;

	//�Ď��J�n
	reader.StartWatch( cardreader[ 0 ] , notifyopt );

	//�ҋ@�B����̓T���v���Ȃ̂ŒP�Ȃ�E�G�C�g�B
	DWORD flag = 1;
	while( flag )
	{
		Sleep( 1000 );
	}

	//�N���[���A�b�v
	reader.EndWatch();
	return 0;
}


#endif

#endif


