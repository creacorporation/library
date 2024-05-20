//----------------------------------------------------------------------------
// Microsoft Excelハンドラ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#include "mExcelHandler.h"
#include "General/mFileUtility.h"
#include "General/mErrorLogger.h"

mExcelHandler::mExcelHandler()
{
	MyApplication = nullptr;
	MyCurrentWorkbook = nullptr;
	MyCurrentWorksheet = nullptr;
	return;
}

mExcelHandler::~mExcelHandler()
{
	UnInitialize();
}

bool mExcelHandler::Initialize( void )
{
	if( MyApplication )
	{
		//二重初期化を防止する
		RaiseError( g_ErrorLogger , 0 , L"Excelはすでに起動されています" );
		return false;
	}
	if( SUCCEEDED( MyApplication.CreateInstance( L"Excel.Application" ) ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"Excelを起動しました" );
		return true;
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelの起動が失敗しました" );
		return false;
	}
}

bool mExcelHandler::UnInitialize( void )
{
	if( MyCurrentRange )
	{
		MyCurrentRange.Release();
		MyCurrentRange = nullptr;
	}

	if( MyCurrentWorksheet )
	{
		MyCurrentWorksheet.Release();
		MyCurrentWorksheet = nullptr;
	}

	if( MyCurrentWorkbook )
	{
		MyCurrentWorkbook.Release();
		MyCurrentWorkbook = nullptr;
	}

	if( MyApplication )
	{
		MyApplication->Quit();
		MyApplication.Release();
		MyApplication = nullptr;
		CreateLogEntry( g_ErrorLogger , 0 , L"Excelを終了しました" );
	}

	return true;
}

//初期化済みか？
bool mExcelHandler::IsInitialized( void )const
{
	return *this;
}

//初期化済みか？
mExcelHandler::operator bool()const
{
	return MyApplication != nullptr;
}

bool mExcelHandler::SetVisible( bool newstate )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}
	MyApplication->PutVisible( newstate );
	return true;
}

bool mExcelHandler::AddNewWorkbook( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}
	MyApplication->Workbooks->Add();
	return true;
}

bool mExcelHandler::OpenWorkbook( const WString& filename , const mExcelHandler::OpenWorkbookOption* opt )
{
	/*
	※Openメソッドの引数

	http://msdn.microsoft.com/ja-jp/library/microsoft.office.interop.excel.workbooks.open%28VS.80%29.aspx

	_WorkbookPtr Open (
        _bstr_t Filename,
        const _variant_t & UpdateLinks = vtMissing,
        const _variant_t & ReadOnly = vtMissing,
        const _variant_t & Format = vtMissing,
        const _variant_t & Password = vtMissing,
        const _variant_t & WriteResPassword = vtMissing,
        const _variant_t & IgnoreReadOnlyRecommended = vtMissing,
        const _variant_t & Origin = vtMissing,
        const _variant_t & Delimiter = vtMissing,
        const _variant_t & Editable = vtMissing,
        const _variant_t & Notify = vtMissing,
        const _variant_t & Converter = vtMissing,
        const _variant_t & AddToMru = vtMissing,
        const _variant_t & Local = vtMissing,
        const _variant_t & CorruptLoad = vtMissing );
	*/

	//アプリが起動していなければエラー
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	//オプションの読み取り
	_variant_t is_read_only;	//リードオンリー？
	bool is_select;
	if( opt )
	{
		is_read_only = ( opt->IsReadOnly ) ? ( VARIANT_TRUE ) : ( VARIANT_FALSE );
		is_select = opt->IsSelect;
	}
	else
	{
		is_read_only = VARIANT_FALSE;
		is_select = false;
	}

	//開く処理
	XlsWorkbooks wbs = MyApplication->Workbooks;
	bool result = true;
	try
	{
		result = wbs->Open( filename.c_str() , vtMissing , is_read_only );

		if( result && is_select )
		{
			WString excelpath;
			mFileUtility::RebuildPath( filename , excelpath , false , false , true , true );
			result = SetCurrentWorkbook( excelpath );
		}
	}
	catch( ... )
	{
		RaiseError( g_ErrorLogger , 0 , L"ファイルを開けませんでした" , filename );
		result = false;
	}

	return result;
}


