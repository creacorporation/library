//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�E�C���h�E�`��p�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�E�C���h�E�`��p�̃f�o�C�X�R���e�L�X�g�ł��B
���̃N���X�ł́AGetDC()���Ăяo����Windows����f�o�C�X�R���e�L�X�g���擾���܂��B
GetDC�Ŏ擾�ł���̂�Windows���ŃO���[�o���ȃf�o�C�X�R���e�L�X�g�Ȃ̂ŁA
�`�悪�I������炳�����Ɣj�����Ȃ��Ƃ����Ȃ��炵���ł��B
�˂���āA���̃N���X�̃C���X�^���X���`�悪�I�������j�����Ă��������B

�悭�킩��񂯂ǁA
Get�`�Ŏn�܂�API�֐��Ŏ�ꂽ�f�o�C�X�R���e�L�X�g�˂������Ɣj������
Create�`�n�܂�API�֐��Ŏ�ꂽ�f�o�C�X�R���e�L�X�g�˃A�v���I���܂ŃO���[�o���ϐ��Ƃ��Ɏ����Ă��Ă�OK
���Ă��Ƃ炵���B
*/

#ifndef MGDIWINDC_H_INCLUDED
#define MGDIWINDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"

class mGdiWinDC : public mGdiDC
{
public:

	//�R���X�g���N�^
	//hwnd : �f�o�C�X�R���e�L�X�g���擾����̂Ɏg�p����E�C���h�E�̃n���h���B
	//       �w�肵���E�C���h�E�̃N���C�A���g�̈�̃f�o�C�X�R���e�L�X�g���擾���܂��B
	mGdiWinDC( HWND hwnd );

	virtual ~mGdiWinDC();

private:
	mGdiWinDC();
	mGdiWinDC( const mGdiWinDC& src ) = delete;
	mGdiWinDC& operator=( const mGdiWinDC& src ) = delete;

protected:

	HWND MyHwnd;	//MyHdc���擾����Ƃ��Ɏg�p�����E�C���h�E�̃n���h��
					//���R���X�g���N�^�Ŏw�肵������

};

#endif //MGDIWINDC_H_INCLUDED


