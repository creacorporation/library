//----------------------------------------------------------------------------
// 日付時刻管理
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mDateTime.h"

//メモ
//出典：http://astronomy.webcrow.jp/time/gregoriancalendar-julianday.html
//(1)その国・地域がグレゴリオ暦採用してから現在まで有効なグレゴリオ暦→ユリウス通日の変換公式
//	[365.25 * Y] + [Y / 400] - [Y / 100] + [30.59 * (M - 2)] + D + 1721088.5 + h / 24 + m / 1440 + s / 86400
//(2)紀元後のユリウス暦→ユリウス通日の変換公式
//　[365.25 * Y] + [30.59 * (M - 2)] + D + 1721086.5 + h / 24 + m / 1440 + s / 86400
//(3)紀元前のユリウス暦→ユリウス通日の変換公式
//　ただし、紀元前1年のY=1とする。
//　(※天文学では紀元前1年をY=0とする場合がある)
//　[-365.25 * Y - 0.75] + [30.59 * (M - 2)] + D + 366 + 1721086.5 + h / 24 + m / 1440 + s / 86400

void mDateTime::Date::SetCurrentLocal( void )
{
	SYSTEMTIME tm;
	GetLocalTime( &tm );

	Year = tm.wYear;
	Month = tm.wMonth;
	Day = tm.wDay;
	return;
}

void mDateTime::Date::SetCurrentSystem( void )
{
	SYSTEMTIME tm;
	GetSystemTime( &tm );

	Year = tm.wYear;
	Month = tm.wMonth;
	Day = tm.wDay;
	return;
}

void mDateTime::Time::SetCurrentLocal( void )
{
	SYSTEMTIME tm;
	GetLocalTime( &tm );

	Hour = tm.wHour;
	Minute = tm.wMinute;
	Second = tm.wSecond;
	Milliseconds = tm.wMilliseconds;
	return;
}

void mDateTime::Time::SetCurrentSystem( void )
{
	SYSTEMTIME tm;
	GetSystemTime( &tm );

	Hour = tm.wHour;
	Minute = tm.wMinute;
	Second = tm.wSecond;
	Milliseconds = tm.wMilliseconds;
	return;
}

void mDateTime::Timestamp::SetCurrentLocal( void )
{
	SYSTEMTIME tm;
	GetLocalTime( &tm );

	Year = tm.wYear;
	Month = tm.wMonth;
	Day = tm.wDay;
	Hour = tm.wHour;
	Minute = tm.wMinute;
	Second = tm.wSecond;
	Milliseconds = tm.wMilliseconds;
	return;
}

void mDateTime::Timestamp::SetCurrentSystem( void )
{
	SYSTEMTIME tm;
	GetSystemTime( &tm );

	Year = tm.wYear;
	Month = tm.wMonth;
	Day = tm.wDay;
	Hour = tm.wHour;
	Minute = tm.wMinute;
	Second = tm.wSecond;
	Milliseconds = tm.wMilliseconds;
	return;
}


bool mDateTime::Date::IsValid( void )const
{
	if( Year == 0 )
	{
		return false;
	}
	if( Month < 1 || 12 < Month )
	{
		return false;
	}
	if( Day < 1 || GetLastDay( Year , Month ) < Day )
	{
		return false;
	}
	return true;
}

INT mDateTime::GetLastDay( INT Year , INT Month )
{
	static const INT LastDayTable[ 12 ] =
	{
	//   1  2  3  4  5  6  7  8  9 10 11 12
		31,28,31,30,31,30,31,31,30,31,30,31
	};

	if( Month < 1 || 12 < Month )
	{
		return -1;
	}
	if( Month == 2 && IsLeapYear( Year ) )
	{
		return 29;
	}
	return LastDayTable[ Month - 1 ];
}

bool mDateTime::IsLeapYear( INT Year )
{
	if( Year % 400 == 0 )
	{
		return true;
	}
	if( Year % 100 == 0 )
	{
		return false;
	}
	if( Year % 4 == 0 )
	{
		return true;
	}
	return false;
}