DWORD mExcelHandler::GetWorkbookCount( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return 0;
	}
	return (DWORD)( MyApplication->GetWorkbooks()->Count );
}

bool mExcelHandler::SetCurrentWorkbook( DWORD index )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	if( index == 0 || GetWorkbookCount() < index )
	{
		//インデックス値不正
		RaiseError( g_ErrorLogger , 0 , L"インデックスが不正です" , index );
		return false;
	}

	MyCurrentWorkbook = MyApplication->Workbooks->GetItem( _variant_t( index ) );
	return true;
}

static WString MakeFuzzyName( const WString& in )
{
	WString tmp = RemoveSpace( in );
	return ConvertString( tmp , LCMAP_FULLWIDTH | LCMAP_HIRAGANA | LCMAP_UPPERCASE );
}

bool mExcelHandler::SetCurrentWorkbook( const WString& name , bool isFuzzyMatch )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	WString tmp_search_name = ( !isFuzzyMatch ) ? ( name ) : ( MakeFuzzyName( name ) );

	//ワークブックの指定が名前出来た場合、
	//全ワークブックから一致する名前を捜し、一致したものをカレントにする。

	DWORD count = GetWorkbookCount();
	XlsWorkbooks wbs = MyApplication->Workbooks;
	for( DWORD i = 1 ; i <= count ; i++ )
	{
		XlsWorkbook wb = wbs->GetItem( _variant_t( i ) );

		_bstr_t wbname = wb->Name;
		if( !isFuzzyMatch )
		{
			if( name.compare( (wchar_t*)wbname ) == 0 )
			{
				//最初に名前が一致したものにする
				MyCurrentWorkbook = wbs->GetItem( _variant_t( i ) );
				return true;
			}
		}
		else
		{
			WString tmp_book_name = MakeFuzzyName( (wchar_t*)wbname );
			if( tmp_search_name == tmp_book_name )
			{
				//最初に名前が一致したものにする
				MyCurrentWorkbook = wbs->GetItem( _variant_t( i ) );
				return true;
			}
		}
	}
	RaiseError( g_ErrorLogger , 0 , L"指定のワークブックはありません" , name );
	return false;

}

WString mExcelHandler::GetWorkbookName( void )const
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return L"";
	}

	return (const wchar_t*)MyCurrentWorkbook->Name;
}

WString mExcelHandler::GetWorkbookName( DWORD index )
{
	DWORD count = GetWorkbookCount();
	if( index == 0 || count < index )
	{
		RaiseError( g_ErrorLogger , 0 , L"指定のワークブックはありません" , index );
		return L"";
	}

	//MyApplication==0だとGetWorkbookCountが0を返すので、ここでのヌルチェックはなし。
	XlsWorkbook wb = MyApplication->Workbooks->GetItem( _variant_t( index ) );
	return (const wchar_t*)wb->Name;
}

bool mExcelHandler::GetWorkbookNames( WStringDeque& retNames )
{
	//既存のデータを破棄
	retNames.clear();

	DWORD count = GetWorkbookCount();
	for( DWORD i = 1 ; i <= count ; i++ )
	{
		XlsWorkbook wb = MyApplication->Workbooks->GetItem( _variant_t( i ) );
		_bstr_t wbname = wb->Name;
		retNames.push_back( (wchar_t*)wbname );
	}

	return true;
}

DWORD mExcelHandler::GetWorksheetCount( void )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return 0;
	}
	return (DWORD)( MyCurrentWorkbook->Sheets->Count );
}


