//----------------------------------------------------------------------------
// ウインドウ管理（GDIフォント）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIFONT_CPP_COMPILING
#include "mGdiFont.h"
#include "General/mErrorLogger.h"

/*
LONG  lfHeight;                  // 文字セルまたは文字の高さ
LONG  lfWidth;                   // 平均文字幅
LONG  lfEscapement;              // 文字送りの方向とX軸との角度
LONG  lfOrientation;             // ベースラインとX軸との角度
LONG  lfWeight;                  // フォントの太さ
BYTE  lfItalic;                  // イタリック体指定
BYTE  lfUnderline;               // 下線付き指定
BYTE  lfStrikeOut;               // 打ち消し線付き指定
BYTE  lfCharSet;                 // キャラクタセット
BYTE  lfOutPrecision;            // 出力精度
BYTE  lfClipPrecision;           // クリッピングの精度
BYTE  lfQuality;                 // 出力品質
BYTE  lfPitchAndFamily;          // ピッチとファミリ
TCHAR lfFaceName[LF_FACESIZE];   // フォント名  
*/

mGdiFont::mGdiFont( const Option* opt )noexcept( false )
{
	MyHandle = nullptr;
	if( opt != nullptr )
	{
		//オプションが指定されていた場合、それを使ってフォントを生成する
		if( opt->method == Option::CreateMethod::USEOPTION )
		{
			MakeFont( (Option_UseOption*)opt );
		}
		else
		{
			throw EXCEPTION( opt->method , L"Unknown create method" );
		}
	}
	else
	{
		//オプションが指定されていなかった場合(nullptrだった場合)
		//ダミーのオプション構造体(全デフォルトで初期化される)を渡す
		Option_UseOption temp_opt;
		MakeFont( &temp_opt );
	}

	//ハンドルが生成されていればOK、そうでなければ例外を投げる
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"CreateFontW failed" );
	}
	return;
}

