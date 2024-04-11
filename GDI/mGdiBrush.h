//----------------------------------------------------------------------------
// ウインドウ管理（GDIブラシ）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDIのブラシです。
・作成するブラシは、Option構造体で指定します。
・ファクトリメソッドのoptにnullptrを渡すと、白のソリッドブラシになります。
*/

#ifndef MGDIBRUSH_H_INCLUDED
#define MGDIBRUSH_H_INCLUDED

#include "mStandard.h"
#include "GDI/mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

class mGdiBrush : public mGdiHandle
{
public:
	//ブラシ生成のオプション
	//ブラシを作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_Solid … ベタ塗りブラシを作成したいとき
	//・Option_Hatch … ハッチブラシを作成したいとき
	//・Option_Pattern … パターンブラシを作成したいとき(未実装)
	//・Option_Transparent … 透明ブラシ(NULLブラシ)を作成したいとき
	struct Option
	{
		//ブラシの種類
		enum BrushKind
		{
			TRANSPARENT_BRUSH,	//透明なブラシ
			SOLID_BRUSH,		//ベタ塗りブラシ
			HATCH_BRUSH,		//ハッチブラシ(チェック柄とか作る用)
			PATTERN_BRUSH,		//ビットマップを使って柄を作るブラシ
		};

		const BrushKind kind;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( BrushKind brush_kind ) : kind( brush_kind )
		{
		}
	};

	//ベタ塗りブラシ用設定構造体
	struct Option_Solid : public Option
	{
		COLORREF color;			//ベタ塗りに使う色
		Option_Solid() : Option( BrushKind::SOLID_BRUSH )
		{
			//デフォルトは白ベタブラシ
			color = RGB( 0xFFu , 0xFFu , 0xFFu );
		}
	};

	//ハッチブラシ用設定構造体
	struct Option_Hatch : public Option
	{
		COLORREF color;		//ハッチブラシの描画色
		enum HatchStyle
		{
			UP_DIAGONAL,		//右上がりの斜線     ／／／／模様
			DOWN_DIAGONAL,		//右下がりの斜線	 ＼＼＼＼模様
			CROSS_DIAGONAL,		//斜線のクロスハッチ ××××模様
			HORIZONTAL,			//水平線のハッチ     ――――模様
			VERTICAL,			//垂直線のハッチ     ｜｜｜｜模様
			CROSS,				//十字のハッチ       ＋＋＋＋模様
		}style;

		Option_Hatch() : Option( BrushKind::HATCH_BRUSH )
		{
			color = RGB( 0 , 0 , 0 );
			style = CROSS_DIAGONAL;
		}
	};

	//パターンブラシ用設定構造体
	struct Option_Pattern : public Option
	{
		Option_Pattern() : Option( BrushKind::PATTERN_BRUSH )
		{
		}
	};

	//透明なブラシ用設定構造体
	struct Option_Transparent : public Option
	{
		Option_Transparent() : Option( BrushKind::TRANSPARENT_BRUSH )
		{
		}
	};

	//ファクトリメソッド
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			//ブラシをコンストラクタで生成するけどー
			result = mNew mGdiBrush( (const Option*)opt );
		}
		catch( mException )
		{
			//例外が発生した場合はnullptrを返す（すると、ファクトリメソッドの呼び出し側も失敗する）
			//※mNewしたところがメモリリークしてそうに思えてしまうけど、ちゃんと解放されるよ。問題ない。
			//　→EffectiveC++第三版の52項
			result = nullptr;
		}
		return result;
	}

	//コンストラクタ
	//このコンストラクタは、MyHandleに格納するブラシの生成失敗時に例外を投げます。
	mGdiBrush( const Option* opt )throw( mException );

	//デストラクタ
	virtual ~mGdiBrush();
	
	//ハンドルの値を取得する(キャスト演算子バージョン)
	operator HBRUSH()const;

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//以下、デフォルト系のは使用不可とする
	mGdiBrush() = delete;
	mGdiBrush( const mGdiBrush& src ) = delete;
	mGdiBrush& operator=( const mGdiBrush& src ) = delete;

private:
	//ソリッドブラシを作成します。成功すればMyHandleに値が入ります。
	//opt : どんなブラシを作成するかの情報
	//ret : 成功時に真
	bool CreateSolidBrush( const Option_Solid& opt );

	//ハッチブラシを作成します。成功すればMyHandleに値が入ります。
	//opt : どんなブラシを作成するかの情報
	//ret : 成功時に真
	bool CreateHatchBrush( const Option_Hatch& opt );

	//パターンブラシを作成します。成功すればMyHandleに値が入ります。
	//opt : どんなブラシを作成するかの情報
	//ret : 成功時に真
	bool CreatePatternBrush( const Option_Pattern& opt );


protected:
	//ハンドルの実体
	HBRUSH MyHandle;
};

#endif	//MGDIREHANDLE_H_INCLUDED

