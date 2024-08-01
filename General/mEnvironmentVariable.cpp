//----------------------------------------------------------------------------
// 環境変数
// Copyright (C) 2023- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MENVIRONMENTVARIABLE_CPP_COMPILING
#include "mEnvironmentVariable.h"

bool mEnvironmentVariable::GetVariable( const WString& key , WString& result , const WString& def )
{
	DWORD reqsize = GetEnvironmentVariableW( key.c_str() , nullptr , 0 );
	if( !reqsize )
	{
		result = def;
		return false;
	}

	std::unique_ptr<wchar_t> p( mNew wchar_t[ reqsize ] );
	if( !GetEnvironmentVariableW( key.c_str() , p.get() , reqsize ) )
	{
		result = def;
		return false;
	}

	result = p.get();
	return true;
}

template<>
bool mEnvironmentVariable::SetVariable< bool >( const WString& key , const WString& val )
{
	if( SetEnvironmentVariableW( key.c_str() , val.c_str() ) )
	{
		return true;
	}
	return false;
}

