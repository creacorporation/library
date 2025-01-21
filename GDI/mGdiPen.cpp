//----------------------------------------------------------------------------
// ウインドウ管理（GDIペン）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIPEN_CPP_COMPILING
#include "mGdiPen.h"
#include "mGdiUtil.h"
#include "General/mErrorLogger.h"

mGdiPen::mGdiPen( const Option* option )noexcept( false )
{
	//オプションがヌルポインタだった場合、黒ペンになります。
	if( option == nullptr )
	{
		MyHandle = (HPEN)::GetStockObject( BLACK_PEN );
		return;
	}
	//生成方法に従ってペンを作成します
	if( option->method == Option::CreateMethod::USEOPTION )
	{
		//オプション構造体を使う場合
		const Option_UseOption* opt = (const Option_UseOption*)option;
		if( !CreateHandle( *opt ) )
		{
			throw EXCEPTION( opt->kind , L"CreateHandle Failed" );
		}
	}
	else if( option->method == Option::CreateMethod::TRANSPARENT_PEN )
	{
		const Option_Transparent* opt = (const Option_Transparent*)option;
		if( !CreateHandle( *opt ) )
		{
			throw EXCEPTION( 0 , L"CreateHandle Failed" );
		}
	}
	else
	{
		//生成方法が不明
		throw EXCEPTION( option->method , L"Unknown create method" );
	}
	return;
}

mGdiPen::~mGdiPen()
{
	::DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//ハンドルの値を取得する(キャスト演算子バージョン)
mGdiPen::operator HPEN()const
{
	return MyHandle;
}

//ハンドルの値を取得する(普通の関数バージョン)
HGDIOBJ mGdiPen::GetHandle( void )const
{
	return MyHandle;
}

//ペン生成
bool mGdiPen::CreateHandle( const Option_UseOption& opt )
{
	//ペンの種類
	INT pen_kind = PS_NULL;
	switch( opt.kind )
	{
	case Option::PenKind::SOLID_PEN:		//実線のペン
		pen_kind = PS_SOLID;
		break;
	case Option::PenKind::DASH_PEN:			//破線のペン
		pen_kind = PS_DASH;
		break;
	case Option::PenKind::DOT_PEN:			//点線のペン
		pen_kind = PS_DOT;
		break;
	case Option::PenKind::DASHDOT_PEN:		//一点鎖ペン
		pen_kind = PS_DASHDOT;
		break;
	case Option::PenKind::DASHDOTDOT_PEN:	//二点鎖ペン
		pen_kind = PS_DASHDOTDOT;
		break;
	default:
		break;
	}
	//色の設定
	RGBQUAD rgb_color = ::COLORREF2RGBQUAD( opt.color );

	//ハンドルの作成
	MyHandle = ::CreatePen( pen_kind , opt.width , ::RGBQUAD2COLORREF( rgb_color ) );

	return MyHandle != nullptr;
}

bool mGdiPen::CreateHandle( const Option_Transparent& opt )
{
	MyHandle = ::CreatePen( PS_NULL , 1 , 0 );
	return MyHandle != nullptr;
}


