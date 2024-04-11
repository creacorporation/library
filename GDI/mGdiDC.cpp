//----------------------------------------------------------------------------
// ウインドウ管理（デバイスコンテキスト）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIDC_CPP_COMPILING
#include "mGdiDC.h"
#include "General/mErrorLogger.h"

#pragma comment( lib , "msimg32.lib" )

mGdiDC::mGdiDC()
{
	MyHdc = nullptr;
}

mGdiDC::~mGdiDC()
{
	//MyHdcのハンドルを解放する責任は派生クラス側にあります

	//デバイスコンテキストのハンドルが解放されたのに、デフォルトのオブジェクトが
	//残留している場合はリソースリークしていそうなのでエラーを記録します。
	if( MyDefaultObj.size() != 0 )
	{
		//ResetSelectedObject()を呼び出し忘れている
		RaiseAssert( g_ErrorLogger , MyDefaultObj.size() , L"HGDIOBJ is not detached" );
	}
}

//オブジェクト(ペン、ブラシ、フォントなど)を選択する
bool mGdiDC::Select( const mGdiHandle& handle )
{
	return Select( handle.GetHandle() );
}

//オブジェクト(ペン、ブラシ、フォントなど)を選択する
//＜SelectObject後のハンドルの扱い＞
//・今から関連付けようとしているオブジェクトの素性は？
// [A]MyAttachedObjに載ってる⇒すでに関連付け済み。別にSelectObject自体が必要ない
// [B]MyDefaultObjに載ってる⇒デフォルトのオブジェクトに戻るからMyDefaultObjから削除
// [C]どちらにも載ってない⇒新規のオブジェクトだからMyAttachedObjに登録
//
//・SelectObjectで関連付けた結果返ってきたオブジェクトは？
// [a]MyAttachedObjに載ってる⇒ユーザが前に関連付けたものだからMyAttachedObjから削除
// [b]そうじゃない⇒デフォルトのオブジェクトだからMyDefaultObjに登録
bool mGdiDC::Select( HGDIOBJ new_object )
{
	//IDが無効だったらエラー
	if( new_object == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Object null" );
		return false;
	}

	HGDIOBJ prev_object = ::SelectObject( MyHdc , new_object );
	if( prev_object == nullptr || prev_object == HGDI_ERROR )
	{
		//SelectObjectが失敗した場合
		RaiseAssert( g_ErrorLogger , 0 , L"SelectObject failed" );
		return false;
	}

	//変更前と後のハンドルの素性を調べて、MyAttachedObjを更新する
	if( MyAttachedObj.count( new_object ) )
	{
		//すでに選択済みのオブジェクトを再選択している場合。
		//何も変化してないからそのまま戻る。
		return true;	//上のコメントの[A]のパターン
	}
	else if( MyDefaultObj.count( new_object ) )
	{
		MyDefaultObj.erase( new_object );	//上のコメントの[B]のパターン
	}
	else
	{
		if( MyAttachedObj.count( new_object ) == 0 )
		{
			MyAttachedObj.insert( new_object );	//上のコメントの[C]のパターン
		}
		else
		{
			//ロジック上来ないはず…
			RaiseAssert( g_ErrorLogger , 0 , L"AttachedObj dupe" );
		}
	}
	//同様にMyDefaultObjを更新する
	if( MyAttachedObj.count( prev_object ) )
	{
		MyAttachedObj.erase( prev_object );	//上のコメントの[a]のパターン
	}
	else
	{
		if( MyDefaultObj.count( prev_object ) == 0 )
		{
			MyDefaultObj.insert( prev_object );	//上のコメントの[b]のパターン
		}
		else
		{
			//ロジック上来ないはず…
			RaiseAssert( g_ErrorLogger , 0 , L"DefaultObj dupe" );
		}
	}
	return true;
}

//オブジェクト(ペン、ブラシ、フォントなど)を選択する
bool mGdiDC::Select( const mGdiResource& res , const WString& id , const WString& subid )
{
	return Select( res.GetItem( id , subid ) );
}

