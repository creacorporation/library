//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�t�H���g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIFONT_CPP_COMPILING
#include "mGdiFont.h"
#include "General/mErrorLogger.h"

/*
LONG  lfHeight;                  // �����Z���܂��͕����̍���
LONG  lfWidth;                   // ���ϕ�����
LONG  lfEscapement;              // ��������̕�����X���Ƃ̊p�x
LONG  lfOrientation;             // �x�[�X���C����X���Ƃ̊p�x
LONG  lfWeight;                  // �t�H���g�̑���
BYTE  lfItalic;                  // �C�^���b�N�̎w��
BYTE  lfUnderline;               // �����t���w��
BYTE  lfStrikeOut;               // �ł��������t���w��
BYTE  lfCharSet;                 // �L�����N�^�Z�b�g
BYTE  lfOutPrecision;            // �o�͐��x
BYTE  lfClipPrecision;           // �N���b�s���O�̐��x
BYTE  lfQuality;                 // �o�͕i��
BYTE  lfPitchAndFamily;          // �s�b�`�ƃt�@�~��
TCHAR lfFaceName[LF_FACESIZE];   // �t�H���g��  
*/

mGdiFont::mGdiFont( const Option* opt )throw( mException )
{
	MyHandle = nullptr;
	if( opt != nullptr )
	{
		//�I�v�V�������w�肳��Ă����ꍇ�A������g���ăt�H���g�𐶐�����
		if( opt->method == Option::CreateMethod::USEOPTION )
		{
			MakeFont( (Option_UseOption*)opt );
		}
		else
		{
			throw EXCEPTION( opt->method , L"Unknown create method" );
		}
	}
	else
	{
		//�I�v�V�������w�肳��Ă��Ȃ������ꍇ(nullptr�������ꍇ)
		//�_�~�[�̃I�v�V�����\����(�S�f�t�H���g�ŏ����������)��n��
		Option_UseOption temp_opt;
		MakeFont( &temp_opt );
	}

	//�n���h������������Ă����OK�A�����łȂ���Η�O�𓊂���
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"CreateFontW failed" );
	}
	return;
}

