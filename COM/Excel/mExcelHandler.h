//----------------------------------------------------------------------------
// Microsoft Excelハンドラ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MEXCELHANDLER_H_INCLUDED
#define MEXCELHANDLER_H_INCLUDED

#ifdef _DEBUG
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#endif

#include "mRevision.h"
#ifdef LIBRARY_ENABLE_EXCEL

//以下エラー対策のため、インクルード順を、winsock2→import→mStandardの順にしてあります。
//・winsock2.hを先にインクルードしておかないとエラーになる
//・mStandard.hのGetOffsetマクロが原因でエラーになる
#include <winsock2.h>

// エクセルのタイプライブラリをインポート
#import "progid:Excel.Sheet" auto_search auto_rename rename_search_namespace("Office") \
	raw_method_prefix( "XL" ) \
	exclude( "IFont","IPicture") \
	no_dual_interfaces

//上記のインポート方法だと、インテリセンスが動かないので、明示的にインクルードする
//※インクルードディレクトリに、中間ファイル出力先(タイプライブラリの出力先)を追加しておく必要あり
#include <Excel.tlh>

// 以下クラス定義
#include <mStandard.h>
#include <COM/mComObject.h>
#include <COM/mVariant.h>
#include "mExcelCellRef.h"

namespace Definitions_mExcelHandler
{
	//検索範囲
	enum SearchAreaEntry
	{
		Worksheet,	//ワークシート
		Range		//指定範囲
	};

	//どの部分と一致？
	enum MatchTypeEntry
	{
		Comment,	//コメントと一致
		Formula,	//数式と一致
		Value,		//値と一致
	};

	//検索方向
	enum SearchDirectionEntry
	{
		BYCOLUMN,		//一番左の列から順に上から下に検索
		BYROW			//一番上の行から順に左から右に検索
	};
		
	//読み取り方向
	enum ScanOrder
	{
		//上の行から順に、横方向(左から右)にスキャン
		//[1] 1→2→3
		//[2] 4→5→6
		//[3] 7→8→9
		SCANORDER_ROW_MAJOR,

		//左の列から順に、縦方向(上から下)にスキャン
		//[1] [2] [3]
		//1↓ 4↓ 7↓
		//2↓ 5↓ 8↓
		//3↓ 6↓ 9↓ 
		SCANORDER_COL_MAJOR,

		//上の行から順に、横方向(右から左)にスキャン
		//※右下のセルの位置に注意し、巨大なループを作らないようにしてください
		//[1] 3←2←1
		//[2] 6←5←4
		//[3] 9←8←7
		SCANORDER_ROW_REVERSE,

		//左の列から順に、縦方向(下から上)にスキャン
		//※右下のセルの位置に注意し、巨大なループを作らないようにしてください
		//[1] [2] [3]
		//3↑ 6↑ 9↑
		//2↑ 5↑ 8↑
		//1↑ 4↑ 7↑ 
		SCANORDER_COL_REVERSE,

		//下の行から順に、横方向(左から右)にスキャン
		//※右下のセルの位置に注意し、巨大なループを作らないようにしてください
		//[3] 7→8→9
		//[2] 4→5→6
		//[1] 1→2→3
		SCANORDER_ROW_MAJOR_BOTTOMUP,

		//右の列から順に、縦方向(上から下)にスキャン
		//※右下のセルの位置に注意し、巨大なループを作らないようにしてください
		//[3] [2] [1]
		//7↓ 4↓ 1↓
		//8↓ 5↓ 2↓
		//9↓ 6↓ 3↓ 
		SCANORDER_COL_MAJOR_RIGHTTOLEFT,

		//下の行から順に、横方向(右から左)にスキャン
		//※右下のセルの位置に注意し、巨大なループを作らないようにしてください
		//[3] 9←8←7
		//[2] 6←5←4
		//[1] 3←2←1
		SCANORDER_ROW_REVERSE_BOTTOMUP,

		//右の列から順に、縦方向(下から上)にスキャン
		//※右下のセルの位置に注意し、巨大なループを作らないようにしてください
		//[3] [2] [1]
		//9↑ 6↑ 3↑
		//8↑ 5↑ 2↑
		//7↑ 4↑ 1↑ 
		SCANORDER_COL_REVERSE_RIGHTTOLEFT,

	};

