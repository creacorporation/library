//----------------------------------------------------------------------------
// ウインドウ管理（イメージリスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIIMAGELIST_CPP_COMPILING
#include "mGdiImagelist.h"
#include "General/mErrorLogger.h"

mGdiImagelist::mGdiImagelist( const Option* option )throw( mException )
{
	MyHandle = nullptr;

	if( option != nullptr )
	{
		if( option->method == Option::CreateMethod::USEOPTION )
		{
			CreateMyHandle( *(const Option_UseOption*)option );
		}
	}

	//アイコンができてなかったらエラーにする
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"Creating cursor failed" );
	}
}

mGdiImagelist::~mGdiImagelist()
{
	//ハンドルの解放
	::ImageList_Destroy( MyHandle );
	MyHandle = nullptr;
}

//ハンドルの値を取得する(キャスト演算子バージョン)
mGdiImagelist::operator HIMAGELIST()const
{
	return MyHandle;
}

//ハンドルの値を取得する(普通の関数バージョン)
HGDIOBJ mGdiImagelist::GetHandle( void )const
{
	return MyHandle;
}

//イメージリストにイメージの追加をする
bool mGdiImagelist::AddImage( const WString& id , const mGdiBitmap& img , const mGdiBitmap* mask )
{
	//IDが空欄ではないかチェック
	if( id == L"" )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not presented" );
		return false;
	}

	//すでに存在するIDの場合はエラーにする
	if( MyIdIndexMap.count( id ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id already exist : " + id );
		return false;
	}

	//存在しないので、追加処理を行う
	int index = -1;
	if( mask != nullptr )
	{
		index = ::ImageList_Add( MyHandle , img , *mask );
	}
	else
	{
		index = ::ImageList_Add( MyHandle , img , nullptr );
	}

	//エラーだった？
	if( index < 0 )
	{
		//エラーなので終了
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_Add failed" );
		return false;
	}

	//正常終了だったのでIDをマップに追加
	MyIdIndexMap.insert( IdIndexMap::value_type( id , index ) );
	return true;
}

//イメージリストにイメージの追加をする
bool mGdiImagelist::AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , const WString& mask )
{
	//mGdiResourceから指定のイメージを抽出する
	const mGdiBitmap* bmp_img = res.GetItem< mGdiBitmap >( img );
	if( bmp_img == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 0 , L"Image id not found : " + img );
		return false;
	}
	//同様にマスクを取得する。マスクは存在しなくてもエラーにはしない。
	const mGdiBitmap* bmp_mask = nullptr;
	if( mask != L"" )
	{
		bmp_mask = res.GetItem< mGdiBitmap >( mask );
	}

	//追加処理。
	return AddImage( id , *bmp_img , bmp_mask );
}

//イメージリストにイメージの追加をする
bool mGdiImagelist::AddImage( const WString& id , const mGdiBitmap& img , COLORREF mask )
{
	//IDが空欄ではないかチェック
	if( id == L"" )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not presented" );
		return false;
	}

	//すでに存在するIDの場合はエラーにする
	if( MyIdIndexMap.count( id ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id already exist : " + id );
		return false;
	}

	//存在しないので、追加処理を行う
	int index = -1;
	index = ::ImageList_AddMasked( MyHandle , img , mask );

	//エラーだった？
	if( index < 0 )
	{
		//エラーなので終了
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_AddMasked failed" );
		return false;
	}

	//正常終了だったのでIDをマップに追加
	MyIdIndexMap.insert( IdIndexMap::value_type( id , index ) );
	return true;
}

//イメージリストにイメージの追加をする
bool mGdiImagelist::AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , COLORREF mask )
{
	//mGdiResourceから指定のイメージを抽出する
	const mGdiBitmap* bmp_img = res.GetItem< mGdiBitmap >( img );
	if( bmp_img == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 0 , L"Image id not found : " + img );
		return false;
	}

	//追加処理。
	return AddImage( id , *bmp_img , mask );
}


