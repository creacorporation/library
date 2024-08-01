//----------------------------------------------------------------------------
// ウインドウ管理（ウインドウの位置指定クラス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
ウインドウの位置を指定するクラスです。
コンパイルする上での都合で、mWindowから分離されました。
(mMenu.hとmWindow.hが循環インクルードになる)
*/

#ifndef MWINDOWPOSITION_H_INCLUDED
#define MWINDOWPOSITION_H_INCLUDED

#include "mStandard.h"

namespace mWindowPosition
{
	//相対的に座標を決定するための情報
	struct POSITION
	{
		FLOAT rate;			//親のクライアントエリアのサイズに対する比率
		INT offset;			//rateで算出した位置に対するオフセット

		POSITION( FLOAT rate , INT offset )
		{
			this->rate = rate;
			this->offset = offset;
		}
		POSITION() = default;

		void set( FLOAT rate , INT offset )
		{
			this->rate = rate;
			this->offset = offset;
		}
	};

	struct PresetFullscreen
	{
		INT gap;
		PresetFullscreen( INT gap )
		{
			this->gap = gap;
		}
	};

	struct PresetUpperLeftWH
	{
		INT left;
		INT top;
		INT width;
		INT height;
		PresetUpperLeftWH( INT Left , INT Top , INT Width , INT Height )
		{
			this->left = Left;
			this->top = Top;
			this->width = Width;
			this->height = Height;
		}
	};

	struct PresetUpperLeftRECT
	{
		INT left;
		INT top;
		INT right;
		INT bottom;
		PresetUpperLeftRECT( INT Left , INT Top , INT Right , INT Bottom )
		{
			this->left = Left;
			this->top = Top;
			this->right = Right;
			this->bottom = Bottom;
		}
	};

	//ウインドウの相対的な位置設定
	//ウインドウ(あるいはボタンなどのパーツ類)を親ウインドウのクライアントエリア内のどのあたりに配置するかを指定します。
	//上下左右について、親ウインドウの上端・左端からの比率＋オフセットで表します。
	//rate=0.0,offset=10	→左端から10ピクセルの位置
	//rate=1.0,offset=-10	→右端から10ピクセルの位置
	//rate=0.5,offset=10	→中央から右に10ピクセルの位置
	//左端(rate=0.1,offset=  10)+右端(rate=0.1,offset=110) →左寄せ固定幅100ピクセル
	//左端(rate=1.0,offset=-110)+右端(rate=1.0,offset=-10) →右寄せ固定幅100ピクセル
	//左端(rate=0.0,offset=  10)+右端(rate=0.5,offset=-10) →幅は親の50パーセント＋10ピクセルのパディング。
	struct WindowPosition
	{
		POSITION left;		//左端の位置
		POSITION right;		//右端の位置
		POSITION top;		//上端の位置
		POSITION bottom;	//下端の位置
		WindowPosition()
		{
			left = { 0.0f , 0 };
			right = { 0.0f , 0 };
			top = { 0.0f , 0 };
			bottom = { 0.0f , 0 };
		}
		WindowPosition( const PresetFullscreen& v )
		{
			set( v );
		}
		void set( const PresetFullscreen& v )
		{
			left = { 0.0f , v.gap };
			right = { 1.0f , -v.gap };
			top = { 0.0f , v.gap };
			bottom = { 1.0f , -v.gap };
		}
		WindowPosition( const PresetUpperLeftWH& v )
		{
			set( v );
		}
		void set( const PresetUpperLeftWH& v )
		{
			left = { 0.0f , v.left };
			right = { 0.0f , v.left + v.width };
			top = { 0.0f , v.top };
			bottom = { 0.0f , v.top + v.height };
		}
		WindowPosition( const PresetUpperLeftRECT& v )
		{
			set( v );
		}
		void set( const PresetUpperLeftRECT& v )
		{
			left = { 0.0f , v.left };
			right = { 0.0f , v.right };
			top = { 0.0f , v.top };
			bottom = { 0.0f , v.bottom };
		}
	};
};

#endif

