//----------------------------------------------------------------------------
// AES�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09�`
//----------------------------------------------------------------------------

#ifndef MAESENCRYPTER_H_INCLUDED
#define MAESENCRYPTER_H_INCLUDED

#include "../mStandard.h"
#include "mAES.h"
#include <memory>

class mAESEncrypter : public mAES
{
public:
	mAESEncrypter();
	virtual ~mAESEncrypter();

	//bufflen�o�C�g�ȉ��̃����_���ȃT�C�Y�̕������t������
	//��������镶����̍Ō�̕�����\0�ł���A������O��\0�͊܂܂Ȃ��B
	// data : �����_���ȃo�C�g����������ރ|�C���^
	// bufflen : data�̃T�C�Y(32�ȏ�𐄏�)
	// retwritten : ���ۂɏ������܂ꂽ�o�C�g��
	bool CreateRandomHeader( BYTE* data , DWORD bufflen , DWORD& retwritten );

	//�Í����f�[�^�̊i�[��
	typedef std::unique_ptr<BYTE> EncryptData;

	//�Í������܂�
	//��isfinal=true�̏ꍇ�A�o�͂̃T�C�Y�����͂�葝���邱�Ƃ�����܂��B
	// isfinal : �Ō�̃f�[�^�ł���ꍇ�Ɏw�肵�܂�(true�̏ꍇ�A�p�f�B���O�̏������s���܂�)
	// data : �Í����������f�[�^�ւ̃|�C���^
	// datalen : data�̃T�C�Y(�����̃f�[�^�T�C�Y)
	// retEncryptData : �Í����f�[�^�̏������ݐ�
	// retwritten : �Í�����̃T�C�Y�B
	bool Encrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retEncryptData , DWORD& retWritten )const;

private:
	mAESEncrypter( const mAESEncrypter& source );
	const mAESEncrypter& operator=( const mAESEncrypter& source ) = delete;


};

#endif