//----------------------------------------------------------------------------
// �X�}�[�g�J�[�h���[�_�[�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSCREADER_H_INCLUDED
#define MSCREADER_H_INCLUDED

#include <mStandard.h>
#include <General/mNotifyOption.h>
#include <General/mTCHAR.h>
#include <General/mBinary.h>
#include <Thread/mThread.h>

#include <winscard.h>
#pragma comment (lib, "winscard.lib")


namespace Definitions_SCReader
{
	//ATR����F�����ꂽ�J�[�h�̎��
	enum CardKind
	{
		//Mifare Classic 1K
		MIFARE_CLASSIC_1K,
		//Felica(Felica Standard/Light/LightS�̔��ʂ͐ڑ����Ă݂Ȃ��ƕ�����Ȃ�)
		FELICA,
		//�s���E���̑�
		UNKNOWN
	};

	//�J�[�h��F�������ꍇ�̃C�x���g�ł�
	struct OnCardPresent
	{
		CardKind Kind;
		DWORD Status;
		mBinary Atr;
	};

	//�J�[�h�����O���ꂽ�ꍇ�̃C�x���g�ł�
	struct OnCardRemoved
	{
		DWORD Status;
		mBinary Atr;
	};

	//���[�_�[�����O���ꂽ�Ǝv����ꍇ�̃C�x���g�ł�
	struct OnReaderGone
	{
		DWORD Status;
	};

};

//�X�}�[�g�J�[�h���[�_�[�̃n���h���B�ȉ��̋@�\������܂��B
//�E�X�}�[�g�J�[�h�T�u�V�X�e�������p�\�ł��邩���肵�A���p�\�ɂȂ�܂őҋ@����
//�E�V�X�e�����F�����Ă���X�}�[�g�J�[�h���[�_�[�̈ꗗ���쐬����
//�E�X�}�[�g�J�[�h���[�_�[��ɃJ�[�h���u���ꂽ�E�O���ꂽ�A���[�_�[���̂��̂����O���ꂽ���Ƃ����o����
class mSCReader
{
public:
	mSCReader();
	virtual ~mSCReader();

	//�J�[�h�̎��
	using CardKind = Definitions_SCReader::CardKind;

	//�ʒm���̃p�����[�^
	union NotifyFunctionOptPtr
	{
		//�J�[�h��F�������ꍇ�̃C�x���g�ł�
		Definitions_SCReader::OnCardPresent* OnCardPresent;
		//�J�[�h�����O���ꂽ�ꍇ�̃C�x���g�ł�
		Definitions_SCReader::OnCardRemoved* OnCardRemoved;
		//���[�_�[�����O���ꂽ�Ǝv����ꍇ�̃C�x���g�ł�
		Definitions_SCReader::OnReaderGone* OnReaderGone;
	};

	//�ʒm�ݒ�
	using NotifyFunction = void(*)( mSCReader& sc , DWORD_PTR parameter , NotifyFunctionOptPtr* opt );
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		//�J�[�h��F�������ꍇ�̃C�x���g�ł�
		NotifierInfo OnCardPresent;
		//�J�[�h�����O���ꂽ�ꍇ�̃C�x���g�ł�
		NotifierInfo OnCardRemoved;
		//���[�_�[�����O���ꂽ�Ǝv����ꍇ�̃C�x���g�ł�
		NotifierInfo OnReaderGone;
	};

	//�X�}�[�g�J�[�h�T�[�r�X�̋N����ҋ@����
	// waittime : �~���b�P�ʂ̑ҋ@����
	//             INFINITE�ŋN���܂ŉi�v�ҋ@
	//             0�Ō��݂̏�Ԃ��m�F���Ē����ɖ߂�
	// ret : ���ԓ��ɃT�[�r�X���N��������^
	bool WaitForServiceAvailable( DWORD waittime = 0 );

	//�ڑ�����Ă���J�[�h���[�_�[�̈ꗗ���擾����
	// retReader : �ڑ�����Ă��郊�[�_�[�̈ꗗ
	// ret : �������^
	bool GetCardReaderList( WStringVector& retReader )const;

	//�J�[�h���[�_�[�̊Ď����J�n����
	// readername : �Ď�����J�[�h���[�_�[�̖��O
	// opt : �ʒm�I�v�V����
	// ret : �������^
	bool StartWatch( const WString& readername , const NotifyOption& opt );

	//�J�[�h���[�_�[�̊Ď����I������
	// ret : �������^
	bool EndWatch( void );

	//�Ď����̃J�[�h���[�_�[�̖��O���擾����
	const WString& GetCurrentCardReaderName( void )const;

private:
	mSCReader( const mSCReader& source );
	const mSCReader& operator=( const mSCReader& source ) = delete;

protected:

	//�X�}�[�g�J�[�h�T�u�V�X�e�����N�����Ă��邩�ǂ������`�F�b�N���邽�߂̃C�x���g�I�u�W�F�N�g
	HANDLE MyServiceAvailableSignal;

	//�X�}�[�g�J�[�h�T�u�V�X�e���̃n���h��
	mutable SCARDCONTEXT MySCardContext;

	//�Ď��X���b�h�̃n���h��
	mThread* MyThread;

	//�Ď����̃J�[�h���[�_�[�̖��O
	WString MyCardReaderName;

	//�R���e�L�X�g���J��
	bool OpenContext( void )const;


};


#ifdef SAMPLE_CODE

#include "mStandard.h"
#pragma comment (lib, "winscard.lib")
#pragma comment (lib, "scarddlg.lib")
#include <Crypto/mSCReader.h>


//�X�}�[�g�J�[�h���Z�b�g���ꂽ�Ƃ��̃R�[���o�b�N
void OnPresent( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	return;
}

//�X�}�[�g�J�[�h�����[�_�[������O���ꂽ�Ƃ��̃R�[���o�b�N
void OnRemove( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
	return;
}

//���[�_�[�����O���ꂽ�Ǝv����ꍇ�̃R�[���o�b�N
//���G���[��ԂȂ̂ŁA�Ăу��[�_�[�����t�����Ă����̃R�[���o�b�N�͂���܂���B�Ď����ċN������K�v������܂��B
void OnReaderGone( mSCReader& sc , DWORD_PTR parameter , mSCReader::NotifyFunctionOptPtr* opt )
{
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
	notifyopt.OnCardRemoved.Mode = mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
	notifyopt.OnCardRemoved.Notifier.CallbackFunction = OnRemove;
	notifyopt.OnReaderGone.Mode = mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
	notifyopt.OnReaderGone.Notifier.CallbackFunction = OnReaderGone;

	//�Ď��J�n
	reader.StartWatch( cardreader[ 0 ] , notifyopt );

	//�ҋ@�B����̓T���v���Ȃ̂ŒP�Ȃ�E�G�C�g�B
	while( 1 )
	{
		Sleep( 1000 );
	}

	//�N���[���A�b�v
	reader.EndWatch();
	return 0;
}
#endif

#endif

