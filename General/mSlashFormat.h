//----------------------------------------------------------------------------
// スラッシュ区切りデータフォーマット
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef SLASHFORMAT_H_INCLUDED
#define SLASHFORMAT_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mException.h"
#include "General/mDateTime.h"
#include <map>

//スラッシュフォーマットのASCII/SJISバージョン
class mSlashFormatA
{
public:
	using KeyValueMap = std::map< AString , AString >;
	AStringDeque Command;
	AString WholeCommand;
	KeyValueMap Values;

public:

	mSlashFormatA() = default;
	mSlashFormatA( const AString& src )noexcept;
	virtual ~mSlashFormatA() = default;
	mSlashFormatA( const mSlashFormatA& src )noexcept;
	void operator=( const mSlashFormatA& src )noexcept;

	void Parse( const AString& src ) noexcept;

public:

	bool Query( const AString& key , char& retVal )const noexcept;
	bool Query( const AString& key , short& retVal )const noexcept;
	bool Query( const AString& key , int& retVal )const noexcept;
	bool Query( const AString& key , unsigned char& retVal )const noexcept;
	bool Query( const AString& key , unsigned short& retVal )const noexcept;
	bool Query( const AString& key , unsigned int& retVal )const noexcept;
	bool Query( const AString& key , unsigned long& retVal )const noexcept;
	bool Query( const AString& key , double& retVal )const noexcept;
	bool Query( const AString& key , AString& retVal )const noexcept;
	bool Query( const AString& key , WString& retVal )const noexcept;
	bool Query( const AString& key , mDateTime::Date& retVal )const noexcept;
	bool Query( const AString& key , mDateTime::Time& retVal )const noexcept;

	template< class T , class ExceptionIfFailed > inline T Get( const AString& key )const
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );

		T retVal;
		if( Query( key , retVal ) )
		{
			return retVal;
		}
		throw ExceptionIfFailed();
	}

	template< class T > bool Query( const AString& key , T& retValue , const T& defvalue )const noexcept
	{
		if( Query( key , retValue ) )
		{
			return true;
		}
		retValue = defvalue;
		return false;
	}

	template< class T > T Get( const AString& key , const T& defvalue )const noexcept
	{
		T tmp;
		if( !Query( key , tmp ) )
		{
			tmp = defvalue;
		}
		return tmp;
	}

	template< class T > T Get( const AString& key , bool& retvalid )const noexcept
	{
		T tmp;
		retvalid = Query( key , tmp );
		return tmp;
	}

};

//スラッシュフォーマットのUNICODEバージョン
class mSlashFormatW
{
public:
	using KeyValueMap = std::map< WString , WString >;
	WStringDeque Command;
	WString WholeCommand;
	KeyValueMap Values;

public:

	mSlashFormatW() = default;
	mSlashFormatW( const WString& src )noexcept;
	virtual ~mSlashFormatW() = default;
	mSlashFormatW( const mSlashFormatW& src )noexcept;
	void operator=( const mSlashFormatW& src )noexcept;

	void Parse( const WString& src ) noexcept;

public:

	bool Query( const WString& key , char& retVal )const noexcept;
	bool Query( const WString& key , short& retVal )const noexcept;
	bool Query( const WString& key , int& retVal )const noexcept;
	bool Query( const WString& key , unsigned char& retVal )const noexcept;
	bool Query( const WString& key , unsigned short& retVal )const noexcept;
	bool Query( const WString& key , unsigned int& retVal )const noexcept;
	bool Query( const WString& key , unsigned long& retVal )const noexcept;
	bool Query( const WString& key , double& retVal )const noexcept;
	bool Query( const WString& key , AString& retVal )const noexcept;
	bool Query( const WString& key , WString& retVal )const noexcept;
	bool Query( const WString& key , mDateTime::Date& retVal )const noexcept;
	bool Query( const WString& key , mDateTime::Time& retVal )const noexcept;

	template< class T , class ExceptionIfFailed > inline T Get( const WString& key )const
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );

		T retVal;
		if( Query( key , retVal ) )
		{
			return retVal;
		}
		throw ExceptionIfFailed();
	}

	template< class T > bool Query( const WString& key , T& retValue , const T& defvalue )const noexcept
	{
		if( Query( key , retValue ) )
		{
			return true;
		}
		retValue = defvalue;
		return false;
	}

	template< class T > T Get( const WString& key , const T& defvalue )const noexcept
	{
		T tmp;
		if( !Query( key , tmp ) )
		{
			tmp = defvalue;
		}
		return tmp;
	}

	template< class T > T Get( const WString& key , bool& retvalid )const noexcept
	{
		T tmp;
		retvalid = Query( key , tmp );
		return tmp;
	}
};


#endif


