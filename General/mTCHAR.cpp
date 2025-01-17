//----------------------------------------------------------------------------
// 文字型定義
// Copyright (C) 2012,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MTCHAR_CPP_COMPILING
#include "mTCHAR.h"
#include <stdarg.h>
#include <algorithm>
#include <inttypes.h>
#include <cinttypes>

TCHAR* mTCHAR::WCHAR2TCHAR( const WCHAR* str )
{
	TCHAR* result = nullptr;

	if( str == nullptr )
	{
		return nullptr;
	}

	if( IsUnicode() )
	{
		INT req = (INT)wcslen( str );
		result = mNew TCHAR[ req + 1 ];
		if( result == nullptr )
		{
			return nullptr;
		}
		wcscpy_s( (WCHAR*)result , req + 1 , str );
		SetLastError( 0 );
	}
	else
	{
		INT req = WideCharToMultiByte( CP_THREAD_ACP , 0 , str , -1 , (LPSTR)result , 0 , nullptr , nullptr );
		result = mNew TCHAR[ req ];
		WideCharToMultiByte( CP_THREAD_ACP , 0 , str , -1 , (LPSTR)result , req , nullptr , nullptr );
	}
	return result;

}

TCHAR* mTCHAR::CHAR2TCHAR( const CHAR* str )
{
	TCHAR* result = nullptr;

	if( str == nullptr )
	{
		return nullptr;
	}

	if( IsUnicode() )
	{
		INT req = MultiByteToWideChar( CP_THREAD_ACP , 0 , (LPCSTR)str , -1 , (LPWSTR)result , 0 );
		result = mNew TCHAR[ req ];
		MultiByteToWideChar( CP_THREAD_ACP , 0 , (LPCSTR)str , -1 , (LPWSTR)result , req );
	}
	else
	{
		INT req = (INT)strlen( str );
		result = mNew TCHAR[ req + 1 ];
		if( result == nullptr )
		{
			return nullptr;
		}
		strcpy_s( (CHAR*)result , req + 1 , str );
		SetLastError( 0 );
	}
	return result;

}

WCHAR* mTCHAR::TCHAR2WCHAR( const TCHAR* str )
{
	WCHAR* result = nullptr;

	if( str == nullptr )
	{
		return nullptr;
	}

	if( IsUnicode() )
	{
		INT req = (INT)tchar_strlen( str );
		result = mNew WCHAR[ req + 1 ];
		if( result == nullptr )
		{
			return nullptr;
		}
		wcscpy_s( result , req + 1 , (WCHAR*)str );
		SetLastError( 0 );
	}
	else
	{
		INT req = MultiByteToWideChar( CP_THREAD_ACP , 0 , (LPCSTR)str , -1 , result , 0 );
		result = mNew WCHAR[ req ];
		MultiByteToWideChar( CP_THREAD_ACP , 0 , (LPCSTR)str , -1 , result , req );
	}
	return result;
}

CHAR* mTCHAR::TCHAR2CHAR( const TCHAR* str )
{
	CHAR* result = nullptr;

	if( str == nullptr )
	{
		return nullptr;
	}

	if( IsUnicode() )
	{
		INT req = WideCharToMultiByte( CP_THREAD_ACP , 0 , (LPCWSTR)str , -1 , (LPSTR)result , 0 , nullptr , nullptr );
		result = mNew CHAR[ req ];
		WideCharToMultiByte( CP_THREAD_ACP , 0 , (LPCWSTR)str , -1 , (LPSTR)result , req , nullptr , nullptr );
	}
	else
	{
		INT req = (INT)tchar_strlen( str );
		result = mNew CHAR[ req + 1 ];
		if( result == nullptr )
		{
			return nullptr;
		}
		strcpy_s( result , req + 1 , (CHAR*)str );
		SetLastError( 0 );
	}
	return result;
}

WCHAR* mTCHAR::CHAR2WCHAR( const CHAR* str )
{
#ifdef UNICODE
	//TCHAR=WCHAR
	return CHAR2TCHAR( str );
#else
	//TCHAR=CHAR
	return TCHAR2WCHAR( str );
#endif
}

CHAR* mTCHAR::WCHAR2CHAR( const WCHAR* str )
{
#ifdef UNICODE
	//TCHAR=WCHAR
	return TCHAR2CHAR( str );
#else
	//TCHAR=CHAR
	return WCHAR2TCHAR( str );
#endif
}


CHAR* mTCHAR::NewAndCopyString( const CHAR* src )
{
	if( !src )
	{
		return nullptr;
	}

	size_t len = strlen( src ) + 1;
	CHAR* ptr = mNew CHAR[ len ];

	return strcpy( ptr , src );
}

WCHAR* mTCHAR::NewAndCopyString( const WCHAR* src )
{
	if( !src )
	{
		return nullptr;
	}

	size_t len = wcslen( src ) + 1;
	WCHAR* ptr = mNew WCHAR[ len ];

	return wcscpy( ptr , src );
}

TString mTCHAR::AString2TString( const AString& src )
{
#ifdef UNICODE
	TCHAR* tmp = CHAR2TCHAR( src.c_str() );
	TString result( tmp );

	mDelete[] tmp;
	return result;
#else
	return TString( src );
#endif
}

TString mTCHAR::WString2TString( const WString& src )
{
#ifdef UNICODE
	return TString( src );
#else
	TCHAR* tmp = WCHAR2TCHAR( src.c_str() );
	TString result( tmp );

	mDelete[] tmp;
	return result;
#endif
}

AString mTCHAR::TString2AString( const TString& src )
{
#ifdef UNICODE
	CHAR* tmp = TCHAR2CHAR( src.c_str() );
	if( tmp == nullptr )
	{
		return "";
	}
	AString result( tmp );

	mDelete[] tmp;
	return result;
#else
	return AString( src );
#endif
}

