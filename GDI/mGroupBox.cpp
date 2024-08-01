//----------------------------------------------------------------------------
// ウインドウ管理（グループボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGROUPBOX_CPP_COMPINLING
#include "mGroupBox.h"
#include "mWindowCollection.h"

mGroupBox::mGroupBox()
{
}

mGroupBox::~mGroupBox()
{
}

LRESULT mGroupBox::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( MyChild != nullptr )
	{
		switch( msg )
		{
		case WM_SIZE:
			//子オブジェクトが存在すれば、それらにサイズを伝播します
			MyChild->AdjustSize( GetMyHwnd() );
			break;
		default:
			break;
		}
	}
	return __super::WindowProcedure( msg , wparam , lparam );
}


//ウインドウクラスの登録をする
bool mGroupBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mGroupBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"BUTTON";

	//オプションの指定がなければそのまま戻る(デフォルトになる)
	if( opt == nullptr )
	{
		return true;
	}
	//オプションの指定があれば、それに従う
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		const mGroupBox::Option_UseOption* op = (const mGroupBox::Option_UseOption*)opt;
		retSetting.Style |= BS_GROUPBOX;
		//キャプション
		retSetting.WindowName = op->Caption;
		//位置とサイズ
		SetWindowPosition( op->Pos );
	}
	return true;
}

