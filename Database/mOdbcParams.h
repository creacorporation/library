//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCPARAMS_H_INCLUDED
#define MODBCPARAMS_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"
#include "../General/mBinary.h"
#include "../General/mDateTime.h"
#include "../General/mException.h"

#include "mOdbc.h"

#include <vector>
#include <unordered_map>
#include <typeinfo>

#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")

class mOdbcParamsEntry
{
public:

	mOdbcParamsEntry();
	virtual ~mOdbcParamsEntry();
	mOdbcParamsEntry( const mOdbcParamsEntry& src );
	const mOdbcParamsEntry& operator=( const mOdbcParamsEntry& src );

	//このパラメーターがヌルであれば真
	bool IsNull( void ) const;

	//このパラメーターをヌルにする
	void SetNull( void );

	//このパラメーターの種類を得る
	mOdbc::ParameterType GetParameterType( void )const;

	//[Setter]64ビット値をセットします
	void Set( int64_t dt );
	//[Setter]32ビット値をセットします
	void Set( int32_t dt );
	//[Setter]16ビット値をセットします
	void Set( int16_t dt );
	//[Setter]8ビット値をセットします
	void Set( int8_t dt );
	//[Setter]単精度浮動小数点値をセットします
	void Set( float dt );
	//[Setter]倍精度浮動小数点値をセットします
	void Set( double dt );
	//[Setter]ANSI/SJIS文字列をセットします
	void Set( const AString& dt );
	//[Setter]Unicode文字列をセットします
	void Set( const WString& dt );
	//[Setter]バイナリ列をセットします
	void Set( const mBinary& dt );
	//[Setter]日付をセットします
	void Set( const mDateTime::Date& dt );
	//[Setter]時刻をセットします
	void Set( const mDateTime::Time& dt );
	//[Setter]タイムスタンプをセットします
	void Set( const mDateTime::Timestamp& dt );


	//値をとるコンストラクタ
	template< class T >
	mOdbcParamsEntry( T value ) : mOdbcParamsEntry()
	{
		Set( value );
	}

	//[Getter]64ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , int64_t& retDt )const;
	//[Getter]32ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , int32_t& retDt )const;
	//[Getter]16ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , int16_t& retDt )const;
	//[Getter]8ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , int8_t& retDt )const;
	//[Getter]単精度浮動小数点値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , float& retDt )const;
	//[Getter]倍精度浮動小数点値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , double& retDt )const;
	//[Getter]ANSI/SJIS文字列を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , AString& retDt )const;
	//[Getter]Unicode文字列を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , WString& retDt )const;
	//[Getter]バイナリ列を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , mBinary& dt )const;
	//[Getter]日付を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , mDateTime::Date& dt )const;
	//[Getter]時刻を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , mDateTime::Time& dt )const;
	//[Getter]タイムスタンプを取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , mDateTime::Timestamp& dt )const;

	//[Getter]64ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , uint64_t& retDt )const;
	//[Getter]32ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , uint32_t& retDt )const;
	//[Getter]16ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , uint16_t& retDt )const;
	//[Getter]8ビット値を取得します
	//retDt : 結果の格納先
	//retresult : nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	void Get( bool& retresult , uint8_t& retDt )const;

	//保持している値をテンプレートで指定した型で返します
	//retValid : 有効な値を取得した場合は真
	//			 nullの場合、保持している値の型が異なるため変換出来ない場合は偽が返ります
	//ret : 保持している値。エラーの場合の値は不定。
	template< class T > T Get( bool& retValid )const
	{
		T Dummy;
		Get( retValid , Dummy );
		return Dummy;
	}

	//保持している値をテンプレートで指定した型で返します
	//retValue : 保持している値
	//ret : 有効な値を取得した場合は真。エラーの場合retValueの値は不定。
	template< class T > bool Query( T& retValue )const
	{
		bool result = false;
		Get( result , retValue );
		return result;
	}

	//保持している値をテンプレートで指定した型で返します
	//ret : 保持している値
	//失敗すると、テンプレートの第二パラメータで指定した型の例外を投げる。例外クラスはmExceptionBaseから派生したものであること。
	// 参考 using err = class ErrorClass : mExceptionBase{};
	template< class T , class ExceptionIfFailed > T Get( void )const
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );

		bool result;
		T buffer = Get< T >( result );

		if( result )
		{
			return buffer;
		}

		throw ExceptionIfFailed();
	}

	//保持している値をテンプレートで指定した型で返します
	//defvalue : エラーまたはnullの場合に返す値
	//ret : 保持している値（エラーの場合はdefvalueの値）
	template< class T > T Get( const T& defvalue )const
	{
		T value;
		bool result;
		Get( result , value );
		if( result )
		{
			return value;
		}
		return defvalue;
	}

	//保持している値をテンプレートで指定した型で返します
	//retValue : 保持している値
	//ret : 有効な値を取得した場合は真。エラーの場合retValueの値は不定。
	template< class T > bool Query( T& retValue , const T& defvalue )const
	{
		bool result;
		Get( result , retValue );
		if( !result )
		{
			retValue = defvalue;
		}
		return result;
	}

