//----------------------------------------------------------------------------
// ウインドウ管理（チェックボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MCHECKBOX_CPP_COMPILING
#include "mCheckBox.h"

mCheckBox::mCheckBox()
{
}

mCheckBox::~mCheckBox()
{
}

//ウインドウクラスの登録をする
bool mCheckBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	const mCheckBox::Option* Option = (const mCheckBox::Option*)opt;
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mCheckBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";
	//オプション指定がない場合はそのまま戻る
	if( opt == nullptr )
	{
		return true;
	}
	//オプションの内容により分岐
	if( ( (const Option*)( opt ) )->method == Option::CreateMethod::USEOPTION )
	{
		const mCheckBox::Option_UseOption* op = ( const mCheckBox::Option_UseOption* )opt;
		//ウインドウのキャプション
		retSetting.WindowName = op->Caption;
		//ボタンのスタイル
		if( op->ThreeState )
		{
			retSetting.Style |= ( BS_3STATE | BS_AUTO3STATE );
		}
		else
		{
			retSetting.Style |= ( BS_CHECKBOX | BS_AUTOCHECKBOX );
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