bool mExcelHandler::SetCurrentWorksheet( DWORD index )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return false;
	}

	if( index == 0 || GetWorksheetCount() < index )
	{
		//インデックス値不正
		RaiseError( g_ErrorLogger , 0 , L"指定のワークブックはありません" , index );
		return false;
	}

	MyCurrentWorksheet = MyCurrentWorkbook->Worksheets->GetItem( _variant_t( index ) );
	return true;

}


bool mExcelHandler::SetCurrentWorksheet( const WString& name , bool isFuzzyMatch )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return false;
	}

	WString tmp_search_name = ( !isFuzzyMatch ) ? ( name ) : ( MakeFuzzyName( name ) );

	DWORD count = GetWorksheetCount();
	XlsWorksheets ws = MyCurrentWorkbook->Worksheets;
	for( DWORD i = 1 ; i <= count ; i++ )
	{
		XlsWorksheet wsptr = ws->GetItem( _variant_t( i ) );

		_bstr_t wsname = wsptr->Name;
		if( !isFuzzyMatch )
		{
			if( name.compare( (wchar_t*)wsname ) == 0 )
			{
				//最初に名前が一致したものにする
				MyCurrentWorksheet = ws->GetItem( _variant_t( i ) );
				return true;
			}
		}
		else
		{
			WString tmp_sheet_name = MakeFuzzyName( (wchar_t*)wsname );
			if( tmp_search_name == tmp_sheet_name )
			{
				//最初に名前が一致したものにする
				MyCurrentWorksheet = ws->GetItem( _variant_t( i ) );
				return true;
			}
		}
	}
	RaiseError( g_ErrorLogger , 0 , L"指定のワークブックはありません" , name );
	return false;
}

WString mExcelHandler::GetWorksheetName( DWORD index )
{

	DWORD count = GetWorksheetCount();
	if( index == 0 || count < index )
	{
		RaiseError( g_ErrorLogger , 0 , L"指定のワークシートはありません" , index );
		return L"";
	}

	XlsWorksheet wsptr = MyCurrentWorkbook->Worksheets->GetItem( _variant_t( index ) );

	return (const wchar_t*)wsptr->Name;
}

WString mExcelHandler::GetWorksheetName( void )
{
	if( MyCurrentWorksheet == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークシートは指定されていません" );
		return L"";
	}

	return (const wchar_t*)MyCurrentWorksheet->Name;
}

bool mExcelHandler::GetWorksheetNames( WStringDeque& retNames )
{
	retNames.clear();

	DWORD count = GetWorksheetCount();
	for( DWORD i = 1 ; i <= count ; i++ )
	{
		XlsWorksheet wsptr = MyCurrentWorkbook->Worksheets->GetItem( _variant_t( i ) );
		_bstr_t wbname = wsptr->Name;
		retNames.push_back( (wchar_t*)wbname );
	}

	return true;
}

bool mExcelHandler::GetLastCell( DWORD* retRow , DWORD* retCol )
{
	if( MyCurrentWorksheet == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークシートは指定されていません" );
		return false;
	}

	if( retRow )
	{
		*retRow = MyCurrentWorksheet->Cells->SpecialCells( Excel::XlCellType::xlCellTypeLastCell )->Row;
	}
	if( retCol )
	{
		*retCol = MyCurrentWorksheet->Cells->SpecialCells( Excel::XlCellType::xlCellTypeLastCell )->Column;
	}
	return true;
}


bool mExcelHandler::SetCurrentRange( const WString& range )
{
	if( MyCurrentWorksheet == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークシートは指定されていません" );
		return false;
	}

	try
	{
		MyCurrentRange = MyCurrentWorksheet->GetRange( range.c_str() );
	}
	catch( _com_error e )
	{
		RaiseError( g_ErrorLogger , 0 , L"範囲指定が失敗しました" , TString2WString( e.ErrorMessage() ) );
		return false;
	}
	return true;
}