WString mTCHAR::TString2WString( const TString& src )
{
#ifdef UNICODE
	return WString( src );
#else
	WCHAR* tmp = TCHAR2WCHAR( src.c_str() );
	if( tmp == nullptr )
	{
		return L"";
	}
	WString result( tmp );

	mDelete[] tmp;
	return result;
#endif
}

WString mTCHAR::AString2WString( const AString& src )
{
	WCHAR* tmp = CHAR2WCHAR( src.c_str() );
	if( tmp == nullptr )
	{
		return L"";
	}
	WString result( tmp );

	mDelete[] tmp;
	return result;
}

AString mTCHAR::WString2AString( const WString& src )
{
	CHAR* tmp = WCHAR2CHAR( src.c_str() );
	if( tmp == nullptr )
	{
		return "";
	}
	AString result( tmp );

	mDelete[] tmp;
	return result;
}

AString mTCHAR::ToAString( const AString& src )
{
	return src;
}
AString mTCHAR::ToAString( const WString& src )
{
	return WString2AString( src );
}

WString mTCHAR::ToWString( const AString& src )
{
	return AString2WString( src );
}
WString mTCHAR::ToWString( const WString& src )
{
	return src;
}

TString mTCHAR::ToTString( const AString& src )
{
	return AString2TString( src );
}
TString mTCHAR::ToTString( const WString& src )
{
	return WString2TString( src );
}

AString mTCHAR::MakeAString( _Printf_format_string_ const char* format    , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );

	AString str;
	sprintf_va( str , format , args );

	//可変長引数リセット
	va_end( args );

	return str;
}

AString mTCHAR::MakeAString( _Printf_format_string_ const wchar_t* format , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );

	AString str;
	AString format_string = ToAString( format );
	sprintf_va( str , format_string.c_str() , args );

	//可変長引数リセット
	va_end( args );

	return str;
}

WString mTCHAR::MakeWString( _Printf_format_string_ const char* format    , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );

	WString str;
	WString format_string = ToWString( format );
	sprintf_va( str , format_string.c_str() , args );

	//可変長引数リセット
	va_end( args );

	return str;
}

WString mTCHAR::MakeWString( _Printf_format_string_ const wchar_t* format , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );

	WString str;
	sprintf_va( str , format , args );

	//可変長引数リセット
	va_end( args );

	return str;
}


TString mTCHAR::MakeTString( _Printf_format_string_ const char* format    , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );
	TString str;

	#ifdef UNICODE
	WString format_string = ToWString( format );
	sprintf_va( str , format_string.c_str() , args );
	#else
	sprintf_va( str , format , args );
	#endif

	//可変長引数リセット
	va_end( args );
	return str;
}

TString mTCHAR::MakeTString( _Printf_format_string_ const wchar_t* format , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , format );
	TString str;

	#ifdef UNICODE
	sprintf_va( str , format , args );
	#else
	AString format_string = ToAString( format );
	sprintf_va( str , format_string.c_str() , args );
	#endif

	//可変長引数リセット
	va_end( args );
	return str;
}

