//----------------------------------------------------------------------------
// ウインドウ管理（プリンター用デバイスコンテキスト）
// Copyright (C) 2019 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MGDIPRINTERDC_CPP_COMPILING
#include "mGdiPrinterDC.h"
#include "General/mErrorLogger.h"
#include "../General/mException.h"


mGdiPrinterDC::mGdiPrinterDC() noexcept
{
}

mGdiPrinterDC::~mGdiPrinterDC()
{
	if( MyHdc != nullptr )
	{
		Abort();
		RaiseError( g_ErrorLogger , 0 , L"印刷途中でオブジェクトを破棄しました" );
	}
	return;
}

bool mGdiPrinterDC::Open( const Option& opt ) noexcept
{
	if( MyHdc != nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタを二重に開こうとしました" );
		return false;
	}

	//作成方法の指定に応じた方法でプリンタを開く
	if( opt.method == Option::CreateMethod::USEOPTION )
	{
		//設定の保持
		const Option_UseOption* op = (const Option_UseOption*)&opt;
		MyOption.reset( mNew Option_UseOption( *op ) );

		//デバイスコンテキストを開く
		MyHdc = CreateDCW( nullptr , op->PrinterName.c_str() , nullptr , nullptr );
		if( MyHdc == nullptr )
		{
			RaiseError( g_ErrorLogger , 0 , L"プリンタを開くことができません" , op->PrinterName );
			return false;
		}

		//ドキュメントの開始
		DOCINFO doc = {};
		doc.cbSize = sizeof( doc );
		doc.lpszDocName = op->DocumentName.c_str();
		if( op->OutFileName == L"" )
		{
			doc.lpszOutput = nullptr;
		}
		else
		{
			doc.lpszOutput = op->OutFileName.c_str();
		}
		doc.lpszDatatype = nullptr;
		doc.fwType = 0;

		SetLastError( 0 );
		if( StartDocW( MyHdc , &doc ) <= 0 )
		{
			RaiseError( g_ErrorLogger , 0 , L"ドキュメントを開くことができません" , op->PrinterName );
			return false;
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタオプションが不正です" );
		return false;
	}
	return true;
}

bool mGdiPrinterDC::Close( void ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	SetLastError( 0 );
	if( ::EndDoc( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"印刷を終了できませんでした" , MyOption->DocumentName );
		return false;
	}

	//選択されているGDIオブジェクトを全部元に戻す
	//※これをやらないとリソースリークするかも
	ResetSelectedObject();

	//デバイスコンテキストとオプションはもう要らないから破棄する
	DeleteDC( MyHdc );
	MyHdc = nullptr;
	MyOption.reset();
	return true;
}

bool mGdiPrinterDC::Abort( void ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	SetLastError( 0 );
	if( ::AbortDoc( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"印刷の中止処理が失敗しました" , MyOption->DocumentName );
		return false;
	}

	//選択されているGDIオブジェクトを全部元に戻す
	//※これをやらないとリソースリークするかも
	ResetSelectedObject();

	//デバイスコンテキストとオプションはもう要らないから破棄する
	DeleteDC( MyHdc );
	MyHdc = nullptr;
	MyOption.reset();
	return true;
}

static bool PageSetup_StandardSize( HDC hdc , const mGdiPrinterDC::PageOption_StandardSize& opt )
{
	DEVMODEW devmode = {0};
	devmode.dmSpecVersion = DM_SPECVERSION;
	devmode.dmSize = sizeof( devmode );

	//用紙サイズの設定
	devmode.dmFields |= DM_PAPERSIZE;
	devmode.dmPaperSize = static_cast<short>( opt.Paper );

	//用紙の向き
	switch( opt.Orientation )
	{
	case mGdiPrinterDC::PageOption_StandardSize::PaperOrientation::PAPER_LANDSCAPE:
		devmode.dmFields |= DM_ORIENTATION;
		devmode.dmOrientation = DMORIENT_LANDSCAPE;
		break;
	case mGdiPrinterDC::PageOption_StandardSize::PaperOrientation::PAPER_PORTRAIT:
		devmode.dmFields |= DM_ORIENTATION;
		devmode.dmOrientation = DMORIENT_PORTRAIT;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"用紙サイズの設定が間違っています" , opt.Orientation );
		break;
	}

	//カラー印刷？
	devmode.dmFields |= DM_COLOR;
	devmode.dmColor = ( opt.IsColor ) ? ( DMCOLOR_COLOR ) : ( DMCOLOR_MONOCHROME );

	//両面印刷設定
	switch( opt.Duplex )
	{
	case mGdiPrinterDC::PageOption_StandardSize::PaperDuplex::PAPER_SIMPLEX:
		devmode.dmFields |= DM_DUPLEX;
		devmode.dmDuplex = DMDUP_SIMPLEX;
		break;
	case mGdiPrinterDC::PageOption_StandardSize::PaperDuplex::PAPER_DUPLEX_HORIZONTAL:
		devmode.dmFields |= DM_DUPLEX;
		devmode.dmDuplex = DMDUP_HORIZONTAL;
		break;
	case mGdiPrinterDC::PageOption_StandardSize::PaperDuplex::PAPER_DUPLEX_VERTICAL:
		devmode.dmFields |= DM_DUPLEX;
		devmode.dmDuplex = DMDUP_VERTICAL;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"両面印刷の設定が間違っています" , opt.Duplex );
		break;
	}

	//設定
	ResetDCW( hdc , &devmode );
	return true;
}

