//----------------------------------------------------------------------------
// ウインドウ管理（GDIペン）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDIのペンです。
・作成するペンは、Option構造体で指定します。
・ファクトリメソッドのoptにnullptrを渡すと、黒の1ポイントのペンになります。
*/

#ifndef MGDIPEN_H_INCLUDED
#define MGDIPEN_H_INCLUDED

#include "mStandard.h"
#include "GDI/mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

class mGdiPen : public mGdiHandle
{
public:

	//オプション構造体
	//実際にイメージリストを作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//ペン生成の方法
		enum CreateMethod
		{
			USEOPTION,			//通常の方法
			TRANSPARENT_PEN		//実際には何も書かないペン
		};

		//ペンの種類
		enum PenKind
		{
			SOLID_PEN,			//実線のペン  ________
			DASH_PEN,			//破線のペン  __ __ __
			DOT_PEN,			//点線のペン  ........
			DASHDOT_PEN,		//一点鎖ペン  __.__.__
			DASHDOTDOT_PEN,		//二点鎖ペン  __..__..
		};

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//ペン生成のオプション
	//どんなペンを生成するかをココで指定します。
	struct Option_UseOption : public Option
	{
		PenKind kind;
		INT width;
		COLORREF color;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			kind = PenKind::SOLID_PEN;
			width = 1;
			color = RGB( 0 , 0 , 0 );
		}
	};

	struct Option_Transparent : public Option
	{
		Option_Transparent() : Option( CreateMethod::TRANSPARENT_PEN )
		{
		}
	};

	//ファクトリメソッド
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			//新しいペンのインスタンスを作成する
			result = mNew mGdiPen( (const Option*)opt );
		}
		catch( mException )
		{
			//ペンの生成に失敗した場合はヌルポインタを返す
			result = nullptr;
		}
		return result;
	}

	//コンストラクタ
	//このコンストラクタは、MyHandleに格納するブラシの生成失敗時に例外を投げます。
	mGdiPen( const Option* option )throw( mException );

	//デストラクタ
	virtual ~mGdiPen();

	//ハンドルの値を取得する(キャスト演算子バージョン)
	operator HPEN()const;

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const override;

private:
	
	//以下、デフォルト系のは使用不可とする
	mGdiPen() = delete;
	mGdiPen( const mGdiPen& src ) = delete;
	mGdiPen& operator=( const mGdiPen& src ) = delete;

private:

	//ペン生成
	//・Option_UseOptionを使用するとき用
	bool CreateHandle( const Option_UseOption& opt );

	//・Option_Transparentを使用するとき用
	bool CreateHandle( const Option_Transparent& opt );

protected:
	//ハンドルの実体
	HPEN MyHandle;
};

#endif	//MGDIPEN_H_INCLUDED

