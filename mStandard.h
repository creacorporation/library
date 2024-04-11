//----------------------------------------------------------------------------
// 基本ヘッダ
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MSTANDARD_H_INCLUDED
#define MSTANDARD_H_INCLUDED

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif //_DEBUG

#include <winsock2.h>
#include <windows.h>
#include <cstddef>
#include <new>
#include <sal.h>
#pragma warning( disable : 4482 )

//スレッドセーフであることを示すマーカー
#define threadsafe /**/

//デバッグ版new、deleteの定義
#ifdef _DEBUG
#include <crtdbg.h>
#define mNew ::new(_NORMAL_BLOCK, __FILE__ ":" __FUNCSIG__ , __LINE__ ) 
#define mPlacementNew(a) ::new( a )
#define mDelete ::delete
#else
#define mNew ::new 
#define mDelete ::delete
#define mPlacementNew(a) ::new( a )
#endif //_DEBUG

//外部参照の解決
#ifdef DEFINE_EXTERN_VALUE
#define mEXTERN /**/
#define mGLOBAL_INITIAL_VALUE(v) =(v)
#else
#define mEXTERN extern
#define mGLOBAL_INITIAL_VALUE(v)
#endif

//配列の要素数を求める
template<typename t , size_t s> constexpr size_t array_count_of( const t(&array)[ s ] )
{
	return s;
}

//ワイド文字列化マクロ( L"hoge"ができない__FILE__等の組み込みマクロ用 )
#define mWCHAR_STRING_INT(quote) L##quote
#define mWCHAR_STRING(quote) mWCHAR_STRING_INT(quote)

//現在コンパイル中のファイル名
//__FILE__からパスを取り除いた物になります。
#define mCURRENT_FILE ( wcsrchr( mWCHAR_STRING(__FILE__) , L'\\') ? wcsrchr( mWCHAR_STRING(__FILE__)  , L'\\' ) + 1 : mWCHAR_STRING(__FILE__) )
#define mCURRENT_FUNCTION mWCHAR_STRING(__FUNCTION__)

//イニシャライズ
//ライブラリを初期化します。使用する前に初期化して下さい。
void InitializeLibrary( void );

//デイニシャライズ
//ライブラリの初期化を解除します。
void DeinitializeLibrary( void );

#endif //MSTANDARD_H_INCLUDED

