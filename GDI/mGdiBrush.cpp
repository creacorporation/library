//----------------------------------------------------------------------------
// ウインドウ管理（GDIブラシ）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIBRUSH_CPP_COMPILING
#include "mGdiBrush.h"
#include "mGdiUtil.h"
#include "General/mErrorLogger.h"

mGdiBrush::mGdiBrush( const Option* opt )throw( mException )
{
	//オプションがヌルポインタだった場合、白のソリッドブラシになります。
	if( opt == nullptr )
	{
		MyHandle = (HBRUSH)::GetStockObject( WHITE_BRUSH );
		return;
	}

	//オプションに指定されたようなブラシを生成します。
	if( opt->kind == Option::BrushKind::SOLID_BRUSH )
	{
		//ソリッドブラシの生成をします。
		const Option_Solid* opt_solid = (const Option_Solid*)opt;
		if( !CreateSolidBrush( *opt_solid ) )
		{
			throw EXCEPTION( opt_solid->color , L"CreateSolidBrush failed" );
		}
		return;
	}
	else if( opt->kind == Option::BrushKind::HATCH_BRUSH )
	{
		//ハッチブラシの作成をします
		const Option_Hatch* opt_hatch = (const Option_Hatch*)opt;
		if( !this->CreateHatchBrush( *opt_hatch ) )	//←WinAPIに同名の関数があるのでthis修飾しています
		{
			throw EXCEPTION( opt_hatch->style , L"CreateHatchBrush failed" );
		}
		return;
	}
	else if( opt->kind == Option::BrushKind::PATTERN_BRUSH )
	{
		//パターンブラシの作成をします
		const Option_Pattern* opt_pat = (const Option_Pattern*)opt;
		if( !this->CreatePatternBrush( *opt_pat ) )	//←WinAPIに同名の関数があるのでthis修飾しています
		{
			throw EXCEPTION( 0 , L"CreatePatternBrush failed" );
		}
		return;
	}
	else if( opt->kind == Option::BrushKind::TRANSPARENT_BRUSH )
	{
		MyHandle = (HBRUSH)::GetStockObject( NULL_BRUSH );
		return;
	}

	//対応していない種別なので例外を投げます。
	MyHandle = nullptr;
	throw EXCEPTION( opt->kind , L"Invalid brush kind" );
}

mGdiBrush::~mGdiBrush()
{
	//ハンドルの解放
	//※GetStockObjectで作ったハンドルも解放してしまって問題は無い。
	DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//ソリッドブラシを作成します。成功すればMyHandleに値が入ります。
bool mGdiBrush::CreateSolidBrush( const Option_Solid& opt )
{
	//ハンドルの作成
	MyHandle = ::CreateSolidBrush( opt.color );
	return ( MyHandle != nullptr );
}

//ハッチブラシを作成します。成功すればMyHandleに値が入ります。
bool mGdiBrush::CreateHatchBrush( const Option_Hatch& opt )
{

	INT hatch_type;
	switch( opt.style )
	{
	case Option_Hatch::HatchStyle::UP_DIAGONAL:		//右上がりの斜線
		hatch_type = HS_FDIAGONAL;
		break;
	case Option_Hatch::HatchStyle::DOWN_DIAGONAL:	//右下がりの斜線
		hatch_type = HS_BDIAGONAL;
		break;
	case Option_Hatch::HatchStyle::CROSS_DIAGONAL:	//斜線のクロスハッチ
		hatch_type = HS_DIAGCROSS;
		break;
	case Option_Hatch::HatchStyle::HORIZONTAL:		//水平線のハッチ
		hatch_type = HS_HORIZONTAL;
		break;
	case Option_Hatch::HatchStyle::VERTICAL:		//垂直線のハッチ
		hatch_type = HS_VERTICAL;
		break;
	case Option_Hatch::HatchStyle::CROSS:			//十字のハッチ
		hatch_type = HS_CROSS;
		break;
	default:
		//変なのが来た場合はエラー終了する
		return false;
	}

	//色の設定
	RGBQUAD rgb_color = ::COLORREF2RGBQUAD( opt.color );

	//ハンドルの作成。失敗時はnullptrなので、nullptrとの比較結果を返します。
	MyHandle = ::CreateHatchBrush( hatch_type , ::RGBQUAD2COLORREF( rgb_color ) );
	return ( MyHandle != nullptr );
}

//パターンブラシを作成します。成功すればMyHandleに値が入ります。
bool mGdiBrush::CreatePatternBrush( const Option_Pattern& opt )
{
	//TODO:ビットマップの実装がまだできていないから、後で作る。
	return false;
}

//ハンドルの値を取得する(キャスト演算子バージョン)
mGdiBrush::operator HBRUSH()const
{
	return MyHandle;
}

//ハンドルの値を取得する(普通の関数バージョン)
HGDIOBJ mGdiBrush::GetHandle( void )const
{
	return MyHandle;
}

