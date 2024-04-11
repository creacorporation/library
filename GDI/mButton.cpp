//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�{�^���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MBUTTON_CPP_COMPINLING
#include "mButton.h"

mButton::mButton()
{
}

mButton::~mButton()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mButton::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mButton::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";
	//�I�v�V�����w�肪�Ȃ��ꍇ�͂��̂܂ܖ߂�
	if( opt == nullptr )
	{
		return true;
	}

	//�I�v�V�����̎w�肪����΁A���̎�ނɂ�萶�����@�𕪊�
	if( ( (const Option*)( opt ) )->method == Option::CreateMethod::USEOPTION )
	{
		const mButton::Option_UseOption* op = ( const mButton::Option_UseOption* )opt;

		//�E�C���h�E�̃L���v�V����
		retSetting.WindowName = op->Caption;
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

