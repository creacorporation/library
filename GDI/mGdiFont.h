//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�t�H���g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI�̃t�H���g�ł��B
*/

#ifndef MGDIFONT_H_INCLUDED
#define MGDIFONT_H_INCLUDED

#include "mStandard.h"
#include "GDI/mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

/*
https://msdn.microsoft.com/ja-jp/library/cc428368.aspx
http://chokuto.ifdef.jp/urawaza/api/CreateFont.html

int nHeight,               // �t�H���g�̍���
int nWidth,                // ���ϕ�����
int nEscapement,           // ������������̊p�x
int nOrientation,          // �x�[�X���C���̊p�x
int fnWeight,              // �t�H���g�̑���
DWORD fdwItalic,           // �Α̂ɂ��邩�ǂ���
DWORD fdwUnderline,        // ������t���邩�ǂ���
DWORD fdwStrikeOut,        // ����������t���邩�ǂ���
DWORD fdwCharSet,          // �L�����N�^�Z�b�g
DWORD fdwOutputPrecision,  // �o�͐��x
DWORD fdwClipPrecision,    // �N���b�s���O���x
DWORD fdwQuality,          // �o�͕i��
DWORD fdwPitchAndFamily,   // �s�b�`�ƃt�@�~��
LPCTSTR lpszFace           // �t�H���g��
*/

namespace mGdiFont_Definitions
{
	//�t�H���g���쐬����Ƃ��Ɏg�p����L�����N�^�Z�b�g
	typedef enum FontCharSet
	{
		ANSI,			//�p��
		BALTIC,			//�o���g�O���̕���(���g�A�j�A��A���g�r�A�ꓙ)
		GB2312,			//�ȑ̎�������(�����A�V���K�|�[��)
		CHINESEBIG5,	//�ɑ̎�������(���ؖ����A���`�A�}�J�I)
		EASTEUROPE,		//�����n����̕���
		GREEK,			//�M���V����
		HANGUL,			//�؍���
		SHIFTJIS,		//���{��
		RUSSIAN,		//���V�A��
		TURKISH,		//�g���R��
		HEBREW,			//�w�u���C��
		ARABIC,			//�A���r�A��
		THAI,			//�^�C��
		LOCALE,			//���݂̃��P�[��(����ݒ�)�Ɋ�Â��Č��߂�
		OEM,			//����ŃE�C���h�E�Y���Ō��߂�
		eMAX_FontCharSet,
	}FontCharSet;

	//�t�H���g���쐬����Ƃ����Y�킳
	enum PrintQuality
	{
		NORMAL ,		//�f�t�H���g�B�R���g���[���p�l���̐ݒ�ɏ]���B
		ANTIALIAESED ,	//�W���̃A���`�G�C���A�X������B�ׂ������̕\�����C�}�C�`�B
		CLEARTYPE ,		//ClearType���g���BXP�ȍ~�̂݁B�Y��B
	};

	//�t�H���g���󎚂���Ƃ��̃s�b�`
	enum PrintPitch
	{
		DEFAULT ,		//�t�H���g�C���ɂ���
		FIXED ,			//�Œ蕝�ɂ���
		VARIABLE ,		//�ϕ��ɂ���
	};
}

class mGdiFont : public mGdiHandle
{
public:


	//�I�v�V�����\����
	//���ۂɃt�H���g���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�t�H���g�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		//�C���e���Z���X�ɒl�������o�Ă��Ďז��Ȃ̂ŊO�ɒǂ��o���Ă���܂�
		using FontCharSet = mGdiFont_Definitions::FontCharSet;
		using PrintQuality = mGdiFont_Definitions::PrintQuality;
		using PrintPitch = mGdiFont_Definitions::PrintPitch;

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�t�H���g�����̃I�v�V����
	//�ǂ�ȃt�H���g�𐶐����邩���R�R�Ŏw�肵�܂��B
	struct Option_UseOption : public Option
	{
		WString name;			//�g�p�������t�H���g�̖��O
		LONG height;			//�t�H���g�̍���
		LONG width;				//�t�H���g�̕�(0�ō������玩���ݒ�)
		bool isbold;			//�����ɂ������ꍇ��true
		bool isitalic;			//�C�^���b�N�ɂ������ꍇ��true
		bool isunderline;		//�A���_�[���C�����~�����ꍇ��true
		bool isstrikeout;		//�ł���������t�������ꍇ��true
		FontCharSet charset;	//�g�p����L�����N�^�Z�b�g
		PrintQuality quality;	//�󎚕i��
		PrintPitch pitch;		//������
		LONG orientation;		//�����̉�]�p�x(�����v���0.1�x�P��) �������̏ꍇ0�A�c�����̏ꍇ-900�������l�ł��B

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			name = L"";	//�t�H���g�����󕶎���̏ꍇ�AWindows���K���Ɍ��߂Ă����
			height = 9;
			width = 0;							//�������玩���ݒ�
			isbold = false;
			isitalic = false;
			isunderline = false;
			isstrikeout = false;
			charset = FontCharSet::LOCALE;		//���݂̃��P�[���Ō��߂�
			quality = PrintQuality::NORMAL;		//�f�t�H���g�B�A���`�G�C���A�X�͂��Ȃ�
			pitch = PrintPitch::DEFAULT;		//���̓t�H���g�C���Ō��߂�
			orientation = 0;					//��]�p�x
		}
	};

	//�t�@�N�g�����\�b�h
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			//�V�����t�H���g�̃C���X�^���X���쐬����
			result = mNew mGdiFont( (const Option*)opt );
		}
		catch( mException )
		{
			//�t�H���g�̐����Ɏ��s�����ꍇ�̓k���|�C���^��Ԃ�
			result = nullptr;
		}
		return result;
	}

	//�R���X�g���N�^
	//���̃R���X�g���N�^�́AMyHandle�Ɋi�[����u���V�̐������s���ɗ�O�𓊂��܂��B
	mGdiFont( const Option* opt )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiFont();
	
	//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
	operator HFONT()const;

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//�ȉ��A�f�t�H���g�n�̂̓R�s�[�����Ɩʓ|�������̂Ŏg�p�֎~

	mGdiFont() = delete;
	mGdiFont( const mGdiFont& src ) = delete;
	mGdiFont& operator=( const mGdiFont& src ) = delete;

private:

	//Option�Ɏw�肵�����e�Ɍ������t�H���g�𐶐����AMyHandle�ɓo�^����
	//�R���X�g���N�^����Ăяo�����z��
	bool MakeFont( const Option_UseOption* opt );


protected:
	//�n���h���̎���
	HFONT MyHandle;
};

#endif	//MGDIFONT_H_INCLUDED

