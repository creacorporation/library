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

