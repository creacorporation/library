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
#include "General/mTCHAR.h"

class mGUID
{
public:

	mGUID();
	mGUID( const WString& src );
	mGUID( const AString& src );
	mGUID( const GUID& src );
	mGUID( const mGUID& source );
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

	//新しいGUIDを生成する
	bool Create( void );

	//GUIDを取得
	operator const GUID&( void )const;
	//GUIDを取得
	const GUID& ToGUID( void )const;
	//GUIDを取得
	operator AString( void )const;
	//GUIDを取得
	const AString ToAString( void )const;
	//GUIDを取得
	operator WString( void )const;
	//GUIDを取得
	const WString ToWString( void )const;

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
			return src.ToGUID().Data1;
		}
	};
}

#endif