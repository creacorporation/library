//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MODBCQUERY_CPP_COMPILING
#include "mOdbcQuery.h"
#include "../General/mErrorLogger.h"



mOdbcQuery::mOdbcQuery()
{
	MyStmt = 0;
	MyIsDataExist = false;
	return;
}

mOdbcQuery::~mOdbcQuery()
{
	if( MyStmt )
	{
		if( !SQL_RESULT_CHECK( SQLFreeStmt( MyStmt , SQL_CLOSE ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"ステートメントの解放に失敗しました" );
		}
		if( !SQL_RESULT_CHECK( SQLFreeHandle( SQL_HANDLE_STMT , MyStmt ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"ステートメントハンドルの解放に失敗しました" );
		}
	}
	return;
}

bool mOdbcQuery::SQL_RESULT_CHECK( SQLRETURN rc )
{
	//成功以外の場合には、何らかの追加情報があるとみて、取得を試みる。
	if( rc != SQL_SUCCESS )
	{
		mOdbcSqlState::AppendLog( MyStmt , mOdbcSqlState::HandleKind::Statement );
	}
	return SQL_SUCCEEDED( rc );
}

//mOdbcDescriptionEntryに値を設定する
// retDesc : 設定先
// DataType : SQLデータ型
// ParameterSize : パラメータのサイズ(桁数)
// DecimalDigits : 小数点以下の桁数
// Nullable : ヌルを許容するか
static void SetDescriptionEntry(
	mOdbcDescriptionEntry& retDesc , 
	SQLSMALLINT DataType,
	SQLULEN ParameterSize,
	SQLSMALLINT DecimalDigits,
	SQLSMALLINT Nullable,
	INT Index )
{
	retDesc.MetaType = DataType;
	retDesc.Nullable = ( Nullable == SQL_NULLABLE ) ? ( true ) : ( false );
	retDesc.Size = ParameterSize;
	retDesc.Digit = DecimalDigits;
	retDesc.Index = Index;

	switch( DataType )
	{
	case SQL_BIGINT: 			//int64_t
	{
		retDesc.ParamType = mOdbc::ParameterType::Int64;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_INTEGER: 			//int32_t
	{
		retDesc.ParamType = mOdbc::ParameterType::Int32;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_SMALLINT: 			//int16_t
	{
		retDesc.ParamType = mOdbc::ParameterType::Int16;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_TINYINT: 			//int8_t
	{
		retDesc.ParamType = mOdbc::ParameterType::Int8;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_FLOAT:				//DOUBLE(倍精度浮動小数点型)←ODBCのFLOATはC言語で言うところのDOUBLE
	case SQL_DOUBLE: 			//DOUBLE(倍精度浮動小数点型)
	{
		retDesc.ParamType = mOdbc::ParameterType::Float;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_REAL: 				//FLOAT
	{
		retDesc.ParamType = mOdbc::ParameterType::Double;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_CHAR: 				//AString
	{
		retDesc.ParamType = mOdbc::ParameterType::AString;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_VARCHAR: 			//AString
	case SQL_LONGVARCHAR: 		//AString
	{
		retDesc.ParamType = mOdbc::ParameterType::AString;
		retDesc.Fixed = false;
		retDesc.Available = true;
		break;
	}

	case SQL_WCHAR: 			//WString
	{
		retDesc.ParamType = mOdbc::ParameterType::WString;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	case SQL_WVARCHAR: 			//WString
	case SQL_WLONGVARCHAR: 		//WString
	{
		retDesc.ParamType = mOdbc::ParameterType::WString;
		retDesc.Fixed = false;
		retDesc.Available = true;
		break;
	}

	case SQL_BINARY: 			//BYTE[]
	{
		retDesc.ParamType = mOdbc::ParameterType::Binary;
		retDesc.Fixed = false;
		retDesc.Available = true;
		break;
	}

	case SQL_VARBINARY: 		//BYTE[]
	case SQL_LONGVARBINARY: 	//BYTE[]
	{
		retDesc.ParamType = mOdbc::ParameterType::Binary;
		retDesc.Fixed = false;
		retDesc.Available = true;
		break;
	}

	case SQL_TYPE_DATE:				//mDateTime::Date
	{
		retDesc.ParamType = mOdbc::ParameterType::Date;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}
	case SQL_TYPE_TIME:				//mDateTime::Time
	{
		retDesc.ParamType = mOdbc::ParameterType::Time;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}
	case SQL_TYPE_TIMESTAMP:			//mDateTime::Date
	{
		retDesc.ParamType = mOdbc::ParameterType::Timestamp;
		retDesc.Fixed = true;
		retDesc.Available = true;
		break;
	}

	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータの種類が認識できませんでした" , DataType );
		retDesc.ParamType = mOdbc::ParameterType::Binary;
		retDesc.Fixed = true;
		retDesc.Available = false;
		break;
	}
}

bool mOdbcQuery::Prepare( const WString& query )
{
	SQLFreeStmt( MyStmt , SQL_CLOSE );
	if( !SQL_RESULT_CHECK( SQLPrepare( MyStmt , const_cast<SQLWCHAR*>( query.c_str() ) , (SQLINTEGER)query.size() ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SQL文をセットできませんでした" );
		return false;
	}

	//既存の結果を消去
	MyParameterDescription.clear();

	//パラメータの数を取得
	SQLSMALLINT cols;
	if( !SQL_RESULT_CHECK( SQLNumParams( MyStmt , &cols ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータ数の取得に失敗しました" );
		return false;
	}
	MyParameterDescription.reserve( cols );

	//情報の取得
	SQLSMALLINT DataType;
	SQLULEN ParameterSize;
	SQLSMALLINT DecimalDigits;
	SQLSMALLINT Nullable;
	for( SQLSMALLINT i = 1 ; i <= cols ; i++ )
	{
		if( !SQL_RESULT_CHECK( SQLDescribeParam( MyStmt , i , &DataType , &ParameterSize , &DecimalDigits , &Nullable ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"パラメータ情報の取得に失敗しました" );
			return false;
		}

		mOdbcDescriptionEntry desc;
		SetDescriptionEntry( desc , DataType , ParameterSize , DecimalDigits , Nullable , i );
		MyParameterDescription.push_back( desc );
	}
	return true;
}

bool mOdbcQuery::Prepare( const WString& query , const mOdbcParameterDescription& desc )
{
	SQLFreeStmt( MyStmt , SQL_CLOSE );
	if( !SQL_RESULT_CHECK( SQLPrepare( MyStmt , const_cast<SQLWCHAR*>( query.c_str() ) , (SQLINTEGER)query.size() ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SQL文をセットできませんでした" );
		return false;
	}

	//vector側でチェックしているので、descがMyParameterDescriptionと同じ実体でもOK
	MyParameterDescription = desc;
	return true;
}


bool mOdbcQuery::Execute( void )
{

	//SQLの実行
	MyIsDataExist = false;
	SQLRETURN rc = SQLExecute( MyStmt );
	if( !SQL_RESULT_CHECK( rc ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SQL文を実行できませんでした" );
		return false;
	}


	//既存の結果を消去
	MyResultDescription.clear();

	//結果セットの取得
	SQLSMALLINT cols = 0;
	if( !SQL_RESULT_CHECK( SQLNumResultCols( MyStmt , &cols ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"結果の列数を取得できませんでした" );
		return false;
	}

	//パラメータの設定
	WCHAR colname_static[ 100 ];
	std::unique_ptr<WCHAR> colname_dynamic;
	WCHAR* colname_ptr = colname_static;

	for( SQLSMALLINT index = 0 ; index < cols ; index++ )
	{
		SQLSMALLINT colnamelen;
		SQLSMALLINT sqltype;
		SQLULEN coldef;
		SQLSMALLINT scale;
		SQLSMALLINT nullable;

		//列情報の取得
		if( !SQL_RESULT_CHECK( SQLDescribeCol( MyStmt , index + 1 , colname_static , (SQLSMALLINT)array_count_of( colname_static ) , &colnamelen , &sqltype , &coldef , &scale , &nullable ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"結果の列情報を取得できませんでした" );
			return false;
		}

		//列名はあらかじめ確保したバッファに入り切ったか？
		colnamelen++;	//末端のヌルの分
		if( array_count_of( colname_static ) < static_cast<size_t>( colnamelen ) )
		{
			//列名が長すぎる場合は、動的にバッファを確保して再チャレンジする
			colname_dynamic.reset( mNew WCHAR[ colnamelen ] );

			if( !SQL_RESULT_CHECK( SQLDescribeCol( MyStmt , index + 1 , colname_dynamic.get() , colnamelen , nullptr , nullptr , nullptr , nullptr , nullptr ) ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"結果の列名を取得できませんでした" );
				return false;
			}
			colname_ptr = colname_dynamic.get();	//列名は動的に確保した
		}
		else
		{
			colname_ptr = colname_static;			//列名は静的に確保した
		}

		//列情報の追記
		mOdbcDescriptionEntry desc;
		SetDescriptionEntry( desc , sqltype , coldef , scale , nullable , index );

		if( MyResultDescription.count( colname_ptr ) == 0 )
		{
			MyResultDescription.insert( std::make_pair( WString( colname_ptr ) , desc ) ); 
		}
		else
		{
			//もし、同名の列が存在する場合は名前に"_n"をつけて回避する
			WString colname_changed;
			int num = 0;
			do
			{
				num++;
				sprintf( colname_changed , L"%s_%d" , colname_ptr , num );
			}while( MyResultDescription.count( colname_changed ) );

			CreateLogEntryF( g_ErrorLogger , 0 , L"列名が重複しているため変更されました" , L"[%s]→[%s]" , colname_ptr , colname_changed.c_str() );
			MyResultDescription.insert( std::make_pair( colname_changed , desc ) ); 
		}
	}

	//バッファの確保
	if( !CreateFetchBuffer( MyResultDescription ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バッファを確保できませんでした" );
		return false;
	}

	MyIsDataExist = true;
	return true;
}

bool mOdbcQuery::Execute( const mOdbcQueryParams& params )
{
	bool result = false;	//実行結果

	SQLSMALLINT ctype;		//C言語的型
	SQLSMALLINT sqltype;	//ODBC的型
	SQLULEN coldef;			//パラメーターの精度（サイズ）
	SQLSMALLINT scale;		//小数点の位置（デシマルやタイムスタンプで有効）
	SQLPOINTER dataptr;		//実際のデータへのポインタ
	SQLLEN datalen;			//データのバイト数

	//パラメータの数をチェック
	if( params.size() != MyParameterDescription.size() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータの数がクエリと異なります" );
		goto end;
	}

	//StrLen_or_IndPtrに渡すバッファを再確保
	MyParameterLenArray.resize( params.size() );

	//パラメータの設定
	for( size_t index = 0 ; index < params.size() ; index++ )
	{
		sqltype = (SQLSMALLINT)( MyParameterDescription[ index ].MetaType );
		coldef = MyParameterDescription[ index ].Size;
		scale = (SQLSMALLINT)( MyParameterDescription[ index ].Digit );

		mOdbcParamsEntry& param = const_cast<mOdbcParamsEntry&>( params[ index ] );
		ctype = ParameterType2CType( param.GetParameterType() );

		if( !param.IsNull() )
		{
			switch( param.GetParameterType() )
			{
			case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt64 );
				datalen = sizeof( param.MyDataEntry.deInt64 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt32 );
				datalen = sizeof( param.MyDataEntry.deInt32 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt16 );
				datalen = sizeof( param.MyDataEntry.deInt16 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt8 );
				datalen = sizeof( param.MyDataEntry.deInt8 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Float:		//単精度浮動小数点
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deFloat );
				datalen = sizeof( param.MyDataEntry.deFloat );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Double:		//倍精度浮動小数点
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deDouble );
				datalen = sizeof( param.MyDataEntry.deDouble );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::AString:		//ANSI文字列
				dataptr = (SQLPOINTER)( param.MyDataEntry.deAString.c_str() );
				datalen = param.MyDataEntry.deAString.size();
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::WString:		//UNICODE文字列
				dataptr = (SQLPOINTER)( param.MyDataEntry.deWString.c_str() );
				datalen = param.MyDataEntry.deWString.size() * sizeof( WString::value_type );	//バイト単位なので
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Binary:		//バイナリ
				dataptr = (SQLPOINTER)( param.MyDataEntry.deBinary.data() );
				datalen = param.MyDataEntry.deBinary.size();
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Date:		//日付
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deDate );
				datalen = sizeof( param.MyDataEntry.deDate );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Time:		//時刻
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deTime );
				datalen = sizeof( param.MyDataEntry.deTime );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deTimestamp );
				datalen = sizeof( param.MyDataEntry.deTimestamp );
				scale = 3;
				MyParameterLenArray[ index ] = datalen;
				break;

			default:
				RaiseAssert( g_ErrorLogger , 0 , L"パラメータが不正です" );
				goto end;
			}
		}
		else
		{
			ctype = ParameterType2CType( param.GetParameterType() );
			dataptr = nullptr;
			datalen = 0;
			MyParameterLenArray[ index ] = SQL_NULL_DATA;
		}

		if( !SQL_RESULT_CHECK( SQLBindParameter( MyStmt , (SQLSMALLINT)( index + 1 ) , SQL_PARAM_INPUT , ctype , sqltype , coldef , scale , dataptr , datalen , &MyParameterLenArray[ index ] ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"パラメータ設定に失敗しました" , index );
			goto end;
		}
	}
	result = Execute();
end:
	SQLFreeStmt( MyStmt , SQL_RESET_PARAMS );	// ←Execute()のエラーを残したいのでここではエラーチェックをしない
	return result;
}

SQLSMALLINT mOdbcQuery::ParameterType2CType( mOdbc::ParameterType type )const
{
	switch( type )
	{
	case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
		return SQL_C_SBIGINT;
	case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
		return SQL_C_LONG;
	case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
		return SQL_C_SHORT;
	case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
		return SQL_C_TINYINT;
	case mOdbc::ParameterType::Float:		//単精度浮動小数点
		return SQL_C_FLOAT;
	case mOdbc::ParameterType::Double:		//倍精度浮動小数点
		return SQL_C_DOUBLE;
	case mOdbc::ParameterType::AString:		//ANSI文字列
		return SQL_C_CHAR;
	case mOdbc::ParameterType::WString:		//UNICODE文字列
		return SQL_C_WCHAR;
	case mOdbc::ParameterType::Binary:		//バイナリ
		return SQL_C_BINARY;
	case mOdbc::ParameterType::Date:		//日付
		return SQL_C_TYPE_DATE;
	case mOdbc::ParameterType::Time:		//時刻
		return SQL_C_TYPE_TIME;
	case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
		return SQL_C_TYPE_TIMESTAMP;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"不明なパラメータです" , type );
		break;
	}
	return SQL_C_BINARY;
}

bool mOdbcQuery::Execute( const WString& query )
{
	if( !Prepare( query ) )
	{
		return false;
	}
	if( !Execute() )
	{
		return false;
	}
	return true;
}

const mOdbcParameterDescription& mOdbcQuery::GetParameterDescription( void )const
{
	return MyParameterDescription;
}

const mOdbcResultDescription& mOdbcQuery::GetResultDescription( void )const
{
	return MyResultDescription;
}

mOdbcQuery::FetchResult mOdbcQuery::Fetch( mOdbcResultParam& retResult )
{
	FetchResult result = FetchResult::FETCH_SUCCEEDED;

	//既存のデータを破棄
	retResult.clear();

	//フェッチ実施
	SQLRETURN rc = SQLFetch( MyStmt );
	switch( rc )
	{
	case SQL_NO_DATA:
		MyIsDataExist = false;
		return FetchResult::FETCH_NOMOREDATA;
	default:
		break;
	}

	//各列のデータを取得してセット
	for( mOdbcResultDescription::const_iterator descitr = MyResultDescription.begin() ; descitr != MyResultDescription.end() ; descitr++ )
	{
		FetchBufferEntry& buff = MyFetchBuffer.at( descitr->second.Index );
		if( buff.value == SQL_NULL_DATA )
		{
			//取得したデータはヌルでした
			retResult[ descitr->first ].SetNull();
		}
		else
		{
			//ヌル以外の何らかのデータを取得した
			if( buff.size < buff.value )
			{
				//バッファのサイズが足りない場合はエラーを記録する
				RaiseError( g_ErrorLogger , 0 , L"バッファサイズが不足したため、データを切り詰めました" , descitr->first );
				result = FetchResult::FETCH_TRUNCATED;
			}

			//フェッチしたデータを設定
			switch( descitr->second.ParamType )
			{
			case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
				retResult[ descitr->first ].Set( *(int64_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
				retResult[ descitr->first ].Set( *(int32_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
				retResult[ descitr->first ].Set( *(int16_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
				retResult[ descitr->first ].Set( *(int8_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Float:		//単精度浮動小数点
				retResult[ descitr->first ].Set( *(float*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Double:		//倍精度浮動小数点
				retResult[ descitr->first ].Set( *(double*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::AString:		//ANSI文字列
				retResult[ descitr->first ].Set( (CHAR*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::WString:		//UNICODE文字列
				retResult[ descitr->first ].Set( (WCHAR*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Binary:		//バイナリ
				break;
			case mOdbc::ParameterType::Date:		//日付
				retResult[ descitr->first ].Set( *(SQL_DATE_STRUCT*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Time:		//時刻
				retResult[ descitr->first ].Set( *(SQL_TIME_STRUCT*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
				retResult[ descitr->first ].Set( *(SQL_TIMESTAMP_STRUCT*)buff.ptr.get() );
				break;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"不明なパラメータです" , descitr->second.ParamType );
				return FetchResult::FETCH_UNKNOWNTYPE;
			}
		}
	}

	return result;
}

bool mOdbcQuery::CreateFetchBuffer( const mOdbcResultDescription& desc )
{
	//ステートメントハンドルからバッファをアンバインドする
	//※SQLExecuteの実行エラーを残したいので、エラーにはしない
	if( !SQL_RESULT_CHECK( SQLFreeStmt( MyStmt , SQL_UNBIND ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"パラメータのリセットが失敗しました" );
		return false;
	}
	
	//既存のバッファを廃棄/再確保する
	MyFetchBuffer.clear();
	MyFetchBuffer.resize( desc.size() );

	//列情報を設定していく…
	for( mOdbcResultDescription::const_iterator itr = desc.begin() ; itr != desc.end() ; itr++ )
	{
		SQLLEN req_size;
		//桁数→バイト数変換
		switch( itr->second.ParamType )
		{
		case mOdbc::ParameterType::Int64:		//符号あり64ビット整数型(符号なしはありません)
			req_size = sizeof( int64_t );
			break;
		case mOdbc::ParameterType::Int32:		//符号あり32ビット整数型(符号なしはありません)
			req_size = sizeof( int32_t );
			break;
		case mOdbc::ParameterType::Int16:		//符号あり16ビット整数型(符号なしはありません)
			req_size = sizeof( int16_t );
			break;
		case mOdbc::ParameterType::Int8:		//符号あり8ビット整数型(符号なしはありません)
			req_size = sizeof( int8_t );
			break;
		case mOdbc::ParameterType::Float:		//単精度浮動小数点
			req_size = sizeof( float );
			break;
		case mOdbc::ParameterType::Double:		//倍精度浮動小数点
			req_size = sizeof( double );
			break;
		case mOdbc::ParameterType::AString:		//ANSI文字列
			req_size = ( MAX_FETCH_BUFFER_SIZE < itr->second.Size ) ? ( MAX_FETCH_BUFFER_SIZE ) : ( itr->second.Size + 1 );
			req_size *= sizeof( CHAR );
			break;
		case mOdbc::ParameterType::WString:		//UNICODE文字列
			req_size = ( MAX_FETCH_BUFFER_SIZE < itr->second.Size ) ? ( MAX_FETCH_BUFFER_SIZE ) : ( itr->second.Size + 1 );
			req_size *= sizeof( WCHAR );
			break;
		case mOdbc::ParameterType::Date:		//日付
			req_size = sizeof( SQL_DATE_STRUCT );
			break;
		case mOdbc::ParameterType::Time:		//時刻
			req_size = sizeof( SQL_TIME_STRUCT );
			break;
		case mOdbc::ParameterType::Timestamp:	//タイムスタンプ
			req_size = sizeof( SQL_TIMESTAMP_STRUCT );
			break;
		case mOdbc::ParameterType::Binary:		//バイナリ
		default:
			req_size = ( MAX_FETCH_BUFFER_SIZE < itr->second.Size ) ? ( MAX_FETCH_BUFFER_SIZE ) : ( itr->second.Size );
			req_size *= sizeof( BYTE );
			break;
		}

		if( !SetFetchBuffer( itr->second , req_size ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"パラメータのセットが失敗しました" , itr->first );
			return false;
		}
	}
	return true;

}

bool mOdbcQuery::ResizeFetchBuffer( const WString colname , size_t buffsize )
{
	mOdbcResultDescription::const_iterator itr = MyResultDescription.find( colname );
	if( itr == MyResultDescription.end() )
	{
		RaiseError( g_ErrorLogger , 0 , L"指定された列は存在しません" , colname );
		return false;
	}

	return SetFetchBuffer( itr->second , buffsize );
}

bool mOdbcQuery::SetFetchBuffer( const mOdbcDescriptionEntry& desc , SQLLEN size )
{
	//CTYPEの取得
	SQLSMALLINT ctype;	//Cデータ型
	ctype = ParameterType2CType( desc.ParamType );

	//バッファの確保
	MyFetchBuffer[ desc.Index ].size = size;
	MyFetchBuffer[ desc.Index ].ptr.reset( mNew BYTE[ size ] );

	//バインド。
	if( !SQL_RESULT_CHECK( SQLBindCol( MyStmt , desc.Index + 1 , ctype , MyFetchBuffer[ desc.Index ].ptr.get() , size ,  &MyFetchBuffer[ desc.Index ].value ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"結果列のバインドに失敗しました" , desc.Size );
		return false;
	}

	return true;
}

bool mOdbcQuery::IsDataExist( void )const
{
	return MyIsDataExist;
}

bool mOdbcQuery::Recycle( void )
{
	MyIsDataExist = false;
	if( MyStmt )
	{
		if( !SQL_RESULT_CHECK( SQLCloseCursor( MyStmt ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"カーソルのクローズに失敗しました" );
		}
		return false;
	}
	return true;
}



