//----------------------------------------------------------------------------
// 環境変数
// Copyright (C) 2023- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------
#ifndef MENVIRONMENTVARIABLE_H_INCLUDED
#define MENVIRONMENTVARIABLE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mException.h"

namespace mEnvironmentVariable
{

	//環境変数を取得します
	// key : 取得する環境変数
	// retResult : 結果の書込み先
	// def : エラー時の値
	// ret : 成功時真
	bool GetVariable( const WString& key , WString& retResult , const WString& def );

	//環境変数を取得します
	// key : 取得する環境変数
	// def : エラー時の値
	// ret : 環境変数の値、エラーの場合はdefの値
	inline WString GetVariable( const WString& key , const WString& def )
	{
		WString result;
		mEnvironmentVariable::GetVariable( key , result , def );
		return result;
	}

	//環境変数を取得します。エラーの場合は例外が発生します。
	// key : 取得する環境変数
	// ret : 環境変数の値
	template< class ExceptionIfFailed >
	inline WString GetVariable( const WString& key )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result;
		if( !mEnvironmentVariable::GetVariable( key , result , L"" ) )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}


	//環境変数を設定します。エラーの場合は例外が発生します。
	// key : 設定する環境変数
	// val : 設定する値
	// ret : 真
	template< class ExceptionIfFailed = bool >
	bool SetVariable( const WString& key , const WString& val )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		if( !mEnvironmentVariable::SetVariable<bool>( key , val ) )
		{
			throw ExceptionIfFailed();
		}
		return true;
	}

	//環境変数を設定します。
	// key : 設定する環境変数
	// val : 設定する値
	// ret : 成功時真
	template<>
	bool SetVariable< bool >( const WString& key , const WString& val );


}


#endif
