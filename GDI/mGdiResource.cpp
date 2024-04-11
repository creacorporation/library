//----------------------------------------------------------------------------
// ウインドウ管理（GDIリソースプール）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIRESOURCE_CPP_COMPILING
#include "mGdiResource.h"
#include "General/mErrorLogger.h"

mGdiResource::mGdiResource()
{
}

mGdiResource::~mGdiResource()
{
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		mDelete itr->second;
	}
}

//コントロールをコレクションに追加する
mGdiHandle* mGdiResource::AddItemInternal( mGdiHandleFactory factory , const WString& id , const void* opt )
{
	//使用不能なIDでないかを確認
	//すでに存在するIDではないか、IDが空文字列ではないかを確認する
	if( id == L"" || MyIdMap.count( id ) )
	{
		//使用不能なIDだった
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"Bad id : " + id );
		return nullptr;
	}

	//オブジェクトのインスタンスを作成する
	mGdiHandle* obj = factory( opt );
	if( obj == nullptr )
	{
		//ファクトリメソッドが失敗した
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"Factory method returned nullptr : " + id );
		return nullptr;
	}

	//生成したオブジェクトを登録する
	MyIdMap.insert( IdMap::value_type( id , obj ) );

	return obj;
}

HGDIOBJ mGdiResource::GetItem( const WString& id , const WString& subid )const
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
	RaiseAssert( g_ErrorLogger , 0 , L"Id not found : " + id );
	return nullptr;
}

bool mGdiResource::RemoveItem( const WString& id )
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

bool mGdiResource::IsExist( const WString& id )const
{
	return MyIdMap.count( id ) != 0;
}
