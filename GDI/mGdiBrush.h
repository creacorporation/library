//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�u���V�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI�̃u���V�ł��B
�E�쐬����u���V�́AOption�\���̂Ŏw�肵�܂��B
�E�t�@�N�g�����\�b�h��opt��nullptr��n���ƁA���̃\���b�h�u���V�ɂȂ�܂��B
*/

#ifndef MGDIBRUSH_H_INCLUDED
#define MGDIBRUSH_H_INCLUDED

#include "mStandard.h"
#include "GDI/mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

class mGdiBrush : public mGdiHandle
{
public:
	//�u���V�����̃I�v�V����
	//�u���V���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_Solid �c �x�^�h��u���V���쐬�������Ƃ�
	//�EOption_Hatch �c �n�b�`�u���V���쐬�������Ƃ�
	//�EOption_Pattern �c �p�^�[���u���V���쐬�������Ƃ�(������)
	//�EOption_Transparent �c �����u���V(NULL�u���V)���쐬�������Ƃ�
	struct Option
	{
		//�u���V�̎��
		enum BrushKind
		{
			TRANSPARENT_BRUSH,	//�����ȃu���V
			SOLID_BRUSH,		//�x�^�h��u���V
			HATCH_BRUSH,		//�n�b�`�u���V(�`�F�b�N���Ƃ����p)
			PATTERN_BRUSH,		//�r�b�g�}�b�v���g���ĕ������u���V
		};

		const BrushKind kind;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( BrushKind brush_kind ) : kind( brush_kind )
		{
		}
	};

	//�x�^�h��u���V�p�ݒ�\����
	struct Option_Solid : public Option
	{
		COLORREF color;			//�x�^�h��Ɏg���F
		Option_Solid() : Option( BrushKind::SOLID_BRUSH )
		{
			//�f�t�H���g�͔��x�^�u���V
			color = RGB( 0xFFu , 0xFFu , 0xFFu );
		}
	};

	//�n�b�`�u���V�p�ݒ�\����
	struct Option_Hatch : public Option
	{
		COLORREF color;		//�n�b�`�u���V�̕`��F
		enum HatchStyle
		{
			UP_DIAGONAL,		//�E�オ��̎ΐ�     �^�^�^�^�͗l
			DOWN_DIAGONAL,		//�E������̎ΐ�	 �_�_�_�_�͗l
			CROSS_DIAGONAL,		//�ΐ��̃N���X�n�b�` �~�~�~�~�͗l
			HORIZONTAL,			//�������̃n�b�`     �\�\�\�\�͗l
			VERTICAL,			//�������̃n�b�`     �b�b�b�b�͗l
			CROSS,				//�\���̃n�b�`       �{�{�{�{�͗l
		}style;

		Option_Hatch() : Option( BrushKind::HATCH_BRUSH )
		{
			color = RGB( 0 , 0 , 0 );
			style = CROSS_DIAGONAL;
		}
	};

	//�p�^�[���u���V�p�ݒ�\����
	struct Option_Pattern : public Option
	{
		Option_Pattern() : Option( BrushKind::PATTERN_BRUSH )
		{
		}
	};

	//�����ȃu���V�p�ݒ�\����
	struct Option_Transparent : public Option
	{
		Option_Transparent() : Option( BrushKind::TRANSPARENT_BRUSH )
		{
		}
	};

	//�t�@�N�g�����\�b�h
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			//�u���V���R���X�g���N�^�Ő������邯�ǁ[
			result = mNew mGdiBrush( (const Option*)opt );
		}
		catch( mException )
		{
			//��O�����������ꍇ��nullptr��Ԃ��i����ƁA�t�@�N�g�����\�b�h�̌Ăяo���������s����j
			//��mNew�����Ƃ��낪���������[�N���Ă����Ɏv���Ă��܂����ǁA�����Ɖ��������B���Ȃ��B
			//�@��EffectiveC++��O�ł�52��
			result = nullptr;
		}
		return result;
	}

	//�R���X�g���N�^
	//���̃R���X�g���N�^�́AMyHandle�Ɋi�[����u���V�̐������s���ɗ�O�𓊂��܂��B
	mGdiBrush( const Option* opt )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiBrush();
	
	//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
	operator HBRUSH()const;

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//�ȉ��A�f�t�H���g�n�͎̂g�p�s�Ƃ���
	mGdiBrush() = delete;
	mGdiBrush( const mGdiBrush& src ) = delete;
	mGdiBrush& operator=( const mGdiBrush& src ) = delete;

private:
	//�\���b�h�u���V���쐬���܂��B���������MyHandle�ɒl������܂��B
	//opt : �ǂ�ȃu���V���쐬���邩�̏��
	//ret : �������ɐ^
	bool CreateSolidBrush( const Option_Solid& opt );

	//�n�b�`�u���V���쐬���܂��B���������MyHandle�ɒl������܂��B
	//opt : �ǂ�ȃu���V���쐬���邩�̏��
	//ret : �������ɐ^
	bool CreateHatchBrush( const Option_Hatch& opt );

	//�p�^�[���u���V���쐬���܂��B���������MyHandle�ɒl������܂��B
	//opt : �ǂ�ȃu���V���쐬���邩�̏��
	//ret : �������ɐ^
	bool CreatePatternBrush( const Option_Pattern& opt );


protected:
	//�n���h���̎���
	HBRUSH MyHandle;
};

#endif	//MGDIREHANDLE_H_INCLUDED

