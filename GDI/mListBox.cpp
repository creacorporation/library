//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���X�g�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MLISTBOX_CPP_COMPINLING
#include "mListBox.h"
#include "General/mErrorLogger.h"

mListBox::mListBox()
{
}

mListBox::~mListBox()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mListBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	//�����̕����g�p����̂�false��Ԃ��̂݁B
	return false;
}

//�E�C���h�E���J��
bool mListBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"LISTBOX";
	retSetting.Style |= LBS_NOTIFY;
	
	//�I�v�V�����w�肪�Ȃ��ꍇ�͂��̂܂ܖ߂�(�f�t�H���g�̃I�u�W�F�N�g�𐶐�)
	if( opt == nullptr )
	{
		return true;
	}

	//�I�v�V�����w�肪����΁A����ɏ]��
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		const mListBox::Option_UseOption* op = (const mListBox::Option_UseOption*)opt;
		//�L���E�����̐؂�ւ�
		retSetting.Style |= ( op->Enable != 0 ) ? ( 0 ) : ( WS_DISABLED );
		//�g����H
		retSetting.Style |= ( op->Border != 0 ) ? ( WS_BORDER ) : ( 0 );
		//�X�N���[���o�[����H
		retSetting.Style |= ( op->ScrollBar != 0 ) ? ( WS_VSCROLL ) : ( 0 );
		//�����\�[�g
		retSetting.Style |= ( op->AutoSort ) ? ( LBS_SORT ) : ( 0 );
		//�����I���^�C�v�H
		retSetting.Style |= ( op->Multiple ) ? ( LBS_MULTIPLESEL ) : ( 0 );
		//�ʒu�ƃT�C�Y
		SetWindowPosition( op->Pos );
	}
	return true;
}

//�A�C�e����ǉ�����
bool mListBox::AddItem( const WString& caption )
{
	//���ڒǉ�
	LRESULT result = ::SendMessageW( GetMyHwnd() , LB_ADDSTRING , 0 , (LPARAM)caption.c_str() );
	switch( result )
	{
	case LB_ERR:
	case LB_ERRSPACE:
		RaiseAssert( g_ErrorLogger , result , L"LB_ADDSTRING failed : " + caption );
		return false;
	default:
		break;
	}
	return true;
}

bool mListBox::AddItem( const WString& caption , const ItemDataEntry& data )
{
	//���ڒǉ�
	if( !AddItem( caption ) )
	{
		return false;
	}

	//�L���v�V�����ƃf�[�^�̊֘A�Â�
	MyItemData[ caption ] = data;
	return true;
}

bool mListBox::AddItem( const WString& caption , INT index )
{
	//���ڒǉ�
	LRESULT result = ::SendMessageW( GetMyHwnd() , LB_INSERTSTRING , index , (LPARAM)caption.c_str() );
	switch( result )
	{
	case LB_ERR:
	case LB_ERRSPACE:
		RaiseAssert( g_ErrorLogger , result , L"LB_INSERTSTRING failed : " + caption );
		return false;
	default:
		break;
	}
	return true;
}

bool mListBox::AddItem( const WString& caption , const ItemDataEntry& data , INT index )
{
	//���ڒǉ�
	if( !AddItem( caption , index ) )
	{
		return false;
	}

	//�L���v�V�����ƃf�[�^�̊֘A�Â�
	MyItemData[ caption ] = data;
	return true;
}

//�A�C�e�����폜����
bool mListBox::RemoveItem( INT index )
{
	//�A�C�e���̐����傫���l�Ƃ��A���̈ʒu�Ƃ��ςȂ̂��w�肵�Ă���΃G���[�ɂ���
	if( index < 0 || (INT)MyItemData.size() <= index )
	{
		RaiseAssert( g_ErrorLogger , index , L"Bad index" );
		return false;
	}

	//���ꂩ��폜���鍀�ڂ̃L���v�V�������擾����
	WString caption = GetItemCaption( index );

	//�폜����
	LRESULT result = ::SendMessageW( GetMyHwnd() , LB_DELETESTRING , (WPARAM)index , 0 );
	switch( result )
	{
	case LB_ERR:
	case LB_ERRSPACE:
		RaiseAssert( g_ErrorLogger , index , L"LB_DELETESTRING failed" );
		return false;
	default:
		break;
	}

	//�폜�ł����̂ŁA�L���v�V�����ƃf�[�^�̊֘A�Â����폜����
	//���ɓ����̃L���v�V�����������ڂ����݂���ꍇ�͊֘A�Â����폜���Ȃ�
	if( MyItemData.count( caption ) == 0 )
	{
		//���̃L���v�V�����ɑ΂���f�[�^�͑��݂��Ȃ�
		return true;
	}
	for( INT i = GetItemCount() - 1 ; 0 <= i ; i-- )
	{
		if( GetItemCaption( i ) == caption )
		{
			//�����̕�������̂ō폜����
			return true;
		}
	}
	//�����̕����Ȃ�����폜
	MyItemData.erase( caption );
	return true;
}