//デバイスコンテキストに関連付けられているオブジェクトを全部元に戻す
bool mGdiDC::ResetSelectedObject( void )
{
	//全部のオブジェクトをデフォルトに戻す
	for( GdiObjectPool::iterator itr = MyDefaultObj.begin() ; itr != MyDefaultObj.end() ; itr++ )
	{
		::SelectObject( MyHdc , *itr );
	}

	//全てがデフォルトに戻っているはずだから、プールしているオブジェクトのことは忘れる
	MyAttachedObj.clear();
	MyDefaultObj.clear();
	return true;
}

//線を描画する
bool mGdiDC::Line( INT to_x , INT to_y )
{
	return ::LineTo( MyHdc , to_x , to_y ) != FALSE;
}

//線を描画する
bool mGdiDC::Line( INT from_x , INT from_y , INT to_x , INT to_y )
{
	if( !::MoveToEx( MyHdc , from_x , from_y , nullptr ) )
	{
		return false;
	}
	return Line( to_x , to_y );
}


bool mGdiDC::LineOffset( INT from_x , INT from_y , INT offset_x , INT offset_y )
{
	return Line( from_x , from_y , from_x + offset_x , from_y + offset_y );
}


//矩形を描画する
bool mGdiDC::Rectangle( INT x1 , INT y1 , INT x2 , INT y2 )
{
	//WindowsAPIのRectangleは右辺と底辺は座標に含まない（1ピクセル内側に右辺と底辺を描画する）
	//という仕様らしい。なんだかピンとこないので、1ピクセル外側にずらして補正する。
	//なんつったって、手続きBASIC世代だからな！
	PositionConvert( x1 , y1 , x2 , y2 );

	//描画系のAPIでエラーを記録すると、すぐログがあふれるので記録しない
	return ::Rectangle( MyHdc , x1 , y1 , x2 , y2 ) != FALSE;
}

bool mGdiDC::RectangleOffset( INT x1 , INT y1 , INT offset_x , INT offset_y )
{
	return Rectangle( x1 , y1 , x1 + offset_x , y1 + offset_y );
}

//円を描画する
bool mGdiDC::Circle( INT x , INT y , INT radius )
{
	return Circle( x - radius , y - radius , x + radius , y + radius );
}

//円を描画する
bool mGdiDC::Circle( INT x1 , INT y1 , INT x2 , INT y2 )
{
	//Rectangleと同じで、Ellipseも右辺と底辺は含まないので補正する
	PositionConvert( x1 , y1 , x2 , y2 );

	//描画系のAPIでエラーを記録すると、すぐログがあふれるので記録しない
	return ::Ellipse( MyHdc , x1 , y1 , x2 , y2 ) != FALSE;
}

//現在の位置にテキストを描画する
bool mGdiDC::Print( const WString& str )
{
	POINT current;
	if( !GetCurrentPositionEx( MyHdc , &current ) )
	{
		return false;
	}
	return Print( str , current.x ,  current.y );
}

//指定の位置にテキストを描画する
bool mGdiDC::Print( const WString& str , INT x , INT y )
{
	return TextOutW( MyHdc , x , y , str.c_str() , (int)str.length() );
}

//座標変換
void mGdiDC::PositionConvert( INT& x1 , INT& y1 , INT& x2 , INT&y2 )const
{
	INT left;
	INT right;
	if( x1 < x2 )
	{
		left = x1;
		right = x2 + 1;	//←この+1が補正分
	}
	else
	{
		left = x2;
		right = x1 + 1;
	}
	INT top;
	INT bottom;
	if( y1 < y2 )
	{
		top = y1;
		bottom = y2 + 1;
	}
	else
	{
		top = y2;
		bottom = y1 + 1;

	}

	x1 = left;
	y1 = top;
	x2 = right;
	y2 = bottom;
	return;
}

