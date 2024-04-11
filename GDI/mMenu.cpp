//----------------------------------------------------------------------------
// ウインドウ管理（メニュー）
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
	//ポップアップメニューにするかを判定
	bool popup;
	if( opt == nullptr )
	{
		//オプションが指定してなければ、ポップアップメニューにしない
		popup = false;
	}
	else if( opt->method == Option::CreateMethod::USEOPTION )
	{
		//オプションが指定してあれば、それを読み取る
		const Option_UseOption* option = (const Option_UseOption*)opt;
		popup = option->IsPopup;
	}
	else
	{
		//なんか変なものが渡された場合はエラーを記録して、ポップアップメニューにしない
		RaiseAssert( g_ErrorLogger , opt->method , L"Unknown create method" );
		popup = false;
	}

	//メニューの作成
	MyRootMenu = mNew MenuHandle( popup );
}

mMenu::~mMenu()
{
	mDelete MyRootMenu;
}

//メニューにアイテムを追加する
bool mMenu::AddItem( const MenuOption& item , const mGdiResource* resource )
{
	//FunctionIDがリジェクトされていないかチェックする
	if( item.FunctionId == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"FunctionID is zero" );
		return false;	//FunctionID=0は指定がなくてもリジェクト
	}

	//アイテムを取得(ない場合は新規作成)
	MenuItemEntry* entry = SearchItemEntry( item.Path , true );
	if( entry == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Couldn't retrieve item" );
		return false;
	}

	//-------------------
	//※内部データの更新
	//-------------------

	//項目のビットマップが有れば設定
	entry->CheckedBitmap = item.CheckedBitmap;
	entry->UnchedkedBitmap = item.UncheckedBitmap;
	//項目のチェックをどれにするか。あとセパレータにするか。
	entry->Type = item.Type;
	//機能ID
	entry->FunctionId = item.FunctionId;
	//キャプション
	entry->OptString = item.OptString;
	entry->UserData = item.UserData;

	//メニューの見た目の更新
	MENUITEMINFOW info;
	SetMenuItemInfoStruct( *entry , info , resource );
	if( !::SetMenuItemInfoW( entry->Parent.Handle , entry->InternalId , false , &info ) )
	{
		//サブメニューの設定に失敗
		RaiseAssert( g_ErrorLogger , 0 , L"SetMenuItemInfoW failed" );
		return false;
	}
	return true;

}

//メニューからアイテムを削除する
bool mMenu::RemoveItem( const WStringVector& Path )
{
	//アイテムを取得(該当アイテムがない場合は別に削除とかどうでもいいので正常終了で戻る)
	MenuItemEntry* entry = SearchItemEntry( Path , false );
	if( entry == nullptr )
	{
		return true;
	}

	//該当アイテムのイテレータを探す
	MenuHandle* ptr = &entry->Parent;
	MenuItem::iterator remove_itr = entry->Parent.Items.begin();
	for( remove_itr = ptr->Items.begin() ; remove_itr != ptr->Items.end() ; remove_itr++ )
	{
		if( *remove_itr == entry )
		{
			//現在イテレータは削除対象を指しているので、削除処理を行う
			if( ::DeleteMenu( entry->Parent.Handle , entry->InternalId , MF_BYCOMMAND ) )
			{
				RaiseAssert( g_ErrorLogger , entry->InternalId , L"DeleteMenu failed" );
				//記録はするが、エラー終了にはしない
			}
			mDelete *remove_itr;					//アイテムを削除
			ptr->Items.erase( remove_itr );		//サブメニューの情報から削除
			return true;
		}
	}

	RaiseAssert( g_ErrorLogger , entry->InternalId , L"Item not found" );
	return false;
}


bool mMenu::ScanFunctionId( MenuHandle* menu , UINT functionid , ScanItemCallback callback_func )const
{
	//ヌルだった場合はそのまま戻る
	if( menu == nullptr )
	{
		return true;
	}

	//配下の項目全部をスキャン
	bool result = true;
	MenuItem::iterator itr;
	for( itr = menu->Items.begin() ; itr != menu->Items.end() ; itr++ )
	{
		//機能IDが一致していれば対象だからコールバック関数を呼ぶ
		if( ( *itr )->FunctionId == functionid )
		{
			result &= callback_func( *itr );
		}
		//サブメニューもスキャン
		result &= ScanFunctionId( ( *itr )->Child , functionid , callback_func );
	}
	return result;
}

