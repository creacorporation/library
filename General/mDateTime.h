//----------------------------------------------------------------------------
// 日付時刻管理
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------
#ifndef MTIMESTAMP_H_INCLUDED
#define MTIMESTAMP_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mException.h"

namespace Definitions_mDateTime
{
	enum DAYOFWEEK
	{
		DAY_SUNDAY,
		DAY_MONDAY,
		DAY_TUESDAY,
		DAY_WEDNESDAY,
		DAY_THURSDAY,
		DAY_FRIDAY,
		DAY_SATURDAY,
		DAY_INVALID,
	};

	enum JAPANESE_ERA
	{
		JPERA_UNKNOWN,		//不明
		JPERA_REIWA,		//令和
		JPERA_HEISEI,		//平成
		JPERA_SHOWA,		//昭和
		JPERA_TAISYO,		//大正
		JPERA_MEIJI,		//明治
		eMAX_JAPANESE_ERA
	};

	enum INIT_WITH
	{
		CURRENT_SYSTEMTIME,
		CURRENT_LOCALTIME,
	};
};

namespace mDateTime
{
	using DAYOFWEEK = Definitions_mDateTime::DAYOFWEEK;
	using JAPANESE_ERA = Definitions_mDateTime::JAPANESE_ERA;
	using INIT_WITH = Definitions_mDateTime::INIT_WITH;

	void AdjustYearMonth( INT& ioYear , INT& ioMonth );

	bool IsLeapYear( INT Year );
	INT GetLastDay( INT Year , INT Month );

	struct JapaneseEra
	{
		JAPANESE_ERA Id;
		const wchar_t* Word;
		INT Year;
	};

	struct YearMonth
	{
		INT Year;	//年(グレゴリオ暦)
		INT Month;	//月

		void Clear()
		{
			Year = 9999;
			Month = 12;
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		YearMonth( INIT_WITH init_with );
		YearMonth( const AString& src );
		YearMonth( const WString& src );

		YearMonth( void )
		{
			Clear();
		}

		YearMonth( INT year , INT month )
		{
			Set( year , month );
		}

		void Set( INT year , INT month )
		{
			Year = year;
			Month = month;
		}

		bool Set( const AString& src );
		bool Set( const WString& src );

		//格納している日付が有効であるか否かを返します
		//ここで言う有効とは、実在する日付かどうかです
		//ret : 日付が有効であれば真
		bool IsValid( void )const;

		//日付に対する和暦を求めます
		//何も指定しないとその月の初日について求めます。日にちを指定するとその日について求めます。
		bool GetJapaneseEra( JapaneseEra& retEra , int day = 1 )const noexcept;

		bool operator<=( const YearMonth& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const YearMonth& src ) const
		{
			if( Year  != src.Year  ) return Year  < src.Year;
			return Month < src.Month;
		}
		bool operator>=( const YearMonth& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const YearMonth& src ) const
		{
			if( Year  != src.Year  ) return Year  > src.Year;
			return Month > src.Month;
		}
		bool operator==( const YearMonth& src ) const
		{
			if( Month != src.Month ) return false;
			if( Year  != src.Year  ) return false;
			return true;
		}
		bool operator!=( const YearMonth& src ) const
		{
			return !( this->operator==( src ) );
		}

		//月を指定日数分だけ変化します
		//変化後の「日」が月の最終日を超えると、最終日に補正します。
		//変化前の年月日が不正であると変化後も不正になります
		void AddMonth( int offset );

		//事業年度を求めます
		//StartMonth : 事業年度の開始月(４ならば４月から翌年３月までが１事業年度)
		INT GetBusinessYear( INT StartMonth = 4 )const;

		//正規化します
		// 2020年13月→2021年1月のように補正します
		void Normalize( void )noexcept;
	};

	//日付・タイムスタンプを指定した月の初日で初期化する
	struct InitWithFirstDayOf
	{
		InitWithFirstDayOf()
		{
			SYSTEMTIME tm;
			GetLocalTime( &tm );
			year = tm.wYear;
			month = tm.wMonth;
		}

