//----------------------------------------------------------------------------
// GUID
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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

//�l���O�N���A����
void mGUID::Clear( void )
{
	ZeroMemory( &MyGUID , sizeof( MyGUID ) );
}

//�l��ݒ肷��
bool mGUID::Set( const WString& src )
{
	if( UuidFromStringW( (RPC_WSTR)src.c_str() , &MyGUID ) == RPC_S_OK )
	{
		return true;
	}
	return false;
}
//�l��ݒ肷��
bool mGUID::Set( const AString& src )
{
	if( UuidFromStringA( (RPC_CSTR)src.c_str() , &MyGUID ) == RPC_S_OK )
	{
		return true;
	}
	return false;
}
//�l��ݒ肷��
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
//�l��ݒ肷��
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
//�l��ݒ肷��
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

//�V����GUID�𐶐�����
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

//GUID���擾
mGUID::operator const GUID&( void )const
{
	return MyGUID;
}

//GUID���擾
const GUID& mGUID::ToGUID( void )const
{
	return MyGUID;
}

//GUID���擾
mGUID::operator AString( void )const
{
	return ToAString();
}

//GUID���擾
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

//GUID���擾
mGUID::operator WString( void )const
{
	return ToWString();
}

//GUID���擾
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




