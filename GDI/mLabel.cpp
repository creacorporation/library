//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���x���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MLABEL_CPP_COMPILING
#include "mLabel.h"
#include "mWindowCollection.h"
#include "General/mErrorLogger.h"

mLabel::mLabel()
{
}

mLabel::~mLabel()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mLabel::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mLabel::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"STATIC";
	//�I�v�V�����̎w�肪�Ȃ���΂��̂܂ܖ߂�
	if( opt == nullptr )
	{
		return true;
	}

	//�I�v�V�����̎w�肪����΂���ɏ]��
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		const mLabel::Option_UseOption* op = (const mLabel::Option_UseOption*)opt;

		//�ݒ肷�镶����
		retSetting.WindowName = op->Caption;
		//�A���p�T���h�̈���
		retSetting.Style |= ( op->NoPrefix ) ? ( SS_NOPREFIX ) : ( 0 );
		//�����̔z�u
		switch( op->Justify )
		{
		case Option::TextJustify::LEFT:
			retSetting.Style |= SS_LEFT;
			break;
		case Option::TextJustify::CENTER:
			retSetting.Style |= SS_CENTER;
			break;
		case Option::TextJustify::RIGHT:
			retSetting.Style |= SS_RIGHT;
			break;
		case Option::TextJustify::LEFTNOWORDWRAP:
			retSetting.Style |= SS_LEFTNOWORDWRAP;
			break;
		default:
			RaiseAssert( g_ErrorLogger , op->Justify , L"" );
			break;
		}
		//�z�u
		SetWindowPosition( op->Pos );
	}
	return true;
}

