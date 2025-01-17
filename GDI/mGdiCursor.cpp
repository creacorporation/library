//----------------------------------------------------------------------------
// ウインドウ管理（カーソル）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#define MGDICURSOR_CPP_COMPILING
#include "mGdiCursor.h"
#include "General/mErrorLogger.h"

mGdiCursor::mGdiCursor( const Option* opt )noexcept( false )
{
	MyHandle = nullptr;

	if( opt != nullptr )
	{
		//作成するカーソルの作成方法に応じた処理
		if( opt->method == Option::CreateMethod::LOADFILE )
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

	//カーソルができてなかったらエラーにする
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"Creating cursor failed" );
	}
}

mGdiCursor::~mGdiCursor()
{
	//ハンドルの解放
	::DestroyCursor( MyHandle );
	MyHandle = nullptr;
}

//ハンドルの値を取得する(キャスト演算子バージョン)
mGdiCursor::operator HCURSOR()const
{
	return MyHandle;
}

//ハンドルの値を取得する(普通の関数バージョン)
HGDIOBJ mGdiCursor::GetHandle( void )const
{
	return MyHandle;
}

//カーソルの生成
bool mGdiCursor::CreateHandle( const Option_LoadFile& opt )
{
	//ハンドルの作成
	MyHandle = (HCURSOR)::LoadImageW( 
		nullptr ,			//インスタンスハンドル。リソース読み込むわけじゃないからヌル
		opt.path.c_str() ,		//読み込むファイル名
		IMAGE_CURSOR ,		//ロードするイメージのタイプ（カーソル）
		0 ,					//幅。0の場合、イメージサイズに合わせる。
		0 ,					//高さ。0の場合、イメージサイズに合わせる。
		LR_LOADFROMFILE );	//ファイルから読み込むときに使うフラグ

	return ( MyHandle != nullptr );
}

//カーソルの生成
bool mGdiCursor::CreateHandle( const Option_Resource& opt )
{
	//このモジュールのハンドル取得
	HINSTANCE module = ::GetModuleHandleW( nullptr );	//解放しなくて良い

	//ハンドルの作成
	MyHandle = (HCURSOR)::LoadImageW( 
		module ,			//インスタンスハンドル。
		opt.name.c_str() ,		//読み込むリソース名
		IMAGE_CURSOR ,		//ロードするイメージのタイプ（カーソル）
		0 ,					//幅。0の場合、イメージサイズに合わせる。
		0 ,					//高さ。0の場合、イメージサイズに合わせる。
		LR_DEFAULTCOLOR );	//モノクロではない場合に指定。

	return ( MyHandle != nullptr );
}