bool mExcelHandler::SetCurrentRange( const mExcelCellRef& range )
{
	return SetCurrentRange( range.GetAddress( false ) );
}

bool mExcelHandler::SetValue( const _variant_t& newval )
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
		return false;
	}

	MyCurrentRange->Value2 = newval;
	return true;

}

WString mExcelHandler::GetValue( void )
{
	_variant_t dat;
	if( !GetValue( dat ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"値の取得が失敗しました" );
		return TEXT( "" );
	}

	dat.ChangeType( VT_BSTR );
	return (wchar_t*)dat.bstrVal;
}

bool mExcelHandler::GetValue( _variant_t& retResult )
{

	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
		return false;
	}

	try
	{
		retResult = MyCurrentRange->Value2;
	}
	catch( _com_error e )
	{
		RaiseError( g_ErrorLogger , 0 , L"値の取得が失敗しました" , TString2WString( e.ErrorMessage() ) );
		return false;
	}
	return true;
}

bool mExcelHandler::Search( const SearchOption& opt , fpSearchFindCallback callback , void* dataptr , DWORD* retCount )
{

	//何と一致？
	Excel::XlFindLookIn lookin;
	switch( opt.MatchType )
	{
	case SearchOption::MatchTypeEntry::Value:	//値と
		lookin = Excel::XlFindLookIn::xlValues;
		break;
	case SearchOption::MatchTypeEntry::Comment:	//コメントと
		lookin = Excel::XlFindLookIn::xlComments;
		break;
	case SearchOption::MatchTypeEntry::Formula:	//数式と
		lookin = Excel::XlFindLookIn::xlFormulas;
		break;
	default:					//変な値
		RaiseAssert( g_ErrorLogger , 0 , L"検索の一致対象が不正です" , opt.MatchType );
		return false;		
	}

	//全体一致？部分一致？
	Excel::XlLookAt lookat = ( opt.MatchWhole ) ? ( Excel::XlLookAt::xlWhole ) : ( Excel::XlLookAt::xlPart );

	//検索方向は？
	Excel::XlSearchOrder order;
	switch( opt.SearchDirection )
	{
	case SearchOption::SearchDirectionEntry::BYCOLUMN:
		order = Excel::XlSearchOrder::xlByColumns;
		break;
	case SearchOption::SearchDirectionEntry::BYROW:
		order = Excel::XlSearchOrder::xlByRows;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"検索方向が不正です" , opt.SearchDirection );
		return false;
	}

	//検索開始
	DWORD count = 0;		//発見した数
	XlsRange search_range;	//サーチ範囲
	XlsRange found;			//発見したセル

	//最初の1個を見つける
	switch( opt.SearchArea )
	{
	case SearchOption::SearchAreaEntry::Range:
		if( MyCurrentRange == nullptr )
		{
			//検索範囲が指定されていないと発見するはずがないので０
			RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
			return false;
		}
		search_range = MyCurrentRange;
		break;
	case SearchOption::SearchAreaEntry::Worksheet:
		if( MyCurrentWorksheet == nullptr )
		{
			RaiseError( g_ErrorLogger , 0 , L"現在のワークシートは指定されていません" );
			return false;
		}
		search_range = MyCurrentWorksheet->Cells;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"検索範囲が不正です" , opt.SearchArea );
		return false;
	}

	//検索！
	found = search_range->Find( opt.What.c_str() , vtMissing , lookin , lookat , order , Excel::XlSearchDirection::xlNext , opt.MatchCase );
	if( found != 0 )
	{
		//該当セルあり
		INT first_row = found->Row;			//最初に発見したセルの行
		INT first_col = found->Column;		//最初に発見したセルの列
		INT row = first_row;				//現在注目しているセルの行
		INT col = first_col;				//現在注目しているセルの列
		_variant_t after;					//次の検索開始位置

		//2個目以降の検索
		do
		{
			count++;
			//コールバック関数の呼び出し
			if( callback )
			{
				if( !callback( dataptr , row , col ) )
				{
					break;	//もう不要の場合
				}
			}

			//次を検索
			after = _variant_t( found.GetInterfacePtr() );
			found = search_range->FindNext( after );
			row = found->Row;
			col = found->Column;

		}while( first_row != row || first_col != col );
	}

	if( retCount )
	{
		*retCount = count;
	}
	return true;
}

