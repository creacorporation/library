//----------------------------------------------------------------------------
// ウインドウ管理（GDIビットマップ）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDIのビットマップです。
・作成するビットマップは、Option構造体で指定します。
*/

/*
●使い方
[例1]test.bmpを読み込んで、resourceに登録します。
mGdiResource resource;
mGdiBitmap::Option_LoadFile opt;
opt.path = L"d:\\test.bmp";
resource.AddItem<mGdiBitmap>( L"IMAGE", &opt );

[例2]直接ビットマップを読み込みます
    (※読み込み失敗時コンストラクタから例外を投げるので注意)
mGdiBitmap::Option_LoadFile opt;
opt.path = L"d:\\test.bmp";
mGdiBitmap bmp( &opt );

*/

#ifndef MGDIBITMAP_H_INCLUDED
#define MGDIBITMAP_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "mGdiDC.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

namespace Definitions_mGdiBitmap
{
	enum Option_CreateMethod
	{
		NEWBITMAP,	//デバイスコンテキストを指定して、互換のビットマップを新規に作成する
		LOADFILE,	//ファイルをロードする
		RESOURCE,	//リソースをロードする
	};
};

class mGdiBitmap : public mGdiHandle
{
public:


	//オプション構造体
	//ビットマップを作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_NewBitmap … デバイスコンテキストと互換のビットマップを作成したいとき
	//・Option_LoadFile … ビットマップファイルを読み込みたいとき
	//・Option_Resource … リソースをロードしたいとき
	struct Option
	{
		//ビットマップ生成の方法
		using CreateMethod = Definitions_mGdiBitmap::Option_CreateMethod;
		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//BMP_NEWBITMAPを指定してビットマップを作成するときのオプション
	//srcdcをnullptrにすると、デスクトップの互換ビットマップを生成します。
	struct Option_NewBitmap : public Option
	{
		//ココに指定したデバイスコンテキストと色数などが互換になる
		const mGdiDC* srcdc;
		//ビットマップの幅
		DWORD width;
		//ビットマップの高さ
		DWORD height;

		Option_NewBitmap() : Option( CreateMethod::NEWBITMAP )
		{
			srcdc = nullptr;
			width = 100;
			height = 100;
		}
	};

	//ファイルをロードしてビットマップを作成するときのオプション
	struct Option_LoadFile : public Option
	{
		//ロードするファイル名
		WString path;

		Option_LoadFile() : Option( CreateMethod::LOADFILE )
		{
		}
	};

	//リソースをロードしてビットマップを作成するときのオプション
	struct Option_Resource : public Option
	{
		//ロードするリソース名
		WString name;

		Option_Resource() : Option( CreateMethod::RESOURCE )
		{
		}
	};


public:

	//ファクトリメソッド
	//optは必ず指定してください。エラーになりnullptrを返します。
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			result = mNew mGdiBitmap( (const Option*)opt );
		}
		catch( mException )
		{
			//nullptrを返すと、ファクトリメソッドの呼び出し側も失敗する
			result = nullptr;
		}
		return result;
	}

	//コンストラクタ
	//このコンストラクタは、MyHandleに格納するビットマップの生成失敗時に例外を投げます。
	//・optは必ず指定して下さい。nullptrを渡すと例外を投げます。
	mGdiBitmap( const Option* opt )throw( mException );

	//デストラクタ
	virtual ~mGdiBitmap();
	
	//ハンドルの値を取得する(キャスト演算子バージョン)
	operator HBITMAP()const;

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const override;

	//ビットマップのサイズを取得する
	bool GetSize( SIZE& retSize )const noexcept;

	//ビットマップの情報を取得する
	bool GetInfo( BITMAP& retInfo )const noexcept;

private:

	//以下、デフォルト系のは使用不可とする
	mGdiBitmap() = delete;
	mGdiBitmap( const mGdiBitmap& src ) = delete;
	mGdiBitmap& operator=( const mGdiBitmap& src ) = delete;

	//ビットマップの生成
	//・Option_NewBitmapを使用するとき用
	bool CreateHandle( const Option_NewBitmap& opt );

	//ビットマップの生成
	//・Option_LoadFileを使用するとき用
	bool CreateHandle( const Option_LoadFile& opt );

	//ビットマップの生成
	//・Option_Resourceを使用するとき用
	bool CreateHandle( const Option_Resource& opt );


protected:

	//ハンドルの実体
	HBITMAP MyHandle;

};

#endif	//MGDIBITMAP_H_INCLUDED




