//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MODBCPARAMS_CPP_COMPILING
#include "mOdbcParams.h"
#include "../General/mErrorLogger.h"

mOdbcParamsEntry::mOdbcParamsEntry()
{
	MyParameterType = mOdbc::ParameterType::Int8;
	MyIsNull = true;
	return;
}

mOdbcParamsEntry::mOdbcParamsEntry( const mOdbcParamsEntry& src ) : mOdbcParamsEntry()
{
	*this = src;
	return;
}

mOdbcParamsEntry::~mOdbcParamsEntry()
{
	SetNull();
	return;
}

const mOdbcParamsEntry& mOdbcParamsEntry::operator=( const mOdbcParamsEntry& src )
{
	MyParameterType = src.MyParameterType;

	if( !src.IsNull() )
	{
		switch( MyParameterType )
		{
		case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
			MyDataEntry.deInt64 = src.MyDataEntry.deInt64;
			break;
		case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
			MyDataEntry.deInt32 = src.MyDataEntry.deInt32;
			break;
		case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
			MyDataEntry.deInt16 = src.MyDataEntry.deInt16;
			break;
		case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
			MyDataEntry.deInt8 = src.MyDataEntry.deInt8;
			break;
		case mOdbc::ParameterType::Float:		//単精度浮動小数点
			MyDataEntry.deFloat = src.MyDataEntry.deFloat;
			break;
		case mOdbc::ParameterType::Double:		//倍精度浮動小数点
			MyDataEntry.deDouble = src.MyDataEntry.deDouble;
			break;
		case mOdbc::ParameterType::AString:		//ANSI文字列
			Set( src.MyDataEntry.deAString );
			break;
		case mOdbc::ParameterType::WString:		//UNICODE文字列
			Set( src.MyDataEntry.deWString );
			break;
		case mOdbc::ParameterType::Binary:			//バイナリ
			Set( src.MyDataEntry.deBinary );
			break;
		case mOdbc::ParameterType::Date:		//日付
			MyDataEntry.deDate = src.MyDataEntry.deDate;
			break;
		case mOdbc::ParameterType::Time:		//時刻
			MyDataEntry.deTime = src.MyDataEntry.deTime;
			break;
		case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
			MyDataEntry.deTimestamp = src.MyDataEntry.deTimestamp;
			break;
		default:
			RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		}
		MyIsNull = false;
	}
	else
	{
		MyIsNull = true;
	}
	return *this;
}

void mOdbcParamsEntry::SetNull( void )
{
	if( IsNull() )
	{
		return;
	}
	MyIsNull = true;

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
		break;
	case mOdbc::ParameterType::AString:		//ANSI文字列
		MyDataEntry.deAString.~AString();
		break;
	case mOdbc::ParameterType::WString:		//UNICODE文字列
		MyDataEntry.deWString.~WString();
		break;
	case mOdbc::ParameterType::Binary:		//バイナリ
		MyDataEntry.deBinary.~mBinary();
		break;
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
	}
	return;

}

bool mOdbcParamsEntry::IsNull( void ) const
{
	return MyIsNull;
}

mOdbc::ParameterType mOdbcParamsEntry::GetParameterType( void )const
{
	return MyParameterType;
}

//--------------------------------------------------------
// Setter
//--------------------------------------------------------

void mOdbcParamsEntry::Set( int64_t dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Int64;
	MyDataEntry.deInt64 = dt;
	return;
}

void mOdbcParamsEntry::Set( int32_t dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Int32;
	MyDataEntry.deInt32 = dt;
	return;
}

void mOdbcParamsEntry::Set( int16_t dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Int16;
	MyDataEntry.deInt16 = dt;
	return;
}

void mOdbcParamsEntry::Set( int8_t dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Int8;
	MyDataEntry.deInt8 = dt;
	return;
}

void mOdbcParamsEntry::Set( float dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Float;
	MyDataEntry.deFloat = dt;
	return;
}

void mOdbcParamsEntry::Set( double dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Double;
	MyDataEntry.deDouble = dt;
	return;
}

void mOdbcParamsEntry::Set( const AString& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::AString;
	mPlacementNew( &MyDataEntry.deAString ) AString( dt );
	return;
}

void mOdbcParamsEntry::Set( const WString& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::WString;
	mPlacementNew( &MyDataEntry.deWString ) WString( dt );
	return;
}

