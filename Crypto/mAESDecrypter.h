//----------------------------------------------------------------------------
// AES�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09�`
//----------------------------------------------------------------------------


#ifndef MAESDECRYPTER_H_INCLUDED
#define MAESDECRYPTER_H_INCLUDED

#include "../mStandard.h"
#include "mAES.h"
#include <memory>

//�Í��������N���X
class mAESDecrypter : public mAES
{
public:
	mAESDecrypter();
	virtual ~mAESDecrypter();

	//�Í����f�[�^�̊i�[��
	typedef std::unique_ptr<BYTE> EncryptData;

	//�Í������������܂�
	// isfinal : �Ō�̃f�[�^�ł���ꍇ�Ɏw�肵�܂�(true�̏ꍇ�A�p�f�B���O�̏������s���܂�)
	// data : �Í�������Ă���f�[�^�ւ̃|�C���^�B�Í������������f�[�^�ŏ㏑������܂��B
	// datalen : data�̃T�C�Y
	// retData : ������̃f�[�^�i�[��
	// retWritten : �Í���������̃T�C�Y�Bisfinal=true�̏ꍇ�A���͂Əo�͂̃T�C�Y���قȂ邱�Ƃ�����܂��B
	//              �T�C�Y���قȂ�ꍇ�́A�K���Í�����蕽���̂ق����������Ȃ�܂��B
	bool Decrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retData , DWORD& retWritten )const;

private:
	mAESDecrypter( const mAESDecrypter& source );
	const mAESDecrypter& operator=( const mAESDecrypter& source ) = delete;

};

#endif