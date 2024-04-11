//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���j���[�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MMENU_CPP_COMPILING
#include "mMenu.h"
#include "mGdiBitmap.h"
#include "General/mErrorLogger.h"
#include "General/mErrorLogger.h"

mMenu::mMenu( const Option* opt )
{
	//�|�b�v�A�b�v���j���[�ɂ��邩�𔻒�
	bool popup;
	if( opt == nullptr )
	{
		//�I�v�V�������w�肵�ĂȂ���΁A�|�b�v�A�b�v���j���[�ɂ��Ȃ�
		popup = false;
	}
	else if( opt->method == Option::CreateMethod::USEOPTION )
	{
		//�I�v�V�������w�肵�Ă���΁A�����ǂݎ��
		const Option_UseOption* option = (const Option_UseOption*)opt;
		popup = option->IsPopup;
	}
	else
	{
		//�Ȃ񂩕ςȂ��̂��n���ꂽ�ꍇ�̓G���[���L�^���āA�|�b�v�A�b�v���j���[�ɂ��Ȃ�
		RaiseAssert( g_ErrorLogger , opt->method , L"Unknown create method" );
		popup = false;
	}

	//���j���[�̍쐬
	MyRootMenu = mNew MenuHandle( popup );
}

mMenu::~mMenu()
{
	mDelete MyRootMenu;
}

//���j���[�ɃA�C�e����ǉ�����
bool mMenu::AddItem( const MenuOption& item , const mGdiResource* resource )
{
	//FunctionID�����W�F�N�g����Ă��Ȃ����`�F�b�N����
	if( item.FunctionId == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"FunctionID is zero" );
		return false;	//FunctionID=0�͎w�肪�Ȃ��Ă����W�F�N�g
	}

	//�A�C�e�����擾(�Ȃ��ꍇ�͐V�K�쐬)
	MenuItemEntry* entry = SearchItemEntry( item.Path , true );
	if( entry == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Couldn't retrieve item" );
		return false;
	}

	//-------------------
	//�������f�[�^�̍X�V
	//-------------------

	//���ڂ̃r�b�g�}�b�v���L��ΐݒ�
	entry->CheckedBitmap = item.CheckedBitmap;
	entry->UnchedkedBitmap = item.UncheckedBitmap;
	//���ڂ̃`�F�b�N���ǂ�ɂ��邩�B���ƃZ�p���[�^�ɂ��邩�B
	entry->Type = item.Type;
	//�@�\ID
	entry->FunctionId = item.FunctionId;
	//�L���v�V����
	entry->OptString = item.OptString;
	entry->UserData = item.UserData;

	//���j���[�̌����ڂ̍X�V
	MENUITEMINFOW info;
	SetMenuItemInfoStruct( *entry , info , resource );
	if( !::SetMenuItemInfoW( entry->Parent.Handle , entry->InternalId , false , &info ) )
	{
		//�T�u���j���[�̐ݒ�Ɏ��s
		RaiseAssert( g_ErrorLogger , 0 , L"SetMenuItemInfoW failed" );
		return false;
	}
	return true;

}

//���j���[����A�C�e�����폜����
bool mMenu::RemoveItem( const WStringVector& Path )
{
	//�A�C�e�����擾(�Y���A�C�e�����Ȃ��ꍇ�͕ʂɍ폜�Ƃ��ǂ��ł������̂Ő���I���Ŗ߂�)
	MenuItemEntry* entry = SearchItemEntry( Path , false );
	if( entry == nullptr )
	{
		return true;
	}

	//�Y���A�C�e���̃C�e���[�^��T��
	MenuHandle* ptr = &entry->Parent;
	MenuItem::iterator remove_itr = entry->Parent.Items.begin();
	for( remove_itr = ptr->Items.begin() ; remove_itr != ptr->Items.end() ; remove_itr++ )
	{
		if( *remove_itr == entry )
		{
			//���݃C�e���[�^�͍폜�Ώۂ��w���Ă���̂ŁA�폜�������s��
			if( ::DeleteMenu( entry->Parent.Handle , entry->InternalId , MF_BYCOMMAND ) )
			{
				RaiseAssert( g_ErrorLogger , entry->InternalId , L"DeleteMenu failed" );
				//�L�^�͂��邪�A�G���[�I���ɂ͂��Ȃ�
			}
			mDelete *remove_itr;					//�A�C�e�����폜
			ptr->Items.erase( remove_itr );		//�T�u���j���[�̏�񂩂�폜
			return true;
		}
	}

	RaiseAssert( g_ErrorLogger , entry->InternalId , L"Item not found" );
	return false;
}


