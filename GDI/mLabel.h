//----------------------------------------------------------------------------
// ウインドウ管理（ラベル）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
ラベルです。ウインドウに貼り付けることができます。

＜参考＞
コントロールの色等々を変えるには、親ウインドウでWM_CTLCOLORSTATICを処理します。
・WM_CTLCOLORSTATIC
   WPARAM = コントロールのデバイスコンテキストのハンドル
   LPARAM = コントロールのウインドウハンドル
   戻り値 = 色を塗るのに使うブラシのハンドル
            ex : GetStockObject( NULL_BRUSH );

*/

#ifndef MLABEL_H_INCLUDED
#define MLABEL_H_INCLUDED

#include "mWindow.h"

class mLabel : public mWindow
{
public:

	//ラベル生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option : public mWindow::Option
	{
		//生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		//テキストの配置を決定する
		enum class TextJustify
		{
			CENTER,				//中央揃え
			LEFT,				//左揃え
			LEFTNOWORDWRAP,		//左揃えだが改行しない
			RIGHT,				//右揃え
		};

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//ラベル生成時のオプション
	struct Option_UseOption : public Option
	{
		WString Caption;		//ボタンに書いてある文字列
		WindowPosition Pos;		//表示位置
		TextJustify Justify;	//テキストの配置
		bool NoPrefix;			//&をプリフィクスとして解釈しない(falseの場合"&+何か"で下線付きになる)
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
			Justify = TextJustify::LEFT;
			NoPrefix = true;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void* option )throw( )
	{
		return mNew mLabel;
	}

protected:
	mLabel();
	virtual ~mLabel();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mLabel( const mLabel& src ) = delete;
	mLabel& operator=( const mLabel& src ) = delete;

};



#endif	//MLABEL_H_INCLUDED

