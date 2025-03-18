//----------------------------------------------------------------------------
// ウインドウ管理（チェックボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
シンプルなチェックボックスです。ウインドウに貼り付けることができます。
・チェックマークを付けたり外したり、いまチェックマークが付いているかを確認したりする関数は
　このクラスの一個上、mCheckboxFamilyで宣言してあります。
*/


#ifndef MCHECKBOX_H_INCLUDED
#define MCHECKBOX_H_INCLUDED

#include "mCheckboxFamily.h"

class mCheckBox : public mCheckboxFamily
{
public:

	//生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option : public mWindow::Option
	{
		//チェックボックス生成の方法
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

	//チェックボタン生成時のオプション（プログラムで指定用）
	struct Option_UseOption : public Option
	{
		WString Caption;		//ボタンに書いてある文字列
		WindowPosition Pos;		//表示位置
		bool Enable;			//有効/無効
		bool ThreeState;		//真の場合3ステート
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Enable = true;
			ThreeState = false;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mCheckBox;
	}

protected:
	mCheckBox();
	virtual ~mCheckBox();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mCheckBox( const mCheckBox& src ) = delete;
	mCheckBox& operator=( const mCheckBox& src ) = delete;

};



#endif	//MCHECKBOX_H_INCLUDED

