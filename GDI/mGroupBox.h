//----------------------------------------------------------------------------
// ウインドウ管理（グループボックス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
グループボックスです。ウインドウに貼り付けることができます。


*/

#ifndef MGROUPBOX_H_INCLUDED
#define MGROUPBOX_H_INCLUDED

#include "mWindow.h"

class mGroupBox : public mWindow
{
public:

	//ウインドウプロシージャ
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )override;

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

	//グループボックス生成時のオプション
	struct Option_UseOption : public Option
	{
		WString Caption;		//グループボックスの左上に書いてある文字列
		WindowPosition Pos;		//表示位置
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Caption = L"";
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mGroupBox;
	}

protected:
	mGroupBox();
	virtual ~mGroupBox();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mGroupBox( const mGroupBox& src ) = delete;
	mGroupBox& operator=( const mGroupBox& src ) = delete;

};



#endif	//MGROUPBOX_H_INCLUDED