struct SearchCellAddr
{
	mExcelHandler::PositionArray* Pos;
	DWORD MaxFound;
	DWORD CurrentFound;
};

static bool SearchCellAddrCallback( void* dataptr , DWORD row , DWORD col )
{
	SearchCellAddr* data = reinterpret_cast<SearchCellAddr*>( dataptr );
	if( data == nullptr || data->Pos == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"セル名探索データがヌルです" );
		return false;
	}

	//取得したアドレスを登録
	data->Pos->push_back( mExcelCellRef::Position( false , false , true , true , row , col ) );

	//ヒット数カウント
	data->CurrentFound++;
	if( data->CurrentFound == data->MaxFound )
	{
		//最大到達
		return false;
	}
	return true;
}

//指定の範囲内を検索し、発見したセルの名称の一覧を返す
bool mExcelHandler::Search( const SearchOption& opt , PositionArray& retAddr , DWORD max_found )
{
	retAddr.clear();

	SearchCellAddr data;
	data.Pos = &retAddr;
	data.CurrentFound = 0;
	data.MaxFound = max_found;

	return Search( opt , SearchCellAddrCallback , reinterpret_cast<void*>( &data ) , nullptr );
}

DWORD mExcelHandler::GetRangeRow( void )
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
		return false;
	}

	return (DWORD)MyCurrentRange->Rows->Count;

}

DWORD mExcelHandler::GetRangeCol( void )
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
		return false;
	}
	return (DWORD)MyCurrentRange->Columns->Count;
}

//マージされたセルかを得る
bool mExcelHandler::IsMerged( void )const
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
		return false;
	}
	return MyCurrentRange->MergeCells;
}

//マージされたセルの範囲を得る
bool mExcelHandler::GetMergeRange( mExcelCellRef& retrange )
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在の範囲は指定されていません" );
		return false;
	}
	Excel::RangePtr ptr = MyCurrentRange->MergeArea;
	WString str = ptr->GetAddress( false , false , Excel::XlReferenceStyle::xlA1 );

	mExcelCellRef::Position pos1;
	mExcelCellRef::Position pos2;
	pos1.Row = ptr->Row;
	pos1.Col = ptr->Column;
	pos2.Row = pos1.Row + ptr->Rows->Count;
	pos2.Col = pos1.Col + ptr->Columns->Count;

	retrange.SetPosition( pos1 , pos2 );
	return true;
}


bool mExcelHandler::Save( void )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return false;
	}

	if( SUCCEEDED( MyCurrentWorkbook->Save() ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"ワークブックを上書きセーブしました" );
		return true;
	}
	RaiseError( g_ErrorLogger , 0 , L"ワークブックを上書きセーブできませんでした" );
	return false;
}

bool mExcelHandler::SaveAs( const WString& filename , mExcelHandler::XlsFileFormat format )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return false;
	}

	if( SUCCEEDED( MyCurrentWorkbook->SaveAs( filename.c_str() , (long)format , vtMissing , vtMissing , vtMissing , vtMissing , Excel::xlExclusive ) ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"ワークブックをセーブしました" , filename );
		return true;
	}
	RaiseError( g_ErrorLogger , 0 , L"ワークブックをセーブできませんでした" , filename );
	return false;

}

bool mExcelHandler::SetAlertEnable( bool newval )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}
	MyApplication->DisplayAlerts = newval;
	return true;
}

bool mExcelHandler::GetAlertEnable( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return true;
	}
	return MyApplication->DisplayAlerts;
}

