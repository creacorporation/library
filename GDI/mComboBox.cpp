//----------------------------------------------------------------------------
// ウインドウ管理（コンボボックス）
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

//ウインドウクラスの登録をする
bool mComboBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	//既存の物を使用するのでfalseを返すのみ。
	return false;
}

//ウインドウを開く
bool mComboBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"COMBOBOX";
	//何もオプションの指定がなければ、特にすることもないので戻る
	if( opt == nullptr )
	{
		return true;
	}
	//オプションが指定されている場合は、それを適用して作成
	if( ( (const Option*)( opt ) )->method == Option::CreateMethod::USEOPTION )
	{
		const mComboBox::Option_UseOption* op = ( const mComboBox::Option_UseOption* )opt;

		//有効・無効の切り替え
		retSetting.Style |= ( !op->Enable ) ? ( WS_DISABLED ) : ( 0 );
		//スタイルの適用(設定対象外)
		switch( op->Style )
		{
		case mComboBox::Option::ControlStyle::SIMPLE:	//シンプル
			retSetting.Style |= CBS_SIMPLE;
			break;
		case mComboBox::Option::ControlStyle::DROPDOWN:	//ドロップダウン
			retSetting.Style |= CBS_DROPDOWN;
			break;
		case mComboBox::Option::ControlStyle::DROPDOWNLIST:	//ドロップダウンリスト
			retSetting.Style |= CBS_DROPDOWNLIST;
			break;
		default:	//その他。イレギュラーなのでエラーにする。
			RaiseAssert( g_ErrorLogger , 0 , L"想定外のスタイルが指定されました" , (int)op->Style );
			retSetting.Style |= CBS_DROPDOWNLIST;
			break;
		}
		//自動ソート
		retSetting.Style |= ( op->AutoSort ) ? ( CBS_SORT ) : ( 0 );
		//位置の記憶
		SetWindowPosition( op->Pos );
	}
	return true;
}

//アイテムを追加する
bool mComboBox::AddItem( const WString& caption )
{
	//項目追加
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
	//項目追加
	if( !AddItem( caption ) )
	{
		return false;
	}

	//キャプションとデータの関連づけ
	MyItemData[ caption ] = data;
	return true;
}

bool mComboBox::AddItem( const WString& caption , INT index )
{
	//項目追加
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
	//項目追加
	if( !AddItem( caption , index ) )
	{
		return false;
	}

	//キャプションとデータの関連づけ
	MyItemData[ caption ] = data;
	return true;
}

//アイテムを削除する
bool mComboBox::RemoveItem( INT index )
{
	//アイテムの数より大きい値とか、負の位置とか変なのを指定していればエラーにする
	if( index < 0 || (INT)MyItemData.size() <= index )
	{
		RaiseAssert( g_ErrorLogger , index , L"Bad index" );
		return false;
	}

	//これから削除する項目のキャプションを取得する
	WString caption = GetItemCaption( index );

	//削除処理
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

	//削除できたので、キャプションとデータの関連づけを削除する
	//他に同名のキャプションをもつ項目が存在する場合は関連づけを削除しない
	if( MyItemData.count( caption ) == 0 )
	{
		//このキャプションに対するデータは存在しない
		return true;
	}
	for( INT i = GetItemCount() - 1 ; 0 <= i ; i-- )
	{
		if( GetItemCaption( i ) == caption )
		{
			//同名の物があるので削除無し
			return true;
		}
	}
	//同名の物がないから削除
	MyItemData.erase( caption );
	return true;
}

//選択されているインデックスの一覧を取得する
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

//指定したインデックスのアイテムを選択状態にする
bool mComboBox::SetSelected( const SelectedItems& items )
{
	//コンボボックスは同時に1個しか選択できない
	if( items.size() != 1 )
	{
		RaiseAssert( g_ErrorLogger , items.size() , L"Bad size" );
		return false;
	}
	return SetSelected( items.front() );
}

//指定したインデックスのアイテムを選択状態にする
bool mComboBox::SetSelected( INT item )
{
	//選択処理
	if( ::SendMessageW( GetMyHwnd() , CB_SETCURSEL , item , 0 ) == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , item , L"CB_SETCURSEL failed" );
		return false;
	}

	return true;
}