bool mMenu::ScanFunctionId( MenuHandle* menu , UINT functionid , ScanItemCallback callback_func )const
{
	//�k���������ꍇ�͂��̂܂ܖ߂�
	if( menu == nullptr )
	{
		return true;
	}

	//�z���̍��ڑS�����X�L����
	bool result = true;
	MenuItem::iterator itr;
	for( itr = menu->Items.begin() ; itr != menu->Items.end() ; itr++ )
	{
		//�@�\ID����v���Ă���ΑΏۂ�����R�[���o�b�N�֐����Ă�
		if( ( *itr )->FunctionId == functionid )
		{
			result &= callback_func( *itr );
		}
		//�T�u���j���[���X�L����
		result &= ScanFunctionId( ( *itr )->Child , functionid , callback_func );
	}
	return result;
}

//���j���[��L���E�����ɂ���B�`�F�b�N�̏�Ԃ��X�V����B
bool mMenu::SetState( UINT FunctionId , bool enable , bool checked )
{
	//�L��������ύX����p�\����
	MENUITEMINFOW info;
	::ZeroMemory( &info , sizeof( MENUITEMINFOW ) );
	info.cbSize = sizeof( MENUITEMINFOW );
	info.fMask = MIIM_STATE;
	info.fState |= ( enable ) ? ( MFS_ENABLED ) : ( MFS_DISABLED );
	info.fState |= ( checked ) ? ( MFS_CHECKED ) : ( MFS_UNCHECKED );

	//�R�[���o�b�N�֐��̒�`
	ScanItemCallback CallbackAction = [ &info ]( MenuItemEntry* entry ) -> bool
	{
		//�����X�V
		if( !::SetMenuItemInfoW( entry->Parent.Handle , entry->InternalId , false , &info ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"SetMenuItemInfoW failed" );
			return false;
		}
		return true;
	};

	//�X�L��������
	return ScanFunctionId( MyRootMenu , FunctionId , CallbackAction );
}

