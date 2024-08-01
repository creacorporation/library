//----------------------------------------------------------------------------
// ウインドウ管理（デバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
デバイスコンテキストのハンドルです。
このクラスは基底クラスで、ハンドルを作る方法によって派生クラスが分かれています。

※ココで派生クラスのご紹介です※
[mGdiWinDC]
ウインドウのクライアント領域に直接描画をするためのクラスです。
このクラスではWinAPIのGetDC()を呼び出して取得したデバイスコンテキストのハンドルを扱います。

[mGdiPaintDC]
WM_PAINTメッセージに対応して、ウインドウを再描画するためのクラスです。
BeginPaint()で取得したデバイスコンテキストのハンドルを扱います。
*/

#ifndef MGDIDC_H_INCLUDED
#define MGDIDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "mGdiResource.h"
#include <unordered_set>
namespace mGdiDC_Definitions
{
	enum PrintHorizontalAlign
	{
		H_ALIGN_LEFT,		//左揃え
		H_ALIGN_CENTER,		//(水平)中央揃え
		H_ALIGN_RIGHT		//右揃え
	};
	enum PrintVerticalAlign
	{
		V_ALIGN_TOP,		//上揃え
		V_ALIGN_CENTER,		//(垂直)中央揃え ※これを指定すると改行は無視されます
		V_ALIGN_BOTTOM		//下揃え ※これを指定すると改行は無視されます
	};
	enum StrechMode
	{
		STRECH_BLACKONWHITE,
		STRECH_WHITEONBLACK,
		STRECH_COLORONCOLOR,
		STRECH_HALFTONE,
	};
}

class mGdiDC
{
public:
	virtual ~mGdiDC();

	//オブジェクト(ペン、ブラシ、フォントなど)を選択する
	//handle : 選択したいオブジェクトを格納したmGdiHandleまたはその派生クラス
	//ret : 成功した場合true
	bool Select( const mGdiHandle& handle );

	//オブジェクトを選択します(HGDIOBJ直指定ver)
	//handle : 関連づけたいオブジェクト
	//ret : 成功した場合true
	bool Select( HGDIOBJ new_object );

	//オブジェクトを選択します(mGdiResourceから抽出ver その1)
	//res : 抽出元のリソースプール
	//id : 取得したいID
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 成功した場合true
	bool Select( const mGdiResource& res , const WString& id , const WString& subid = L"" );

	//オブジェクトを選択します(mGdiResourceから抽出ver その2)
	//res : 抽出元のリソースプール
	//id : 取得したいID
	//subid : idがなかった場合に取得したいID(不要な場合は空文字列でOK)
	//ret : 成功した場合true
	//型なし版とは、mGdiResourceからオブジェクトを抽出したときに型のチェックが入る点が違います。
	template< class T >
	bool Select( const mGdiResource& res , const WString& id , const WString& subid = L"" )
	{
		const T* object = res.GetItem<T>( id , subid );
		if( object == nullptr )
		{
			return false;
		}
		return Select( *object );
	}

	//線を描画する
	//( from_x , from_y )この座標から線を引きます
	//( to_x , to_y )この座標に向けて線を引きます
	//ret : 成功時真
	//・fromを省略した場合は、現在の位置から線を引きます。
	//・この関数を実行後は、toに指定した位置が「現在の位置」となります
	bool Line( INT to_x , INT to_y );
	bool Line( INT from_x , INT from_y , INT to_x , INT to_y );
	bool LineOffset( INT from_x , INT from_y , INT offset_x , INT offset_y );

	//矩形を描画する
	//( x1 , y1 )-( x2 , y2 )を対角線上の頂点とする長方形を描画します。
	//ret : 成功時真
	//・外枠が現在のペンで、内側が現在のブラシで塗りつぶされる。
	//・外枠が要らないならヌルペン(mGdiPen的にはTRANSPARENT_PEN)を使う
	//・塗りつぶさないならヌルブラシ(mGdiBrush的にはTRANSPARENT_BRUSH)を使う
	//【重要】WinAPIは右辺、底辺について、指定した座標の1ピクセル内側に描画されますが、
	//       この関数はそれを補正しています。指定した座標上を右辺＆底辺が通ります。
	bool Rectangle( INT x1 , INT y1 , INT x2 , INT y2 );

