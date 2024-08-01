//----------------------------------------------------------------------------
// 正規表現クラス(std::regexpのラッパー)
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MREGEXP_H_INCLUDED
#define MREGEXP_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "General/mErrorLogger.h"
#include <regex>

template< class T > class mRegexp
{
public:

	//正規表現実行時のオプション
	struct Option
	{
		//trueにすると、大文字・小文字を区別しない
		//(今のところ、trueにすると正しく動作しなくなる。std::regexがバグってる気がする。)
		bool IgnoreCase;

		//trueにすると、マッチした部分文字列を格納しない
		bool NoSubstr;

		Option( void )
		{
			IgnoreCase = false;
			NoSubstr = false;
		}
		Option( bool ignore_case , bool nosubstr )
		{
			IgnoreCase = ignore_case;
			NoSubstr = nosubstr;
		}
	};

	//正規表現オブジェクトを構築する
	//opt : 正規表現マッチ処理時のオプション
	//      省略やnullptrでも可。その場合は、デフォルトの設定(Optionのコンストラクタに任せたときと同じ)になります。
	//正規表現のパターンを含んでいないため、このままでは何ともマッチしません。
	//マッチさせる前に、SetPattern()をコールしてください。
	mRegexp( const Option* opt = nullptr ) noexcept
	{
		std::basic_string<T> pattern;
		mRegexp( pattern , opt );
	}

	//正規表現オブジェクトを構築する
	//pattern : 正規表現のパターン
	//opt : 正規表現マッチ処理時のオプション
	//      省略やnullptrでも可。その場合は、デフォルトの設定(Optionのコンストラクタに任せたときと同じ)になります。
	//正規表現が間違っててもエラーになりません(g_ErrorLoggerに記録はされる)
	//正規表現がエラーになりうる場合はSetPattern()を使ってください。
	mRegexp( std::basic_string<T> pattern , const Option* opt = nullptr ) noexcept
	{
		if( opt != nullptr )
		{
			CreateRegexpObject( pattern , opt );
		}
		else
		{
			Option dummy_opt;
			CreateRegexpObject( pattern , &dummy_opt );
		}
	}

	//正規表現オブジェクトを構築する
	//pattern : 正規表現のパターン
	//opt : 正規表現マッチ処理時のオプション
	//正規表現が間違っててもエラーになりません(g_ErrorLoggerに記録はされる)
	//正規表現がエラーになりうる場合はSetPattern()を使ってください。
	mRegexp( std::basic_string<T> pattern , const Option& opt ) noexcept
	{
		CreateRegexpObject( pattern , &opt );
	}

	//見れば分かる、デストラクタ
	virtual ~mRegexp() = default;

	//正規表現パターンをセットする
	//pattern : 正規表現のパターン
	//opt : 正規表現マッチ処理時のオプション
	//      省略やnullptrを指定した場合は、それまでのオプション指定が維持されます
	//ret : 成功時true
	bool SetPattern( std::basic_string<T> pattern , const Option* opt = nullptr )
	{
		return CreateRegexpObject( pattern , opt );
	}

	//正規表現パターンをセットする
	//pattern : 正規表現のパターン
	//opt : 正規表現マッチ処理時のオプション
	//ret : 成功時true
	bool SetPattern( std::basic_string<T> pattern , const Option& opt )
	{
		return CreateRegexpObject( pattern , &opt );
	}

	//文字列が正規表現とマッチするか？
	//target : マッチさせる文字列
	//ret : マッチした場合true
	//・targetと部分的にマッチすればOKです(全体とマッチしたい場合はExactMatchを使用します)
	//・マッチさせたあと、Submatch()で部分文字列を取得できます(Option::NoSubstr=falseの場合)
	bool Match( const std::basic_string<T> target )
	{
		return MatchInternal( target , false );
	}

	//文字列全体が正規表現とマッチするか？
	//target : マッチさせる文字列
	//ret : マッチした場合true
	//・targetと全体がマッチすればOKです(一部分のみマッチしたい場合はMatchを使用します)
	//・マッチさせたあと、Submatch()で部分文字列を取得できます(Option::NoSubstr=falseの場合)
	bool ExactMatch( const std::basic_string<T> target )
	{
		return MatchInternal( target , true );
	}

	//部分文字列を格納したコンテナ
	typedef std::match_results<typename std::basic_string<T>::const_iterator> Matches;

	//部分文字列を格納したコンテナへの参照を取得します
	//ret : 部分文字列を格納したコンテナへの参照
	const Matches& Submatch( void ) const noexcept
	{
		return MyMatches;
	}

	//部分文字列を取得します
	//index : 部分文字列のインデックス（0=マッチ全体、1〜=部分文字列）
	//ret : 部分文字列
	std::basic_string<T> Submatch( size_t index ) const noexcept
	{
		return MyMatches[ index ];
	}

	//エラーコードを取得する
	std::regex_constants::error_type GetErrorCode( void )const
	{
		return MyErrorCode;
	}

	//エラーメッセージを取得する
	const WString& GetErrorMessage( void )const
	{
		return MyErrorMessage;
	}

	mRegexp( const mRegexp<T>& src )
	{
		MyRegex = src.MyRegex;
		MyTargetCopy = src.MyTargetCopy;
		MyMatches = src.MyMatches;
		MyErrorCode = src.MyErrorCode;
		MyErrorMessage = src.MyErrorMessage;
	}

	mRegexp<T>& operator=( const mRegexp<T>& src )
	{
		MyRegex = src.MyRegex;
		MyTargetCopy = src.MyTargetCopy;
		MyMatches = src.MyMatches;
		MyErrorCode = src.MyErrorCode;
		MyErrorMessage = src.MyErrorMessage;
	}

private:
	mRegexp() = delete;

protected:

	//正規表現オブジェクト
	std::basic_regex<T> MyRegex;

	//マッチさせた場合に、ターゲット文字列を一旦コピーするためのもの
	//部分文字列は、ターゲット文字列の一部分への参照を配列にした形で格納されるので、
	//ターゲットが一時オブジェクトだったりするとヌルリファレンスになってしまう。
	//それだと困るので、部分文字列を使う場合は一度コピーする。
	std::basic_string<T> MyTargetCopy;

	//部分文字列の格納先
	Matches MyMatches;

	//ライブラリ内でエラーが起きた場合のエラーコード格納先
	std::regex_constants::error_type MyErrorCode;

	//ライブラリ内でエラーが起きた場合のエラーメッセージ格納先
	WString MyErrorMessage;

	//正規表現オブジェクトを構築する
	//pattern : 正規表現のパターン
	//opt : 正規表現マッチ処理時のオプション
	//      省略やnullptrを指定した場合は、それまでのオプション指定が維持されます
	//ret : 成功時true
	bool CreateRegexpObject( typename  std::basic_string<T> pattern , const Option* opt = nullptr )
	{
		//現在のオプション設定値を取得
		DWORD flags = (DWORD)MyRegex.flags();

		//オプションが設定されている場合は、それに従い値を更新
		//nullptrである場合は、現状維持なので何もしない
		if( opt != nullptr )
		{
			//大文字・小文字の区別？
			flags &= ~std::regex_constants::icase;
			flags |= ( opt->IgnoreCase ) ? ( std::regex_constants::icase ) : ( 0 );
			//部分文字列を使う？
			flags &= ~std::regex_constants::nosubs;
			flags |= ( opt->NoSubstr ) ? ( std::regex_constants::nosubs ) : ( 0 );
		}

		//正規表現オブジェクトを構築。
		//エラーがある場合は例外が飛んでくるので、try-catchを使う。
		bool result = true;
		try
		{
			MyRegex.assign( pattern.cbegin() , pattern.cend() , (std::regex_constants::syntax_option_type)flags );
		}
		catch( std::regex_error e )
		{
			//エラーの場合は、エラーの記録をしてfalseを返す
			MyErrorCode = e.code();
			MyErrorMessage = AString2WString( e.what() );
			RaiseError( g_ErrorLogger , MyErrorCode , MyErrorMessage );
			return false;
		}
		return true;
	}

	//文字列が正規表現とマッチするか？
	//target : マッチさせる文字列
	//is_exact : trueの場合はtarget全体とマッチ、falseの場合は部分マッチ
	//ret : マッチした場合true
	bool MatchInternal( const std::basic_string<T> target , bool is_exact = false )
	{
		bool result = false;	//マッチしたか？

		//部分文字列を使用している場合は、ターゲットの文字列をコピーしてからマッチする
		//(ターゲットが一時変数だった場合ヌルリになるため)
		if( ( MyRegex.flags() & std::regex_constants::nosubs ) == 0 )
		{
			//部分文字列を使用する場合。
			//部分一致⇒regex_search 完全一致⇒regex_match
			MyTargetCopy = target;
			if( is_exact )
			{
				result = std::regex_match( MyTargetCopy.cbegin() , MyTargetCopy.cend() , MyMatches , MyRegex );
			}
			else
			{
				result = std::regex_search( MyTargetCopy.cbegin() , MyTargetCopy.cend() , MyMatches , MyRegex );
			}
		}
		else
		{
			//部分一致を使用しない場合
			if( is_exact )
			{
				result = std::regex_match( target.cbegin() , target.cend() , MyRegex );
			}
			else
			{
				result = std::regex_search( target.cbegin() , target.cend() , MyRegex );
			}
		}
		return result;
	}

};

//AString / CHAR用
typedef mRegexp<CHAR> mRegexpA;

//TString / TCHAR用
typedef mRegexp<TCHAR> mRegexpT;

//WString / WCHAR用
typedef mRegexp<WCHAR> mRegexpW;

#endif //MREGEXP_H_INCLUDED