INT mTCHAR::sprintf_va( AString& ret_dest , _Printf_format_string_ const CHAR* format , va_list args )
{
	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscprintf( format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	CHAR* buffer = mNew CHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vsprintf( buffer , format , args );

	//結果をセットしてテンポラリメモリを破棄
	ret_dest = buffer;
	mDelete[] buffer;

	return result;
}

INT mTCHAR::sprintf_va( AString& ret_dest , _Printf_format_string_ const WCHAR* format , va_list args )
{

	//出力がCHAR型なので、formatを一旦CHAR型に変換する
	CHAR* CHAR_format = WCHAR2CHAR( format );
	if( CHAR_format == nullptr )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscprintf( CHAR_format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	CHAR* buffer = mNew CHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vsprintf( buffer , CHAR_format , args );

	//結果をセットしてテンポラリメモリを破棄
	ret_dest = buffer;

	mDelete[] buffer;
	mDelete[] CHAR_format;

	return result;
}

INT mTCHAR::sprintf_va( WString& ret_dest , _Printf_format_string_ const CHAR* format , va_list args )
{
	ret_dest = L"";
	if( !format )
	{
		return -1;
	}

	//出力がWCHAR型なので、formatを一旦WCHAR型に変換する
	WCHAR* wCHAR_format = CHAR2WCHAR( format );
	if( wCHAR_format == nullptr )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscwprintf( wCHAR_format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	WCHAR* buffer = mNew WCHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vswprintf( buffer , wCHAR_format , args );

	//結果をセットしてテンポラリメモリを破棄
	ret_dest = buffer;

	mDelete[] buffer;
	mDelete[] wCHAR_format;

	return result;
}

INT mTCHAR::sprintf_va( WString& ret_dest , _Printf_format_string_ const WCHAR* format , va_list args )
{
	ret_dest = L"";
	if( !format )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscwprintf( format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	WCHAR* buffer = mNew WCHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vswprintf( buffer , format , args );

	//結果をセットしてテンポラリメモリを破棄
	ret_dest = buffer;
	mDelete[] buffer;

	return result;

}

INT mTCHAR::sprintf_va( AString* ret_dest , _Printf_format_string_ const CHAR* format , va_list args )
{
	if( ret_dest == nullptr )
	{
		return -1;
	}

	*ret_dest = "";
	if( !format )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscprintf( format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	CHAR* buffer = mNew CHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vsprintf( buffer , format , args );

	//結果をセットしてテンポラリメモリを破棄
	*ret_dest = buffer;
	mDelete[] buffer;

	return result;
}

INT mTCHAR::sprintf_va( AString* ret_dest , _Printf_format_string_ const WCHAR* format , va_list args )
{
	if( ret_dest == nullptr )
	{
		return -1;
	}

	*ret_dest = "";
	if( !format )
	{
		return -1;
	}

	//出力がCHAR型なので、formatを一旦CHAR型に変換する
	CHAR* CHAR_format = WCHAR2CHAR( format );
	if( CHAR_format == nullptr )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscprintf( CHAR_format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	CHAR* buffer = mNew CHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vsprintf( buffer , CHAR_format , args );

	//結果をセットしてテンポラリメモリを破棄
	*ret_dest = buffer;

	mDelete[] buffer;
	mDelete[] CHAR_format;

	return result;
}

INT mTCHAR::sprintf_va( WString* ret_dest , _Printf_format_string_ const CHAR* format , va_list args )
{
	if( ret_dest == nullptr )
	{
		return -1;
	}

	*ret_dest = L"";
	if( !format )
	{
		return -1;
	}

	//出力がWCHAR型なので、formatを一旦WCHAR型に変換する
	WCHAR* wCHAR_format = CHAR2WCHAR( format );
	if( wCHAR_format == nullptr )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscwprintf( wCHAR_format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	WCHAR* buffer = mNew WCHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vswprintf( buffer , wCHAR_format , args );

	//結果をセットしてテンポラリメモリを破棄
	*ret_dest = buffer;

	mDelete[] buffer;
	mDelete[] wCHAR_format;

	return result;
}

INT mTCHAR::sprintf_va( WString* ret_dest , _Printf_format_string_ const WCHAR* format , va_list args )
{
	if( ret_dest == nullptr )
	{
		return -1;
	}

	*ret_dest = L"";
	if( !format )
	{
		return -1;
	}

	//必要文字数をカウントしてバッファを確保
	INT bufflen = _vscwprintf( format , args ) + 1;
	if( bufflen == -1 )
	{
		return -1;
	}
	WCHAR* buffer = mNew WCHAR[ bufflen ];

	//フォーマットした文字列を書き込み
	INT result = vswprintf( buffer , format , args );

	//結果をセットしてテンポラリメモリを破棄
	*ret_dest = buffer;
	mDelete[] buffer;

	return result;
}

bool mTCHAR::Binary2String( WString& ret_dest , const BYTE* dat , DWORD len )
{
	const WCHAR HexCharTable[ 16 ] =
	{
		L'0' , L'1' , L'2' , L'3' , L'4' , L'5' , L'6' , L'7' ,
		L'8' , L'9' , L'a' , L'b' , L'c' , L'd' , L'e' , L'f'
	};

	ret_dest.clear();
	if( dat == nullptr || len == 0 )
	{
		return true;
	}

	ret_dest.reserve( len * 2 );
	for( DWORD i = 0 ; i < len ; i++ )
	{
		ret_dest.push_back( HexCharTable[ ( dat[ i ] >> 4 ) & 0x0000000f ] );
		ret_dest.push_back( HexCharTable[ ( dat[ i ] >> 0 ) & 0x0000000f ] );
	}
	return true;
}

bool mTCHAR::Binary2String( AString& ret_dest , const BYTE* dat , DWORD len )
{
	const CHAR HexCharTable[ 16 ] =
	{
		'0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' ,
		'8' , '9' , 'a' , 'b' , 'c' , 'd' , 'e' , 'f'
	};


	ret_dest.clear();
	if( dat == nullptr || len == 0 )
	{
		return true;
	}

	ret_dest.reserve( len * 2 );
	for( DWORD i = 0 ; i < len ; i++ )
	{
		ret_dest.push_back( HexCharTable[ ( dat[ i ] >> 4 ) & 0x0000000f ] );
		ret_dest.push_back( HexCharTable[ ( dat[ i ] >> 0 ) & 0x0000000f ] );
	}
	return true;
}


//文字列を大文字に変換する
AString mTCHAR::ToUpper( const AString& src )
{
	AString dst;
	dst.resize( src.size() );
	std::transform( src.begin() , src.end() , dst.begin() , toupper );

	return dst;
}

//文字列を大文字に変換する
WString mTCHAR::ToUpper( const WString& src )
{
	WString dst;
	dst.resize( src.size() );
	std::transform( src.begin() , src.end() , dst.begin() , toupper );

	return dst;
}

//文字列を小文字に変換する
AString mTCHAR::ToLower( const AString& src )
{
	AString dst;
	dst.resize( src.size() );
	std::transform( src.begin() , src.end() , dst.begin() , tolower );

	return dst;
}

//文字列を小文字に変換する
WString mTCHAR::ToLower( const WString& src )
{
	WString dst;
	dst.resize( src.size() );
	std::transform( src.begin() , src.end() , dst.begin() , tolower );

	return dst;
}

template<class c,class d>
static void ParseStringTemplate( const std::basic_string<c>& str , c delimiter , d& retResult , bool noempty )
{
	retResult.clear();

	size_t prev_idx = 0;
	size_t next_idx = 0;
	while( 1 )
	{
		next_idx = str.find( delimiter , prev_idx );
		if( next_idx == std::basic_string<c>::npos )
		{
			if( !noempty || ( 0 < str.substr( prev_idx ).size() ) )
			{
				retResult.push_back( str.substr( prev_idx ) );
			}
			return;
		}

		if( prev_idx == next_idx )
		{
			//空
			if( !noempty )
			{
				retResult.push_back( std::basic_string<c>() );
			}
		}
		else
		{
			retResult.push_back( str.substr( prev_idx , next_idx - prev_idx ) );
		}

		prev_idx = next_idx + 1;
	}
}

void mTCHAR::ParseString( const AString& str , CHAR delimiter , AStringVector& retParsed , bool noempty )
{
	return ParseStringTemplate<CHAR,AStringVector>( str , delimiter , retParsed , noempty );
}

void mTCHAR::ParseString( const AString& str , CHAR delimiter , AStringDeque& retParsed , bool noempty )
{
	return ParseStringTemplate<CHAR,AStringDeque>( str , delimiter , retParsed , noempty );
}

void mTCHAR::ParseString( const WString& str , WCHAR delimiter , WStringVector& retParsed , bool noempty )
{
	return ParseStringTemplate<WCHAR,WStringVector>( str , delimiter , retParsed , noempty );
}

void mTCHAR::ParseString( const WString& str , WCHAR delimiter , WStringDeque& retParsed , bool noempty )
{
	return ParseStringTemplate<WCHAR,WStringDeque>( str , delimiter , retParsed , noempty );
}

template<class c>
static void ParseStringReturnTemplate( const AString& str , c& retResult , bool noemptyline )
{
	bool detect_cr = false;
	AString tmpstr;

	auto NextLine = [ &tmpstr , &retResult ]( bool noemptyline ) -> void
	{
		if( noemptyline && tmpstr.empty() )
		{
			return;
		}
		retResult.push_back( tmpstr );
		tmpstr.clear();
	};

	for( AString::const_iterator itr = str.begin() ; itr != str.end() ; itr++ )
	{
		switch( *itr )
		{
		case '\r':
			if( detect_cr )
			{
				NextLine( noemptyline );
			}
			detect_cr = true;
			break;
		case '\n':
			NextLine( noemptyline );
			detect_cr = false;
			break;
		default:
			if( detect_cr )
			{
				NextLine( noemptyline );
				detect_cr = false;
			}
			tmpstr.push_back( *itr );
		}
	}
	NextLine( !detect_cr );
	return;
}

void mTCHAR::ParseString( const AString& str , AStringVector& retParsed , bool noemptyline )
{
	ParseStringReturnTemplate( str , retParsed , noemptyline );
}

void mTCHAR::ParseString( const AString& str , AStringDeque& retParsed , bool noemptyline )
{
	ParseStringReturnTemplate( str , retParsed , noemptyline );
}

template<class c>
static void ParseStringReturnTemplate( const WString& str , c& retResult , bool noemptyline )
{
	bool detect_cr = false;
	WString tmpstr;

	auto NextLine = [ &tmpstr , &retResult ]( bool noemptyline ) -> void
	{
		if( noemptyline && tmpstr.empty() )
		{
			return;
		}
		retResult.push_back( tmpstr );
		tmpstr.clear();
	};

	for( WString::const_iterator itr = str.begin() ; itr != str.end() ; itr++ )
	{
		switch( *itr )
		{
		case L'\r':
			if( detect_cr )
			{
				NextLine( noemptyline );
			}
			detect_cr = true;
			break;
		case L'\n':
			NextLine( noemptyline );
			detect_cr = false;
			break;
		default:
			if( detect_cr )
			{
				NextLine( noemptyline );
				detect_cr = false;
			}
			tmpstr.push_back( *itr );
		}
	}
	NextLine( !detect_cr );
	return;
}

void mTCHAR::ParseString( const WString& str , WStringVector& retParsed , bool noemptyline )
{
	ParseStringReturnTemplate( str , retParsed , noemptyline );
}

void mTCHAR::ParseString( const WString& str , WStringDeque& retParsed , bool noemptyline )
{
	ParseStringReturnTemplate( str , retParsed , noemptyline );
}

//文字列を改行でパースする
void mTCHAR::ParseStringNewLine( const AString& str , AStringVector& retParsed , bool noemptyline )
{
	ParseString( str , retParsed , noemptyline );
}

void mTCHAR::ParseStringNewLine( const AString& str , AStringDeque& retParsed , bool noemptyline )
{
	ParseString( str , retParsed , noemptyline );
}

void mTCHAR::ParseStringNewLine( const WString& str , WStringVector& retParsed , bool noemptyline )
{
	ParseString( str , retParsed , noemptyline );
}

void mTCHAR::ParseStringNewLine( const WString& str , WStringDeque& retParsed , bool noemptyline )
{
	ParseString( str , retParsed , noemptyline );
}

//文字列を空白文字でパースする
static bool QueryIsSpace( char c )
{
	return isascii( c ) && isspace( c );
}

static bool QueryIsSpace( wchar_t c )
{
	return wchar_isascii( c ) && wchar_isspace( c );
}

template<class c,class d>
static void ParseStringSpaceTemplate( const std::basic_string<c>& str , d& retResult , bool noemptyline )
{
	std::basic_string<c> token;
	auto PushToken = [ &retResult , &token , noemptyline ]( void ) -> void
	{
		if( token.size() || !noemptyline )
		{
			retResult.push_back( token );
			token.clear();
		}
	};

	for( DWORD i = 0 ; i < str.size() ; i++ )
	{
		if(  QueryIsSpace( str[ i ] ) )
		{
			PushToken();
		}
		else
		{
			token.push_back( str[ i ] );
		}
	}
	PushToken();
	return;
}

void mTCHAR::ParseStringSpace( const AString& str , AStringVector& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , retParsed , noemptyline );
}

void mTCHAR::ParseStringSpace( const AString& str , AStringDeque& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , retParsed , noemptyline );
}

void mTCHAR::ParseStringSpace( const WString& str , WStringVector& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , retParsed , noemptyline );
}

void mTCHAR::ParseStringSpace( const WString& str , WStringDeque& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , retParsed , noemptyline );
}

template<class c,class d>
static void ParseStringSpaceTemplate( const std::basic_string<c>& str , c delimiter , d& retResult , bool noemptyline )
{
	std::basic_string<c> token;
	auto PushToken = [ &retResult , &token , noemptyline ]( void ) -> void
	{
		if( token.size() || !noemptyline )
		{
			retResult.push_back( token );
			token.clear();
		}
	};

	for( DWORD i = 0 ; i < str.size() ; i++ )
	{
		if( str[ i ] == delimiter || QueryIsSpace( str[ i ] ) )
		{
			PushToken();
		}
		else
		{
			token.push_back( str[ i ] );
		}
	}
	PushToken();
	return;
}


void mTCHAR::ParseStringSpace( const AString& str , CHAR delimiter , AStringVector& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , delimiter , retParsed , noemptyline );
}
void mTCHAR::ParseStringSpace( const AString& str , CHAR delimiter , AStringDeque& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , delimiter , retParsed , noemptyline );
}
void mTCHAR::ParseStringSpace( const WString& str , WCHAR delimiter , WStringVector& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , delimiter , retParsed , noemptyline );
}
void mTCHAR::ParseStringSpace( const WString& str , WCHAR delimiter , WStringDeque& retParsed , bool noemptyline )
{
	ParseStringSpaceTemplate( str , delimiter , retParsed , noemptyline );
}


