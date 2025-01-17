//----------------------------------------------------------------------------
// ウインドウ管理（２イメージのイメージリスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
イメージリストを２つ管理します。
*/

#ifndef MGDIDUALIMAGELIST_H_INCLUDED
#define MGDIDUALIMAGELIST_H_INCLUDED

#include "mGdiImagelist.h"
#include "mGdiResource.h"
#include "General/mException.h"

class mGdiDualImagelist : public mGdiHandle
{
public:

	//オプション構造体
	typedef mGdiImagelist::Option Option;

	//イメージリストにイメージの追加をする
	//id : 追加するイメージに付与するID(同一のイメージリスト内で重複不可)
	//img1,img2 : 追加するイメージ
	//mask1,mask2 : イメージの透過マスク(nullptrまたは省略の場合はマスクなし)
	//ret : 成功時真
	bool AddImage( const WString& id ,
		const mGdiBitmap& img1 ,
		const mGdiBitmap& img2 , 
		const mGdiBitmap* mask1 = nullptr ,
		const mGdiBitmap* mask2 = nullptr );

	//イメージリストにイメージの追加をする(mGdiResourceからイメージを抽出するVer)
	//res : イメージ抽出元
	//id : 追加するイメージに付与するID(イメージリストに登録するときのID)
	//img1,img2 : 追加するイメージのID(mGdiResourceから抽出するときのID)
	//mask1,mask2 : イメージの透過マスクのID(mGdiResourceから抽出するときのID。空文字列の場合はマスクなし)
	//ret : 成功時真
	bool AddImageBitmap( const mGdiResource& res , 
		const WString& id ,
		const WString& img1 ,
		const WString& img2 , 
		const WString& mask1 = L"" ,
		const WString& mask2 = L"" );

	//イメージリストにイメージの追加をする
	//id : 追加するイメージに付与するID(同一のイメージリスト内で重複不可)
	//img1,img2 : 追加するイメージ
	//mask1,img2 : イメージの透過マスク(色を指定) ※省略できません
	//ret : 成功時真
	bool AddImage( const WString& id ,
		const mGdiBitmap& img1 ,
		const mGdiBitmap& img2 ,
		COLORREF mask1 ,
		COLORREF mask2 );

	//イメージリストにイメージの追加をする(mGdiResourceからイメージを抽出するVer)
	//res : イメージ抽出元
	//id : 追加するイメージに付与するID(イメージリストに登録するときのID)
	//img1,img2 : 追加するイメージのID(mGdiResourceから抽出するときのID)
	//mask1,img2 : イメージの透過マスク(色を指定) ※省略できません
	//ret : 成功時真
	bool AddImageBitmap( const mGdiResource& res , 
		const WString& id ,
		const WString& img1 ,
		const WString& img2 ,
		COLORREF mask1 ,
		COLORREF mask2 );

	//イメージリストにイメージの追加をする
	//id : 追加するイメージに付与するID(同一のイメージリスト内で重複不可)
	//img1,img2 : 追加するイメージ
	//ret : 成功時真
	bool AddImage( const WString& id , 
		const mGdiIcon& img1 ,
		const mGdiIcon& img2 );

	//イメージリストにイメージの追加をする(mGdiResourceからイメージを抽出するVer)
	//res : イメージ抽出元
	//id : 追加するイメージに付与するID(イメージリストに登録するときのID)
	//img1,img2 : 追加するイメージのID(mGdiResourceから抽出するときのID)
	//ret : 成功時真
	bool AddImageIcon( const mGdiResource& res , 
		const WString& id , 
		const WString& img1 ,
		const WString& img2 );

	//イメージリストからイメージを削除する
	//id : 削除するイメージのID
	//ret : 成功時真
	bool RemoveImage( const WString& id );

	//IDからインデックスを取得する
	//id : 取得したいID
	//ret : インデックス。エラーの場合負の数
	INT GetIndex( const WString& id )const;

public:

	//ファクトリメソッド
	//optは必ず指定してください。エラーになりnullptrを返します。
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiDualImagelist* result;
		try
		{
			result = mNew mGdiDualImagelist( (const Option*)opt );
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
	mGdiDualImagelist( const Option* opt )noexcept( false );

	//デストラクタ
	virtual ~mGdiDualImagelist();
	
	//ハンドルの値を取得する（１つめのイメージリストのハンドルを取得する）
	virtual HGDIOBJ GetHandle( void )const override;

	//ハンドルの値を取得する（２つめのイメージリストのハンドルを取得する）
	HGDIOBJ GetHandle2( void )const;

private:

	//以下、デフォルト系のは使用不可とする
	mGdiDualImagelist() = delete;
	mGdiDualImagelist( const mGdiDualImagelist& src ) = delete;
	mGdiDualImagelist& operator=( const mGdiDualImagelist& src ) = delete;

protected:

	//ハンドルの実体（ノーマル）
	mGdiImagelist* MyHandle;

	//ハンドルの実体（ホット）
	HIMAGELIST MyHandle2;

};

#endif	//MGDIDUALIMAGELIST_H_INCLUDED

