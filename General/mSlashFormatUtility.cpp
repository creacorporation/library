//----------------------------------------------------------------------------
// スラッシュ区切りデータフォーマット
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------


#include "mSlashFormatUtility.h"
#include "mTCHAR.h"

void mSlashFormatUtility::Parse( const AString& src , mSlashFormatA& retData )
{
	retData.Parse( src );
	return;
}

void mSlashFormatUtility::Parse( const WString& src , mSlashFormatW& retData )
{
	retData.Parse( src );
	return;
}

//文字列のスラッシュを//に置換する
WString mSlashFormatUtility::EscapeSlash( const WString& src )
{
	return ReplaceString( src , L"/" , L"//" , nullptr );
}

//文字列のスラッシュを//に置換する
AString mSlashFormatUtility::EscapeSlash( const AString& src )
{
	return ReplaceString( src , "/" , "//" , nullptr );
}
