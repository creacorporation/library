//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�������f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIMEMDC_CPP_COMPILING
#include "mGdiMemDC.h"
#include "General/mErrorLogger.h"

mGdiMemDC::mGdiMemDC()
{
	MyHdc = ::CreateCompatibleDC( nullptr );
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"CreateCompatibleDC failed" );
	}
}

mGdiMemDC::mGdiMemDC( const mGdiDC& src )
{
	MyHdc = ::CreateCompatibleDC( src.MyHdc );
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"CreateCompatibleDC failed" );
	}
}

mGdiMemDC::mGdiMemDC( const mGdiDC* src )
{
	if( !src )
	{
		MyHdc = ::CreateCompatibleDC( nullptr );
	}
	else
	{
		MyHdc = ::CreateCompatibleDC( src->MyHdc );
	}

	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"CreateCompatibleDC failed" );
	}
}


mGdiMemDC::~mGdiMemDC()
{
	//�I������Ă���GDI�I�u�W�F�N�g��S�����ɖ߂�
	//����������Ȃ��ƃ��\�[�X���[�N���邩��
	ResetSelectedObject();

	//�n���h���̉�������ďI��
	::DeleteDC( MyHdc );

}

