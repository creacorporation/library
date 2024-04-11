//----------------------------------------------------------------------------
// 色情報管理
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#include "mGdiColor.h"
#include "General/mErrorLogger.h"

mGdiColor::mGdiColor()
{
	MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
	MyColor.RGB.clear();
}

mGdiColor::~mGdiColor()
{
}

mGdiColor::mGdiColor( const mGdiColor& source )
{
	MyPrimaryColorSpace = source.MyPrimaryColorSpace;
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB = source.MyColor.RGB;
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV = source.MyColor.HSV;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
		MyColor.RGB.clear();
		break;
	}
	return;
}

const mGdiColor& mGdiColor::operator=( const mGdiColor& source )
{
	MyPrimaryColorSpace = source.MyPrimaryColorSpace;
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB = source.MyColor.RGB;
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV = source.MyColor.HSV;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
		MyColor.RGB.clear();
		break;
	}
	return *this;
}

//値の設定(コンストラクタVer)
mGdiColor::mGdiColor( const RGBColor& source )
{
	*this = source;
}

//値の設定(コンストラクタVer)
mGdiColor::mGdiColor( const HSVColor& source )
{
	*this = source;
}

//値の設定(コンストラクタVer)
mGdiColor::mGdiColor( const RGBQUAD& source )
{
	*this = source;
}


void mGdiColor::Set( const RGBColor& col )
{
	*this = col;
}

void mGdiColor::Set( const HSVColor& col )
{
	*this = col;
}

void mGdiColor::Set( const RGBQUAD& col )
{
	*this = col;
}

void mGdiColor::Set( const BrR& col )
{
	RGBColor c( *this );
	c.R = col;
	*this = c;
}

void mGdiColor::Set( const BrG& col )
{
	RGBColor c( *this );
	c.G = col;
	*this = c;
}

void mGdiColor::Set( const BrB& col )
{
	RGBColor c( *this );
	c.B = col;
	*this = c;
}

void mGdiColor::Set( const BrA& col )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB.A = col;
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV.A = col;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}
	return;
}

void mGdiColor::Set( const BrH& col )
{
	HSVColor c( *this );
	c.H = col;
	*this = c;
}

void mGdiColor::Set( const BrS& col )
{
	HSVColor c( *this );
	c.S = col;
	*this = c;
}

void mGdiColor::Set( const BrV& col )
{
	HSVColor c( *this );
	c.V = col;
	*this = c;
}

void mGdiColor::Get( RGBColor& retCol )const
{
	retCol = *this;
	return;
}

void mGdiColor::Get( HSVColor& retCol )const
{
	retCol = *this;
	return;
}

void mGdiColor::Get( RGBQUAD& retCol )const
{
	retCol = *this;
	return;
}

mGdiColor::operator RGBColor( void )const
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		return MyColor.RGB;
	case PrimaryColorSpace::COLORSPACE_HSV:
		return HSVtoRGB( MyColor.HSV );
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}

	RGBColor tmp;
	tmp.clear();
	return tmp;
}

mGdiColor::operator HSVColor( void )const
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		return RGBtoHSV( MyColor.RGB );
	case PrimaryColorSpace::COLORSPACE_HSV:
		return MyColor.HSV;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}

	HSVColor tmp;
	tmp.clear();
	return tmp;
}

mGdiColor::operator RGBQUAD( void )const
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		return RGBtoRGBQUAD( MyColor.RGB );
	case PrimaryColorSpace::COLORSPACE_HSV:
		return HSVtoRGBQUAD( MyColor.HSV );
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}

	RGBQUAD tmp;
	tmp.rgbRed = 0;
	tmp.rgbGreen = 0;
	tmp.rgbBlue = 0;
	tmp.rgbReserved = 0;
	return tmp;
}

const mGdiColor& mGdiColor::operator=( const RGBColor& src )
{
	MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
	MyColor.RGB = src;
	return *this;
}

const mGdiColor& mGdiColor::operator=( const HSVColor& src )
{
	MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_HSV;
	MyColor.HSV = src;
	return *this;
}

