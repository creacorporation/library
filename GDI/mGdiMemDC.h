//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�������f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�ėp�̃f�o�C�X�R���e�L�X�g�ł��B
���̃N���X�ł́ACreateCompatibleDC()���Ăяo����Windows����f�o�C�X�R���e�L�X�g���擾���܂��B
��Ƀr�b�g�}�b�v��ێ����邽�߂̃N���X�ł��B
�v���C�x�[�g�f�o�C�X�R���e�L�X�g�ƂȂ邽�߁A���̃N���X���O���[�o���ϐ��ɕێ����Ă���肠��܂���B
*/

#ifndef MGDIMEMDC_H_INCLUDED
#define MGDIMEMDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"

class mGdiMemDC : public mGdiDC
{
public:

	//�R���X�g���N�^
	//�������Ȃ��ꍇ�́A�f�X�N�g�b�v�ƌ݊��̃f�o�C�X�R���e�L�X�g���쐬����܂��B
	mGdiMemDC();

	//�R���X�g���N�^
	//src : �R�R�Ŏw�肵�����̂ƌ݊��̃f�o�C�X�R���e�L�X�g���쐬����܂��B
	mGdiMemDC( const mGdiDC& src );

	//�R���X�g���N�^
	//src : �R�R�Ŏw�肵�����̂ƌ݊��̃f�o�C�X�R���e�L�X�g���쐬����܂��B
	//      nullptr���w�肷��ƃf�X�N�g�b�v���w�肵�����̂ƌ��Ȃ��܂�
	mGdiMemDC( const mGdiDC* src );

	virtual ~mGdiMemDC();

private:
	mGdiMemDC( const mGdiMemDC& src ) = delete;
	mGdiMemDC& operator=( const mGdiMemDC& src ) = delete;

protected:


};

#endif //MGDIMEMDC_H_INCLUDED