double mDateTime::Date::ToModJulian( void )const
{
	int year  = ( Month < 3 ) ? ( Year  -  1 ) : ( Year  );
	int month = ( Month < 3 ) ? ( Month + 12 ) : ( Month );

	long double result 
		= floor( 365.25 * year )
		+ floor( year / 400 )
		- floor( year / 100 )
		+ floor( 30.59 * ( month - 2 ) )
		+ Day - 678912;
	return result;
}

void mDateTime::Date::FromModJulian( double julian )
{
	INT n = INT( floor( julian + 678881 ) );
	INT a = ( 4 * n ) + 3 + 4 * INT( floor( 0.75 * ( floor ( ( 4 * ( n + 1 ) ) / 146097 + 1 ) ) ) );
	INT b = 5 * INT( floor( ( a % 1461 ) / 4 ) ) + 2;
	Year = INT( floor( a / 1461 ) );
	Month = INT( floor( b / 153 ) ) + 3;
	Day = INT( floor( ( b % 153 ) / 5 ) ) + 1;

	if( 12 < Month )
	{
		Month -= 12;
		Year++;
	}
	return;
}

mDateTime::DAYOFWEEK mDateTime::Date::DayOfWeek( void ) const noexcept
{
	int year  = ( Month < 3 ) ? ( Year  -  1 ) : ( Year  );
	int month = ( Month < 3 ) ? ( Month + 12 ) : ( Month );

	return DAYOFWEEK( ( year + year / 4 - year / 100 + year / 400 + ( 13 * month + 8 ) / 5 + Day ) % 7 );

}

void mDateTime::AdjustYearMonth( INT& ioYear , INT& ioMonth )
{

	if( ioMonth <= 0 )
	{
		ioMonth = ( -ioMonth );
		ioYear -= ( ( ioMonth / 12 ) + 1 );
		ioMonth = ( 12 - ( ioMonth % 12 ) );
	}
	else if( 12 < ioMonth )
	{
		ioMonth--;
		ioYear += ( ioMonth / 12 );
		ioMonth = ( ioMonth % 12 ) + 1;
	}
	else
	{
		;
	}
}

//指定年月の初日に設定します
void mDateTime::Date::SetFirstDay( INT year , INT month )noexcept
{
	AdjustYearMonth( year , month );
	Year = year;
	Month = month;
	Day = 1;
}

void mDateTime::Date::SetFirstDay( const YearMonth& ym )noexcept
{
	SetFirstDay( ym.Year , ym.Month );
}

//指定年月の最終日に設定します
void mDateTime::Date::SetLastDay( INT year , INT month )noexcept
{
	AdjustYearMonth( year , month );
	Year = year;
	Month = month;
	Day = GetLastDay( year , month );
}

void mDateTime::Date::SetLastDay( const YearMonth& ym )noexcept
{
	SetFirstDay( ym.Year , ym.Month );
}

void mDateTime::Date::AddDay( int offset )
{
	//オフセットの絶対値が29未満であれば、2ヶ月またぐ可能性はないから
	//簡易な計算で処理する。
	if( offset == 0 )
	{
	}
	else if( ( 0 < offset ) && ( offset < 29 ) )
	{
		Day += offset;
		int LastDay = GetLastDay( Year , Month );
		if( LastDay < Day )
		{
			Day -= LastDay;
			Month++;
			if( 13 == Month )
			{
				Month = 1;
				Year++;
			}
		}
	}
	else if( ( -29 < offset ) && ( offset < 0 ) )
	{
		Day += offset;
		if( Day < 1 )
		{
			Month--;
			if( Month == 0 )
			{
				Year--;
				Month = 12;
			}
			Day += GetLastDay( Year , Month );
		}
	}
	else
	{
		double julian = ToModJulian();
		julian += offset;
		FromModJulian( julian );
	}
}

