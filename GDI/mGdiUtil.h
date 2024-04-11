//----------------------------------------------------------------------------
// GDI�p���[�e�B���e�B���[�`��
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI�p�֗̕������Ȋ֐��Q

*/
#ifndef MGDIUTIL_H_INCLUDED
#define MGDIUTIL_H_INCLUDED

#include "mStandard.h"

//COLORREF��RGBQUAD�ɕϊ�����
//src : �ϊ�����COLORREF
//ret : �ϊ�����RGBQUAD�\����
inline RGBQUAD COLORREF2RGBQUAD( COLORREF src )
{
	RGBQUAD rgb;
	rgb.rgbRed = GetRValue( src );
	rgb.rgbGreen = GetGValue( src );
	rgb.rgbBlue = GetBValue( src );
	rgb.rgbReserved = 0;
	return rgb;
}

//RGBQUAD��COLORREF�ɕϊ�����
//src : �ϊ�����RGBQUAD�\����
//ret : �ϊ�����COLORREF
inline COLORREF RGBQUAD2COLORREF( const RGBQUAD& src )
{
	return RGB( src.rgbRed , src.rgbGreen , src.rgbBlue );
}

COLORREF HSV2COLORLEF( double H , double S , double V );

#endif //MGDIUTIL_H_INCLUDED