//�I������Ă���C���f�b�N�X�̈ꗗ���擾����
mListBox::SelectedItems mListBox::GetSelected( void )const
{
	//�I������Ă��鍀�ڂ̐����擾����
	LRESULT selcount = ::SendMessageW( GetMyHwnd() , LB_GETSELCOUNT , 0 , 0 );

	//���ڂ̐���LB_ERR���Ԃ��Ă���ꍇ�́A1�����I���ł��郊�X�g�{�b�N�X
	//�����łȂ���΁A���̐������I������Ă���
	if( selcount == LB_ERR )
	{
		//1�����I���ł��Ȃ����X�g�{�b�N�X�̏ꍇ�ALB_GETCURSEL�őI������Ă��鍀�ڂ��擾�ł���B
		//���̏ꍇ�A�����I������Ă��Ȃ����LB_ERR���Ԃ�B
		LRESULT result = ::SendMessageW( GetMyHwnd() , LB_GETCURSEL , 0 , 0 );
		if( result == LB_ERR )
		{
			//�����I������Ă��Ȃ�
			return SelectedItems();
		}
		//�I������Ă��鍀�ڂ�ǉ�
		SelectedItems items;
		items.push_back( (INT)result );
		return items;
	}
	else
	{
		//�����I���ł��郊�X�g�{�b�N�X�ł́ALB_GETSELITEMS���g���đI������Ă���A�C�e�����擾����
		//LB_GETSELITEMS��LPARAM��x64�ł�INT�̔z��ŗǂ��͗l
		INT* selected = mNew INT[ selcount ];

		//�I������Ă��鍀�ڂ̈ꗗ���擾
		LRESULT result = ::SendMessageW( GetMyHwnd() , LB_GETSELITEMS , selcount , (LPARAM)selected );
		if( result == LB_ERR )
		{
			mDelete[] selected;
			return SelectedItems();
		}

		//�z����x�N�g���ɒu������
		SelectedItems items;
		for( LONG_PTR i = 0 ; i < selcount ; i++ )
		{
			items.push_back( selected[ i ] );
		}
		mDelete[] selected;
		return items;
	}
}

//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
bool mListBox::SetSelected( const SelectedItems& items )
{
	//�I������Ă��鍀�ڂ̐����擾����
	if( ::SendMessageW( GetMyHwnd() , LB_GETSELCOUNT , 0 , 0 ) == LB_ERR )
	{
		//1�����I���ł��Ȃ����X�g�{�b�N�X�̏ꍇ�A
		//���������Ă������ł��Ȃ��̂ŃG���[�ɂ���
		if( items.size() != 1 )
		{
			RaiseAssert( g_ErrorLogger , items.size() , L"Bad item number" );
			return false;
		}
	}

	//�S�I������U��������
	if( ::SendMessageW( GetMyHwnd() , LB_SETSEL , 0 , -1 ) == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , -1 , L"LB_SETSEL failed" );
		return false;
	}

	//�w�肳�ꂽ�C���f�b�N�X�ɑ΂��A�����I����Ԃɂ���
	for( SelectedItems::const_iterator itr = items.begin() ; itr != items.end() ; itr++ )
	{
		if( !SetSelected( *itr ) )
		{
			RaiseAssert( g_ErrorLogger , *itr , L"SetSelected failed" );
			return false;
		}
	}
	return true;
}

//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
bool mListBox::SetSelected( INT item )
{
	return ::SendMessageW( GetMyHwnd() , LB_SETSEL , 1 , item ) != LB_ERR;
}

