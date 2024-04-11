//----------------------------------------------------------------------------
// —áŠOƒNƒ‰ƒX
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MEXCEPTION_CPP_COMPILING
#include "mException.h"
#include "General/mErrorLogger.h"

mExceptionBase::mExceptionBase()noexcept
{
	MyLastError = GetLastError();
}

mExceptionBase::~mExceptionBase()noexcept
{
	return;
}

mExceptionBase::mExceptionBase( const mExceptionBase& source )noexcept
{
	*this = source;
}

mExceptionBase& mExceptionBase::operator=( const mExceptionBase& source )noexcept
{
	MyLastError = source.MyLastError;
	return *this;
}

DWORD mExceptionBase::GetErrorCode( void )const noexcept
{
	return MyLastError;
}

mException::mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , const WString& explain2 ) noexcept
{
	MyPath = path;
	MyLine = line;
	MyCode = code;
	MyExplain1 = explain1;
	MyExplain2 = explain2;

	g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_EXCEPTION , MyPath , MyLine , MyLastError , MyCode , MyExplain1 , MyExplain2 );
	return;
}

mException::mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , DWORD_PTR val ) noexcept
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



mException::mException() noexcept
{
	MyLine = 0;
	MyCode = 0;
	return;
}

mException::~mException() noexcept
{
	return;
}

mException::mException( const mException& source )noexcept
{
	*this = source;
}

mException& mException::operator=( const mException& source )noexcept
{
	MyPath = source.MyPath;
	MyLine = source.MyLine;
	MyCode = source.MyCode;
	MyExplain1 = source.MyExplain1;
	MyExplain2 = source.MyExplain2;
	return *this;
}


WString mException::GetPath( void )const noexcept
{
	return MyPath;
}

DWORD mException::GetLine( void )const noexcept
{
	return MyLine;
}


ULONG_PTR mException::GetCode( void )const noexcept
{
	return MyCode;
}


WString mException::GetExplain( void )const noexcept
{
	return MyExplain1;
}

WString mException::GetExplainOption( void )const noexcept
{
	return MyExplain2;
}


