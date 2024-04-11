//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�y���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIPEN_CPP_COMPILING
#include "mGdiPen.h"
#include "mGdiUtil.h"
#include "General/mErrorLogger.h"

mGdiPen::mGdiPen( const Option* option )throw( mException )
{
	//�I�v�V�������k���|�C���^�������ꍇ�A���y���ɂȂ�܂��B
	if( option == nullptr )
	{
		MyHandle = (HPEN)::GetStockObject( BLACK_PEN );
		return;
	}
	//�������@�ɏ]���ăy�����쐬���܂�
	if( option->method == Option::CreateMethod::USEOPTION )
	{
		//�I�v�V�����\���̂��g���ꍇ
		const Option_UseOption* opt = (const Option_UseOption*)option;
		if( !CreateHandle( *opt ) )
		{
			throw EXCEPTION( opt->kind , L"CreateHandle Failed" );
		}
	}
	else if( option->method == Option::CreateMethod::TRANSPARENT_PEN )
	{
		const Option_Transparent* opt = (const Option_Transparent*)option;
		if( !CreateHandle( *opt ) )
		{
			throw EXCEPTION( 0 , L"CreateHandle Failed" );
		}
	}
	else
	{
		//�������@���s��
		throw EXCEPTION( option->method , L"Unknown create method" );
	}
	return;
}

mGdiPen::~mGdiPen()
{
	::DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
mGdiPen::operator HPEN()const
{
	return MyHandle;
}

//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
HGDIOBJ mGdiPen::GetHandle( void )const
{
	return MyHandle;
}

//�y������
bool mGdiPen::CreateHandle( const Option_UseOption& opt )
{
	//�y���̎��
	INT pen_kind = PS_NULL;
	switch( opt.kind )
	{
	case Option::PenKind::SOLID_PEN:		//�����̃y��
		pen_kind = PS_SOLID;
		break;
	case Option::PenKind::DASH_PEN:			//�j���̃y��
		pen_kind = PS_DASH;
		break;
	case Option::PenKind::DOT_PEN:			//�_���̃y��
		pen_kind = PS_DOT;
		break;
	case Option::PenKind::DASHDOT_PEN:		//��_���y��
		pen_kind = PS_DASHDOT;
		break;
	case Option::PenKind::DASHDOTDOT_PEN:	//��_���y��
		pen_kind = PS_DASHDOTDOT;
		break;
	default:
		break;
	}
	//�F�̐ݒ�
	RGBQUAD rgb_color = ::COLORREF2RGBQUAD( opt.color );

	//�n���h���̍쐬
	MyHandle = ::CreatePen( pen_kind , opt.width , ::RGBQUAD2COLORREF( rgb_color ) );

	return MyHandle != nullptr;
}

bool mGdiPen::CreateHandle( const Option_Transparent& opt )
{
	MyHandle = ::CreatePen( PS_NULL , 1 , 0 );
	return MyHandle != nullptr;
}


