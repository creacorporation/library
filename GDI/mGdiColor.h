//----------------------------------------------------------------------------
// �F���Ǘ�
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MGDICOLOR_H_INCLUDED
#define MGDICOLOR_H_INCLUDED

#include "mStandard.h"

class mGdiColor
{
public:

	mGdiColor();
	virtual ~mGdiColor();
	mGdiColor( const mGdiColor& source );
	const mGdiColor& operator=( const mGdiColor& source );

	//�g�r�u�F��Ԃɂ��F�\��
	//�E�ŏ�=0�`�ő�=1�ƂȂ�܂����A�l�Ƃ��Ă͂��͈̔͊O�̒l���ێ��ł��܂��B
	//�����̒l�́AHSV�F��ԁERGBQUAD�Ƃ̕ϊ�����0�`1�͈̔͂ɕ␳����܂�
	struct RGBColor
	{
		DOUBLE R;	//0��R��1�ɐ��K�����ꂽ�q�l
		DOUBLE G;	//0��R��1�ɐ��K�����ꂽ�f�l
		DOUBLE B;	//0��R��1�ɐ��K�����ꂽ�a�l
		DOUBLE A;	//0��R��1�ɐ��K�����ꂽ�A���t�@�l

		void clear( void )
		{
			R = 0.0;
			G = 0.0;
			B = 0.0;
			A = 0.0;
		}
	};

	//�g�r�u�F��Ԃɂ��F�\��
	//�E�F����1��=1�Ƃ����l�ł��B
	//�E�ʓx�A���x�A�A���t�@�l�́A�ŏ�=0�`�ő�=1�ƂȂ�܂����A�l�Ƃ��Ă͂��͈̔͊O�̒l���ێ��ł��܂��B
	//�����̒l�́ARGB�F��ԁERGBQUAD�Ƃ̕ϊ�����0�`1�͈̔͂ɕ␳����܂�
	struct HSVColor
	{
		DOUBLE H;	//�����1.0�ɐ��K�����ꂽ�F��
		DOUBLE S;	//0��R��1�ɐ��K�����ꂽ�ʓx
		DOUBLE V;	//0��R��1�ɐ��K�����ꂽ���x
		DOUBLE A;	//0��R��1�ɐ��K�����ꂽ�A���t�@�l

		void clear( void )
		{
			H = 0.0;
			S = 0.0;
			V = 0.0;
			A = 0.0;
		}
	};

	//�P�x�l
	template< class T > struct Brightness
	{
		DOUBLE v;
		Brightness( DOUBLE src )
		{
			v = src;
		}
		operator DOUBLE()const
		{
			return v;
		}
	};

	//�ԋP�x�l(0�`1)
	using BrR = Brightness< struct BrR >;
	//�΋P�x�l(0�`1)
	using BrG = Brightness< struct BrG >;
	//�P�x�l(0�`1)
	using BrB = Brightness< struct BrB >;
	//�A���t�@�l(0�`1)
	using BrA = Brightness< struct BrA >;
	//�F��(0�`1)
	using BrH = Brightness< struct BrH >;
	//�ʓx(0�`1)
	using BrS = Brightness< struct BrS >;
	//���x(0�`1)
	using BrV = Brightness< struct BrV >;

	//---------------------------------------------
	//�l�ݒ�n
	//---------------------------------------------

	//�l�̐ݒ�(�R���X�g���N�^Ver)
	// source : �ݒ肵�����l
	mGdiColor( const RGBColor& source );

	//�l�̐ݒ�(�R���X�g���N�^Ver)
	// source : �ݒ肵�����l
	mGdiColor( const HSVColor& source );

	//�l�̐ݒ�(�R���X�g���N�^Ver)
	// source : �ݒ肵�����l
	mGdiColor( const RGBQUAD& source );

	//�l�̐ݒ�(�R���X�g���N�^Ver)
	// source : �ݒ肵�����l
	mGdiColor( COLORREF source );

	//�l�̐ݒ�
	// col : �ݒ肵�����l
	void Set( const RGBColor& col );

	//�l�̐ݒ�
	// col : �ݒ肵�����l
	void Set( const HSVColor& col );

	//�l�̐ݒ�
	// col : �ݒ肵�����l
	void Set( const RGBQUAD& col );

	//�l�̐ݒ�
	// col : �ݒ肵�����l
	void Set( COLORREF col );

