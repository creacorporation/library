//----------------------------------------------------------------------------
// �����^��`
// Copyright (C) 2012,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MTCHAR_H_INCLUDED
#define MTCHAR_H_INCLUDED

/*
���p�r
������^�������̂ɕ֗������Ȋ֐��Q�ł��B

�Estd:string�̂��F�B�AAString(char)�ATString(TCHAR)�AWString(wchar_t)
�Etchar_*�Awchar_*�̒�`
�@TCHAR�pstrcpy��_tcscpy�Ƃ����������o���Ă��Ȃ��̂ŁB
�Echar,TCHAR,wchar_t�̑��ݕϊ�
�EAString�AWString�psprintf

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

//CHAR�^,WCHAR�^,TCHAR�^�̑��ݕϊ�
//�Ԃ��Ă����|�C���^�́AmDelete[]�Ŕj�����邱�ƁB
TCHAR* WCHAR2TCHAR( const WCHAR* str );
TCHAR* CHAR2TCHAR( const CHAR* str );
WCHAR* TCHAR2WCHAR( const TCHAR* str );
CHAR* TCHAR2CHAR( const TCHAR* str );
WCHAR* CHAR2WCHAR( const CHAR* str );
CHAR* WCHAR2CHAR( const WCHAR* str );

//��������R�s�[���āA���̕������Ԃ��B
//�Ԃ��Ă����|�C���^�́AmDelete[]�Ŕj�����邱�ƁB
CHAR* NewAndCopyString( const CHAR* src );
WCHAR* NewAndCopyString( const WCHAR* src );

//CHAR�^,WCHAR�^,TCHAR�^�̑��ݕϊ�
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

//AString��UTF16(LE)�̕����񂪊i�[����Ă���Ƃ��AWString�ɕϊ����Ď��o���܂�
WString CastToWString( const AString& src );

//WString�̃G���f�B�A����ϊ����܂�(UTF16LE <-->UTF16BE)
WString SwitchEndian( const WString& src );

INT sprintf_va( AString& ret_dest , const CHAR* format , va_list va );
INT sprintf_va( AString& ret_dest , const WCHAR* format , va_list va );
INT sprintf_va( WString& ret_dest , const CHAR* format , va_list va );
INT sprintf_va( WString& ret_dest , const WCHAR* format , va_list va );

INT sprintf_va( AString* ret_dest , const CHAR* format , va_list va );
INT sprintf_va( AString* ret_dest , const WCHAR* format , va_list va );
INT sprintf_va( WString* ret_dest , const CHAR* format , va_list va );
INT sprintf_va( WString* ret_dest , const WCHAR* format , va_list va );

//���ӁI
//�p�����[�^�ɕ�������g�p����Ƃ��A%s��CHAR*�^��WCHAR�^���́A
//ret_dest�̌^�Ɉˑ�����B���Ȃ킿�A
//ret_dest��String�^�@��	%s��const CHAR*�^
//ret_dest��WString�^�@��	%s��const WCHAR*�^
//�ƂȂ�B�ԈႦ��Ɣ�������̂Œ��ӂ��邱�ƁB

template< class t , class u > INT sprintf( t& ret_dest , const u* format , ... )
{
	ret_dest.clear();
	if( !format )
	{
		return -1;
	}

	//�ϒ����X�g
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( ret_dest , format , args );

	//�ϒ��������Z�b�g
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

	//�ϒ����X�g
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( ret_dest , format , args );

	//�ϒ��������Z�b�g
	va_end( args );

	return result;
}

//���ɕ������A������sprintf
template< class t , class u > INT appendf( t& ret_dest , const u* format , ... )
{
	if( !format )
	{
		return -1;
	}
	//���I�u�W�F�N�g
	t tmpstr;

	//�ϒ����X�g
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( tmpstr , format , args );

	//�ϒ��������Z�b�g
	va_end( args );

	//������A��
	ret_dest.append( tmpstr );

	return result;
}

//���ɕ������A������sprintf
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

	//���I�u�W�F�N�g
	t tmpstr;

	//�ϒ����X�g
	va_list args;
	va_start( args , format );

	INT result = sprintf_va( ret_dest , format , args );

	//�ϒ��������Z�b�g
	va_end( args );

	//������A��
	ret_dest->append( tmpstr );

	return result;
}

//������I�u�W�F�N�g��Ԃ�sprintf
template< class t > std::basic_string<t> makeprintf( const t* format , ... )
{
	//�ϒ����X�g
	va_list args;
	va_start( args , format );

	//���I�u�W�F�N�g
	std::basic_string<t> tmpstr;

	if( format )
	{
		sprintf_va( tmpstr , format , args );
	}
	//�ϒ��������Z�b�g
	va_end( args );

	return std::move( tmpstr );
}

//�o�C�i����16�i���ϊ�
bool Binary2String( WString& ret_dest , const BYTE* dat , DWORD len );
bool Binary2String( AString& ret_dest , const BYTE* dat , DWORD len );

//�������啶���ɕϊ�����
AString ToUpper( const AString& src );
//�������啶���ɕϊ�����
WString ToUpper( const WString& src );

//��������������ɕϊ�����
AString ToLower( const AString& src );
//��������������ɕϊ�����
WString ToLower( const WString& src );

//������𐔒l�ɕϊ�����
int ToInt( const AString& src , int defvalue = 0 );
//������𐔒l�ɕϊ�����
int ToInt( const WString& src , int defvalue = 0 );
//������𐔒l�ɕϊ�����
bool ToInt( const AString& src , int& retvalue , int defvalue );
//������𐔒l�ɕϊ�����
bool ToInt( const WString& src , int& retvalue , int defvalue );

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
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

//16�i������𐔒l�ɕϊ�����
unsigned int HexToUInt( const AString& src , unsigned int defvalue = 0 );
//16�i������𐔒l�ɕϊ�����
unsigned int HexToUInt( const WString& src , unsigned int defvalue = 0 );
//16�i������𐔒l�ɕϊ�����
bool HexToUInt( const AString& src , unsigned int& retvalue , unsigned int defvalue );
//16�i������𐔒l�ɕϊ�����
bool HexToUInt( const WString& src , unsigned int& retvalue , unsigned int defvalue );

//16�i������𐔒l�ɕϊ�����
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

//16�i������𐔒l�ɕϊ�����
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

//16�i������𐔒l�ɕϊ�����
unsigned int HexToULong( const AString& src , unsigned long defvalue = 0 );
//16�i������𐔒l�ɕϊ�����
unsigned int HexToULong( const WString& src , unsigned long defvalue = 0 );
//16�i������𐔒l�ɕϊ�����
bool HexToULong( const AString& src , unsigned long& retvalue , unsigned long defvalue );
//16�i������𐔒l�ɕϊ�����
bool HexToULong( const WString& src , unsigned long& retvalue , unsigned long defvalue );

//16�i������𐔒l�ɕϊ�����
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

//16�i������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
unsigned int ToUInt( const AString& src , unsigned int defvalue = 0 );
//������𐔒l�ɕϊ�����
unsigned int ToUInt( const WString& src , unsigned int defvalue = 0 );
//������𐔒l�ɕϊ�����
bool ToUInt( const AString& src , unsigned int& retvalue , unsigned int defvalue );
//������𐔒l�ɕϊ�����
bool ToUInt( const WString& src , unsigned int& retvalue , unsigned int defvalue );

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
unsigned int ToULong( const AString& src , unsigned long defvalue = 0 );
//������𐔒l�ɕϊ�����
unsigned int ToULong( const WString& src , unsigned long defvalue = 0 );
//������𐔒l�ɕϊ�����
bool ToULong( const AString& src , unsigned long& retvalue , unsigned long defvalue );
//������𐔒l�ɕϊ�����
bool ToULong( const WString& src , unsigned long& retvalue , unsigned long defvalue );

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
int64_t ToInt64( const AString& src , int64_t defvalue = 0 );
//������𐔒l�ɕϊ�����
int64_t ToInt64( const WString& src , int64_t defvalue = 0 );
//������𐔒l�ɕϊ�����
bool ToInt64( const AString& src , int64_t& retvalue , int64_t defvalue );
//������𐔒l�ɕϊ�����
bool ToInt64( const WString& src , int64_t& retvalue , int64_t defvalue );

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
uint64_t ToUInt64( const AString& src , uint64_t defvalue = 0 );
//������𐔒l�ɕϊ�����
uint64_t ToUInt64( const WString& src , uint64_t defvalue = 0 );
//������𐔒l�ɕϊ�����
bool ToUInt64( const AString& src , uint64_t& retvalue , uint64_t defvalue );
//������𐔒l�ɕϊ�����
bool ToUInt64( const WString& src , uint64_t& retvalue , uint64_t defvalue );

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
double ToDouble( const AString& src , double defvalue = 0.0 );
//������𐔒l�ɕϊ�����
double ToDouble( const WString& src , double defvalue = 0.0 );
//������𐔒l�ɕϊ�����
bool ToDouble( const AString& src , double& retvalue , double defvalue );
//������𐔒l�ɕϊ�����
bool ToDouble( const WString& src , double& retvalue , double defvalue );

//������𐔒l�ɕϊ�����
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

//������𐔒l�ɕϊ�����
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

//����������̕����Ńp�[�X����
void ParseString( const AString& str , CHAR delimiter , AStringVector& retParsed , bool noempty = false );
//����������̕����Ńp�[�X����
void ParseString( const AString& str , CHAR delimiter , AStringDeque& retParsed , bool noempty = false );
//����������̕����Ńp�[�X����
void ParseString( const WString& str , WCHAR delimiter , WStringVector& retParsed , bool noempty = false );
//����������̕����Ńp�[�X����
void ParseString( const WString& str , WCHAR delimiter , WStringDeque& retParsed , bool noempty = false );

//����������̕�����Ńp�[�X����
void ParseString( const AString& str , const AString& delimiter , AStringVector& retParsed , bool noempty = false );
//����������̕�����Ńp�[�X����
void ParseString( const AString& str , const AString& delimiter , AStringDeque& retParsed , bool noempty = false );
//����������̕�����Ńp�[�X����
void ParseString( const WString& str , const WString& delimiter , WStringVector& retParsed , bool noempty = false );
//����������̕�����Ńp�[�X����
void ParseString( const WString& str , const WString& delimiter , WStringDeque& retParsed , bool noempty = false );

//����������s�Ńp�[�X����
void ParseString( const AString& str , AStringVector& retParsed , bool noemptyline );
//����������s�Ńp�[�X����
void ParseString( const AString& str , AStringDeque& retParsed , bool noemptyline );
//����������s�Ńp�[�X����
void ParseString( const WString& str , WStringVector& retParsed , bool noemptyline );
//����������s�Ńp�[�X����
void ParseString( const WString& str , WStringDeque& retParsed , bool noemptyline );

//����������s�Ńp�[�X����(ParseString�Ɠ���)
void ParseStringNewLine( const AString& str , AStringVector& retParsed , bool noemptyline );
//����������s�Ńp�[�X����(ParseString�Ɠ���)
void ParseStringNewLine( const AString& str , AStringDeque& retParsed , bool noemptyline );
//����������s�Ńp�[�X����(ParseString�Ɠ���)
void ParseStringNewLine( const WString& str , WStringVector& retParsed , bool noemptyline );
//����������s�Ńp�[�X����(ParseString�Ɠ���)
void ParseStringNewLine( const WString& str , WStringDeque& retParsed , bool noemptyline );

//��������󔒕����Ńp�[�X����
void ParseStringSpace( const AString& str , AStringVector& retParsed , bool noemptyline );
//��������󔒕����Ńp�[�X����
void ParseStringSpace( const AString& str , AStringDeque& retParsed , bool noemptyline );
//��������󔒕����Ńp�[�X����
void ParseStringSpace( const WString& str , WStringVector& retParsed , bool noemptyline );
//��������󔒕����Ńp�[�X����
void ParseStringSpace( const WString& str , WStringDeque& retParsed , bool noemptyline );

//��������󔒕����܂��͓���̕����Ńp�[�X����
void ParseStringSpace( const AString& str , CHAR delimiter , AStringVector& retParsed , bool noemptyline );
//��������󔒕����܂��͓���̕����Ńp�[�X����
void ParseStringSpace( const AString& str , CHAR delimiter , AStringDeque& retParsed , bool noemptyline );
//��������󔒕����܂��͓���̕����Ńp�[�X����
void ParseStringSpace( const WString& str , WCHAR delimiter , WStringVector& retParsed , bool noemptyline );
//��������󔒕����܂��͓���̕����Ńp�[�X����
void ParseStringSpace( const WString& str , WCHAR delimiter , WStringDeque& retParsed , bool noemptyline );


//������̑��ݕϊ�
AString ConvertString( const AString& src , DWORD MapFlag , LCID locale = LOCALE_USER_DEFAULT );
AString ConvertHankaku2Zenkaku( const AString& src );	//���p���S�p
AString ConvertZenkaku2Hankaku( const AString& src );	//�S�p�����p
AString ConvertKatakana2Hiragana( const AString& src );	//�J�^�J�i���Ђ炪��
AString ConvertHiragana2Katakana( const AString& src );	//�Ђ炪�ȁ��J�^�J�i
AString ConvertLower2Upper( const AString& src );		//���������啶��
AString ConvertUpper2Lower( const AString& src );		//�啶����������

WString ConvertString( const WString& src , DWORD MapFlag , LCID locale = LOCALE_USER_DEFAULT );
WString ConvertHankaku2Zenkaku( const WString& src );	//���p���S�p
WString ConvertZenkaku2Hankaku( const WString& src );	//�S�p�����p
WString ConvertKatakana2Hiragana( const WString& src );	//�J�^�J�i���Ђ炪��
WString ConvertHiragana2Katakana( const WString& src );	//�Ђ炪�ȁ��J�^�J�i
WString ConvertLower2Upper( const WString& src );		//���������啶��
WString ConvertUpper2Lower( const WString& src );		//�啶����������

//�����񒆂̓��蕶�����u������
// src : ���̕�����
// findat : �T��������
// replaceto : �������������������ɒu��������
// ret_count : �u��������(nullptr��)
// ret : �u����������
AString ReplaceString( const AString& src , const AString& findat , const AString replaceto , DWORD* ret_count = nullptr );

//�����񒆂̓��蕶�����u������
// src : ���̕�����
// findat : �T��������
// replaceto : �������������������ɒu��������
// ret_count : �u��������(nullptr��)
// ret : �u����������
WString ReplaceString( const WString& src , const WString& findat , const WString replaceto , DWORD* ret_count = nullptr );

//�����񒆂̓��蕶�����u������
//���̃o�[�W�����̒u���ł́A��������J��Ԃ��u�����܂�
//��FAABAA��AB��BC�ɒu������ꍇ
//    AABAA��ABCAA��BCCAA �F�u��������=2
//�y���Ӂz�������[�v�ɂȂ�Ȃ��悤�C�����Ă�������
// src : ���̕�����
// findat : �T��������
// replaceto : �������������������ɒu��������
// ret_count : �u��������(nullptr��)
// ret : �u����������
AString ReplaceStringRecursive( const AString& src , const AString& findat , const AString replaceto , DWORD* ret_count = nullptr );

//�����񒆂̓��蕶�����u������
//���̃o�[�W�����̒u���ł́A��������J��Ԃ��u�����܂�
//��FAABAA��AB��BC�ɒu������ꍇ
//    AABAA��ABCAA��BCCAA �F�u��������=2
//�y���Ӂz�������[�v�ɂȂ�Ȃ��悤�C�����Ă�������
// src : ���̕�����
// findat : �T��������
// replaceto : �������������������ɒu��������
// ret_count : �u��������(nullptr��)
// ret : �u����������
WString ReplaceStringRecursive( const WString& src , const WString& findat , const WString replaceto , DWORD* ret_count = nullptr );

//NULL�����Ă�����strcmp
int StrCmp( const wchar_t* s1 , const wchar_t* s2 );
//NULL�����Ă�����strcmp
int StrCmp( const char* s1 , const char* s2 );

//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
//�I�[�`�F�b�N���Ȃ����ߒ��ӂ��Ă�������
void ReadDoubleNullString( const WCHAR* str , WStringVector& retArray );
//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
//�I�[�`�F�b�N���Ȃ����ߒ��ӂ��Ă�������
void ReadDoubleNullString( const WCHAR* str , WStringDeque& retArray );
//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
//�I�[�`�F�b�N���Ȃ����ߒ��ӂ��Ă�������
void ReadDoubleNullString( const CHAR* str , AStringVector& retArray );
//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
//�I�[�`�F�b�N���Ȃ����ߒ��ӂ��Ă�������
void ReadDoubleNullString( const CHAR* str , AStringDeque& retArray );

//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
void ReadDoubleNullString( const WString& str , WStringVector& retArray );
//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
void ReadDoubleNullString( const WString& str , WStringDeque& retArray );
//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
void ReadDoubleNullString( const AString& str , AStringVector& retArray );
//���s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶�����ǂݎ���āA�A���C�ɓW�J����
void ReadDoubleNullString( const AString& str , AStringDeque& retArray );

//�A���C�ɂ�����s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶������쐬����
void MakeDoubleNullString( const WStringVector& arr , WString& retStr );
//�A���C�ɂ�����s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶������쐬����
void MakeDoubleNullString( const WStringDeque& arr , WString& retStr );
//�A���C�ɂ�����s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶������쐬����
void MakeDoubleNullString( const AStringVector& arr , AString& retStr );
//�A���C�ɂ�����s���k�������A�ŏI�s���k�������~2�ɂȂ��Ă��镶������쐬����
void MakeDoubleNullString( const AStringDeque& arr , AString& retStr );

//������̗��[�ɂ���󔒂����ׂč폜����
AString TrimString( const AString& str );

//������̗��[�ɂ���󔒂����ׂč폜����
WString TrimString( const WString& str );

//������̋󔒂����ׂč폜����
AString RemoveSpace( const AString& str );

//������̋󔒂����ׂč폜����
WString RemoveSpace( const WString& str );

#endif

