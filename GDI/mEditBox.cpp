//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�G�f�B�b�g�R���g���[���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MEDITBOX_CPP_COMPILING
#include "mEditBox.h"
#include "mWindowCollection.h"
#include "General/mErrorLogger.h"

mEditBox::mEditBox()
{
	MyRedirectEnter = false;
}

mEditBox::~mEditBox()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mEditBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mEditBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"EDIT";
	//�I�v�V�����w�肪�Ȃ��ꍇ�͂��̂܂ܖ߂�
	if( opt == nullptr )
	{
		return true;
	}

	//�I�v�V�����̎w�肪����΁A���̎�ނɂ�萶�����@�𕪊�
	if( ((const Option*)(opt))->method == Option::CreateMethod::USEOPTION )
	{
		const mEditBox::Option_UseOption* op = (const mEditBox::Option_UseOption*)opt;

		//�ݒ肷�镶����
		retSetting.WindowName = op->Text;
		//�����s�^�C�v
		retSetting.Style |= ( op->Multiline != 0 ) ? ( ES_MULTILINE ) : ( 0 );
		//�����X�N���[���o�[��\������
		retSetting.Style |= ( op->HScrollBar != 0 ) ? ( ES_AUTOHSCROLL ) : ( 0 );
		//�����X�N���[���o�[��\������
		retSetting.Style |= ( op->VScrollBar != 0 ) ? ( ES_AUTOVSCROLL ) : ( 0 );
		//�p�X���[�h�^�C�v
		retSetting.Style |= ( op->Password != 0 ) ? ( ES_PASSWORD ) : ( 0 );
		//���[�h�I�����[
		retSetting.Style |= ( op->Readonly != 0 ) ? ( ES_READONLY ) : ( 0 );
		//�����̂�
		retSetting.Style |= ( op->Number != 0 ) ? ( ES_NUMBER ) : ( 0 );
		//�G���^�[�L�[�̃��_�C���N�g
		MyRedirectEnter = op->RedirectEnter != 0;

		//������̔z�u
		switch( op->Justify ) 
		{
		case Option::TextJustify::LEFT:
			retSetting.Style |= ES_LEFT;
			break;
		case Option::TextJustify::CENTER:
			retSetting.Style |= ES_CENTER;
			break;
		case Option::TextJustify::RIGHT:
			retSetting.Style |= ES_RIGHT;
			break;
		default:
			break;
		}
		//������̑啶���E�������������ϊ�����
		switch( op->Case )
		{
		case Option::TextCase::NOCHANGE:
			break;
		case Option::TextCase::LOWERCASE:
			retSetting.Style |= ES_LOWERCASE;
			break;
		case Option::TextCase::UPPERCASE:
			retSetting.Style |= ES_UPPERCASE;
			break;
		default:
			break;
		}
		//�z�u
		SetWindowPosition( op->Pos );
	}

	//�v���V�[�W���̍����ւ����K�v�ł���΃t���O���Z�b�g
	if( MyRedirectEnter )
	{
		retSetting.ProcedureChange = true;
	}
	return true;
}

//�E�C���h�E�v���V�[�W��
LRESULT mEditBox::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( msg == WM_CHAR )
	{
		//�G���^�[�L�[�����_�C���N�g���邩�H
		if( ( MyRedirectEnter ) && ( wparam == VK_RETURN ) )
		{
			PostMessageW( GetMyParent() , msg , wparam , lparam );
		}
	}
	return __super::WindowProcedure( msg , wparam , lparam );
}

//�G�f�B�b�g�R���g���[���̕�������Z�b�g
bool mEditBox::SetText( const WString& NewText )
{
	return SetWindowTextW( GetMyHwnd() , NewText.c_str() );
}

//�G�f�B�b�g�R���g���[���̕�������擾
bool mEditBox::GetText( WString& retText )
{
	wchar_t* str = 0;
	DWORD len = GetTextLength();

	retText.clear();

	if( len )
	{
		str = mNew wchar_t[ len + 1 ];
		if( !str )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"mNew�����s���܂���" );
			return false;
		}
		if( GetWindowTextW( GetMyHwnd() , str , len + 1 ) )
		{
			retText = str; 
		}
	}
	mDelete[] str;
	return true;
}

//�G�f�B�b�g�R���g���[���̕�����̒����i�ڈ��j���擾
DWORD mEditBox::GetTextLength( void )const
{
	return (DWORD)GetWindowTextLengthW( GetMyHwnd() );
}