	//矩形を描画する
	//( x1 , y1 )-( x1+x2 , y1+y2 )を対角線上の頂点とする長方形を描画します。
	//ret : 成功時真
	//・外枠が現在のペンで、内側が現在のブラシで塗りつぶされる。
	//・外枠が要らないならヌルペン(mGdiPen的にはTRANSPARENT_PEN)を使う
	//・塗りつぶさないならヌルブラシ(mGdiBrush的にはTRANSPARENT_BRUSH)を使う
	//【重要】WinAPIは右辺、底辺について、指定した座標の1ピクセル内側に描画されますが、
	//       この関数はそれを補正しています。指定した座標上を右辺＆底辺が通ります。
	bool RectangleOffset( INT x1 , INT y1 , INT offset_x , INT offset_ );

	//円を描画する
	//指定座標を中心とした、指定半径の円を描きます
	//( x , y )円の中心
	//radius : 円の半径
	//・外枠が現在のペンで、内側が現在のブラシで塗りつぶされる。
	//・外枠が要らないならヌルペン(mGdiPen的にはTRANSPARENT_PEN)を使う
	//・塗りつぶさないならヌルブラシ(mGdiBrush的にはTRANSPARENT_BRUSH)を使う
	//・【注意】たとえば、半径を5とした場合中心から±5ピクセルを描くため、
	//          描かれた円の占める幅・高さは11ピクセルになります。
	//          イメージ的には、円の外側0.5ピクセル分は円の外側と考えればよいかと。
	bool Circle( INT x , INT y , INT radius );

	//円を描画する
	//指定座標を中心とした、指定半径の円を描きます
	//( x1 , y1 )-( x2 , y2 )を対角線上の頂点とする長方形に外接する円を描きます。
	//・外枠が現在のペンで、内側が現在のブラシで塗りつぶされる。
	//・外枠が要らないならヌルペン(mGdiPen的にはTRANSPARENT_PEN)を使う
	//・塗りつぶさないならヌルブラシ(mGdiBrush的にはTRANSPARENT_BRUSH)を使う
	bool Circle( INT x1 , INT y1 , INT x2 , INT y2 );

	//指定範囲を指定範囲にコピーする(その１)
	//srcdcで指定したDCの( src_x1 , src_y1 )-( src_x2 , src_y2 )を対角線上の頂点とする長方形を、
	//このオブジェクトの ( dst_x1 , dst_y1 )-( dst_x2 , dst_y2 )を対角線上の頂点とする長方形とする位置に貼り付けます。
	//ret : 成功時true
	//・コピー元とコピー先で幅・高さが違うと拡大縮小します
	bool Copy( const mGdiDC& srcdc ,							//コピー元デバイスコンテキスト
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//コピー元長方形
		INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,		//コピー先長方形
		DWORD raster = SRCCOPY );

	//指定範囲を指定範囲にコピーする(その２)
	//srcdcで指定したDCの( src_x1 , src_y1 )-( src_x2 , src_y2 )を対角線上の頂点とする長方形を、
	//このオブジェクトの ( dst_x1 , dst_y1 )を左上の頂点とする位置に貼り付けます。
	//ret : 成功時true
	bool Copy( const mGdiDC& srcdc ,							//コピー元デバイスコンテキスト
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//コピー元長方形
		INT dst_x1 , INT dst_y1 ,								//コピー先座標(左上)
		DWORD raster = SRCCOPY );

