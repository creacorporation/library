//----------------------------------------------------------------------------
// ウインドウ管理（ボタン）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MBUTTON_CPP_COMPINLING
#include "mButton.h"

mButton::mButton()
{
}

mButton::~mButton()
{
}

//ウインドウクラスの登録をする
bool mButton::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mButton::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";
	//オプション指定がない場合はそのまま戻る
	if( opt == nullptr )
	{
		return true;
	}

	//オプションの指定があれば、その種類により生成方法を分岐
	if( ( (const Option*)( opt ) )->method == Option::CreateMethod::USEOPTION )
	{
		const mButton::Option_UseOption* op = ( const mButton::Option_UseOption* )opt;

		//ウインドウのキャプション
		retSetting.WindowName = op->Caption;
		
		//オルタネイト
		if( op->Alternate )
		{
			retSetting.Style |= BS_PUSHLIKE | BS_AUTOCHECKBOX;
		}
		//有効・無効
		if( !op->Enable )
		{
			retSetting.Style |= WS_DISABLED;
		}
		//位置とサイズ
		SetWindowPosition( op->Pos );
	}
	return true;
}

bool mButton::IsChecked( void )const
{
	LRESULT state = MessageSend( BM_GETCHECK , 0 , 0 );
	return state & BST_CHECKED;
}


//オルタネイトのボタン(ボタンを押した後に離してもその状態を保持するボタン)のチェック状態を設定する
bool mButton::SetCheck( bool check )const
{
	if( check )
	{
		MessageSend( BM_SETCHECK , BST_CHECKED , 0 );
	}
	else
	{
		MessageSend( BM_SETCHECK , BST_UNCHECKED , 0 );
	}
	return true;
}