bool mExcelHandler::SetCalculation( mExcelHandler::XlsCalculation newval )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	MyApplication->Calculation = newval;
	return true;
}


mExcelHandler::XlsCalculation mExcelHandler::GetCalculation( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return XlsCalculation::xlCalculationAutomatic;
	}
	return MyApplication->Calculation;
}

bool mExcelHandler::CalcAll( void )
{
	//全ワークブック
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	return SUCCEEDED( MyApplication->Calculate() );
}

bool mExcelHandler::CalcWorksheet( void )
{
	//現在のワークシート
	if( MyCurrentWorksheet == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return false;
	}

	return SUCCEEDED( MyCurrentWorksheet->Calculate() );
}

//イベントの有効・無効設定
bool mExcelHandler::SetEventEnable( bool newval )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	MyApplication->EnableEvents = newval;
	return true;

}

//イベントの有効・無効状態を取得する
bool mExcelHandler::SetEventEnable( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	return MyApplication->EnableEvents;
}

bool mExcelHandler::Close( bool force , const WString* bookname )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	if( bookname )
	{
		WString workbook_name;
		if( !mFileUtility::RebuildPath( bookname->c_str() , workbook_name , false , false , true , true ) )
		{
			workbook_name = *bookname;
		}
		if( !SetCurrentWorkbook( workbook_name ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"指定のワークブックはありません" , *bookname );
			return false;
		}
	}
	else
	{
		if( MyCurrentWorkbook == nullptr )
		{
			return true;
		}
	}
	try
	{
		if( FAILED( MyCurrentWorkbook->Close( !force ) ) )
		{
			throw;
		}
		MyCurrentWorkbook = nullptr;
	}
	catch( ... )
	{
		//・閉じるのに失敗し、上のthrowが実行された場合
		//・ユーザーがExcelを閉じるなどして、COMオブジェクトが無効の場合
		RaiseError( g_ErrorLogger , 0 , L"ワークブックを閉じることが出来ませんでした" , force );
		return false;
	}

	return true;
}

bool mExcelHandler::IsModified( void )const
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"現在のワークブックは指定されていません" );
		return false;
	}

	return MyCurrentWorkbook->Saved;

}

bool mExcelHandler::AddNewWorksheet( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excelは起動していません" );
		return false;
	}

	MyApplication->Worksheets->Add();
	return true;
}