const mGdiColor& mGdiColor::operator=( const RGBQUAD& src )
{
	MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
	MyColor.RGB = RGBQUADtoRGB( src );
	return *this;
}

mGdiColor::HSVColor mGdiColor::RGBtoHSV( const RGBColor& src )
{
	HSVColor hsv;
	RGBColor rgb;

	//RGB値の上限／下限チェック
	auto Limit = []( DOUBLE v ) -> DOUBLE
	{
		if( 1.0 <= v )
		{
			return 1.0;
		}
		if( v < 0.0 )
		{
			return 0.0;
		}
		return v;
	};

	rgb.R = Limit( src.R );
	rgb.G = Limit( src.G );
	rgb.B = Limit( src.B );
	rgb.A = Limit( src.A );

	//色相・明度・彩度の変換
	if( rgb.R < rgb.G )
	{
		if( rgb.G <= rgb.B )
		{
			//R<G<B	 MAX=B  MIN=R
			//R<G=B	 MAX=B  MIN=R
			hsv.H = ( 1.0 / 6.0 ) * ( 4 + ( rgb.R - rgb.B ) / ( rgb.B - rgb.R ) );
			hsv.S = ( rgb.B - rgb.R ) / ( rgb.B );
			hsv.V = rgb.B;
		}
		else if( rgb.B <= rgb.R )
		{
			//B<R<G	 MAX=G  MIN=B
			//B=R<G	 MAX=G  MIN=B
			hsv.H = ( 1.0 / 6.0 ) * ( 2 + ( rgb.B - rgb.G ) / ( rgb.G - rgb.B ) );
			hsv.S = ( rgb.G - rgb.B ) / ( rgb.G );
			hsv.V = rgb.G;
		}
		else
		{
			//R<B<G  MAX=G  MIN=R
			//R<B=G  MAX=G  MIN=R
			//R=B<G  MAX=G  MIN=R 
			hsv.H = ( 1.0 / 6.0 ) * ( 2 + ( rgb.B - rgb.G ) / ( rgb.G - rgb.R ) );
			hsv.S = ( rgb.G - rgb.R ) / ( rgb.G );
			hsv.V = rgb.G;
		}
	}
	else if( rgb.G < rgb.R )
	{
		if( rgb.B <= rgb.G )
		{
			//B<G<R  MAX=R  MIN=B
			//B=G<R  MAX=R  MIN=B
			hsv.H = ( 1.0 / 6.0 ) * ( ( rgb.G - rgb.R ) / ( rgb.R - rgb.B ) );
			hsv.S = ( rgb.R - rgb.B ) / ( rgb.R );
			hsv.V = rgb.R;
		}
		else if( rgb.R <= rgb.B )
		{
			//G<R<B  MAX=B  MIN=G
			//G<R=B  MAX=B  MIN=G
			hsv.H = ( 1.0 / 6.0 ) * ( 4 + ( rgb.R - rgb.B ) / ( rgb.B - rgb.G ) );
			hsv.S = ( rgb.B - rgb.G ) / ( rgb.B );
			hsv.V = rgb.B;
		}
		else
		{
			//G<B<R  MAX=R  MIN=G
			//G<B=R  MAX=R  MIN=G
			//G=B<R  MAX=R  MIN=G
			hsv.H = ( 1.0 / 6.0 ) * ( ( rgb.G - rgb.R ) / ( rgb.R - rgb.G ) );
			hsv.S = ( rgb.R - rgb.G ) / ( rgb.R );
			hsv.V = rgb.R;
		}
	}
	else
	{
		if( rgb.B < rgb.R )
		{
			//B<R=G MAX=R  MIN=B
			hsv.H = ( 1.0 / 6.0 ) * ( ( rgb.G - rgb.R ) / ( rgb.R - rgb.B ) );
			hsv.S = ( rgb.R - rgb.B ) / ( rgb.R );
			hsv.V = rgb.R;
		}
		else if( rgb.R < rgb.B )
		{
			//R=G<B MAX=B  MIN=R
			hsv.H = ( 1.0 / 6.0 ) * ( 4 + ( rgb.R - rgb.B ) / ( rgb.B - rgb.R ) );
			hsv.S = ( rgb.B - rgb.R ) / ( rgb.B );
			hsv.V = rgb.B;
		}
		else
		{
			//R=G=B
			hsv.H = 0.0;
			hsv.S = 0.0;
			hsv.V = rgb.R;
		}
	}

	//色相がマイナスになっている場合の補正
	if( hsv.H < 0.0 )
	{
		hsv.H += 1.0;
	}

	//アルファ値はそのままコピー
	hsv.A = rgb.A;

	return hsv;
}

