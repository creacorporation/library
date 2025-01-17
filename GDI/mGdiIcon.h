﻿//----------------------------------------------------------------------------
// ウインドウ管理（アイコン）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MGDIICON_H_INCLUDED
#define MGDIICON_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

class mGdiIcon : public mGdiHandle
{
public:


	//オプション構造体
	//アイコン作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_LoadFile … ビットマップファイルを読み込みたいとき
	//・Option_Resource … リソースをロードしたいとき
	struct Option
	{
		//アイコン生成の方法
		enum CreateMethod
		{
			LOADFILE,	//ファイルをロードする
			RESOURCE,	//リソースをロードする
		};
		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//ファイルをロードしてアイコン作成するときのオプション
	struct Option_LoadFile : public Option
	{
		WString path;			//ロードするファイル名
		Option_LoadFile() : Option( CreateMethod::LOADFILE )
		{
		}
	};

	//リソースをロードしてアイコン作成するときのオプション
	struct Option_Resource : public Option
	{
		WString name;			//ロードするリソース名
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
			result = mNew mGdiIcon( (const Option*)opt );
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
	mGdiIcon( const Option* opt )noexcept( false );

	//デストラクタ
	virtual ~mGdiIcon();
	
	//ハンドルの値を取得する(キャスト演算子バージョン)
	operator HICON()const;

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//以下、デフォルト系のは使用不可とする
	mGdiIcon() = delete;
	mGdiIcon( const mGdiIcon& src ) = delete;
	mGdiIcon& operator=( const mGdiIcon& src ) = delete;

	//カーソル生成
	//・Option_LoadFileを使用するとき用
	bool CreateHandle( const Option_LoadFile& opt );

	//カーソル生成
	//・Option_Resourceを使用するとき用
	bool CreateHandle( const Option_Resource& opt );

protected:

	//ハンドルの実体
	HICON MyHandle;

};

#endif	//MGDIICON_H_INCLUDED

