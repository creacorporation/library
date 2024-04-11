//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�E�C���h�E�`��p�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIWINDC_CPP_COMPILING
#include "mGdiWinDC.h"
#include "General/mErrorLogger.h"

mGdiWinDC::mGdiWinDC( HWND hwnd )
{
	MyHdc = ::GetDC( hwnd );
	if( MyHdc != nullptr )
	{
		MyHwnd = hwnd;
	}
	else
	{
		MyHwnd = nullptr;
		RaiseAssert( g_ErrorLogger , 0 , L"GetDC failed" );
	}
}

mGdiWinDC::~mGdiWinDC()
{
	//�I������Ă���GDI�I�u�W�F�N�g��S�����ɖ߂�
	//����������Ȃ��ƃ��\�[�X���[�N���邩��
	ResetSelectedObject();

	//�n���h���̉�������ďI��
	::ReleaseDC( MyHwnd , MyHdc );
}


