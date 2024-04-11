//----------------------------------------------------------------------------
// RSA�Í����N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MRSAVERIFIER_H_INCLUDED
#define MRSAVERIFIER_H_INCLUDED

#include "../mStandard.h"
#include "mRSA.h"
#include "mHash.h"
#include <memory>

class mRSAVerifier : public mRSA
{
public:
	mRSAVerifier();
	virtual ~mRSAVerifier();

	//�����������ݗp�o�b�t�@
	typedef std::unique_ptr<BYTE> SignBuffer;

	//�������s��
	//�K�v�Ȍ��F�閧��
	//data : �������s���n�b�V���l
	//retSign : �������i�[����o�b�t�@
	//retWritten : �������܂ꂽ�o�C�g��
	//ret : �������^
	bool Sign( const mHash& data , SignBuffer& retSign , DWORD& retWritten )const;

	//���؂��s��
	//�������ɏ�������Ă��邩�𔻒肵�܂�
	//�K�v�Ȍ��F���J��
	//data : ��������Ă���f�[�^�̃n�b�V��
	//Sign : ����
	//SignLen : �����̒���
	//ret : ���ؐ������^
	bool Verify( const mHash& data , const BYTE* Sign , DWORD SignLen )const;

private:
	mRSAVerifier( const mRSAVerifier& source );
	const mRSAVerifier& operator=( const mRSAVerifier& source ) = delete;

protected:

	//�n���ꂽmHash�I�u�W�F�N�g�Ɠ���n�b�V���l�����n���h�������
	//�쐬�����n���h���̃v���o�C�_�́A���̃C���X�^���X�̃v���o�C�_���g��
	//data : �R�s�[���̃I�u�W�F�N�g
	//retHash : �R�s�[�̊i�[��
	//ret : �������^
	bool DupeHCRYPTHASH( const mHash& data , HCRYPTHASH& retHash )const;
};

#ifdef SAMPLE_CODE
//�g����
int main( int argc , char** argv )
{
	InitializeLibrary();

	//��������
	//�E�f�[�^�̃n�b�V���l�����߂�
	BYTE Data1[] = "Secret Data";
	mHash hash;
	hash.Init( mHash::HashAlgorithm::SHA256 );
	hash.Hash( Data1 , sizeof( Data1 ) );

	//�E�����p�̃I�u�W�F�N�g�����A�V�������y�A������
	//�E�L�[�R���e�i�̖��O���w�肵�Ȃ��ꍇ�́A�����Ɏg���Ȃ�
	mRSAVerifier sign;
	sign.Init( L"Test Program" , false );
	sign.GenerateNewKey( mRSA::KEYLENGTH::KEYLEN_2048BIT );

	//�E���J�����G�N�X�|�[�g�i�������؎��Ɏg���j
	DWORD pubkey_size;
	mRSA::KeyBinary pubkey;
	sign.ExportKey( mRSA::KEYTYPE::KEY_PUBLIC , pubkey , pubkey_size );

	//�E��������
	mRSAVerifier::SignBuffer signbuffer;
	DWORD signlen;
	sign.Sign( hash , signbuffer , signlen );

	//�����؁�
	//�E�f�[�^�̃n�b�V���l�����߂�
	hash.Init( mHash::HashAlgorithm::SHA256 );
	hash.Hash( Data1 , sizeof( Data1 ) );

	//�E���؎��̓L�[�R���e�i�����K�v�͂Ȃ�
	//�E���ؑ��ɂ͔閧���͂Ȃ��O��Ȃ̂ŁA�����ł͌��J���̂݃C���|�[�g����
	//�EVerify()�Ō��؂���
	mRSAVerifier verify;
	verify.Init();
	verify.ImportKey( pubkey.get() , pubkey_size );
	verify.Verify( hash , signbuffer.get() , signlen );

	return 0;
}
#endif

#endif