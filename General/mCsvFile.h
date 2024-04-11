//----------------------------------------------------------------------------
// CSVファイルハンドラ
// Copyright (C) 2020-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MCSVFILE_H_INCLUDED
#define MCSVFILE_H_INCLUDED

#include "mStandard.h"
#include "General/mErrorLogger.h"
#include "General/mTCHAR.h"
#include "General/mFileReadStreamBase.h"
#include <deque>

namespace mCsvFile
{
	using WRow = std::deque< WString >; 
	using WTable = std::deque< WRow >;

	using ARow = std::deque< AString >; 
	using ATable = std::deque< ARow >;

	//CSVファイルを読み取って配列に格納する
	//・読み取るときの文字コードは、retTableに指定した型に合わせる
	//filename : 読み取るファイル名
	//retTable : 格納先
	//empty_str : 読み取り結果が長方形になっていないとき（行／列数がでこぼこになっているとき）
	//　　　　　　長方形になるようにこのデータを補って整形する。nullptrなら整形しない。
	//ret : 成功時真
	bool ReadCsvFile( const WString& filename , WTable& retTable , WString* empty_str = nullptr );

	//CSVファイルを読み取って配列に格納する
	//・読み取るときの文字コードは、retTableに指定した型に合わせる
	//filename : 読み取るファイル名
	//retTable : 格納先
	//empty_str : 読み取り結果が長方形になっていないとき（行／列数がでこぼこになっているとき）
	//　　　　　　長方形になるようにこのデータを補って整形する。nullptrなら整形しない。
	//ret : 成功時真
	bool ReadCsvFile( const WString& filename , ATable& retTable , AString* empty_str = nullptr );


	//CSVファイルを読み取って配列に格納する
	//・読み取るときの文字コードは、retTableに指定した型に合わせる
	//stream : 読み取るストリーム
	//retTable : 格納先
	//empty_str : 読み取り結果が長方形になっていないとき（行／列数がでこぼこになっているとき）
	//　　　　　　長方形になるようにこのデータを補って整形する。nullptrなら整形しない。
	//ret : 成功時真
	bool ReadCsvFile( mFileReadStreamBase& stream , WTable& retTable , WString* empty_str = nullptr );

	//CSVファイルを読み取って配列に格納する
	//・読み取るときの文字コードは、retTableに指定した型に合わせる
	//stream : 読み取るストリーム
	//retTable : 格納先
	//empty_str : 読み取り結果が長方形になっていないとき（行／列数がでこぼこになっているとき）
	//　　　　　　長方形になるようにこのデータを補って整形する。nullptrなら整形しない。
	//ret : 成功時真
	bool ReadCsvFile( mFileReadStreamBase& stream , ATable& retTable , AString* empty_str = nullptr );


};


#endif
