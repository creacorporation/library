//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�`�F�b�N�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MCHECKBOX_CPP_COMPILING
#include "mCheckBox.h"

mCheckBox::mCheckBox()
{
}

mCheckBox::~mCheckBox()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mCheckBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	const mCheckBox::Option* Option = (const mCheckBox::Option*)opt;
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mCheckBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";
	//�I�v�V�����w�肪�Ȃ��ꍇ�͂��̂܂ܖ߂�
	if( opt == nullptr )
	{
		return true;
	}
	//�I�v�V�����̓��e�ɂ�蕪��
	if( ( (const Option*)( opt ) )->method == Option::CreateMethod::USEOPTION )
	{
		const mCheckBox::Option_UseOption* op = ( const mCheckBox::Option_UseOption* )opt;
		//�E�C���h�E�̃L���v�V����
		retSetting.WindowName = op->Caption;
		//�{�^���̃X�^�C��
		if( op->ThreeState )
		{
			retSetting.Style |= ( BS_3STATE | BS_AUTO3STATE );
		}
		else
		{
			retSetting.Style |= ( BS_CHECKBOX | BS_AUTOCHECKBOX );
		}
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