	//�l�̐ݒ�i�ԋP�x�l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrR& col );

	//�l�̐ݒ�i�΋P�x�l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrG& col );

	//�l�̐ݒ�i�P�x�l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrB& col );

	//�l�̐ݒ�i�A���t�@�l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrA& col );

	//�l�̐ݒ�i�F���l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrH& col );

	//�l�̐ݒ�i�ʓx�l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrS& col );

	//�l�̐ݒ�i���x�l�̂݁j
	// col : �ݒ肵�����l
	void Set( const BrV& col );

	//�l�̐ݒ�
	// src : �ݒ肵�����l
	const mGdiColor& operator=( const RGBColor& src );

	//�l�̐ݒ�
	// src : �ݒ肵�����l
	const mGdiColor& operator=( const HSVColor& src );

	//�l�̐ݒ�
	// src : �ݒ肵�����l
	const mGdiColor& operator=( const RGBQUAD& src );

	//�l�̐ݒ�
	// src : �ݒ肵�����l
	const mGdiColor& operator=( COLORREF src );

	//---------------------------------------------
	//�l�擾�n
	//---------------------------------------------

	//�l�̎擾
	//���݂̒l��RGB�ł͂Ȃ��ꍇ�́ARGB�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// retCol : �擾�����l
	void Get( RGBColor& retCol )const;

	//�l�̎擾
	//���݂̒l��HSV�ł͂Ȃ��ꍇ�́AHSV�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// retCol : �擾�����l
	void Get( HSVColor& retCol )const;

	//�l�̎擾
	//���݂̒l��RGB�ł͂Ȃ��ꍇ�́ARGB�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// retCol : �擾�����l
	void Get( RGBQUAD& retCol )const;

	//�l�̎擾
	//���݂̒l��RGB�ł͂Ȃ��ꍇ�́ARGB�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// retCol : �擾�����l
	void Get( COLORREF& retCol )const;

	//�l�̎擾
	//���݂̒l��RGB�ł͂Ȃ��ꍇ�́ARGB�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// ret : RGB�l
	operator RGBColor( void )const;

	//�l�̎擾
	//���݂̒l��HSV�ł͂Ȃ��ꍇ�́AHSV�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// ret : HSV�l
	operator HSVColor( void )const;

	//�l�̎擾
	//���݂̒l��RGB�ł͂Ȃ��ꍇ�́ARGB�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// ret : RGBQUAD�l
	operator RGBQUAD( void )const;

	//�l�̎擾
	//���݂̒l��RGB�ł͂Ȃ��ꍇ�́ARGB�ɕϊ������l���擾���܂����A
	//�C���X�^���X���ێ����Ă���l�͕ω����܂���B
	// ret : RGBQUAD�l
	operator COLORREF( void )const;

	//---------------------------------------------
	//�l���Z�n
	//---------------------------------------------

	//�l�̉��Z(�eRGB�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const RGBColor& src );

	//�l�̉��Z(HSV�l�ɂ��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const HSVColor& src );

	//�l�̉��Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const RGBQUAD& src );

	//�l�̉��Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( COLORREF src );

	//�l�̉��Z(�ԋP�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrR& src );

	//�l�̉��Z(�΋P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrG& src );

	//�l�̉��Z(�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrB& src );