bool mExcelHandler::ReadArray( const _variant_t& arr , fpCellReadCallback reader , void* dataptr , ScanOrder order )
{
	bool result = false;

	//アレイじゃなかったらＮＧ
	if( arr.vt != VT_SAFEARRAY && ( arr.vt & VT_ARRAY ) != VT_ARRAY )
	{
		RaiseError( g_ErrorLogger , 0 , L"指定したオブジェクトはアレイではありません" );
		return false;
	}

	//アレイをロックする
	if( FAILED( SafeArrayLock( arr.parray ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"アレイをロックできませんでした" );
		return false;
	}

	//●ここからreturnで戻っちゃダメ

	VARIANT*	var_cell;
	_variant_t	cell;

	//アレイの１次元目の要素数を取得
	using index_converter = LONG(*)( LONG , LONG , LONG );
	index_converter normal_order = []( LONG i , LONG lb , LONG ub )-> LONG
	{
		return i;
	};
	index_converter reverse_order = []( LONG i , LONG lb , LONG ub )-> LONG
	{
		return ub - ( i - lb );
	};

	LONG index[ 2 ];
	LONG ub_1;
	LONG lb_1;
	UINT dim_1;
	UINT dim_2;
	index_converter converter_1;
	index_converter converter_2;

	switch( order )
	{
	case ScanOrder::SCANORDER_ROW_MAJOR:				//上の行から順に、横方向(左から右)にスキャン
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = normal_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_COL_MAJOR:				//左の列から順に、縦方向(上から下)にスキャン
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = normal_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_ROW_REVERSE:				//上の行から順に、横方向(右から左)にスキャン
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = normal_order;
		converter_2 = reverse_order;
		break;
	case ScanOrder::SCANORDER_COL_REVERSE:				//左の列から順に、縦方向(下から上)にスキャン
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = normal_order;
		converter_2 = reverse_order;
		break;
	case ScanOrder::SCANORDER_ROW_MAJOR_BOTTOMUP:		//下の行から順に、横方向(左から右)にスキャン
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = reverse_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_COL_MAJOR_RIGHTTOLEFT:	//右の列から順に、縦方向(上から下)にスキャン
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = reverse_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_ROW_REVERSE_BOTTOMUP:		//下の行から順に、横方向(右から左)にスキャン
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = reverse_order;
		converter_2 = reverse_order;
		break;
	case ScanOrder::SCANORDER_COL_REVERSE_RIGHTTOLEFT:	//右の列から順に、縦方向(下から上)にスキャン
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = reverse_order;
		converter_2 = reverse_order;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"スキャン方向の設定が不正です" );
		goto end;
	}

	SafeArrayGetUBound( arr.parray , dim_1 , &ub_1 );
	SafeArrayGetLBound( arr.parray , dim_1 , &lb_1 );

	//それではいよいよループです
	for( LONG loop_i = lb_1 ; loop_i <= ub_1 ; loop_i++ )
	{
		LONG i = converter_1( loop_i , lb_1 , ub_1 );
		index[ dim_1 - 1 ] = i;

		//アレイの２次元目の要素数を取得
		LONG ub_2;
		LONG lb_2;
		SafeArrayGetUBound( arr.parray , dim_2 , &ub_2 );
		SafeArrayGetLBound( arr.parray , dim_2 , &lb_2 );

		for( LONG loop_j = lb_2 ; loop_j <= ub_2 ; loop_j++ )
		{
			LONG j = converter_2( loop_j , lb_2 , ub_2 );
			ScanContinue cont_flag;
			index[ dim_2 - 1 ] = j;

			//値取得
			if( FAILED( SafeArrayPtrOfIndex( arr.parray , index , (void HUGEP* FAR*)&var_cell ) ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"アレイの要素に対するポインタを取得できませんでした" );
				goto end;
			}

			//コールバック呼び出し
			cell.Attach( *var_cell );
			try
			{
				switch( order )
				{
				case ScanOrder::SCANORDER_ROW_MAJOR:				//上の行から順に、横方向(左から右)にスキャン
				case ScanOrder::SCANORDER_ROW_REVERSE:				//上の行から順に、横方向(右から左)にスキャン
				case ScanOrder::SCANORDER_ROW_MAJOR_BOTTOMUP:		//下の行から順に、横方向(左から右)にスキャン
				case ScanOrder::SCANORDER_ROW_REVERSE_BOTTOMUP:		//下の行から順に、横方向(右から左)にスキャン
					cont_flag = reader( cell , dataptr , i - lb_1 , j - lb_2 , ub_1 - lb_1 , ub_2 - lb_2 );
					break;
				case ScanOrder::SCANORDER_COL_MAJOR:				//左の列から順に、縦方向(上から下)にスキャン
				case ScanOrder::SCANORDER_COL_REVERSE:				//左の列から順に、縦方向(下から上)にスキャン
				case ScanOrder::SCANORDER_COL_MAJOR_RIGHTTOLEFT:	//右の列から順に、縦方向(上から下)にスキャン
				case ScanOrder::SCANORDER_COL_REVERSE_RIGHTTOLEFT:	//右の列から順に、縦方向(下から上)にスキャン
					cont_flag = reader( cell , dataptr , j - lb_2 , i - lb_1 , ub_2 - lb_2 , ub_1 - lb_1 );
					break;
				default:
					throw;
				}

				cell.Detach();
			}
			catch( ... )
			{
				SafeArrayUnlock( arr.parray );
				throw;
			}

			//コールバック関数が返したフラグの処理
			//次のセルにいく？
			if( cont_flag == ScanContinue::SCAN_FINISH_TRUE )
			{
				result = true;
				goto end;
			}
			else if( cont_flag == ScanContinue::SCAN_FINISH_FALSE )
			{
				result = false;
				goto end;
			}
			else if( cont_flag == ScanContinue::SCAN_NEXTCELL )
			{
				;
			}
			else if( cont_flag == ScanContinue::SCAN_NEXTGROUP )
			{
				break;
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"コールバック関数が返した値が不正です" , cont_flag );
			}
		}
	}
	result = true;

