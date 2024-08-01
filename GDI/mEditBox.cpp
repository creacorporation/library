//----------------------------------------------------------------------------
// ウインドウ管理（エディットコントロール）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MEDITBOX_CPP_COMPILING
#include "mEditBox.h"
#include "mWindowCollection.h"
#include "General/mErrorLogger.h"

mEditBox::mEditBox()
{
	MyRedirectEnter = false;
}

mEditBox::~mEditBox()
{
}

//ウインドウクラスの登録をする
bool mEditBox::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mEditBox::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = L"EDIT";
	//オプション指定がない場合はそのまま戻る
	if( opt == nullptr )
	{
		return true;
	}

	//オプションの指定があれば、その種類により生成方法を分岐
	if( ((const Option*)(opt))->method == Option::CreateMethod::USEOPTION )
	{
		const mEditBox::Option_UseOption* op = (const mEditBox::Option_UseOption*)opt;

		//設定する文字列
		retSetting.WindowName = op->Text;
		//複数行タイプ
		retSetting.Style |= ( op->Multiline != 0 ) ? ( ES_MULTILINE ) : ( 0 );
		//水平スクロールバーを表示する
		retSetting.Style |= ( op->HScrollBar != 0 ) ? ( ES_AUTOHSCROLL ) : ( 0 );
		//垂直スクロールバーを表示する
		retSetting.Style |= ( op->VScrollBar != 0 ) ? ( ES_AUTOVSCROLL ) : ( 0 );
		//パスワードタイプ
		retSetting.Style |= ( op->Password != 0 ) ? ( ES_PASSWORD ) : ( 0 );
		//リードオンリー
		retSetting.Style |= ( op->Readonly != 0 ) ? ( ES_READONLY ) : ( 0 );
		//数字のみ
		retSetting.Style |= ( op->Number != 0 ) ? ( ES_NUMBER ) : ( 0 );
		//エンターキーのリダイレクト
		MyRedirectEnter = op->RedirectEnter != 0;

		//文字列の配置
		switch( op->Justify ) 
		{
		case Option::TextJustify::LEFT:
			retSetting.Style |= ES_LEFT;
			break;
		case Option::TextJustify::CENTER:
			retSetting.Style |= ES_CENTER;
			break;
		case Option::TextJustify::RIGHT:
			retSetting.Style |= ES_RIGHT;
			break;
		default:
			break;
		}
		//文字列の大文字・小文字を自動変換する
		switch( op->Case )
		{
		case Option::TextCase::NOCHANGE:
			break;
		case Option::TextCase::LOWERCASE:
			retSetting.Style |= ES_LOWERCASE;
			break;
		case Option::TextCase::UPPERCASE:
			retSetting.Style |= ES_UPPERCASE;
			break;
		default:
			break;
		}
		//配置
		SetWindowPosition( op->Pos );
	}

	//プロシージャの差し替えが必要であればフラグをセット
	if( MyRedirectEnter )
	{
		retSetting.ProcedureChange = true;
	}
	return true;
}

//ウインドウプロシージャ
LRESULT mEditBox::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( msg == WM_CHAR )
	{
		//エンターキーをリダイレクトするか？
		if( ( MyRedirectEnter ) && ( wparam == VK_RETURN ) )
		{
			PostMessageW( GetMyParent() , msg , wparam , lparam );
		}
	}
	return __super::WindowProcedure( msg , wparam , lparam );
}

//エディットコントロールの文字列をセット
bool mEditBox::SetText( const WString& NewText )
{
	return SetWindowTextW( GetMyHwnd() , NewText.c_str() );
}

//エディットコントロールの文字列を取得
bool mEditBox::GetText( WString& retText )
{
	wchar_t* str = 0;
	DWORD len = GetTextLength();

	retText.clear();

	if( len )
	{
		str = mNew wchar_t[ len + 1 ];
		if( !str )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"mNewが失敗しました" );
			return false;
		}
		if( GetWindowTextW( GetMyHwnd() , str , len + 1 ) )
		{
			retText = str; 
		}
	}
	mDelete[] str;
	return true;
}

//エディットコントロールの文字列の長さ（目安）を取得
DWORD mEditBox::GetTextLength( void )const
{
	return (DWORD)GetWindowTextLengthW( GetMyHwnd() );
}




