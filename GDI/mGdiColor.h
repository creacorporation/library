//----------------------------------------------------------------------------
// 色情報管理
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MGDICOLOR_H_INCLUDED
#define MGDICOLOR_H_INCLUDED

#include "mStandard.h"

class mGdiColor
{
public:

	mGdiColor();
	virtual ~mGdiColor();
	mGdiColor( const mGdiColor& source );
	const mGdiColor& operator=( const mGdiColor& source );

	//ＨＳＶ色空間による色表現
	//・最小=0〜最大=1となりますが、値としてはその範囲外の値も保持できます。
	//これらの値は、HSV色空間・RGBQUADとの変換時に0〜1の範囲に補正されます
	struct RGBColor
	{
		DOUBLE R;	//0≦R≦1に正規化されたＲ値
		DOUBLE G;	//0≦R≦1に正規化されたＧ値
		DOUBLE B;	//0≦R≦1に正規化されたＢ値
		DOUBLE A;	//0≦R≦1に正規化されたアルファ値

		void clear( void )
		{
			R = 0.0;
			G = 0.0;
			B = 0.0;
			A = 0.0;
		}
	};

	//ＨＳＶ色空間による色表現
	//・色相は1周=1とした値です。
	//・彩度、明度、アルファ値は、最小=0〜最大=1となりますが、値としてはその範囲外の値も保持できます。
	//これらの値は、RGB色空間・RGBQUADとの変換時に0〜1の範囲に補正されます
	struct HSVColor
	{
		DOUBLE H;	//一周＝1.0に正規化された色相
		DOUBLE S;	//0≦R≦1に正規化された彩度
		DOUBLE V;	//0≦R≦1に正規化された明度
		DOUBLE A;	//0≦R≦1に正規化されたアルファ値

		void clear( void )
		{
			H = 0.0;
			S = 0.0;
			V = 0.0;
			A = 0.0;
		}
	};

	//輝度値
	template< class T > struct Brightness
	{
		DOUBLE v;
		Brightness( DOUBLE src )
		{
			v = src;
		}
		operator DOUBLE()const
		{
			return v;
		}
	};

	//赤輝度値(0〜1)
	using BrR = Brightness< struct BrR >;
	//緑輝度値(0〜1)
	using BrG = Brightness< struct BrG >;
	//青輝度値(0〜1)
	using BrB = Brightness< struct BrB >;
	//アルファ値(0〜1)
	using BrA = Brightness< struct BrA >;
	//色相(0〜1)
	using BrH = Brightness< struct BrH >;
	//彩度(0〜1)
	using BrS = Brightness< struct BrS >;
	//明度(0〜1)
	using BrV = Brightness< struct BrV >;

	//---------------------------------------------
	//値設定系
	//---------------------------------------------

	//値の設定(コンストラクタVer)
	// source : 設定したい値
	mGdiColor( const RGBColor& source );

	//値の設定(コンストラクタVer)
	// source : 設定したい値
	mGdiColor( const HSVColor& source );

	//値の設定(コンストラクタVer)
	// source : 設定したい値
	mGdiColor( const RGBQUAD& source );

	//値の設定(コンストラクタVer)
	// source : 設定したい値
	mGdiColor( COLORREF source );

	//値の設定
	// col : 設定したい値
	void Set( const RGBColor& col );

	//値の設定
	// col : 設定したい値
	void Set( const HSVColor& col );

	//値の設定
	// col : 設定したい値
	void Set( const RGBQUAD& col );

	//値の設定
	// col : 設定したい値
	void Set( COLORREF col );

	//値の設定（赤輝度値のみ）
	// col : 設定したい値
	void Set( const BrR& col );

	//値の設定（緑輝度値のみ）
	// col : 設定したい値
	void Set( const BrG& col );

	//値の設定（青輝度値のみ）
	// col : 設定したい値
	void Set( const BrB& col );

	//値の設定（アルファ値のみ）
	// col : 設定したい値
	void Set( const BrA& col );

	//値の設定（色相値のみ）
	// col : 設定したい値
	void Set( const BrH& col );

	//値の設定（彩度値のみ）
	// col : 設定したい値
	void Set( const BrS& col );

	//値の設定（明度値のみ）
	// col : 設定したい値
	void Set( const BrV& col );

	//値の設定
	// src : 設定したい値
	const mGdiColor& operator=( const RGBColor& src );

	//値の設定
	// src : 設定したい値
	const mGdiColor& operator=( const HSVColor& src );

	//値の設定
	// src : 設定したい値
	const mGdiColor& operator=( const RGBQUAD& src );

	//値の設定
	// src : 設定したい値
	const mGdiColor& operator=( COLORREF src );

	//---------------------------------------------
	//値取得系
	//---------------------------------------------

	//値の取得
	//現在の値がRGBではない場合は、RGBに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// retCol : 取得した値
	void Get( RGBColor& retCol )const;

	//値の取得
	//現在の値がHSVではない場合は、HSVに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// retCol : 取得した値
	void Get( HSVColor& retCol )const;

	//値の取得
	//現在の値がRGBではない場合は、RGBに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// retCol : 取得した値
	void Get( RGBQUAD& retCol )const;

	//値の取得
	//現在の値がRGBではない場合は、RGBに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// retCol : 取得した値
	void Get( COLORREF& retCol )const;

	//値の取得
	//現在の値がRGBではない場合は、RGBに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// ret : RGB値
	operator RGBColor( void )const;

	//値の取得
	//現在の値がHSVではない場合は、HSVに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// ret : HSV値
	operator HSVColor( void )const;

	//値の取得
	//現在の値がRGBではない場合は、RGBに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// ret : RGBQUAD値
	operator RGBQUAD( void )const;