template<class c,class d>
static void ParseStringTemplate( const std::basic_string<c>& str , const std::basic_string<c>& delimiter , d& retResult , bool noempty )
{
	retResult.clear();

	size_t prev_idx = 0;
	size_t next_idx = 0;
	while( 1 )
	{
		next_idx = str.find( delimiter , prev_idx );
		if( next_idx == std::basic_string<c>::npos )
		{
			retResult.push_back( str.substr( prev_idx ) );
			return;
		}

		if( prev_idx == next_idx )
		{
			//空
			if( !noempty )
			{
				retResult.push_back( std::basic_string<c>() );
			}			
		}
		else
		{
			retResult.push_back( str.substr( prev_idx , next_idx - prev_idx ) );
		}

		prev_idx = next_idx + delimiter.length();
	}
}

void mTCHAR::ParseString( const AString& str , const AString& delimiter , AStringVector& retParsed , bool noempty )
{
	return ParseStringTemplate<CHAR,AStringVector>( str , delimiter , retParsed , noempty );
}

void mTCHAR::ParseString( const AString& str , const AString& delimiter , AStringDeque& retParsed , bool noempty )
{
	return ParseStringTemplate<CHAR,AStringDeque>( str , delimiter , retParsed , noempty );
}

void mTCHAR::ParseString( const WString& str , const WString& delimiter , WStringVector& retParsed , bool noempty )
{
	return ParseStringTemplate<WCHAR,WStringVector>( str , delimiter , retParsed , noempty );
}

