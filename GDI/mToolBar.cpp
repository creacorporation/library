//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�c�[���o�[�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MTOOLBAR_CPP_COMPILING
#include "mToolBar.h"
#include <commctrl.h>
#include <algorithm>

#pragma comment (lib, "comctl32.lib")

mToolBar::mToolBar()
{
	//�����ɃC���X�^���X�����󂾂��A�Ƃ肠������nullptr�ɏ��������Ă����B
	MyImgage = nullptr;
}

mToolBar::~mToolBar()
{
	mDelete MyImgage;
}

//�E�C���h�E�N���X�̓o�^������
bool mToolBar::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	::InitCommonControls();
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mToolBar::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = TOOLBARCLASSNAMEW;
	return true;
}

bool mToolBar::OnCreate( const void* opt )
{
	//TBBUTTON�\���̂̃T�C�Y��ݒ�
	::SendMessageW( GetMyHwnd() , TB_BUTTONSTRUCTSIZE , sizeof( TBBUTTON ) , 0 );


	//�I�v�V�����w�肠��H
	if( opt != nullptr )
	{
		//�I�v�V�����̎w�肪����ꍇ�A����ɉ������ݒ������

		//�I�v�V�����ʂ�̃C���[�W���X�g�𐶐�
		const mToolBar::Option_UseOption* op = (const mToolBar::Option_UseOption*)opt;
		MyImgage = mNew mGdiDualImagelist( &op->ImgOpt );

		//�t���b�g�X�^�C���ɂ���H
		LPARAM style = ::SendMessageW( GetMyHwnd() , TB_GETSTYLE , 0 , 0 );
		if( op->IsFlat )
		{
			style |= TBSTYLE_FLAT;
		}
		//���X�g�X�^�C���ɂ���H
		if( op->IsListStyle )
		{
			style |= TBSTYLE_LIST;
		}
		//�c�[���`�b�v���g���H
		if( op->IsTooltipReq )
		{
			style |= TBSTYLE_TOOLTIPS;
		}
		::SendMessageW( GetMyHwnd() , TB_SETSTYLE , 0 , style );
	}
	else
	{
		//opt���k���������ꍇ�A�f�t�H���g�ō쐬�B
		//�t�@�N�g�����\�b�h��opt���k���������ꍇ���͂����Ă���̂ŁA
		//���̂Ƃ���͂����ɗ��邱�Ƃ͂Ȃ����A
		//�Y�ꂽ����Ɍp���Ƃ����ăk���ŗ���悤�ɂȂ邩������Ȃ��̂ŁB
		mGdiImagelist::Option_UseOption Option;
		MyImgage = mNew mGdiDualImagelist( &Option );
	}

	//�h���b�v�_�E���o����悤��
	LPARAM exstyle = ::SendMessageW( GetMyHwnd() , TB_GETEXTENDEDSTYLE , 0 , 0 );
	exstyle |= TBSTYLE_EX_DRAWDDARROWS;	//�E���Ɂ��}�[�N���o����悤�ɂ���
	::SendMessageW( GetMyHwnd() , TB_SETEXTENDEDSTYLE , 0 , exstyle );

	//�C���[�W���X�g���c�[���o�[�ɑ΂��ēo�^����B
	::SendMessageW( GetMyHwnd() , TB_SETIMAGELIST , 0 , (LPARAM)MyImgage->GetHandle() );
	::SendMessageW( GetMyHwnd() , TB_SETHOTIMAGELIST , 0 , (LPARAM)MyImgage->GetHandle2() );

	return true;
}

bool mToolBar::ExecUpdate( void )
{
	//�܂��A�S�A�C�e�����폜����
	ExecClear();
	MyInternalIdStock.Clear();

	//�ǉ�����A�C�e���̐��́H
	size_t item_count = MyItemOption.size();
	if( item_count == 0 )
	{
		//�ǉ����镨���Ȃ��ꍇ�́A����ȏシ�邱�Ƃ��Ȃ�����߂�
		return true;
	}

	//�{�^���̏��̈�m��
	std::vector<TBBUTTON> button( item_count );

	//TBBUTTON�̒��g�𖄂߂�B
	for( size_t i = 0 ; i < item_count ; i++ )
	{
		//�g�p����A�C�R���̃C���f�b�N�X���擾���邪�A�Y�������̏ꍇ�̓A�C�R���Ȃ��ɂ���B
		INT img_index = MyImgage->GetIndex( MyItemOption[ i ].ImageId );
		button[ i ].iBitmap = ( 0 <= img_index ) ? ( img_index ) : ( I_IMAGENONE );

		//�{�^���͗L��
		button[ i ].fsState = TBSTATE_ENABLED;

		//�{�^���̃L���v�V����
		button[ i ].iString = (INT_PTR)MyItemOption[ i ].Caption.c_str();

		//�ȉ��Q�́A�c�[���o�[���N���b�N���ꂽ�Ƃ���WM_NOTIFY��NMMOUSE����擾�ł���
		button[ i ].idCommand = (INT)i;						//(NMMOUSE::dwItemSpec)0�x�[�X�̃C���f�b�N�X
		button[ i ].dwData = MyItemOption[ i ].FunctionId;	//(NMMOUSE::dwItemData)FunctionID

		//�X�^�C���B�Z�p���[�^��AUTOSIZE�͗v��Ȃ��B
		switch( MyItemOption[ i ].Type )
		{
		case ItemType::BUTTONTYPE:
			button[ i ].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
			break;
		case ItemType::CHECKBOXTYPE:
			button[ i ].fsStyle = BTNS_CHECK | BTNS_AUTOSIZE;
			break;
		case ItemType::SEPARATOR:
			button[ i ].fsStyle = BTNS_SEP;
			break;
		case ItemType::DROPDOWN:
			button[ i ].fsStyle = BTNS_DROPDOWN | BTNS_AUTOSIZE;
			break;
		case ItemType::WHOLEDROPDOWN:
			button[ i ].fsStyle = BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE;
			break;
		default:
			break;
		}
	}
	::SendMessageW( GetMyHwnd() , TB_ADDBUTTONSW , item_count , (LPARAM)button.data() );
	return true;
}

