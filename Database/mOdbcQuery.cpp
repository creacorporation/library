//----------------------------------------------------------------------------
// ODBC�ڑ��p���C�u����
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
			RaiseAssert( g_ErrorLogger , 0 , L"�X�e�[�g�����g�̉���Ɏ��s���܂���" );
		}
		if( !SQL_RESULT_CHECK( SQLFreeHandle( SQL_HANDLE_STMT , MyStmt ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�X�e�[�g�����g�n���h���̉���Ɏ��s���܂���" );
		}
	}
	return;
}

bool mOdbcQuery::SQL_RESULT_CHECK( SQLRETURN rc )
{
	//�����ȊO�̏ꍇ�ɂ́A���炩�̒ǉ���񂪂���Ƃ݂āA�擾�����݂�B
	if( rc != SQL_SUCCESS )
	{
		mOdbcSqlState::AppendLog( MyStmt , mOdbcSqlState::HandleKind::Statement );
	}
	return SQL_SUCCEEDED( rc );
}

//mOdbcDescriptionEntry�ɒl��ݒ肷��
// retDesc : �ݒ��
// DataType : SQL�f�[�^�^
// ParameterSize : �p�����[�^�̃T�C�Y(����)
// DecimalDigits : �����_�ȉ��̌���
// Nullable : �k�������e���邩
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

	case SQL_FLOAT:				//DOUBLE(�{���x���������_�^)��ODBC��FLOAT��C����Ō����Ƃ����DOUBLE
	case SQL_DOUBLE: 			//DOUBLE(�{���x���������_�^)
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
		RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^�̎�ނ��F���ł��܂���ł���" , DataType );
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
		RaiseAssert( g_ErrorLogger , 0 , L"SQL�����Z�b�g�ł��܂���ł���" );
		return false;
	}

	//�����̌��ʂ�����
	MyParameterDescription.clear();

	//�p�����[�^�̐����擾
	SQLSMALLINT cols;
	if( !SQL_RESULT_CHECK( SQLNumParams( MyStmt , &cols ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^���̎擾�Ɏ��s���܂���" );
		return false;
	}
	MyParameterDescription.reserve( cols );

	//���̎擾
	SQLSMALLINT DataType;
	SQLULEN ParameterSize;
	SQLSMALLINT DecimalDigits;
	SQLSMALLINT Nullable;
	for( SQLSMALLINT i = 1 ; i <= cols ; i++ )
	{
		if( !SQL_RESULT_CHECK( SQLDescribeParam( MyStmt , i , &DataType , &ParameterSize , &DecimalDigits , &Nullable ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^���̎擾�Ɏ��s���܂���" );
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
		RaiseAssert( g_ErrorLogger , 0 , L"SQL�����Z�b�g�ł��܂���ł���" );
		return false;
	}

	//vector���Ń`�F�b�N���Ă���̂ŁAdesc��MyParameterDescription�Ɠ������̂ł�OK
	MyParameterDescription = desc;
	return true;
}


bool mOdbcQuery::Execute( void )
{

	//SQL�̎��s
	MyIsDataExist = false;
	SQLRETURN rc = SQLExecute( MyStmt );
	if( !SQL_RESULT_CHECK( rc ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SQL�������s�ł��܂���ł���" );
		return false;
	}


	//�����̌��ʂ�����
	MyResultDescription.clear();

	//���ʃZ�b�g�̎擾
	SQLSMALLINT cols = 0;
	if( !SQL_RESULT_CHECK( SQLNumResultCols( MyStmt , &cols ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���ʂ̗񐔂��擾�ł��܂���ł���" );
		return false;
	}

	//�p�����[�^�̐ݒ�
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

		//����̎擾
		if( !SQL_RESULT_CHECK( SQLDescribeCol( MyStmt , index + 1 , colname_static , (SQLSMALLINT)array_count_of( colname_static ) , &colnamelen , &sqltype , &coldef , &scale , &nullable ) ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"���ʂ̗�����擾�ł��܂���ł���" );
			return false;
		}

		//�񖼂͂��炩���ߊm�ۂ����o�b�t�@�ɓ���؂������H
		colnamelen++;	//���[�̃k���̕�
		if( array_count_of( colname_static ) < static_cast<size_t>( colnamelen ) )
		{
			//�񖼂���������ꍇ�́A���I�Ƀo�b�t�@���m�ۂ��čă`�������W����
			colname_dynamic.reset( mNew WCHAR[ colnamelen ] );

			if( !SQL_RESULT_CHECK( SQLDescribeCol( MyStmt , index + 1 , colname_dynamic.get() , colnamelen , nullptr , nullptr , nullptr , nullptr , nullptr ) ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"���ʂ̗񖼂��擾�ł��܂���ł���" );
				return false;
			}
			colname_ptr = colname_dynamic.get();	//�񖼂͓��I�Ɋm�ۂ���
		}
		else
		{
			colname_ptr = colname_static;			//�񖼂͐ÓI�Ɋm�ۂ���
		}

		//����̒ǋL
		mOdbcDescriptionEntry desc;
		SetDescriptionEntry( desc , sqltype , coldef , scale , nullable , index );

		if( MyResultDescription.count( colname_ptr ) == 0 )
		{
			MyResultDescription.insert( std::make_pair( WString( colname_ptr ) , desc ) ); 
		}
		else
		{
			//�����A�����̗񂪑��݂���ꍇ�͖��O��"_n"�����ĉ������
			WString colname_changed;
			int num = 0;
			do
			{
				num++;
				sprintf( colname_changed , L"%s_%d" , colname_ptr , num );
			}while( MyResultDescription.count( colname_changed ) );

			CreateLogEntryF( g_ErrorLogger , 0 , L"�񖼂��d�����Ă��邽�ߕύX����܂���" , L"[%s]��[%s]" , colname_ptr , colname_changed.c_str() );
			MyResultDescription.insert( std::make_pair( colname_changed , desc ) ); 
		}
	}

	//�o�b�t�@�̊m��
	if( !CreateFetchBuffer( MyResultDescription ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@���m�ۂł��܂���ł���" );
		return false;
	}

	MyIsDataExist = true;
	return true;
}

bool mOdbcQuery::Execute( const mOdbcQueryParams& params )
{
	bool result = false;	//���s����

	SQLSMALLINT ctype;		//C����I�^
	SQLSMALLINT sqltype;	//ODBC�I�^
	SQLULEN coldef;			//�p�����[�^�[�̐��x�i�T�C�Y�j
	SQLSMALLINT scale;		//�����_�̈ʒu�i�f�V�}����^�C���X�^���v�ŗL���j
	SQLPOINTER dataptr;		//���ۂ̃f�[�^�ւ̃|�C���^
	SQLLEN datalen;			//�f�[�^�̃o�C�g��

	//�p�����[�^�̐����`�F�b�N
	if( params.size() != MyParameterDescription.size() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^�̐����N�G���ƈقȂ�܂�" );
		goto end;
	}

	//StrLen_or_IndPtr�ɓn���o�b�t�@���Ċm��
	MyParameterLenArray.resize( params.size() );

	//�p�����[�^�̐ݒ�
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
			case mOdbc::ParameterType::Int64:		//��������64�r�b�g�����^(�����Ȃ��͂���܂���)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt64 );
				datalen = sizeof( param.MyDataEntry.deInt64 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Int32:		//��������32�r�b�g�����^(�����Ȃ��͂���܂���)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt32 );
				datalen = sizeof( param.MyDataEntry.deInt32 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Int16:		//��������16�r�b�g�����^(�����Ȃ��͂���܂���)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt16 );
				datalen = sizeof( param.MyDataEntry.deInt16 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Int8:		//��������8�r�b�g�����^(�����Ȃ��͂���܂���)
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deInt8 );
				datalen = sizeof( param.MyDataEntry.deInt8 );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Float:		//�P���x���������_
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deFloat );
				datalen = sizeof( param.MyDataEntry.deFloat );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Double:		//�{���x���������_
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deDouble );
				datalen = sizeof( param.MyDataEntry.deDouble );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::AString:		//ANSI������
				dataptr = (SQLPOINTER)( param.MyDataEntry.deAString.c_str() );
				datalen = param.MyDataEntry.deAString.size();
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::WString:		//UNICODE������
				dataptr = (SQLPOINTER)( param.MyDataEntry.deWString.c_str() );
				datalen = param.MyDataEntry.deWString.size() * sizeof( WString::value_type );	//�o�C�g�P�ʂȂ̂�
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Binary:		//�o�C�i��
				dataptr = (SQLPOINTER)( param.MyDataEntry.deBinary.data() );
				datalen = param.MyDataEntry.deBinary.size();
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Date:		//���t
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deDate );
				datalen = sizeof( param.MyDataEntry.deDate );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Time:		//����
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deTime );
				datalen = sizeof( param.MyDataEntry.deTime );
				MyParameterLenArray[ index ] = datalen;
				break;
			case mOdbc::ParameterType::Timestamp:	//�^�C���X�^���v
				dataptr = (SQLPOINTER)&( param.MyDataEntry.deTimestamp );
				datalen = sizeof( param.MyDataEntry.deTimestamp );
				scale = 3;
				MyParameterLenArray[ index ] = datalen;
				break;

			default:
				RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^���s���ł�" );
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
			RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^�ݒ�Ɏ��s���܂���" , index );
			goto end;
		}
	}
	result = Execute();
end:
	SQLFreeStmt( MyStmt , SQL_RESET_PARAMS );	// ��Execute()�̃G���[���c�������̂ł����ł̓G���[�`�F�b�N�����Ȃ�
	return result;
}

SQLSMALLINT mOdbcQuery::ParameterType2CType( mOdbc::ParameterType type )const
{
	switch( type )
	{
	case mOdbc::ParameterType::Int64:		//��������64�r�b�g�����^(�����Ȃ��͂���܂���)
		return SQL_C_SBIGINT;
	case mOdbc::ParameterType::Int32:		//��������32�r�b�g�����^(�����Ȃ��͂���܂���)
		return SQL_C_LONG;
	case mOdbc::ParameterType::Int16:		//��������16�r�b�g�����^(�����Ȃ��͂���܂���)
		return SQL_C_SHORT;
	case mOdbc::ParameterType::Int8:		//��������8�r�b�g�����^(�����Ȃ��͂���܂���)
		return SQL_C_TINYINT;
	case mOdbc::ParameterType::Float:		//�P���x���������_
		return SQL_C_FLOAT;
	case mOdbc::ParameterType::Double:		//�{���x���������_
		return SQL_C_DOUBLE;
	case mOdbc::ParameterType::AString:		//ANSI������
		return SQL_C_CHAR;
	case mOdbc::ParameterType::WString:		//UNICODE������
		return SQL_C_WCHAR;
	case mOdbc::ParameterType::Binary:		//�o�C�i��
		return SQL_C_BINARY;
	case mOdbc::ParameterType::Date:		//���t
		return SQL_C_TYPE_DATE;
	case mOdbc::ParameterType::Time:		//����
		return SQL_C_TYPE_TIME;
	case mOdbc::ParameterType::Timestamp:	//�^�C���X�^���v
		return SQL_C_TYPE_TIMESTAMP;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�s���ȃp�����[�^�ł�" , type );
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

	//�����̃f�[�^��j��
	retResult.clear();

	//�t�F�b�`���{
	SQLRETURN rc = SQLFetch( MyStmt );
	switch( rc )
	{
	case SQL_NO_DATA:
		MyIsDataExist = false;
		return FetchResult::FETCH_NOMOREDATA;
	default:
		break;
	}

	//�e��̃f�[�^���擾���ăZ�b�g
	for( mOdbcResultDescription::const_iterator descitr = MyResultDescription.begin() ; descitr != MyResultDescription.end() ; descitr++ )
	{
		FetchBufferEntry& buff = MyFetchBuffer.at( descitr->second.Index );
		if( buff.value == SQL_NULL_DATA )
		{
			//�擾�����f�[�^�̓k���ł���
			retResult[ descitr->first ].SetNull();
		}
		else
		{
			//�k���ȊO�̉��炩�̃f�[�^���擾����
			if( buff.size < buff.value )
			{
				//�o�b�t�@�̃T�C�Y������Ȃ��ꍇ�̓G���[���L�^����
				RaiseError( g_ErrorLogger , 0 , L"�o�b�t�@�T�C�Y���s���������߁A�f�[�^��؂�l�߂܂���" , descitr->first );
				result = FetchResult::FETCH_TRUNCATED;
			}

			//�t�F�b�`�����f�[�^��ݒ�
			switch( descitr->second.ParamType )
			{
			case mOdbc::ParameterType::Int64:		//��������64�r�b�g�����^(�����Ȃ��͂���܂���)
				retResult[ descitr->first ].Set( *(int64_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Int32:		//��������32�r�b�g�����^(�����Ȃ��͂���܂���)
				retResult[ descitr->first ].Set( *(int32_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Int16:		//��������16�r�b�g�����^(�����Ȃ��͂���܂���)
				retResult[ descitr->first ].Set( *(int16_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Int8:		//��������8�r�b�g�����^(�����Ȃ��͂���܂���)
				retResult[ descitr->first ].Set( *(int8_t*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Float:		//�P���x���������_
				retResult[ descitr->first ].Set( *(float*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Double:		//�{���x���������_
				retResult[ descitr->first ].Set( *(double*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::AString:		//ANSI������
				retResult[ descitr->first ].Set( (CHAR*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::WString:		//UNICODE������
				retResult[ descitr->first ].Set( (WCHAR*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Binary:		//�o�C�i��
				break;
			case mOdbc::ParameterType::Date:		//���t
				retResult[ descitr->first ].Set( *(SQL_DATE_STRUCT*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Time:		//����
				retResult[ descitr->first ].Set( *(SQL_TIME_STRUCT*)buff.ptr.get() );
				break;
			case mOdbc::ParameterType::Timestamp:	//�^�C���X�^���v
				retResult[ descitr->first ].Set( *(SQL_TIMESTAMP_STRUCT*)buff.ptr.get() );
				break;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"�s���ȃp�����[�^�ł�" , descitr->second.ParamType );
				return FetchResult::FETCH_UNKNOWNTYPE;
			}
		}
	}

	return result;
}

bool mOdbcQuery::CreateFetchBuffer( const mOdbcResultDescription& desc )
{
	//�X�e�[�g�����g�n���h������o�b�t�@���A���o�C���h����
	//��SQLExecute�̎��s�G���[���c�������̂ŁA�G���[�ɂ͂��Ȃ�
	if( !SQL_RESULT_CHECK( SQLFreeStmt( MyStmt , SQL_UNBIND ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^�̃��Z�b�g�����s���܂���" );
		return false;
	}
	
	//�����̃o�b�t�@��p��/�Ċm�ۂ���
	MyFetchBuffer.clear();
	MyFetchBuffer.resize( desc.size() );

	//�����ݒ肵�Ă����c
	for( mOdbcResultDescription::const_iterator itr = desc.begin() ; itr != desc.end() ; itr++ )
	{
		SQLLEN req_size;
		//�������o�C�g���ϊ�
		switch( itr->second.ParamType )
		{
		case mOdbc::ParameterType::Int64:		//��������64�r�b�g�����^(�����Ȃ��͂���܂���)
			req_size = sizeof( int64_t );
			break;
		case mOdbc::ParameterType::Int32:		//��������32�r�b�g�����^(�����Ȃ��͂���܂���)
			req_size = sizeof( int32_t );
			break;
		case mOdbc::ParameterType::Int16:		//��������16�r�b�g�����^(�����Ȃ��͂���܂���)
			req_size = sizeof( int16_t );
			break;
		case mOdbc::ParameterType::Int8:		//��������8�r�b�g�����^(�����Ȃ��͂���܂���)
			req_size = sizeof( int8_t );
			break;
		case mOdbc::ParameterType::Float:		//�P���x���������_
			req_size = sizeof( float );
			break;
		case mOdbc::ParameterType::Double:		//�{���x���������_
			req_size = sizeof( double );
			break;
		case mOdbc::ParameterType::AString:		//ANSI������
			req_size = ( MAX_FETCH_BUFFER_SIZE < itr->second.Size ) ? ( MAX_FETCH_BUFFER_SIZE ) : ( itr->second.Size + 1 );
			req_size *= sizeof( CHAR );
			break;
		case mOdbc::ParameterType::WString:		//UNICODE������
			req_size = ( MAX_FETCH_BUFFER_SIZE < itr->second.Size ) ? ( MAX_FETCH_BUFFER_SIZE ) : ( itr->second.Size + 1 );
			req_size *= sizeof( WCHAR );
			break;
		case mOdbc::ParameterType::Date:		//���t
			req_size = sizeof( SQL_DATE_STRUCT );
			break;
		case mOdbc::ParameterType::Time:		//����
			req_size = sizeof( SQL_TIME_STRUCT );
			break;
		case mOdbc::ParameterType::Timestamp:	//�^�C���X�^���v
			req_size = sizeof( SQL_TIMESTAMP_STRUCT );
			break;
		case mOdbc::ParameterType::Binary:		//�o�C�i��
		default:
			req_size = ( MAX_FETCH_BUFFER_SIZE < itr->second.Size ) ? ( MAX_FETCH_BUFFER_SIZE ) : ( itr->second.Size );
			req_size *= sizeof( BYTE );
			break;
		}

		if( !SetFetchBuffer( itr->second , req_size ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�p�����[�^�̃Z�b�g�����s���܂���" , itr->first );
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
		RaiseError( g_ErrorLogger , 0 , L"�w�肳�ꂽ��͑��݂��܂���" , colname );
		return false;
	}

	return SetFetchBuffer( itr->second , buffsize );
}

bool mOdbcQuery::SetFetchBuffer( const mOdbcDescriptionEntry& desc , SQLLEN size )
{
	//CTYPE�̎擾
	SQLSMALLINT ctype;	//C�f�[�^�^
	ctype = ParameterType2CType( desc.ParamType );

	//�o�b�t�@�̊m��
	MyFetchBuffer[ desc.Index ].size = size;
	MyFetchBuffer[ desc.Index ].ptr.reset( mNew BYTE[ size ] );

	//�o�C���h�B
	if( !SQL_RESULT_CHECK( SQLBindCol( MyStmt , desc.Index + 1 , ctype , MyFetchBuffer[ desc.Index ].ptr.get() , size ,  &MyFetchBuffer[ desc.Index ].value ) ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���ʗ�̃o�C���h�Ɏ��s���܂���" , desc.Size );
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
			RaiseAssert( g_ErrorLogger , 0 , L"�J�[�\���̃N���[�Y�Ɏ��s���܂���" );
		}
		return false;
	}
	return true;
}



