//----------------------------------------------------------------------------
// ウインドウ管理（GDIリソースプール）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDIリソースをストックします

HBRUSH brush;			//ブラシ
HPEN pen;				//ペン
HFONT font;				//フォント
HBITMAP bmp;			//ビットマップ
HIMAGELIST imglist;		//イメージリスト

*/

#ifndef MGDIRESOURCE_H_INCLUDED
#define MGDIRESOURCE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"
#include "GDI/mGdiHandle.h"
#include <list>
#include <unordered_map>
#include <typeinfo>

class mGdiResource
{
public:
	mGdiResource();
	virtual ~mGdiResource();

	//ファクトリメソッドのtypedef。
	typedef mGdiHandle* (*mGdiHandleFactory)( const void* opt );

	//アイテムを登録する。これで登録したオブジェクトは、GetItemで取得できます。
	//テンプレートに指定したクラスのFactory()が呼び出され、そこで生成したポインタを
	//idに指定した名前で内部のコンテナに格納します。Factory()にはoptで指定したポインタが渡されます。
	//id : 登録するアイテムに付ける名前
	//item : 登録するアイテム
	//ret : 生成したオブジェクト(GetItemで取れる物と同じ)
	template< class T > T* AddItem( const WString& id , const struct T::Option* opt )
	{
		T* obj = (T*)mGdiResource::AddItemInternal( T::Factory , id , opt );
		if( obj == nullptr )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"AddItem failed : " + id );
			return nullptr;
		}
		return obj;
	}

	//アイテムを取得する
	//AddItemで登録したオブジェクトを返します。
	//idで指定した物が存在しない場合は、subidで指定した物を探します。
	//それもない場合は、nullptrを返します。
	//id : 取得したいID(AddItemで指定したもの)
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 取得したオブジェクト。該当無しの場合nullptr。
	template< class T > T* GetItem( const WString& id , const WString& subid = L"" )const
	{
		//アイテムの検索
		IdMap::const_iterator itr = MyIdMap.find( id );
		if( itr == MyIdMap.end() )
		{
			//該当IDの登録がない場合は、subidで検索
			itr = MyIdMap.find( subid );
			if( itr == MyIdMap.end() )
			{
				//その登録もない場合は該当なしにつきエラー
				RaiseAssert( g_ErrorLogger , 0 , L"Id not found : " + id );
				return nullptr;
			}
		}
		if( dynamic_cast< T* >( itr->second ) == nullptr )
		{
			//テンプレートと取得したオブジェクトの型が違う
			RaiseAssert( g_ErrorLogger , 0 , L"Typeinfo mismatch : " + id );
			return nullptr;
		}
		return (T*)itr->second;
	}

	//アイテムのハンドルを取得する
	//AddItemで登録したオブジェクトを検索し、そのオブジェクトのハンドルを返します。
	//ハンドルが存在しなかった場合はnullptrが返ります。
	//id : 取得したいID(AddItemで指定したもの)
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 取得したオブジェクトのハンドル。該当無しの場合nullptr。
	HGDIOBJ GetItem( const WString& id , const WString& subid = L"" )const;

	//アイテムを削除する
	//id : 削除したいID
	//ret： 成功時true
	bool RemoveItem( const WString& id );

	//指定したIDのオブジェクトが存在するかを返します
	//id : 調べたいID
	//ret : 存在すればtrue。存在しなければfalse。
	bool IsExist( const WString& id )const;

private:
	mGdiResource( const mGdiResource& src ) = delete;
	mGdiResource& operator=( const mGdiResource& src ) = delete;

	//コントロールをコレクションに追加する。AddControlの内部処理。
	//factory : ファクトリメソッド
	//id : 登録するID
	//opt : ファクトリメソッドで生成したオブジェクトに渡すオプション
	//ret : 生成したオブジェクトのポインタ
	mGdiHandle* AddItemInternal( mGdiHandleFactory factory , const WString& id , const void* opt );


protected:

	//オブジェクトのマッピング
	//左：オブジェクトのID
	//右：オブジェクトへのポインタ
	typedef std::unordered_map<WString,mGdiHandle*> IdMap;
	IdMap MyIdMap;
	
};

#endif	//MGDIRESOURCE_H_INCLUDED

