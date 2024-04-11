//----------------------------------------------------------------------------
// ウインドウ管理（ラジオボタン）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
シンプルなチェックボックスです。ウインドウに貼り付けることができます。

*/


#ifndef MRADIOBUTTON_H_INCLUDED
#define MRADIOBUTTON_H_INCLUDED

#include "mCheckboxFamily.h"

class mRadioButton : public mCheckboxFamily
{
public:

	//ボタン生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//ボタン生成の方法
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

	//ボタン生成時のオプション
	struct Option_UseOption : public Option
	{
		WString Caption;		//ボタンに書いてある文字列
		WindowPosition Pos;		//表示位置
		bool Enable;			//有効/無効
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Enable = true;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void* option )throw( )
	{
		return mNew mRadioButton;
	}

protected:
	mRadioButton();
	virtual ~mRadioButton();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mRadioButton( const mRadioButton& src ) = delete;
	mRadioButton& operator=( const mRadioButton& src ) = delete;

};



#endif	//MRADIOBUTTON_H_INCLUDED

