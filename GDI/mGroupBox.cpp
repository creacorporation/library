//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�O���[�v�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGROUPBOX_CPP_COMPINLING
#include "mGroupBox.h"
#include "mWindowCollection.h"

mGroupBox::mGroupBox()
{
}

mGroupBox::~mGroupBox()
{
}

LRESULT mGroupBox::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( MyChild != nullptr )
	{
		switch( msg )
		{
		case WM_SIZE:
			//�q�I�u�W�F�N�g�����݂���΁A�����ɃT�C�Y��`�d���܂�
			MyChild->AdjustSize( GetMyHwnd() );
			break;
		default:
			break;
		}
	}
	return __super::WindowProcedure( msg , wparam , lparam );
}


//�E�C���h�E�N���X�̓o�^������
bool mGroupBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mGroupBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";

	//�I�v�V�����̎w�肪�Ȃ���΂��̂܂ܖ߂�(�f�t�H���g�ɂȂ�)
	if( opt == nullptr )
	{
		return true;
	}
	//�I�v�V�����̎w�肪����΁A����ɏ]��
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		const mGroupBox::Option_UseOption* op = (const mGroupBox::Option_UseOption*)opt;
		retSetting.Style |= BS_GROUPBOX;
		//�L���v�V����
		retSetting.WindowName = op->Caption;
		//�ʒu�ƃT�C�Y
		SetWindowPosition( op->Pos );
	}
	return true;
}