void mTCHAR::ParseString( const WString& str , const WString& delimiter , WStringDeque& retParsed , bool noempty )
{
	return ParseStringTemplate<WCHAR,WStringDeque>( str , delimiter , retParsed , noempty );
}

static char* NewAndConvertString( const char* str , DWORD MapFlag , LCID locale )
{
	int len = LCMapStringA( locale , MapFlag , str , -1 , 0 , 0 ) + 2;
	if( len <= 0 )
	{
		return 0;
	}

	char* result = mNew char[ len ];
	int written = LCMapStringA( locale , MapFlag , str , -1 , result , len );

	result[ written ] = '\0';
	return result;
}

AString mTCHAR::ConvertString( const AString& src , DWORD MapFlag , LCID locale )
{
	char* str = NewAndConvertString( src.c_str() , MapFlag , locale );
	AString r( str );
	mDelete[] str;

	return r;
}

AString mTCHAR::ConvertHankaku2Zenkaku( const AString& src )
{
	return ConvertString( src, LCMAP_FULLWIDTH );
}

AString mTCHAR::ConvertZenkaku2Hankaku( const AString& src )
{
	return ConvertString( src, LCMAP_HALFWIDTH );
}

AString mTCHAR::ConvertKatakana2Hiragana( const AString& src )
{
	return ConvertString( src, LCMAP_HIRAGANA );
}

AString mTCHAR::ConvertHiragana2Katakana( const AString& src )
{
	return ConvertString( src, LCMAP_KATAKANA );
}

AString mTCHAR::ConvertLower2Upper( const AString& src )
{
	return ConvertString( src, LCMAP_UPPERCASE );
}

AString mTCHAR::ConvertUpper2Lower( const AString& src )
{
	return ConvertString( src, LCMAP_LOWERCASE );
}

static wchar_t* NewAndConvertString( const wchar_t* str , DWORD MapFlag , LCID locale )
{
	int len = LCMapStringW( locale , MapFlag , str , -1 , 0 , 0 ) + 2;
	if( len <= 0 )
	{
		return 0;
	}

	wchar_t* result = mNew wchar_t[ len ];
	int written = LCMapStringW( locale , MapFlag , str , -1 , result , len );

	result[ written ] = L'\0';
	return result;
}

WString mTCHAR::ConvertString( const WString& src , DWORD MapFlag , LCID locale )
{
	wchar_t* str = NewAndConvertString( src.c_str() , MapFlag , locale );
	WString r( str );
	mDelete[] str;

	return r;
}

WString mTCHAR::ConvertHankaku2Zenkaku( const WString& src )
{
	return ConvertString( src, LCMAP_FULLWIDTH );
}

WString mTCHAR::ConvertZenkaku2Hankaku( const WString& src )
{
	return ConvertString( src, LCMAP_HALFWIDTH );
}

WString mTCHAR::ConvertKatakana2Hiragana( const WString& src )
{
	return ConvertString( src, LCMAP_HIRAGANA );
}

WString mTCHAR::ConvertHiragana2Katakana( const WString& src )
{
	return ConvertString( src, LCMAP_KATAKANA );
}

WString mTCHAR::ConvertLower2Upper( const WString& src )
{
	return ConvertString( src, LCMAP_UPPERCASE );
}

WString mTCHAR::ConvertUpper2Lower( const WString& src )
{
	return ConvertString( src, LCMAP_LOWERCASE );
}

