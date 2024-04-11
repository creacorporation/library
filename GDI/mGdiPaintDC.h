//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�ĕ`��p�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�ĕ`��p�̃f�o�C�X�R���e�L�X�g�ł��B
���̃N���X�ł́ABeginPaint()���Ăяo����Windows����f�o�C�X�R���e�L�X�g���擾���܂��B
�K�X�ĕ`����s���A�S���I������C���X�^���X��j������ƁA�f�X�g���N�^��EndPaint���Ăяo���܂��B
*/

#ifndef MGDIPAINTDC_H_INCLUDED
#define MGDIPAINTDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"

class mGdiPaintDC : public mGdiDC
{
public:

	//�R���X�g���N�^
	//hwnd : �f�o�C�X�R���e�L�X�g���擾����̂Ɏg�p����E�C���h�E�̃n���h���B
	//       �w�肵���E�C���h�E�̃N���C�A���g�̈�̃f�o�C�X�R���e�L�X�g���擾���܂��B
	mGdiPaintDC( HWND hwnd );

	virtual ~mGdiPaintDC();

	//�ĕ`��̈���擾����
	//retArea : �ĕ`�悪�K�v�ȗ̈�
	//ret : ������true
	bool GetRedrawArea( RECT& retArea )const;

private:
	mGdiPaintDC();
	mGdiPaintDC( const mGdiPaintDC& src ) = delete;
	mGdiPaintDC& operator=( const mGdiPaintDC& src ) = delete;

protected:

	//MyHdc���擾����Ƃ��Ɏg�p�����E�C���h�E�̃n���h��
	//���R���X�g���N�^�Ŏw�肵������
	HWND MyHwnd;	

	//�ĕ`��͈�
	PAINTSTRUCT MyPaintStruct;

};

#endif //MGDIPAINTDC_H_INCLUDED