	//�l�̉��Z(�A���t�@�l)
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrA& src );

	//�l�̉��Z(�F��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrH& src );

	//�l�̉��Z(�ʓx)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrS& src );

	//�l�̉��Z(���x)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă�����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator+=( const BrV& src );

	//�l�̌��Z(�eRGB�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const RGBColor& src );

	//�l�̌��Z(HSV�l�ɂ��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const HSVColor& src );

	//�l�̌��Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const RGBQUAD& src );

	//�l�̌��Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( COLORREF src );

	//�l�̌��Z(�ԋP�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrR& src );

	//�l�̌��Z(�΋P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrG& src );

	//�l�̌��Z(�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrB& src );

	//�l�̌��Z(�A���t�@�l)
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrA& src );

	//�l�̌��Z(�F��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrH& src );

	//�l�̌��Z(�ʓx)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrS& src );

	//�l�̌��Z(���x)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă��猸�Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ���Z����l
	// ret : ���݂̒l
	const mGdiColor& operator-=( const BrV& src );

	//�l�̏�Z(�eRGB�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const RGBColor& src );

	//�l�̏�Z(HSV�l�ɂ��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const HSVColor& src );

	//�l�̏�Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const RGBQUAD& src );

	//�l�̏�Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( COLORREF src );

	//�l�̏�Z(�ԋP�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrR& src );

	//�l�̏�Z(�΋P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrG& src );

	//�l�̏�Z(�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrB& src );

	//�l�̏�Z(�A���t�@�l)
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrA& src );

	//�l�̏�Z(�F��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrH& src );

	//�l�̏�Z(�ʓx)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrS& src );

	//�l�̏�Z(���x)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����Z���܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��Z����l
	// ret : ���݂̒l
	const mGdiColor& operator*=( const BrV& src );

	//�l�̏�]�Z(�eRGB�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const RGBColor& src );

	//�l�̏�]�Z(HSV�l�ɂ��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const HSVColor& src );

	//�l�̏�]�Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const RGBQUAD& src );

	//�l�̏�]�Z(255�K��RGB�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( COLORREF src );

	//�l�̏�]�Z(�ԋP�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrR& src );

	//�l�̏�]�Z(�΋P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrG& src );

	//�l�̏�]�Z(�P�x�l)
	//�E���݂̒l��RGB�ł͂Ȃ��ꍇ�ARGB�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrB& src );

	//�l�̏�]�Z(�A���t�@�l)
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrA& src );

	//�l�̏�]�Z(�F��)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrH& src );

	//�l�̏�]�Z(�ʓx)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrS& src );

	//�l�̏�]�Z(���x)
	//�E���݂̒l��HSV�ł͂Ȃ��ꍇ�AHSV�ɕϊ����Ă����]�����߂܂��B
	//�@���̂Ƃ��A�l��0�`1�͈̔͂ɂȂ��ꍇ�́A0�`1�ɕ␳����܂��B
	// src : ��]�Z����l
	// ret : ���݂̒l
	const mGdiColor& operator/=( const BrV& src );

	//---------------------------------------------
	//�l�ϊ��n
	//---------------------------------------------

	//RGB�F��Ԃ�HSV�F��Ԃɕϊ�����
	//���̊֐��́A�N���X�̃C���X�^���X���쐬���Ȃ��Ă��Ăяo�����Ƃ��o���܂��B
	static HSVColor RGBtoHSV( const RGBColor& src );

	//RGBQUAD�l��HSV�F��Ԃɕϊ�����
	//���̊֐��́A�N���X�̃C���X�^���X���쐬���Ȃ��Ă��Ăяo�����Ƃ��o���܂��B
	static HSVColor RGBQUADtoHSV( const RGBQUAD& src ); 

	//RGB�F��Ԃ�RGB�F��Ԃɕϊ�����
	//���̊֐��́A�N���X�̃C���X�^���X���쐬���Ȃ��Ă��Ăяo�����Ƃ��o���܂��B
	static RGBColor HSVtoRGB( const HSVColor& src );

	//RGBQUAD�l��RGB�F��Ԃɕϊ�����
	//���̊֐��́A�N���X�̃C���X�^���X���쐬���Ȃ��Ă��Ăяo�����Ƃ��o���܂��B
	static RGBColor RGBQUADtoRGB( const RGBQUAD& src );

	//HSV�F��Ԃ�RGBQUAD�l�ϊ�����
	//���̊֐��́A�N���X�̃C���X�^���X���쐬���Ȃ��Ă��Ăяo�����Ƃ��o���܂��B
	static RGBQUAD HSVtoRGBQUAD( const HSVColor& src );

	//RGB�F��Ԃ�RGBQUAD�l�ϊ�����
	//���̊֐��́A�N���X�̃C���X�^���X���쐬���Ȃ��Ă��Ăяo�����Ƃ��o���܂��B
	static RGBQUAD RGBtoRGBQUAD( const RGBColor& src );

protected:

	enum PrimaryColorSpace
	{
		COLORSPACE_RGB,		//RGB�F���
		COLORSPACE_HSV,		//HSV�F���
	};
	PrimaryColorSpace MyPrimaryColorSpace;

	union Color
	{
		RGBColor RGB;
		HSVColor HSV;
	};
	Color MyColor;

	void ChangeToRGB( void );
	void ChangeToHSV( void );
};


#endif //MGDIUTIL_H_INCLUDED