void mOdbcParamsEntry::Set( const mBinary& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Binary;
	mPlacementNew( &MyDataEntry.deBinary ) mBinary( dt );
	return;
}

//[Setter]日付をセットします
void mOdbcParamsEntry::Set( const mDateTime::Date& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Date;
	MyDataEntry.deDate.year = static_cast<SQLSMALLINT>( dt.Year );
	MyDataEntry.deDate.month = static_cast<SQLUSMALLINT>( dt.Month );
	MyDataEntry.deDate.day = static_cast<SQLUSMALLINT>( dt.Day );
	return;
}

//[Setter]時刻をセットします
void mOdbcParamsEntry::Set( const mDateTime::Time& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Time;
	MyDataEntry.deTime.hour = static_cast<SQLUSMALLINT>( dt.Hour );
	MyDataEntry.deTime.minute = static_cast<SQLUSMALLINT>( dt.Minute );
	MyDataEntry.deTime.second = static_cast<SQLUSMALLINT>( dt.Second );
}

//[Setter]タイムスタンプをセットします
void mOdbcParamsEntry::Set( const mDateTime::Timestamp& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Timestamp;
	MyDataEntry.deTimestamp.year = static_cast<SQLSMALLINT>( dt.Year );
	MyDataEntry.deTimestamp.month = static_cast<SQLUSMALLINT>( dt.Month );
	MyDataEntry.deTimestamp.day = static_cast<SQLUSMALLINT>( dt.Day );
	MyDataEntry.deTimestamp.hour = static_cast<SQLUSMALLINT>( dt.Hour );
	MyDataEntry.deTimestamp.minute = static_cast<SQLUSMALLINT>( dt.Minute );
	MyDataEntry.deTimestamp.second = static_cast<SQLUSMALLINT>( dt.Second );
	MyDataEntry.deTimestamp.fraction = dt.Milliseconds * 1000000;
}

//[Setter]日付をセットします
void mOdbcParamsEntry::Set( const SQL_DATE_STRUCT& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Date;
	MyDataEntry.deDate = dt;
}

//[Setter]時刻をセットします
void mOdbcParamsEntry::Set( const SQL_TIME_STRUCT& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Time;
	MyDataEntry.deTime = dt;
}

//[Setter]タイムスタンプをセットします
void mOdbcParamsEntry::Set( const SQL_TIMESTAMP_STRUCT& dt )
{
	SetNull();

	MyIsNull = false;
	MyParameterType = mOdbc::ParameterType::Timestamp;
	MyDataEntry.deTimestamp = dt;
}

//--------------------------------------------------------
// Getter
//--------------------------------------------------------

void mOdbcParamsEntry::Get( bool& retresult , int64_t& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = 0;
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt64;
		break;
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt32;
		break;
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt16;
		break;
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt8;
		break;
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = 0;
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = 0;
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , int32_t& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = 0;
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt32;
		break;
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt16;
		break;
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt8;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = 0;
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = 0;
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , int16_t& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = 0;
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt16;
		break;
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt8;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = 0;
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = 0;
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , int8_t& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = 0;
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
		retDt = MyDataEntry.deInt8;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = 0;
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = 0;
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , uint64_t& retDt )const
{
	Get( retresult , (int64_t&)retDt );
	if( retresult )
	{
		retresult = ( 0 <= (int64_t)retDt );
	}
}

void mOdbcParamsEntry::Get( bool& retresult , uint32_t& retDt )const
{
	Get( retresult , (int32_t&)retDt );
	if( retresult )
	{
		retresult = ( 0 <= (int32_t)retDt );
	}
}

void mOdbcParamsEntry::Get( bool& retresult , uint16_t& retDt )const
{
	Get( retresult , (int16_t&)retDt );
	if( retresult )
	{
		retresult = ( 0 <= (int16_t)retDt );
	}
}

void mOdbcParamsEntry::Get( bool& retresult , uint8_t& retDt )const
{
	Get( retresult , (int8_t&)retDt );
	if( retresult )
	{
		retresult = ( 0 <= (int8_t)retDt );
	}
}

