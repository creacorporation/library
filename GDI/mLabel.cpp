//----------------------------------------------------------------------------
// ウインドウ管理（ラベル）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MLABEL_CPP_COMPILING
#include "mLabel.h"
#include "mWindowCollection.h"
#include "General/mErrorLogger.h"

mLabel::mLabel()
{
}

mLabel::~mLabel()
{
}

//ウインドウクラスの登録をする
bool mLabel::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mLabel::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"STATIC";
	//オプションの指定がなければそのまま戻る
	if( opt == nullptr )
	{
		return true;
	}

	//オプションの指定があればそれに従う
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		const mLabel::Option_UseOption* op = (const mLabel::Option_UseOption*)opt;

		//設定する文字列
		retSetting.WindowName = op->Caption;
		//アンパサンドの扱い
		retSetting.Style |= ( op->NoPrefix ) ? ( SS_NOPREFIX ) : ( 0 );
		//文字の配置
		switch( op->Justify )
		{
		case Option::TextJustify::LEFT:
			retSetting.Style |= SS_LEFT;
			break;
		case Option::TextJustify::CENTER:
			retSetting.Style |= SS_CENTER;
			break;
		case Option::TextJustify::RIGHT:
			retSetting.Style |= SS_RIGHT;
			break;
		case Option::TextJustify::LEFTNOWORDWRAP:
			retSetting.Style |= SS_LEFTNOWORDWRAP;
			break;
		default:
			RaiseAssert( g_ErrorLogger , op->Justify , L"" );
			break;
		}
		//配置
		SetWindowPosition( op->Pos );
	}
	return true;
}