end:
	SafeArrayUnlock( arr.parray );
	return result;
}


struct ReadArrayDataCapsule
{
	//本来のコールバック関数
	mExcelHandler::fpCellReadCallbackV reader;
	//本来のデータ
	void* dataptr;
};

//mVariantに変換するコールバックプロキシ
static mExcelHandler::ScanContinue ReadArrayProxy( const _variant_t& value , void* dataptr , DWORD row , DWORD col , DWORD max_row , DWORD max_col )
{
	mVariant variant( value );
	ReadArrayDataCapsule* Cupsule = (ReadArrayDataCapsule*)dataptr;

	return Cupsule->reader( variant , Cupsule->dataptr , row , col , max_row , max_col );
}

bool mExcelHandler::ReadArray( const _variant_t& arr , fpCellReadCallbackV reader , void* dataptr , ScanOrder order )
{
	ReadArrayDataCapsule Cupsule;
	Cupsule.reader = reader;
	Cupsule.dataptr = dataptr;
	return ReadArray( arr , ReadArrayProxy , &Cupsule , order );
}

bool mExcelHandler::ReadArray( fpCellReadCallback reader , void* dataptr , ScanOrder order )
{
	if( !reader )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"コールバック関数が指定されていません" );
		return false;
	}

	_variant_t val;
	if( !GetValue( val ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"値を取得できませんでした" );
		return false;
	}
	if( val.vt == VT_SAFEARRAY )	//２次元（以上の）配列
	{
		return ReadArray( val , reader , dataptr , order );
	}
	else if( ( val.vt & VT_ARRAY ) == VT_ARRAY )	//１次元配列
	{
		return ReadArray( val , reader , dataptr , order );

	}
	else	//単発データ
	{
		if( reader( val , dataptr , 0 , 0 , 0 , 0 ) == ScanContinue::SCAN_FINISH_FALSE )
		{
			return false;
		}
		return true;
	}
}

bool mExcelHandler::ReadArray( fpCellReadCallbackV reader , void* dataptr , ScanOrder order )
{
	ReadArrayDataCapsule Cupsule;
	Cupsule.reader = reader;
	Cupsule.dataptr = dataptr;
	return ReadArray( ReadArrayProxy , &Cupsule , order );
}

RGBQUAD mExcelHandler::GetBackgroundColor( void )
{
	RGBQUAD result;
	result.rgbRed = 0xFF;
	result.rgbGreen = 0xFF;
	result.rgbBlue = 0xFF;
	result.rgbReserved = 0;

	try
	{
		_variant_t col = MyCurrentRange->Interior->Color;
		col.ChangeType( VT_UI4 );

		uint32_t val = col;
		result.rgbBlue		= ( val >>  0 ) & 0xFFu;
		result.rgbGreen		= ( val >>  8 ) & 0xFFu;
		result.rgbRed		= ( val >> 16 ) & 0xFFu;
		result.rgbReserved	= ( val >> 24 ) & 0xFFu;
	}
	catch( ... )
	{
		RaiseError( g_ErrorLogger , 0 , L"背景色を取得できませんでした" );
		result.rgbRed = 0xFF;
		result.rgbGreen = 0xFF;
		result.rgbBlue = 0xFF;
		result.rgbReserved = 0xFF;
	}
	return result;
}
