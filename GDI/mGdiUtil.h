//----------------------------------------------------------------------------
// GDI用ユーティリティルーチン
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDI用の便利そうな関数群

*/
#ifndef MGDIUTIL_H_INCLUDED
#define MGDIUTIL_H_INCLUDED

#include "mStandard.h"

//COLORREFをRGBQUADに変換する
//src : 変換元のCOLORREF
//ret : 変換したRGBQUAD構造体
inline RGBQUAD COLORREF2RGBQUAD( COLORREF src )
{
	RGBQUAD rgb;
	rgb.rgbRed = GetRValue( src );
	rgb.rgbGreen = GetGValue( src );
	rgb.rgbBlue = GetBValue( src );
	rgb.rgbReserved = 0;
	return rgb;
}

//RGBQUADをCOLORREFに変換する
//src : 変換元のRGBQUAD構造体
//ret : 変換したCOLORREF
inline COLORREF RGBQUAD2COLORREF( const RGBQUAD& src )
{
	return RGB( src.rgbRed , src.rgbGreen , src.rgbBlue );
}

COLORREF HSV2COLORLEF( double H , double S , double V );

#endif //MGDIUTIL_H_INCLUDED