mGdiColor::HSVColor mGdiColor::RGBQUADtoHSV( const RGBQUAD& src )
{
	RGBColor rgb = RGBQUADtoRGB( src );
	return RGBtoHSV( rgb );
}

mGdiColor::RGBColor mGdiColor::HSVtoRGB( const HSVColor& src )
{
	HSVColor hsv = src;
	RGBColor rgb;

	//色相(H)は0～1に補正
	DOUBLE integer = 0.0;	//整数部(ダミー)
	hsv.H = modf( src.H , &integer );
	if( hsv.H < 0 )
	{
		hsv.H += 1.0;
	}

	//色相以外(SVA)は上限／下限のチェック
	auto Limit = []( DOUBLE v ) -> DOUBLE
	{
		if( 1.0 <= v )
		{
			return 1.0;
		}
		if( v < 0.0 )
		{
			return 0.0;
		}
		return v;
	};
	hsv.S = Limit( src.S );
	hsv.V = Limit( src.V );
	hsv.A = Limit( src.A );

	//RGBへの変換
	if( hsv.S == 0.0 )
	{
		rgb.R = hsv.V;
		rgb.G = hsv.V;
		rgb.B = hsv.V;
	}
	else if( hsv.H < ( 1.0 / 6.0 ) )
	{
		rgb.R = hsv.V;
		rgb.G = ( hsv.V ) * ( 1 - ( hsv.S * ( 1 - ( hsv.H - ( 0.0 / 6.0 ) ) ) ) );
		rgb.B = ( hsv.V ) * ( 1 - hsv.S );
	}
	else if( hsv.H < ( 2.0 / 6.0 ) )
	{
		rgb.R = ( hsv.V ) * ( 1 - ( hsv.S * ( hsv.H - ( 1.0 / 6.0 ) ) ) ); 
		rgb.G = ( hsv.V );
		rgb.B = ( hsv.V ) * ( 1 - hsv.S );
	}
	else if( hsv.H < ( 3.0 / 6.0 ) )
	{
		rgb.R = ( hsv.V ) * ( 1 - hsv.S );
		rgb.G = ( hsv.V );
		rgb.B = ( hsv.V ) * ( 1 - ( hsv.S * ( 1 - ( hsv.H - ( 2.0 / 6.0 ) ) ) ) );
	}
	else if( hsv.H < ( 4.0 / 6.0 ) )
	{
		rgb.R = ( hsv.V ) * ( 1 - hsv.S );
		rgb.G = ( hsv.V ) * ( 1 - ( hsv.S * ( hsv.H - ( 3.0 / 6.0 ) ) ) ); 
		rgb.B = ( hsv.V );
	}
	else if( hsv.H < ( 5.0 / 6.0 ) )
	{
		rgb.R = ( hsv.V ) * ( 1 - ( hsv.S * ( 1 - ( hsv.H - ( 4.0 / 6.0 ) ) ) ) );
		rgb.G = ( hsv.V ) * ( 1 - hsv.S );
		rgb.B = ( hsv.V );
	}
	else
	{
		rgb.R = ( hsv.V );
		rgb.G = ( hsv.V ) * ( 1 - hsv.S );
		rgb.B = ( hsv.V ) * ( 1 - ( hsv.S * ( hsv.H - ( 5.0 / 6.0 ) ) ) ); 
	}

	//アルファ値はそのままコピー
	rgb.A = hsv.A;
	return rgb;
}

