//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�O���[�v�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�O���[�v�{�b�N�X�ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B


*/

#ifndef MGROUPBOX_H_INCLUDED
#define MGROUPBOX_H_INCLUDED

#include "mWindow.h"

class mGroupBox : public mWindow
{
public:

	//�E�C���h�E�v���V�[�W��
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )override;

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

	//�O���[�v�{�b�N�X�������̃I�v�V����
	struct Option_UseOption : public Option
	{
		WString Caption;		//�O���[�v�{�b�N�X�̍���ɏ����Ă��镶����
		WindowPosition Pos;		//�\���ʒu
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mGroupBox;
	}

protected:
	mGroupBox();
	virtual ~mGroupBox();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mGroupBox( const mGroupBox& src ) = delete;
	mGroupBox& operator=( const mGroupBox& src ) = delete;

};



#endif	//MGROUPBOX_H_INCLUDED

