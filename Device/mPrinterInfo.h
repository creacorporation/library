//----------------------------------------------------------------------------
// プリンタ情報
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MPRINTERINFO_H_INCLUDED
#define MPRINTERINFO_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <vector>

namespace mPrinterInfo
{
	//システムに存在するプリンタの一覧情報
	struct PrinterInfoEntry
	{
		//プリンタの名前
		//プリンタのハンドルを開くなど、選択するときはこの文字列を使用
		WString Name;

		//プリンタの説明
		WString Description;

		//デフォルトプリンタならtrue
		bool IsDefault;
	};

	//システムに存在するプリンタの一覧情報
	using PrinterInfo = std::vector< PrinterInfoEntry >;

	//用紙の種類ID
	using PaperId = DWORD;

	//用紙の情報
	struct PaperInfoEntry
	{
		//用紙の名前（可読のもの）
		WString FriendlyName;
		//用紙ＩＤ
		//・プリンタの用紙設定でこの値を使う
		//※実態はwingdi.hで定義されている"DMPAPER_"で始まるマクロの値
		PaperId Id;
		//用紙の幅(0.1ミリ単位 / 1cm = 100)
		DWORD Width;
		//用紙の高さ(0.1ミリ単位 / 1cm = 100)
		DWORD Height;
	};

	//用紙の一覧情報
	using PaperInfo = std::vector< PaperInfoEntry >;

	//プリンタの情報
	struct PrinterProperty
	{
		//カラープリンタであれば真
		bool IsColor;
		//両面プリンターであれば真
		bool IsDuplex;
		//解像度
		DWORD Dpi;
		//受付可能な用紙の種類
		PaperInfo AcceptablePaper;
	};
};


#endif
