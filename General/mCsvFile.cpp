//----------------------------------------------------------------------------
// CSVファイルハンドラ
// Copyright (C) 2020-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#include "mCsvFile.h"
#include <General/mFileReadStream.h>
#include <General/mErrorLogger.h>

static bool OpenCsvFile( const WString& filename , mFileReadStream& retfp )
{
	//ファイルを開くときの情報
	mFileReadStream::Option opt;
	opt.AccessRead = true;
	opt.AccessWrite = false;
	opt.Mode = mFileReadStream::CreateMode::OpenExisting;
	opt.ShareRead = true;
	opt.ShareWrite = false;
	opt.Path = filename;

	//ファイルを開く
	if( !retfp.Open( opt ) )
	{
		//ファイルが開けなかった
		RaiseError( g_ErrorLogger , 0 , L"ファイルを開けません" );
		return false;
	}
	return true;
}

template < class base_type >
using Cell = std::basic_string< base_type >;

template < class base_type >
using Row = std::deque< Cell< base_type > >;

template < class base_type >
using Table = std::deque< Row< base_type > >;

template< class base_type >
static bool ReadCsvFileTemplate( mFileReadStreamBase& fp , Table< base_type >& retTable )
{

	//特殊文字の検出状態
	bool quote_detect = false;	//ダブルクォーテーションの検出
	bool quoted = false;		//ダブルクォーテーションで囲まれている
	bool cr_detect = false;		//改行コード(CR)の検出
	Cell< base_type > cell;		//読み取り中のセル
	Row< base_type > row;		//読み取り中の行

	//行を移動するラムダ
	auto InsertCell = [&row,&cell]( bool noempty ) -> void 
	{
		if( !noempty || !cell.empty() )
		{
			row.push_back( std::move( cell ) );
			cell = Cell< base_type >();
		}
	};

	//行を移動するラムダ
	auto InsertRow = [&retTable,&row]( bool noempty ) -> void 
	{
		if( !noempty || !row.empty() )
		{
			retTable.push_back( std::move( row ) );
			row = Row< base_type >();
		}
	};

	//読み取りメインループ
	while( !fp.IsEOF() )
	{
		base_type c;
		if( !fp.ReadSingle( c ) )
		{
			continue;
		}

		//改行処理
		if( c == (base_type)'\n' )
		{
			if( !quoted )
			{
				//行の区切り(LF/CRLF改行)
				InsertCell( true );
				InsertRow( false );
				cr_detect = false;
				continue;
			}
		}
		else if( c == (base_type)'\r' )
		{
			if( !quoted )
			{
				if( cr_detect )
				{
					//行の区切り(CR改行)＋CR
					InsertCell( true );
					InsertRow( false );
				}
				cr_detect = true;
				continue;
			}
		}
		else
		{
			if( cr_detect )
			{
				//行の区切り(CR改行)
				InsertCell( true );
				InsertRow( false );
			}
			cr_detect = false;
		}

		//ダブルクォーテーション処理
		if( c == (base_type)'"' )
		{
			if( quote_detect )
			{
				//2連続
				quote_detect = false;
			}
			else
			{
				//ダブルクォーテーション検出
				quote_detect = true;
				continue;
			}
		}
		else
		{
			if( quote_detect )
			{
				//囲み開始・終了
				quoted = !quoted;
				quote_detect = false;
			}
		}

		//コンマ処理
		if( c == (base_type)',' )
		{
			if( !quoted )
			{
				//列(セル)の区切り
				InsertCell( false );
				continue;
			}
		}

		//その他
		cell.push_back( c );
	}

	InsertCell( true );
	InsertRow( true );
	return true;
}

template< class base_type >
static void FillEmptyCell( Table< base_type >& retTable , const std::basic_string< base_type >* empty_str )
{
	if( !empty_str )
	{
		return;
	}

	size_t size_max = 0 ;
	for( Table< base_type >::const_iterator itr = retTable.begin() ; itr != retTable.end() ; itr++ )
	{
		if( size_max < itr->size() )
		{
			size_max = itr->size();
		}
	}

	for( Table< base_type >::iterator itr = retTable.begin() ; itr != retTable.end() ; itr++ )
	{
		itr->resize( size_max , *empty_str );
	}
}

bool mCsvFile::ReadCsvFile( const WString& filename , WTable& retTable , const WString* empty_str )
{
	//ファイルを開く（テンプレートと関係ない制御は外に追い出す）
	mFileReadStream fp;
	if( !OpenCsvFile( filename , fp ) )
	{
		return false;
	}
	//読み取り
	return ReadCsvFile( fp , retTable , empty_str );
}

bool mCsvFile::ReadCsvFile( const WString& filename , ATable& retTable , const AString* empty_str )
{
	//ファイルを開く（テンプレートと関係ない制御は外に追い出す）
	mFileReadStream fp;
	if( !OpenCsvFile( filename , fp ) )
	{
		return false;
	}
	//読み取り
	return ReadCsvFile( fp , retTable , empty_str );
}

//CSVファイルを読み取って配列に格納する
//・読み取るときの文字コードは、retTableに指定した型に合わせる
//stream : 読み取るストリーム
//retTable : 格納先
//empty_str : 読み取り結果が長方形になっていないとき（行／列数がでこぼこになっているとき）
//　　　　　　長方形になるようにこのデータを補って整形する。nullptrなら整形しない。
//ret : 成功時真
bool mCsvFile::ReadCsvFile( mFileReadStreamBase& stream , WTable& retTable , const WString* empty_str )
{
	//読み取り
	if( !ReadCsvFileTemplate( stream , retTable ) )
	{
		return false;
	}
	FillEmptyCell( retTable , empty_str );
	return true;
}

//CSVファイルを読み取って配列に格納する
//・読み取るときの文字コードは、retTableに指定した型に合わせる
//stream : 読み取るストリーム
//retTable : 格納先
//empty_str : 読み取り結果が長方形になっていないとき（行／列数がでこぼこになっているとき）
//　　　　　　長方形になるようにこのデータを補って整形する。nullptrなら整形しない。
//ret : 成功時真
bool mCsvFile::ReadCsvFile( mFileReadStreamBase& stream , ATable& retTable , const AString* empty_str )
{
	//読み取り
	if( !ReadCsvFileTemplate( stream , retTable ) )
	{
		return false;
	}
	FillEmptyCell( retTable , empty_str );
	return true;
}