	//読み取りフラグ
	enum ScanContinue
	{
		SCAN_FINISH_TRUE = 0,	//読み取りを終了し、ReadArrayはtrueを返します
		SCAN_FINISH_FALSE,		//読み取りを終了し、ReadArrayはfalseを返します
		SCAN_NEXTCELL,			//次のセルを読み取ります
		SCAN_NEXTGROUP,			//現在の行または列の読み取りは終了し、次の行または列から読み取ります
	};
};

class mExcelHandler : public mComObject
{
public:
	mExcelHandler();
	virtual ~mExcelHandler();

	//-------------------------------------------------------
	// システム
	//-------------------------------------------------------

	//初期化
	//このメソッドをコールすると、実際にExcelが起動します
	//true : 初期化成功
	//false: 初期化失敗
	bool Initialize( void );

	//初期化済みか？
	// ret : 真なら初期化済み
	bool IsInitialized( void )const;

	//初期化済みか？
	// ret : 真なら初期化済み
	operator bool()const;

	//クリーンアップ
	//このメソッドをコールすると、Excelが終了します
	//true : 成功
	//false: 失敗
	bool UnInitialize( void );

	//可視・不可視の変更
	//newstate true : 可視
	//         false: 不可視（デフォルト）
	bool SetVisible( bool newstate );

	//アラートを発生するか否かを設定する
	//※「ファイルを保存しますか」のようなメッセージ
	//newval : true : 表示する
	//         false : 表示しない
	bool SetAlertEnable( bool newval );

	//アラートを発生するか否かの状態を取得する
	//※「ファイルを保存しますか」のようなメッセージ
	//ret : true : 表示する
	//      false : 表示しない
	bool GetAlertEnable( void );

	//自動再計算モード
	typedef Excel::XlCalculation XlsCalculation;

	//自動再計算モードを設定する
	// newval : 新しい設定値
	// ret : 成功時真
	bool SetCalculation( XlsCalculation newval );

	//現在の自動再計算モードを取得する
	// ret : 現在の設定値
	XlsCalculation GetCalculation( void );

	//手動再計算を行う(全ワークブック)
	// ret : 成功時真
	bool CalcAll( void );

	//手動再計算を行う(現在のワークシート)
	// ret : 成功時真
	bool CalcWorksheet( void );

	//イベントの有効・無効設定
	//※無効にするとWorkbook_Openプロシージャ等実行されなくなります
	//newval : true : 有効にする
	//      false : 無効にする
	//ret : 成功時真
	bool SetEventEnable( bool newval );

	//イベントの有効・無効状態を取得する
	//※無効の場合Workbook_Openプロシージャ等実行されません
	//ret : true : 有効である
	//      false : 無効である
	bool SetEventEnable( void );


	//-------------------------------------------------------
	// ワークブック関係−ファイル操作系
	//-------------------------------------------------------

	//ワークブックを開くときのオプション
	struct OpenWorkbookOption
	{
		bool IsReadOnly;	//trueにすると読み取り専用で開く
		bool IsSelect;		//開いたら早速選択する

		OpenWorkbookOption()
		{
			IsReadOnly = false;
			IsSelect = false;
		}
	};

	//既存のワークブックを開く
	// filename : 開くファイル名
	// opt      : ファイルを開くときのオプション
	// ret : 成功時真
	bool OpenWorkbook( const WString& filename , const OpenWorkbookOption* opt = 0 );

	//上書き保存
	// ret : 成功時真
	bool Save( void );

	//Ｅｘｃｅｌファイルのフォーマット
	typedef Excel::XlFileFormat XlsFileFormat;

	//名前を付けて保存
	//filename : 保存するファイル名
	//format : Ｅｘｃｅｌファイルのフォーマット
	// ret : 成功時真
	bool SaveAs( const WString& filename , XlsFileFormat format );

	//閉じる
	// force : true  変更がある場合も、変更を破棄して閉じる
	//         false 変更がある場合は、ダイアログを表示してユーザに指示を求める
	// bookname : ファイル名を指定した場合、そのファイル。nullptrを指定した場合、現在のファイル。
	// ret : 成功時真
	bool Close( bool force , const WString* bookname = 0 );

	//ワークブックが更新されているかを得る
	//更新されていればtrue
	bool IsModified( void )const;

