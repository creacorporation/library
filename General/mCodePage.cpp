//----------------------------------------------------------------------------
// コードページハンドラ
// Copyright (C) 2020 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MCODEPAGE_CPP_COMPILING
#include "mCodePage.h"
#include "General/mErrorLogger.h"

static UINT CodePage2WinDefinition( mCodePage::CodePage cp )
{
	UINT code;
	switch( cp )
	{
	case mCodePage::CodePage::SHIFTJIS:
		code = 932;
		break;
	case mCodePage::CodePage::EUCJP:
		code = 20932;
		break;
	case mCodePage::CodePage::UTF8:
		code = 65001;
		break;
	case mCodePage::CodePage::UTF16:
		code = 1200;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"コードページの値が不正です" );
		code = 0;
		break;
	}
	return code;
}

static mCodePage::CodePage WinDefinition2CodePage( UINT cp )
{
	mCodePage::CodePage  code;
	switch( cp )
	{
	case 932:
		code = mCodePage::CodePage::SHIFTJIS;
		break;
	case 20932:
		code = mCodePage::CodePage::EUCJP;
		break;
	case 65001:
		code = mCodePage::CodePage::UTF8;
		break;
	case 1200:
		code = mCodePage::CodePage::UTF16;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"コードページの値が不明です" );
		code = mCodePage::CodePage::UNKNOWN;
		break;
	}
	return code;
}

//コードページを設定します
bool mCodePage::SetConsoleInputCodePage( CodePage cp )
{
	if( !SetConsoleCP( CodePage2WinDefinition( cp ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"コードページの設定は失敗しました" );
		return false;
	}
	return true;
}

//コードページを取得します
bool mCodePage::GetConsoleInputCodePage( CodePage& retCp )
{
	retCp = WinDefinition2CodePage( GetConsoleCP() );
	return retCp != mCodePage::CodePage::UNKNOWN;
}

//コードページを設定します
bool mCodePage::SetConsoleOutputCodePage( CodePage cp )
{
	if( !SetConsoleOutputCP( CodePage2WinDefinition( cp ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"コードページの設定は失敗しました" );
		return false;
	}
	return true;
}

//コードページを取得します
bool mCodePage::GetConsoleOutputCodePage( CodePage& retCp )
{
	retCp = WinDefinition2CodePage( GetConsoleOutputCP() );
	return retCp != mCodePage::CodePage::UNKNOWN;
}



