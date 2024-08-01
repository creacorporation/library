//----------------------------------------------------------------------------
// Microsoft Excelハンドラ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mExcelCellRef.h"
#include "General/mErrorLogger.h"

mExcelCellRef::mExcelCellRef()
{
	return;
}


mExcelCellRef::~mExcelCellRef()
{
	return;
}

mExcelCellRef::mExcelCellRef( const mExcelCellRef& src )
{
	MyPos1 = src.MyPos1;
	MyPos2 = src.MyPos2;
	MyAddress = src.MyAddress;
}

mExcelCellRef::mExcelCellRef( const Position& src )
{
	MyPos1 = src;
	MyPos2.Clear();
	return;
}

mExcelCellRef::mExcelCellRef( const Position& src1 , const Position& src2 )
{
	MyPos1 = src1;
	MyPos2 = src2;
	return;
}

const mExcelCellRef& mExcelCellRef::operator=( const mExcelCellRef& src )
{
	MyPos1 = src.MyPos1;
	MyPos2 = src.MyPos2;
	MyAddress = src.MyAddress;
	return *this;
}

void mExcelCellRef::Clear( void )
{
	MyAddress = L"";
	MyPos1.Clear();
	MyPos2.Clear();
}

bool mExcelCellRef::UpdateAddress( bool IsRC )const
{
	WString StrPos;

	if( GetRowCount() == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"有効な範囲を保持していません" );
		goto badend;
	}

	//左上のセル
	if( !BuildCellName( MyAddress , IsRC ,  MyPos1.AbsRow , MyPos1.AbsCol , MyPos1.Row , MyPos1.Col , MyPos1.WholeCol , MyPos1.WholeRow ) )
	{
		goto badend;
	}

	//右下のセル
	if( MyPos2.IsValid() )
	{
		if( !BuildCellName( StrPos , IsRC , MyPos2.AbsRow , MyPos2.AbsCol , MyPos2.Row , MyPos2.Col , MyPos2.WholeCol , MyPos2.WholeRow ) )
		{
			goto badend;
		}
		MyAddress += L":";
		MyAddress += StrPos;
	}
	return true;

badend:
	MyAddress = L"";
	return false;
}

bool mExcelCellRef::BuildCellName( WString& retCell , bool IsRC , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow , bool nocol )
{
	//結果初期化
	retCell.clear();

	//R1C1形式か否か？
	if( IsRC )
	{
		return BuildCellNameRC( retCell , IsAbsRow , IsAbsCol , row , col , norow , nocol );
	}
	else
	{
		return BuildCellNameA1( retCell , IsAbsRow , IsAbsCol , row , col , norow , nocol );
	}

	return true;
}

bool mExcelCellRef::BuildCellNameA1( WString& retCell , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow = false , bool nocol = false )
{
	AString r;
	AString c;

	//-----COL
	if( !nocol )
	{
		DWORD rest;
		if( col < 1 )
		{
			RaiseError( g_ErrorLogger , 0 , L"A1形式では、列番号は1開始です" );
			return false;
		}

		do
		{
			col--;
			rest = col % 26;
			col /= 26;
			r.insert( 0 , 1 , (char)( rest + 'A' ) );
		}while( 0 < col );

		if( IsAbsCol )
		{
			r.insert( 0 , "$" );
		}
	}

	//-----ROW
	if( !norow )
	{
		if( row < 1 )
		{
			RaiseError( g_ErrorLogger , 0 , L"A1形式では、行番号は1開始です" );
			return false;
		}

		if( IsAbsRow )
		{
			sprintf( &c , "$%d" , row );
		}
		else
		{
			sprintf( &c , "%d" , row );
		}
	}

	//結合
	r.append( c );
	retCell = AString2WString( r );

	return true;
}

bool mExcelCellRef::BuildCellNameRC( WString& retCell , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow = false , bool nocol = false )
{
	//-----ROW
	if( !norow )
	{
		if( row != 0 )
		{
			if( IsAbsRow )
			{
				if( row < 1 )
				{
					RaiseError( g_ErrorLogger , 0 , L"R1C1形式で絶対位置を指定する場合、行番号は1開始です" );
					return false;
				}
				sprintf( retCell , L"R%d" , row );
			}
			else
			{
				sprintf( retCell , L"R[%d]" , row );
			}
		}
		else
		{
			retCell = L"R";
		}
	}

	//-----COL
	if( !nocol )
	{
		if( col != 0 )
		{
			WString t;
			if( IsAbsCol )
			{
				if( col < 1 )
				{
					RaiseError( g_ErrorLogger , 0 , L"R1C1形式で絶対位置を指定する場合、列番号は1開始です" );
					return false;
				}
				sprintf( &t , L"C%d" , col );
			}
			else
			{
				sprintf( &t , L"C[%d]" , col );
			}
			retCell.append( t );
		}
		else
		{
			retCell.append( L"C" );
		}
	}

	return true;
}