void mDateTime::Date::AddMonth( int offset )
{
	Month += offset;
	AdjustYearMonth( Year , Month );
	if( Day < 1 )
	{
		Day = 1;
	}
	else
	{
		INT LastDay = GetLastDay( Year , Month );
		if( LastDay < Day )
		{
			Day = LastDay;
		}
	}
}

static const wchar_t* JapaneseEraWordTable[] =
{
	L"",		//JPERA_UNKNOWN,
	L"令和",	//JPERA_REIWA,
	L"平成",	//JPERA_HEISEI,
	L"昭和",	//JPERA_SHOWA,
	L"大正",	//JPERA_TAISYO,
	L"明治",	//JPERA_MEIJI,
};


//日付に対する和暦を求めます
bool mDateTime::Date::GetJapaneseEra( JapaneseEra& retEra )const noexcept
{
	mDateTime::Date end_day;

	end_day.Set( 2019 , 4 , 30 );
	if( end_day < *this )
	{
		retEra.Id = JAPANESE_ERA::JPERA_REIWA;
		retEra.Word = JapaneseEraWordTable[ retEra.Id ];
		retEra.Year = Year - 2018;
		return true;
	}
	end_day.Set( 1989 , 1 , 7 );
	if( end_day < *this )
	{
		retEra.Id = JAPANESE_ERA::JPERA_HEISEI;
		retEra.Word = JapaneseEraWordTable[ retEra.Id ];
		retEra.Year = Year - 1988;
		return true;
	}
	end_day.Set( 1926 , 12 , 24 );
	if( end_day < *this )
	{
		retEra.Id = JAPANESE_ERA::JPERA_SHOWA;
		retEra.Word = JapaneseEraWordTable[ retEra.Id ];
		retEra.Year = Year - 1925;
		return true;
	}
	end_day.Set( 1912 , 7 , 29 );
	if( end_day < *this )
	{
		retEra.Id = JAPANESE_ERA::JPERA_TAISYO;
		retEra.Word = JapaneseEraWordTable[ retEra.Id ];
		retEra.Year = Year - 1911;
		return true;
	}
	end_day.Set( 1868 , 1 , 24 );
	if( end_day < *this )
	{
		retEra.Id = JAPANESE_ERA::JPERA_MEIJI;
		retEra.Word = JapaneseEraWordTable[ retEra.Id ];
		retEra.Year = Year - 1867;
		return true;
	}

	retEra.Id = JAPANESE_ERA::JPERA_UNKNOWN;
	retEra.Word = JapaneseEraWordTable[ retEra.Id ];
	retEra.Year = Year;
	return false;
}


//格納している時刻が有効であるか否かを返します
bool mDateTime::Time::IsValid( void )const
{
	if( Hour < 0 || 23 < Hour )
	{
		return false;
	}
	if( Minute < 0 || 59 < Minute )
	{
		return false;
	}
	if( Second < 0 || 59 < Second )
	{
		return false;
	}
	if( Milliseconds < 0 || 999 < Milliseconds )
	{
		return false;
	}
	return true;
}

//格納している時刻に対して、1日を1.0とした比率を求めます
double mDateTime::Time::ToValue( void )const
{
	return ( ( 3600000 * Hour ) + ( 60000 * Minute ) + ( Second * 1000 ) + ( Milliseconds ) ) / 86400000.5;
}

double mDateTime::Time::ToHour( void )const
{
	return ( Hour ) + ( Minute / 60.0 ) + ( Second / 3600.0 ) + ( Milliseconds / 3600000.0 );
}

double mDateTime::Time::ToMinute( void )const
{
	return ( Hour * 60 ) + ( Minute ) + ( Second / 60.0 ) + ( Milliseconds / 60000.0 );
}

double mDateTime::Time::ToSecond( void )const
{
	return ( Hour * 3600 ) + ( Minute * 60 ) + ( Second ) + ( Milliseconds / 1000.0 );
}

