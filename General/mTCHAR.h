//----------------------------------------------------------------------------
// 文字型定義
// Copyright (C) 2012,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MTCHAR_H_INCLUDED
#define MTCHAR_H_INCLUDED

/*
●用途
文字列型を扱うのに便利そうな関数群です。

・std:stringのお友達、AString(char)、TString(TCHAR)、WString(wchar_t)
・tchar_*、wchar_*の定義
　TCHAR用strcpy→_tcscpyとかいちいち覚えてられないので。
・char,TCHAR,wchar_tの相互変換
・AString、WString用sprintf

*/

#pragma warning( disable : 4996 )

#include "mStandard.h"
#include "mSecureAllocator.h"
#include <tchar.h>
#include <string>
#include <deque>
#include <vector>

typedef std::string AString;
typedef std::basic_string<TCHAR> TString;
typedef std::basic_string<WCHAR> WString;

typedef std::basic_string<CHAR,mSecureAllocator<CHAR>> SecureAString;
typedef std::basic_string<TCHAR,mSecureAllocator<TCHAR>> SecureTString;
typedef std::basic_string<WCHAR,mSecureAllocator<WCHAR>> SecureWString;

typedef std::deque<AString> AStringDeque;
typedef std::deque<TString> TStringDeque;
typedef std::deque<WString> WStringDeque;

typedef std::vector<AString> AStringVector;
typedef std::vector<TString> TStringVector;
typedef std::vector<WString> WStringVector;

#include "mException.h"

#define tchar_strcpy		_tcscpy
#define tchar_strcpy_s		_tcscpy_s
#define tchar_strncpy		_tcsncpy
#define tchar_strcat		_tcscat
#define tchar_tcsncat		_tcsncat
#define tchar_strlen		_tcslen
#define tchar_strcmp		_tcscmp
#define tchar_strncmp		_tcsncmp
#define tchar_strstr		_tcsstr
#define tchar_sscanf		_stscanf
#define tchar_sprintf		_stprintf
#define tchar_scprintf		_sctprintf
#define tchar_strtok		_tcstok
#define tchar_strchr		_tcschr
#define tchar_strdup		_tcsdup
#define tchar_printf		_tprintf
#define tchar_scanf			_tscanf
#define tchar_fopen			_tfopen
#define tchar_fprintf		_ftprintf
#define tchar_fputf			_fputts
#define tchar_fgets			_fgetts
#define tchar_fscanf		_ftscanf
#define tchar_fullpath		_tfullpath
#define tchar_strnlen		_tcsnlen

#define	tchar_isalnum		_istalnum
#define	tchar_isgraph		_istgraph
#define	tchar_isalpha		_istalpha
#define	tchar_islower		_istlower
#define	tchar_isblank		_istblank
#define	tchar_isprint		_istprint
#define	tchar_iscntrl		_istcntrl
#define	tchar_ispunct		_istpunct
#define	tchar_isspace		_istspace
#define	tchar_isupper		_istupper
#define	tchar_isdigit		_istdigit
#define	tchar_isxdigit		_istxdigit
#define tchar_toupper		_totupper
#define tchar_tolower		_totlower
#define tchar_isascii		_istascii

#define	wchar_strcpy		wcscpy
#define	wchar_strcpy_s		wcscpy_s
#define	wchar_strncpy		wcsncpy
#define	wchar_strcat		wcscat
#define	wchar_tcsncat		wcsncat
#define	wchar_strlen		wcslen
#define	wchar_strcmp		wcscmp
#define	wchar_strncmp		wcsncmp
#define	wchar_strstr		wcsstr
#define	wchar_sscanf		swscanf
#define	wchar_sprintf		_swprintf
#define	wchar_scprintf		_scwprintf
#define	wchar_strtok		wcstok
#define	wchar_strchr		wcschr
#define	wchar_strdup		_wcsdup
#define	wchar_printf		wprintf
#define	wchar_scanf			wscanf
#define	wchar_fopen			_wfopen
#define	wchar_fprintf		fwprintf
#define	wchar_fputf			fputws
#define	wchar_fgets			fgetws
#define	wchar_fscanf		fwscanf
#define	wchar_fullpath		_wfullpath
#define	wchar_strnlen		wcsnlen