//メニューを有効・無効にする。チェックの状態を更新する。
bool mMenu::SetState( UINT FunctionId , bool enable , bool checked )
{
	//有効無効を変更する用構造体
	MENUITEMINFOW info;
	::ZeroMemory( &info , sizeof( MENUITEMINFOW ) );
	info.cbSize = sizeof( MENUITEMINFOW );
	info.fMask = MIIM_STATE;
	info.fState |= ( enable ) ? ( MFS_ENABLED ) : ( MFS_DISABLED );
	info.fState |= ( checked ) ? ( MFS_CHECKED ) : ( MFS_UNCHECKED );

	//コールバック関数の定義
	ScanItemCallback CallbackAction = [ &info ]( MenuItemEntry* entry ) -> bool
	{
		//情報を更新
		if( !::SetMenuItemInfoW( entry->Parent.Handle , entry->InternalId , false , &info ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"SetMenuItemInfoW failed" );
			return false;
		}
		return true;
	};

	//スキャン処理
	return ScanFunctionId( MyRootMenu , FunctionId , CallbackAction );
}

//指定したパスを持つアイテムへのポインタを返します
mMenu::MenuItemEntry* mMenu::SearchItemEntry( const WStringVector& Path , bool Create )
{
	//指定したMenuHandleの配下からpathitemの名前を探し、そのポインタを返す。
	//menu_ptr : 探索する対象
	//pathitem : 探し出す名前
	//ret : 見つかればそのポインタ。見つからなければnullptr。
	auto SubMenuIndexQuery = []( MenuHandle* menu_ptr , const WString& pathitem ) -> MenuItemEntry*
	{
		//メニュー項目のなかから該当の項目を探す
		MenuItem::iterator itr;
		for( itr = menu_ptr->Items.begin() ; itr != menu_ptr->Items.end() ; itr++ )
		{
			if( ( *itr )->Name == pathitem )
			{
				//見つかった
				return *itr;
			}
		}
		//見つからなかった
		return nullptr;
	};

	MenuItemEntry* item_ptr = nullptr;		//現在注目しているアイテム（最終的に結果になる）
	MenuHandle** menu_ptr = &MyRootMenu;	//現在注目しているサブメニュー

	WStringVector::const_iterator path_itr;
	for( path_itr = Path.begin() ; path_itr != Path.end() ; path_itr++ )
	{
		//サブメニューある？
		if( *menu_ptr == nullptr )
		{
			//このアイテムにはサブメニューはまだ無い
			if( Create )
			{
				//Create指定がある場合は、新しいサブメニューを生成する
				*menu_ptr = mNew MenuHandle( true );
				if( *menu_ptr == nullptr )
				{
					RaiseAssert( g_ErrorLogger , 0 , L"mNewが失敗しました" );
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
						//サブメニューの設定に失敗
						RaiseAssert( g_ErrorLogger , 0 , L"SetMenuItemInfoW failed" );
						return nullptr;
					}
				}
			}
			else
			{
				//指定がない場合は不存在エラー
				RaiseError( g_ErrorLogger , 0 , L"Menu ID is not found : " + *path_itr );
				return nullptr;
			}
		}
		//メニュー項目のなかから該当の項目を探す
		item_ptr = SubMenuIndexQuery( *menu_ptr , *path_itr ); 
		if( item_ptr == nullptr )
		{
			//該当項目なし
			if( Create )
			{
				//Create指定がある場合、新しいエントリを生成して挿入
				item_ptr = mNew MenuItemEntry( **menu_ptr , *this );
				if( item_ptr == nullptr )
				{
					RaiseAssert( g_ErrorLogger , 0 , L"mNewが失敗しました" );
					return nullptr;
				}
				( *menu_ptr )->Items.push_back( item_ptr );

				item_ptr->Name = *path_itr;
				item_ptr->InternalId = MyInternalIdStock.Get();

				//挿入位置
				INT item_count = GetMenuItemCount( item_ptr->Parent.Handle );

				//アイテムの挿入
				MENUITEMINFOW info;
				::ZeroMemory( &info , sizeof( MENUITEMINFOW ) );
				info.cbSize = sizeof( MENUITEMINFOW );
				info.fMask = MIIM_ID | MIIM_STRING;
				info.wID = item_ptr->InternalId;
				info.dwTypeData = const_cast<LPWSTR>( path_itr->c_str() );
				info.cch = 0;//(UINT)path_itr->size();

				if( !::InsertMenuItemW( item_ptr->Parent.Handle , (UINT)item_count , true , &info ) )
				{
					//アイテムの挿入に失敗
					RaiseAssert( g_ErrorLogger , 0 , L"InsertMenuItemW failed" );
					return nullptr;
				}
			}
			else
			{
				//指定がない場合は不存在エラー
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
	//srcの内容を反映できるMENUITEMINFOW構造体を作成して返す。
	::ZeroMemory( &retDst , sizeof( MENUITEMINFOW ) );
	retDst.cbSize = sizeof( MENUITEMINFOW );
	retDst.fMask = MIIM_ID | MIIM_STRING;
	retDst.wID = src.InternalId;

	//チェックのタイプを設定
	switch( src.Type )
	{
	case MenuType::NORMALCHECK:	//ふつーのチェック（レ印）
		retDst.fMask |= MIIM_FTYPE;
		retDst.fType |= 0;
		retDst.dwTypeData = const_cast<LPWSTR>( src.Name.c_str() );
		break;
	case MenuType::RADIOCHECK:	//●印
		retDst.fMask |= MIIM_FTYPE;
		retDst.fType |= MFT_RADIOCHECK;
		retDst.dwTypeData = const_cast<LPWSTR>( src.Name.c_str() );
		break;
	case MenuType::SEPARATOR:	//セパレータ。セパレータなのでキャプションとか指定しても無駄。
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

	//メニュー項目のビットマップがあれば設定
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
	//ヌルだった場合はそのまま戻る
	if( menu == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Handle is null" );
		return false;
	}

	//配下の項目全部をスキャン
	bool result = true;
	MenuItem::iterator itr;
	for( itr = menu->Items.begin() ; itr != menu->Items.end() ; itr++ )
	{
		retPath.push_back( ( *itr )->Name );

		//内部IDが一致していればこれ
		if( ( *itr )->InternalId == internalid )
		{
			return true;
		}
		//サブメニューもスキャンし、発見されたら終了
		if( ScanInternalId( ( *itr )->Child , internalid , retPath ) )
		{
			return true;
		}

		retPath.pop_back();
	}
	//該当なし
	RaiseError( g_ErrorLogger , 0 , L"Menu scanning failed" );
	return false;
}

bool mMenu::QuerySelectedMenu( WPARAM wparam , MenuOption& retItem )const
{
	//該当アイテムを探索
	WStringDeque path;
	if( !ScanInternalId( MyRootMenu , LOWORD( wparam ) , path ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"Menu scanning failed" );
		return false;
	}

	//DequeをVectorに変換
	retItem.Path.clear();
	retItem.Path.reserve( path.size() );
	while( !path.empty() )
	{
		retItem.Path.push_back( path.front() );
		path.pop_front();
	}

	//アイテムのポインタを取得
	//Createパラメータがfalseの場合はメンバ変数は変化しない
	MenuItemEntry* item = const_cast<mMenu*>(this)->SearchItemEntry( retItem.Path , false );
	if( item == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Menu item missing : " + retItem.Path.back() );
		return false;
	}

	//結果をコピー
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
	//とりあえず結果をクリア
	retItem.clear();

	//コールバック関数を準備して
	ScanItemCallback CallbackAction = [ &retItem , this ]( MenuItemEntry* item ) -> bool
	{
		MenuOption entry;
		//渡されたitemの情報をInternalIdを使って検索する
		QuerySelectedMenu( item->InternalId , entry );
		//結果一覧に追加
		retItem.push_back( entry );
		return true;
	};

	//function_idでスキャン
	ScanFunctionId( MyRootMenu , function_id , CallbackAction );

	//結果が１個でも登録されていれば該当項目があったということなので真を返す
	return !retItem.empty();

}


