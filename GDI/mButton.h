//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�{�^���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�V���v���ȃ{�^���ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B
*/

#ifndef MBUTTON_H_INCLUDED
#define MBUTTON_H_INCLUDED

#include "mWindow.h"

class mButton : public mWindow
{
public:


	//�{�^���������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�{�^�������̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�{�^���������̃I�v�V����
	struct Option_UseOption : public Option
	{
		WString Caption;		//�{�^���ɏ����Ă��镶����
		WindowPosition Pos;		//�\���ʒu
		bool Enable;			//�L��(true)/����(false)
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Enable = true;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mButton;
	}

protected:
	mButton();
	virtual ~mButton();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mButton( const mButton& src ) = delete;
	mButton& operator=( const mButton& src ) = delete;

};



#endif	//MBUTTON_H_INCLUDED