mGdiColor::RGBColor mGdiColor::RGBQUADtoRGB( const RGBQUAD& src )
{
	RGBColor rgb;
	rgb.R = ( 1.0 / 512.0 ) + ( src.rgbRed / 256.0 );
	rgb.G = ( 1.0 / 512.0 ) + ( src.rgbGreen / 256.0 );
	rgb.B = ( 1.0 / 512.0 ) + ( src.rgbBlue / 256.0 );
	rgb.A = ( 1.0 / 512.0 ) + ( src.rgbReserved / 256.0 );
	return rgb;
}

RGBQUAD mGdiColor::HSVtoRGBQUAD( const HSVColor& src )
{
	RGBColor rgb = HSVtoRGB( src );
	return RGBtoRGBQUAD( rgb );
}

RGBQUAD mGdiColor::RGBtoRGBQUAD( const RGBColor& src )
{
	auto Calc = []( DOUBLE v ) -> BYTE
	{
		v += ( 1.0 / 512.0 );
		if( 1.0 <= v )
		{
			return 255;
		}
		if( v < 0 )
		{
			return 0;
		}
		return (BYTE)( 256.0 * v );
	};

	RGBQUAD rgb;
	rgb.rgbRed = Calc( src.R );
	rgb.rgbGreen = Calc( src.G );
	rgb.rgbBlue = Calc( src.B );
	rgb.rgbReserved = Calc( src.A );
	return rgb;
}

const mGdiColor& mGdiColor::operator+=( const RGBColor& src )
{
	ChangeToRGB();
	MyColor.RGB.R += src.R;
	MyColor.RGB.G += src.G;
	MyColor.RGB.B += src.B;
	MyColor.RGB.A += src.A;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const HSVColor& src )
{
	ChangeToHSV();
	MyColor.HSV.H += src.H;
	MyColor.HSV.S += src.S;
	MyColor.HSV.V += src.V;
	MyColor.HSV.A += src.A;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const RGBQUAD& src )
{
	RGBColor rgb = RGBQUADtoRGB( src );
	return this->operator+=( rgb );
}

const mGdiColor& mGdiColor::operator+=( const BrR& src )
{
	ChangeToRGB();
	MyColor.RGB.R += src;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const BrG& src )
{
	ChangeToRGB();
	MyColor.RGB.G += src;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const BrB& src )
{
	ChangeToRGB();
	MyColor.RGB.B += src;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const BrA& src )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB.A += src;
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV.A += src;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const BrH& src )
{
	ChangeToHSV();
	MyColor.HSV.H += src;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const BrS& src )
{
	ChangeToHSV();
	MyColor.HSV.S += src;
	return *this;
}

const mGdiColor& mGdiColor::operator+=( const BrV& src )
{
	ChangeToHSV();
	MyColor.HSV.V += src;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const RGBColor& src )
{
	ChangeToRGB();
	MyColor.RGB.R -= src.R;
	MyColor.RGB.G -= src.G;
	MyColor.RGB.B -= src.B;
	MyColor.RGB.A -= src.A;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const HSVColor& src )
{
	ChangeToHSV();
	MyColor.HSV.H -= src.H;
	MyColor.HSV.S -= src.S;
	MyColor.HSV.V -= src.V;
	MyColor.HSV.A -= src.A;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const RGBQUAD& src )
{
	RGBColor rgb = RGBQUADtoRGB( src );
	return this->operator-=( rgb );
}

const mGdiColor& mGdiColor::operator-=( const BrR& src )
{
	ChangeToRGB();
	MyColor.RGB.R -= src;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const BrG& src )
{
	ChangeToRGB();
	MyColor.RGB.G -= src;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const BrB& src )
{
	ChangeToRGB();
	MyColor.RGB.B -= src;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const BrA& src )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB.A -= src;
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV.A -= src;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const BrH& src )
{
	ChangeToHSV();
	MyColor.HSV.H -= src;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const BrS& src )
{
	ChangeToHSV();
	MyColor.HSV.S -= src;
	return *this;
}