template< class T >
static T ReplaceStringTemplate( const T& src , const T& findat , const T replaceto , DWORD* ret_count )
{
	T result = src;
	T::size_type pos = result.find( findat );

	while( pos != T::npos )
	{
		result.replace( pos , findat.size() , replaceto );
		pos = result.find( findat , pos + replaceto.size() );

		if( ret_count )
		{
			(*ret_count)++;
		}
	}

	return result;
}

//文字列中の特定文字列を置換する
AString mTCHAR::ReplaceString( const AString& src , const AString& findat , const AString& replaceto , DWORD* ret_count )
{
	return ReplaceStringTemplate( src , findat , replaceto , ret_count );
}

//文字列中の特定文字列を置換する
WString mTCHAR::ReplaceString( const WString& src , const WString& findat , const WString& replaceto , DWORD* ret_count )
{
	return ReplaceStringTemplate( src , findat , replaceto , ret_count );
}

template< class T >
static T ReplaceStringRecursiveTemplate( const T& src , const T& findat , const T replaceto , DWORD* ret_count )
{
	T result = src;
	T::size_type pos = result.find( findat );

	while( pos != T::npos )
	{
		result.replace( pos , findat.size() , replaceto );
		pos = result.find( findat );

		if( ret_count )
		{
			(*ret_count)++;
		}
	}
	return result;
}

//文字列中の特定文字列を置換する
AString mTCHAR::ReplaceStringRecursive( const AString& src , const AString& findat , const AString& replaceto , DWORD* ret_count )
{
	return ReplaceStringRecursiveTemplate( src , findat , replaceto , ret_count );
}

//文字列中の特定文字列を置換する
WString mTCHAR::ReplaceStringRecursive( const WString& src , const WString& findat , const WString& replaceto , DWORD* ret_count )
{
	return ReplaceStringRecursiveTemplate( src , findat , replaceto , ret_count );
}

int mTCHAR::ToInt( const AString& src , int defvalue )
{
	int result;
	ToInt( src , result , defvalue );
	return result;
}

int mTCHAR::ToInt( const WString& src , int defvalue )
{
	int result;
	ToInt( src , result , defvalue );
	return result;
}

double mTCHAR::ToDouble( const AString& src , double defvalue )
{
	double result;
	ToDouble( src , result , defvalue );
	return result;
}

