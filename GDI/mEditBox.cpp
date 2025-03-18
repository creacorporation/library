//----------------------------------------------------------------------------
// ウインドウ管理（エディットコントロール）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2025 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MEDITBOX_CPP_COMPILING
#include "mEditBox.h"
#include "mWindowCollection.h"
#include "General/mErrorLogger.h"

#include <CommCtrl.h>

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
		retSetting.Style |= ( op->HScrollBar != 0 ) ? ( ES_AUTOHSCROLL | WS_HSCROLL ) : ( 0 );
		//垂直スクロールバーを表示する
		retSetting.Style |= ( op->VScrollBar != 0 ) ? ( ES_AUTOVSCROLL | WS_VSCROLL ) : ( 0 );
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
bool mEditBox::GetText( WString& retText )const
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

WString mEditBox::GetText( void )const
{
	WString str;
	if( !GetText( str ) )
	{
		return L"";
	}
	return str;
}

//エディットコントロールの文字列の長さ（目安）を取得
DWORD mEditBox::GetTextLength( void )const
{
	return (DWORD)GetWindowTextLengthW( GetMyHwnd() );
}

DWORD mEditBox::GetLineCount( void )const
{
	return static_cast<DWORD>( MessageSend( EM_GETLINECOUNT , 0 , 0 ) );
}

DWORD mEditBox::GetFirstVisibleLine( void )const
{
	return static_cast<DWORD>( MessageSend( EM_GETFIRSTVISIBLELINE , 0 , 0 ) );
}

//指定した行の記載を取得する
bool mEditBox::GetLine( DWORD Line , WString& retText )const
{
	//指定した行の最初の文字のインデックス
	DWORD pos = static_cast<DWORD>( MessageSend( EM_LINEINDEX , Line , 0 ) );
	DWORD len = static_cast<DWORD>( MessageSend( EM_LINELENGTH , pos , 0 ) );
	
	if( std::numeric_limits<wchar_t>::max() < len )
	{
		return false;
	}

	std::unique_ptr<wchar_t> p( mNew wchar_t[ len + 1 ] );
	p.get()[ 0 ] = static_cast<wchar_t>( len );
	DWORD readlen = static_cast<DWORD>( MessageSend( EM_GETLINE , Line , (LPARAM)p.get() ) );
	if( !readlen )
	{
		return false;
	}

	p.get()[ readlen ] = 0;
	retText = p.get();
	return true;
}


//指定した行の記載を取得する
WString mEditBox::GetLine( DWORD Line )const
{
	WString str;
	if( !GetLine( Line , str ) )
	{
		return L"";
	}
	return str;
}

//指定した行が一番上になるようにスクロールする
bool mEditBox::Scroll( DWORD Line )const
{
	//現在の行位置を取得する
	DWORD current = GetFirstVisibleLine();

	//目的の行とのオフセットを取得
	INT diff = Line - current;

	//差がないなら何もしない
	if( diff == 0 )
	{
		return true;
	}

	//行移動
	return ScrollOffset( diff );
}

//指定した行数分上下にスクロールする
bool mEditBox::ScrollOffset( INT Line )const
{
	return MessageSend( EM_LINESCROLL , 0 , (LPARAM)Line );
}

bool mEditBox::ScrollToCaret( void )const
{
	return MessageSend( EM_SCROLLCARET , 0 , 0 );
}

bool mEditBox::GetCaretPos( DWORD* retLine , DWORD* retCol , DWORD* retPos )const
{
	//キャレットがある文字位置を取得する

	if( !retPos && !retLine && !retCol )
	{
		return true;
	}
	DWORD pos = static_cast<DWORD>( MessageSend( EM_GETCARETINDEX , 0 , 0 ) );
	if( retPos )
	{
		*retPos = pos;
	}
	if( retLine )
	{
		*retLine = static_cast<DWORD>( MessageSend( EM_LINEFROMCHAR , (WPARAM)pos , 0 ) );
	}
	if( retCol )
	{
		DWORD linehead = static_cast<DWORD>( MessageSend( EM_LINEINDEX , (WPARAM)-1 , 0 ) );
		*retCol = pos - linehead;
	}
	return true;
}

//キャレットの位置を設定します
// Line : 行番号
// Col  : 行内の水平位置
bool mEditBox::SetCaretPos( DWORD Line , DWORD Col )const
{
	DWORD linehead = static_cast<DWORD>( MessageSend( EM_LINEINDEX , (WPARAM)Line , 0 ) );
	return SetCaretPos( linehead + Col );
}

//キャレットの位置を設定します
// Pos  : バッファ全体でのオフセット
bool mEditBox::SetCaretPos( DWORD Pos )const
{
//	return MessageSend( EM_SETCARETINDEX , (WPARAM)Pos , 0 );
	return MessageSend( EM_SETSEL , (WPARAM)Pos , (LPARAM)Pos );
}


