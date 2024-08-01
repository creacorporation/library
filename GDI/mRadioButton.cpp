//----------------------------------------------------------------------------
// ウインドウ管理（チェックボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MRADIOBUTTON_CPP_COMPINLING
#include "mRadioButton.h"
#include "General/mErrorLogger.h"

mRadioButton::mRadioButton()
{
}

mRadioButton::~mRadioButton()
{
}

//ウインドウクラスの登録をする
bool mRadioButton::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	const mRadioButton::Option* Option = (const mRadioButton::Option*)opt;
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mRadioButton::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";

	//オプション指定がない場合はそのまま戻る
	if( opt == nullptr )
	{
		return true;
	}

	//オプションの指定があれば、その種類により生成方法を決める
	if( ( (const Option*)opt )->method == Option::CreateMethod::USEOPTION )
	{
		const mRadioButton::Option_UseOption* op = ( const mRadioButton::Option_UseOption* )opt;
		//ウインドウのキャプション
		retSetting.WindowName = op->Caption;
		//自動ラジオボタンにする
		retSetting.Style |= BS_AUTORADIOBUTTON;
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

