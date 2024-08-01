//----------------------------------------------------------------------------
// ウインドウ管理（GDIビットマップ）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIBITMAP_CPP_COMPILING
#include "mGdiBitmap.h"
#include "General/mErrorLogger.h"
#include "mGdiMemDC.h"

mGdiBitmap::mGdiBitmap( const Option* opt )throw( mException )
{
	MyHandle = nullptr;

	if( opt != nullptr )
	{
		//作成するビットマップの種類に応じた処理
		if( opt->method == Option::CreateMethod::NEWBITMAP )
		{
			//デバイスコンテキストから生成する場合
			const Option_NewBitmap* opt_bitmap = (const Option_NewBitmap*)opt;
			CreateHandle( *opt_bitmap );
		}
		else if( opt->method == Option::CreateMethod::LOADFILE )
		{
			//ファイルを読み込む場合
			const Option_LoadFile* opt_loadfile = (const Option_LoadFile*)opt;
			CreateHandle( *opt_loadfile );
		}
		else if( opt->method == Option::CreateMethod::RESOURCE )
		{
			//ファイルを読み込む場合
			const Option_Resource* opt_resource = (const Option_Resource*)opt;
			CreateHandle( *opt_resource );
		}
		else
		{
			throw EXCEPTION( opt->method , L"Unknown create method" );
		}
	}

	//ビットマップができてなかったらエラーにする
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"Creating bitmap failed" );
	}
}

mGdiBitmap::~mGdiBitmap()
{
	//ハンドルの解放
	DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//ハンドルの値を取得する(キャスト演算子バージョン)
mGdiBitmap::operator HBITMAP()const
{
	return MyHandle;
}

//ハンドルの値を取得する(普通の関数バージョン)
HGDIOBJ mGdiBitmap::GetHandle( void )const
{
	return MyHandle;
}

//ビットマップの生成
bool mGdiBitmap::CreateHandle( const Option_NewBitmap& opt )
{
	if( opt.srcdc == nullptr )
	{
		//srcdcがnullptrの場合は、デスクトップと互換のビットマップを作る
		HWND hwnd = ::GetDesktopWindow();
		HDC hdc = ::GetDC( hwnd );
		MyHandle = ::CreateCompatibleBitmap( hdc , opt.width , opt.height );
		::ReleaseDC( hwnd , hdc );
		//GetDesktopWindowで取得したハンドルは解放しなくていい
	}
	else
	{
		//全部指定があるので、それを元に作る
		MyHandle = ::CreateCompatibleBitmap( opt.srcdc->MyHdc , opt.width , opt.height );
	}
	return ( MyHandle != nullptr );
}

//ビットマップの生成
bool mGdiBitmap::CreateHandle( const Option_LoadFile& opt )
{

	MyHandle = (HBITMAP)::LoadImageW( 
		nullptr ,			//インスタンスハンドル。リソース読み込むわけじゃないからヌル
		opt.path.c_str() ,		//読み込むファイル名
		IMAGE_BITMAP ,		//ロードするイメージのタイプ（ビットマップ）
		0 ,					//幅。0の場合、イメージサイズに合わせる。
		0 ,					//高さ。0の場合、イメージサイズに合わせる。
		LR_LOADFROMFILE );	//ファイルから読み込むときに使うフラグ

	return ( MyHandle != nullptr );
}

//ビットマップの生成
bool mGdiBitmap::CreateHandle( const Option_Resource& opt )
{
	//このモジュールのハンドル取得
	HINSTANCE module = ::GetModuleHandleW( nullptr );	//解放しなくて良い

	MyHandle = (HBITMAP)::LoadImageW( 
		module ,			//インスタンスハンドル。
		opt.name.c_str() ,		//読み込むリソース名
		IMAGE_BITMAP ,		//ロードするイメージのタイプ（ビットマップ）
		0 ,					//幅。0の場合、イメージサイズに合わせる。
		0 ,					//高さ。0の場合、イメージサイズに合わせる。
		LR_DEFAULTCOLOR );	//モノクロではない場合に指定。

	return ( MyHandle != nullptr );
}

bool mGdiBitmap::GetSize( SIZE& retSize )const noexcept
{
	BITMAP bmp;
	if( !GetInfo( bmp ) )
	{
		return false;
	}
	retSize.cx = bmp.bmWidth;
	retSize.cy = bmp.bmHeight;
	return true;

}

bool mGdiBitmap::GetInfo( BITMAP& retInfo )const noexcept
{
	if( !GetObject( MyHandle , sizeof( BITMAP ) , &retInfo ) )
	{
		return false;
	}
	return true;
}