	//-------------------------------------------------------
	// ワークブック関係
	//-------------------------------------------------------

	//ワークブックの新規作成
	// ret : 成功時真
	bool AddNewWorkbook( void );

	//開いているワークブックがいくつあるかを返す
	// ret : 開いているワークブックの数
	DWORD GetWorkbookCount( void );

	//現在のワークブックを変更します
	// index : 選択するワークブックのインデックス
	// ret : 変更に成功した場合はtrue。実在しないワークブックを指定した場合など、失敗したらfalse。
	bool SetCurrentWorkbook( DWORD index );

	//現在のワークブックを変更します
	// name  : 選択するワークブックの名前
	// isFuzzyMatch : 空白の有無、全角半角、大文字小文字の違いを無視して検索する
	//                ※無視した結果、複数が一致した場合はそのなかで最初に見つけたものを採用する
	// ret : 変更に成功した場合はtrue。実在しないワークブックを指定した場合など、失敗したらfalse。
	bool SetCurrentWorkbook( const WString& name , bool isFuzzyMatch = false );

	//現在のワークブックを得る
	// ret : 現在のワークブック名。指定されていない場合は空文字列
	WString GetWorkbookName( void )const;

	//ワークブックの名前を得る
	// index : 名前を知りたいワークブックのインデックス
	// retName : indexで指定したワークブックの名前
	//  ※indexで指定したワークブックがない場合は空文字列
	WString GetWorkbookName( DWORD index );

	//ワークブックの名前を得る
	// retNames : 全てのワークブックの名前
	// ret : 成功時真
	bool GetWorkbookNames( WStringDeque& retNames );

	//-------------------------------------------------------
	// ワークシート関係
	//-------------------------------------------------------

	//ワークシートの新規作成
	// ret : 成功時真
	bool AddNewWorksheet( void );

	//現在のワークブックにシートがいくつあるかを返す
	// ret : 現在のワークブックに存在するシートの数
	DWORD GetWorksheetCount( void );

	//現在のワークシートを変更する
	// index : 選択するワークシートのインデックス(1開始)
	// ret : 変更に成功した場合はtrue。実在しないワークシートを指定した場合など、失敗したらfalse。
	bool SetCurrentWorksheet( DWORD index );

	//現在のワークシートを変更する
	// name  : 選択するワークシートの名前
	// isFuzzyMatch : 空白の有無、全角半角、大文字小文字の違いを無視して検索する
	//                ※無視した結果、複数が一致した場合はそのなかで最初に見つけたものを採用する
	// ret : 変更に成功した場合はtrue。実在しないワークシートを指定した場合など、失敗したらfalse。
	bool SetCurrentWorksheet( const WString& name , bool isFuzzyMatch = false );

	//ワークシートの名前を得る
	// ret : 現在のワークシート名。指定されていない場合は空文字列
	WString GetWorksheetName( void );

	//ワークシートの名前を得る
	// index : 名前を知りたいワークシートのインデックス(1開始)
	// ret : ワークシートの名前
	// ※indexで指定したワークシートがない場合は空文字列
	WString GetWorksheetName( DWORD index );

	//現在のワークブックにあるワークシートの名前の一覧を得る
	// retNames : 全てのワークシートの名前
	// ret : 成功時真
	bool GetWorksheetNames( WStringDeque& retNames );

	//ワークシートの現在のサイズを得る
	//ワークシートのデータが入力されている範囲を返す
	// ActiveSheet.Cells.SpecialCells(xlLastCell).Row
	// ActiveSheet.Cells.SpecialCells(xlLastCell).Columns
	//で得られる値を返します。
	// retRow : (ret)行数　値が不要ならnullptr可
	// retCol : (ret)列数　値が不要ならnullptr可
	// ret : 成功時真
	bool GetLastCell( DWORD* retRow , DWORD* retCol );

	//-------------------------------------------------------
	// 範囲関係
	//-------------------------------------------------------

	//現在のワークシートの範囲を指定する
	//range : 指定したい範囲
	// ret : 成功時真
	bool SetCurrentRange( const mExcelCellRef& range );

	//現在のワークシートの範囲を指定する
	//range : 指定したい範囲
	// ret : 成功時真
	bool SetCurrentRange( const WString& range );

	//現在の範囲の行数を得る
	//ret : 現在の範囲に含む行数
	DWORD GetRangeRow( void );

