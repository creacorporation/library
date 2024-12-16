//----------------------------------------------------------------------------
// GUID
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MGUID_H_INCLUDED
#define MGUID_H_INCLUDED

#include "mStandard.h"
#include <General/mTCHAR.h>
#include <General/mDateTime.h>

class mGUID
{
public:

	enum class INIT_WITH
	{
		Normal,
		V7
	};

	mGUID();
	mGUID( const WString& src );
	mGUID( const AString& src );
	mGUID( const GUID& src );
	mGUID( const mGUID& source );
	mGUID( INIT_WITH ini );
	mGUID( uint32_t dt1 , uint16_t dt2 , uint16_t dt3 , uint64_t dt4 );

	const mGUID& operator=( const WString& src );
	const mGUID& operator=( const AString& src );
	const mGUID& operator=( const GUID& src );
	const mGUID& operator=( const mGUID& src );

	virtual ~mGUID();

	//値を０クリアする(nil GUIDにする)
	void Clear( void );

	//値を設定する
	bool Set( const WString& src );
	//値を設定する
	bool Set( const AString& src );
	//値を設定する
	bool Set( const GUID& src );
	//値を設定する
	bool Set( const mGUID& src );
	//値を設定する
	bool Set( uint32_t dt1 , uint16_t dt2 , uint16_t dt3 , uint64_t dt4 );

	//新しいGUIDを生成する(WinAPIに任せる)
	bool Create( void );
	//v7形式で新しいGUIDを生成する
	// ts : GUIDに含めるタイムスタンプ。nullptrの場合現在時刻。
	bool CreateV7( const mDateTime::Timestamp* ts = nullptr );

	//GUIDのバージョンを得る
	int Version( void )const;
	//v7形式のGUIDから時刻を読み取る
	mDateTime::Timestamp ReadTimestamp( void )const;
	//v7形式のGUIDから時刻を読み取る
	bool ReadTimestamp( mDateTime::Timestamp& retTimestamp )const;

	//GUIDを取得
	operator const GUID&( void )const;
	//GUIDを取得
	const GUID& ToGUID( void )const;
	//GUIDを取得
	operator AString( void )const;
	//GUIDを取得
	const AString ToAString( bool with_brace = false )const;
	//GUIDを取得
	operator WString( void )const;
	//GUIDを取得
	const WString ToWString( bool with_brace = false )const;

	bool operator ==( const mGUID& src )const;
	bool operator !=( const mGUID& src )const;
	bool operator <( const mGUID& src )const;

protected:
	GUID MyGUID;

};

namespace std
{
	template<>
	class hash< mGUID >
	{
	public:
		size_t operator() ( const mGUID &src ) const
		{
			return *(size_t*)( src.ToGUID().Data4 );
		}
	};
}

#endif