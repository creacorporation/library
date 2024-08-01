//----------------------------------------------------------------------------
// ウインドウ管理（ツールバー）
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
	//すぐにインスタンスを作る訳だが、とりあえずはnullptrに初期化しておく。
	MyImgage = nullptr;
}

mToolBar::~mToolBar()
{
	mDelete MyImgage;
}

//ウインドウクラスの登録をする
bool mToolBar::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	::InitCommonControls();
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mToolBar::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = TOOLBARCLASSNAMEW;
	return true;
}

bool mToolBar::OnCreate( const void* opt )
{
	//TBBUTTON構造体のサイズを設定
	::SendMessageW( GetMyHwnd() , TB_BUTTONSTRUCTSIZE , sizeof( TBBUTTON ) , 0 );


	//オプション指定あり？
	if( opt != nullptr )
	{
		//オプションの指定がある場合、それに沿った設定をする

		//オプション通りのイメージリストを生成
		const mToolBar::Option_UseOption* op = (const mToolBar::Option_UseOption*)opt;
		MyImgage = mNew mGdiDualImagelist( &op->ImgOpt );

		//フラットスタイルにする？
		LPARAM style = ::SendMessageW( GetMyHwnd() , TB_GETSTYLE , 0 , 0 );
		if( op->IsFlat )
		{
			style |= TBSTYLE_FLAT;
		}
		//リストスタイルにする？
		if( op->IsListStyle )
		{
			style |= TBSTYLE_LIST;
		}
		//ツールチップを使う？
		if( op->IsTooltipReq )
		{
			style |= TBSTYLE_TOOLTIPS;
		}
		::SendMessageW( GetMyHwnd() , TB_SETSTYLE , 0 , style );
	}
	else
	{
		//optがヌルだった場合、デフォルトで作成。
		//ファクトリメソッドでoptがヌルだった場合をはじいているので、
		//今のところはここに来ることはないが、
		//忘れたころに継承とかしてヌルで来るようになるかもしれないので。
		mGdiImagelist::Option_UseOption Option;
		MyImgage = mNew mGdiDualImagelist( &Option );
	}

	//ドロップダウン出来るように
	LPARAM exstyle = ::SendMessageW( GetMyHwnd() , TB_GETEXTENDEDSTYLE , 0 , 0 );
	exstyle |= TBSTYLE_EX_DRAWDDARROWS;	//右側に▼マークを出せるようにする
	::SendMessageW( GetMyHwnd() , TB_SETEXTENDEDSTYLE , 0 , exstyle );

	//イメージリストをツールバーに対して登録する。
	::SendMessageW( GetMyHwnd() , TB_SETIMAGELIST , 0 , (LPARAM)MyImgage->GetHandle() );
	::SendMessageW( GetMyHwnd() , TB_SETHOTIMAGELIST , 0 , (LPARAM)MyImgage->GetHandle2() );

	return true;
}

bool mToolBar::ExecUpdate( void )
{
	//まず、全アイテムを削除する
	ExecClear();
	MyInternalIdStock.Clear();

	//追加するアイテムの数は？
	size_t item_count = MyItemOption.size();
	if( item_count == 0 )
	{
		//追加する物がない場合は、それ以上することがないから戻る
		return true;
	}

	//ボタンの情報領域確保
	std::vector<TBBUTTON> button( item_count );

	//TBBUTTONの中身を埋める。
	for( size_t i = 0 ; i < item_count ; i++ )
	{
		//使用するアイコンのインデックスを取得するが、該当無しの場合はアイコンなしにする。
		INT img_index = MyImgage->GetIndex( MyItemOption[ i ].ImageId );
		button[ i ].iBitmap = ( 0 <= img_index ) ? ( img_index ) : ( I_IMAGENONE );

		//ボタンは有効
		button[ i ].fsState = TBSTATE_ENABLED;

		//ボタンのキャプション
		button[ i ].iString = (INT_PTR)MyItemOption[ i ].Caption.c_str();

		//以下２つは、ツールバーがクリックされたときにWM_NOTIFYのNMMOUSEから取得できる
		button[ i ].idCommand = (INT)i;						//(NMMOUSE::dwItemSpec)0ベースのインデックス
		button[ i ].dwData = MyItemOption[ i ].FunctionId;	//(NMMOUSE::dwItemData)FunctionID

		//スタイル。セパレータはAUTOSIZEは要らない。
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
	//全ボタンを削除する。全部まとめて消す方法が見当たらないので、
	//エラーになるまで、順次先頭のボタンを消していく。
	bool result;
	do
	{
		result = ( ::SendMessageW( GetMyHwnd() , TB_DELETEBUTTON , 0 , 0 ) != 0 );
	}while( result );

	return true;
}

//アイコンを有効・無効にする
bool mToolBar::SetState( UINT FunctionId , bool enable , bool checked )
{
	for( size_t i = 0 ; i < MyItemOption.size() ; i++ )
	{
		//FunctionIDが一致する場合は、
		if( MyItemOption[ i ].FunctionId == FunctionId )
		{
			//現在のステータスを取得
			LRESULT state = ::SendMessageW( GetMyHwnd() , TB_GETSTATE , i , 0 );
			//有効にする？
			if( enable )
			{
				state |= TBSTATE_ENABLED;
			}
			else
			{
				state &= ~TBSTATE_ENABLED;
			}
			//チェック状態にする？
			//※BTNS_CHECKでなくてもチェック状態には出来るが、ユーザーが押せなくなる。
			//  再度設定すれば元に戻るので、あえてエラーにはしない。
			if( checked )
			{
				state |= TBSTATE_CHECKED;
			}
			else
			{
				state &= ~TBSTATE_CHECKED;
			}
			//新ステータスを設定
			if( !::SendMessageW( GetMyHwnd() , TB_SETSTATE , i , LOWORD( state ) ) )
			{
				RaiseAssert( g_ErrorLogger , i , L"TB_SETSTATE failed" );
			}
		}
	}
	return true;
}

//アイテムの情報を取得する
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

//アイテムの情報を取得する
bool mToolBar::QueryItem( const LPNMMOUSE mouse , ItemOptionEntry& retInfo )const
{
	if( mouse == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Opt mouse is nullptr" );
		return false;
	}

	//dwItemSpecはこのクラスで設定していて、INTからはみ出るのはあり得ない
	//(値はTBBUTTONの要素数)ので、INTにキャストしてOK
	return QueryItem( (INT)mouse->dwItemSpec , retInfo );
}


//-------------------------------------------------------------
// ここから、ToolbarUpdateHandleの実装
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

//ツールバーのアイコンにするイメージリストを取得
mGdiDualImagelist& mToolBar::ToolbarUpdateHandle::Image()const
{
	return *MyImgage;
}

mToolBar::ItemOption& mToolBar::ToolbarUpdateHandle::Item()const
{
	return *MyItemOption;
}

