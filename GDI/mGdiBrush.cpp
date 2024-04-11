//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�u���V�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIBRUSH_CPP_COMPILING
#include "mGdiBrush.h"
#include "mGdiUtil.h"
#include "General/mErrorLogger.h"

mGdiBrush::mGdiBrush( const Option* opt )throw( mException )
{
	//�I�v�V�������k���|�C���^�������ꍇ�A���̃\���b�h�u���V�ɂȂ�܂��B
	if( opt == nullptr )
	{
		MyHandle = (HBRUSH)::GetStockObject( WHITE_BRUSH );
		return;
	}

	//�I�v�V�����Ɏw�肳�ꂽ�悤�ȃu���V�𐶐����܂��B
	if( opt->kind == Option::BrushKind::SOLID_BRUSH )
	{
		//�\���b�h�u���V�̐��������܂��B
		const Option_Solid* opt_solid = (const Option_Solid*)opt;
		if( !CreateSolidBrush( *opt_solid ) )
		{
			throw EXCEPTION( opt_solid->color , L"CreateSolidBrush failed" );
		}
		return;
	}
	else if( opt->kind == Option::BrushKind::HATCH_BRUSH )
	{
		//�n�b�`�u���V�̍쐬�����܂�
		const Option_Hatch* opt_hatch = (const Option_Hatch*)opt;
		if( !this->CreateHatchBrush( *opt_hatch ) )	//��WinAPI�ɓ����̊֐�������̂�this�C�����Ă��܂�
		{
			throw EXCEPTION( opt_hatch->style , L"CreateHatchBrush failed" );
		}
		return;
	}
	else if( opt->kind == Option::BrushKind::PATTERN_BRUSH )
	{
		//�p�^�[���u���V�̍쐬�����܂�
		const Option_Pattern* opt_pat = (const Option_Pattern*)opt;
		if( !this->CreatePatternBrush( *opt_pat ) )	//��WinAPI�ɓ����̊֐�������̂�this�C�����Ă��܂�
		{
			throw EXCEPTION( 0 , L"CreatePatternBrush failed" );
		}
		return;
	}
	else if( opt->kind == Option::BrushKind::TRANSPARENT_BRUSH )
	{
		MyHandle = (HBRUSH)::GetStockObject( NULL_BRUSH );
		return;
	}

	//�Ή����Ă��Ȃ���ʂȂ̂ŗ�O�𓊂��܂��B
	MyHandle = nullptr;
	throw EXCEPTION( opt->kind , L"Invalid brush kind" );
}

mGdiBrush::~mGdiBrush()
{
	//�n���h���̉��
	//��GetStockObject�ō�����n���h����������Ă��܂��Ė��͖����B
	DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//�\���b�h�u���V���쐬���܂��B���������MyHandle�ɒl������܂��B
bool mGdiBrush::CreateSolidBrush( const Option_Solid& opt )
{
	//�n���h���̍쐬
	MyHandle = ::CreateSolidBrush( opt.color );
	return ( MyHandle != nullptr );
}

//�n�b�`�u���V���쐬���܂��B���������MyHandle�ɒl������܂��B
bool mGdiBrush::CreateHatchBrush( const Option_Hatch& opt )
{

	INT hatch_type;
	switch( opt.style )
	{
	case Option_Hatch::HatchStyle::UP_DIAGONAL:		//�E�オ��̎ΐ�
		hatch_type = HS_FDIAGONAL;
		break;
	case Option_Hatch::HatchStyle::DOWN_DIAGONAL:	//�E������̎ΐ�
		hatch_type = HS_BDIAGONAL;
		break;
	case Option_Hatch::HatchStyle::CROSS_DIAGONAL:	//�ΐ��̃N���X�n�b�`
		hatch_type = HS_DIAGCROSS;
		break;
	case Option_Hatch::HatchStyle::HORIZONTAL:		//�������̃n�b�`
		hatch_type = HS_HORIZONTAL;
		break;
	case Option_Hatch::HatchStyle::VERTICAL:		//�������̃n�b�`
		hatch_type = HS_VERTICAL;
		break;
	case Option_Hatch::HatchStyle::CROSS:			//�\���̃n�b�`
		hatch_type = HS_CROSS;
		break;
	default:
		//�ςȂ̂������ꍇ�̓G���[�I������
		return false;
	}

	//�F�̐ݒ�
	RGBQUAD rgb_color = ::COLORREF2RGBQUAD( opt.color );

	//�n���h���̍쐬�B���s����nullptr�Ȃ̂ŁAnullptr�Ƃ̔�r���ʂ�Ԃ��܂��B
	MyHandle = ::CreateHatchBrush( hatch_type , ::RGBQUAD2COLORREF( rgb_color ) );
	return ( MyHandle != nullptr );
}

//�p�^�[���u���V���쐬���܂��B���������MyHandle�ɒl������܂��B
bool mGdiBrush::CreatePatternBrush( const Option_Pattern& opt )
{
	//TODO:�r�b�g�}�b�v�̎������܂��ł��Ă��Ȃ�����A��ō��B
	return false;
}

//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
mGdiBrush::operator HBRUSH()const
{
	return MyHandle;
}

//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
HGDIOBJ mGdiBrush::GetHandle( void )const
{
	return MyHandle;
}

