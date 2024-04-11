//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCDESCRIPTION_H_INCLUDED
#define MODBCDESCRIPTION_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"

#include "mOdbc.h"

#include <vector>
#include <unordered_map>

#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")

struct mOdbcDescriptionEntry
{
	//サイズ
	DWORD_PTR Size;
	//サイズ
	DWORD_PTR Digit;
	//ヌルを許容する場合真
	bool Nullable;
	//固定長である場合：真
	//可変長である場合：偽
	bool Fixed;
	//メタODBC型
	DWORD MetaType;
	//推奨データ型
	mOdbc::ParameterType ParamType;
	//公式対応しているか
	bool Available;
	//パラメータ、結果セットの列番号
	INT Index;
};

//パラメータクエリのパラメータ情報を保持する
//パラメータの左側から順に配列に格納する
typedef std::vector<mOdbcDescriptionEntry> mOdbcParameterDescription;

//クエリによって得られた結果セットの情報を保持する
//左：列の名前
//右：列の情報
typedef std::unordered_map<WString,mOdbcDescriptionEntry> mOdbcResultDescription;

#endif