//1日を1.0とした比率から時刻を取得して格納します
void mDateTime::Time::FromValue( double value )
{
	const double factor = ( 1 / 86400000.5 ); 
	bool is_minus = ( value < 0 );

	if( !is_minus )
	{
		value = fmod( value , 1.0 );
	}
	else
	{
		value = fmod( -value , 1.0 );
	}

	INT mil = INT( value / factor );
	Hour = mil / 3600000;
	mil -= ( Hour * 3600000 );
	Minute = mil / 60000;
	mil -= ( Minute * 60000 );
	Second = mil / 1000;
	mil -= ( Second * 1000 );
	Milliseconds = mil;

	if( is_minus )
	{
		Hour *= -1;
		Minute *= -1;
		Second *= -1;
		Milliseconds *= -1;
	}
	return;
}

void mDateTime::Time::Normalize( void )noexcept
{
	//※高速化しようと場合分けしてみたけど、結局割り算がいっぱい出てきたので、簡単に解決することにした
	FromValue( ToValue() );
}

const mDateTime::Time mDateTime::Time::operator+( const Time& v )const
{
	mDateTime::Time t( *this );
	t += v;
	return t;
}

void mDateTime::Time::operator+=( const Time& v )
{
	Hour += v.Hour;
	Minute += v.Minute;
	Second += v.Second;
	Milliseconds += v.Milliseconds;
	Normalize();
}

const mDateTime::Time mDateTime::Time::operator-( const Time& v )const
{
	mDateTime::Time t( *this );
	t -= v;
	return t;
}

void mDateTime::Time::operator-=( const Time& v )
{
	Hour -= v.Hour;
	Minute -= v.Minute;
	Second -= v.Second;
	Milliseconds -= v.Milliseconds;
	Normalize();
}

const mDateTime::Time mDateTime::Time::operator*( double v )const
{
	return mDateTime::Time( ToValue() * v );
}

void mDateTime::Time::operator*=( double v )
{
	FromValue( ToValue() * v );
}

const mDateTime::Time mDateTime::Time::operator/( double v )const
{
	return mDateTime::Time( ToValue() / v );
}

double mDateTime::Time::operator/( const Time& divider )const
{
	return ToValue() / divider.ToValue();
}

void mDateTime::Time::operator/=( double v )
{
	FromValue( ToValue() / v );
}

const mDateTime::Time mDateTime::Time::operator%( double v )const
{
	return mDateTime::Time( ::fmod( ToValue() , v ) );
}

void mDateTime::Time::operator%=( double v )
{
	FromValue( ::fmod( ToValue() , v ) );
}

//格納している日付の修正ユリウス通日を求めます
double mDateTime::Timestamp::ToModJulian( void )const
{
	Date date = GetDate();
	Time time = GetTime();

	return date.ToModJulian() + time.ToValue();
}

void mDateTime::Timestamp::FromModJulian( double julian )
{
	double day = double( floorl( julian ) );
	double tim = double( fmodl( julian , 1.0 ) );

	Date date;
	date.FromModJulian( day );
	Set( date );

	Time time;
	time.FromValue( tim );
	Set( time );
	return;
}

//格納している日付が有効であるか否かを返します
bool mDateTime::Timestamp::IsValid( void )const
{
	Date date = GetDate();
	if( !date.IsValid() )
	{
		return false;
	}

	Time time = GetTime();
	return time.IsValid();
}

//日付を取り出します
mDateTime::Date mDateTime::Timestamp::GetDate( void )const
{
	Date date;
	date.Year = Year;
	date.Month = Month;
	date.Day = Day;
	return date;
}

//時刻を取り出します
mDateTime::Time mDateTime::Timestamp::GetTime( void )const
{
	Time time;
	time.Hour = Hour;
	time.Minute = Minute;
	time.Second = Second;
	time.Milliseconds = Milliseconds;
	return time;
}