	//指定範囲を指定範囲にコピーする(その３)
	//srcdcで指定したDCの( x1 , y1 )-( x2 , y2 )を対角線上の頂点とする長方形を、
	//このオブジェクトの同一位置に貼り付けます。
	//ret : 成功時true
	bool Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , DWORD raster = SRCCOPY );

	//透明色つきで指定範囲を指定範囲にコピーする(その１)
	//srcdcで指定したDCの( src_x1 , src_y1 )-( src_x2 , src_y2 )を対角線上の頂点とする長方形を、
	//このオブジェクトの ( dst_x1 , dst_y1 )-( dst_x2 , dst_y2 )を対角線上の頂点とする長方形とする位置に貼り付けます。
	//ret : 成功時true
	//・コピー元とコピー先で幅・高さが違うと拡大縮小します
	bool Copy( const mGdiDC& srcdc ,							//コピー元デバイスコンテキスト
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//コピー元長方形
		INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,		//コピー先長方形
		const RGBQUAD& transparent_color );

	//透明色つきで指定範囲を指定範囲にコピーする(その２)
	//srcdcで指定したDCの( src_x1 , src_y1 )-( src_x2 , src_y2 )を対角線上の頂点とする長方形を、
	//このオブジェクトの ( dst_x1 , dst_y1 )を左上の頂点とする位置に貼り付けます。
	//ret : 成功時true
	bool Copy( const mGdiDC& srcdc ,							//コピー元デバイスコンテキスト
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//コピー元長方形
		INT dst_x1 , INT dst_y1 ,								//コピー先座標(左上)
		const RGBQUAD& transparent_color );

	//透明色つきで指定範囲を指定範囲にコピーする(その３)
	//srcdcで指定したDCの( x1 , y1 )-( x2 , y2 )を対角線上の頂点とする長方形を、
	//このオブジェクトの同一位置に貼り付けます。
	//ret : 成功時true
	bool Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , const RGBQUAD& transparent_color );

	//現在の位置にテキストを描画する
	//・改行は無視されます
	// str : 描画する文字列
	bool Print( const WString& str );

	//指定の位置にテキストを描画する
	//・改行は無視されます
	//( x , y )描画する位置
	// str : 描画する文字列
	bool Print( const WString& str , INT x , INT y );

	//描画したときのサイズを得る
	//・改行は無視されます
	// str : 描画する文字列
	// retSize : 描画したときのサイズ
	//※フォントに角度が設定されている場合は、正しい結果を返しません。
	//  （角度ゼロだと仮定して計算するっぽい）
	bool GetPrintSize( const WString& str , SIZE& retSize );

	//
	struct PrintOptions
	{
		using PrintHorizontalAlign = mGdiDC_Definitions::PrintHorizontalAlign;
		using PrintVerticalAlign = mGdiDC_Definitions::PrintVerticalAlign;

		//タブの大きさ(1にするとただのスペースと同じ)
		DWORD TabSize;

		//水平位置
		PrintHorizontalAlign HorizontalAlign;

		//垂直位置
		PrintVerticalAlign VerticalAlign;

		PrintOptions()
		{
			TabSize = 1;
			HorizontalAlign = PrintHorizontalAlign::H_ALIGN_LEFT;
			VerticalAlign = PrintVerticalAlign::V_ALIGN_TOP;
		}
	};

	//現在の位置にテキストを描画する
	//・垂直位置が中央揃えか下揃えの場合、改行は無視されます
	// str : 描画する文字列
	bool Print( const WString& str , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//・垂直位置が中央揃えか下揃えの場合、改行は無視されます
	//( x , y )描画する位置
	// str : 描画する文字列
	bool Print( const WString& str , INT x , INT y , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//・垂直位置が中央揃えか下揃えの場合、改行は無視されます
	//( x , y )描画する位置
	// str : 描画する文字列
	bool Print( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//( x , y )描画する位置
	// str : 描画する文字列
	bool PrintMultiline( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//( x , y )描画する位置
	// str : 描画する文字列
	bool PrintMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//・垂直位置が中央揃えか下揃えの場合、改行は無視されます
	//( x , y )描画する位置
	// str : 描画する文字列
	bool PrintOffset( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//( x , y )描画する位置
	// str : 描画する文字列
	bool PrintOffsetMultiline( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt );

	//指定の位置にテキストを描画する
	//( x , y )描画する位置
	// str : 描画する文字列
	bool PrintOffsetMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt );

	//描画したときのサイズを得る
	//・改行は無視されます
	// str : 描画する文字列
	// retSize : 描画したときのサイズ
	//※フォントに角度が設定されている場合は、正しい結果を返しません。
	//  （角度ゼロだと仮定して計算するっぽい）
	bool GetPrintSize( const WString& str , SIZE& retSize , const PrintOptions& opt );

	//各行の描画サイズ
	using MultilineSize = std::deque< SIZE >;

	//描画したときのサイズを得る
	// str : 描画する文字列
	// retSize : 描画したときのサイズ
	//※フォントに角度が設定されている場合は、正しい結果を返しません。
	//  （角度ゼロだと仮定して計算するっぽい）
	bool GetPrintSizeMultiline( const WString& lines , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt );

	//描画したときのサイズを得る
	// str : 描画する文字列
	// retSize : 描画したときのサイズ
	//※フォントに角度が設定されている場合は、正しい結果を返しません。
	//  （角度ゼロだと仮定して計算するっぽい）
	bool GetPrintSizeMultiline( const WStringDeque& lines , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt );

	//テキストの文字色を指定
	// color : テキスト色
	bool SetTextColor( COLORREF color );

	//現在のテキストの文字色を取得
	// retColor : 現在の文字色
	// ret : 成功時真
	bool GetTextColor( COLORREF& retColor )const noexcept;

	//テキストのバックグラウンド色を指定
	// color : バックグラウンド色
	//　・指定を省略すると透明になります
	bool SetBackgroundColor( COLORREF color );

	//テキストのバックグラウンド色を指定
	// color : バックグラウンド色
	//　・指定を省略すると透明になります
	bool SetBackgroundColor( void );

	//現在の背景色を取得
	// retColor : 現在の背景色
	// retIsTransparent : 透明であれば真
	// ret : 成功時真
	bool GetBackgroundColor( COLORREF& retColor , bool& retIsTransparent )const noexcept;

	using StrechMode = mGdiDC_Definitions::StrechMode;
	//拡大・縮小時の伸縮モードを設定する
	bool SetStrechMode( StrechMode mode );

private:

	mGdiDC( const mGdiDC& src ) = delete;
	mGdiDC& operator=( const mGdiDC& src ) = delete;

	//MyHdcへの無制限アクセスが必要なため
	//mGdiMemDC : CreateCompatibleDCを呼ぶため
	//mGdiBitmap : CreateCompatibleBitmapを呼ぶため
	friend class mGdiMemDC;
	friend class mGdiBitmap;

	//ハンドルプール
	typedef std::unordered_set<HGDIOBJ> GdiObjectPool;
	GdiObjectPool MyDefaultObj;		//最初からデバイスコンテキストに関連付けられていたハンドル
	GdiObjectPool MyAttachedObj;	//現在ユーザーがデバイスコンテキストに関連付けているハンドル

protected:

	//派生クラスでインスタンスを作る前提なので、このクラスのデフォルトコンストラクタは隠しておく
	mGdiDC();

	//デバイスコンテキストのハンドル
	HDC MyHdc;

	//デバイスコンテキストに関連付けられているオブジェクトを全部元に戻す
	//ret : 成功時true
	bool ResetSelectedObject( void );

	//座標変換
	//・x1とx2、y1とy2の位置関係が反転している場合、小さい方がx1,y1になるように入れ替えます。
	//・右辺、底辺の位置を1ピクセル内側に補正します
	//x1,y1,x2,y2 : in/out 補正対象の座標
	void PositionConvert( INT& x1 , INT& y1 , INT& x2 , INT&y2 )const;

};

#endif //MGDIDC_H_INCLUDED

