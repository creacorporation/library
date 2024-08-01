//----------------------------------------------------------------------------
// プリンタ列挙クラス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MPRINTERENUMERAR_H_INCLUDED
#define MPRINTERENUMERAR_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mDeviceEnumerar.h"
#include "mPrinterInfo.h"
#include <vector>

class mPrinterEnumerar : protected mDeviceEnumerarBase
{
public:
	mPrinterEnumerar() noexcept;
	virtual ~mPrinterEnumerar();

	//デフォルトのプリンターの名前を得る
	bool GetDefaultPrinterName( WString& retName ) noexcept;

	//プリンター情報
	using PrinterInfo = mPrinterInfo::PrinterInfo;

	//システムから見えているプリンタの一覧を得る
	//retinfo : 結果の格納先
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 成功時真
	bool GetPrinterInfo( PrinterInfo& retinfo , bool reload = true ) noexcept;

	//システムから見えているプリンタの一覧を得る
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 結果への参照
	const PrinterInfo& GetPrinterInfo( bool reload = true ) noexcept;

	//プリンター設定情報
	using PrinterProperty = mPrinterInfo::PrinterProperty;

	//プリンターの設定情報を取得する
	// name : 取得したいプリンターの名前
	// retProperty : 取得した結果
	bool GetPrinterProperty( const WString& name , PrinterProperty& retProperty ) noexcept;

private:
	mPrinterEnumerar( const mPrinterEnumerar& src ) = delete;
	const mPrinterEnumerar& operator=( const mPrinterEnumerar& src ) = delete;

protected:

	bool Reload( void );
	PrinterInfo MyPrinterInfo;

};


#endif