//日付をセットします。時刻は変化しません。
void mDateTime::Timestamp::Set( const Date& date )
{
	Year = date.Year;
	Month = date.Month;
	Day = date.Day;
	return;
}

//時刻をセットします。日付は変化しません。
void mDateTime::Timestamp::Set( const Time& time )
{
	Hour = time.Hour;
	Minute = time.Minute;
	Second = time.Second;
	Milliseconds = time.Milliseconds;
	return;
}

//日付と時刻をセットします
void mDateTime::Timestamp::Set( const Date& date , const Time& time )
{
	Set( date );
	Set( time );
	return;
}

//指定年月の初日に設定します
void mDateTime::Timestamp::SetFirstDay( INT year , INT month )noexcept
{
	Year = year;
	Month = month;
	Day = 1;
}

//指定年月の最終日に設定します
void mDateTime::Timestamp::SetLastDay( INT year , INT month )noexcept
{
	Year = year;
	Month = month;
	Day = GetLastDay( year , month );
}

mDateTime::DAYOFWEEK mDateTime::Timestamp::DayOfWeek( void ) const noexcept
{
	return GetDate().DayOfWeek();
}

//日付に対する和暦を求めます
bool mDateTime::Timestamp::GetJapaneseEra( JapaneseEra& retEra )const noexcept
{
	return GetDate().GetJapaneseEra( retEra );
}

void mDateTime::Timestamp::Normalize( void )noexcept
{
	FromModJulian( ToModJulian() );
}

mDateTime::Date::Date( INIT_WITH init_by )
{
	switch( init_by )
	{
	case INIT_WITH::CURRENT_LOCALTIME:
		SetCurrentLocal();
		break;
	case INIT_WITH::CURRENT_SYSTEMTIME:
		SetCurrentSystem();
		break;
	default:
		Clear();
		break;
	}
	return;
}

mDateTime::Time::Time( INIT_WITH init_by )
{
	switch( init_by )
	{
	case INIT_WITH::CURRENT_LOCALTIME:
		SetCurrentLocal();
		break;
	case INIT_WITH::CURRENT_SYSTEMTIME:
		SetCurrentSystem();
		break;
	default:
		Clear();
		break;
	}
	return;
}

mDateTime::Timestamp::Timestamp( INIT_WITH init_by )
{
	switch( init_by )
	{
	case INIT_WITH::CURRENT_LOCALTIME:
		SetCurrentLocal();
		break;
	case INIT_WITH::CURRENT_SYSTEMTIME:
		SetCurrentSystem();
		break;
	default:
		Clear();
		break;
	}
	return;
}

mDateTime::Date::Date( const InitWithFirstDayOf& init_with )
{
	Clear();
	SetFirstDay( init_with.year , init_with.month );
}

mDateTime::Date::Date( const InitWithLastDayOf& init_with )
{
	Clear();
	SetLastDay( init_with.year , init_with.month );
}

mDateTime::Timestamp::Timestamp( const InitWithFirstDayOf& init_with )
{
	Clear();
	SetFirstDay( init_with.year , init_with.month );
}

mDateTime::Timestamp::Timestamp( const InitWithLastDayOf& init_with )
{
	Clear();
	SetLastDay( init_with.year , init_with.month );
}

mDateTime::Date::Date( const AString& src )
{
	if( !Set( src ) )
	{
		Clear();
	}
}

mDateTime::Date::Date( const WString& src )
{
	if( !Set( src ) )
	{
		Clear();
	}
}

mDateTime::Time::Time( const AString& src )
{
	if( !Set( src ) )
	{
		Clear();
	}
}

mDateTime::Time::Time( const WString& src )
{
	if( !Set( src ) )
	{
		Clear();
	}
}

mDateTime::Time::Time( double src )
{
	FromValue( src );
}

bool mDateTime::Date::Set( const AString& src )
{
	if( sscanf( src.c_str() , "%d/%d/%d" , &Year , &Month , &Day ) == 3 )
	{
		return true;
	}
	if( sscanf( src.c_str() , "%d-%d-%d" , &Year , &Month , &Day ) == 3 )
	{
		return true;
	}
	return false;
}