//指定範囲を指定範囲にコピーする(その１)
bool mGdiDC::Copy( const mGdiDC& srcdc , 
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 , 
	INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,
	DWORD raster )
{
	//座標変換
	PositionConvert( src_x1 , src_y1 , src_x2 , src_y2 );
	PositionConvert( dst_x1 , dst_y1 , dst_x2 , dst_y2 );

	INT src_w = src_x2 - src_x1;	//ソースの幅
	INT src_h = src_y2 - src_y1;	//ソースの高さ
	INT dst_w = dst_x2 - dst_x1;	//貼り付け先の幅
	INT dst_h = dst_y2 - dst_y1;	//貼り付け先の高さ

	//コピー元とコピー先で幅が同一であるかを判定し、Bitbltを使うか、StretchBltを使うか決める
	if( ( src_w == dst_w ) && ( src_h == dst_h ) )
	{
		//幅と高さが同じであるからbitblt
		return ::BitBlt( MyHdc, dst_x1 , dst_y1 , src_w , src_h , srcdc.MyHdc , src_x1 , src_y1 , raster ) != FALSE;
	}
	else
	{
		//幅と高さが異なっているのでStretchBlt
		return ::StretchBlt( MyHdc , dst_x1 , dst_y1 , dst_w , dst_h , srcdc.MyHdc , src_x1 , src_y1 , src_w , src_h , raster ) != FALSE;
	}
}


//指定範囲を指定範囲にコピーする(その２)
bool mGdiDC::Copy( const mGdiDC& srcdc ,
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,
	INT dst_x1 , INT dst_y1 , DWORD raster )
{
	//コピー先の右・下の座標を、コピー元の幅・高さを使って求める
	INT width;	//幅
	if( src_x1 < src_x2 )
	{
		width = src_x2 - src_x1 + 1;
	}
	else
	{
		width = src_x1 - src_x2 + 1;
	}
	INT height;	//高さ
	if( src_x1 < src_x2 )
	{
		height = src_y2 - src_y1 + 1;
	}
	else
	{
		height = src_y1 - src_y2 + 1;
	}

	//求めた幅・高さを補って、「その１」を呼び出す。やや不経済か？どうでもいいか。
	return Copy( srcdc , src_x1 , src_y1 , src_x2 , src_y2 , dst_x1 , dst_y1 , dst_x1 + width , dst_y1 + height , raster );
}

//指定範囲を指定範囲にコピーする(その３)
bool mGdiDC::Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , DWORD raster )
{
	//座標は同じなので、コピー先にもコピー元の座標をそのまま使っちゃいます
	return Copy( srcdc , x1 , y1 , x2 , y2 , x1 , y1 , x2 , y2 , raster );
}

bool mGdiDC::GetPrintSize( const WString& str , SIZE& retSize )
{
	return GetTextExtentPoint32W( MyHdc , str.c_str() , (int)str.length() , &retSize );
}

static bool MakeDrawTextParameter( const mGdiDC::PrintOptions& opt , UINT& retformat , DRAWTEXTPARAMS& retparams )
{
	bool result = true;
	retformat = 0;
	retformat |= DT_NOPREFIX;

	retparams = { 0 };
	retparams.cbSize = sizeof( DRAWTEXTPARAMS );

	//垂直位置
	switch( opt.VerticalAlign )
	{
	case mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_TOP:
		//retformat |= DT_TOP;
		break;
	case mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_CENTER:
		retformat |= DT_VCENTER;
		retformat |= DT_SINGLELINE;
		break;
	case mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_BOTTOM:
		retformat |= DT_BOTTOM;
		retformat |= DT_SINGLELINE;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"垂直位置が不正です" );
		result = false;
		break;
	}

	//水平位置
	switch( opt.HorizontalAlign )
	{
	case mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_LEFT:
		retformat |= DT_LEFT;
		break;
	case mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_CENTER:
		retformat |= DT_CENTER;
		break;
	case mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_RIGHT:
		retformat |= DT_RIGHT;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"水平位置が不正です" );
		result = false;
		break;
	}

	//タブの展開
	if( 2 <= opt.TabSize )
	{
		retparams.iTabLength = opt.TabSize;
		retformat |= DT_EXPANDTABS;		//タブの展開を行う
		retformat |= DT_TABSTOP;		//タブサイズの設定を行う
	}
	return true;
}

//現在の位置にテキストを描画する
bool mGdiDC::Print( const WString& str , const PrintOptions& opt )
{
	//現在の位置を取得して
	POINT current;
	if( !GetCurrentPositionEx( MyHdc , &current ) )
	{
		return false;
	}

	//描画
	return Print( str , current.x , current.y , opt );
}

