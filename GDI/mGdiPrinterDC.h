//----------------------------------------------------------------------------
// ウインドウ管理（プリンター用デバイスコンテキスト）
// Copyright (C) 2019 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

//●用途
//GDIを経由して印刷を行います
//以下サンプル
#if 0
#include "mStandard.h"
#include "General/mTCHAR.h"
#include "Device/mPrinterEnumerar.h"
#include "GDI/mGdiPrinterDC.h"

//コマンドラインオプション
mOptionParser MyOption;

int main( int argc , char** argv )
{
	InitializeLibrary();

	//プリンター情報を取得する
	mPrinterEnumerar printers;				//プリンター列挙クラス
	mPrinterEnumerar::PrinterInfo info;		//プリンター一覧
	printers.GetPrinterInfo( info );

	//デフォルトプリンターの名前を取得する
	WString defprinter;	//デフォルトプリンターの名前
	printers.GetDefaultPrinterName( defprinter );

	//選択したプリンターの情報を取得する
	mPrinterEnumerar::PrinterProperty prop;	//プリンター情報
	printers.GetPrinterProperty( defprinter , prop );
		 
	//印刷する
	mGdiPrinterDC printer;
	mGdiPrinterDC::Option_UseOption opt;
	opt.DocumentName = L"TestPage";			//ドキュメント名
	opt.PrinterName = defprinter;			//ここでは、先に取得したデフォルトプリンターにする

	mGdiPrinterDC::PageOption_StandardSize pageopt;
	pageopt.Paper = DMPAPER_A4;				//直接A4を指定(本来はmPrinterEnumerar::PrinterPropertyの値からコピーすべき)

	//プリンタをオープンして、1枚目の描画を開始
	printer.Open( opt );
	printer.StartPage( pageopt );

	//座標変換の設定をする
	SIZE sz;
	//＜紙の左上を原点にして描画する場合＞
	// ○ 紙の端からの位置に対して描画できる
	// × 印刷可能範囲の端が座標0にならない
	printer.GetPaperSize( sz );
	printer.SetView( mGdiPrinterDC::Origin::ORIGIN_PAPEREDGE , sz );
	//↑GetPaperSize()は1/10mm単位で結果を返すので、
	//  A4(210×297mm)ならば、(2100,2970)が返る。
	//  これをそのままSetViewに渡しているので、紙全体を(0,0)-(2099,2969)のビットマップのように描画できる設定になる。
	//  つまり、1ピクセル = 1/10mm になる。

	//＜印刷可能範囲の左上を原点にして描画する場合＞
	// ○ (0,0)の位置から座標計算できるので、分かりやすい
	// × プリンタによって印刷位置が微妙に変わる
//	printer.GetPrintableSize( sz );
//	printer.SetView( mGdiPrinterDC::Origin::ORIGIN_PRINTABLEAREA , sz );

	//描画する
	RECT rect;
	printer.Select( GetStockObject( BLACK_PEN ) );
	printer.GetPrintableArea( rect );	//印刷可能範囲を取得
	printer.Rectangle( rect.left , rect.top , rect.right , rect.bottom );	//印刷可能範囲いっぱいに長方形を描く
	printer.Rectangle( 0 , 0 , 100 , 100 );	//座標を指定して長方形を描く
	printer.Print( L"Test" , 200 , 200 );	//座標を指定して"Test"と文字を書く

	//ドキュメント終了
	printer.EndPage();
	printer.Close();	//Close()により実際のプリントがはじまる

	return 0;
}
#endif

#ifndef MGDIPRINTERDC_H_INCLUDED
#define MGDIPRINTERDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"
#include <memory>
#include "../Device/mPrinterInfo.h"

namespace Definitions_mGdiPrinterDC
{
	//原点位置指定
	enum Origin
	{
		ORIGIN_PAPEREDGE,		//原点を紙の左上に指定します
		ORIGIN_PRINTABLEAREA,	//原点を印刷可能範囲の左上に指定します
	};

	//用紙の向き
	enum PaperOrientation
	{
		PAPER_PORTRAIT,			//縦長
		PAPER_LANDSCAPE			//横長
	};

	//両面印刷
	enum PaperDuplex
	{
		PAPER_SIMPLEX,				//片面印刷
		PAPER_DUPLEX_HORIZONTAL,	//長辺綴じ
		PAPER_DUPLEX_VERTICAL,		//短辺綴じ
	};
};

class mGdiPrinterDC : public mGdiDC
{
public:

	//オプション構造体(ドキュメント)
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		enum CreateMethod
		{
			USEOPTION,
		};
		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。

		//ドキュメント名
		//・スプーラに表示される今回ジョブの名前
		WString DocumentName;

		//プリンタ名
		//・印刷先のプリンタの名前
		WString PrinterName;

		//出力ファイル
		//・仮想プリンタ(Microsoft Print to PDF等)で出力ファイル名を指定
		WString OutFileName;