const mGdiColor& mGdiColor::operator-=( const BrV& src )
{
	ChangeToHSV();
	MyColor.HSV.V -= src;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const RGBColor& src )
{
	ChangeToRGB();
	MyColor.RGB.R *= src.R;
	MyColor.RGB.G *= src.G;
	MyColor.RGB.B *= src.B;
	MyColor.RGB.A *= src.A;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const HSVColor& src )
{
	ChangeToHSV();
	MyColor.HSV.H *= src.H;
	MyColor.HSV.S *= src.S;
	MyColor.HSV.V *= src.V;
	MyColor.HSV.A *= src.A;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const RGBQUAD& src )
{
	RGBColor rgb = RGBQUADtoRGB( src );
	return this->operator*=( rgb );
}

const mGdiColor& mGdiColor::operator*=( const BrR& src )
{
	ChangeToRGB();
	MyColor.RGB.R *= src;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const BrG& src )
{
	ChangeToRGB();
	MyColor.RGB.G *= src;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const BrB& src )
{
	ChangeToRGB();
	MyColor.RGB.B *= src;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const BrA& src )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB.A *= src;
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV.A *= src;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const BrH& src )
{
	ChangeToHSV();
	MyColor.HSV.H *= src;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const BrS& src )
{
	ChangeToHSV();
	MyColor.HSV.S *= src;
	return *this;
}

const mGdiColor& mGdiColor::operator*=( const BrV& src )
{
	ChangeToHSV();
	MyColor.HSV.V *= src;
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const RGBColor& src )
{
	ChangeToRGB();
	MyColor.RGB.R = ( src.R == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.R / src.R );
	MyColor.RGB.G = ( src.G == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.G / src.G );
	MyColor.RGB.B = ( src.B == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.B / src.B );
	MyColor.RGB.A = ( src.A == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.A / src.A );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const HSVColor& src )
{
	ChangeToHSV();
	MyColor.HSV.H = ( src.H == 0.0 ) ? ( 0.0 ) : ( MyColor.HSV.H / src.H );
	MyColor.HSV.S = ( src.S == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.S / src.S );
	MyColor.HSV.V = ( src.V == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.V / src.V );
	MyColor.HSV.A = ( src.A == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.A / src.A );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const RGBQUAD& src )
{
	RGBColor rgb = RGBQUADtoRGB( src );
	return this->operator/=( rgb );
}

const mGdiColor& mGdiColor::operator/=( const BrR& src )
{
	ChangeToRGB();
	MyColor.RGB.R = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.R / src );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const BrG& src )
{
	ChangeToRGB();
	MyColor.RGB.G = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.G / src );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const BrB& src )
{
	ChangeToRGB();
	MyColor.RGB.B = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.B / src );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const BrA& src )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyColor.RGB.A = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.RGB.A / src );
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyColor.HSV.A = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.A / src );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		break;
	}
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const BrH& src )
{
	ChangeToHSV();
	MyColor.HSV.H = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.H / src );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const BrS& src )
{
	ChangeToHSV();
	MyColor.HSV.S = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.S / src );
	return *this;
}

const mGdiColor& mGdiColor::operator/=( const BrV& src )
{
	ChangeToHSV();
	MyColor.HSV.V = ( src == 0.0 ) ? ( 1.0 ) : ( MyColor.HSV.V / src );
	return *this;
}

void mGdiColor::ChangeToRGB( void )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
		MyColor.RGB = HSVtoRGB( MyColor.HSV );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_RGB;
		MyColor.RGB.clear();
		break;
	}
	return;
}

void mGdiColor::ChangeToHSV( void )
{
	switch( MyPrimaryColorSpace )
	{
	case PrimaryColorSpace::COLORSPACE_RGB:
		MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_HSV;
		MyColor.HSV = RGBtoHSV( MyColor.RGB );
		break;
	case PrimaryColorSpace::COLORSPACE_HSV:
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"色空間のIDが不正値です" , MyPrimaryColorSpace );
		MyPrimaryColorSpace = PrimaryColorSpace::COLORSPACE_HSV;
		MyColor.HSV.clear();
		break;
	}
	return;
}