//�w�肵���L���v�V���������A�C�e���̃C���f�b�N�X��Ԃ�
INT mListBox::SearchItem( const WString& caption )
{
	LRESULT result = ::SendMessageW( GetMyHwnd() , LB_FINDSTRINGEXACT , 0 , (LPARAM)caption.c_str() );
	if( result == LB_ERR )
	{
		//�w��L���v�V�������Ȃ��̂͑z�肳��Ă��邩��A�G���[�̋L�^�͂Ȃ��B
		return -1;
	}
	return (INT)result;
}

//�w��C���f�b�N�X�̃L���v�V�������擾����
WString mListBox::GetItemCaption( INT index )const
{
	LRESULT result;	//���ʂ̈ꎞ�i�[�p
	
	//���x���̕�����̒������擾����
	result = ::SendMessageW( GetMyHwnd() , LB_GETTEXTLEN , index , 0 );
	if( result == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"LB_GETTEXTLEN failed" );
		return L"";
	}

	//�i�[�p�̃o�b�t�@���쐬
	WCHAR* str_buffer = mNew WCHAR[ (size_t)result ];

	//�o�b�t�@�ɕ�������擾����
	result = ::SendMessageW( GetMyHwnd() , LB_GETTEXT , index , (LPARAM)str_buffer );
	if( result == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"LB_GETTEXT failed" );
		mDelete[] str_buffer;
		return L"";
	}

	//�擾���������񂩂�WString�I�u�W�F�N�g���\�z���Č��ʂɂ���
	WString result_string = str_buffer;
	mDelete[] str_buffer;
	return result_string;
}

//�w��C���f�b�N�X�Ɋ֘A�Â����Ă���f�[�^���擾����
bool mListBox::GetItemData( INT index , ItemDataEntry& retdata )const
{
	//�w�肳�ꂽ�C���f�b�N�X�̃L���v�V�������擾
	WString caption = GetItemCaption( index );

	//�擾�����L���v�V��������f�[�^���擾
	ItemData::const_iterator itr = MyItemData.find( caption );
	if( itr == MyItemData.end() )
	{
		//�w��L���v�V�����Ɋ֘A�Â����Ă���f�[�^�͂Ȃ�
		return false;
	}
	retdata = itr->second;
	return true;
}

//�����A�C�e�������邩���J�E���g����
INT mListBox::GetItemCount( void )const
{
	return (INT)::SendMessage( GetMyHwnd() , LB_GETCOUNT , 0 , 0 );
}

//�A�C�e�����ړ�����
bool mListBox::MoveItem( INT index , INT moveto )
{
	INT itemcount = GetItemCount();
	//�E�C���f�b�N�X��2�ȉ����Ɠ���ւ��ł��Ȃ��̂ŃG���[
	//�Eindex�����̐���������A�A�C�e���̐��𒴂��Ă���ꍇ�̓G���[
	//�Emoveto�����̐���������A�A�C�e���̐��𒴂��Ă���ꍇ�̓G���[
	if( ( itemcount < 2 ) ||
		( index  < 0 || itemcount <= index  ) ||
		( moveto < 0 || itemcount <= moveto ) )
	{
		RaiseAssert( g_ErrorLogger , ( index << 16 ) + moveto , L"" );
		return false;
	}
	//index��moveto�����������������ւ��K�v�Ȃ�
	if( index == moveto )
	{
		return true;
	}
	//�A�C�e���̃L���v�V�������擾����
	WString caption = GetItemCaption( index );

	//���݂̈ʒu����A�C�e�����폜���A�V�����ʒu�ɃA�C�e����}������
	//��MyItemData�̓A�C�e���̈ʒu���ς�邾��������ύX�͕s�v
	LRESULT result;
	result = ::SendMessageW( GetMyHwnd() , LB_DELETESTRING , (WPARAM)index , 0 );
	if( result == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , ( index << 16 ) + moveto , L"LB_DELETESTRING failed : " + caption );
		return false;
	}
	result = ::SendMessageW( GetMyHwnd() , LB_INSERTSTRING , (WPARAM)moveto , (LPARAM)caption.c_str() );
	if( result == LB_ERR || result == LB_ERRSPACE )
	{
		RaiseAssert( g_ErrorLogger , ( index << 16 ) + moveto , L"LB_INSERTSTRING failed : " + caption );
		return false;
	}
	return true;
}