//指定したキャプションをもつアイテムのインデックスを返す
INT mComboBox::SearchItem( const WString& caption )
{
	LRESULT result = ::SendMessageW( GetMyHwnd() , CB_FINDSTRINGEXACT , 0 , (LPARAM)caption.c_str() );
	if( result == CB_ERR )
	{
		//指定キャプションがないのは想定されているから、エラーの記録はなし。
		return -1;
	}
	return (INT)result;
}

//指定インデックスのキャプションを取得する
WString mComboBox::GetItemCaption( INT index )const
{
	LRESULT result;	//結果の一時格納用
	
	//ラベルの文字列の長さを取得する
	result = ::SendMessageW( GetMyHwnd() , CB_GETLBTEXTLEN , index , 0 );
	if( result == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"CB_GETLBTEXTLEN failed" );
		return L"";
	}

	//格納用のバッファを作成
	WCHAR* str_buffer = mNew WCHAR[ result + 1 ];

	//バッファに文字列を取得する
	result = ::SendMessageW( GetMyHwnd() , CB_GETLBTEXT , index , (LPARAM)str_buffer );
	if( result == CB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"CB_GETLBTEXT failed" );
		mDelete[] str_buffer;
		return L"";
	}

	//取得した文字列からWStringオブジェクトを構築して結果にする
	WString result_string = str_buffer;
	mDelete[] str_buffer;
	return result_string;
}

//指定インデックスに関連づけられているデータを取得する
bool mComboBox::GetItemData( INT index , ItemDataEntry& retdata )const
{
	//指定されたインデックスのキャプションを取得
	WString caption = GetItemCaption( index );

	//取得したキャプションからデータを取得
	ItemData::const_iterator itr = MyItemData.find( caption );
	if( itr == MyItemData.end() )
	{
		//指定キャプションに関連づけられているデータはない
		return false;
	}
	retdata = itr->second;
	return true;
}

//指定インデックスに関連づけられているデータを取得する
const mListboxFamily::ItemDataEntry& mComboBox::GetItemData( INT index )const
{
	static const mListboxFamily::ItemDataEntry dummy;
	
	//指定されたインデックスのキャプションを取得
	WString caption = GetItemCaption( index );

	//取得したキャプションからデータを取得
	ItemData::const_iterator itr = MyItemData.find( caption );
	if( itr == MyItemData.end() )
	{
		//指定キャプションに関連づけられているデータはない
		return dummy;
	}
	return itr->second;
}

//現在選択されている項目に関連づけられているデータを取得する
const mListboxFamily::ItemDataEntry& mComboBox::GetItemData( void )const
{
	static const mListboxFamily::ItemDataEntry dummy;

	mComboBox::SelectedItems selected = GetSelected();
	if( selected.empty() )
	{
		return dummy;
	}
	return GetItemData( *selected.begin() );
}

//いくつアイテムがあるかをカウントする
INT mComboBox::GetItemCount( void ) const
{
	return (INT)::SendMessage( GetMyHwnd() , CB_GETCOUNT , 0 , 0 );
}

//アイテムを移動する
bool mComboBox::MoveItem( INT index , INT moveto )
{
	INT itemcount = GetItemCount();
	//・インデックスが2つ以下だと入れ替えできないのでエラー
	//・indexが負の数だったり、アイテムの数を超えている場合はエラー
	//・movetoが負の数だったり、アイテムの数を超えている場合はエラー
	if( ( itemcount < 2 ) ||
		( index  < 0 || itemcount <= index  ) ||
		( moveto < 0 || itemcount <= moveto ) )
	{
		RaiseAssert( g_ErrorLogger , ( index << 16 ) + moveto , L"" );
		return false;
	}
	//indexとmovetoが同じだったら入れ替え必要なし
	if( index == moveto )
	{
		return true;
	}
	//アイテムのキャプションを取得する
	WString caption = GetItemCaption( index );

	//現在の位置からアイテムを削除し、新しい位置にアイテムを挿入する
	//※MyItemDataはアイテムの位置が変わるだけだから変更は不要
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