//指定の位置にテキストを描画する
bool mGdiDC::Print( const WString& str , INT x , INT y , const PrintOptions& opt )
{
	//APIに渡すフォーマット情報を作成
	UINT format ;
	DRAWTEXTPARAMS option;

	MakeDrawTextParameter( opt , format , option );
	format |= DT_NOCLIP;	//←左上しか指定されていないので、クリップなし

	//座標指定して
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x;
	rect.bottom = y;

	//描画
	return DrawTextEx( MyHdc , const_cast<LPWSTR>( str.c_str() ) , (int)str.length() ,  &rect , format , &option );
}

//指定の位置にテキストを描画する
bool mGdiDC::Print( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt )
{
	//APIに渡すフォーマット情報を作成
	UINT format ;
	DRAWTEXTPARAMS option;

	MakeDrawTextParameter( opt , format , option );
	//左上・右下ともに指定されているので、クリップあり。
	format |= DT_NOCLIP;	

	//座標指定して
	RECT rect;
	rect.left = x1;
	rect.top = y1;
	rect.right = x2;
	rect.bottom = y2;

	//描画
	return DrawTextEx( MyHdc , const_cast<LPWSTR>( str.c_str() ) , (int)str.length() ,  &rect , format , &option );
}

bool mGdiDC::PrintMultiline( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt )
{
	//行単位にパース
	WStringDeque lines;
	ParseString( str , lines , false );

	return PrintMultiline( lines , x1 , y1 , x2 , y2 , opt );
}


bool mGdiDC::PrintMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt )
{
	//ユーザーの指定してきた座標について加工
	int top = ( y1 < y2 ) ? ( y1 ) : ( y2 );		//ユーザーが指定してきた描画位置の上端
	int bottom = ( y1 < y2 ) ? ( y2 ) : ( y1 );		//ユーザーが指定してきた描画位置の下端
	SIZE areasize;									//ユーザーが指定してきた描画サイズ
	areasize.cx = ( x1 < x2 ) ? ( x2 - x1 ) : ( x1 - x2 );
	areasize.cy = bottom - top;

	//各行のサイズを求める
	SIZE reqsize = { 0 };		//総サイズ
	MultilineSize reqlinesize;	//行毎のサイズ
	if( !GetPrintSizeMultiline( lines , reqsize , reqlinesize , opt ) )
	{
		return false;
	}

	//求めた全体の描画に必要な範囲が、指定エリアに納まっているか確認して、収まっていないなら補正する
	//X（水平）方向についてはAPIがやってくれるから、Y（垂直）方向にのみ処理する
	RECT print_rect;	//実際に描画を行う範囲
	print_rect.left = x1;
	print_rect.right = x2;

	if( areasize.cy < reqsize.cy )
	{
		//入らない
		int diff = reqsize.cy - areasize.cy;	//←不足分
		switch( opt.VerticalAlign )
		{
		case PrintOptions::PrintVerticalAlign::V_ALIGN_TOP:
			print_rect.top = top;
			print_rect.bottom = bottom + diff;
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_CENTER:
			print_rect.top = top - ( diff / 2 );
			print_rect.bottom = bottom + ( diff / 2 );
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_BOTTOM:
			print_rect.top = top - diff;
			print_rect.bottom = bottom;
			break;
		default:
			return false;
		}
	}
	else
	{
		//入る
		int diff = areasize.cy - reqsize.cy;	//←余ったスペース
		switch( opt.VerticalAlign )
		{
		case PrintOptions::PrintVerticalAlign::V_ALIGN_TOP:
			print_rect.top = top;
			print_rect.bottom = top + diff;
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_CENTER:
			print_rect.top = top + ( diff / 2 );
			print_rect.bottom = bottom - ( diff / 2 );
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_BOTTOM:
			print_rect.top = bottom - diff;
			print_rect.bottom = bottom;
			break;
		default:
			return false;
		}
	}

	//APIに渡すフォーマット情報を作成
	UINT format ;
	DRAWTEXTPARAMS option;
	MakeDrawTextParameter( opt , format , option );

	//描画する
	int current_y = print_rect.top;
	for( size_t i = 0 ; i < lines.size() ; i++ )
	{
		//描画位置を決定
		RECT area;
		area.left = print_rect.left;
		area.right = print_rect.right;
		area.top = current_y;
		area.bottom = current_y + reqlinesize[ i ].cy;

		if( bottom < area.top )
		{
			//以降印刷範囲超えにつき印刷できない
			break;
		}
		if( top < area.bottom )
		{
			//印刷可能範囲
			if( !DrawTextEx( MyHdc , const_cast<LPWSTR>( lines[ i ].c_str() ) , (int)lines[ i ].length() , &area , format , &option ) )
			{
				return false;
			}
		}
		current_y += reqlinesize[ i ].cy;
	}
	return true;
}


