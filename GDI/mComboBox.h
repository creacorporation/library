//----------------------------------------------------------------------------
// ウインドウ管理（コンボボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
コンボボックスのクラスです。ウインドウに貼り付けることができます。

・自動ソート、スタイルの変更は出来ません（制御が変わってしまうため）
*/

#ifndef MCOMBOBOX_H_INCLUDED
#define MCOMBOBOX_H_INCLUDED

#include "mWindow.h"
#include "mListboxFamily.h"

class mComboBox : public mListboxFamily
{
public:

	//コンボボックス生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
	//コンボボックス生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		//スタイル
		enum ControlStyle
		{
			SIMPLE,			//【シンプル】エディットコントロール＋常に表示されているリスト
			DROPDOWN,		//【ドロップダウン】エディットコントロール＋ドロップダウンするリスト
			DROPDOWNLIST,	//【ドロップダウンリスト】編集不能のテキスト＋ドロップダウンするリスト
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
		ControlStyle Style;		//スタイル
		WindowPosition Pos;		//表示位置
		bool Enable;			//有効/無効
		bool AutoSort;			//自動ソート有効
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Style = ControlStyle::DROPDOWNLIST;
			Enable = true;
			AutoSort = false;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mComboBox;
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
	virtual bool MoveItem( INT index , INT moveto ) override;

protected:

	mComboBox();
	virtual ~mComboBox();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mComboBox( const mComboBox& src ) = delete;
	mComboBox& operator=( const mComboBox& src ) = delete;

};



#endif	//MCOMBOBOX_H_INCLUDED
