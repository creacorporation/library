//----------------------------------------------------------------------------
// ウインドウ管理（リストボックス）
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

//ウインドウクラスの登録をする
bool mListBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	//既存の物を使用するのでfalseを返すのみ。
	return false;
}

//ウインドウを開く
bool mListBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"LISTBOX";
	retSetting.Style |= LBS_NOTIFY;
	
	//オプション指定がない場合はそのまま戻る(デフォルトのオブジェクトを生成)
	if( opt == nullptr )
	{
		return true;
	}

	//オプション指定があれば、それに従う
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		const mListBox::Option_UseOption* op = (const mListBox::Option_UseOption*)opt;
		//有効・無効の切り替え
		retSetting.Style |= ( op->Enable != 0 ) ? ( 0 ) : ( WS_DISABLED );
		//枠あり？
		retSetting.Style |= ( op->Border != 0 ) ? ( WS_BORDER ) : ( 0 );
		//スクロールバーあり？
		retSetting.Style |= ( op->ScrollBar != 0 ) ? ( WS_VSCROLL ) : ( 0 );
		//自動ソート
		retSetting.Style |= ( op->AutoSort ) ? ( LBS_SORT ) : ( 0 );
		//複数選択タイプ？
		retSetting.Style |= ( op->Multiple ) ? ( LBS_MULTIPLESEL ) : ( 0 );
		//位置とサイズ
		SetWindowPosition( op->Pos );
	}
	return true;
}

//アイテムを追加する
bool mListBox::AddItem( const WString& caption )
{
	//項目追加
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
	//項目追加
	if( !AddItem( caption ) )
	{
		return false;
	}

	//キャプションとデータの関連づけ
	MyItemData[ caption ] = data;
	return true;
}

bool mListBox::AddItem( const WString& caption , INT index )
{
	//項目追加
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
bool mListBox::RemoveItem( INT index )
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
mListBox::SelectedItems mListBox::GetSelected( void )const
{
	//選択されている項目の数を取得する
	LRESULT selcount = ::SendMessageW( GetMyHwnd() , LB_GETSELCOUNT , 0 , 0 );

	//項目の数にLB_ERRが返ってくる場合は、1個だけ選択できるリストボックス
	//そうでなければ、その数だけ選択されている
	if( selcount == LB_ERR )
	{
		//1個しか選択できないリストボックスの場合、LB_GETCURSELで選択されている項目が取得できる。
		//この場合、何も選択されていなければLB_ERRが返る。
		LRESULT result = ::SendMessageW( GetMyHwnd() , LB_GETCURSEL , 0 , 0 );
		if( result == LB_ERR )
		{
			//何も選択されていない
			return SelectedItems();
		}
		//選択されている項目を追加
		SelectedItems items;
		items.push_back( (INT)result );
		return items;
	}
	else
	{
		//複数選択できるリストボックスでは、LB_GETSELITEMSを使って選択されているアイテムを取得する
		//LB_GETSELITEMSのLPARAMはx64でもINTの配列で良い模様
		INT* selected = mNew INT[ selcount ];

		//選択されている項目の一覧を取得
		LRESULT result = ::SendMessageW( GetMyHwnd() , LB_GETSELITEMS , selcount , (LPARAM)selected );
		if( result == LB_ERR )
		{
			mDelete[] selected;
			return SelectedItems();
		}

		//配列をベクトルに置き換え
		SelectedItems items;
		for( LONG_PTR i = 0 ; i < selcount ; i++ )
		{
			items.push_back( selected[ i ] );
		}
		mDelete[] selected;
		return items;
	}
}

//指定したインデックスのアイテムを選択状態にする
bool mListBox::SetSelected( const SelectedItems& items )
{
	//選択されている項目の数を取得する
	if( ::SendMessageW( GetMyHwnd() , LB_GETSELCOUNT , 0 , 0 ) == LB_ERR )
	{
		//1個しか選択できないリストボックスの場合、
		//複数個あっても処理できないのでエラーにする
		if( items.size() != 1 )
		{
			RaiseAssert( g_ErrorLogger , items.size() , L"Bad item number" );
			return false;
		}
	}

	//全選択を一旦解除する
	if( ::SendMessageW( GetMyHwnd() , LB_SETSEL , 0 , -1 ) == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , -1 , L"LB_SETSEL failed" );
		return false;
	}

	//指定されたインデックスに対し、順次選択状態にする
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

//指定したインデックスのアイテムを選択状態にする
bool mListBox::SetSelected( INT item )
{
	return ::SendMessageW( GetMyHwnd() , LB_SETSEL , 1 , item ) != LB_ERR;
}

//指定したキャプションをもつアイテムのインデックスを返す
INT mListBox::SearchItem( const WString& caption )
{
	LRESULT result = ::SendMessageW( GetMyHwnd() , LB_FINDSTRINGEXACT , 0 , (LPARAM)caption.c_str() );
	if( result == LB_ERR )
	{
		//指定キャプションがないのは想定されているから、エラーの記録はなし。
		return -1;
	}
	return (INT)result;
}

//指定インデックスのキャプションを取得する
WString mListBox::GetItemCaption( INT index )const
{
	LRESULT result;	//結果の一時格納用
	
	//ラベルの文字列の長さを取得する
	result = ::SendMessageW( GetMyHwnd() , LB_GETTEXTLEN , index , 0 );
	if( result == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"LB_GETTEXTLEN failed" );
		return L"";
	}

	//格納用のバッファを作成
	WCHAR* str_buffer = mNew WCHAR[ (size_t)result ];

	//バッファに文字列を取得する
	result = ::SendMessageW( GetMyHwnd() , LB_GETTEXT , index , (LPARAM)str_buffer );
	if( result == LB_ERR )
	{
		RaiseAssert( g_ErrorLogger , index , L"LB_GETTEXT failed" );
		mDelete[] str_buffer;
		return L"";
	}

	//取得した文字列からWStringオブジェクトを構築して結果にする
	WString result_string = str_buffer;
	mDelete[] str_buffer;
	return result_string;
}

//指定インデックスに関連づけられているデータを取得する
bool mListBox::GetItemData( INT index , ItemDataEntry& retdata )const
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

//いくつアイテムがあるかをカウントする
INT mListBox::GetItemCount( void )const
{
	return (INT)::SendMessage( GetMyHwnd() , LB_GETCOUNT , 0 , 0 );
}

//アイテムを移動する
bool mListBox::MoveItem( INT index , INT moveto )
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

