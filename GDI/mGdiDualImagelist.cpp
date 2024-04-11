//----------------------------------------------------------------------------
// ウインドウ管理（２イメージのイメージリスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIDUALIMAGELIST_CPP_COMPILING
#include "mGdiDualImagelist.h"
#include "General/mErrorLogger.h"

mGdiDualImagelist::mGdiDualImagelist( const Option* opt )throw( mException )
{
	MyHandle = mNew mGdiImagelist( opt );
	MyHandle2 = ImageList_Duplicate( (HIMAGELIST)MyHandle->GetHandle() );
	if( MyHandle2 == nullptr )
	{
		throw EXCEPTION( 0 , L"ImageList_Duplicate failed" );
	}
	return;
}

mGdiDualImagelist::~mGdiDualImagelist()
{
	//ハンドルの解放
	::ImageList_Destroy( MyHandle2 );
	MyHandle2 = nullptr;
	mDelete MyHandle;
	MyHandle = nullptr;
}

//ハンドルの値を取得する（１つめのイメージリストのハンドルを取得する）
HGDIOBJ mGdiDualImagelist::GetHandle( void )const
{
	return MyHandle->GetHandle();
}

//ハンドルの値を取得する（２つめのイメージリストのハンドルを取得する）
HGDIOBJ mGdiDualImagelist::GetHandle2( void )const
{
	return MyHandle2;
}

//イメージリストにイメージの追加をする
bool mGdiDualImagelist::AddImage( const WString& id ,
	const mGdiBitmap& img1 ,
	const mGdiBitmap& img2 ,
	const mGdiBitmap* mask1 ,
	const mGdiBitmap* mask2 )
{
	//まず、１番目のイメージリストに登録
	if( !MyHandle->AddImage( id , img1 , mask1 ) )
	{
		//エラーメッセージは１番目の方で登録しているからココでは何もしない。
		return false;
	}

	//１番目が成功したので、２番目に登録
	INT index = -1;
	if( mask2 != nullptr )
	{
		//マスクがある場合
		index = ::ImageList_Add( MyHandle2 , img2 , *mask2 );
	}
	else
	{
		//マスクがない場合
		index = ::ImageList_Add( MyHandle2 , img2 , nullptr );
	}

	//２番目の登録は成功だったか？
	if( index < 0 )
	{
		//失敗しているので、１番目の登録を取り消す
		MyHandle->RemoveImage( id );
		//エラーメッセージを登録
		RaiseAssert( g_ErrorLogger , index , L"ImageList_Add(2) failed" );
		return false;
	}
	return true;
}

//イメージリストにイメージの追加をする(mGdiResourceからイメージを抽出するVer)
bool mGdiDualImagelist::AddImageBitmap( const mGdiResource& res ,
	const WString& id ,
	const WString& img1 ,
	const WString& img2 ,
	const WString& mask1 ,
	const WString& mask2 )
{
	//mGdiResourceから指定のイメージを抽出する
	//※１つめ
	const mGdiBitmap* bmp_img1 = res.GetItem< mGdiBitmap >( img1 );
	if( bmp_img1 == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 1 , L"Image id not found : " + img1 );
		return false;
	}
	//２つめ
	const mGdiBitmap* bmp_img2 = res.GetItem< mGdiBitmap >( img2 );
	if( bmp_img2 == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 2 , L"Image id not found : " + img2 );
		return false;
	}

	//イメージを2つとも取れたから、今度はマスクを取得する。マスクは存在しなくてもエラーにはしない。
	//１つめ
	const mGdiBitmap* bmp_mask1 = nullptr;
	if( mask1 != L"" )
	{
		bmp_mask1 = res.GetItem< mGdiBitmap >( mask1 );
	}
	//２つめ
	const mGdiBitmap* bmp_mask2 = nullptr;
	if( mask2 != L"" )
	{
		bmp_mask2 = res.GetItem< mGdiBitmap >( mask2 );
	}

	//追加処理。
	return AddImage( id , *bmp_img1 , *bmp_img2 , bmp_mask1 , bmp_mask2 );
}


