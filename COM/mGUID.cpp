//----------------------------------------------------------------------------
// GUID
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MGUID_CPP_COMPILING
#include "mGUID.h"
#include <General/mTCHAR.h>
#include <rpcdce.h>

mGUID::mGUID()
{
	Clear();
}

mGUID::mGUID( const WString& src )
{
	Set( src );
}

mGUID::mGUID( const AString& src )
{
	Set( src );
}

mGUID::mGUID( const GUID& src )
{
	Set( src );
}

mGUID::mGUID( const mGUID& src )
{
	Set( src );
}

mGUID::mGUID( uint32_t dt1 , uint16_t dt2 , uint16_t dt3 , uint64_t dt4 )
{
	Set( dt1 , dt2 , dt3 , dt4 );
}

const mGUID& mGUID::operator=( const WString& src )
{
	Set( src );
	return *this;
}

const mGUID& mGUID::operator=( const AString& src )
{
	Set( src );
	return *this;
}

const mGUID& mGUID::operator=( const GUID& src )
{
	Set( src );
	return *this;
}

const mGUID& mGUID::operator=( const mGUID& src )
{
	Set( src );
	return *this;
}

mGUID::~mGUID()
{
}

//値を０クリアする
void mGUID::Clear( void )
{
	ZeroMemory( &MyGUID , sizeof( MyGUID ) );
}

//値を設定する
bool mGUID::Set( const WString& src )
{
	if( UuidFromStringW( (RPC_WSTR)src.c_str() , &MyGUID ) == RPC_S_OK )
	{
		return true;
	}
	return false;
}
//値を設定する
bool mGUID::Set( const AString& src )
{
	if( UuidFromStringA( (RPC_CSTR)src.c_str() , &MyGUID ) == RPC_S_OK )
	{
		return true;
	}
	return false;
}
//値を設定する
bool mGUID::Set( const GUID& src )
{
	MyGUID.Data1 = src.Data1;
	MyGUID.Data2 = src.Data2;
	MyGUID.Data3 = src.Data3;
	for( int i = 0 ; i < sizeof( MyGUID.Data4 ) ; i++ )
	{
		MyGUID.Data4[ i ] = src.Data4[ i ];
	}
	return true;
}
//値を設定する
bool mGUID::Set( const mGUID& src )
{
	MyGUID.Data1 = src.MyGUID.Data1;
	MyGUID.Data2 = src.MyGUID.Data2;
	MyGUID.Data3 = src.MyGUID.Data3;
	for( int i = 0 ; i < sizeof( MyGUID.Data4 ) ; i++ )
	{
		MyGUID.Data4[ i ] = src.MyGUID.Data4[ i ];
	}
	return true;
}
//値を設定する
bool mGUID::Set( uint32_t dt1 , uint16_t dt2 , uint16_t dt3 , uint64_t dt4 )
{
	MyGUID.Data1 = dt1;
	MyGUID.Data2 = dt2;
	MyGUID.Data3 = dt3;

	union
	{
		uint64_t u64;
		unsigned char c[ 8 ];
	} u;
	u.u64 = dt4;

	for( int i = 0 ; i < sizeof( MyGUID.Data4 ) ; i++ )
	{
		MyGUID.Data4[ i ] = u.c[ i ];
	}
	return true;
}

//新しいGUIDを生成する
bool mGUID::Create( void )
{
	switch( UuidCreate( &MyGUID ) )
	{
	case RPC_S_OK:
	case RPC_S_UUID_LOCAL_ONLY:
	case RPC_S_UUID_NO_ADDRESS:
		return true;
	default:
		break;
	}
	return false;
}

//GUIDを取得
mGUID::operator const GUID&( void )const
{
	return MyGUID;
}

//GUIDを取得
const GUID& mGUID::ToGUID( void )const
{
	return MyGUID;
}

//GUIDを取得
mGUID::operator AString( void )const
{
	return ToAString();
}

//GUIDを取得
const AString mGUID::ToAString( void )const
{
	RPC_CSTR str;
	if( UuidToStringA( &MyGUID , &str ) != RPC_S_OK )
	{
		return false;
	}
	AString result = reinterpret_cast<char*>( str );
	RpcStringFreeA( &str );

	return std::move( result );
}

//GUIDを取得
mGUID::operator WString( void )const
{
	return ToWString();
}

//GUIDを取得
const WString mGUID::ToWString( void )const
{
	RPC_WSTR str;
	if( UuidToStringW( &MyGUID , &str ) != RPC_S_OK )
	{
		return false;
	}
	WString result = reinterpret_cast<wchar_t*>( str );
	RpcStringFreeW( &str );

	return std::move( result );
}

bool mGUID::operator ==( const mGUID& src )const
{
	RPC_STATUS dummy;
	return UuidEqual( (UUID*)&MyGUID , (UUID*)&src.MyGUID , &dummy );
}

bool mGUID::operator !=( const mGUID& src )const
{
	RPC_STATUS dummy;
	return !UuidEqual( (UUID*)&MyGUID , (UUID*)&src.MyGUID , &dummy );
}

bool mGUID::operator <( const mGUID& src )const
{
	if( MyGUID.Data1 != src.MyGUID.Data1 )
	{
		return MyGUID.Data1 < src.MyGUID.Data1;
	}
	if( MyGUID.Data2 != src.MyGUID.Data2 )
	{
		return MyGUID.Data2 < src.MyGUID.Data2;
	}
	if( MyGUID.Data3 != src.MyGUID.Data3 )
	{
		return MyGUID.Data3 < src.MyGUID.Data3;
	}
	for( int i = 0 ; i < sizeof( MyGUID.Data4 ) ; i++ )
	{
		if( MyGUID.Data4 != src.MyGUID.Data4 )
		{
			return MyGUID.Data4 < src.MyGUID.Data4;
		}
	}
	return false;
}




