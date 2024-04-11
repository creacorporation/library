//----------------------------------------------------------------------------
// レジストリアクセス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mRegistry.h"
#include "General/mErrorLogger.h"
#include <memory>

mRegistry::mRegistry()
{
	MyKey = (HKEY)INVALID_HANDLE_VALUE;
}

mRegistry::~mRegistry()
{
	Close();
}

bool mRegistry::Open( ParentKey parent , const WString& path , BOOL access_write )
{
	Close();

	HKEY parent_handle;
	switch( parent )
	{
	case ParentKey::CURRENT_USER:
		parent_handle = HKEY_CURRENT_USER;
		break;
	case ParentKey::LOCAL_MACHINE:
		parent_handle = HKEY_LOCAL_MACHINE;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"親キーの指定が不正です" );
		return false;
	}

	DWORD priv = KEY_READ;
	if( access_write )
	{
		priv |= KEY_WRITE;
	}

	LONG result = RegCreateKeyEx( 
		parent_handle ,				// 開いている親キーのハンドル
		path.c_str(),				// 開くべきサブキーの名前
		0,							// 予約済み
		0,							// クラスの文字列
		REG_OPTION_NON_VOLATILE,	// 特別なオプション
		priv,						// セキュリティアクセスマスク
		0,							// 継承の指定
		&MyKey,						// 開くことに成功したサブキーのハンドル
		0 );						// 既存かどうかを示す値が格納される変数

	if( result != ERROR_SUCCESS )
	{
		MyKey = (HKEY)INVALID_HANDLE_VALUE;
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーのハンドルを取得できませんでした" );
		return false;
	}

	return true;
}

bool mRegistry::Close( void )
{
	if( MyKey != INVALID_HANDLE_VALUE )
	{
		RegCloseKey( MyKey );
	}
	return true;
}

DWORD mRegistry::GetDword( const WString &entry , DWORD def_value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return def_value;
	}

	//値読み取り
	DWORD data_type;
	DWORD data_value;
	DWORD data_size = sizeof( data_value );
	LONG result = RegQueryValueEx( MyKey , entry.c_str() , 0 , &data_type , (LPBYTE)&data_value , &data_size );

	//チェック
	if( result != ERROR_SUCCESS )	//エラー？
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリの読み取りが失敗しました" , entry );
		return def_value;
	}
	if( data_type != REG_DWORD )	//型のチェック
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリの型が違います" , entry );
		return def_value;
	}
	if( data_size != sizeof( data_value ) )	//サイズのチェック
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリのデータサイズが違います" , entry );
		return def_value;
	}
	return data_value;
}

bool mRegistry::SetDword( const WString &entry , DWORD value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return false;
	}

	//書き込み
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_DWORD , (LPCBYTE)&value , sizeof( value ) ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリへの書き込みが失敗しました" , entry );
	}
	return true;
}

ULONGLONG mRegistry::GetQword( const WString &entry , ULONGLONG def_value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return def_value;
	}

	//値読み取り
	DWORD data_type;
	ULONGLONG data_value;
	DWORD data_size = sizeof( data_value );
	LONG result = RegQueryValueEx( MyKey , entry.c_str() , 0 , &data_type , (LPBYTE)&data_value , &data_size );

	//チェック
	if( result != ERROR_SUCCESS )	//エラー？
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリの読み取りが失敗しました" , entry );
		return def_value;
	}
	if( data_type != REG_QWORD )	//型のチェック
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリの型が違います" , entry );
		return def_value;
	}
	if( data_size != sizeof( data_value ) )	//サイズのチェック
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリのデータサイズが違います" , entry );
		return def_value;
	}
	return data_value;
}

bool mRegistry::SetQword( const WString &entry , ULONGLONG value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return false;
	}

	//書き込み
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_QWORD , (LPCBYTE)&value , sizeof( value ) ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリへの書き込みが失敗しました" , entry );
	}
	return true;
}