		//日付・タイムスタンプを指定した月の初日で初期化する
		//　monthにゼロ以下の数字、13以上の数字を入れてもかまいません。
		//　month=0,-1・・・→前年12月、11月・・・
		//　month=13,14・・・→翌年1月、2月・・・
		//　とみなします。
		InitWithFirstDayOf( INT Year , INT Month )
		{
			year = Year;
			month = Month;
		}

		//日付・タイムスタンプを指定した月の初日で初期化する
		InitWithFirstDayOf( const YearMonth& ym )
		{
			year = ym.Year;
			month = ym.Month;
		}

		INT year;
		INT month;
	};

	//日付・タイムスタンプを指定した月の末日で初期化する
	struct InitWithLastDayOf
	{
		InitWithLastDayOf()
		{
			SYSTEMTIME tm;
			GetLocalTime( &tm );
			year = tm.wYear;
			month = tm.wMonth;
		}
		//日付・タイムスタンプを指定した月の末日で初期化する
		//　monthにゼロ以下の数字、13以上の数字を入れてもかまいません。
		//　month=0,-1・・・→前年12月、11月・・・
		//　month=13,14・・・→翌年1月、2月・・・
		//　とみなします。
		InitWithLastDayOf( INT Year , INT Month )
		{
			year = Year;
			month = Month;
		}

		//日付・タイムスタンプを指定した月の初日で初期化する
		InitWithLastDayOf( const YearMonth& ym )
		{
			year = ym.Year;
			month = ym.Month;
		}

		INT year;
		INT month;
	};

	//グレゴリオ暦による日付を格納します
	//・日本がグレゴリオ暦を採用したのは1873年1月1日です。
	//　日本の暦計算をする目的で、これ以前の日付を格納すると誤った結果になります。
	struct Date
	{
		INT Year;	//年(グレゴリオ暦)
		INT Month;	//月
		INT Day;	//日