#define	wchar_isalnum		iswalnum
#define	wchar_isgraph		iswgraph
#define	wchar_isalpha		iswalpha
#define	wchar_islower		iswlower
#define	wchar_isblank		iswblank
#define	wchar_isprint		iswprint
#define	wchar_iscntrl		iswcntrl
#define	wchar_ispunct		iswpunct
#define	wchar_isspace		iswspace
#define	wchar_isupper		iswupper
#define	wchar_isdigit		iswdigit
#define	wchar_isxdigit		iswxdigit
#define wchar_toupper		towupper
#define wchar_tolower		towlower
#define wchar_isascii		iswascii

inline bool IsUnicode( void )
{
#ifdef UNICODE
	return TRUE;
#else
	return FALSE;
#endif
}

//CHAR型,WCHAR型,TCHAR型の相互変換
//返ってきたポインタは、mDelete[]で破棄すること。
TCHAR* WCHAR2TCHAR( const WCHAR* str );
TCHAR* CHAR2TCHAR( const CHAR* str );
WCHAR* TCHAR2WCHAR( const TCHAR* str );
CHAR* TCHAR2CHAR( const TCHAR* str );
WCHAR* CHAR2WCHAR( const CHAR* str );
CHAR* WCHAR2CHAR( const WCHAR* str );

//文字列をコピーして、その文字列を返す。
//返ってきたポインタは、mDelete[]で破棄すること。
CHAR* NewAndCopyString( const CHAR* src );
WCHAR* NewAndCopyString( const WCHAR* src );

//CHAR型,WCHAR型,TCHAR型の相互変換
TString WString2TString( const WString& src );
TString AString2TString( const AString& src );
WString TString2WString( const TString& src );
AString TString2AString( const TString& src );
WString AString2WString( const AString& src );
AString WString2AString( const WString& src );

AString AStringToUtf8( const AString& src );
AString WStringToUtf8( const WString& src );
AString Utf8ToAString( const AString& src );
WString Utf8ToWString( const AString& src );

//AStringにUTF16(LE)の文字列が格納されているとき、WStringに変換して取り出します
WString CastToWString( const AString& src );

//WStringのエンディアンを変換します(UTF16LE <-->UTF16BE)
WString SwitchEndian( const WString& src );

INT sprintf_va( AString& ret_dest , const CHAR* format , va_list va );
INT sprintf_va( AString& ret_dest , const WCHAR* format , va_list va );
INT sprintf_va( WString& ret_dest , const CHAR* format , va_list va );
INT sprintf_va( WString& ret_dest , const WCHAR* format , va_list va );

INT sprintf_va( AString* ret_dest , const CHAR* format , va_list va );
INT sprintf_va( AString* ret_dest , const WCHAR* format , va_list va );
INT sprintf_va( WString* ret_dest , const CHAR* format , va_list va );
INT sprintf_va( WString* ret_dest , const WCHAR* format , va_list va );

//注意！
//パラメータに文字列を使用するとき、%sがCHAR*型かWCHAR型かは、
//ret_destの型に依存する。すなわち、
//ret_destがString型　→	%sはconst CHAR*型
//ret_destがWString型　→	%sはconst WCHAR*型
//となる。間違えると爆発するので注意すること。

template< class t , class u > INT sprintf( t& ret_dest , const u* format , ... )
{
	ret_dest.clear();
	if( !format )
	{
		return -1;
	}

	//可変長リスト
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( ret_dest , format , args );

	//可変長引数リセット
	va_end( args );

	return result;
}

template< class t , class u > INT sprintf( t* ret_dest , const u* format , ... )
{
	if( !ret_dest )
	{
		return -1;
	}
	ret_dest->clear();
	if( !format )
	{
		return -1;
	}

	//可変長リスト
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( ret_dest , format , args );

	//可変長引数リセット
	va_end( args );

	return result;
}

