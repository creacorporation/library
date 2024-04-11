//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�G�f�B�b�g�R���g���[���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�G�f�B�b�g�R���g���[���ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B
*/

#ifndef MEDITBOX_H_INCLUDED
#define MEDITBOX_H_INCLUDED

#include "mWindow.h"

class mEditBox : public mWindow
{
public:


	//�R���g���[���������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�G�f�B�b�g�{�b�N�X�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		//������̔z�u
		enum TextJustify
		{
			LEFT,		//������
			CENTER,		//��������
			RIGHT		//�E����
		};

		//������̑啶���E�������������ϊ�����
		enum TextCase
		{
			NOCHANGE,	//�����Ȃ�
			UPPERCASE,	//�啶���ɕύX
			LOWERCASE,	//�������ɕύX
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�R���g���[���������̃I�v�V����
	struct Option_UseOption : public Option
	{
		WString Text;			//�{�^���ɏ����Ă��镶����
		WindowPosition Pos;		//�\���ʒu
		bool Multiline;			//�����s�^�C�v
		bool HScrollBar;		//�����X�N���[���o�[��\������
		bool VScrollBar;		//�����X�N���[���o�[��\������
		bool Password;			//�p�X���[�h�^�C�v
		bool Readonly;			//���[�h�I�����[
		bool Number;			//�����̂�
		bool RedirectEnter;		//�G���^�[�L�[�̓��͂����_�C���N�g����
		TextJustify Justify;	//������̔z�u
		TextCase Case;			//������̑啶���E�������������ϊ�����

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Text = L"";
			Multiline = false;
			HScrollBar = false;
			VScrollBar = false;
			Password = false;
			Readonly = false;
			Number = false;
			RedirectEnter = false;
			Justify = TextJustify::LEFT;
			Case = TextCase::NOCHANGE;
		}
	};

	//�G�f�B�b�g�R���g���[���̕�������Z�b�g
	bool SetText( const WString& NewText );

	//�G�f�B�b�g�R���g���[���̕�������擾
	bool GetText( WString& retText );

	//�G�f�B�b�g�R���g���[���̕�����̒����i�ڈ��j���擾
	DWORD GetTextLength( void )const;

public:

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mEditBox;
	}

	//�E�C���h�E�v���V�[�W��
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam );

protected:
	mEditBox();
	virtual ~mEditBox();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

	//true�Ȃ�΃G���^�[�L�[��e�E�C���h�E�Ƀ��_�C���N�g����
	bool MyRedirectEnter;

private:

	mEditBox( const mEditBox& src ) = delete;
	mEditBox& operator=( const mEditBox& src ) = delete;

};



#endif	//MEDITBOX_H_INCLUDED