//指定の位置にテキストを描画する
bool mGdiDC::PrintOffset( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt )
{
	return Print( str , x1 , y1 , x1 + offset_x , y1 + offset_y , opt );
}

//描画したときのサイズを得る
bool mGdiDC::GetPrintSize( const WString& str , SIZE& retSize , const PrintOptions& opt )
{
	//APIに渡すフォーマット情報を作成
	PrintOptions tmp_opt = opt;
	tmp_opt.VerticalAlign = mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_TOP;
	tmp_opt.HorizontalAlign = mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_LEFT;

	UINT format ;
	DRAWTEXTPARAMS option;
	MakeDrawTextParameter( tmp_opt , format , option );
	format |= DT_CALCRECT;
	format |= DT_NOCLIP;

	RECT rect = { 0 };
	if( !DrawTextEx( MyHdc , const_cast<LPWSTR>( str.c_str() ) , (int)str.length() ,  &rect , format , &option ) )
	{
		return false;
	}
	retSize.cx = ( rect.left < rect.right ) ? ( rect.right - rect.left ) : ( rect.left - rect.right );
	retSize.cx++;
	retSize.cy = ( rect.top < rect.bottom ) ? ( rect.bottom - rect.top ) : ( rect.top - rect.bottom );
	retSize.cy++;
	return true;
}

//指定の位置にテキストを描画する
bool mGdiDC::PrintOffsetMultiline( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt )
{
	return PrintMultiline( str , x1 , y1 , x1 + offset_x , y1 + offset_y , opt );
}

//指定の位置にテキストを描画する
bool mGdiDC::PrintOffsetMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt )
{
	return PrintMultiline( lines , x1 , y1 , x1 + offset_x , y1 + offset_y , opt );
}

bool mGdiDC::GetPrintSizeMultiline( const WString& str , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt )
{
	WStringDeque lines;
	ParseString( str , lines , false );
	return GetPrintSizeMultiline( lines , retSize , retLineSize , opt );
}

bool mGdiDC::GetPrintSizeMultiline( const WStringDeque& lines , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt )
{
	retSize.cx = 0;
	retSize.cy = 0;
	retLineSize.clear();

	//APIに渡すフォーマット情報を作成
	PrintOptions tmp_opt = opt;
	tmp_opt.VerticalAlign = mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_TOP;
	tmp_opt.HorizontalAlign = mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_LEFT;

	UINT format ;
	DRAWTEXTPARAMS option;
	MakeDrawTextParameter( tmp_opt , format , option );
	format |= DT_CALCRECT;
	format |= DT_NOCLIP;

	//描画に必要な矩形を求める
	for( WStringDeque::const_iterator itr = lines.begin() ; itr != lines.end() ; itr++ )
	{
		//1行分のサイズを求める
		RECT rect = { 0 };
		if( !DrawTextEx( MyHdc , const_cast<LPWSTR>( (*itr).c_str() ) , (int)(*itr).length() , &rect , format , &option ) )
		{
			return false;
		}

		//で、それを合計して、全体の描画に必要な範囲を求める
		SIZE sz;
		sz.cx = ( rect.left < rect.right ) ? ( rect.right - rect.left + 1 ) : ( rect.left - rect.right + 1 );
		sz.cy = ( rect.top < rect.bottom ) ? ( rect.bottom - rect.top + 1 ) : ( rect.top - rect.bottom + 1 );

		if( retSize.cx < sz.cx )
		{
			retSize.cx = sz.cx;
		}
		retSize.cy += sz.cy;

		//さらに、各行のサイズも結果として返す
		retLineSize.push_back( std::move( sz ) );
	}
	return true;
}


//テキストの文字色を指定
bool mGdiDC::SetTextColor( COLORREF color )
{
	::SetTextColor( MyHdc , color );
	return true;
}

//テキストのバックグラウンド色を指定
bool mGdiDC::SetBackgroundColor( COLORREF color )
{
	SetBkMode( MyHdc , OPAQUE );
	SetBkColor( MyHdc , color );
	return true;
}

