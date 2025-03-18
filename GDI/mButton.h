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
	struct Option : public mWindow::Option
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
		bool Alternate;			//オルタネイトにする(ボタンを押した後に離してもその状態を保持するボタン)
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Enable = true;
			Alternate = false;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mButton;
	}

	//オルタネイトのボタン(ボタンを押した後に離してもその状態を保持するボタン)のチェック状態を設定する
	// check : 押されていれば真
	bool SetCheck( bool check )const;

	//オルタネイトのボタン(ボタンを押した後に離してもその状態を保持するボタン)が、現在押されているかどうかを確認する
	// ret : 押されていれば真
	bool IsChecked( void )const;

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

