//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���x���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
���x���ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B

���Q�l��
�R���g���[���̐F���X��ς���ɂ́A�e�E�C���h�E��WM_CTLCOLORSTATIC���������܂��B
�EWM_CTLCOLORSTATIC
   WPARAM = �R���g���[���̃f�o�C�X�R���e�L�X�g�̃n���h��
   LPARAM = �R���g���[���̃E�C���h�E�n���h��
   �߂�l = �F��h��̂Ɏg���u���V�̃n���h��
            ex : GetStockObject( NULL_BRUSH );

*/

#ifndef MLABEL_H_INCLUDED
#define MLABEL_H_INCLUDED

#include "mWindow.h"

class mLabel : public mWindow
{
public:

	//���x���������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		//�e�L�X�g�̔z�u�����肷��
		enum TextJustify
		{
			CENTER,				//��������
			LEFT,				//������
			LEFTNOWORDWRAP,		//�������������s���Ȃ�
			RIGHT,				//�E����
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//���x���������̃I�v�V����
	struct Option_UseOption : public Option
	{
		WString Caption;		//�{�^���ɏ����Ă��镶����
		WindowPosition Pos;		//�\���ʒu
		TextJustify Justify;	//�e�L�X�g�̔z�u
		bool NoPrefix;			//&���v���t�B�N�X�Ƃ��ĉ��߂��Ȃ�(false�̏ꍇ"&+����"�ŉ����t���ɂȂ�)
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Justify = TextJustify::LEFT;
			NoPrefix = true;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void* option )throw( )
	{
		return mNew mLabel;
	}

protected:
	mLabel();
	virtual ~mLabel();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mLabel( const mLabel& src ) = delete;
	mLabel& operator=( const mLabel& src ) = delete;

};



#endif	//MLABEL_H_INCLUDED