//後ろに文字列を連結するsprintf
template< class t , class u > INT appendf( t& ret_dest , const u* format , ... )
{
	if( !format )
	{
		return -1;
	}
	//仮オブジェクト
	t tmpstr;

	//可変長リスト
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( tmpstr , format , args );

	//可変長引数リセット
	va_end( args );

	//文字列連結
	ret_dest.append( tmpstr );

	return result;
}

//後ろに文字列を連結するsprintf
template< class t , class u > INT appendf( t* ret_dest , const u* format , ... )
{
	if( !ret_dest )
	{
		return -1;
	}
	if( !format )
	{
		return -1;
	}

	//仮オブジェクト
	t tmpstr;

	//可変長リスト
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( ret_dest , format , args );

	//可変長引数リセット
	va_end( args );

	//文字列連結
	ret_dest->append( tmpstr );

	return result;
}

//文字列オブジェクトを返すsprintf
template< class t > std::basic_string<t> makeprintf( const t* format , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );

	//仮オブジェクト
	std::basic_string<t> tmpstr;

	if( format )
	{
		sprintf_va( tmpstr , format , args );
	}
	//可変長引数リセット
	va_end( args );

	return std::move( tmpstr );
}

//バイナリ→16進数変換
bool Binary2String( WString& ret_dest , const BYTE* dat , DWORD len );
bool Binary2String( AString& ret_dest , const BYTE* dat , DWORD len );

//文字列を大文字に変換する
AString ToUpper( const AString& src );
//文字列を大文字に変換する
WString ToUpper( const WString& src );

//文字列を小文字に変換する
AString ToLower( const AString& src );
//文字列を小文字に変換する
WString ToLower( const WString& src );

