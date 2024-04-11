//----------------------------------------------------------------------------
// AES�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09�`
//----------------------------------------------------------------------------


#ifndef MAES_H_INCLUDED
#define MAES_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>
#include "../General/mTCHAR.h"

//AES��p�Í����N���X
//�E�ȒP�d�l
//�EAES256bit��p
//�E����256�r�b�g�̃o�C�i�����p�X���[�h�e�L�X�g
//�E�p�X���[�h�e�L�X�g�œ��͂����ꍇ�́ASHA256(�p�X���[�h + salt )�œ�����o�C�i�������Ƃ���B

//������
//PKCS#5�p�f�B���O�Ƃ́H
//�p�f�B���O����o�C�g���̒l�Ńp�f�B���O����B
//16�o�C�g�u���b�N��12�o�C�g�g���Ă���Ȃ�A4�o�C�g�p�f�B���O����̂ŁA
//����4�o�C�g���u0x04�v�Ńp�f�B���O����B
//�u���b�N���҂�����Ȃ�A����1�u���b�N���ۂ��ƃp�f�B���O����B

class mAES
{
public:
	mAES();
	virtual ~mAES();

	//������
	//key : �Í����L�[(�R�Q�o�C�g�̃o�C�i��)
	//ret : �������^
	bool Init( const BYTE* key );

	//������
	// key : �Í����L�[(������)
	// salt : �Í����L�[�Ɋ܂߂�\���g
	// saltsize : �\���g�̃o�C�g��
	//ret : �������^
	bool Init( const SecureAString& key , const BYTE* salt = nullptr , DWORD saltsize = 0 );

	//�Í����L�[��������A������Ԃɖ߂��܂�
	void Deinit( void );

	//IV���Z�b�g����
	// iv ��16�o�C�g�̃o�C�i��
	//ret : �������^
	bool SetIV( const BYTE* iv );

	//�Í����L�[���Z�b�g����Ă��邩��Ԃ�
	//ret : �Z�b�g����Ă���Ƃ��^
	bool IsInitialized( void )const;

private:
	mAES( const mAES& source );
	const mAES& operator=( const mAES& source ) = delete;

protected:

	HCRYPTPROV	MyCryptProv;	//�Í����v���o�C�_�̃n���h��
	HCRYPTKEY	MyCryptKey;		//�Í����L�[�̃n���h��

	//�v���o�C�_�̏�����
	//ret : �������^
	bool InitProvider( void );

	//�������֐��i�����p�j
	// hash : �L�[���n�b�V�������I�u�W�F�N�g
	// ret : �������^
	bool Init( HCRYPTHASH hash );

};

#endif