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


namespace mGdiUtil
{
	struct Point
	{
		float x = 0;
		float y = 0;
		Point()
		{
		}
		Point( int init_x , int init_y )
		{
			x = static_cast<float>( init_x );
			y = static_cast<float>( init_y );
		}
		Point( float init_x , float init_y )
		{
			x = init_x;
			y = init_y;
		}
		Point( const POINT& src )
		{
			x = static_cast<float>( src.x );
			y = static_cast<float>( src.y );
		}
		Point( const Point& src )
		{
			x = src.x;
			y = src.y;
		}
		explicit operator POINT()const
		{
			POINT p;
			p.x = static_cast<LONG>( x );
			p.y = static_cast<LONG>( y );
			return p;
		}
		Point operator+( const Point& p )const
		{
			return Point( x + p.x , y + p.y );
		}
		const Point& operator+=( const Point& p )
		{
			x += p.x;
			y += p.y;
			return *this;
		}
		Point operator-( const Point& p )const
		{
			return Point( x - p.x , y - p.y );
		}
		const Point& operator-=( const Point& p )
		{
			x -= p.x;
			y -= p.y;
			return *this;
		}
		bool IsAlmostEqual( const Point& p , float eps = 0.01f )const
		{
			return ( abs( x - p.x ) <= eps ) && ( abs( y - p.y ) <= eps );
		}
		bool operator==( const Point& p )const = delete;
		bool operator!=( const Point& p )const = delete;
	};
	struct Size
	{
		float cx = 0;
		float cy = 0;
		Size()
		{
		}
		Size( int size_x , int size_y )
		{
			cx = static_cast<float>( size_x );
			cy = static_cast<float>( size_y );
		}
		Size( float size_x , float size_y )
		{
			cx = size_x;
			cy = size_y;
		}
		Size( const SIZE& src )
		{
			cx = static_cast<float>( src.cx );
			cy = static_cast<float>( src.cy );
		}
		Size( const Size& src )
		{
			cx = src.cx;
			cy = src.cy;
		}
		Size( const Point& p1 , const Point& p2 )
		{
			cx = abs( p2.x - p1.x ) + 1;
			cy = abs( p2.y - p1.y ) + 1;
		}
		explicit operator SIZE()const
		{
			SIZE s;
			s.cx = static_cast<LONG>( cx );
			s.cy = static_cast<LONG>( cy );
			return s;
		}
		bool IsAlmostEqual( const Size& sz , float eps = 0.01f )const
		{
			return ( abs( cx - sz.cx ) <= eps ) && ( abs( cy - sz.cy ) <= eps );
		}
		bool operator==( const Size& sz )const = delete;
		bool operator!=( const Size& sz )const = delete;
	};
	struct Rect
	{
		float left = 0;
		float top = 0;
		float right = 0;
		float bottom = 0;
		Rect()
		{
		}
		Rect( const RECT& src )
		{
			left = static_cast<float>( src.left );
			top = static_cast<float>( src.top );
			right = static_cast<float>( src.right );
			bottom = static_cast<float>( src.bottom );
		};
		Rect( const Rect& src )
		{
			left = static_cast<float>( src.left );
			top = static_cast<float>( src.top );
			right = static_cast<float>( src.right );
			bottom = static_cast<float>( src.bottom );
		};
		Rect( const Point& p1 , const Point& p2 )
		{
			left = p1.x;
			top = p1.y;
			right = p2.x;
			bottom = p2.y;
		};

		enum class INIT_OPTION
		{
			NORMALIZED
		};
		Rect( const Rect& src , INIT_OPTION )
		{
			left = std::min( src.left , src.right );
			top = std::min( src.top , src.bottom );
			right = std::max( src.left , src.right );
			bottom = std::max( src.top , src.bottom );
		};
		Rect( const Point& p1 , const Point& p2 , INIT_OPTION )
		{
			left = std::min( p1.x , p2.x );
			top = std::min( p1.y , p2.y );
			right = std::max( p1.x , p2.x );
			bottom = std::max( p1.y , p2.y );
		};
		Rect GetNormalized( void )const
		{
			Rect result;
			result.left = std::min( left , right );
			result.top = std::min( top , bottom );
			result.right = std::max( left , right );
			result.bottom = std::max( top , bottom );
			return result;
		};
		explicit operator RECT()const
		{
			RECT result;
			result.left = static_cast<LONG>( left );
			result.top = static_cast<LONG>( top );
			result.right = static_cast<LONG>( right );
			result.bottom = static_cast<LONG>( bottom );
			return result;
		};
		explicit operator Size()const
		{
			Size result;
			result.cx = abs( right - left ) + 1;
			result.cy = abs( bottom - top ) + 1;
			return result;
		};
	};
};

#endif //MGDIUTIL_H_INCLUDED