mGdiFont::~mGdiFont()
{
	::DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
mGdiFont::operator HFONT()const
{
	return MyHandle;
}

//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
HGDIOBJ mGdiFont::GetHandle( void )const
{
	return MyHandle;
}

bool mGdiFont::MakeFont( const Option_UseOption* opt )
{
	//MyHandle�ɂ��łɒl������ꍇ�̓G���[
	if( MyHandle != nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Handle is already allocated" );
		return false;
	}

	//�ȉ��A�I�v�V�������w�肳��Ă���Ƃ��̏���
	LOGFONT font;
	font.lfHeight = opt->height;	//�t�H���g�̍���
	font.lfWidth = opt->width;		//�t�H���g�̕�
	font.lfEscapement = opt->orientation;	//��������̉�]����
	font.lfOrientation = opt->orientation;					//�����̉�]���@
	font.lfWeight = ( opt->isbold ) ? ( FW_BOLD ) : ( FW_DONTCARE );	//�{�[���h�ɂ���H
	font.lfItalic = ( opt->isitalic ) ? ( TRUE ) : ( FALSE );			//�C�^���b�N�ɂ���H
	font.lfUnderline = ( opt->isunderline ) ? ( TRUE ) : ( FALSE );		//�A���_�[���C���~�����H
	font.lfStrikeOut = ( opt->isstrikeout ) ? ( TRUE ) : ( FALSE );		//�ł���������t����H
	font.lfOutPrecision = OUT_DEFAULT_PRECIS;	//�o�͐��x
	font.lfClipPrecision = CLIP_DEFAULT_PRECIS;	//�N���b�s���O���x
	font.lfPitchAndFamily = FF_DONTCARE;		//�t�H���g�t�@�~��

	//�L�����N�^�Z�b�g�̎w��
	switch( opt->charset )
	{
	case Option::FontCharSet::ANSI:			//�p��
		font.lfCharSet = ANSI_CHARSET;
		break;
	case Option::FontCharSet::BALTIC:		//�o���g�O���̕���(���g�A�j�A��A���g�r�A�ꓙ)
		font.lfCharSet = BALTIC_CHARSET;
		break;
	case Option::FontCharSet::GB2312:		//�ȑ̎�������(�����A�V���K�|�[��)
		font.lfCharSet = GB2312_CHARSET;
		break;
	case Option::FontCharSet::CHINESEBIG5:	//�ɑ̎�������(���ؖ����A���`�A�}�J�I)
		font.lfCharSet = CHINESEBIG5_CHARSET;
		break;
	case Option::FontCharSet::EASTEUROPE:	//�����n����̕���
		font.lfCharSet = EASTEUROPE_CHARSET;
		break;
	case Option::FontCharSet::GREEK:		//�M���V����
		font.lfCharSet = GREEK_CHARSET;
		break;
	case Option::FontCharSet::HANGUL:		//�؍���
		font.lfCharSet = HANGUL_CHARSET;
		break;
	case Option::FontCharSet::SHIFTJIS:		//���{��
		font.lfCharSet = SHIFTJIS_CHARSET;
		break;
	case Option::FontCharSet::RUSSIAN:		//���V�A��
		font.lfCharSet = RUSSIAN_CHARSET;
		break;
	case Option::FontCharSet::TURKISH:		//�g���R��
		font.lfCharSet = TURKISH_CHARSET;
		break;
	case Option::FontCharSet::HEBREW:		//�w�u���C��
		font.lfCharSet = HEBREW_CHARSET;
		break;
	case Option::FontCharSet::ARABIC:		//�A���r�A��
		font.lfCharSet = ARABIC_CHARSET;
		break;
	case Option::FontCharSet::THAI:			//�^�C��
		font.lfCharSet = THAI_CHARSET;
		break;
	case Option::FontCharSet::LOCALE:		//���݂̃��P�[��(����ݒ�)�Ɋ�Â��Č��߂�
		font.lfCharSet = DEFAULT_CHARSET;
		break;
	case Option::FontCharSet::OEM:			//����ŃE�C���h�E�Y���Ō��߂�
		font.lfCharSet = OEM_CHARSET;
		break;
	default:
		//�͈͊O�̒l���w�肳��Ă����ꍇ�́A�d������������f�t�H���g�ɂ��Ă���
		RaiseAssert( g_ErrorLogger , opt->charset , L"Bad charset id" );
		font.lfCharSet = DEFAULT_CHARSET;
		break;
	}
	//�o�͕i��
	switch( opt->quality )
	{
	case Option::PrintQuality::NORMAL:
		font.lfQuality = DEFAULT_QUALITY;
		break;
	case Option::PrintQuality::ANTIALIAESED:
		font.lfQuality = ANTIALIASED_QUALITY;
		break;
	case Option::PrintQuality::CLEARTYPE:
		font.lfQuality = CLEARTYPE_QUALITY;
		break;
	default:
		RaiseAssert( g_ErrorLogger , opt->quality , L"Bad font quality id" );
		font.lfQuality = DEFAULT_QUALITY;
		break;
	}
	//�t�H���g�s�b�`
	switch( opt->pitch )
	{
	case Option::PrintPitch::DEFAULT:	//�t�H���g�C���ɂ���
		font.lfPitchAndFamily |= DEFAULT_PITCH;
		break;
	case Option::PrintPitch::FIXED:		//�Œ蕝�ɂ���
		font.lfPitchAndFamily |= FIXED_PITCH;
		break;
	case Option::PrintPitch::VARIABLE:	//�ϕ��ɂ���
		font.lfPitchAndFamily |= VARIABLE_PITCH;
		break;
	default:
		RaiseAssert( g_ErrorLogger , opt->pitch , L"Bad font pitch id" );
		font.lfPitchAndFamily |= DEFAULT_PITCH;
		break;
	}
	//�t�H���g��
	if( LF_FACESIZE <= opt->name.size() )
	{
		//�o�b�t�@�ɓ��肫��Ȃ��ꍇ�͋󕶎���ɂ���B
		//�o�b�t�@�ɂ͏I�[�̃k���������܂߂Ȃ���΂Ȃ�Ȃ��̂ɒ���
		RaiseAssert( g_ErrorLogger , opt->name.size() , L"�t�H���g�����������܂�" , opt->name );
		font.lfFaceName[ 0 ] = L'\0';
	}
	else
	{
		//�o�b�t�@�ɓ��肫��ꍇ�͕��ʂɏ�������
		wchar_sprintf( font.lfFaceName , L"%s" , opt->name.c_str() );
	}

	//�n���h���𐶐�����
	MyHandle = ::CreateFontIndirectW( &font );
	if( MyHandle == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�t�H���g�̐��������s���܂���" );
		return false;
	}
	return true;
}