//イメージリストにイメージの追加をする
bool mGdiDualImagelist::AddImage( const WString& id ,
	const mGdiBitmap& img1 ,
	const mGdiBitmap& img2 ,
	COLORREF mask1 ,
	COLORREF mask2 )
{
	//まず、１番目のイメージリストに登録
	if( !MyHandle->AddImage( id , img1 , mask1 ) )
	{
		//エラーメッセージは１番目の方で登録しているからココでは何もしない。
		return false;
	}

	//２番目のイメージリストに追加処理を行う
	int index = -1;
	index = ::ImageList_AddMasked( MyHandle2 , img2 , mask2 );

	//２番目の登録は成功だったか？
	if( index < 0 )
	{
		//失敗しているので、１番目の登録を取り消す
		MyHandle->RemoveImage( id );
		//エラーメッセージを登録
		RaiseAssert( g_ErrorLogger , index , L"ImageList_AddMasked(2) failed" );
		return false;
	}
	return true;
}

//イメージリストにイメージの追加をする(mGdiResourceからイメージを抽出するVer)
bool mGdiDualImagelist::AddImageBitmap( const mGdiResource& res ,
	const WString& id ,
	const WString& img1 ,
	const WString& img2 ,
	COLORREF mask1 ,
	COLORREF mask2 )
{
	//mGdiResourceから指定のイメージを抽出する
	//※１つめ
	const mGdiBitmap* bmp_img1 = res.GetItem< mGdiBitmap >( img1 );
	if( bmp_img1 == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 1 , L"Image id not found : " + img1 );
		return false;
	}
	//２つめ
	const mGdiBitmap* bmp_img2 = res.GetItem< mGdiBitmap >( img2 );
	if( bmp_img2 == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 2 , L"Image id not found : " + img2 );
		return false;
	}
	return AddImage( id , *bmp_img1 , *bmp_img2 );
}

//イメージリストにイメージの追加をする
bool mGdiDualImagelist::AddImage( const WString& id ,
	const mGdiIcon& img1 ,
	const mGdiIcon& img2 )
{
	//まず、１番目のイメージリストに登録
	if( !MyHandle->AddImage( id , img1 ) )
	{
		//エラーメッセージは１番目の方で登録しているからココでは何もしない。
		return false;
	}
	//２番目のイメージリストに追加処理を行う
	int index = -1;
	index = ::ImageList_AddIcon( MyHandle2 , img2 );

	//２番目の登録は成功だったか？
	if( index < 0 )
	{
		//失敗しているので、１番目の登録を取り消す
		MyHandle->RemoveImage( id );
		//エラーメッセージを登録
		RaiseAssert( g_ErrorLogger , index , L"ImageList_AddIcon(2) failed" );
		return false;
	}
	return true;
}

//イメージリストにイメージの追加をする(mGdiResourceからイメージを抽出するVer)
bool mGdiDualImagelist::AddImageIcon( const mGdiResource& res ,
	const WString& id ,
	const WString& img1 ,
	const WString& img2 )
{
	//mGdiResourceから指定のイメージを抽出する
	//※１つめ
	const mGdiIcon* icon_img1 = res.GetItem< mGdiIcon >( img1 );
	if( icon_img1 == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 1 , L"Image id not found : " + img1 );
		return false;
	}
	//２つめ
	const mGdiIcon* icon_img2 = res.GetItem< mGdiIcon >( img2 );
	if( icon_img2 == nullptr )
	{
		//抽出失敗。イメージが取得できない場合はエラーで終了
		RaiseError( g_ErrorLogger , 2 , L"Image id not found : " + img2 );
		return false;
	}
	return AddImage( id , *icon_img1 , *icon_img2 );
}

//イメージリストからイメージを削除する
bool mGdiDualImagelist::RemoveImage( const WString& id )
{
	//消去するイメージのインデックスを取得
	INT index = MyHandle->GetIndex( id );
	if( index < 0 )
	{
		RaiseAssert( g_ErrorLogger , index , L"Id not found : " + id );
		return false;
	}

	//１番目から消す
	if( !MyHandle->RemoveImage( id ) )
	{
		//エラーメッセージは１番目の中身で登録しているからココでは何もしない。
		return false;
	}

	//２番目のイメージリストから削除
	if( !::ImageList_Remove( MyHandle2 , index ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_Remove(2) failed" );
		return false;
	}
	return true;
}

//IDからインデックスを取得する
INT mGdiDualImagelist::GetIndex( const WString& id )const
{
	return MyHandle->GetIndex( id );
}


