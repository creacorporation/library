//----------------------------------------------------------------------------
// ウインドウ管理（リストボックス系）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
リストボックス系のコントロールの基底クラスです。
*/

#ifndef MLISTBOXFAMILY_H_INCLUDED
#define MLISTBOXFAMILY_H_INCLUDED

#include "mWindow.h"
#include <unordered_map>
#include <vector>

class mListboxFamily : public mWindow
{
public:

	//リストボックスの項目に関連づけられているデータ
	struct ItemDataEntry
	{
		ULONG_PTR data;		//ユーザ定義の値
		WString str;		//ユーザ定義の文字列(リストボックスには表示されない)
	};

	//アイテムを追加する
	//caption : リストボックス等に表示する文字列
	//data : ユーザ定義のデータ
	//index : 挿入する位置（指定すると自動ソートは働かず、指定位置に挿入されます。-1にすると末尾。）
	//ret : 正常終了の場合真
	//・すでにある項目(caption)を複数登録すると、コンボボックスには同じ項目がいくつも出現しますが、
	//  取得できるItemDataEntryは共用となり、最後に登録したものになります。
	virtual bool AddItem( const WString& caption ) = 0;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data ) = 0;
	virtual bool AddItem( const WString& caption , INT index ) = 0;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data , INT index ) = 0;

	//アイテムを削除する
	//index : 削除対象
	//ret : 削除された場合は真。削除されなかった場合は偽。
	virtual bool RemoveItem( INT index ) = 0;

	//選択されているインデックスの一覧を取得する
	typedef std::vector<INT> SelectedItems;
	virtual SelectedItems GetSelected( void )const = 0;

	//指定したインデックスのアイテムを選択状態にする
	//items : 選択状態にするアイテムの一覧
	//ret : 成功時真、失敗時偽。
	virtual bool SetSelected( const SelectedItems& items ) = 0;

	//指定したインデックスのアイテムを選択状態にする
	//item : 選択状態にするアイテム
	//ret : 成功時真、失敗時偽。
	virtual bool SetSelected( INT item ) = 0;

	//指定したキャプションをもつアイテムのインデックスを返す
	//caption : 探したいアイテムのキャプション
	//ret : 指定キャプションを持つアイテムのインデックス。見つからない場合負の数。
	//同じキャプションを持つ物がいくつもある場合、最初に見つかったインデックスを返します。
	virtual INT SearchItem( const WString& caption ) = 0;

	//指定インデックスのキャプションを取得する
	//index : 取得したい位置
	//ret : 指定した位置にあるアイテムのキャプション。アイテムがない場合は空文字列。
	virtual WString GetItemCaption( INT index )const = 0;

	//指定インデックスに関連づけられているデータを取得する
	//index : 取得したい位置
	//retdata : 指定した位置にあるアイテムのデータの格納先
	//ret : 成功時真、失敗時偽。
	//※指定インデックスに関連付いているデータが存在しない場合、偽が返る
	virtual bool GetItemData( INT index , ItemDataEntry& retdata )const = 0;

	//いくつアイテムがあるかをカウントする
	//ret : 存在するアイテムの数
	//エラーの場合は負の数。
	virtual INT GetItemCount( void )const = 0;

	//アイテムを移動する
	//indexで指定したアイテムが、movetoで指定した位置に移動します
	//ret : 成功時真、失敗時偽。
	virtual bool MoveItem( INT index , INT moveto ) = 0;

protected:
	mListboxFamily();
	virtual ~mListboxFamily();

	//アイテムの文字列と関連づけられているデータのマップ
	typedef std::unordered_map<WString,ItemDataEntry> ItemData;
	ItemData MyItemData;

private:

	mListboxFamily( const mListboxFamily& src ) = delete;
	mListboxFamily& operator=( const mListboxFamily& src ) = delete;

};



#endif	//MLISTBOXFAMILY_H_INCLUDED