	//現在の範囲の列数を得る
	//ret : 現在の範囲に含む列数
	DWORD GetRangeCol( void );

	//結合セルかを得る
	// ret : 結合セルであれば真
	bool IsMerged( void )const;

	//結合セルの範囲を得る
	// retrange : 結合セルの範囲
	// ret : 成功時真
	bool GetMergeRange( mExcelCellRef& retrange );

	//-------------------------------------------------------
	// 検索関係
	//-------------------------------------------------------

	//Searchに渡すコールバック関数
	// dataptr : Searchに渡した値（ここに読み取り結果を格納）
	// row,col : Searchに渡した範囲の左上からのオフセット(左上のセルがrow=0,col=0)
	// ret     : 検索を続行するときtrue、終了するときfalse
	typedef bool (*fpSearchFindCallback)( void* dataptr , DWORD row , DWORD col );

	//Searchに渡す検索条件
	struct SearchOption
	{
		WString What;	//検索する文字列

		//検索範囲
		using SearchAreaEntry = Definitions_mExcelHandler::SearchAreaEntry;
		SearchAreaEntry SearchArea;

		//どの部分と一致？
		using MatchTypeEntry = Definitions_mExcelHandler::MatchTypeEntry;
		MatchTypeEntry MatchType;

		//true = 大文字と小文字を区別する
		//false= 大文字と小文字を区別しない
		bool MatchCase;

		//true = 完全一致
		//false= 部分一致
		bool MatchWhole;

		//検索方向
		using SearchDirectionEntry = Definitions_mExcelHandler::SearchDirectionEntry;
		SearchDirectionEntry SearchDirection;

	};

	//指定の範囲内を検索する
	// opt      : 検索条件
	// callback : 発見するたびに呼び出すコールバック関数
	// dataptr  : コールバック関数に、ヒットしたセルとともに引き渡すポインタ（内容は任意）
	// retCount : コールバック関数を呼び出した回数(不要ならnullptr可)
	// ret      : 正常終了時true
	bool Search( const SearchOption& opt , fpSearchFindCallback callback , void* dataptr , DWORD* retCount = nullptr );

	//発見したセルのアドレス一覧
	typedef std::deque<mExcelCellRef::Position> PositionArray;

	//指定の範囲内を検索し、発見したセルのアドレス一覧を返す
	// opt       : 検索条件
	// retPos    : 発見したセルのアドレス格納先
	// max_found : 最大いくつまで探索するか
	// ret       : 成功時真
	bool Search( const SearchOption& opt , PositionArray& retPos , DWORD max_found = MAXDWORD32 );

	//-------------------------------------------------------
	// 値読み書き系（単品Ｖｅｒ）
	//-------------------------------------------------------

	//現在のワークシートの指定セルの値を文字列で得る
	//複数セルを設定すると値が何も入らない
	// ret : 得られたセルの値
	WString GetValue( void );

	//現在のワークシートの指定セルの値をvariant型で得る
	// retResult : 指定セルの値
	// ret : 成功時真
	bool GetValue( _variant_t& retResult );

	//現在の範囲に値を設定する
	// newval : 設定したい値
	// ret : 成功時真
	bool SetValue( const _variant_t& newval );

	//-------------------------------------------------------
	// 値読み書き系（まとめてＶｅｒ）
	//-------------------------------------------------------

	//読み取り方向
	using ScanOrder = Definitions_mExcelHandler::ScanOrder;

	//読み取りフラグ
	using ScanContinue = Definitions_mExcelHandler::ScanContinue;

	//ReadArrayに渡すコールバック関数
	// value   : 各セルの値(ここに渡された値を読み取るのがこの関数の役目)
	// dataptr : ReadArrayに渡した値（ここに読み取り結果を格納）
	// row,col : ReadArrayに渡した範囲の左上からのオフセット(左上のセルがrow=0,col=0)
	// ret     : 返した値に応じて、次以降のセルのスキャンを続けるかが決まります
	//※このコールバック関数内で例外を発生しても安全です(特に注意が払われています)
	typedef ScanContinue (*fpCellReadCallback)( const _variant_t& value , void* dataptr , DWORD row , DWORD col , DWORD max_row , DWORD max_col );