bool mExcelCellRef::SetPosition( const Position& pos )
{
	if( !pos.IsValid() )
	{
		Clear();
		return false;
	}

	MyPos1 = pos;
	MyPos2.Clear();
	return true;
}

bool mExcelCellRef::SetPosition( const Position& pos1 , const Position& pos2 )
{
	if( !pos1.IsValid() || !pos2.IsValid() )
	{
		Clear();
		return false;
	}

	MyPos1 = pos1;
	MyPos2 = pos2;
	return true;
}

const WString& mExcelCellRef::GetAddress( bool IsRC )const
{
	UpdateAddress( IsRC );
	return MyAddress;
}

bool mExcelCellRef::GetPosition( Position* retPos1 , Position* retPos2 )
{
	if( retPos1 )
	{
		*retPos1 = MyPos1;
	}
	if( retPos2 )
	{
		*retPos2 = MyPos2;
	}
	return true;
}

const mExcelCellRef::Position& mExcelCellRef::GetPos1( void )const
{
	return MyPos1;
}

const mExcelCellRef::Position& mExcelCellRef::GetPos2( void )const
{
	return MyPos2;
}

DWORD mExcelCellRef::GetRowCount( const Position* pos )const
{
	if( !MyPos1.IsValid() )
	{
		//有効な位置を保持していない
		return 0;
	}
	if( !MyPos2.IsValid() )
	{
		//MyPos1だけで示される1つのセルを示している
		return 1;
	}
	if( ( MyPos1.WholeCol != MyPos2.WholeCol ) ||
		( MyPos1.WholeRow != MyPos2.WholeRow ) )
	{
		//MyPos1とMyPos2の関係が不正
		return 0;
	}
	//その他
	if( pos != nullptr )
	{
		if( !pos->IsValid() )
		{
			//基準位置が不正
			return 0;
		}

		INT tmp1 = ( MyPos1.AbsRow ) ? ( MyPos1.Row ) : ( MyPos1.Row + pos->Row );
		INT tmp2 = ( MyPos2.AbsRow ) ? ( MyPos2.Row ) : ( MyPos2.Row + pos->Row );
		if( tmp1 < tmp2 )
		{
			return tmp2 - tmp1 + 1;
		}
		return tmp1 - tmp2 + 1;
	}
	else
	{
		if( MyPos1.Row < MyPos2.Row )
		{
			return MyPos2.Row - MyPos1.Row + 1;
		}
		return MyPos1.Row - MyPos2.Row + 1;
	}
}

DWORD mExcelCellRef::GetColCount( const Position* pos )const
{
	if( MyPos1.WholeRow && MyPos1.WholeCol )
	{
		//有効な位置を保持していない
		return 0;
	}
	if( !MyPos2.WholeRow && !MyPos2.WholeCol )
	{
		//MyPos1だけで示される1つのセルを示している
		return 1;
	}
	if( ( MyPos1.WholeCol != MyPos2.WholeCol ) ||
		( MyPos1.WholeRow != MyPos2.WholeRow ) )
	{
		//MyPos1とMyPos2の関係が不正
		return 0;
	}
	//その他
	if( pos != nullptr )
	{
		if( !pos->IsValid() )
		{
			//基準位置が不正
			return 0;
		}

		INT tmp1 = ( MyPos1.AbsCol ) ? ( MyPos1.Col ) : ( MyPos1.Col + pos->Col );
		INT tmp2 = ( MyPos2.AbsCol ) ? ( MyPos2.Col ) : ( MyPos2.Col + pos->Col );
		if( tmp1 < tmp2 )
		{
			return tmp2 - tmp1 + 1;
		}
		return tmp1 - tmp2 + 1;
	}
	else
	{
		if( MyPos1.Col < MyPos2.Col )
		{
			return MyPos2.Col - MyPos1.Col + 1;
		}
		return MyPos1.Col - MyPos2.Col + 1;
	}
}



mExcelCellRef::Position::Position( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , const WString& col )
{
	INT col_index = 0;
	
	for( WString::const_iterator itr = col.begin() ; itr != col.end() ; itr++ )
	{
		if( L'A' <= *itr && *itr <= L'Z' )
		{
			col_index *= 26;
			col_index += ( *itr - L'A' + 1 );
		}
		else if( L'a' <= *itr && *itr <= L'z' )
		{
			col_index *= 26;
			col_index += ( *itr - L'a' + 1 );
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"A1形式ではない文字が指定されました" );
			return;
		}
	}
	Set( whole_row , whole_col , abs_row , abs_col , row , col_index );
}