//イメージリストにイメージの追加をする
bool mGdiImagelist::AddImage( const WString& id , const mGdiIcon& img )
{
	//IDが空欄ではないかチェック
	if( id == L"" )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not presented" );
		return false;
	}

	//すでに存在するIDの場合はエラーにする
	if( MyIdIndexMap.count( id ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id already exist : " + id );
		return false;
	}

	//存在しないので、追加処理を行う
	int index = -1;
	index = ::ImageList_AddIcon( MyHandle , img );

	//エラーだった？
	if( index < 0 )
	{
		//エラーなので終了
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_AddIcon failed" );
		return false;
	}

	//正常終了だったのでIDをマップに追加
	MyIdIndexMap.insert( IdIndexMap::value_type( id , index ) );
	return true;
}

//イメージリストにイメージの追加をする
bool mGdiImagelist::AddImageIcon( const mGdiResource& res , const WString& id , const WString& img )
{
	//mGdiResourceから指定のイメージを抽出する
	const mGdiIcon* icon_img = res.GetItem< mGdiIcon >( img );
	if( icon_img == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 0 , L"Icon id not found : " + img );
		return false;
	}

	//追加処理。
	return AddImage( id , *icon_img );
}

//イメージリストからイメージを削除する
bool mGdiImagelist::RemoveImage( const WString& id )
{
	//削除対象を探す
	IdIndexMap::iterator itr = MyIdIndexMap.find( id );
	if( itr == MyIdIndexMap.end() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not found : " + id );
		return false;
	}

	//削除するイメージリストのインデックス
	INT index = itr->second;

	//イメージリストから削除
	if( !::ImageList_Remove( MyHandle , index ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_Remove failed" );
		return false;
	}

	//文字列ID--インデックスのマップから該当のを削除
	MyIdIndexMap.erase( itr );

	//削除すると無くなった分のインデックスが1ずつずれるのでその補正をする
	//イメージリストって内部的にはvector的な構造なんですかね？
	for( itr = MyIdIndexMap.begin() ; itr != MyIdIndexMap.end() ; itr++ )
	{
		if( index < itr->second )
		{
			itr->second--;
		}
	}
	return true;
}

//IDからインデックスを取得する
INT mGdiImagelist::GetIndex( const WString& id )const
{
	//IDが空文字列の場合は、エラーにする
	if( id == L"" )
	{
		RaiseError( g_ErrorLogger , 0 , L"Id not specified" );
		return -1;
	}

	//対象を探す
	IdIndexMap::const_iterator itr = MyIdIndexMap.find( id );
	if( itr == MyIdIndexMap.end() )
	{
		//有りませんでした
		RaiseError( g_ErrorLogger , 0 , L"Id not found : " + id );
		return -1;
	}
	return itr->second;
}

//Optionに指定した内容に見合うオブジェクトを生成し、MyHandleに登録する
//コンストラクタから呼び出される想定
bool mGdiImagelist::CreateMyHandle( const Option_UseOption& opt )
{
	UINT flag = 0;
	//使用する色数に応じて、フラグを設定
	switch( opt.color )
	{
	case Option::ColorDepth::COLOR4:
		flag |= ILC_COLOR4;
		break;
	case Option::ColorDepth::COLOR8:
		flag |= ILC_COLOR8;
		break;
	case Option::ColorDepth::COLOR16:
		flag |= ILC_COLOR16;
		break;
	case Option::ColorDepth::COLOR24:
		flag |= ILC_COLOR24;
		break;
	default:
		//未定義の場合、デフォルトを決めるのは困難そうなのでエラーにする
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid color depth" );
		return false;
	}
	//マスクを使用するためフラグを立てる
	flag |= ILC_MASK;

	//ハンドルの生成
	MyHandle = ::ImageList_Create( opt.width , opt.height , flag , opt.initial_size , opt.grow_size );

	return MyHandle != nullptr;
}

