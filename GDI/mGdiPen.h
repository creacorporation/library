//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�y���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI�̃y���ł��B
�E�쐬����y���́AOption�\���̂Ŏw�肵�܂��B
�E�t�@�N�g�����\�b�h��opt��nullptr��n���ƁA����1�|�C���g�̃y���ɂȂ�܂��B
*/

#ifndef MGDIPEN_H_INCLUDED
#define MGDIPEN_H_INCLUDED

#include "mStandard.h"
#include "GDI/mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

class mGdiPen : public mGdiHandle
{
public:

	//�I�v�V�����\����
	//���ۂɃC���[�W���X�g���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�y�������̕��@
		enum CreateMethod
		{
			USEOPTION,			//�ʏ�̕��@
			TRANSPARENT_PEN		//���ۂɂ͉��������Ȃ��y��
		};

		//�y���̎��
		enum PenKind
		{
			SOLID_PEN,			//�����̃y��  ________
			DASH_PEN,			//�j���̃y��  __ __ __
			DOT_PEN,			//�_���̃y��  ........
			DASHDOT_PEN,		//��_���y��  __.__.__
			DASHDOTDOT_PEN,		//��_���y��  __..__..
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�y�������̃I�v�V����
	//�ǂ�ȃy���𐶐����邩���R�R�Ŏw�肵�܂��B
	struct Option_UseOption : public Option
	{
		PenKind kind;
		INT width;
		COLORREF color;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			kind = PenKind::SOLID_PEN;
			width = 1;
			color = RGB( 0 , 0 , 0 );
		}
	};

	struct Option_Transparent : public Option
	{
		Option_Transparent() : Option( CreateMethod::TRANSPARENT_PEN )
		{
		}
	};

	//�t�@�N�g�����\�b�h
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			//�V�����y���̃C���X�^���X���쐬����
			result = mNew mGdiPen( (const Option*)opt );
		}
		catch( mException )
		{
			//�y���̐����Ɏ��s�����ꍇ�̓k���|�C���^��Ԃ�
			result = nullptr;
		}
		return result;
	}

	//�R���X�g���N�^
	//���̃R���X�g���N�^�́AMyHandle�Ɋi�[����u���V�̐������s���ɗ�O�𓊂��܂��B
	mGdiPen( const Option* option )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiPen();

	//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
	operator HPEN()const;

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const override;

private:
	
	//�ȉ��A�f�t�H���g�n�͎̂g�p�s�Ƃ���
	mGdiPen() = delete;
	mGdiPen( const mGdiPen& src ) = delete;
	mGdiPen& operator=( const mGdiPen& src ) = delete;

private:

	//�y������
	//�EOption_UseOption���g�p����Ƃ��p
	bool CreateHandle( const Option_UseOption& opt );

	//�EOption_Transparent���g�p����Ƃ��p
	bool CreateHandle( const Option_Transparent& opt );

protected:
	//�n���h���̎���
	HPEN MyHandle;
};

#endif	//MGDIPEN_H_INCLUDED

