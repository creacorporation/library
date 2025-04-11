//----------------------------------------------------------------------------
// 例外クラス
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MEXCEPTION_CPP_COMPILING
#include "mException.h"
#include "General/mErrorLogger.h"


mException::mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , const WString& explain2 )
{
	MyPath = path;
	MyLine = line;
	MyCode = code;
	MyExplain1 = explain1;
	MyExplain2 = explain2;

	g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_EXCEPTION , MyPath , MyLine , MyLastError , MyCode , MyExplain1 , MyExplain2 );
	return;
}

mException::mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , DWORD_PTR val )
{
	MyPath = path;
	MyLine = line;
	MyCode = code;
	MyExplain1 = explain1;

#ifdef _WIN64
	sprintf( MyExplain2 , L"%llX(%lld)" , val , val );
#else
	sprintf( MyExplain2 , L"%lX(%ld)" , val , val );
#endif
	g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_EXCEPTION , MyPath , MyLine , MyLastError , MyCode , MyExplain1 , MyExplain2 );
	return;
}



mException::mException()
{
	MyLine = 0;
	MyCode = 0;
	return;
}

mException::~mException()
{
	return;
}

mException::mException( const mException& source )
{
	*this = source;
}

mException& mException::operator=( const mException& source )
{
	MyPath = source.MyPath;
	MyLine = source.MyLine;
	MyCode = source.MyCode;
	MyExplain1 = source.MyExplain1;
	MyExplain2 = source.MyExplain2;
	return *this;
}


WString mException::GetPath( void )const
{
	return MyPath;
}

DWORD mException::GetLine( void )const
{
	return MyLine;
}


ULONG_PTR mException::GetCode( void )const
{
	return MyCode;
}


WString mException::GetExplain( void )const
{
	return MyExplain1;
}

WString mException::GetExplainOption( void )const
{
	return MyExplain2;
}


