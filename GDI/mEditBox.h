//----------------------------------------------------------------------------
// ウインドウ管理（エディットコントロール）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
エディットコントロールです。ウインドウに貼り付けることができます。
*/

#ifndef MEDITBOX_H_INCLUDED
#define MEDITBOX_H_INCLUDED

#include "mWindow.h"

class mEditBox : public mWindow
{
public:


	//コントロール生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//エディットボックス生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		//文字列の配置
		enum TextJustify
		{
			LEFT,		//左揃え
			CENTER,		//中央揃え
			RIGHT		//右揃え
		};

		//文字列の大文字・小文字を自動変換する
		enum TextCase
		{
			NOCHANGE,	//処理なし
			UPPERCASE,	//大文字に変更
			LOWERCASE,	//小文字に変更
		};

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//コントロール生成時のオプション
	struct Option_UseOption : public Option
	{
		WString Text;			//ボタンに書いてある文字列
		WindowPosition Pos;		//表示位置
		bool Multiline;			//複数行タイプ
		bool HScrollBar;		//水平スクロールバーを表示する
		bool VScrollBar;		//垂直スクロールバーを表示する
		bool Password;			//パスワードタイプ
		bool Readonly;			//リードオンリー
		bool Number;			//数字のみ
		bool RedirectEnter;		//エンターキーの入力をリダイレクトする
		TextJustify Justify;	//文字列の配置
		TextCase Case;			//文字列の大文字・小文字を自動変換する

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Text = L"";
			Multiline = false;
			HScrollBar = false;
			VScrollBar = false;
			Password = false;
			Readonly = false;
			Number = false;
			RedirectEnter = false;
			Justify = TextJustify::LEFT;
			Case = TextCase::NOCHANGE;
		}
	};

	//エディットコントロールの文字列をセット
	bool SetText( const WString& NewText );

	//エディットコントロールの文字列を取得
	bool GetText( WString& retText );

	//エディットコントロールの文字列の長さ（目安）を取得
	DWORD GetTextLength( void )const;

public:

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mEditBox;
	}

	//ウインドウプロシージャ
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam );

protected:
	mEditBox();
	virtual ~mEditBox();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

	//trueならばエンターキーを親ウインドウにリダイレクトする
	bool MyRedirectEnter;

private:

	mEditBox( const mEditBox& src ) = delete;
	mEditBox& operator=( const mEditBox& src ) = delete;

};



#endif	//MEDITBOX_H_INCLUDED

