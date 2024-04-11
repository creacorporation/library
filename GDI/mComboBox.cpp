//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�R���{�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MCOMBOBOX_CPP_COMPINLING
#include "mComboBox.h"
#include "General/mErrorLogger.h"

mComboBox::mComboBox()
{
}

mComboBox::~mComboBox()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mComboBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	//�����̕����g�p����̂�false��Ԃ��̂݁B
	return false;
}

//�E�C���h�E���J��
bool mComboBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"COMBOBOX";
	//�����I�v�V�����̎w�肪�Ȃ���΁A���ɂ��邱�Ƃ��Ȃ��̂Ŗ߂�
	if( opt == nullptr )
	{
		return true;
	}
	//�I�v�V�������w�肳��Ă���ꍇ�́A�����K�p���č쐬
	if( ( (const Option*)( opt ) )->method == Option::CreateMethod::USEOPTION )
	{
		const mComboBox::Option_UseOption* op = ( const mComboBox::Option_UseOption* )opt;
		//�L���E�����̐؂�ւ�
		retSetting.Style |= ( op->Enable ) ? ( WS_DISABLED ) : ( 0 );
		//�X�^�C���̓K�p(�ݒ�ΏۊO)
		switch( op->Style )
		{
		case mComboBox::Option::ControlStyle::SIMPLE:	//�V���v��
			retSetting.Style |= CBS_SIMPLE;
			break;
		case mComboBox::Option::ControlStyle::DROPDOWN:	//�h���b�v�_�E��
			retSetting.Style |= CBS_DROPDOWN;
			break;
		case mComboBox::Option::ControlStyle::DROPDOWNLIST:	//�h���b�v�_�E�����X�g
			retSetting.Style |= CBS_DROPDOWNLIST;
			break;
		default:	//���̑��B�C���M�����[�Ȃ̂ŃG���[�ɂ���B
			RaiseAssert( g_ErrorLogger , 0 , L"�z��O�̃X�^�C�����w�肳��܂���" , op->Style );
			retSetting.Style |= CBS_DROPDOWNLIST;
			break;
		}
		//�����\�[�g
		retSetting.Style |= ( op->AutoSort ) ? ( CBS_SORT ) : ( 0 );
		//�ʒu�̋L��
		SetWindowPosition( op->Pos );
	}
	return true;
}

//�A�C�e����ǉ�����
bool mComboBox::AddItem( const WString& caption )
{
	//���ڒǉ�
	LRESULT result = ::SendMessageW( GetMyHwnd() , CB_ADDSTRING , 0 , (LPARAM)caption.c_str() );
	switch( result )
	{
	case CB_ERR:
	case CB_ERRSPACE:
		RaiseAssert( g_ErrorLogger , result , L"CB_ADDSTRING failed : " + caption );
		return false;
	default:
		break;
	}
	return true;
}

bool mComboBox::AddItem( const WString& caption , const ItemDataEntry& data )
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

bool mComboBox::AddItem( const WString& caption , INT index )
{
	//���ڒǉ�
	LRESULT result = ::SendMessageW( GetMyHwnd() , CB_INSERTSTRING , index , (LPARAM)caption.c_str() );
	switch( result )
	{
	case CB_ERR:
	case CB_ERRSPACE:
		RaiseAssert( g_ErrorLogger , result , L"CB_INSERTSTRING failed : " + caption );
		return false;
	default:
		break;
	}
	return true;
}

bool mComboBox::AddItem( const WString& caption , const ItemDataEntry& data , INT index )
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
bool mComboBox::RemoveItem( INT index )
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
	LRESULT result = ::SendMessageW( GetMyHwnd() , CB_DELETESTRING , (WPARAM)index , 0 );
	switch( result )
	{
	case CB_ERR:
	case CB_ERRSPACE:
		RaiseAssert( g_ErrorLogger , index , L"CB_DELETESTRING failed" );
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
mComboBox::SelectedItems mComboBox::GetSelected( void )const
{
	LRESULT result = ::SendMessageW( GetMyHwnd() , CB_GETCURSEL , 0 , 0 );
	if( result == CB_ERR )
	{
		return SelectedItems();
	}

	SelectedItems items;
	items.push_back( (INT)result );
	return items;
}

//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
bool mComboBox::SetSelected( const SelectedItems& items )
{
	//�R���{�{�b�N�X�͓�����1�����I���ł��Ȃ�
	if( items.size() != 1 )
	{
		RaiseAssert( g_ErrorLogger , items.size() , L"Bad size" );
		return false;
	}
	return SetSelected( items.front() );
}

//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
bool mComboBox::SetSelected( INT item )
{
	//�I������
	if( ::SendMessageW( GetMyHwnd() , CB_SETCURSEL , item , 0 ) == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , item , L"CB_SETCURSEL failed" );
		return false;
	}

	return true;
}

//�w�肵���L���v�V���������A�C�e���̃C���f�b�N�X��Ԃ�
INT mComboBox::SearchItem( const WString& caption )
{
	LRESULT result = ::SendMessageW( GetMyHwnd() , CB_FINDSTRINGEXACT , 0 , (LPARAM)caption.c_str() );
	if( result == CB_ERR )
	{
		//�w��L���v�V�������Ȃ��̂͑z�肳��Ă��邩��A�G���[�̋L�^�͂Ȃ��B
		return -1;
	}
	return (INT)result;
}

//�w��C���f�b�N�X�̃L���v�V�������擾����
WString mComboBox::GetItemCaption( INT index )const
{
	LRESULT result;	//���ʂ̈ꎞ�i�[�p
	
	//���x���̕�����̒������擾����
	result = ::SendMessageW( GetMyHwnd() , CB_GETLBTEXTLEN , index , 0 );
	if( result == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"CB_GETLBTEXTLEN failed" );
		return L"";
	}

	//�i�[�p�̃o�b�t�@���쐬
	WCHAR* str_buffer = mNew WCHAR[ (size_t)result ];

	//�o�b�t�@�ɕ�������擾����
	result = ::SendMessageW( GetMyHwnd() , CB_GETLBTEXT , index , (LPARAM)str_buffer );
	if( result == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"CB_GETLBTEXT failed" );
		mDelete[] str_buffer;
		return L"";
	}

	//�擾���������񂩂�WString�I�u�W�F�N�g���\�z���Č��ʂɂ���
	WString result_string = str_buffer;
	mDelete[] str_buffer;
	return result_string;
}

//�w��C���f�b�N�X�Ɋ֘A�Â����Ă���f�[�^���擾����
bool mComboBox::GetItemData( INT index , ItemDataEntry& retdata )const
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
INT mComboBox::GetItemCount( void ) const
{
	return (INT)::SendMessage( GetMyHwnd() , CB_GETCOUNT , 0 , 0 );
}

//�A�C�e�����ړ�����
bool mComboBox::MoveItem( INT index , INT moveto )
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
	result = ::SendMessageW( GetMyHwnd() , CB_DELETESTRING , (WPARAM)index , 0 );
	if( result == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , ( index << 16 ) + moveto , L"CB_DELETESTRING failed : " + caption );
		return false;
	}
	result = ::SendMessageW( GetMyHwnd() , CB_INSERTSTRING , (WPARAM)moveto , (LPARAM)caption.c_str() );
	if( result == CB_ERR || result == CB_ERRSPACE )
	{
		RaiseAssert( g_ErrorLogger , ( index << 16 ) + moveto , L"CB_INSERTSTRING failed : " + caption );
		return false;
	}
	return true;
}