//�w�肵���p�X�����A�C�e���ւ̃|�C���^��Ԃ��܂�
mMenu::MenuItemEntry* mMenu::SearchItemEntry( const WStringVector& Path , bool Create )
{
	//�w�肵��MenuHandle�̔z������pathitem�̖��O��T���A���̃|�C���^��Ԃ��B
	//menu_ptr : �T������Ώ�
	//pathitem : �T���o�����O
	//ret : ������΂��̃|�C���^�B������Ȃ����nullptr�B
	auto SubMenuIndexQuery = []( MenuHandle* menu_ptr , const WString& pathitem ) -> MenuItemEntry*
	{
		//���j���[���ڂ̂Ȃ�����Y���̍��ڂ�T��
		MenuItem::iterator itr;
		for( itr = menu_ptr->Items.begin() ; itr != menu_ptr->Items.end() ; itr++ )
		{
			if( ( *itr )->Name == pathitem )
			{
				//��������
				return *itr;
			}
		}
		//������Ȃ�����
		return nullptr;
	};

	MenuItemEntry* item_ptr = nullptr;		//���ݒ��ڂ��Ă���A�C�e���i�ŏI�I�Ɍ��ʂɂȂ�j
	MenuHandle** menu_ptr = &MyRootMenu;	//���ݒ��ڂ��Ă���T�u���j���[

	WStringVector::const_iterator path_itr;
	for( path_itr = Path.begin() ; path_itr != Path.end() ; path_itr++ )
	{
		//�T�u���j���[����H
		if( *menu_ptr == nullptr )
		{
			//���̃A�C�e���ɂ̓T�u���j���[�͂܂�����
			if( Create )
			{
				//Create�w�肪����ꍇ�́A�V�����T�u���j���[�𐶐�����
				*menu_ptr = mNew MenuHandle( true );
				if( *menu_ptr == nullptr )
				{
					RaiseAssert( g_ErrorLogger , 0 , L"mNew�����s���܂���" );
					return nullptr;
				}
				if( item_ptr != nullptr )
				{
					MENUITEMINFOW info;
					::ZeroMemory( &info , sizeof( MENUITEMINFOW ) );
					info.cbSize = sizeof( MENUITEMINFOW );
					info.fMask = MIIM_SUBMENU;
					info.hSubMenu = ( *menu_ptr )->Handle;

					if( !::SetMenuItemInfoW( item_ptr->Parent.Handle , item_ptr->InternalId , false , &info ) )
					{
						//�T�u���j���[�̐ݒ�Ɏ��s
						RaiseAssert( g_ErrorLogger , 0 , L"SetMenuItemInfoW failed" );
						return nullptr;
					}
				}
			}
			else
			{
				//�w�肪�Ȃ��ꍇ�͕s���݃G���[
				RaiseError( g_ErrorLogger , 0 , L"Menu ID is not found : " + *path_itr );
				return nullptr;
			}
		}
		//���j���[���ڂ̂Ȃ�����Y���̍��ڂ�T��
		item_ptr = SubMenuIndexQuery( *menu_ptr , *path_itr ); 
		if( item_ptr == nullptr )
		{
			//�Y�����ڂȂ�
			if( Create )
			{
				//Create�w�肪����ꍇ�A�V�����G���g���𐶐����đ}��
				item_ptr = mNew MenuItemEntry( **menu_ptr , *this );
				if( item_ptr == nullptr )
				{
					RaiseAssert( g_ErrorLogger , 0 , L"mNew�����s���܂���" );
					return nullptr;
				}
				( *menu_ptr )->Items.push_back( item_ptr );

				item_ptr->Name = *path_itr;
				item_ptr->InternalId = MyInternalIdStock.Get();

				//�}���ʒu
				INT item_count = GetMenuItemCount( item_ptr->Parent.Handle );

				//�A�C�e���̑}��
				MENUITEMINFOW info;
				::ZeroMemory( &info , sizeof( MENUITEMINFOW ) );
				info.cbSize = sizeof( MENUITEMINFOW );
				info.fMask = MIIM_ID | MIIM_STRING;
				info.wID = item_ptr->InternalId;
				info.dwTypeData = const_cast<LPWSTR>( path_itr->c_str() );
				info.cch = 0;//(UINT)path_itr->size();

				if( !::InsertMenuItemW( item_ptr->Parent.Handle , (UINT)item_count , true , &info ) )
				{
					//�A�C�e���̑}���Ɏ��s
					RaiseAssert( g_ErrorLogger , 0 , L"InsertMenuItemW failed" );
					return nullptr;
				}
			}
			else
			{
				//�w�肪�Ȃ��ꍇ�͕s���݃G���[
				RaiseError( g_ErrorLogger , 0 , L"Menu ID is not found : " + *path_itr );
				return nullptr;
			}
		}
		menu_ptr = &item_ptr->Child;
	}
	return item_ptr;
}

HMENU mMenu::GetMenuHandle( void )
{
	return MyRootMenu->Handle;
}

bool mMenu::SetMenuItemInfoStruct( const MenuItemEntry& src , MENUITEMINFOW& retDst , const mGdiResource* res )const
{
	//src�̓��e�𔽉f�ł���MENUITEMINFOW�\���̂��쐬���ĕԂ��B
	::ZeroMemory( &retDst , sizeof( MENUITEMINFOW ) );
	retDst.cbSize = sizeof( MENUITEMINFOW );
	retDst.fMask = MIIM_ID | MIIM_STRING;
	retDst.wID = src.InternalId;

	//�`�F�b�N�̃^�C�v��ݒ�
	switch( src.Type )
	{
	case MenuType::NORMALCHECK:	//�ӂ[�̃`�F�b�N�i����j
		retDst.fMask |= MIIM_FTYPE;
		retDst.fType |= 0;
		retDst.dwTypeData = const_cast<LPWSTR>( src.Name.c_str() );
		break;
	case MenuType::RADIOCHECK:	//����
		retDst.fMask |= MIIM_FTYPE;
		retDst.fType |= MFT_RADIOCHECK;
		retDst.dwTypeData = const_cast<LPWSTR>( src.Name.c_str() );
		break;
	case MenuType::SEPARATOR:	//�Z�p���[�^�B�Z�p���[�^�Ȃ̂ŃL���v�V�����Ƃ��w�肵�Ă����ʁB
		retDst.fMask |= MIIM_FTYPE;
		retDst.fType |= MFT_SEPARATOR;
		break;
	default:
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)src.Type , L"Bad Menutype" );
		retDst.fMask |= MIIM_FTYPE;
		retDst.fType |= 0;
		retDst.dwTypeData = const_cast<LPWSTR>( src.Name.c_str() );
		return false;
	}

	//���j���[���ڂ̃r�b�g�}�b�v������ΐݒ�
	if( res != nullptr )
	{
		const mGdiBitmap* checked_bmp = res->GetItem< mGdiBitmap >( src.CheckedBitmap );
		if( checked_bmp != nullptr )
		{
			retDst.fMask |= MIIM_CHECKMARKS;
			retDst.hbmpChecked = (HBITMAP)checked_bmp->GetHandle();
		}
		const mGdiBitmap* unchecked_bmp = res->GetItem< mGdiBitmap >( src.UnchedkedBitmap );
		if( unchecked_bmp != nullptr )
		{
			retDst.fMask |= MIIM_CHECKMARKS;
			retDst.hbmpUnchecked = (HBITMAP)unchecked_bmp->GetHandle();
		}
	}
	return true;
}