void mOdbcParamsEntry::Get( bool& retresult , float& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = 0.0f;
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
		retDt = MyDataEntry.deFloat;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = 0.0f;
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = 0.0f;
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , double& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = 0.0l;
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
		retDt = MyDataEntry.deFloat;
		break;
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
		retDt = MyDataEntry.deDouble;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = 0.0l;
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = 0.0l;
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , AString& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = "";
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::AString:		//ANSI文字列
		retDt = MyDataEntry.deAString;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = "";
		return;
	case mOdbc::ParameterType::Date:		//日付
		sprintf( retDt , "%04d-%02d-%02d" ,
			MyDataEntry.deDate.year ,
			MyDataEntry.deDate.month ,
			MyDataEntry.deDate.day );
		break;
	case mOdbc::ParameterType::Time:		//時刻
		sprintf( retDt , "%02d:%02d:%02d" ,
			MyDataEntry.deTime.hour ,
			MyDataEntry.deTime.minute ,
			MyDataEntry.deTime.second );
		break;
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		sprintf( retDt , "%04d-%02d-%02d %02d:%02d:%02d.%d" ,
			MyDataEntry.deTimestamp.year ,
			MyDataEntry.deTimestamp.month ,
			MyDataEntry.deTimestamp.day ,
			MyDataEntry.deTimestamp.hour ,
			MyDataEntry.deTimestamp.minute ,
			MyDataEntry.deTimestamp.second ,
			MyDataEntry.deTimestamp.fraction );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = "";
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , WString& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt = L"";
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::WString:		//UNICODE文字列
		retDt = MyDataEntry.deWString;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::Binary:		//バイナリ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt = L"";
		return;
	case mOdbc::ParameterType::Date:		//日付
		sprintf( retDt , L"%04d-%02d-%02d" ,
			MyDataEntry.deDate.year ,
			MyDataEntry.deDate.month ,
			MyDataEntry.deDate.day );
		break;
	case mOdbc::ParameterType::Time:		//時刻
		sprintf( retDt , L"%02d:%02d:%02d" ,
			MyDataEntry.deTime.hour ,
			MyDataEntry.deTime.minute ,
			MyDataEntry.deTime.second );
		break;
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		sprintf( retDt , L"%04d-%02d-%02d %02d:%02d:%02d.%d" ,
			MyDataEntry.deTimestamp.year ,
			MyDataEntry.deTimestamp.month ,
			MyDataEntry.deTimestamp.day ,
			MyDataEntry.deTimestamp.hour ,
			MyDataEntry.deTimestamp.minute ,
			MyDataEntry.deTimestamp.second ,
			MyDataEntry.deTimestamp.fraction );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt = L"";
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , mBinary& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt.clear();
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::Binary:		//バイナリ
		retDt = MyDataEntry.deBinary;
		break;
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::AString:		//ANSI文字列
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	case mOdbc::ParameterType::Date:		//日付
	case mOdbc::ParameterType::Time:		//時刻
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt.clear();
		return;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt.clear();
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , mDateTime::Date& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt.Clear();
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	{
		if( wchar_sscanf( MyDataEntry.deWString.c_str() , L"%04d-%02d-%02d" , &retDt.Year , &retDt.Month , &retDt.Day ) != 3 )
		{
			RaiseError( g_ErrorLogger , 0 , L"日付のフォーマットに変換できません" , MyDataEntry.deWString );
			retDt.Clear();
			return;
		}
		break;
	}
	case mOdbc::ParameterType::AString:		//ANSI文字列
	{
		if( sscanf( MyDataEntry.deAString.c_str() , "%04d-%02d-%02d" , &retDt.Year , &retDt.Month , &retDt.Day ) != 3 )
		{
			RaiseError( g_ErrorLogger , 0 , L"日付のフォーマットに変換できません" , MyDataEntry.deWString );
			retDt.Clear();
			return;
		}
		break;
	}
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Time:		//時刻
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt.Clear();
		return;
	case mOdbc::ParameterType::Date:		//日付
		retDt.Year = MyDataEntry.deDate.year;
		retDt.Month = MyDataEntry.deDate.month;
		retDt.Day = MyDataEntry.deDate.day;
		break;
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		retDt.Year = MyDataEntry.deTimestamp.year;
		retDt.Month = MyDataEntry.deTimestamp.month;
		retDt.Day = MyDataEntry.deTimestamp.day;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt.Clear();
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , mDateTime::Time& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt.Clear();
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	{
		if( wchar_sscanf( MyDataEntry.deWString.c_str() , L"%02d:%02d:%02d.%03d" , &retDt.Hour , &retDt.Minute , &retDt.Second , &retDt.Milliseconds ) != 4 )
		{
			RaiseError( g_ErrorLogger , 0 , L"時刻のフォーマットに変換できません" , MyDataEntry.deWString );
			retDt.Clear();
			return;
		}
		break;
	}
	case mOdbc::ParameterType::AString:		//ANSI文字列
	{
		if( sscanf( MyDataEntry.deAString.c_str() , "%02d:%02d:%02d.%03d" , &retDt.Hour , &retDt.Minute , &retDt.Second , &retDt.Milliseconds ) != 4 )
		{
			RaiseError( g_ErrorLogger , 0 , L"時刻のフォーマットに変換できません" , MyDataEntry.deWString );
			retDt.Clear();
			return;
		}
		break;
	}
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::Binary:		//バイナリ
	case mOdbc::ParameterType::Date:		//日付
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt.Clear();
		return;
	case mOdbc::ParameterType::Time:		//時刻
		retDt.Hour = MyDataEntry.deTime.hour;
		retDt.Minute = MyDataEntry.deTime.minute;
		retDt.Second = MyDataEntry.deTime.second;
		retDt.Milliseconds = 0;
		break;
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		retDt.Hour = MyDataEntry.deTimestamp.hour;
		retDt.Minute = MyDataEntry.deTimestamp.minute;
		retDt.Second = MyDataEntry.deTimestamp.second;
		retDt.Milliseconds = MyDataEntry.deTimestamp.fraction / 1000000;
	break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt.Clear();
		return;
	}
	retresult = true;
	return;
}

