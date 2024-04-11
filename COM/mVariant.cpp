//----------------------------------------------------------------------------
// Variant型ハンドラ
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mVariant.h"

mVariant::mVariant()
{
}

mVariant::~mVariant()
{
}

mVariant::mVariant( const _variant_t& src )
{
	MyOriginal = src;
	MyConverted = 0;
}

mVariant::mVariant( const mVariant& src )
{
	MyOriginal = src.MyOriginal;
	MyConverted = 0;
}

const mVariant& mVariant::operator=( const _variant_t& src )
{
	MyOriginal = src;
	MyConverted = 0;
	return *this;
}

const mVariant& mVariant::operator=( const mVariant& src )
{
	MyOriginal = src.MyOriginal;
	MyConverted = 0;
	return *this;
}

bool mVariant::IsEmpty( void )const
{
	return MyOriginal.vt == VT_EMPTY;
}

void mVariant::SetEmpty( void )
{
	MyOriginal.Clear();
}

const mVariant& mVariant::operator=( char val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( short val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( int val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( long val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( __int64 val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( unsigned char val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( unsigned short val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( unsigned int val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( unsigned long val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( unsigned __int64 val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( float val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( double val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( const WString& val )
{
	Set( val );
	return *this;
}

const mVariant& mVariant::operator=( const AString& val )
{
	Set( val );
	return *this;
}

void mVariant::Set( char val )
{
	MyOriginal = val;
}

void mVariant::Set( short val )
{
	MyOriginal = val;
}

void mVariant::Set( int val )
{
	MyOriginal = val;
}

void mVariant::Set( long val )
{
	MyOriginal = val;
}

void mVariant::Set( __int64 val )
{
	MyOriginal = val;
}

void mVariant::Set( unsigned char val )
{
	MyOriginal = val;
}

void mVariant::Set( unsigned short val )
{
	MyOriginal = val;
}

void mVariant::Set( unsigned int val )
{
	MyOriginal = val;
}

void mVariant::Set( unsigned long val )
{
	MyOriginal = val;
}

void mVariant::Set( unsigned __int64 val )
{
	MyOriginal = val;
}

void mVariant::Set( float val )
{
	MyOriginal = val;
}

void mVariant::Set( double val )
{
	MyOriginal = val;
}

void mVariant::Set( const WString& val )
{
	MyOriginal = val.c_str();
}

void mVariant::Set( const AString& val )
{
	MyOriginal = AString2WString( val ).c_str();
}

void mVariant::Get( bool& retresult , char& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%hc" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_I1 , &MyOriginal );
			retval = MyConverted.cVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , short& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		int tmp = 0;
		if( wchar_sscanf( MyOriginal.bstrVal , L"%d" , &tmp ) != 1 )
		{
			retresult = false;
			return;
		}
		if( tmp < SHRT_MIN || SHRT_MAX < tmp )
		{
			retresult = false;
			return;
		}
		retval = (short)tmp;
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_I2 , &MyOriginal );
			retval = MyConverted.iVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , int& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%d" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_INT , &MyOriginal );
			retval = MyConverted.intVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , long& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%ld" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_I4 , &MyOriginal );
			retval = MyConverted.lVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}



void mVariant::Get( bool& retresult , __int64& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%lld" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_I8 , &MyOriginal );
			retval = MyConverted.llVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}


void mVariant::Get( bool& retresult , unsigned char& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%hc" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_UI1 , &MyOriginal );
			retval = MyConverted.bVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , unsigned short& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		unsigned int tmp = 0;
		if( wchar_sscanf( MyOriginal.bstrVal , L"%u" , &tmp ) != 1 )
		{
			retresult = false;
			return;
		}
		if( tmp < 0 || USHRT_MAX < tmp )
		{
			retresult = false;
			return;
		}
		retval = (unsigned short)tmp;
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_UI2 , &MyOriginal );
			retval = MyConverted.uiVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , unsigned int& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%u" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_UI4 , &MyOriginal );
			retval = MyConverted.uintVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , unsigned long& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%lu" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_UI4 , &MyOriginal );
			retval = MyConverted.ulVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}


void mVariant::Get( bool& retresult , unsigned __int64& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%llu" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_UI4 , &MyOriginal );
			retval = MyConverted.ulVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}

void mVariant::Get( bool& retresult , float& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%f" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_R4 , &MyOriginal );
			retval = MyConverted.fltVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}


void mVariant::Get( bool& retresult , double& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		if( wchar_sscanf( MyOriginal.bstrVal , L"%lf" , &retval ) != 1 )
		{
			retresult = false;
			return;
		}
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_R8 , &MyOriginal );
			retval = MyConverted.dblVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}


void mVariant::Get( bool& retresult , WString& retval )const
{
	if( MyOriginal.vt == VT_BSTR )
	{
		retval = MyOriginal.bstrVal;
		retresult = true;
		return;
	}
	else
	{
		try
		{
			MyConverted.ChangeType( VT_BSTR , &MyOriginal );
			retval = MyConverted.bstrVal;
		}
		catch( ... )
		{
			retresult = false;
			return;
		}
	}
	retresult = true;
	return;
}


void mVariant::Get( bool& retresult , AString& retval )const
{
	WString tmp;
	Get( retresult , tmp );

	if( retresult )
	{
		retval = WString2AString( tmp );
	}
	return;
}

WString mVariant::Get( const wchar_t* defvalue )const
{
	WString tmp;
	bool result;
	Get( result , tmp );

	if( !result )
	{
		return WString( defvalue );
	}
	return tmp;
}

AString mVariant::Get( const char* defvalue )const
{
	WString tmp;
	bool result;
	Get( result , tmp );

	if( !result )
	{
		return defvalue;
	}
	return WString2AString( tmp );
}