static bool PageSetup_Nochange( HDC hdc , const mGdiPrinterDC::PageOption_Nochange& opt )
{
	return true;
}

bool mGdiPrinterDC::StartPage( const PageOption& opt ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	//用紙設定の変更
	bool rc;
	if( opt.method == PageOption::CreateMethod::STANDARD_SIZE )
	{
		//規格サイズに変更する場合
		rc = PageSetup_StandardSize( MyHdc , (PageOption_StandardSize&)opt );
	}
	else if( opt.method == PageOption::CreateMethod::NOCHANGE )
	{
		//前ページと同じの場合
		rc = PageSetup_Nochange( MyHdc , (PageOption_Nochange&)opt );
	}
	else
	{
		rc = false;
	}
	if( !rc )
	{
		RaiseError( g_ErrorLogger , 0 , L"用紙設定を変更できませんでした" , MyOption->DocumentName );
	}

	SetLastError( 0 );
	if( ::StartPage( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"ページを開始できませんでした" , MyOption->DocumentName );
		return false;
	}
	return true;
}

bool mGdiPrinterDC::EndPage( void ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	SetLastError( 0 );
	if( ::EndPage( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"ページを終了できませんでした" , MyOption->DocumentName );
		return false;
	}
	return true;
}

bool mGdiPrinterDC::GetPrintableSize( SIZE& retSize ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	int sx = GetDeviceCaps( MyHdc , HORZRES );					//画面の幅(ピクセル単位)
	int sy = GetDeviceCaps( MyHdc , VERTRES );					//画面の高さ(ピクセル単位)

	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	retSize.cx = ( LONG )( ( 254.0 * sx ) / dpi_x ) - 1;		//計算値の1ピクセル内側までが印刷可能範囲
	retSize.cy = ( LONG )( ( 254.0 * sy ) / dpi_y ) - 1;		//
	return true;
}

//用紙のサイズを得る
bool mGdiPrinterDC::GetPaperSize( SIZE& retSize ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	int sx = GetDeviceCaps( MyHdc , PHYSICALWIDTH );			//画面の幅(デバイス単位)
	int sy = GetDeviceCaps( MyHdc , PHYSICALHEIGHT );			//画面の高さ(デバイス単位)

	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	retSize.cx = ( LONG )( ( 254.0 * sx ) / dpi_x + 0.5 );
	retSize.cy = ( LONG )( ( 254.0 * sy ) / dpi_y + 0.5 );
	return true;
}

bool mGdiPrinterDC::GetPrintableMargin( RECT& retMargin ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	int margin_x = GetDeviceCaps( MyHdc , PHYSICALOFFSETX );	//左マージン(デバイス単位)
	int margin_y = GetDeviceCaps( MyHdc , PHYSICALOFFSETY );	//上マージン(デバイス単位)
	int paper_w = GetDeviceCaps( MyHdc , PHYSICALWIDTH );		//用紙の幅(デバイス単位)
	int paper_h = GetDeviceCaps( MyHdc , PHYSICALHEIGHT );		//用紙の高さ(デバイス単位)
	int print_w = GetDeviceCaps( MyHdc , HORZRES );				//印刷可能範囲の幅(ピクセル単位)
	int print_h = GetDeviceCaps( MyHdc , VERTRES );				//印刷可能範囲の高さ(ピクセル単位)
	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	SIZE papersize;
	papersize.cx = ( LONG )( ( 254.0 * paper_w ) / dpi_x );
	papersize.cy = ( LONG )( ( 254.0 * paper_h ) / dpi_y );

	SIZE printsize;
	printsize.cx = ( LONG )( ( 254.0 * print_w ) / dpi_x );
	printsize.cy = ( LONG )( ( 254.0 * print_h ) / dpi_y );

	retMargin.left = ( LONG )( ( 254.0 * margin_x ) / ( dpi_x ) ) + 1;
	retMargin.top = ( LONG )( ( 254.0 * margin_y ) / ( dpi_y ) ) + 1;
	retMargin.right = papersize.cx - ( retMargin.left + printsize.cx ) + 1;
	retMargin.bottom = papersize.cy - ( retMargin.top + printsize.cy ) + 1;
	return true;
}

