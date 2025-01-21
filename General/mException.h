//----------------------------------------------------------------------------
// 例外クラス
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
例外発生時に投げるオブジェクトの定義です。
EXCEPTIONマクロを使用して投げるのがおすすめです。

例：
	throw EXCEPTION( 0x80007AFF , L"エラーです" );
*/

#ifndef MEXCEPTION_H_INCLUDED
#define MEXCEPTION_H_INCLUDED

#include "mStandard.h"

class mExceptionBase
{
public:
	mExceptionBase()noexcept;
	virtual ~mExceptionBase()noexcept;
	mExceptionBase( const mExceptionBase& source )noexcept;
	mExceptionBase& operator=( const mExceptionBase& source )noexcept;

	DWORD GetErrorCode( void )const noexcept;
protected:
	//例外エラー発生時のエラーコード(GetLastErrorの値)
	DWORD MyLastError;
};

#include "mTCHAR.h"
#pragma warning( disable : 4290 )
#include <stdexcept>

class mException : public mExceptionBase
{
public:

	//例外オブジェクトを生成します
	//path : 例外を発生したファイル名
	//line : 例外を発生した行数
	//code : 例外発生時のエラーコード
	//explain : 例外に対する説明
	//※自動的に例外の発生をログに記録します
	mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , const WString& explain2 = L"" )noexcept;

	//例外オブジェクトを生成します
	//path : 例外を発生したファイル名
	//line : 例外を発生した行数
	//code : 例外発生時のエラーコード
	//explain : 例外に対する説明
	//※自動的に例外の発生をログに記録します
	mException( const WString& path , DWORD line , ULONG_PTR code , const WString& explain1 , DWORD_PTR val )noexcept;

	//空の例外オブジェクトを生成します
	//※自動的に例外の発生をログに記録しません
	mException() noexcept;

	virtual ~mException()noexcept;
	mException( const mException& source )noexcept;
	mException& operator=( const mException& source )noexcept;

public:

	//例外を発生したファイル名を取得します
	//ret : 例外発生時のファイル名
	WString GetPath( void )const noexcept;

	//例外を発生したコード行を得ます
	//ret : 例外発生時のコード行
	DWORD GetLine( void )const noexcept;

	//例外を発生したエラーコードを得ます
	//ret : 例外発生時のエラーコード
	ULONG_PTR GetCode( void )const noexcept;

	//例外に対する説明を得ます
	//ret : 例外発生時の説明文
	WString GetExplain( void )const noexcept;

	//例外に対する拡張説明を得ます
	//ret : 例外発生時の説明文
	WString GetExplainOption( void )const noexcept;

protected:

	//例外発生時のファイル名
	WString MyPath;
	//例外発生時のコード行
	DWORD MyLine;
	//例外発生時のエラーコード
	ULONG_PTR MyCode;
	//例外発生時の説明文
	WString MyExplain1;
	WString MyExplain2;

};

//例外オブジェクトを生成します
//code : 例外発生時のエラーコード
//explain : 例外に対する説明
#define EXCEPTION(error_code,...) mException( mCURRENT_FILE , __LINE__ , error_code , __VA_ARGS__ )

//チェックに失敗した場合はテンプレート引数で指定した例外を投げる
//例外クラスはmExceptionBaseから派生したものであること。
// ※参考→ using err = class ErrorClass : mExceptionBase{};
//          Check<err>( SomeFunction() );
template< class ExceptionIfFailed >
inline void Check( bool v )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
	if( !v ) throw ExceptionIfFailed();
}

#endif //MEXCEPTION_H_INCLUDED 