//レジストリから可変長データを読み取る
static bool ReadInternal( HKEY key , const WString& entry , WString& retData , DWORD& data_type )
{
	DWORD required_size = 0;
	if( RegQueryValueEx( key , entry.c_str() , 0 , nullptr , nullptr , &required_size ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリの文字列データのサイズを取得できません" , entry );
		return false;
	}

	std::unique_ptr<BYTE> ptr( mNew BYTE[ required_size ] );
	if( RegQueryValueEx( key , entry.c_str() , 0 , &data_type , ptr.get() , &required_size ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリの文字列データを取得できません" , entry );
		return false;
	}

	retData.resize( required_size / sizeof( wchar_t ) );
	MoveMemory( const_cast<wchar_t*>( retData.data() ) , ptr.get() , required_size );
	return true;
}


//1行読み取り
WString mRegistry::GetString( const WString& entry , const WString& def_value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return def_value;
	}

	//文字列読み取り
	DWORD data_type;
	WString result_string;
	if( !ReadInternal( MyKey , entry , result_string , data_type ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリのデータを読み取れません" , entry );
		return def_value;
	}
	//値の種別が文字列であるかを確認する
	if( data_type != REG_SZ )
	{
		//文字列ではない
		RaiseError( g_ErrorLogger , 0 , L"レジストリの型が違います" , entry );
		return def_value;
	}

	return result_string;
}

//1行書き出し
bool mRegistry::SetString( const WString& entry , const WString& value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return false;
	}

	//書き込み
	DWORD write_size = (DWORD)( sizeof( wchar_t ) * ( value.size() + 1 ) );	//+1はNULL分
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_SZ , (LPCBYTE)value.c_str() , write_size ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリへの書き込みが失敗しました" , entry );
		return false;
	}
	return true;
}

//複数行読み取り
bool mRegistry::GetMultiString( const WString& entry , WStringVector& ret_value )
{
	ret_value.clear();

	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return false;
	}

	//文字列読み取り
	DWORD data_type;
	WString result_string;
	if( !ReadInternal( MyKey , entry , result_string , data_type ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリのデータを読み取れません" , entry );
		return false;
	}
	//値の種別が文字列であるかを確認する
	if( data_type != REG_MULTI_SZ )
	{
		//文字列ではない
		RaiseError( g_ErrorLogger , 0 , L"レジストリの型が違います" , entry );
		return false;
	}

	//結果をパースする
	ReadDoubleNullString( result_string , ret_value );
	return true;
}

//複数行書き出し
bool mRegistry::SetMultiString( const WString& entry , const WStringVector& value )
{
	//キーが開いていない場合はエラー
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリキーが開かれていません" , entry );
		return false;
	}

	WString str;
	MakeDoubleNullString( value , str );

	//書き込み
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_MULTI_SZ , (LPCBYTE)str.data() , (DWORD)( str.size() * sizeof( wchar_t ) ) ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリへの書き込みが失敗しました" , entry );
		return false;
	}
	return true;
}

//エントリの削除
bool mRegistry::DeleteEntry( const WString& entry )
{
	if( RegDeleteValue( MyKey , entry.c_str() ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"レジストリエントリの削除が失敗しました" , entry );
		return false;
	}
	return true;
}

bool mRegistry::EnumValues( WStringDeque& retValues )
{
	DWORD index = 0;
	bool result = true;

	wchar_t buffer[ 256 ];
	wchar_t* dynamic_buffer = nullptr;
	wchar_t* buffer_ptr = buffer;
	DWORD buffer_size = (DWORD)array_count_of( buffer );

	retValues.clear();

	while( 1 )
	{
		DWORD result_size = buffer_size;
		LSTATUS rc = RegEnumValueW( MyKey , index , buffer_ptr , &result_size , nullptr , nullptr , nullptr , nullptr );

		if( rc == ERROR_SUCCESS )
		{
			retValues.push_back( buffer_ptr );
		}
		else if( rc == ERROR_NO_MORE_ITEMS )
		{
			break;
		}
		else if( rc == ERROR_MORE_DATA )
		{
			if( dynamic_buffer )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"レジストリキーの列挙でバッファが不足しました" );
				result = false;
				break;
			}
			buffer_size = 32767;
			dynamic_buffer = mNew wchar_t[ buffer_size ];	//レジストリキーの長さ制限最大
			buffer_ptr = dynamic_buffer;
			continue;
		}
		else
		{
			result = false;
			break;
		}
		index++;
	}

	mDelete[] dynamic_buffer;
	return result;
}