bool mDateTime::Date::Set( const WString& src )
{
	if( wchar_sscanf( src.c_str() , L"%d/%d/%d" , &Year , &Month , &Day ) == 3 )
	{
		return true;
	}
	if( wchar_sscanf( src.c_str() , L"%d-%d-%d" , &Year , &Month , &Day ) == 3 )
	{
		return true;
	}
	return false;
}

bool mDateTime::Time::Set( const AString& src )
{
	Clear();
	if( sscanf( src.c_str() , "%d:%d:%d.%d" , &Hour , &Minute , &Second , &Milliseconds ) <= 0 )
	{
		return true;
	}
	return false;
}

bool mDateTime::Time::Set( const WString& src )
{
	Clear();
	if( wchar_sscanf( src.c_str() , L"%d:%d:%d.%d" , &Hour , &Minute , &Second , &Milliseconds ) <= 0 )
	{
		return true;
	}
	return false;
}


void mDateTime::YearMonth::SetCurrentLocal( void )
{
	SYSTEMTIME tm;
	GetLocalTime( &tm );

	Year = tm.wYear;
	Month = tm.wMonth;
	return;
}

void mDateTime::YearMonth::SetCurrentSystem( void )
{
	SYSTEMTIME tm;
	GetSystemTime( &tm );

	Year = tm.wYear;
	Month = tm.wMonth;
}

mDateTime::YearMonth::YearMonth( INIT_WITH init_with )
{
	switch( init_with )
	{
	case INIT_WITH::CURRENT_LOCALTIME:
		SetCurrentLocal();
		break;
	case INIT_WITH::CURRENT_SYSTEMTIME:
		SetCurrentSystem();
		break;
	default:
		Clear();
		break;
	}
	return;
}

mDateTime::YearMonth::YearMonth( const AString& src )
{
	Set( src );
}

mDateTime::YearMonth::YearMonth( const WString& src )
{
	Set( src );
}

bool mDateTime::YearMonth::Set( const AString& src )
{
	if( sscanf( src.c_str() , "%d/%d" , &Year , &Month ) == 2 )
	{
		return true;
	}
	if( sscanf( src.c_str() , "%d-%d" , &Year , &Month ) == 2 )
	{
		return true;
	}
	return false;
}

bool mDateTime::YearMonth::Set( const WString& src )
{
	if( wchar_sscanf( src.c_str() , L"%d/%d" , &Year , &Month ) == 2 )
	{
		return true;
	}
	if( wchar_sscanf( src.c_str() , L"%d-%d" , &Year , &Month ) == 2 )
	{
		return true;
	}
	return false;
}

bool mDateTime::YearMonth::IsValid( void )const
{
	if( Year == 0 )
	{
		return false;
	}
	if( Month < 1 || 12 < Month )
	{
		return false;
	}
	return true;
}

bool mDateTime::YearMonth::GetJapaneseEra( JapaneseEra& retEra , int day )const noexcept
{
	mDateTime::Date t( Year , Month , day );
	return t.GetJapaneseEra( retEra );
}

void mDateTime::YearMonth::AddMonth( int offset )
{
	Month += offset;
	AdjustYearMonth( Year , Month );
}

void mDateTime::YearMonth::Normalize( void )noexcept
{
	AdjustYearMonth( Year , Month );
}

INT mDateTime::Date::GetBusinessYear( INT StartMonth )const
{
	return ( Month < StartMonth ) ? ( Year - 1 ) : ( Year );
}

INT mDateTime::YearMonth::GetBusinessYear( INT StartMonth )const
{
	return ( Month < StartMonth ) ? ( Year - 1 ) : ( Year );
}

INT mDateTime::Timestamp::GetBusinessYear( INT StartMonth )const
{
	return ( Month < StartMonth ) ? ( Year - 1 ) : ( Year );
}
