//----------------------------------------------------------------------------
// ウインドウ管理（イメージリスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
イメージリストの管理をします。
アイコン等々をまとめて管理し、ツールバーにぶち込んだりできます。

mGdiHandleを継承してはいますが、これはmGdiResourceでまとめて管理できるようにするのが目的で、
イメージリストは、ブラシやペン、フォントと同じような使いかたはできません。
(たとえば、mGdiDC::Select()でデバイスコンテキストに関連づけできません。)

イメージリストに登録したビットマップやアイコンを直接何かに描画する方法は提供していません。
(mGdiDCに例外的な処理が増え、ややこしくなってしまうためです。)
もっぱら、ツールバーのアイコンを登録するためのクラスとして使って下さい。
直接描画したいなら、mGdiBitmapを使って下さい。
*/

#ifndef MGDIIMAGELIST_H_INCLUDED
#define MGDIIMAGELIST_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "mGdiBitmap.h"
#include "mGdiIcon.h"
#include "mGdiResource.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"
#include <CommCtrl.h>
#include <unordered_map>

class mGdiImagelist : public mGdiHandle
{
public:

	//オプション構造体
	//実際にイメージリストを作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//イメージリスト生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		//何色使えるか？
		enum ColorDepth
		{
			COLOR4,		//4ビットカラー		16色
			COLOR8,		//8ビットカラー		256色
			COLOR16,	//16ビットカラー	65536色
			COLOR24,	//24ビットカラー	1677万色
		};

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//オプション構造体
	struct Option_UseOption : public Option
	{
		INT width;			//イメージの幅
		INT height;			//イメージの高さ
		INT initial_size;	//最初にオブジェクトを作ったときのキャパシティ(何枚イメージを格納できるか)
		INT grow_size;		//キャパシティが足りなくなったときに何枚分ずつキャパが拡張されるか
		ColorDepth color;	//イメージのビット数
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			width = 32;
			height = 32;
			initial_size = 32;
			grow_size = 16;
			color = ColorDepth::COLOR16;
		}
	};

	//イメージリストにイメージの追加をする
	//id : 追加するイメージに付与するID(同一のイメージリスト内で重複不可)
	//img : 追加するイメージ
	//mask : イメージの透過マスク(nullの場合はマスクなし)
	//ret : 成功時真
	bool AddImage( const WString& id , const mGdiBitmap& img , const mGdiBitmap* mask = nullptr );

	//イメージリストにイメージの追加をする
	//mGdiResourceからmGdiBitmapを抽出して追加する
	//res : イメージ抽出元
	//id : 追加するイメージに付与するID
	//img : 追加するイメージ(mGdiResourceに登録されているID)
	//mask : イメージの透過マスク(mGdiResourceに登録されているID)空文字列のときはマスクなし。
	//ret : 成功時真
	bool AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , const WString& mask = L"" );

	//イメージリストにイメージの追加をする
	//id : 追加するイメージに付与するID(同一のイメージリスト内で重複不可)
	//img : 追加するイメージ
	//mask : イメージの透過マスク(色を指定)
	//ret : 成功時真
	bool AddImage( const WString& id , const mGdiBitmap& img , COLORREF mask );

	//イメージリストにイメージの追加をする
	//mGdiResourceからmGdiBitmapを抽出して追加する
	//res : イメージ抽出元
	//id : 追加するイメージに付与するID
	//img : 追加するイメージ(mGdiResourceに登録されているID)
	//mask : イメージの透過マスク(色を指定)
	//ret : 成功時真
	bool AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , COLORREF mask );

	//イメージリストにイメージの追加をする
	//id : 追加するイメージに付与するID(同一のイメージリスト内で重複不可)
	//img : 追加するイメージ
	//ret : 成功時真
	bool AddImage( const WString& id , const mGdiIcon& img );

	//イメージリストにイメージの追加をする
	//mGdiResourceからmGdiIconを抽出して追加する
	//res : イメージ抽出元
	//id : 追加するイメージに付与するID
	//img : 追加するイメージ(mGdiResourceに登録されているID)
	//ret : 成功時真
	bool AddImageIcon( const mGdiResource& res , const WString& id , const WString& img );

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
		mGdiImagelist* result;
		try
		{
			result = mNew mGdiImagelist( (const Option*)opt );
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
	mGdiImagelist( const Option* option )throw( mException );

	//デストラクタ
	virtual ~mGdiImagelist();
	
	//ハンドルの値を取得する(キャスト演算子バージョン)
	operator HIMAGELIST()const;

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//以下、デフォルト系のは使用不可とする
	mGdiImagelist() = delete;
	mGdiImagelist( const mGdiImagelist& src ) = delete;
	mGdiImagelist& operator=( const mGdiImagelist& src ) = delete;

	//Optionに指定した内容に見合うオブジェクトを生成し、MyHandleに登録する
	//コンストラクタから呼び出される想定
	bool CreateMyHandle( const Option_UseOption& opt );

protected:

	//ハンドルの実体
	HIMAGELIST MyHandle;

	//文字列IDとImageListのインデックスの関連づけ
	typedef std::unordered_map<WString,INT> IdIndexMap;
	IdIndexMap MyIdIndexMap;

};

#endif	//MGDIIMAGELIST_H_INCLUDED

