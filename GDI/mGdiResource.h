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

template< class C = WString >
class mGdiResourceTemplate
{
public:
	mGdiResourceTemplate()
	{
	}
	virtual ~mGdiResourceTemplate()
	{
		Reset();
	}

	//ファクトリメソッドのtypedef。
	typedef mGdiHandle* (*mGdiHandleFactory)( const void* opt );

	//アイテムを登録する。これで登録したオブジェクトは、GetItemで取得できます。
	//テンプレートに指定したクラスのFactory()が呼び出され、そこで生成したポインタを
	//idに指定した名前で内部のコンテナに格納します。Factory()にはoptで指定したポインタが渡されます。
	//id : 登録するアイテムに付ける名前
	//item : 登録するアイテム
	//ret : 生成したオブジェクト(GetItemで取れる物と同じ)
	template< class T >
	T* AddItem( const C& id , const struct T::Option* opt )
	{
		T* obj = (T*)AddItemInternal( T::Factory , id , opt );
		if( obj == nullptr )
		{
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
	template< class T >
	T* GetItem( const C& id )const
	{
		//アイテムの検索
		IdMap::const_iterator itr = MyIdMap.find( id );
		if( itr == MyIdMap.end() )
		{
			//その登録もない場合は該当なしにつきエラー
			CreateLogEntry( g_ErrorLogger , 0 , L"該当IDがありません" );
			return nullptr;
		}
		return (T*)itr->second;
	}

	//アイテムを取得する
	//AddItemで登録したオブジェクトを返します。
	//idで指定した物が存在しない場合は、subidで指定した物を探します。
	//それもない場合は、nullptrを返します。
	//id : 取得したいID(AddItemで指定したもの)
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 取得したオブジェクト。該当無しの場合nullptr。
	template< class T >
	T* GetItem( const C& id , const C& subid )const
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
				CreateLogEntry( g_ErrorLogger , 0 , L"該当IDがありません" );
				return nullptr;
			}
		}
		return (T*)itr->second;
	}

	//アイテムのハンドルを取得する
	//AddItemで登録したオブジェクトを検索し、そのオブジェクトのハンドルを返します。
	//ハンドルが存在しなかった場合はnullptrが返ります。
	//id : 取得したいID(AddItemで指定したもの)
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 取得したオブジェクトのハンドル。該当無しの場合nullptr。
	HGDIOBJ GetItem( const C& id )const
	{
		IdMap::const_iterator itr;

		//まずは最初のidで検索
		itr = MyIdMap.find( id );
		if( itr != MyIdMap.end() )
		{
			return itr->second->GetHandle();
		}
		//該当無しエラー
		CreateLogEntry( g_ErrorLogger , 0 , L"該当IDがありません" );
		return nullptr;
	}

	//アイテムのハンドルを取得する
	//AddItemで登録したオブジェクトを検索し、そのオブジェクトのハンドルを返します。
	//ハンドルが存在しなかった場合はnullptrが返ります。
	//id : 取得したいID(AddItemで指定したもの)
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 取得したオブジェクトのハンドル。該当無しの場合nullptr。
	HGDIOBJ GetItem( const C& id , const C& subid )const
	{
		IdMap::const_iterator itr;

		//まずは最初のidで検索
		itr = MyIdMap.find( id );
		if( itr != MyIdMap.end() )
		{
			return itr->second->GetHandle();
		}
		//該当IDの登録がない場合は、subidで検索
		itr = MyIdMap.find( subid ); 
		if( itr != MyIdMap.end() )
		{
			return itr->second->GetHandle();
		}
		//その登録もない場合は該当無しエラー
		CreateLogEntry( g_ErrorLogger , 0 , L"該当IDがありません" );
		return nullptr;
	}

	//アイテムを削除する
	//id : 削除したいID
	//ret： 成功時true
	bool RemoveItem( const C& id )
	{
		//アイテムを削除する
		IdMap::iterator itr = MyIdMap.find( id );
		if( itr == MyIdMap.end() )
		{
			return false;
		}
		mDelete itr->second;
		MyIdMap.erase( itr );
		return true;
	}

	//指定したIDのオブジェクトが存在するかを返します
	//id : 調べたいID
	//ret : 存在すればtrue。存在しなければfalse。
	bool IsExist( const C& id )const
	{
		return MyIdMap.count( id ) != 0;
	}

	//アイテムを全部削除する
	void Reset( void )
	{
		for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
		{
			mDelete itr->second;
		}
		MyIdMap.clear();
	}

private:
	mGdiResourceTemplate( const mGdiResourceTemplate& src ) = delete;
	mGdiResourceTemplate& operator=( const mGdiResourceTemplate& src ) = delete;

	//コントロールをコレクションに追加する。AddControlの内部処理。
	//factory : ファクトリメソッド
	//id : 登録するID
	//opt : ファクトリメソッドで生成したオブジェクトに渡すオプション
	//ret : 生成したオブジェクトのポインタ
	mGdiHandle* AddItemInternal( mGdiHandleFactory factory , const C& id , const void* opt )
	{
		//使用不能なIDでないかを確認
		//すでに存在するIDではないかを確認する
		if( MyIdMap.count( id ) )
		{
			//使用不能なIDだった
			RaiseError( g_ErrorLogger , (ULONG_PTR)factory , L"GDIオブジェクトのIDが重複しています" );
			return nullptr;
		}

		//オブジェクトのインスタンスを作成する
		mGdiHandle* obj = factory( opt );
		if( obj == nullptr )
		{
			//ファクトリメソッドが失敗した
			RaiseError( g_ErrorLogger , (ULONG_PTR)factory , L"GDIオブジェクトのファクトリメソッドが失敗しました" );
			return nullptr;
		}

		//生成したオブジェクトを登録する
		MyIdMap.insert( IdMap::value_type( id , obj ) );

		return obj;
	}


protected:

	//オブジェクトのマッピング
	//左：オブジェクトのID
	//右：オブジェクトへのポインタ
	typedef std::unordered_map<C,mGdiHandle*> IdMap;
	IdMap MyIdMap;
	
};

using mGdiResource = mGdiResourceTemplate<WString>;

#endif	//MGDIRESOURCE_H_INCLUDED

