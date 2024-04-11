//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�`�F�b�N�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�V���v���ȃ`�F�b�N�{�b�N�X�ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B
�E�`�F�b�N�}�[�N��t������O������A���܃`�F�b�N�}�[�N���t���Ă��邩���m�F�����肷��֐���
�@���̃N���X�̈��AmCheckboxFamily�Ő錾���Ă���܂��B
*/


#ifndef MCHECKBOX_H_INCLUDED
#define MCHECKBOX_H_INCLUDED

#include "mCheckboxFamily.h"

class mCheckBox : public mCheckboxFamily
{
public:

	//�������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�`�F�b�N�{�b�N�X�����̕��@
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

	//�`�F�b�N�{�^���������̃I�v�V�����i�v���O�����Ŏw��p�j
	struct Option_UseOption : public Option
	{
		WString Caption;		//�{�^���ɏ����Ă��镶����
		WindowPosition Pos;		//�\���ʒu
		bool Enable;			//�L��/����
		bool ThreeState;		//�^�̏ꍇ3�X�e�[�g
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Enable = true;
			ThreeState = false;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mCheckBox;
	}

protected:
	mCheckBox();
	virtual ~mCheckBox();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mCheckBox( const mCheckBox& src ) = delete;
	mCheckBox& operator=( const mCheckBox& src ) = delete;

};



#endif	//MCHECKBOX_H_INCLUDED