	//値の取得
	//現在の値がRGBではない場合は、RGBに変換した値を取得しますが、
	//インスタンスが保持している値は変化しません。
	// ret : RGBQUAD値
	operator COLORREF( void )const;

	//---------------------------------------------
	//値演算系
	//---------------------------------------------

	//値の加算(各RGB輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const RGBColor& src );

	//値の加算(HSV値による)
	//・現在の値がHSVではない場合、HSVに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const HSVColor& src );

	//値の加算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const RGBQUAD& src );

	//値の加算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( COLORREF src );

	//値の加算(赤輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrR& src );

	//値の加算(緑輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrG& src );

	//値の加算(青輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrB& src );

	//値の加算(アルファ値)
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrA& src );

	//値の加算(色相)
	//・現在の値がHSVではない場合、HSVに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrH& src );

	//値の加算(彩度)
	//・現在の値がHSVではない場合、HSVに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrS& src );

	//値の加算(明度)
	//・現在の値がHSVではない場合、HSVに変換してから加算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 加算する値
	// ret : 現在の値
	const mGdiColor& operator+=( const BrV& src );

	//値の減算(各RGB輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const RGBColor& src );

	//値の減算(HSV値による)
	//・現在の値がHSVではない場合、HSVに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const HSVColor& src );

	//値の減算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const RGBQUAD& src );

	//値の減算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( COLORREF src );

	//値の減算(赤輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrR& src );

	//値の減算(緑輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrG& src );

	//値の減算(青輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrB& src );

	//値の減算(アルファ値)
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrA& src );

	//値の減算(色相)
	//・現在の値がHSVではない場合、HSVに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrH& src );

	//値の減算(彩度)
	//・現在の値がHSVではない場合、HSVに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrS& src );

	//値の減算(明度)
	//・現在の値がHSVではない場合、HSVに変換してから減算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 減算する値
	// ret : 現在の値
	const mGdiColor& operator-=( const BrV& src );

	//値の乗算(各RGB輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const RGBColor& src );

	//値の乗算(HSV値による)
	//・現在の値がHSVではない場合、HSVに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const HSVColor& src );

	//値の乗算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const RGBQUAD& src );

	//値の乗算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( COLORREF src );

	//値の乗算(赤輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrR& src );

	//値の乗算(緑輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrG& src );

	//値の乗算(青輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrB& src );

	//値の乗算(アルファ値)
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrA& src );

	//値の乗算(色相)
	//・現在の値がHSVではない場合、HSVに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrH& src );

	//値の乗算(彩度)
	//・現在の値がHSVではない場合、HSVに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrS& src );

	//値の乗算(明度)
	//・現在の値がHSVではない場合、HSVに変換してから乗算します。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 乗算する値
	// ret : 現在の値
	const mGdiColor& operator*=( const BrV& src );

	//値の剰余算(各RGB輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const RGBColor& src );

	//値の剰余算(HSV値による)
	//・現在の値がHSVではない場合、HSVに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const HSVColor& src );

	//値の剰余算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const RGBQUAD& src );

	//値の剰余算(255階調RGB値)
	//・現在の値がRGBではない場合、RGBに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( COLORREF src );

	//値の剰余算(赤輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrR& src );

	//値の剰余算(緑輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrG& src );

	//値の剰余算(青輝度値)
	//・現在の値がRGBではない場合、RGBに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrB& src );

	//値の剰余算(アルファ値)
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrA& src );

	//値の剰余算(色相)
	//・現在の値がHSVではない場合、HSVに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrH& src );

	//値の剰余算(彩度)
	//・現在の値がHSVではない場合、HSVに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrS& src );

	//値の剰余算(明度)
	//・現在の値がHSVではない場合、HSVに変換してから剰余を求めます。
	//　このとき、値が0〜1の範囲にない場合は、0〜1に補正されます。
	// src : 剰余算する値
	// ret : 現在の値
	const mGdiColor& operator/=( const BrV& src );

	//---------------------------------------------
	//値変換系
	//---------------------------------------------

	//RGB色空間をHSV色空間に変換する
	//この関数は、クラスのインスタンスを作成しなくても呼び出すことが出来ます。
	static HSVColor RGBtoHSV( const RGBColor& src );

	//RGBQUAD値をHSV色空間に変換する
	//この関数は、クラスのインスタンスを作成しなくても呼び出すことが出来ます。
	static HSVColor RGBQUADtoHSV( const RGBQUAD& src ); 

	//RGB色空間をRGB色空間に変換する
	//この関数は、クラスのインスタンスを作成しなくても呼び出すことが出来ます。
	static RGBColor HSVtoRGB( const HSVColor& src );

	//RGBQUAD値をRGB色空間に変換する
	//この関数は、クラスのインスタンスを作成しなくても呼び出すことが出来ます。
	static RGBColor RGBQUADtoRGB( const RGBQUAD& src );

	//HSV色空間をRGBQUAD値変換する
	//この関数は、クラスのインスタンスを作成しなくても呼び出すことが出来ます。
	static RGBQUAD HSVtoRGBQUAD( const HSVColor& src );

	//RGB色空間をRGBQUAD値変換する
	//この関数は、クラスのインスタンスを作成しなくても呼び出すことが出来ます。
	static RGBQUAD RGBtoRGBQUAD( const RGBColor& src );

protected:

	enum PrimaryColorSpace
	{
		COLORSPACE_RGB,		//RGB色空間
		COLORSPACE_HSV,		//HSV色空間
	};
	PrimaryColorSpace MyPrimaryColorSpace;

	union Color
	{
		RGBColor RGB;
		HSVColor HSV;
	};
	Color MyColor;

	void ChangeToRGB( void );
	void ChangeToHSV( void );
};


#endif //MGDIUTIL_H_INCLUDED
