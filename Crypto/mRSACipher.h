//----------------------------------------------------------------------------
// RSA�Í����N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MRSACIPHER_H_INCLUDED
#define MRSACIPHER_H_INCLUDED

#include "../mStandard.h"
#include "MRSA.h"
#include <memory>
class mRSACipher : public mRSA
{
public:
	mRSACipher();
	virtual ~mRSACipher();

	//�Í����ς݃f�[�^�̊i�[��
	typedef std::unique_ptr<BYTE> EncryptData;

	//�Í�������
	//type : �Í����Ɏg����(���J�� or �閧��)
	//data : �Í�������f�[�^
	//       ��(RSAbit����8)-11 �o�C�g���f�[�^���̌��E�B
	//       2048bitRSA �̏ꍇ 245�o�C�g
	//       4096bitRSA �̏ꍇ 501�o�C�g
	//datalen : data�̒���
	//retEncrypted : �Í��������f�[�^�̊i�[��
	//retwritten : �Í����̌��ʁAretEncrypted�ɏ������܂ꂽ�T�C�Y�B
	bool Encrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retEncrypted , DWORD& retWritten )const;

	//��������
	//type : �����Ɏg����(���J�� or �閧��)
	//data : ��������f�[�^
	//datalen : data�̒���(1024bitRSA�Ȃ�128�o�C�g�A2048bitRSA�Ȃ�256�o�C�g�ɂȂ锤)
	//bufflen : �o�b�t�@�̃T�C�Y
	//retwritten : �����̌��ʁAdata�ɏ������܂ꂽ�T�C�Y�B
	bool Decrypt( KEYTYPE type , const BYTE* data , DWORD datalen , EncryptData& retDecrypted , DWORD& retWritten )const;

private:

	mRSACipher( const mRSACipher& source );
	const mRSACipher& operator=( const mRSACipher& source ) = delete;


};

#endif
