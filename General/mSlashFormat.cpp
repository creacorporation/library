//----------------------------------------------------------------------------
// スラッシュ区切りデータフォーマット
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------


#include "mSlashFormat.h"
#include "mTCHAR.h"

mSlashFormatA::mSlashFormatA( const mSlashFormatA& src )noexcept
{
	*this = src;
}

mSlashFormatA::mSlashFormatA( const AString& src )noexcept
{
	Parse( src );
}

void mSlashFormatA::operator=( const mSlashFormatA& src )noexcept
{
	Command = src.Command;
	WholeCommand = src.WholeCommand;
	Values = src.Values;
}

mSlashFormatW::mSlashFormatW( const mSlashFormatW& src )noexcept
{
	*this = src;
}

mSlashFormatW::mSlashFormatW( const WString& src )noexcept
{
	Parse( src );
}

void mSlashFormatW::operator=( const mSlashFormatW& src )noexcept
{
	Command = src.Command;
	WholeCommand = src.WholeCommand;
	Values = src.Values;
}

inline int template_isgraph( char c )
{
	return isgraph( (unsigned char)c );
}

inline int template_isgraph( wchar_t c )
{
	return wchar_isgraph( (unsigned short)c );
}

template< class T , class S >
static void ParseTemplate( const std::basic_string<T>& src , S& retData )
{
	using XString = std::basic_string<T>;

	int token_index = 0;
	auto PushToken = [&token_index , &retData]( const XString& token ) -> void
	{
		if( token_index == 0 )
		{
			static const T delimiter[ 3 ] = { (T)':' , (T)':' , (T)'\0' };

			retData.WholeCommand = token;
			ParseString( token , XString( delimiter ) , retData.Command );
		}
		else
		{
			if( token.empty() )
			{
				return;
			}
			size_t pos = token.find_first_of( (T)'=' );
			if( pos == XString::npos )
			{
				//ない
				retData.Values[ token ] = XString();
			}
			else if( pos == 0 )
			{
				//＝が最初
				retData.Values[ XString() ] = token.substr( pos + 1 );
			}
			else
			{
				//それ以外
				retData.Values[ token.substr( 0 , pos ) ] = token.substr( pos + 1 );
			}
		}
		token_index++;
	};

	XString token;
	int slash_count = 0;
	for( typename XString::const_iterator itr = src.begin() ; itr != src.end() ; itr++ )
	{
		if( ( token_index == 0 ) && !template_isgraph( *itr ) )
		{
			continue;
		}
		if( *itr == (T)'/' )
		{
			slash_count++;
			if( slash_count == 1 )
			{
				continue;
			}
		}
		if( slash_count == 1 )
		{
			PushToken( token );
			token.clear();
		}
		token += *itr;
		slash_count = 0;
	}

	PushToken( token );
	return;
}

void mSlashFormatA::Parse( const AString& src ) noexcept
{
	return ParseTemplate( src , *this );
}

void mSlashFormatW::Parse( const WString& src ) noexcept
{
	return ParseTemplate( src , *this );
}

static bool QueryInternalA( const mSlashFormatA::KeyValueMap& kv , const AString& key , const char* format_dec , void* ptr )
{
	if( kv.count( key ) == 0 )
	{
		return false;
	}
	// cppcheck-suppress invalidScanfArgType_int
	if( sscanf( kv.at( key ).c_str() , format_dec , ptr ) == 1 )
	{
		return true;
	}
	return false;
}

bool mSlashFormatA::Query( const AString& key , char& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%hhd" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , short& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%hd" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , int& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%ld" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , unsigned char& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%hhu" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , unsigned short& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%hu" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , unsigned int& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%lu" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , unsigned long& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%lu" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , double& retVal )const noexcept
{
	return QueryInternalA( Values , key , "%lf" , &retVal );
}

bool mSlashFormatA::Query( const AString& key , AString& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	retVal = Values.at( key );
	return true;
}

bool mSlashFormatA::Query( const AString& key , WString& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	retVal = AString2WString( Values.at( key ) );
	return true;
}

bool mSlashFormatA::Query( const AString& key , mDateTime::Date& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	return retVal.Set( Values.at( key ) );
}

bool mSlashFormatA::Query( const AString& key , mDateTime::Time& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	return retVal.Set( Values.at( key ) );
}


static bool QueryInternalW( const mSlashFormatW::KeyValueMap& kv , const WString& key , const wchar_t* format_dec , void* ptr )
{
	if( kv.count( key ) == 0 )
	{
		return false;
	}
	// cppcheck-suppress invalidScanfArgType_int
	if( wchar_sscanf( kv.at( key ).c_str() , format_dec , ptr ) == 1 )
	{
		return true;
	}
	return false;
}

bool mSlashFormatW::Query( const WString& key , char& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%hhd" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , short& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%hd" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , int& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%ld" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , unsigned char& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%hhu" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , unsigned short& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%hu" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , unsigned int& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%lu" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , unsigned long& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%lu" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , double& retVal )const noexcept
{
	return QueryInternalW( Values , key , L"%lf" , &retVal );
}

bool mSlashFormatW::Query( const WString& key , AString& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	retVal = WString2AString( Values.at( key ) );
	return true;
}

bool mSlashFormatW::Query( const WString& key , WString& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	retVal = Values.at( key );
	return true;
}

bool mSlashFormatW::Query( const WString& key , mDateTime::Date& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	return retVal.Set( Values.at( key ) );
}

bool mSlashFormatW::Query( const WString& key , mDateTime::Time& retVal )const noexcept
{
	if( Values.count( key ) == 0 )
	{
		return false;
	}
	return retVal.Set( Values.at( key ) );
}