		void Clear()
		{
			Year = 9999;
			Month = 12;
			Day = 31;
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		Date( INIT_WITH init_with );

		//注意：InitWithFirstDayOfを一時オブジェクトとして作る場合に、初期化引数を変数にすると、関数定義に誤認されるから注意。
		//×　mDateTime::Date day( mDateTime::InitWithFirstDayOf( year_month ) );
		//○　mDateTime::Date day = mDateTime::InitWithFirstDayOf( year_month );
		Date( const InitWithFirstDayOf& init_with );

		//注意：InitWithLastDayOfを一時オブジェクトとして作る場合に、初期化引数を変数にすると、関数定義に誤認されるから注意。
		//×　mDateTime::Date day( mDateTime::InitWithLastDayOf( year_month ) );
		//○　mDateTime::Date day = mDateTime::InitWithLastDayOf( year_month );
		Date( const InitWithLastDayOf& init_with );
		Date( const AString& src );
		Date( const WString& src );
		Date( const SYSTEMTIME& src );

		template< class ExceptionIfFailed >
		Date( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		template< class ExceptionIfFailed >
		Date( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		Date( void )
		{
			Clear();
		}

		Date( INT year , INT month , INT day )
		{
			Set( year , month , day );
		}

		void Set( INT year , INT month , INT day )
		{
			Year = year;
			Month = month;
			Day = day;
		}

		//文字列を読み取って日付をセットします。
		//文字列を日付として読み取れない場合は、テンプレートで指定した型の例外を投げます。この型はmExceptionの派生型でなければなりません。
		// ※参考→ using err = class ErrorClass : mException{};
		//src : 読み取る文字列
		template< class ExceptionIfFailed >
		void Set( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//文字列を読み取って日付をセットします。
		//文字列を日付として読み取れない場合は、テンプレートで指定した型の例外を投げます。この型はmExceptionの派生型でなければなりません。
		// ※参考→ using err = class ErrorClass : mException{};
		//src : 読み取る文字列
		template< class ExceptionIfFailed >
		void Set( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//文字列を読み取って日付をセットします。
		//src : 読み取る文字列
		//ret : 成功時真
		bool Set( const AString& src );

		//文字列を読み取って日付をセットします。
		//src : 読み取る文字列
		//ret : 成功時真
		bool Set( const WString& src );

		//SYSTEMTIME構造体を読み取って日付をセットします。
		//src : 読み取る文字列
		//ret : 成功時真
		bool Set( const SYSTEMTIME& src );

		//格納している日付が有効であるか否かを返します
		//ここで言う有効とは、実在する日付かどうかです
		//ret : 日付が有効であれば真
		bool IsValid( void )const;

		//格納している日付の修正ユリウス通日を求めます
		//※有効な日付を格納していない場合は不正な結果になります
		//ret : 格納している日付の修正ユリウス通日
		double ToModJulian( void )const;

		//修正ユリウス通日から日付を取得して格納します
		//julian : 修正ユリウス通日
		void FromModJulian( double julian );

		//文字列化して返します
		// format : フォーマット文字列。ヌルにすると現在のユーザーの規定。
		//  中身はGetDateFormatExのため、そのフォーマットに従う
		// 【カンペ】
		//  d/dd      = 日(先頭0なし/0あり) 
		//  M/MM      = 月(先頭0なし/0あり) 
		//  y/yy/yyyy = 年(先頭0なし/2桁0あり/和暦) 
		//  ddd  = Mon,Tue,Wed...
		//  MMM  = Jan,Feb,Mar...
		//  g,gg = 昭和,平成,令和...
		//  そのまま表示したい文字は '月' のようにシングルクォートでくくる。'そのものは''で表記する。
		//    例:  "yy'年'M'月'"
		AString ToAString( const char* format = nullptr )const;

		//文字列化して返します
		// format : フォーマット文字列。ヌルにすると現在のユーザーの規定。
		//  中身はGetDateFormatExのため、そのフォーマットに従う
		// 【カンペ】
		//  d/dd      = 日(先頭0なし/0あり) 
		//  M/MM      = 月(先頭0なし/0あり) 
		//  y/yy/yyyy = 年(先頭0なし/2桁0あり/和暦) 
		//  ddd  = Mon,Tue,Wed...
		//  MMM  = Jan,Feb,Mar...
		//  g,gg = 昭和,平成,令和...
		//  そのまま表示したい文字は '月' のようにシングルクォートでくくる。'そのものは''で表記する。
		//    例:  "yy'年'M'月'"
		WString ToWString( const wchar_t* format = nullptr )const;

		//日付に対する和暦を求めます
		bool GetJapaneseEra( JapaneseEra& retEra )const noexcept;

		//曜日を求めます
		DAYOFWEEK DayOfWeek( void ) const noexcept;

		//指定年月の初日に設定します
		//　monthにゼロ以下の数字、13以上の数字を入れてもかまいません。
		//　month=0,-1・・・→前年12月、11月・・・
		//　month=13,14・・・→翌年1月、2月・・・
		//　とみなします。
		void SetFirstDay( INT year , INT month )noexcept;

		//指定年月の初日に設定します
		//　monthにゼロ以下の数字、13以上の数字を入れてもかまいません。
		//　month=0,-1・・・→前年12月、11月・・・
		//　month=13,14・・・→翌年1月、2月・・・
		//　とみなします。
		void SetFirstDay( const YearMonth& ym )noexcept;

		//指定年月の最終日に設定します
		//　monthにゼロ以下の数字、13以上の数字を入れてもかまいません。
		//　month=0,-1・・・→前年12月、11月・・・
		//　month=13,14・・・→翌年1月、2月・・・
		//　とみなします。
		void SetLastDay( INT year , INT month )noexcept;

		//指定年月の最終日に設定します
		//　monthにゼロ以下の数字、13以上の数字を入れてもかまいません。
		//　month=0,-1・・・→前年12月、11月・・・
		//　month=13,14・・・→翌年1月、2月・・・
		//　とみなします。
		void SetLastDay( const YearMonth& ym )noexcept;

		bool operator<=( const Date& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const Date& src ) const
		{
			if( Year  != src.Year  ) return Year  < src.Year;
			if( Month != src.Month ) return Month < src.Month;
			return Day < src.Day;
		}
		bool operator>=( const Date& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const Date& src ) const
		{
			if( Year  != src.Year  ) return Year  > src.Year;
			if( Month != src.Month ) return Month > src.Month;
			return Day > src.Day;
		}
		bool operator==( const Date& src ) const
		{
			if( Day   != src.Day   ) return false;
			if( Month != src.Month ) return false;
			if( Year  != src.Year  ) return false;
			return true;
		}
		bool operator!=( const Date& src ) const
		{
			return !( this->operator==( src ) );
		}

		//日付を指定日数分だけ変化します
		//変化前の年月日が不正であると変化後も不正になります
		void AddDay( int offset );

		//月を指定月数分だけ変化します
		//変化後の「日」が月の最終日を超えると、最終日に補正します。
		//ただし、この補正が起きたことを記憶するわけではないので、繰り返し変化する場合は注意が必要です。
		//例：2020年1月31日→AddMonth(1)→2020年2月29日→AddMonth(1)→2020年3月29日
		//変化前の年月日が不正であると変化後も不正になります
		void AddMonth( int offset );

		//事業年度を求めます
		//StartMonth : 事業年度の開始月(４ならば４月から翌年３月までが１事業年度)
		INT GetBusinessYear( INT StartMonth = 4 )const;

	};

	//時差格納用時刻構造体
	struct TimeBias
	{
		INT Hour;			//時
		UINT Minute;		//分(0-59)
		TimeBias()
		{
			Clear();
		}
		void Clear()
		{
			Hour = 0;
			Minute = 0;
		}
		void Set( INT hour = 0 , INT minute = 0 )
		{
			Hour = hour;
			Minute = minute;
		}
	};

	//時刻を格納します
	struct Time
	{
		INT Hour;			//時(0-23)
		INT Minute;			//分(0-59)
		INT Second;			//秒(0-59)
		INT Milliseconds;	//ミリ秒(0-999)

		void Clear()
		{
		   Hour = 0;
		   Minute = 0;
		   Second = 0;
		   Milliseconds = 0;
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		Time( double src );
		Time( INIT_WITH INIT_WITH );
		Time( const AString& src );
		Time( const WString& src );
		Time( const SYSTEMTIME& src );

		template< class ExceptionIfFailed >
		Time( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		template< class ExceptionIfFailed >
		Time( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		Time( INT hour = 0 , INT minute = 0 , INT second = 0 , INT milliseconds = 0 )
		{
		   Set( hour , minute , second , milliseconds );
		}

		void Set( INT hour = 0 , INT minute = 0 , INT second = 0 , INT milliseconds = 0 )
		{
		   Hour = hour;
		   Minute = minute;
		   Second = second;
		   Milliseconds = milliseconds;
		}

		//文字列を読み取って時刻をセットします。
		//src : 読み取る文字列
		//ret : 成功時真
		bool Set( const AString& src );

		//文字列を読み取って時刻をセットします。
		//src : 読み取る文字列
		//ret : 成功時真
		bool Set( const WString& src );

		//SYSTEMTIMEを読み取って時刻をセットします。
		//src : 読み取るSYSTEMTIME
		//ret : 成功時真
		bool Set( const SYSTEMTIME& src );

		//文字列を読み取って時刻をセットします。
		//文字列を日付として読み取れない場合は、テンプレートで指定した型の例外を投げます。この型はmExceptionの派生型でなければなりません。
		// ※参考→ using err = class ErrorClass : mException{};
		//src : 読み取る文字列
		template< class ExceptionIfFailed >
		void Set( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//文字列を読み取って時刻をセットします。
		//文字列を日付として読み取れない場合は、テンプレートで指定した型の例外を投げます。この型はmExceptionの派生型でなければなりません。
		// ※参考→ using err = class ErrorClass : mException{};
		//src : 読み取る文字列
		template< class ExceptionIfFailed >
		void Set( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//格納している時刻が有効であるか否かを返します
		//※ここで言う有効とは、0:0:0.000〜23:59:59.999の範囲内かつ、各パートが繰り上がり・繰り下がりなく表記可能か否かです。
		//ret : 時刻が有効であれば真
		bool IsValid( void )const;

		//格納している時刻に対して、1日を1.0とした比率を求めます
		//※有効な日付を格納していない場合は不正な結果になります
		//※修正ユリウス通日の小数点以下の値として使用できます
		//ret : 格納している時刻の1日に対する比率
		double ToValue( void )const;

		//1日を1.0とした比率から時刻を取得して格納します
		// value : 格納している時刻の1日に対する比率
		void FromValue( double value );

		//時間単位に直します
		double ToHour( void )const;

		//分単位に直します
		double ToMinute( void )const;

		//秒単位に直します
		double ToSecond( void )const;

		//正規化します
		// 1:00:80 → 1:01:20のように補正します
		void Normalize( void )noexcept;

		//文字列化して返します
		// format : フォーマット文字列。ヌルにすると現在のユーザーの規定。
		//  中身はGetTimeFormatExのため、そのフォーマットに従う
		//  ミリ秒の処理はされない
		// 【カンペ】
		//  h/hh = 12時間制の時(先頭0なし/0あり) 
		//  H/HH = 24時間制の時(先頭0なし/0あり) 
		//  m/mm = 分(先頭0なし/0あり) 
		//  s/ss = 秒(先頭0なし/0あり) 
		//  t    = A or P
		//  tt   = AM or PM
		//  そのまま表示したい文字は '分' のようにシングルクォートでくくる。'そのものは''で表記する。
		//    例:  "HH'時'mm'分'"　→　15時07分
		AString ToAString( const char* format = nullptr )const;

		//文字列化して返します
		// format : フォーマット文字列。ヌルにすると現在のユーザーの規定。
		//  中身はGetTimeFormatExのため、そのフォーマットに従う
		//  ミリ秒の処理はされない
		// 【カンペ】
		//  h/hh = 12時間制の時(先頭0なし/0あり) 
		//  H/HH = 24時間制の時(先頭0なし/0あり) 
		//  m/mm = 分(先頭0なし/0あり) 
		//  s/ss = 秒(先頭0なし/0あり) 
		//  t    = A or P
		//  tt   = AM or PM
		//  そのまま表示したい文字は '分' のようにシングルクォートでくくる。'そのものは''で表記する。
		//    例:  "HH'時'mm'分'"　→　15時07分
		WString ToWString( const wchar_t* format = nullptr )const;

		bool operator<=( const Time& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const Time& src ) const
		{
		   if( Hour   != src.Hour   ) return Hour   < src.Hour  ;
		   if( Minute != src.Minute ) return Minute < src.Minute;
		   if( Second != src.Second ) return Second < src.Second;
		   return Milliseconds < src.Milliseconds;
		}
		bool operator>=( const Time& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const Time& src ) const
		{
		   if( Hour   != src.Hour   ) return Hour   > src.Hour  ;
		   if( Minute != src.Minute ) return Minute > src.Minute;
		   if( Second != src.Second ) return Second > src.Second;
		   return Milliseconds < src.Milliseconds;
		}
		bool operator==( const Time& src ) const
		{
		   if( Second       != src.Second       ) return false;
		   if( Minute       != src.Minute       ) return false;
		   if( Hour         != src.Hour         ) return false;
		   if( Milliseconds != src.Milliseconds ) return false;
		   return true;
		}
		bool operator!=( const Time& src ) const
		{
			return !( this->operator==( src ) );
		}

		const Time operator+( const Time& v )const;
		void operator+=( const Time& v );
		const Time operator-( const Time& v )const;
		void operator-=( const Time& v );
		const Time operator*( double v )const;
		void operator*=( double v );
		const Time operator/( double v )const;
		double operator/( const Time& divider )const;
		void operator/=( double v );
		const Time operator%( double v )const;
		void operator%=( double v );

	};

	//グレゴリオ暦によるタイムスタンプを格納します
	//・日本がグレゴリオ暦を採用したのは1873年1月1日です。
	//　日本の暦計算をする目的で、これ以前の日付を格納すると誤った結果になります。
	struct Timestamp
	{
		INT Year;			//年(グレゴリオ暦)
		INT Month;			//月
		INT Day;			//日
		INT Hour;			//時(0-23)
		INT Minute;			//分(0-59)
		INT Second;			//秒(0-59)
		INT Milliseconds;	//ミリ秒(0-999)

		void Clear()
		{
		   Year = 9999;
		   Month = 12;
		   Day = 31;
		   Hour = 0;
		   Minute = 0;
		   Second = 0;
		   Milliseconds = 0;
		}

		Timestamp()
		{
			Clear();
		}
		Timestamp( INIT_WITH INIT_WITH );
		Timestamp( const InitWithFirstDayOf& init_with );
		Timestamp( const InitWithLastDayOf& init_with );
		Timestamp( const SYSTEMTIME& src );
		Timestamp( const FILETIME& src );

		//文字列を読み取って時刻をセットします。
		//文字列はRFC3339形式です(DateやTimeと形式が違うので注意)
		//src : 読み取る文字列
		//retBias : 時差部分(不要ならnull可)
		//ret : 成功時真
		bool Set( const AString& src , TimeBias* retBias = nullptr );

		//文字列を読み取って時刻をセットします。
		//文字列はRFC3339形式です(DateやTimeと形式が違うので注意)
		//src : 読み取る文字列
		//retBias : 時差部分(不要ならnull可)
		//ret : 成功時真
		bool Set( const WString& src , TimeBias* retBias = nullptr );

		//文字列を読み取って時刻をセットします。
		//文字列はRFC3339形式です(DateやTimeと形式が違うので注意)
		//文字列を日付として読み取れない場合は、テンプレートで指定した型の例外を投げます。この型はmExceptionの派生型でなければなりません。
		// ※参考→ using err = class ErrorClass : mException{};
		//src : 読み取る文字列
		//retBias : 時差部分(不要ならnull可)
		template< class ExceptionIfFailed >
		void Set( const AString& src , TimeBias* retBias = nullptr )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src , retBias ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//文字列を読み取って時刻をセットします。
		//文字列はRFC3339形式です(DateやTimeと形式が違うので注意)
		//文字列を日付として読み取れない場合は、テンプレートで指定した型の例外を投げます。この型はmExceptionの派生型でなければなりません。
		// ※参考→ using err = class ErrorClass : mException{};
		//src : 読み取る文字列
		//retBias : 時差部分(不要ならnull可)
		template< class ExceptionIfFailed >
		void Set( const WString& src , TimeBias* retBias = nullptr )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src , retBias ) )
			{
				throw ExceptionIfFailed();
			}
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		//格納している日付が有効であるか否かを返します
		//ret : 日付が有効であれば真
		bool IsValid( void )const;

		//格納している日付の修正ユリウス通日を求めます
		//※有効な日付を格納していない場合は不正な結果になります
		//ret : 格納している日付の修正ユリウス通日
		double ToModJulian( void )const;

		//文字列化して返します
		//文字列はRFC3339形式です(DateやTimeと違い、フォーマット指定ではない)
		// bias : UTCとの時差。日本時間なら+9:00。ヌルにするとUTC(末尾がZ)になる。時差不明表記(-00:00)は未対応。
		AString ToAString( const mDateTime::TimeBias* bias = nullptr )const;

		//文字列化して返します
		//文字列はRFC3339形式です(DateやTimeと違い、フォーマット指定ではない)
		// bias : UTCとの時差。日本時間なら+9:00。ヌルにするとUTC(末尾がZ)になる。時差不明表記(-00:00)は未対応。
		WString ToWString( const mDateTime::TimeBias* bias = nullptr )const;

		//修正ユリウス通日から日付を取得して格納します
		//julian : 修正ユリウス通日
		void FromModJulian( double julian );

		//日付を取り出します
		Date GetDate( void )const;

		//時刻を取り出します
		Time GetTime( void )const;

		//SYSTEMTIME構造体として取り出します
		SYSTEMTIME ToSystemtime( void )const;

		//FILETIME構造体として取り出します
		FILETIME ToFileTime( void )const;

		//日付をセットします。時刻は変化しません。
		void Set( const Date& date );

		//時刻をセットします。日付は変化しません。
		void Set( const Time& time );

		//指定年月の初日に設定します。時刻は変化しません。
		void SetFirstDay( INT year , INT month )noexcept;

		//指定年月の最終日に設定します。時刻は変化しません。
		void SetLastDay( INT year , INT month )noexcept;

		//日付と時刻をセットします
		void Set( const Date& date , const Time& time );

		//日付と時刻をセットします
		void Set( const SYSTEMTIME& src );

		//日付と時刻をセットします
		void Set( const FILETIME& src );

		//曜日を求めます
		//・日付のみに対する曜日になります。水曜日の25時に対しては水曜日を返します。
		DAYOFWEEK DayOfWeek( void ) const noexcept;

		//日付に対する和暦を求めます
		bool GetJapaneseEra( JapaneseEra& retEra )const noexcept;

		//事業年度を求めます
		//StartMonth : 事業年度の開始月(４ならば４月から翌年３月までが１事業年度)
		INT GetBusinessYear( INT StartMonth = 4 )const;

	   //正規化します
		//・1月1日28時→1月2日4時のように補正します
		//・時刻はいかなる値が入ってても問題ありません。
		//・日付は実在しない日が入っていると不正になります。
		void Normalize( void )noexcept;

		//ミリ秒単位のUNIX時間に変換します
		uint64_t ToUnixtimeMillisecond( void )const;

		//ミリ秒単位のUNIX時間から設定します
		void FromUnixtimeMillisecond( uint64_t );

		//現在格納している時間をシステム時間として、ローカル時間に変換した結果をます
		Timestamp ToLocalTime( void )const;

		//現在格納している時間をローカル時間として、システム時間に変換した結果をます
		Timestamp ToSystemTime( void )const;

		bool operator<=( const Timestamp& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const Timestamp& src ) const
		{
		   if( Year   != src.Year   ) return Year   < src.Year  ;
		   if( Month  != src.Month  ) return Month  < src.Month ;
		   if( Day    != src.Day    ) return Day    < src.Day   ;
		   if( Hour   != src.Hour   ) return Hour   < src.Hour  ;
		   if( Minute != src.Minute ) return Minute < src.Minute;
		   if( Second != src.Second ) return Second < src.Second;
		   return Milliseconds < src.Milliseconds;
		}
		bool operator>=( const Timestamp& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const Timestamp& src ) const
		{
		   if( Year   != src.Year   ) return Year   > src.Year  ;
		   if( Month  != src.Month  ) return Month  > src.Month ;
		   if( Day    != src.Day    ) return Day    > src.Day   ;
		   if( Hour   != src.Hour   ) return Hour   > src.Hour  ;
		   if( Minute != src.Minute ) return Minute > src.Minute;
		   if( Second != src.Second ) return Second > src.Second;
		   return Milliseconds > src.Milliseconds;
		}
		bool operator==( const Timestamp& src ) const
		{
		   if( Second       != src.Second       ) return false;
		   if( Minute       != src.Minute       ) return false;
		   if( Hour         != src.Hour         ) return false;
		   if( Day          != src.Day          ) return false;
		   if( Month        != src.Month        ) return false;
		   if( Year         != src.Year         ) return false;
		   if( Milliseconds != src.Milliseconds ) return false;
		   return true;
		}
		bool operator!=( const Timestamp& src ) const
		{
			return !( this->operator==( src ) );
		}

		const Timestamp operator+( const TimeBias& v )const;
		void operator+=( const TimeBias& v );
		const Timestamp operator-( const TimeBias& v )const;
		void operator-=( const TimeBias& v );
	};
};

namespace std
{
    template<>
    class hash< mDateTime::Date >
	{
        public:
        size_t operator() ( const mDateTime::Date &src ) const
		{
			return ( src.Year << 16 ) + ( (uint8_t)src.Month << 8 ) + ( src.Day );
		}
    };

	template<>
	class hash< mDateTime::YearMonth >
	{
	public:
		size_t operator() ( const mDateTime::YearMonth &src ) const
		{
			return ( src.Year << 16 ) + ( src.Month );
		}
	};

    template<>
    class hash< mDateTime::Time >
	{
        public:
        size_t operator() ( const mDateTime::Time &src ) const
		{
			return ( src.Hour << 16 ) + ( (uint8_t)src.Minute << 8 ) + ( src.Second ) + ( src.Milliseconds );
		}
    };

    template<>
    class hash< mDateTime::Timestamp >
	{
        public:
        size_t operator() ( const mDateTime::Timestamp &src ) const
		{
			size_t d = ( src.Year << 16 ) + ( (uint8_t)src.Month << 8 ) + ( src.Day );
			size_t t = ( src.Hour << 16 ) + ( (uint8_t)src.Minute << 8 ) + ( src.Second ) + ( src.Milliseconds );
#if _WIN64
			return ( d << 32 ) + t;
#else
			return d ^ t;
#endif
		}
    };
};

#endif

