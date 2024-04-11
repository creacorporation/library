//----------------------------------------------------------------------------
// ウインドウ管理（ボタン）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
シンプルなボタンです。ウインドウに貼り付けることができます。
*/

#ifndef MBUTTON_H_INCLUDED
#define MBUTTON_H_INCLUDED

#include "mWindow.h"

class mButton : public mWindow
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
		bool Enable;			//有効(true)/無効(false)
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Enable = true;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mButton;
	}

protected:
	mButton();
	virtual ~mButton();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mButton( const mButton& src ) = delete;
	mButton& operator=( const mButton& src ) = delete;

};



#endif	//MBUTTON_H_INCLUDED

