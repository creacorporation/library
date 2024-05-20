//----------------------------------------------------------------------------
// Microsoft Excel�n���h��
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
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
		//��d��������h�~����
		RaiseError( g_ErrorLogger , 0 , L"Excel�͂��łɋN������Ă��܂�" );
		return false;
	}
	if( SUCCEEDED( MyApplication.CreateInstance( L"Excel.Application" ) ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"Excel���N�����܂���" );
		return true;
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�̋N�������s���܂���" );
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
		CreateLogEntry( g_ErrorLogger , 0 , L"Excel���I�����܂���" );
	}

	return true;
}

//�������ς݂��H
bool mExcelHandler::IsInitialized( void )const
{
	return *this;
}

//�������ς݂��H
mExcelHandler::operator bool()const
{
	return MyApplication != nullptr;
}

bool mExcelHandler::SetVisible( bool newstate )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}
	MyApplication->PutVisible( newstate );
	return true;
}

bool mExcelHandler::AddNewWorkbook( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}
	MyApplication->Workbooks->Add();
	return true;
}

bool mExcelHandler::OpenWorkbook( const WString& filename , const mExcelHandler::OpenWorkbookOption* opt )
{
	/*
	��Open���\�b�h�̈���

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

	//�A�v�����N�����Ă��Ȃ���΃G���[
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	//�I�v�V�����̓ǂݎ��
	_variant_t is_read_only;	//���[�h�I�����[�H
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

	//�J������
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
		RaiseError( g_ErrorLogger , 0 , L"�t�@�C�����J���܂���ł���" , filename );
		result = false;
	}

	return result;
}


DWORD mExcelHandler::GetWorkbookCount( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return 0;
	}
	return (DWORD)( MyApplication->GetWorkbooks()->Count );
}

bool mExcelHandler::SetCurrentWorkbook( DWORD index )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	if( index == 0 || GetWorkbookCount() < index )
	{
		//�C���f�b�N�X�l�s��
		RaiseError( g_ErrorLogger , 0 , L"�C���f�b�N�X���s���ł�" , index );
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
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	WString tmp_search_name = ( !isFuzzyMatch ) ? ( name ) : ( MakeFuzzyName( name ) );

	//���[�N�u�b�N�̎w�肪���O�o�����ꍇ�A
	//�S���[�N�u�b�N�����v���閼�O��{���A��v�������̂��J�����g�ɂ���B

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
				//�ŏ��ɖ��O����v�������̂ɂ���
				MyCurrentWorkbook = wbs->GetItem( _variant_t( i ) );
				return true;
			}
		}
		else
		{
			WString tmp_book_name = MakeFuzzyName( (wchar_t*)wbname );
			if( tmp_search_name == tmp_book_name )
			{
				//�ŏ��ɖ��O����v�������̂ɂ���
				MyCurrentWorkbook = wbs->GetItem( _variant_t( i ) );
				return true;
			}
		}
	}
	RaiseError( g_ErrorLogger , 0 , L"�w��̃��[�N�u�b�N�͂���܂���" , name );
	return false;

}

WString mExcelHandler::GetWorkbookName( void )const
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return L"";
	}

	return (const wchar_t*)MyCurrentWorkbook->Name;
}

WString mExcelHandler::GetWorkbookName( DWORD index )
{
	DWORD count = GetWorkbookCount();
	if( index == 0 || count < index )
	{
		RaiseError( g_ErrorLogger , 0 , L"�w��̃��[�N�u�b�N�͂���܂���" , index );
		return L"";
	}

	//MyApplication==0����GetWorkbookCount��0��Ԃ��̂ŁA�����ł̃k���`�F�b�N�͂Ȃ��B
	XlsWorkbook wb = MyApplication->Workbooks->GetItem( _variant_t( index ) );
	return (const wchar_t*)wb->Name;
}

bool mExcelHandler::GetWorkbookNames( WStringDeque& retNames )
{
	//�����̃f�[�^��j��
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
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return 0;
	}
	return (DWORD)( MyCurrentWorkbook->Sheets->Count );
}


bool mExcelHandler::SetCurrentWorksheet( DWORD index )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return false;
	}

	if( index == 0 || GetWorksheetCount() < index )
	{
		//�C���f�b�N�X�l�s��
		RaiseError( g_ErrorLogger , 0 , L"�w��̃��[�N�u�b�N�͂���܂���" , index );
		return false;
	}

	MyCurrentWorksheet = MyCurrentWorkbook->Worksheets->GetItem( _variant_t( index ) );
	return true;

}


bool mExcelHandler::SetCurrentWorksheet( const WString& name , bool isFuzzyMatch )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
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
				//�ŏ��ɖ��O����v�������̂ɂ���
				MyCurrentWorksheet = ws->GetItem( _variant_t( i ) );
				return true;
			}
		}
		else
		{
			WString tmp_sheet_name = MakeFuzzyName( (wchar_t*)wsname );
			if( tmp_search_name == tmp_sheet_name )
			{
				//�ŏ��ɖ��O����v�������̂ɂ���
				MyCurrentWorksheet = ws->GetItem( _variant_t( i ) );
				return true;
			}
		}
	}
	RaiseError( g_ErrorLogger , 0 , L"�w��̃��[�N�u�b�N�͂���܂���" , name );
	return false;
}

WString mExcelHandler::GetWorksheetName( DWORD index )
{

	DWORD count = GetWorksheetCount();
	if( index == 0 || count < index )
	{
		RaiseError( g_ErrorLogger , 0 , L"�w��̃��[�N�V�[�g�͂���܂���" , index );
		return L"";
	}

	XlsWorksheet wsptr = MyCurrentWorkbook->Worksheets->GetItem( _variant_t( index ) );

	return (const wchar_t*)wsptr->Name;
}

WString mExcelHandler::GetWorksheetName( void )
{
	if( MyCurrentWorksheet == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�V�[�g�͎w�肳��Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�V�[�g�͎w�肳��Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�V�[�g�͎w�肳��Ă��܂���" );
		return false;
	}

	try
	{
		MyCurrentRange = MyCurrentWorksheet->GetRange( range.c_str() );
	}
	catch( _com_error e )
	{
		RaiseError( g_ErrorLogger , 0 , L"�͈͎w�肪���s���܂���" , TString2WString( e.ErrorMessage() ) );
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
		RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"�l�̎擾�����s���܂���" );
		return TEXT( "" );
	}

	dat.ChangeType( VT_BSTR );
	return (wchar_t*)dat.bstrVal;
}

bool mExcelHandler::GetValue( _variant_t& retResult )
{

	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
		return false;
	}

	try
	{
		retResult = MyCurrentRange->Value2;
	}
	catch( _com_error e )
	{
		RaiseError( g_ErrorLogger , 0 , L"�l�̎擾�����s���܂���" , TString2WString( e.ErrorMessage() ) );
		return false;
	}
	return true;
}

bool mExcelHandler::Search( const SearchOption& opt , fpSearchFindCallback callback , void* dataptr , DWORD* retCount )
{

	//���ƈ�v�H
	Excel::XlFindLookIn lookin;
	switch( opt.MatchType )
	{
	case SearchOption::MatchTypeEntry::Value:	//�l��
		lookin = Excel::XlFindLookIn::xlValues;
		break;
	case SearchOption::MatchTypeEntry::Comment:	//�R�����g��
		lookin = Excel::XlFindLookIn::xlComments;
		break;
	case SearchOption::MatchTypeEntry::Formula:	//������
		lookin = Excel::XlFindLookIn::xlFormulas;
		break;
	default:					//�ςȒl
		RaiseAssert( g_ErrorLogger , 0 , L"�����̈�v�Ώۂ��s���ł�" , opt.MatchType );
		return false;		
	}

	//�S�̈�v�H������v�H
	Excel::XlLookAt lookat = ( opt.MatchWhole ) ? ( Excel::XlLookAt::xlWhole ) : ( Excel::XlLookAt::xlPart );

	//���������́H
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
		RaiseAssert( g_ErrorLogger , 0 , L"�����������s���ł�" , opt.SearchDirection );
		return false;
	}

	//�����J�n
	DWORD count = 0;		//����������
	XlsRange search_range;	//�T�[�`�͈�
	XlsRange found;			//���������Z��

	//�ŏ���1��������
	switch( opt.SearchArea )
	{
	case SearchOption::SearchAreaEntry::Range:
		if( MyCurrentRange == nullptr )
		{
			//�����͈͂��w�肳��Ă��Ȃ��Ɣ�������͂����Ȃ��̂łO
			RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
			return false;
		}
		search_range = MyCurrentRange;
		break;
	case SearchOption::SearchAreaEntry::Worksheet:
		if( MyCurrentWorksheet == nullptr )
		{
			RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�V�[�g�͎w�肳��Ă��܂���" );
			return false;
		}
		search_range = MyCurrentWorksheet->Cells;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�����͈͂��s���ł�" , opt.SearchArea );
		return false;
	}

	//�����I
	found = search_range->Find( opt.What.c_str() , vtMissing , lookin , lookat , order , Excel::XlSearchDirection::xlNext , opt.MatchCase );
	if( found != 0 )
	{
		//�Y���Z������
		INT first_row = found->Row;			//�ŏ��ɔ��������Z���̍s
		INT first_col = found->Column;		//�ŏ��ɔ��������Z���̗�
		INT row = first_row;				//���ݒ��ڂ��Ă���Z���̍s
		INT col = first_col;				//���ݒ��ڂ��Ă���Z���̗�
		_variant_t after;					//���̌����J�n�ʒu

		//2�ڈȍ~�̌���
		do
		{
			count++;
			//�R�[���o�b�N�֐��̌Ăяo��
			if( callback )
			{
				if( !callback( dataptr , row , col ) )
				{
					break;	//�����s�v�̏ꍇ
				}
			}

			//��������
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
		RaiseAssert( g_ErrorLogger , 0 , L"�Z�����T���f�[�^���k���ł�" );
		return false;
	}

	//�擾�����A�h���X��o�^
	data->Pos->push_back( mExcelCellRef::Position( false , false , true , true , row , col ) );

	//�q�b�g���J�E���g
	data->CurrentFound++;
	if( data->CurrentFound == data->MaxFound )
	{
		//�ő哞�B
		return false;
	}
	return true;
}

//�w��͈͓̔����������A���������Z���̖��̂̈ꗗ��Ԃ�
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
		RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
		return false;
	}

	return (DWORD)MyCurrentRange->Rows->Count;

}

DWORD mExcelHandler::GetRangeCol( void )
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
		return false;
	}
	return (DWORD)MyCurrentRange->Columns->Count;
}

//�}�[�W���ꂽ�Z�����𓾂�
bool mExcelHandler::IsMerged( void )const
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
		return false;
	}
	return MyCurrentRange->MergeCells;
}

//�}�[�W���ꂽ�Z���͈̔͂𓾂�
bool mExcelHandler::GetMergeRange( mExcelCellRef& retrange )
{
	if( MyCurrentRange == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂͈͎̔͂w�肳��Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return false;
	}

	if( SUCCEEDED( MyCurrentWorkbook->Save() ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"���[�N�u�b�N���㏑���Z�[�u���܂���" );
		return true;
	}
	RaiseError( g_ErrorLogger , 0 , L"���[�N�u�b�N���㏑���Z�[�u�ł��܂���ł���" );
	return false;
}

bool mExcelHandler::SaveAs( const WString& filename , mExcelHandler::XlsFileFormat format )
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return false;
	}

	if( SUCCEEDED( MyCurrentWorkbook->SaveAs( filename.c_str() , (long)format , vtMissing , vtMissing , vtMissing , vtMissing , Excel::xlExclusive ) ) )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"���[�N�u�b�N���Z�[�u���܂���" , filename );
		return true;
	}
	RaiseError( g_ErrorLogger , 0 , L"���[�N�u�b�N���Z�[�u�ł��܂���ł���" , filename );
	return false;

}

bool mExcelHandler::SetAlertEnable( bool newval )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}
	MyApplication->DisplayAlerts = newval;
	return true;
}

bool mExcelHandler::GetAlertEnable( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return true;
	}
	return MyApplication->DisplayAlerts;
}

bool mExcelHandler::SetCalculation( mExcelHandler::XlsCalculation newval )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	MyApplication->Calculation = newval;
	return true;
}


mExcelHandler::XlsCalculation mExcelHandler::GetCalculation( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return XlsCalculation::xlCalculationAutomatic;
	}
	return MyApplication->Calculation;
}

bool mExcelHandler::CalcAll( void )
{
	//�S���[�N�u�b�N
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	return SUCCEEDED( MyApplication->Calculate() );
}

bool mExcelHandler::CalcWorksheet( void )
{
	//���݂̃��[�N�V�[�g
	if( MyCurrentWorksheet == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return false;
	}

	return SUCCEEDED( MyCurrentWorksheet->Calculate() );
}

//�C�x���g�̗L���E�����ݒ�
bool mExcelHandler::SetEventEnable( bool newval )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	MyApplication->EnableEvents = newval;
	return true;

}

//�C�x���g�̗L���E������Ԃ��擾����
bool mExcelHandler::SetEventEnable( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	return MyApplication->EnableEvents;
}

bool mExcelHandler::Close( bool force , const WString* bookname )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
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
			RaiseError( g_ErrorLogger , 0 , L"�w��̃��[�N�u�b�N�͂���܂���" , *bookname );
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
		//�E����̂Ɏ��s���A���throw�����s���ꂽ�ꍇ
		//�E���[�U�[��Excel�����Ȃǂ��āACOM�I�u�W�F�N�g�������̏ꍇ
		RaiseError( g_ErrorLogger , 0 , L"���[�N�u�b�N����邱�Ƃ��o���܂���ł���" , force );
		return false;
	}

	return true;
}

bool mExcelHandler::IsModified( void )const
{
	if( MyCurrentWorkbook == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"���݂̃��[�N�u�b�N�͎w�肳��Ă��܂���" );
		return false;
	}

	return MyCurrentWorkbook->Saved;

}

bool mExcelHandler::AddNewWorksheet( void )
{
	if( MyApplication == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"Excel�͋N�����Ă��܂���" );
		return false;
	}

	MyApplication->Worksheets->Add();
	return true;
}

bool mExcelHandler::ReadArray( const _variant_t& arr , fpCellReadCallback reader , void* dataptr , ScanOrder order )
{
	bool result = false;

	//�A���C����Ȃ�������m�f
	if( arr.vt != VT_SAFEARRAY && ( arr.vt & VT_ARRAY ) != VT_ARRAY )
	{
		RaiseError( g_ErrorLogger , 0 , L"�w�肵���I�u�W�F�N�g�̓A���C�ł͂���܂���" );
		return false;
	}

	//�A���C�����b�N����
	if( FAILED( SafeArrayLock( arr.parray ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�A���C�����b�N�ł��܂���ł���" );
		return false;
	}

	//����������return�Ŗ߂�����_��

	VARIANT*	var_cell;
	_variant_t	cell;

	//�A���C�̂P�����ڂ̗v�f�����擾
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
	case ScanOrder::SCANORDER_ROW_MAJOR:				//��̍s���珇�ɁA������(������E)�ɃX�L����
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = normal_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_COL_MAJOR:				//���̗񂩂珇�ɁA�c����(�ォ�牺)�ɃX�L����
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = normal_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_ROW_REVERSE:				//��̍s���珇�ɁA������(�E���獶)�ɃX�L����
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = normal_order;
		converter_2 = reverse_order;
		break;
	case ScanOrder::SCANORDER_COL_REVERSE:				//���̗񂩂珇�ɁA�c����(�������)�ɃX�L����
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = normal_order;
		converter_2 = reverse_order;
		break;
	case ScanOrder::SCANORDER_ROW_MAJOR_BOTTOMUP:		//���̍s���珇�ɁA������(������E)�ɃX�L����
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = reverse_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_COL_MAJOR_RIGHTTOLEFT:	//�E�̗񂩂珇�ɁA�c����(�ォ�牺)�ɃX�L����
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = reverse_order;
		converter_2 = normal_order;
		break;
	case ScanOrder::SCANORDER_ROW_REVERSE_BOTTOMUP:		//���̍s���珇�ɁA������(�E���獶)�ɃX�L����
		dim_1 = 1;
		dim_2 = 2;
		converter_1 = reverse_order;
		converter_2 = reverse_order;
		break;
	case ScanOrder::SCANORDER_COL_REVERSE_RIGHTTOLEFT:	//�E�̗񂩂珇�ɁA�c����(�������)�ɃX�L����
		dim_1 = 2;
		dim_2 = 1;
		converter_1 = reverse_order;
		converter_2 = reverse_order;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"�X�L���������̐ݒ肪�s���ł�" );
		goto end;
	}

	SafeArrayGetUBound( arr.parray , dim_1 , &ub_1 );
	SafeArrayGetLBound( arr.parray , dim_1 , &lb_1 );

	//����ł͂��悢�惋�[�v�ł�
	for( LONG loop_i = lb_1 ; loop_i <= ub_1 ; loop_i++ )
	{
		LONG i = converter_1( loop_i , lb_1 , ub_1 );
		index[ dim_1 - 1 ] = i;

		//�A���C�̂Q�����ڂ̗v�f�����擾
		LONG ub_2;
		LONG lb_2;
		SafeArrayGetUBound( arr.parray , dim_2 , &ub_2 );
		SafeArrayGetLBound( arr.parray , dim_2 , &lb_2 );

		for( LONG loop_j = lb_2 ; loop_j <= ub_2 ; loop_j++ )
		{
			LONG j = converter_2( loop_j , lb_2 , ub_2 );
			ScanContinue cont_flag;
			index[ dim_2 - 1 ] = j;

			//�l�擾
			if( FAILED( SafeArrayPtrOfIndex( arr.parray , index , (void HUGEP* FAR*)&var_cell ) ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"�A���C�̗v�f�ɑ΂���|�C���^���擾�ł��܂���ł���" );
				goto end;
			}

			//�R�[���o�b�N�Ăяo��
			cell.Attach( *var_cell );
			try
			{
				switch( order )
				{
				case ScanOrder::SCANORDER_ROW_MAJOR:				//��̍s���珇�ɁA������(������E)�ɃX�L����
				case ScanOrder::SCANORDER_ROW_REVERSE:				//��̍s���珇�ɁA������(�E���獶)�ɃX�L����
				case ScanOrder::SCANORDER_ROW_MAJOR_BOTTOMUP:		//���̍s���珇�ɁA������(������E)�ɃX�L����
				case ScanOrder::SCANORDER_ROW_REVERSE_BOTTOMUP:		//���̍s���珇�ɁA������(�E���獶)�ɃX�L����
					cont_flag = reader( cell , dataptr , i - lb_1 , j - lb_2 , ub_1 - lb_1 , ub_2 - lb_2 );
					break;
				case ScanOrder::SCANORDER_COL_MAJOR:				//���̗񂩂珇�ɁA�c����(�ォ�牺)�ɃX�L����
				case ScanOrder::SCANORDER_COL_REVERSE:				//���̗񂩂珇�ɁA�c����(�������)�ɃX�L����
				case ScanOrder::SCANORDER_COL_MAJOR_RIGHTTOLEFT:	//�E�̗񂩂珇�ɁA�c����(�ォ�牺)�ɃX�L����
				case ScanOrder::SCANORDER_COL_REVERSE_RIGHTTOLEFT:	//�E�̗񂩂珇�ɁA�c����(�������)�ɃX�L����
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

			//�R�[���o�b�N�֐����Ԃ����t���O�̏���
			//���̃Z���ɂ����H
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
				RaiseError( g_ErrorLogger , 0 , L"�R�[���o�b�N�֐����Ԃ����l���s���ł�" , cont_flag );
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
	//�{���̃R�[���o�b�N�֐�
	mExcelHandler::fpCellReadCallbackV reader;
	//�{���̃f�[�^
	void* dataptr;
};

//mVariant�ɕϊ�����R�[���o�b�N�v���L�V
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
		RaiseAssert( g_ErrorLogger , 0 , L"�R�[���o�b�N�֐����w�肳��Ă��܂���" );
		return false;
	}

	_variant_t val;
	if( !GetValue( val ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�l���擾�ł��܂���ł���" );
		return false;
	}
	if( val.vt == VT_SAFEARRAY )	//�Q�����i�ȏ�́j�z��
	{
		return ReadArray( val , reader , dataptr , order );
	}
	else if( ( val.vt & VT_ARRAY ) == VT_ARRAY )	//�P�����z��
	{
		return ReadArray( val , reader , dataptr , order );

	}
	else	//�P���f�[�^
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
		RaiseError( g_ErrorLogger , 0 , L"�w�i�F���擾�ł��܂���ł���" );
		result.rgbRed = 0xFF;
		result.rgbGreen = 0xFF;
		result.rgbBlue = 0xFF;
		result.rgbReserved = 0xFF;
	}
	return result;
}
