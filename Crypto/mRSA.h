//----------------------------------------------------------------------------
// RSA�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MRSA_H_INCLUDED
#define MRSA_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>
#include <memory>
#include "../General/mTCHAR.h"

//RSA���Ǘ��N���X
//���ł��邱�Ɓ�
//�E���y�A�̊Ǘ������邱�Ƃ��ł��܂�
//�E�L�[�R���e�i���g�킸�A�e���|�����ȃJ�M����邱�Ƃ��ł��܂�(���̏ꍇ�d�q�����͕s��)
//�E�V����������邱�Ƃ��ł��܂�
//�E�쐬���������G�N�X�|�[�g�ł��܂�
//�E�����C���|�[�g�ł��܂�
//�����Ӂ�
//�E�Í������s���ɂ́A�h���N���X��mRSACipher���g���܂�
//�E�d�q�������s���ɂ́A�h���N���X��mRSAVerifier���g���܂�

//�Q�l�ɂȂ邩������Ȃ�URL
// http://eternalwindows.jp/crypto/csp/csp00.html
// http://msdn.microsoft.com/en-us/library/aa380252(v=vs.85).aspx

class mRSA
{
public:
	mRSA();
	virtual ~mRSA();

	//���̎��(���J��/�閧��)
	enum KEYTYPE
	{
		KEY_PUBLIC,			//���J��
		KEY_PRIVATE,		//�閧��
	};

	//���̃r�b�g��
	enum KEYLENGTH
	{
		KEYLEN_2048BIT,		//2048bit
		KEYLEN_4096BIT,		//4096bit
	};

	//������
	//���̏��������\�b�h���g�p�����ꍇ�A
	//�E���͎����ŃG�N�X�|�[�g���Ȃ�����ۑ�����Ȃ�
	//�E�����ɂ͎g���Ȃ�
	//len : ���̃r�b�g��
	bool Init( void );

	//������
	//���̏��������\�b�h���g�p�����ꍇ�A
	//�E�w�肵�����O�̃L�[�R���e�i���쐬�����
	//�E���łɓ����̃L�[�R���e�i�����݂���ꍇ�A���̃L�[�R���e�i�̃L�[�����[�h����
	//len : ���̃r�b�g��
	//container_name : �L�[�R���e�i�̖��O
	//machine_keyset : �^�̏ꍇ�A�L�[�R���e�i���R���s���[�^�Ɋ֘A�t����i�T�[�r�X�v���O�����p�j
	//				   �U�̏ꍇ�A�L�[�R���e�i�����[�U�Ɋ֘A�t����i��ʃA�v���p�j
	bool Init( const WString& container_name , bool machine_keyset );

	//�L�[�R���e�i��j������
	//container_name : �폜�������L�[�R���e�i�̖��O
	//                 �����w�肵�Ȃ��ꍇ�A�f�t�H���g�̃L�[�R���e�i���w�肵�����ƂɂȂ�
	//machine_keyset : �^�̏ꍇ�A�L�[�R���e�i���R���s���[�^�Ɋ֘A�t����i�T�[�r�X�v���O�����p�j
	//				   �U�̏ꍇ�A�L�[�R���e�i�����[�U�Ɋ֘A�t����i��ʃA�v���p�j
	//���w�肵���L�[�R���e�i�Ɋ܂܂��L�[�͂��ׂč폜�����̂Œ���
	static bool DestroyKeyContainer( const WString& container_name , bool machine_keyset );

	//�V�������y�A�����
	//���݂̌��͔j������
	//���n���h����j�����邾���ŁA�R���e�i��j������킯�ł͂Ȃ�
	bool GenerateNewKey( KEYLENGTH len );

	//����j������
	//���n���h����j�����邾���ŁA�R���e�i��j������킯�ł͂Ȃ�
	bool Clear( void );

	//���o�C�i���̃T�C�Y�𓾂�
	//RSA�̃r�b�g���ł͂Ȃ��AGetKeyBinary�Ō��ʂ̊i�[�ɕK�v�ȃo�b�t�@�̃T�C�Y���Ԃ�B
	//type : �擾��������(���J�� or �閧��)
	DWORD GetKeySize( KEYTYPE type );

	//���o�C�i��
	typedef std::unique_ptr<BYTE> KeyBinary;

	//���o�C�i���𓾂�
	//���ʂ�PRIVATEKEYBLOB�܂��́APUBLICKEYBLOB�t�H�[�}�b�g�̃o�C�i���B
	//type : �擾��������(���J�� or �閧��)
	//buffer : �i�[��̃o�b�t�@
	//len : �o�b�t�@�̃T�C�Y
	//retWritten : �������܂ꂽ�o�C�g��
	bool ExportKey( KEYTYPE type , KeyBinary& retKey , DWORD& retWritten );

	//�����o�C�i������C���|�[�g����
	//data : ���BPRIVATEKEYBLOB�܂��́APUBLICKEYBLOB�t�H�[�}�b�g�̃o�C�i���B
	//datalen : data�̃o�C�g��
	//�������^
	bool ImportKey( const BYTE* data , DWORD datalen );

private:
	mRSA( const mRSA& source );
	const mRSA& operator=( const mRSA& source ) = delete;

protected:

	//�Í����v���o�C�_�̃n���h��
	HCRYPTPROV	MyCryptProv;		//
	HCRYPTKEY	MyCryptKey;			//���J���{�閧��
	HCRYPTKEY	MyCryptKeyPub;		//���J���̂�

	//���o�C�i���𓾂�
	bool GetKey( KEYTYPE type , BYTE* buffer , DWORD& size );

	//MyCryptKey������J�������𒊏o����MyCryptKeyPub�ɃZ�b�g����
	bool ExtractPublicKey( void );

	//���o�C�i���̃C���|�[�g
	bool ImportKeyInternal( HCRYPTKEY* key , DWORD len , const BYTE* buffer );

};

#ifdef SAMPLE_CODE

int main( int argc , char** argv )
{
	InitializeLibrary();

	//�E�V�������y�A������
	//���L�[�R���e�i�̖��O���w�肵�Ȃ��ꍇ�́A�����Ɏg���Ȃ�
	mRSA key;
	key.Init( L"Test Program" , false );
	key.GenerateNewKey( mRSA::KEYLENGTH::KEYLEN_2048BIT );

	//�E���J�����G�N�X�|�[�g
	DWORD pubkey_size;
	mRSA::KeyBinary pubkey;
	key.ExportKey( mRSA::KEYTYPE::KEY_PUBLIC , pubkey , pubkey_size );

	//�E�閧�����G�N�X�|�[�g
	DWORD privkey_size;
	mRSA::KeyBinary privkey;
	key.ExportKey( mRSA::KEYTYPE::KEY_PRIVATE , privkey , privkey_size );

	//�G�N�X�|�[�g�������J����ʂ̃I�u�W�F�N�g�ɃC���|�[�g����
	mRSA other_key;
	other_key.Init();
	other_key.ImportKey( pubkey.get() , pubkey_size );

	return 0;
}

#endif

#endif