//文字列を数値に変換する
int ToInt( const AString& src , int defvalue = 0 );
//文字列を数値に変換する
int ToInt( const WString& src , int defvalue = 0 );
//文字列を数値に変換する
bool ToInt( const AString& src , int& retvalue , int defvalue );
//文字列を数値に変換する
bool ToInt( const WString& src , int& retvalue , int defvalue );

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline int ToInt( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	int result;
	if( !ToInt( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline int ToInt( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	int result;
	if( !ToInt( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//16進文字列を数値に変換する
unsigned int HexToUInt( const AString& src , unsigned int defvalue = 0 );
//16進文字列を数値に変換する
unsigned int HexToUInt( const WString& src , unsigned int defvalue = 0 );
//16進文字列を数値に変換する
bool HexToUInt( const AString& src , unsigned int& retvalue , unsigned int defvalue );
//16進文字列を数値に変換する
bool HexToUInt( const WString& src , unsigned int& retvalue , unsigned int defvalue );

//16進文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned int HexToUInt( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned int result;
	if( !HexToUInt( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//16進文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned int HexToUInt( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned int result;
	if( !HexToUInt( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//16進文字列を数値に変換する
unsigned int HexToULong( const AString& src , unsigned long defvalue = 0 );
//16進文字列を数値に変換する
unsigned int HexToULong( const WString& src , unsigned long defvalue = 0 );
//16進文字列を数値に変換する
bool HexToULong( const AString& src , unsigned long& retvalue , unsigned long defvalue );
//16進文字列を数値に変換する
bool HexToULong( const WString& src , unsigned long& retvalue , unsigned long defvalue );

//16進文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned long HexToULong( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned long result;
	if( !HexToULong( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//16進文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned long HexToULong( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned long result;
	if( !HexToULong( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
unsigned int ToUInt( const AString& src , unsigned int defvalue = 0 );
//文字列を数値に変換する
unsigned int ToUInt( const WString& src , unsigned int defvalue = 0 );
//文字列を数値に変換する
bool ToUInt( const AString& src , unsigned int& retvalue , unsigned int defvalue );
//文字列を数値に変換する
bool ToUInt( const WString& src , unsigned int& retvalue , unsigned int defvalue );

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned int ToUInt( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned int result;
	if( !ToUInt( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned int ToUInt( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned int result;
	if( !ToUInt( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
unsigned int ToULong( const AString& src , unsigned long defvalue = 0 );
//文字列を数値に変換する
unsigned int ToULong( const WString& src , unsigned long defvalue = 0 );
//文字列を数値に変換する
bool ToULong( const AString& src , unsigned long& retvalue , unsigned long defvalue );
//文字列を数値に変換する
bool ToULong( const WString& src , unsigned long& retvalue , unsigned long defvalue );

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned int ToULong( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned long result;
	if( !ToULong( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline unsigned int ToULong( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	unsigned long result;
	if( !ToULong( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
int64_t ToInt64( const AString& src , int64_t defvalue = 0 );
//文字列を数値に変換する
int64_t ToInt64( const WString& src , int64_t defvalue = 0 );
//文字列を数値に変換する
bool ToInt64( const AString& src , int64_t& retvalue , int64_t defvalue );
//文字列を数値に変換する
bool ToInt64( const WString& src , int64_t& retvalue , int64_t defvalue );

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline int64_t ToInt64( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	int64_t result;
	if( !ToInt64( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline int64_t ToInt64( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	int64_t result;
	if( !ToInt64( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
uint64_t ToUInt64( const AString& src , uint64_t defvalue = 0 );
//文字列を数値に変換する
uint64_t ToUInt64( const WString& src , uint64_t defvalue = 0 );
//文字列を数値に変換する
bool ToUInt64( const AString& src , uint64_t& retvalue , uint64_t defvalue );
//文字列を数値に変換する
bool ToUInt64( const WString& src , uint64_t& retvalue , uint64_t defvalue );

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline uint64_t ToUInt64( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	uint64_t result;
	if( !ToUInt64( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline uint64_t ToUInt64( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	uint64_t result;
	if( !ToUInt64( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
double ToDouble( const AString& src , double defvalue = 0.0 );
//文字列を数値に変換する
double ToDouble( const WString& src , double defvalue = 0.0 );
//文字列を数値に変換する
bool ToDouble( const AString& src , double& retvalue , double defvalue );
//文字列を数値に変換する
bool ToDouble( const WString& src , double& retvalue , double defvalue );

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline double ToDouble( const AString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	double result;
	if( !ToDouble( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//文字列を数値に変換する
template< class ExceptionIfFailed >
inline double ToDouble( const WString& src )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
	double result;
	if( !ToDouble( src , result , 0 ) )
	{
		throw ExceptionIfFailed();
	}
	return result;
}

//数値を文字列に変換する
AString ToAString( uint32_t v );
//数値を文字列に変換する
AString ToAString( uint64_t v );
//数値を文字列に変換する
AString ToAString( int32_t v );
//数値を文字列に変換する
AString ToAString( int64_t v );
//数値を文字列に変換する
AString ToAString( double v );

//数値を文字列に変換する
WString ToWString( uint32_t v );
//数値を文字列に変換する
WString ToWString( uint64_t v );
//数値を文字列に変換する
WString ToWString( int32_t v );
//数値を文字列に変換する
WString ToWString( int64_t v );
//数値を文字列に変換する
WString ToWString( double v );

//文字列を特定の文字でパースする
void ParseString( const AString& str , CHAR delimiter , AStringVector& retParsed , bool noempty = false );
//文字列を特定の文字でパースする
void ParseString( const AString& str , CHAR delimiter , AStringDeque& retParsed , bool noempty = false );
//文字列を特定の文字でパースする
void ParseString( const WString& str , WCHAR delimiter , WStringVector& retParsed , bool noempty = false );
//文字列を特定の文字でパースする
void ParseString( const WString& str , WCHAR delimiter , WStringDeque& retParsed , bool noempty = false );

//文字列を特定の文字列でパースする
void ParseString( const AString& str , const AString& delimiter , AStringVector& retParsed , bool noempty = false );
//文字列を特定の文字列でパースする
void ParseString( const AString& str , const AString& delimiter , AStringDeque& retParsed , bool noempty = false );
//文字列を特定の文字列でパースする
void ParseString( const WString& str , const WString& delimiter , WStringVector& retParsed , bool noempty = false );
//文字列を特定の文字列でパースする
void ParseString( const WString& str , const WString& delimiter , WStringDeque& retParsed , bool noempty = false );

//文字列を改行でパースする
void ParseString( const AString& str , AStringVector& retParsed , bool noemptyline );
//文字列を改行でパースする
void ParseString( const AString& str , AStringDeque& retParsed , bool noemptyline );
//文字列を改行でパースする
void ParseString( const WString& str , WStringVector& retParsed , bool noemptyline );
//文字列を改行でパースする
void ParseString( const WString& str , WStringDeque& retParsed , bool noemptyline );

//文字列を改行でパースする(ParseStringと同じ)
void ParseStringNewLine( const AString& str , AStringVector& retParsed , bool noemptyline );
//文字列を改行でパースする(ParseStringと同じ)
void ParseStringNewLine( const AString& str , AStringDeque& retParsed , bool noemptyline );
//文字列を改行でパースする(ParseStringと同じ)
void ParseStringNewLine( const WString& str , WStringVector& retParsed , bool noemptyline );
//文字列を改行でパースする(ParseStringと同じ)
void ParseStringNewLine( const WString& str , WStringDeque& retParsed , bool noemptyline );

//文字列を空白文字でパースする
void ParseStringSpace( const AString& str , AStringVector& retParsed , bool noemptyline );
//文字列を空白文字でパースする
void ParseStringSpace( const AString& str , AStringDeque& retParsed , bool noemptyline );
//文字列を空白文字でパースする
void ParseStringSpace( const WString& str , WStringVector& retParsed , bool noemptyline );
//文字列を空白文字でパースする
void ParseStringSpace( const WString& str , WStringDeque& retParsed , bool noemptyline );

//文字列を空白文字または特定の文字でパースする
void ParseStringSpace( const AString& str , CHAR delimiter , AStringVector& retParsed , bool noemptyline );
//文字列を空白文字または特定の文字でパースする
void ParseStringSpace( const AString& str , CHAR delimiter , AStringDeque& retParsed , bool noemptyline );
//文字列を空白文字または特定の文字でパースする
void ParseStringSpace( const WString& str , WCHAR delimiter , WStringVector& retParsed , bool noemptyline );
//文字列を空白文字または特定の文字でパースする
void ParseStringSpace( const WString& str , WCHAR delimiter , WStringDeque& retParsed , bool noemptyline );


//文字列の相互変換
AString ConvertString( const AString& src , DWORD MapFlag , LCID locale = LOCALE_USER_DEFAULT );
AString ConvertHankaku2Zenkaku( const AString& src );	//半角→全角
AString ConvertZenkaku2Hankaku( const AString& src );	//全角→半角
AString ConvertKatakana2Hiragana( const AString& src );	//カタカナ→ひらがな
AString ConvertHiragana2Katakana( const AString& src );	//ひらがな→カタカナ
AString ConvertLower2Upper( const AString& src );		//小文字→大文字
AString ConvertUpper2Lower( const AString& src );		//大文字→小文字

WString ConvertString( const WString& src , DWORD MapFlag , LCID locale = LOCALE_USER_DEFAULT );
WString ConvertHankaku2Zenkaku( const WString& src );	//半角→全角
WString ConvertZenkaku2Hankaku( const WString& src );	//全角→半角
WString ConvertKatakana2Hiragana( const WString& src );	//カタカナ→ひらがな
WString ConvertHiragana2Katakana( const WString& src );	//ひらがな→カタカナ
WString ConvertLower2Upper( const WString& src );		//小文字→大文字
WString ConvertUpper2Lower( const WString& src );		//大文字→小文字

//文字列中の特定文字列を置換する
// src : 元の文字列
// findat : 探す文字列
// replaceto : 発見した文字列をこれに置き換える
// ret_count : 置換した回数(nullptr可)
// ret : 置換した結果
AString ReplaceString( const AString& src , const AString& findat , const AString& replaceto , DWORD* ret_count = nullptr );

//文字列中の特定文字列を置換する
// src : 元の文字列
// findat : 探す文字列
// replaceto : 発見した文字列をこれに置き換える
// ret_count : 置換した回数(nullptr可)
// ret : 置換した結果
WString ReplaceString( const WString& src , const WString& findat , const WString& replaceto , DWORD* ret_count = nullptr );

//文字列中の特定文字列を置換する
//このバージョンの置換では、文字列を繰り返し置換します
//例：AABAAのABをBCに置換する場合
//    AABAA→ABCAA→BCCAA ：置換した回数=2
//【注意】無限ループにならないよう気をつけてください
// src : 元の文字列
// findat : 探す文字列
// replaceto : 発見した文字列をこれに置き換える
// ret_count : 置換した回数(nullptr可)
// ret : 置換した結果
AString ReplaceStringRecursive( const AString& src , const AString& findat , const AString& replaceto , DWORD* ret_count = nullptr );

//文字列中の特定文字列を置換する
//このバージョンの置換では、文字列を繰り返し置換します
//例：AABAAのABをBCに置換する場合
//    AABAA→ABCAA→BCCAA ：置換した回数=2
//【注意】無限ループにならないよう気をつけてください
// src : 元の文字列
// findat : 探す文字列
// replaceto : 発見した文字列をこれに置き換える
// ret_count : 置換した回数(nullptr可)
// ret : 置換した結果
WString ReplaceStringRecursive( const WString& src , const WString& findat , const WString& replaceto , DWORD* ret_count = nullptr );

//NULLを入れてもいいstrcmp
int StrCmp( const wchar_t* s1 , const wchar_t* s2 );
//NULLを入れてもいいstrcmp
int StrCmp( const char* s1 , const char* s2 );

//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
//終端チェックがないため注意してください
void ReadDoubleNullString( const WCHAR* str , WStringVector& retArray );
//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
//終端チェックがないため注意してください
void ReadDoubleNullString( const WCHAR* str , WStringDeque& retArray );
//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
//終端チェックがないため注意してください
void ReadDoubleNullString( const CHAR* str , AStringVector& retArray );
//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
//終端チェックがないため注意してください
void ReadDoubleNullString( const CHAR* str , AStringDeque& retArray );

//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
void ReadDoubleNullString( const WString& str , WStringVector& retArray );
//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
void ReadDoubleNullString( const WString& str , WStringDeque& retArray );
//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
void ReadDoubleNullString( const AString& str , AStringVector& retArray );
//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
void ReadDoubleNullString( const AString& str , AStringDeque& retArray );

//アレイにから改行がヌル文字、最終行がヌル文字×2になっている文字列を作成する
void MakeDoubleNullString( const WStringVector& arr , WString& retStr );
//アレイにから改行がヌル文字、最終行がヌル文字×2になっている文字列を作成する
void MakeDoubleNullString( const WStringDeque& arr , WString& retStr );
//アレイにから改行がヌル文字、最終行がヌル文字×2になっている文字列を作成する
void MakeDoubleNullString( const AStringVector& arr , AString& retStr );
//アレイにから改行がヌル文字、最終行がヌル文字×2になっている文字列を作成する
void MakeDoubleNullString( const AStringDeque& arr , AString& retStr );

//文字列の両端にある空白をすべて削除する
AString TrimString( const AString& str );

//文字列の両端にある空白をすべて削除する
WString TrimString( const WString& str );

//文字列の空白をすべて削除する
AString RemoveSpace( const AString& str );

//文字列の空白をすべて削除する
WString RemoveSpace( const WString& str );

#endif