bool mToolBar::ExecClear( void )
{
	//�S�{�^�����폜����B�S���܂Ƃ߂ď������@����������Ȃ��̂ŁA
	//�G���[�ɂȂ�܂ŁA�����擪�̃{�^���������Ă����B
	bool result;
	do
	{
		result = ( ::SendMessageW( GetMyHwnd() , TB_DELETEBUTTON , 0 , 0 ) != 0 );
	}while( result );

	return true;
}

//�A�C�R����L���E�����ɂ���
bool mToolBar::SetState( UINT FunctionId , bool enable , bool checked )
{
	for( size_t i = 0 ; i < MyItemOption.size() ; i++ )
	{
		//FunctionID����v����ꍇ�́A
		if( MyItemOption[ i ].FunctionId == FunctionId )
		{
			//���݂̃X�e�[�^�X���擾
			LRESULT state = ::SendMessageW( GetMyHwnd() , TB_GETSTATE , i , 0 );
			//�L���ɂ���H
			if( enable )
			{
				state |= TBSTATE_ENABLED;
			}
			else
			{
				state &= ~TBSTATE_ENABLED;
			}
			//�`�F�b�N��Ԃɂ���H
			//��BTNS_CHECK�łȂ��Ă��`�F�b�N��Ԃɂ͏o���邪�A���[�U�[�������Ȃ��Ȃ�B
			//  �ēx�ݒ肷��Ό��ɖ߂�̂ŁA�����ăG���[�ɂ͂��Ȃ��B
			if( checked )
			{
				state |= TBSTATE_CHECKED;
			}
			else
			{
				state &= ~TBSTATE_CHECKED;
			}
			//�V�X�e�[�^�X��ݒ�
			if( !::SendMessageW( GetMyHwnd() , TB_SETSTATE , i , LOWORD( state ) ) )
			{
				RaiseAssert( g_ErrorLogger , i , L"TB_SETSTATE failed" );
			}
		}
	}
	return true;
}

//�A�C�e���̏����擾����
bool mToolBar::QueryItem( INT index , ItemOptionEntry& retInfo )const
{
	if( index < 0 || (INT)MyItemOption.size() <= index )
	{
		RaiseAssert( g_ErrorLogger , index , L"Bad index" );
		return false;
	}
	retInfo = MyItemOption[ index ];
	return true;
}

//�A�C�e���̏����擾����
bool mToolBar::QueryItem( const LPNMMOUSE mouse , ItemOptionEntry& retInfo )const
{
	if( mouse == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Opt mouse is nullptr" );
		return false;
	}

	//dwItemSpec�͂��̃N���X�Őݒ肵�Ă��āAINT����͂ݏo��̂͂��蓾�Ȃ�
	//(�l��TBBUTTON�̗v�f��)�̂ŁAINT�ɃL���X�g����OK
	return QueryItem( (INT)mouse->dwItemSpec , retInfo );
}


//-------------------------------------------------------------
// ��������AToolbarUpdateHandle�̎���
//-------------------------------------------------------------
mToolBar::ToolbarUpdateHandle::ToolbarUpdateHandle( mToolBar& toolbar )
{
	MyParent = &toolbar;
	MyImgage = toolbar.MyImgage;
	MyItemOption = &toolbar.MyItemOption;
}

mToolBar::ToolbarUpdateHandle::~ToolbarUpdateHandle()
{
	MyParent->ExecUpdate();
	return;
}

//�c�[���o�[�̃A�C�R���ɂ���C���[�W���X�g���擾
mGdiDualImagelist& mToolBar::ToolbarUpdateHandle::Image()const
{
	return *MyImgage;
}

mToolBar::ItemOption& mToolBar::ToolbarUpdateHandle::Item()const
{
	return *MyItemOption;
}

