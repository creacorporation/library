//----------------------------------------------------------------------------
// スラッシュ区切りデータフォーマット
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef SLASHFORMATUTILITY_H_INCLUDED
#define SLASHFORMATUTILITY_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <map>
#include "mSlashFormat.h"

/*

以下の形式のテキストファイル
<command>/<key1>=<val1>/<key2>=<val2>/..../<keyn>=<valn>

command = コマンド。::で区切って階層構造にすることができる。
key = コマンドに付随する設定値のキー。最初に発見した=の位置までがkeyになる。
val = コマンドに付随する設定値の値。最初に発見した=の位置以降がvalになる。

・いずれにも\r,\n,\0を含めることは出来ない
・keyに=を含むことは出来ない。
・command,key,valにスラッシュを含めたい場合は、//にする。

例1：
Info::Price::Current/Id=12345/Name=山田太郎/Price=200
例2：
Set::Date/Date=2020//03//26
	※日付のスラッシュが//になっていることに注目

*/

namespace mSlashFormatUtility
{

	//文字列をSlashFormat形式にパースする
	// src : パースする文字列
	// retData : パースした結果
	void Parse( const AString& src , mSlashFormatA& retData );

	//文字列をSlashFormat形式にパースする
	// src : パースする文字列
	// retData : パースした結果
	void Parse( const WString& src , mSlashFormatW& retData );

	//文字列のスラッシュを//に置換する
	WString EscapeSlash( const WString& src );

	//文字列のスラッシュを//に置換する
	AString EscapeSlash( const AString& src );

	//SlashFormatを文字列にエンコードする
	// dt : エンコード対象のデータ
	// retStr : 結果格納先
	// ret : 成功時真(偽の場合は使えない文字を含んでいる可能性が高い）
	bool Encode( const mSlashFormatA& dt , AString& retStr );

	//SlashFormatを文字列にエンコードする
	// dt : エンコード対象のデータ
	// retStr : 結果格納先
	// ret : 成功時真(偽の場合は使えない文字を含んでいる可能性が高い）
	bool Encode( const mSlashFormatW& dt , WString& retStr );

};

#endif