	//ReadArrayに渡すコールバック関数
	// value   : 各セルの値(ここに渡された値を読み取るのがこの関数の役目)
	// dataptr : ReadArrayに渡した値（ここに読み取り結果を格納）
	// row,col : ReadArrayに渡した範囲の左上からのオフセット(左上のセルがrow=0,col=0)
	// ret     : 返した値に応じて、次以降のセルのスキャンを続けるかが決まります
	//※このコールバック関数内で例外を発生しても安全です(特に注意が払われています)
	typedef ScanContinue (*fpCellReadCallbackV)( const mVariant& value , void* dataptr , DWORD row , DWORD col , DWORD max_row , DWORD max_col );

	//arrに指定したSAFEARRAYの各セルに対してfpCellReadCallbackを呼び出す
	// arr     : SAFEARRAYを含んだvariant型の変数
	// reader  : arrの各セルに対してこのコールバック関数が呼ばれます
	//           readerが1回でもfalseを返すと、そこで打ち切られます
	// dataptr : readerに指定した関数に渡されます
	// ret     : readerが全セルに対してtrueを返したとき、true
	//           readerが1回でもfalseを返したとき、false
	//※readerに渡すコールバック関数内で例外を発生しても安全です(特に注意が払われています)
	bool ReadArray( const _variant_t& arr , fpCellReadCallback reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//arrに指定したSAFEARRAYの各セルに対してfpCellReadCallbackを呼び出す
	// arr     : SAFEARRAYを含んだvariant型の変数
	// reader  : arrの各セルに対してこのコールバック関数が呼ばれます
	//           readerが1回でもfalseを返すと、そこで打ち切られます
	// dataptr : readerに指定した関数に渡されます
	// ret     : readerが全セルに対してtrueを返したとき、true
	//           readerが1回でもfalseを返したとき、false
	//※readerに渡すコールバック関数内で例外を発生しても安全です(特に注意が払われています)
	bool ReadArray( const _variant_t& arr , fpCellReadCallbackV reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//現在選択した範囲の各セルに対してfpCellReadCallbackを呼び出す
	// reader  : arrの各セルに対してこのコールバック関数が呼ばれます
	//           readerが1回でもfalseを返すと、そこで打ち切られます
	// dataptr : readerに指定した関数に渡されます
	// ret     : readerが全セルに対してtrueを返したとき、true
	//           readerが1回でもfalseを返したとき、false
	//※readerに渡すコールバック関数内で例外を発生しても安全です(特に注意が払われています)
	bool ReadArray( fpCellReadCallback reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//現在選択した範囲の各セルに対してfpCellReadCallbackを呼び出す
	// reader  : arrの各セルに対してこのコールバック関数が呼ばれます
	//           readerが1回でもfalseを返すと、そこで打ち切られます
	// dataptr : readerに指定した関数に渡されます
	// ret     : readerが全セルに対してtrueを返したとき、true
	//           readerが1回でもfalseを返したとき、false
	//※readerに渡すコールバック関数内で例外を発生しても安全です(特に注意が払われています)
	bool ReadArray( fpCellReadCallbackV reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//-------------------------------------------------------
	// 書式
	//-------------------------------------------------------

	//指定セルの背景色をRGBQUAD値で取得する
	// ret : 該当セルの背景色。エラーの場合は0xFFFFFFFFになります。
	//       ※ARGBのAの部分が0以外かどうかで判断できます
	RGBQUAD GetBackgroundColor( void );

private:

	mExcelHandler( const mExcelHandler& src );
	const mExcelHandler& operator=( const mExcelHandler& src );

	//エクセル固有の型に名前付け
	typedef Excel::_ApplicationPtr XlsApplication;		//アプリケーション
	typedef Excel::WorkbooksPtr XlsWorkbooks;			//ワークブック
	typedef Excel::_WorkbookPtr XlsWorkbook;			//ワークブック
	typedef Excel::SheetsPtr XlsWorksheets;				//ワークシート
	typedef Excel::_WorksheetPtr XlsWorksheet;			//ワークシート
	typedef Excel::RangePtr XlsRange;					//範囲

	XlsApplication MyApplication;		//アプリケーションハンドル
	XlsWorkbook MyCurrentWorkbook;		//現在のワークブック
	XlsWorksheet MyCurrentWorksheet;	//現在のワークシート
	XlsRange MyCurrentRange;			//現在の範囲

};

#endif

#endif