void mOdbcParamsEntry::Get( bool& retresult , mDateTime::Timestamp& retDt )const
{
	retresult = false;
	if( IsNull() )
	{
		retDt.Clear();
		return;
	}

	switch( MyParameterType )
	{
	case mOdbc::ParameterType::WString:		//UNICODE文字列
	{
		if( wchar_sscanf( MyDataEntry.deWString.c_str() , L"%04d-%02d-%02d %02d:%02d:%02d.%03d" ,
			&retDt.Year , &retDt.Month , &retDt.Day , &retDt.Hour , &retDt.Minute , &retDt.Second , &retDt.Milliseconds ) != 7 )
		{
			RaiseError( g_ErrorLogger , 0 , L"タイムスタンプのフォーマットに変換できません" , MyDataEntry.deWString );
			retDt.Clear();
			return;
		}
		break;
	}
	case mOdbc::ParameterType::AString:		//ANSI文字列
	{
		if( sscanf( MyDataEntry.deAString.c_str() , "%04d-%02d-%02d %02d:%02d:%02d.%03d" ,
			&retDt.Year , &retDt.Month , &retDt.Day , &retDt.Hour , &retDt.Minute , &retDt.Second , &retDt.Milliseconds ) != 7 )
		{
			RaiseError( g_ErrorLogger , 0 , L"時刻のフォーマットに変換できません" , MyDataEntry.deWString );
			retDt.Clear();
			return;
		}
		break;
	}
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
	case mOdbc::ParameterType::Binary:		//バイナリ
		RaiseError( g_ErrorLogger , 0 , L"指定した型には変換できません" , MyParameterType );
		retDt.Clear();
		return;
	case mOdbc::ParameterType::Date:		//日付
		retDt.Clear();
		retDt.Year = MyDataEntry.deDate.year;
		retDt.Month = MyDataEntry.deDate.month;
		retDt.Day = MyDataEntry.deDate.day;
		break;
	case mOdbc::ParameterType::Time:		//時刻
		retDt.Clear();
		retDt.Hour = MyDataEntry.deTime.hour;
		retDt.Minute = MyDataEntry.deTime.minute;
		retDt.Second = MyDataEntry.deTime.second;
		retDt.Milliseconds = 0;
		break;
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		retDt.Year = MyDataEntry.deTimestamp.year;
		retDt.Month = MyDataEntry.deTimestamp.month;
		retDt.Day = MyDataEntry.deTimestamp.day;
		retDt.Hour = MyDataEntry.deTimestamp.hour;
		retDt.Minute = MyDataEntry.deTimestamp.minute;
		retDt.Second = MyDataEntry.deTimestamp.second;
		retDt.Milliseconds = MyDataEntry.deTimestamp.fraction / 1000000;
	break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" , MyParameterType );
		retDt.Clear();
		return;
	}
	retresult = true;
	return;
}