bool mGdiPrinterDC::SetView( Origin origin , const SIZE& size ) noexcept
{
	RECT margin;
	margin.left = 0;
	margin.top = 0;
	margin.right = 0;
	margin.bottom = 0;

	return SetView( origin , size , margin );
}

bool mGdiPrinterDC::SetView( Origin origin , const SIZE& size , const RECT& margin ) noexcept
{

	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"プリンタが開かれていません" );
		return false;
	}

	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	RECT margin_dev;
	margin_dev.left = (LONG)( margin.left / 254.0 * dpi_x );
	margin_dev.top = (LONG)( margin.top / 254.0 * dpi_y );
	margin_dev.right = (LONG)( margin.right / 254.0 * dpi_x );
	margin_dev.bottom = (LONG)( margin.bottom / 254.0 * dpi_y );

	using err = class ErrorClass : public mExceptionBase{};
	try
	{
		if( origin == Origin::ORIGIN_PRINTABLEAREA )
		{
			Check<err>( SetMapMode( MyHdc , MM_ANISOTROPIC) );
			int sx = GetDeviceCaps( MyHdc , HORZRES );				//画面の幅(デバイス単位)
			int sy = GetDeviceCaps( MyHdc , VERTRES );				//画面の高さ(デバイス単位)		
			Check<err>( SetViewportExtEx( MyHdc , 
				sx - MyOption->PrintAreaMargin * 2 - margin_dev.left - margin_dev.right ,
				sy - MyOption->PrintAreaMargin * 2 - margin_dev.top - margin_dev.bottom ,
				nullptr ) );	//論理座標系
			Check<err>( SetWindowExtEx( MyHdc , size.cx , size.cy , nullptr ) );	//物理座標系

			Check<err>( SetViewportOrgEx( MyHdc ,
				MyOption->PrintAreaMargin + margin_dev.left ,
				MyOption->PrintAreaMargin + margin_dev.top ,
				nullptr ) );
		}
		else if( origin == Origin::ORIGIN_PAPEREDGE )
		{
			Check<err>( SetMapMode( MyHdc , MM_ANISOTROPIC) );
			int px = GetDeviceCaps( MyHdc , PHYSICALWIDTH );		//画面の幅(デバイス単位)
			int py = GetDeviceCaps( MyHdc , PHYSICALHEIGHT );		//画面の高さ(デバイス単位)		
			Check<err>( SetViewportExtEx( MyHdc ,
				px - MyOption->PrintAreaMargin * 2 - margin_dev.left - margin_dev.right ,
				py - MyOption->PrintAreaMargin * 2 - margin_dev.top - margin_dev.bottom ,
				nullptr ) );	//論理座標系
			Check<err>( SetWindowExtEx( MyHdc , size.cx , size.cy , nullptr ) );	//物理座標系

			int margin_l = GetDeviceCaps( MyHdc , PHYSICALOFFSETX );		//左マージン(デバイス単位)
			int margin_u = GetDeviceCaps( MyHdc , PHYSICALOFFSETY );		//上マージン(デバイス単位)
			Check<err>( SetViewportOrgEx( MyHdc ,
				MyOption->PrintAreaMargin + margin_dev.left - margin_l ,
				MyOption->PrintAreaMargin + margin_dev.top - margin_u,
				nullptr ) );
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"原点の設定値が異常です" , MyOption->DocumentName );
			return false;
		}
	}
	catch( err& e )
	{
		SetLastError( e.GetErrorCode() );
		RaiseError( g_ErrorLogger , 0 , L"座標設定が失敗しました" , MyOption->DocumentName );
		return false;
	}

	return true;
}


