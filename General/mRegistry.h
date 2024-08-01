//----------------------------------------------------------------------------
// レジストリアクセス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MREGISTRY_H_INCLUDED
#define MREGISTRY_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <memory>
#include <setupapi.h>

//レジストリのラッパー
namespace Definitions_mRegistry
{
	enum ParentKey
	{
		CURRENT_USER,	//HKEY_CURRENT_USER
		LOCAL_MACHINE,	//HKEY_LOCAL_MACHINE
	};
};

class mRegistry
{
public:
	mRegistry();
	virtual ~mRegistry();

	using ParentKey = Definitions_mRegistry::ParentKey;

	//レジストリのパスを開く
	bool Open( ParentKey parent , const WString& path , BOOL access_write );

	//レジストリのパスを閉じる
	bool Close( void );

	//DWORD値の読み書き
	// entry : エントリの名前
	// def_value : 該当のエントリがなかった場合、その他エラーの場合に取得するデフォルトの値
	// ret : 読み取った値。エラーの場合はdef_valueに設定した値
	DWORD GetDword( const WString& entry , DWORD def_value );

	//DWORD値の読み書き
	// entry : エントリの名前
	// value : 設定する値
	// ret : 成功時真
	bool SetDword( const WString& entry , DWORD value );

	//QWORD値の読み書き
	// entry : エントリの名前
	// def_value : 該当のエントリがなかった場合、その他エラーの場合に取得するデフォルトの値
	// ret : 読み取った値。エラーの場合はdef_valueに設定した値
	ULONGLONG GetQword( const WString& entry , ULONGLONG def_value );

	//DWORD値の読み書き
	// entry : エントリの名前
	// value : 設定する値
	// ret : 成功時真
	bool SetQword( const WString& entry , ULONGLONG value );


	//WString値の読み書き
	// entry : エントリの名前
	// def_value : 該当のエントリがなかった場合、その他エラーの場合に取得するデフォルトの値
	// ret : 読み取った値。エラーの場合はdef_valueに設定した値
	WString GetString( const WString& entry , const WString& def_value );

	//WString値の読み書き
	// entry : エントリの名前
	// value : 設定する値
	// ret : 成功時真
	bool SetString( const WString& entry , const WString& value );

	//string_vector値の読み書き
	// entry : エントリの名前
	// ret_value : 読み取った値
	// ret : 成功時真
	bool GetMultiString( const WString& entry , WStringVector& ret_value );

	//string_vector値の読み書き
	// entry : エントリの名前
	// value : 設定する値
	// ret : 成功時真
	bool SetMultiString( const WString& entry , const WStringVector& value );

	//エントリの削除
	// entry : エントリの名前
	// ret : 成功時真
	bool DeleteEntry( const WString& entry );

	//レジストリの値の一覧を取得する
	bool EnumValues( WStringDeque& retValues );

private:
	mRegistry( const mRegistry& src );
	const mRegistry& operator=( const mRegistry& src );

protected:

	//レジストリのハンドル
	HKEY MyKey;

};



#endif //MREGISTRY_H_INCLUDED