		//印刷可能範囲に対するマージン(デバイス単位)
		//・デバイスから得られる印刷可能範囲ぴったりに印刷すると端数やらなんやらで切れる可能性がある
		//・ここに値を指定すると、デバイスから得られた印刷可能範囲に対して指定量のマージンを設定する
		DWORD PrintAreaMargin;

	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
			PrintAreaMargin = 10;
		}
	};

	struct Option_UseOption : public Option
	{
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
		}
	};

	//オプション構造体(ページ)
	struct PageOption
	{
		enum CreateMethod
		{
			STANDARD_SIZE,		//規格サイズの用紙
			NOCHANGE,			//前ページと同じ設定
		};
		const CreateMethod method;	//RTTIの代用

	protected:
		PageOption() = delete;
		PageOption( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//前と同じ設定によるプリント
	struct PageOption_Nochange : public PageOption
	{
		PageOption_Nochange() : PageOption( CreateMethod::NOCHANGE )
		{
		}
	};

	//規格化された用紙サイズによるプリント
	struct PageOption_StandardSize : public PageOption
	{
		using PaperId = mPrinterInfo::PaperId;
		using PaperOrientation = Definitions_mGdiPrinterDC::PaperOrientation;
		using PaperDuplex = Definitions_mGdiPrinterDC::PaperDuplex;

		//用紙ID
		//使用できる値は、mPrinterEnumerarクラスを使って、
		//mPrinterInfo::PrinterProperty::PaperInfo::PaperIdの値を取得すればわかる。
		//また、実際の値はwingdi.hで定義されている"DMPAPER_"で始まるマクロの値なので、直接指定してもよい。
		PaperId Paper;

		//用紙の向き
		PaperOrientation Orientation;

		//カラー印刷なら真
		//falseにすると、カラープリンタでもモノクロ印刷になります。
		bool IsColor;

		//両面印刷設定
		PaperDuplex Duplex;

		PageOption_StandardSize() : PageOption( CreateMethod::STANDARD_SIZE )
		{
			IsColor = false;
			Paper = DMPAPER_A4;
			Orientation = PaperOrientation::PAPER_PORTRAIT;
			Duplex = PaperDuplex::PAPER_SIMPLEX;
		}
	};

	//コンストラクタ
	mGdiPrinterDC() noexcept;
	virtual ~mGdiPrinterDC();

	//印刷を開始する
	// opt : 印刷オプション
	// ret : 成功時真
	bool Open( const Option& opt ) noexcept;

	//印刷を完了する
	//・このメソッドの実行により実際の印刷作業が開始されます
	//ret : 成功時真
	bool Close( void ) noexcept;

	//印刷をキャンセルする
	//・Open()をしてからClose()するまでの間にキャンセルする場合コール
	//ret : 成功時真
	bool Abort( void ) noexcept;

	//次ページを開始する
	//・最初のページでも呼び出しが必要
	//・GDIオブジェクト類(ブラシ等)の選択状況はリセットされるので再設定が必要
	// opt : 印刷オプション
	// ret : 成功時真
	bool StartPage( const PageOption& opt ) noexcept;

	//ページを完了する
	//・最後のページでも呼び出しが必要
	// ret : 成功時真
	bool EndPage( void ) noexcept;

	//用紙のサイズを得る(0.1ミリ単位 / 1cm = 100)
	//retSize : 得られた用紙サイズ
	//ret : 成功時真
	//・StartPage()実行前は、前のページの情報が得られる
	//・端数が出ることがあります(内部でインチ・mmの変換が行われるため)
	bool GetPaperSize( SIZE& retSize ) noexcept;

	//印刷可能範囲のサイズを得る(0.1ミリ単位 / 1cm = 100)
	//retSize : 得られた印刷可能サイズ
	//ret : 成功時真
	//・StartPage()実行前は、前のページの情報が得られる
	//・端数が出ることがあります(内部でインチ・mmの変換が行われるため)
	//※SetView()で指定した描画対象ではなく、
	//　プリンタの仕様と紙の種類から導かれる値になります
	bool GetPrintableSize( SIZE& retSize ) noexcept;

	//紙の上下左右の各端から印刷可能範囲までのマージンを得る(0.1ミリ単位 / 1cm = 100)
	//retSize : 得られたマージンサイズ
	//ret : 成功時真
	//・StartPage()実行前は、前のページの情報が得られる
	//・端数が出ることがあります(内部でインチ・mmの変換が行われるため)
	bool GetPrintableMargin( RECT& retMargin ) noexcept;

	//原点位置指定
	using Origin = Definitions_mGdiPrinterDC::Origin;

	//描画対象の範囲を、何ピクセル四方のスクリーンと見なすかを指定します
	//origin : 原点位置指定
	// ORIGIN_PAPEREDGE
	//	・原点を紙の左上に指定します
	//	・紙全体が描画対象の範囲となります
	//	・実際にプリンタが印刷できる範囲はGetPrintableArea()で調べます
	// ORIGIN_PRINTABLEAREA
	//	・原点を印刷可能範囲の左上に指定します
	//	・印刷可能範囲全体が描画対象の範囲となります
	//size : 何ピクセル四方のスクリーンと見なすか
	bool SetView( Origin origin , const SIZE& size ) noexcept;

	//描画対象の範囲を、何ピクセル四方のスクリーンと見なすかを指定します
	//origin : 原点位置指定
	// ORIGIN_PAPEREDGE
	//	・原点を紙の左上に指定します
	//	・紙全体が描画対象の範囲となります
	//	・実際にプリンタが印刷できる範囲はGetPrintableArea()で調べます
	// ORIGIN_PRINTABLEAREA
	//	・原点を印刷可能範囲の左上に指定します
	//	・印刷可能範囲全体が描画対象の範囲となります
	//size : 何ピクセル四方のスクリーンと見なすか
	//margin : originで指定した範囲に対して確保するマージン(0.1ミリ単位 / 1cm = 100)
	bool SetView( Origin origin , const SIZE& size , const RECT& margin ) noexcept;


private:
	mGdiPrinterDC( const mGdiPrinterDC& src ) = delete;
	mGdiPrinterDC& operator=( const mGdiPrinterDC& src ) = delete;

protected:

	using OptionPtr = std::unique_ptr< Option >;
	OptionPtr MyOption;

};

#endif //MGDIPAINTDC_H_INCLUDED