private:
	friend class mOdbcQuery;

	//[Setter]日付をセットします
	void Set( const SQL_DATE_STRUCT& dt );
	//[Setter]時刻をセットします
	void Set( const SQL_TIME_STRUCT& dt );
	//[Setter]タイムスタンプをセットします
	void Set( const SQL_TIMESTAMP_STRUCT& dt );

protected:

	//パラメーターの種類を決める(RTTI相当)
	mOdbc::ParameterType MyParameterType;

	//このパラメーターがヌルであれば真
	bool MyIsNull;

	//このパラメータの実データ
	union DataEntry
	{
		int64_t deInt64;
		int32_t deInt32;
		int16_t deInt16;
		int8_t deInt8;
		float deFloat;
		double deDouble;
		::AString deAString;
		::WString deWString;
		mBinary deBinary;
		SQL_DATE_STRUCT deDate;
		SQL_TIME_STRUCT deTime;
		SQL_TIMESTAMP_STRUCT deTimestamp;

		DataEntry()
		{
		}
		~DataEntry()
		{
		}
	};
	DataEntry MyDataEntry;

};

//クエリのパラメータ設定
typedef std::vector< mOdbcParamsEntry > mOdbcQueryParams;

//結果セットの取得用
// 左　：　結果の列名
// 右　：　取得結果
typedef std::unordered_map< WString , mOdbcParamsEntry > mOdbcResultParam;

//クエリ結果から指定の列を取得する。
// param : クエリ結果
// key : 読み取る列の名前
// def : デフォルトの値
// ret : 成功した場合、その列の値。
//       指定の列が存在しない場合、または読み取れない場合はデフォルトの値
template< class T >
T ReadOdbcResult( const mOdbcResultParam& param , const WString key , const T& def )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		return def;
	}
	return itr->second.Get<T>( def );
}

//クエリ結果から指定の列を取得する。
// param : クエリ結果
// key : 読み取る列の名前
// retValue : 成功した場合、その列の値。
//			  指定の列が存在しない場合、または読み取れない場合はデフォルトの値
// def : デフォルトの値
// ret : 成功した場合は真。指定の列が存在しない場合、または読み取れない場合は偽。
template< class T >
bool QueryOdbcResult( const mOdbcResultParam& param , const WString key , T& retValue , const T& def )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		retValue = def;
		return false;
	}
	return itr->second.Query<T>( retValue , def );
}

//クエリ結果から指定の列を取得する。
//失敗すると、テンプレートの第二パラメータで指定した型の例外を投げる。例外クラスはmExceptionBaseから派生したものであること。
// ※参考→ using err = class ErrorClass : mExceptionBase{};
// param : クエリ結果
// key : 読み取る列の名前
// ret : 読み取った列の値
template< class T , class ExceptionIfFailed >
T ReadOdbcResult( const mOdbcResultParam& param , const WString key )
{
	static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );

	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		throw ExceptionIfFailed();
	}
	return itr->second.Get<T,ExceptionIfFailed>();
}

//クエリ結果から指定の列を取得する。
// param : クエリ結果
// key : 読み取る列の名前
// retValid : 成功した場合は真。指定の列が存在しない場合、または読み取れない場合は偽。
// ret : 成功した場合、その列の値。
//       指定の列が存在しない場合、または読み取れない場合は不定。
template< class T >
T ReadOdbcResult( const mOdbcResultParam& param , const WString key , bool& retValid )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		return T();
	}
	return itr->second.Get<T>( retValid );
}

//クエリ結果から指定の列を取得する。
// param : クエリ結果
// key : 読み取る列の名前
// retValue : 成功した場合、その列の値。
//			  指定の列が存在しない場合、または読み取れない場合は不定。
// ret : 成功した場合は真。指定の列が存在しない場合、または読み取れない場合は偽。
template< class T >
bool QueryOdbcResult( const mOdbcResultParam& param , const WString key , T& retValue )
{
	mOdbcResultParam::const_iterator itr = param.find( key );
	if( itr == param.end() )
	{
		return false;
	}
	return itr->second.Query<T>( retValue );
}

#endif