mGdiFont::~mGdiFont()
{
	::DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//ハンドルの値を取得する(キャスト演算子バージョン)
mGdiFont::operator HFONT()const
{
	return MyHandle;
}

//ハンドルの値を取得する(普通の関数バージョン)
HGDIOBJ mGdiFont::GetHandle( void )const
{
	return MyHandle;
}

bool mGdiFont::MakeFont( const Option_UseOption* opt )
{
	//MyHandleにすでに値がある場合はエラー
	if( MyHandle != nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Handle is already allocated" );
		return false;
	}

	//以下、オプションが指定されているときの処理
	LOGFONT font;
	font.lfHeight = opt->height;	//フォントの高さ
	font.lfWidth = opt->width;		//フォントの幅
	font.lfEscapement = opt->orientation;	//文字送りの回転方向
	font.lfOrientation = opt->orientation;					//文字の回転方法
	font.lfWeight = ( opt->isbold ) ? ( FW_BOLD ) : ( FW_DONTCARE );	//ボールドにする？
	font.lfItalic = ( opt->isitalic ) ? ( TRUE ) : ( FALSE );			//イタリックにする？
	font.lfUnderline = ( opt->isunderline ) ? ( TRUE ) : ( FALSE );		//アンダーライン欲しい？
	font.lfStrikeOut = ( opt->isstrikeout ) ? ( TRUE ) : ( FALSE );		//打ち消し線を付ける？
	font.lfOutPrecision = OUT_DEFAULT_PRECIS;	//出力精度
	font.lfClipPrecision = CLIP_DEFAULT_PRECIS;	//クリッピング精度
	font.lfPitchAndFamily = FF_DONTCARE;		//フォントファミリ

	//キャラクタセットの指定
	switch( opt->charset )
	{
	case Option::FontCharSet::ANSI:			//英語
		font.lfCharSet = ANSI_CHARSET;
		break;
	case Option::FontCharSet::BALTIC:		//バルト三国の文字(リトアニア語、ラトビア語等)
		font.lfCharSet = BALTIC_CHARSET;
		break;
	case Option::FontCharSet::GB2312:		//簡体字中国語(中国、シンガポール)
		font.lfCharSet = GB2312_CHARSET;
		break;
	case Option::FontCharSet::CHINESEBIG5:	//繁体字中国語(中華民国、香港、マカオ)
		font.lfCharSet = CHINESEBIG5_CHARSET;
		break;
	case Option::FontCharSet::EASTEUROPE:	//東欧系言語の文字
		font.lfCharSet = EASTEUROPE_CHARSET;
		break;
	case Option::FontCharSet::GREEK:		//ギリシャ語
		font.lfCharSet = GREEK_CHARSET;
		break;
	case Option::FontCharSet::HANGUL:		//韓国語
		font.lfCharSet = HANGUL_CHARSET;
		break;
	case Option::FontCharSet::SHIFTJIS:		//日本語
		font.lfCharSet = SHIFTJIS_CHARSET;
		break;
	case Option::FontCharSet::RUSSIAN:		//ロシア語
		font.lfCharSet = RUSSIAN_CHARSET;
		break;
	case Option::FontCharSet::TURKISH:		//トルコ語
		font.lfCharSet = TURKISH_CHARSET;
		break;
	case Option::FontCharSet::HEBREW:		//ヘブライ語
		font.lfCharSet = HEBREW_CHARSET;
		break;
	case Option::FontCharSet::ARABIC:		//アラビア語
		font.lfCharSet = ARABIC_CHARSET;
		break;
	case Option::FontCharSet::THAI:			//タイ語
		font.lfCharSet = THAI_CHARSET;
		break;
	case Option::FontCharSet::LOCALE:		//現在のロケール(言語設定)に基づいて決める
		font.lfCharSet = DEFAULT_CHARSET;
		break;
	case Option::FontCharSet::OEM:			//何語版ウインドウズかで決める
		font.lfCharSet = OEM_CHARSET;
		break;
	default:
		//範囲外の値が指定されてきた場合は、仕方が無いからデフォルトにしておく
		RaiseAssert( g_ErrorLogger , opt->charset , L"Bad charset id" );
		font.lfCharSet = DEFAULT_CHARSET;
		break;
	}
	//出力品質
	switch( opt->quality )
	{
	case Option::PrintQuality::NORMAL:
		font.lfQuality = DEFAULT_QUALITY;
		break;
	case Option::PrintQuality::ANTIALIAESED:
		font.lfQuality = ANTIALIASED_QUALITY;
		break;
	case Option::PrintQuality::CLEARTYPE:
		font.lfQuality = CLEARTYPE_QUALITY;
		break;
	default:
		RaiseAssert( g_ErrorLogger , opt->quality , L"Bad font quality id" );
		font.lfQuality = DEFAULT_QUALITY;
		break;
	}
	//フォントピッチ
	switch( opt->pitch )
	{
	case Option::PrintPitch::DEFAULT:	//フォント任せにする
		font.lfPitchAndFamily |= DEFAULT_PITCH;
		break;
	case Option::PrintPitch::FIXED:		//固定幅にする
		font.lfPitchAndFamily |= FIXED_PITCH;
		break;
	case Option::PrintPitch::VARIABLE:	//可変幅にする
		font.lfPitchAndFamily |= VARIABLE_PITCH;
		break;
	default:
		RaiseAssert( g_ErrorLogger , opt->pitch , L"Bad font pitch id" );
		font.lfPitchAndFamily |= DEFAULT_PITCH;
		break;
	}
	//フォント名
	if( LF_FACESIZE <= opt->name.size() )
	{
		//バッファに入りきらない場合は空文字列にする。
		//バッファには終端のヌル文字も含めなければならないのに注意
		RaiseAssert( g_ErrorLogger , opt->name.size() , L"フォント名が長すぎます" , opt->name );
		font.lfFaceName[ 0 ] = L'\0';
	}
	else
	{
		//バッファに入りきる場合は普通に書き込む
		wchar_sprintf( font.lfFaceName , L"%s" , opt->name.c_str() );
	}

	//ハンドルを生成する
	MyHandle = ::CreateFontIndirectW( &font );
	if( MyHandle == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"フォントの生成が失敗しました" );
		return false;
	}
	return true;
}