double mTCHAR::ToDouble( const WString& src , double defvalue )
{
	double result;
	ToDouble( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::ToUInt( const AString& src , unsigned int defvalue )
{
	unsigned int result;
	ToUInt( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::ToUInt( const WString& src , unsigned int defvalue )
{
	unsigned int result;
	ToUInt( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::ToULong( const AString& src , unsigned long defvalue )
{
	unsigned long result;
	ToULong( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::ToULong( const WString& src , unsigned long defvalue )
{
	unsigned long result;
	ToULong( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::HexToUInt( const AString& src , unsigned int defvalue )
{
	unsigned int result;
	HexToUInt( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::HexToUInt( const WString& src , unsigned int defvalue )
{
	unsigned int result;
	HexToUInt( src , result , defvalue );
	return result;
}

unsigned int mTCHAR::HexToULong( const AString& src , unsigned long defvalue )
{
	unsigned long result;
	HexToULong( src , result , defvalue );
	return result;}

unsigned int mTCHAR::HexToULong( const WString& src , unsigned long defvalue )
{
	unsigned long result;
	HexToULong( src , result , defvalue );
	return result;
}

int64_t mTCHAR::ToInt64( const AString& src , int64_t defvalue )
{
	int64_t result;
	ToInt64( src , result , defvalue );
	return result;
}


int64_t mTCHAR::ToInt64( const WString& src , int64_t defvalue )
{
	int64_t result;
	ToInt64( src , result , defvalue );
	return result;
}

uint64_t mTCHAR::ToUInt64( const AString& src , uint64_t defvalue )
{
	uint64_t result;
	ToUInt64( src , result , defvalue );
	return result;
}

uint64_t mTCHAR::ToUInt64( const WString& src , uint64_t defvalue )
{
	uint64_t result;
	ToUInt64( src , result , defvalue );
	return result;
}

static const char* SkipWhiteSpace( const AString& src )
{
	static const char* blank = "";

	const char* p = src.c_str();
	while( 1 )
	{
		if( !wchar_isspace( *p ) )
		{
			break;
		}
		if( *p == '\0' )
		{
			return blank;
		}
		p++;
	}
	return p;
}

static const wchar_t* SkipWhiteSpace( const WString& src )
{
	static const wchar_t* blank = L"";

	const wchar_t* p = src.c_str();
	while( 1 )
	{
		if( !wchar_isspace( *p ) )
		{
			break;
		}
		if( *p == L'\0' )
		{
			return blank;
		}
		p++;
	}
	return p;
}

bool mTCHAR::ToInt( const AString& src , int& retvalue , int defvalue )
{
	const char* p = SkipWhiteSpace( src );

	if( sscanf( p , "%d" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToInt( const WString& src , int& retvalue , int defvalue )
{
	const wchar_t* p = SkipWhiteSpace( src );

	if( wchar_sscanf( p , L"%d" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToUInt( const AString& src , unsigned int& retvalue , unsigned int defvalue )
{
	const char* p = SkipWhiteSpace( src );

	if( sscanf( p , "%u" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToUInt( const WString& src , unsigned int& retvalue , unsigned int defvalue )
{
	const wchar_t* p = SkipWhiteSpace( src );

	if( wchar_sscanf( p , L"%u" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToULong( const AString& src , unsigned long& retvalue , unsigned long defvalue )
{
	const char* p = SkipWhiteSpace( src );

	if( sscanf( p , "%lu" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToULong( const WString& src , unsigned long& retvalue , unsigned long defvalue )
{
	const wchar_t* p = SkipWhiteSpace( src );

	if( wchar_sscanf( p , L"%lu" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::HexToUInt( const AString& src , unsigned int& retvalue , unsigned int defvalue )
{
	const AString t = SkipWhiteSpace( src );

	if( 2 < t.size() && src[ 0 ] == '0' && src[ 1 ] == 'x' )
	{
		if( sscanf( t.substr( 2 ).c_str() , "%x" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	else
	{
		if( sscanf( t.c_str() , "%x" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	return true;
}

bool mTCHAR::HexToUInt( const WString& src , unsigned int& retvalue , unsigned int defvalue )
{
	const WString t = SkipWhiteSpace( src );

	if( 2 < t.size() && src[ 0 ] == L'0' && src[ 1 ] == L'x' )
	{
		if( wchar_sscanf( t.substr( 2 ).c_str() , L"%x" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	else
	{
		if( wchar_sscanf( t.c_str() , L"%x" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	return true;
}

bool mTCHAR::HexToULong( const AString& src , unsigned long& retvalue , unsigned long defvalue )
{
	const AString t = SkipWhiteSpace( src );

	if( 2 < t.size() && src[ 0 ] == '0' && src[ 1 ] == 'x' )
	{
		if( sscanf( t.substr( 2 ).c_str() , "%lx" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	else
	{
		if( sscanf( t.c_str() , "%lx" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	return true;
}

bool mTCHAR::HexToULong( const WString& src , unsigned long& retvalue , unsigned long defvalue )
{
	const WString t = SkipWhiteSpace( src );

	if( 2 < t.size() && src[ 0 ] == L'0' && src[ 1 ] == L'x' )
	{
		if( wchar_sscanf( t.substr( 2 ).c_str() , L"%lx" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	else
	{
		if( wchar_sscanf( t.c_str() , L"%lx" , &retvalue ) != 1 )
		{
			retvalue = defvalue;
			return false;
		}
	}
	return true;
}


#define __U(x) L ## x
#define _U(x) __U(x)

bool mTCHAR::ToInt64( const AString& src , int64_t& retvalue , int64_t defvalue )
{
	const char* p = SkipWhiteSpace( src );

	if( sscanf( p , "%" SCNd64  , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToInt64( const WString& src , int64_t& retvalue , int64_t defvalue )
{
	const wchar_t* p = SkipWhiteSpace( src );

	if( wchar_sscanf( p , L"%" _U(SCNd64) , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToUInt64( const AString& src , uint64_t& retvalue , uint64_t defvalue )
{
	const char* p = SkipWhiteSpace( src );

	if( sscanf( p , "%" SCNu64  , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToUInt64( const WString& src , uint64_t& retvalue , uint64_t defvalue )
{
	const wchar_t* p = SkipWhiteSpace( src );

	if( wchar_sscanf( p , L"%" _U(SCNu64) , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToDouble( const AString& src , double& retvalue , double defvalue )
{
	const char* p = SkipWhiteSpace( src );

	if( sscanf( p , "%lf" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

bool mTCHAR::ToDouble( const WString& src , double& retvalue , double defvalue )
{
	const wchar_t* p = SkipWhiteSpace( src );

	if( wchar_sscanf( p , L"%lf" , &retvalue ) != 1 )
	{
		retvalue = defvalue;
		return false;
	}
	return true;
}

AString mTCHAR::ToAString( uint32_t v )
{
	AString s;
	sprintf( s , "%u" , v );
	return s;
}
AString mTCHAR::ToAString( uint64_t v )
{
	AString s;
	sprintf( s , "%llu" , v );
	return s;
}
AString mTCHAR::ToAString( int32_t v )
{
	AString s;
	sprintf( s , "%d" , v );
	return s;
}
AString mTCHAR::ToAString( int64_t v )
{
	AString s;
	sprintf( s , "%lld" , v );
	return s;
}
AString mTCHAR::ToAString( double v )
{
	AString s;
	sprintf( s , "%lf" , v );
	return s;
}

WString mTCHAR::ToWString( uint32_t v )
{
	WString s;
	sprintf( s , "%u" , v );
	return s;
}
WString mTCHAR::ToWString( uint64_t v )
{
	WString s;
	sprintf( s , L"%llu" , v );
	return s;
}
WString mTCHAR::ToWString( int32_t v )
{
	WString s;
	sprintf( s , L"%d" , v );
	return s;
}
WString mTCHAR::ToWString( int64_t v )
{
	WString s;
	sprintf( s , L"%lld" , v );
	return s;
}
WString mTCHAR::ToWString( double v )
{
	WString s;
	sprintf( s , L"%lf" , v );
	return s;
}

int mTCHAR::StrCmp( const wchar_t* s1 , const wchar_t* s2 )
{
	if( s1 == nullptr )
	{
		if( s2 == nullptr )
		{
			return 0;
		}
		return wchar_strcmp( L"" , s2 );
	}
	return wchar_strcmp( s1 , s2 );
}

int mTCHAR::StrCmp( const char* s1 , const char* s2 )
{
	if( s1 == nullptr )
	{
		if( s2 == nullptr )
		{
			return 0;
		}
		return strcmp( "" , s2 );
	}
	return strcmp( s1 , s2 );
}


//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
template< typename T , class U >
static void ReadDoubleNullStringTemplate( const T* str , U& arr )
{
	arr.clear();
	if( !str )
	{
		return;
	}
	const T* head;

	while( 1 )
	{
		head = str;
		str++;

		if( *head == 0 )
		{
			if( *str == 0 )
			{
				return;
			}
		}
		else
		{
			while( *str )
			{
				str++;
			}
			arr.push_back( head );
		}
	}
	return;
}


void mTCHAR::ReadDoubleNullString( const WCHAR* str , WStringVector& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

void mTCHAR::ReadDoubleNullString( const WCHAR* str , WStringDeque& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

void mTCHAR::ReadDoubleNullString( const CHAR* str , AStringVector& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

void mTCHAR::ReadDoubleNullString( const CHAR* str , AStringDeque& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

//改行がヌル文字、最終行がヌル文字×2になっている文字列を読み取って、アレイに展開する
template< class c_array , typename c_char >
static void ReadDoubleNullStringTemplate( const std::basic_string< c_char >& str , c_array& arr )
{
	ParseString( str , c_char( 0 ) , arr );
	if( arr.size() )
	{
		arr.pop_back();
	}
	if( arr.size() )
	{
		arr.pop_back();
	}
}

void mTCHAR::ReadDoubleNullString( const WString& str , WStringVector& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

void mTCHAR::ReadDoubleNullString( const WString& str , WStringDeque& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

void mTCHAR::ReadDoubleNullString( const AString& str , AStringVector& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

void mTCHAR::ReadDoubleNullString( const AString& str , AStringDeque& retArray )
{
	ReadDoubleNullStringTemplate( str , retArray );
}

//アレイにから改行がヌル文字、最終行がヌル文字×2になっている文字列を作成する
template< class c_array , typename c_char >
static void MakeDoubleNullStringTemplate( const c_array& arr , std::basic_string< c_char >& str )
{
	str.clear();

	for( c_array::const_iterator itr = arr.begin() ; itr != arr.end() ; itr++ )
	{
		str += *itr;
		str += (c_char)0;
	}
	str += (c_char)0;
}

void mTCHAR::MakeDoubleNullString( const WStringVector& arr , WString& retStr )
{
	MakeDoubleNullStringTemplate( arr , retStr );
}

void mTCHAR::MakeDoubleNullString( const WStringDeque& arr , WString& retStr )
{
	MakeDoubleNullStringTemplate( arr , retStr );
}

void mTCHAR::MakeDoubleNullString( const AStringVector& arr , AString& retStr )
{
	MakeDoubleNullStringTemplate( arr , retStr );
}

void mTCHAR::MakeDoubleNullString( const AStringDeque& arr , AString& retStr )
{
	MakeDoubleNullStringTemplate( arr , retStr );
}

AString mTCHAR::AStringToUtf8( const AString& src )
{
	WString t = AString2WString( src );
	return WStringToUtf8( t );
}

AString mTCHAR::WStringToUtf8( const WString& src )
{
	int sz = WideCharToMultiByte( CP_UTF8 , 0 , src.c_str() , -1 , nullptr , 0 , nullptr , nullptr );
	if( sz <= 1 )
	{
		return "";
	}

	AString result;
	result.resize( sz - 1 );
	WideCharToMultiByte( CP_UTF8 , 0 , src.c_str() , -1 , const_cast<char*>( result.data() ) , sz , nullptr , nullptr );

	return std::move( result );
}

AString mTCHAR::Utf8ToAString( const AString& src )
{
	WString t = Utf8ToWString( src );
	return WString2AString( t );
}

WString mTCHAR::Utf8ToWString( const AString& src )
{
	int sz = MultiByteToWideChar( CP_UTF8 , 0 , src.c_str() , -1 , nullptr , 0 );
	if( sz <= 1 )
	{
		return L"";
	}

	WString result;
	result.resize( sz - 1 );
	MultiByteToWideChar( CP_UTF8 , 0 , src.c_str() , -1 , const_cast<wchar_t*>( result.data() ) , sz );

	return std::move( result );
}

WString mTCHAR::CastToWString( const AString& src )
{
	WString result;
	size_t sz = src.size() / sizeof( wchar_t );

	result.resize( sz );
	MoveMemory( const_cast<wchar_t*>( result.data() ) , src.data() , sz * sizeof( wchar_t ) );
	return result;
}

WString mTCHAR::SwitchEndian( const WString& src )
{
	WString result;
	result.reserve( src.size() );
	for( WString::const_iterator itr = src.begin() ; itr != src.end() ; itr++ )
	{
		result.push_back( ( ( (*itr) & 0xFF00 ) >> 8 ) | ( ( (*itr) & 0x00FF ) << 8 ) );
	}
	return result;
}

//文字列の両端にある空白をすべて削除する
AString mTCHAR::TrimString( const AString& str )
{
	DWORD begin = 0;
	DWORD end = (DWORD)str.size();

	for( ; begin < end ; begin++ )
	{
		if( !isspace( str[ begin ] ) )
		{
			break;
		}
	}
	for( ; begin < end ; end-- )
	{
		if( !isspace( str[ end - 1 ] ) )
		{
			break;
		}
	}

	if( begin == end )
	{
		return "";
	}
	return str.substr( begin , end - begin );
}

//文字列の両端にある空白をすべて削除する
WString mTCHAR::TrimString( const WString& str )
{
	DWORD begin = 0;
	DWORD end = (DWORD)str.size();

	for( ; begin < end ; begin++ )
	{
		if( !wchar_isspace( str[ begin ] ) )
		{
			break;
		}
	}
	for( ; begin < end ; end-- )
	{
		if( !wchar_isspace( str[ end - 1 ] ) )
		{
			break;
		}
	}

	if( begin == end )
	{
		return L"";
	}
	return str.substr( begin , end - begin );
}

//文字列の空白をすべて削除する
AString mTCHAR::RemoveSpace( const AString& str )
{
	AString ret;
	for( AString::const_iterator itr = str.begin() ; itr != str.end() ; itr++ )
	{
		if( !isspace( *itr ) )
		{
			ret.push_back( *itr );
		}
	}
	return ret;
}

//文字列の空白をすべて削除する
WString mTCHAR::RemoveSpace( const WString& str )
{
	WString ret;
	for( WString::const_iterator itr = str.begin() ; itr != str.end() ; itr++ )
	{
		if( !wchar_isspace( *itr ) )
		{
			ret.push_back( *itr );
		}
	}
	return ret;
}

