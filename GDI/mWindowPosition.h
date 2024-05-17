//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�E�C���h�E�̈ʒu�w��N���X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�E�C���h�E�̈ʒu���w�肷��N���X�ł��B
�R���p�C�������ł̓s���ŁAmWindow���番������܂����B
(mMenu.h��mWindow.h���z�C���N���[�h�ɂȂ�)
*/

#ifndef MWINDOWPOSITION_H_INCLUDED
#define MWINDOWPOSITION_H_INCLUDED

#include "mStandard.h"

namespace mWindowPosition
{
	//���ΓI�ɍ��W�����肷�邽�߂̏��
	struct POSITION
	{
		FLOAT rate;			//�e�̃N���C�A���g�G���A�̃T�C�Y�ɑ΂���䗦
		INT offset;			//rate�ŎZ�o�����ʒu�ɑ΂���I�t�Z�b�g

		POSITION( FLOAT rate , INT offset )
		{
			this->rate = rate;
			this->offset = offset;
		}
		POSITION() = default;

		void set( FLOAT rate , INT offset )
		{
			this->rate = rate;
			this->offset = offset;
		}
	};

	struct PresetFullscreen
	{
		INT gap;
		PresetFullscreen( INT gap )
		{
			this->gap = gap;
		}
	};

	struct PresetUpperLeftWH
	{
		INT left;
		INT top;
		INT width;
		INT height;
		PresetUpperLeftWH( INT Left , INT Top , INT Width , INT Height )
		{
			this->left = Left;
			this->top = Top;
			this->width = Width;
			this->height = Height;
		}
	};

	struct PresetUpperLeftRECT
	{
		INT left;
		INT top;
		INT right;
		INT bottom;
		PresetUpperLeftRECT( INT Left , INT Top , INT Right , INT Bottom )
		{
			this->left = Left;
			this->top = Top;
			this->right = Right;
			this->bottom = Bottom;
		}
	};

	//�E�C���h�E�̑��ΓI�Ȉʒu�ݒ�
	//�E�C���h�E(���邢�̓{�^���Ȃǂ̃p�[�c��)��e�E�C���h�E�̃N���C�A���g�G���A���̂ǂ̂�����ɔz�u���邩���w�肵�܂��B
	//�㉺���E�ɂ��āA�e�E�C���h�E�̏�[�E���[����̔䗦�{�I�t�Z�b�g�ŕ\���܂��B
	//rate=0.0,offset=10	�����[����10�s�N�Z���̈ʒu
	//rate=1.0,offset=-10	���E�[����10�s�N�Z���̈ʒu
	//rate=0.5,offset=10	����������E��10�s�N�Z���̈ʒu
	//���[(rate=0.1,offset=  10)+�E�[(rate=0.1,offset=110) �����񂹌Œ蕝100�s�N�Z��
	//���[(rate=1.0,offset=-110)+�E�[(rate=1.0,offset=-10) ���E�񂹌Œ蕝100�s�N�Z��
	//���[(rate=0.0,offset=  10)+�E�[(rate=0.5,offset=-10) �����͐e��50�p�[�Z���g�{10�s�N�Z���̃p�f�B���O�B
	struct WindowPosition
	{
		POSITION left;		//���[�̈ʒu
		POSITION right;		//�E�[�̈ʒu
		POSITION top;		//��[�̈ʒu
		POSITION bottom;	//���[�̈ʒu
		WindowPosition()
		{
			left = { 0.0f , 0 };
			right = { 0.0f , 0 };
			top = { 0.0f , 0 };
			bottom = { 0.0f , 0 };
		}
		WindowPosition( const PresetFullscreen& v )
		{
			set( v );
		}
		void set( const PresetFullscreen& v )
		{
			left = { 0.0f , v.gap };
			right = { 1.0f , -v.gap };
			top = { 0.0f , v.gap };
			bottom = { 1.0f , -v.gap };
		}
		WindowPosition( const PresetUpperLeftWH& v )
		{
			set( v );
		}
		void set( const PresetUpperLeftWH& v )
		{
			left = { 0.0f , v.left };
			right = { 0.0f , v.left + v.width };
			top = { 0.0f , v.top };
			bottom = { 0.0f , v.top + v.height };
		}
		WindowPosition( const PresetUpperLeftRECT& v )
		{
			set( v );
		}
		void set( const PresetUpperLeftRECT& v )
		{
			left = { 0.0f , v.left };
			right = { 0.0f , v.right };
			top = { 0.0f , v.top };
			bottom = { 0.0f , v.bottom };
		}
	};
};

#endif

