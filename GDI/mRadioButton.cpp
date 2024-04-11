//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�`�F�b�N�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MRADIOBUTTON_CPP_COMPINLING
#include "mRadioButton.h"
#include "General/mErrorLogger.h"

mRadioButton::mRadioButton()
{
}

mRadioButton::~mRadioButton()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mRadioButton::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	const mRadioButton::Option* Option = (const mRadioButton::Option*)opt;
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mRadioButton::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";

	//�I�v�V�����w�肪�Ȃ��ꍇ�͂��̂܂ܖ߂�
	if( opt == nullptr )
	{
		return true;
	}

	//�I�v�V�����̎w�肪����΁A���̎�ނɂ�萶�����@�����߂�
	if( ( (const Option*)opt )->method == Option::CreateMethod::USEOPTION )
	{
		const mRadioButton::Option_UseOption* op = ( const mRadioButton::Option_UseOption* )opt;
		//�E�C���h�E�̃L���v�V����
		retSetting.WindowName = op->Caption;
		//�������W�I�{�^���ɂ���
		retSetting.Style |= BS_AUTORADIOBUTTON;
		//�L���E����
		if( !op->Enable )
		{
			retSetting.Style |= WS_DISABLED;
		}
		//�ʒu�ƃT�C�Y
		SetWindowPosition( op->Pos );
	}
	return true;
}

