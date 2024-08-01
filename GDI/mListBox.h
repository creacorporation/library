//----------------------------------------------------------------------------
// ウインドウ管理（リストボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
リストボックスのクラスです。ウインドウに貼り付けることができます。

*/

#ifndef MLISTBOX_H_INCLUDED
#define MLISTBOX_H_INCLUDED

#include "mWindow.h"
#include "mListboxFamily.h"

class mListBox : public mListboxFamily
{
public:


	//リストボックス生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//リストボックス生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//オプション
	struct Option_UseOption : public Option
	{
		WindowPosition Pos;		//表示位置
		bool Enable;			//有効/無効
		bool AutoSort;			//自動ソート有効
		bool Border;			//ボーダーあり
		bool ScrollBar;			//スクロールバーあり
		bool Multiple;			//複数選択可能？
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Enable = true;
			AutoSort = false;
			Border = true;
			ScrollBar = true;
			Multiple = false;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mListBox;
	}

	//アイテムを追加する
	virtual bool AddItem( const WString& caption )override;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data )override;
	virtual bool AddItem( const WString& caption , INT index )override;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data , INT index )override;

	//アイテムを削除する
	virtual bool RemoveItem( INT index )override;

	//選択されているインデックスの一覧を取得する
	virtual SelectedItems GetSelected( void )const override;

	//指定したインデックスのアイテムを選択状態にする
	virtual bool SetSelected( const SelectedItems& items )override;

	//指定したインデックスのアイテムを選択状態にする
	virtual bool SetSelected( INT item )override;

	//指定したキャプションをもつアイテムのインデックスを返す
	virtual INT SearchItem( const WString& caption )override;

	//指定インデックスのキャプションを取得する
	virtual WString GetItemCaption( INT index )const override;

	//指定インデックスに関連づけられているデータを取得する
	virtual bool GetItemData( INT index , ItemDataEntry& retdata )const override;

	//いくつアイテムがあるかをカウントする
	virtual INT GetItemCount( void )const override;

	//アイテムを移動する
	virtual bool MoveItem( INT index , INT moveto )override;

protected:

	mListBox();
	virtual ~mListBox();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mListBox( const mListBox& src ) = delete;
	mListBox& operator=( const mListBox& src ) = delete;

};



#endif	//MLISTBOX_H_INCLUDED