//テキストのバックグラウンド色を指定
bool mGdiDC::SetBackgroundColor( void )
{
	SetBkMode( MyHdc , TRANSPARENT );
	return true;
}

//現在のテキストの文字色を取得
bool mGdiDC::GetTextColor( COLORREF& retColor )const noexcept
{
	retColor = ::GetTextColor( MyHdc );
	if( retColor == CLR_INVALID )
	{
		return false;
	}
	return true;
}

//現在の背景色を取得
bool mGdiDC::GetBackgroundColor( COLORREF& retColor , bool& retIsTransparent )const noexcept
{
	int IsTransparent = ::GetBkMode( MyHdc );
	retIsTransparent = ( IsTransparent == TRANSPARENT );
	retColor = ::GetBkColor( MyHdc );

	if( IsTransparent == 0 || retColor == CLR_INVALID )
	{
		return false;
	}
	return true;
}

//透明色つきで指定範囲を指定範囲にコピーする(その１)
bool mGdiDC::Copy( const mGdiDC& srcdc ,							//コピー元デバイスコンテキスト
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//コピー元長方形
	INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,		//コピー先長方形
	const RGBQUAD& transparent_color )
{
	//座標変換
	PositionConvert( src_x1 , src_y1 , src_x2 , src_y2 );
	PositionConvert( dst_x1 , dst_y1 , dst_x2 , dst_y2 );

	INT src_w = src_x2 - src_x1;	//ソースの幅
	INT src_h = src_y2 - src_y1;	//ソースの高さ
	INT dst_w = dst_x2 - dst_x1;	//貼り付け先の幅
	INT dst_h = dst_y2 - dst_y1;	//貼り付け先の高さ

	UINT transparent = ( transparent_color.rgbRed   << 16 ) |
					   ( transparent_color.rgbGreen <<  8 ) |
					   ( transparent_color.rgbBlue  <<  0 ) ;

	return ::TransparentBlt( MyHdc , dst_x1 , dst_y1 , dst_w , dst_h , srcdc.MyHdc , src_x1 , src_y1 , src_w , src_h , transparent );
}

//透明色つきで指定範囲を指定範囲にコピーする(その２)
bool mGdiDC::Copy( const mGdiDC& srcdc ,							//コピー元デバイスコンテキスト
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//コピー元長方形
	INT dst_x1 , INT dst_y1 ,								//コピー先座標(左上)
	const RGBQUAD& transparent_color )
{
	//コピー先の右・下の座標を、コピー元の幅・高さを使って求める
	INT width;	//幅
	if( src_x1 < src_x2 )
	{
		width = src_x2 - src_x1 + 1;
	}
	else
	{
		width = src_x1 - src_x2 + 1;
	}
	INT height;	//高さ
	if( src_x1 < src_x2 )
	{
		height = src_y2 - src_y1 + 1;
	}
	else
	{
		height = src_y1 - src_y2 + 1;
	}

	//求めた幅・高さを補って、「その１」を呼び出す。
	return Copy( srcdc , src_x1 , src_y1 , src_x2 , src_y2 , dst_x1 , dst_y1 , dst_x1 + width , dst_y1 + height , transparent_color );
}


//透明色つきで指定範囲を指定範囲にコピーする(その３)
bool mGdiDC::Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , const RGBQUAD& transparent_color )
{
	//座標は同じなので、コピー先にもコピー元の座標をそのまま使っちゃいます
	return Copy( srcdc , x1 , y1 , x2 , y2 , x1 , y1 , x2 , y2 , transparent_color );
}

bool mGdiDC::SetStrechMode( StrechMode mode )
{

	int new_mode;
	switch( mode )
	{
	case StrechMode::STRECH_BLACKONWHITE:
		new_mode = BLACKONWHITE;
		break;
	case StrechMode::STRECH_WHITEONBLACK:
		new_mode = WHITEONBLACK;
		break;
	case StrechMode::STRECH_COLORONCOLOR:
		new_mode = COLORONCOLOR;
		break;
	case StrechMode::STRECH_HALFTONE:
		new_mode = HALFTONE;
		break;
	default:
		return false;
	}
	return SetStretchBltMode( MyHdc , new_mode );
}