bool mMenu::ScanInternalId( MenuHandle* menu , USHORT internalid , WStringDeque& retPath )const
{
	//�k���������ꍇ�͂��̂܂ܖ߂�
	if( menu == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Handle is null" );
		return false;
	}

	//�z���̍��ڑS�����X�L����
	bool result = true;
	MenuItem::iterator itr;
	for( itr = menu->Items.begin() ; itr != menu->Items.end() ; itr++ )
	{
		retPath.push_back( ( *itr )->Name );

		//����ID����v���Ă���΂���
		if( ( *itr )->InternalId == internalid )
		{
			return true;
		}
		//�T�u���j���[���X�L�������A�������ꂽ��I��
		if( ScanInternalId( ( *itr )->Child , internalid , retPath ) )
		{
			return true;
		}

		retPath.pop_back();
	}
	//�Y���Ȃ�
	RaiseError( g_ErrorLogger , 0 , L"Menu scanning failed" );
	return false;
}

bool mMenu::QuerySelectedMenu( WPARAM wparam , MenuOption& retItem )const
{
	//�Y���A�C�e����T��
	WStringDeque path;
	if( !ScanInternalId( MyRootMenu , LOWORD( wparam ) , path ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"Menu scanning failed" );
		return false;
	}

	//Deque��Vector�ɕϊ�
	retItem.Path.clear();
	retItem.Path.reserve( path.size() );
	while( !path.empty() )
	{
		retItem.Path.push_back( path.front() );
		path.pop_front();
	}

	//�A�C�e���̃|�C���^���擾
	//Create�p�����[�^��false�̏ꍇ�̓����o�ϐ��͕ω����Ȃ�
	MenuItemEntry* item = const_cast<mMenu*>(this)->SearchItemEntry( retItem.Path , false );
	if( item == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Menu item missing : " + retItem.Path.back() );
		return false;
	}

	//���ʂ��R�s�[
	retItem.Type = item->Type;
	retItem.CheckedBitmap = item->CheckedBitmap;
	retItem.UncheckedBitmap = item->UnchedkedBitmap;
	retItem.FunctionId = item->FunctionId;
	retItem.UserData = item->UserData;
	retItem.OptString = item->OptString;
	return true;
}

bool mMenu::QueryItem( UINT function_id , MenuOptionArray& retItem )const
{
	//�Ƃ肠�������ʂ��N���A
	retItem.clear();

	//�R�[���o�b�N�֐�����������
	ScanItemCallback CallbackAction = [ &retItem , this ]( MenuItemEntry* item ) -> bool
	{
		MenuOption entry;
		//�n���ꂽitem�̏���InternalId���g���Č�������
		QuerySelectedMenu( item->InternalId , entry );
		//���ʈꗗ�ɒǉ�
		retItem.push_back( entry );
		return true;
	};

	//function_id�ŃX�L����
	ScanFunctionId( MyRootMenu , function_id , CallbackAction );

	//���ʂ��P�ł��o�^����Ă���ΊY�����ڂ��������Ƃ������ƂȂ̂Ő^��Ԃ�
	return !retItem.empty();

}


