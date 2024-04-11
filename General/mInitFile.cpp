//----------------------------------------------------------------------------
// INIファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#define MINITFILE_CPP_COMPINLING
#include "mInitFile.h"
#include "General/mRegexp.h"

const mInitFileSection mInitFile::EmptySection;

mInitFile::mInitFile()
{
}

mInitFile::~mInitFile()
{
}

mInitFile::mInitFile( const mInitFile& src )
{
	*this = src;
}

mInitFile& mInitFile::operator=( const mInitFile& src )
{
	MySectionDataMap = src.MySectionDataMap;
	return *this;
}

//読み込み済みのデータを破棄する
void mInitFile::Clear( void )noexcept
{
	//現在保持しているデータが有れば破棄する。そして空文字列のセクションを追加する。
	MySectionDataMap.clear();
	MySectionDataMap[ L"" ];	//これでsecond側がデフォルトコンストラクタで作成される
	return;
}

//INIファイルを読み込みます
bool mInitFile::Read( const WString& filename , bool tolower , Encode enc )noexcept
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
	mFileReadStream fp;
	if( !fp.Open( opt ) )
	{
		//ファイルが開けなかった
		RaiseError( g_ErrorLogger , 0 , L"Open file failed" , filename );
		return false;
	}

	//エンコードの指定
	if( !fp.SetEncode( enc ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"エンコードの指定に失敗しました" , filename );
		return false;
	}

	//ファイルが開けたので読み込み処理をする
	return Read( fp , tolower );
}

bool mInitFile::Read( mFileReadStream& fp , bool tolower )
{
	mRegexpW InitFileCommentPattern( LR"(^\s*(;|$))" );
	mRegexpW InitFileSectionPattern1( LR"(^\[(\S+)\])" );
	mRegexpW InitFileSectionPattern2( LR"(^<(\S+)>)" );
	mRegexpW InitFileKeyValsPattern1( LR"(^\s*([A-Za-z0-9._()@#]+)\s*(\[([0-9]+)\])*\s*=\s*(.*?)\s*(;|$))" );
	mRegexpW InitFileKeyValsPattern2( LR"(^\s*(.*?)\s*(;|$))" );
	mRegexpW InitFileKeyValsPattern3( LR"(^\s*\.([A-Za-z0-9._()@#]+)\s*(\[([0-9]+)\])*\s*=\s*(.*?)\s*(;|$))" );

	//現在のセクションを設定
	//※空文字列のセクションがない可能性もあるので、先にタッチしておく。
	MySectionDataMap[ L"" ]; //これでsecond側がデフォルトコンストラクタで作成される
	SectionDataMap::iterator sect_itr = MySectionDataMap.find( L"" );

	//ファイルを１行ずつ読み取って内容を判別し、データを登録する。
	DWORD line_count = 0;			//行番号
	bool line_read_mode = false;	//false=[]のセクション true=<>のセクション

	WString line;				//ファイルから読み取った行

	mInitFileSection::Key key;	//=の左側…キー
	WString val;				//=の右側…値
	while( !fp.IsEOF() )
	{
		if( !fp.ReadLine( line ) )
		{
			continue;
		}

		//各行の判定
		if( InitFileCommentPattern.Match( line ) )
		{
			//コメント or 空行なので無視
			continue;
		}
		else if( InitFileSectionPattern1.Match( line ) )
		{
			//セクション名( []で囲まれた行 )の処理
			WString current_section;
			if( tolower )
			{
				current_section = ToLower( InitFileSectionPattern1.Submatch()[ 1 ] );
			}
			else
			{
				current_section = InitFileSectionPattern1.Submatch()[ 1 ];
			}

			//既存オブジェクトのクリア
			MySectionDataMap[ current_section ].MySectionName = current_section;
			sect_itr = MySectionDataMap.find( current_section );
			sect_itr->second.MyKeyValueMap.clear();
			sect_itr->second.MySectionType = mInitFileSection::SECTIONTYPE_NORMAL;

			line_read_mode = false;
			line_count = 0;
			continue;
		}
		else if( InitFileSectionPattern2.Match( line ) )
		{
			//セクション名( <>で囲まれた行 )の処理
			WString current_section;
			if( tolower )
			{
				current_section = ToLower( InitFileSectionPattern2.Submatch()[ 1 ] );
			}
			else
			{
				current_section = InitFileSectionPattern2.Submatch()[ 1 ];
			}

			//既存オブジェクトのクリア
			MySectionDataMap[ current_section ].MySectionName = current_section;
			sect_itr = MySectionDataMap.find( current_section );
			sect_itr->second.MyKeyValueMap.clear();
			sect_itr->second.MySectionType = mInitFileSection::SECTIONTYPE_ROWBASE;

			line_read_mode = true;
			line_count = 0;
			continue;
		}

		//キーと値のペアの処理
		if( line_read_mode )
		{
			//行単位の読み込みの場合
			if( InitFileKeyValsPattern3.Match( line ) )
			{
				key.key = L"";
				key.index = line_count;

				if( tolower )
				{
					key.subkey = ToLower( InitFileKeyValsPattern3.Submatch()[ 1 ] );
				}
				else
				{
					key.subkey = InitFileKeyValsPattern3.Submatch()[ 1 ];
				}
				val = InitFileKeyValsPattern3.Submatch()[ 4 ];
			}
			else if( InitFileKeyValsPattern2.Match( line ) )
			{
				key.key = L"";
				key.index = line_count;
				key.subkey = L"";
				val = InitFileKeyValsPattern2.Submatch()[ 1 ];
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"Iniファイルの書式が違います" , line );
				continue;
			}
			line_count++;
		}
		else
		{
			//キー=値 の形になっている行？
			if( InitFileKeyValsPattern1.Match( line ) )
			{
				//=の左側…キー
				WString key_str = InitFileKeyValsPattern1.Submatch()[ 1 ];
				if( key_str[ 0 ] == L'.' )	//.で始まる？
				{
					key.index = 0;
					if( key_str.size() == 1 )
					{
						//.だけの場合
						key.subkey = L"";
					}
					else if( tolower )
					{
						key.subkey = ToLower( key_str.substr( 1 ) );
					}
					else
					{
						key.subkey = key_str.substr( 1 );
					}
				}
				else
				{
					if( tolower )
					{
						key.key = ToLower( key_str );
					}
					else
					{
						key.key = key_str;
					}

					WString index_str = InitFileKeyValsPattern1.Submatch()[ 3 ];
					if( wchar_sscanf( index_str.c_str() , L"%d" , &key.index ) < 0 )
					{
						key.index = 0;
					}
					key.subkey = L"";
				}
				//=の右側…値
				val = InitFileKeyValsPattern1.Submatch()[ 4 ];
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"Iniファイルの書式が違います" , line );
				continue;
			}
		}

		//すでに存在するキー？
		mInitFileSection::KeyValueMap::iterator val_itr = sect_itr->second.MyKeyValueMap.find( key );
		if( val_itr == sect_itr->second.MyKeyValueMap.end() )
		{
			//新規のキーなので追加
			sect_itr->second.MyKeyValueMap.insert( mInitFileSection::KeyValueMap::value_type( key , val ) );
		}
		else
		{
			//既存のキーなので上書き
			val_itr->second = val;
		}
	}
	return true;
}

//INIファイルを書き込みます
bool mInitFile::Write( const WString& filename )
{
	//ファイルを開くときの情報
	mFileWriteStream::Option opt;
	opt.AccessRead = false;
	opt.AccessWrite = true;
	opt.Mode = mFileReadStream::CreateMode::CreateAlways;
	opt.ShareRead = true;
	opt.ShareWrite = false;
	opt.Path = filename;

	//ファイルを開く
	mFileWriteStream fp;
	if( !fp.Open( opt ) )
	{
		//ファイルが開けなかった
		RaiseError( g_ErrorLogger , 0 , L"Open file failed" , filename );
		return false;
	}

	//ファイルが開けたので書き込み処理をする
	return Write( fp );
}

//INIファイルを書き込みます
bool mInitFile::Write( mFileWriteStream& fp )
{
	bool result = true;

	//セクション一覧の作成
	WStringDeque sectionlist;
	if( !GetSectionList( sectionlist ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"セクション一覧を作成できませんでした" );
		return false;
	}

	//各セクションのデータを出力
	for( WStringDeque::const_iterator itr = sectionlist.begin() ; itr != sectionlist.end() ; itr++ )
	{
		const mInitFileSection* section = GetSection( *itr );
		if( section == nullptr )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"セクションのポインタが取得できませんでした" , *itr );
			result = false;
		}
		else if( 0 < section->GetCount() )
		{
			//1個以上キーを含んでいる場合のみ出力

			if( !section->Write( fp ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"セクションの出力が失敗しました" , *itr );
			}
			fp.WriteString( L"\r\n" );
		}
		else
		{
			;	//nothing to do
		}
	}
	return result;
}

//セクションを取得します
const mInitFileSection* mInitFile::GetSection( const WString& section )const noexcept
{
	SectionDataMap::const_iterator itr = MySectionDataMap.find( section );
	if( itr == MySectionDataMap.end() )
	{
		return nullptr;
	}
	return &itr->second;
}

//セクションを取得します
mInitFileSection* mInitFile::GetSectionForModification( const WString& section )noexcept
{
	SectionDataMap::iterator itr = MySectionDataMap.find( section );
	if( itr == MySectionDataMap.end() )
	{
		return nullptr;
	}
	return &itr->second;
}

//セクションを追加します
mInitFileSection* mInitFile::CreateNewSection( const WString& section )noexcept
{
	MySectionDataMap[ section ];
	return GetSectionForModification( section );
}

//セクションを削除します
bool mInitFile::DeleteSection( const WString& section )noexcept
{
	SectionDataMap::iterator itr = MySectionDataMap.find( section );
	if( itr == MySectionDataMap.end() )
	{
		return false;
	}

	MySectionDataMap.erase( itr );
	return true;
}

//セクションが存在するかを判定します
bool mInitFile::IsExistSection( const WString& section )const noexcept
{
	return MySectionDataMap.count( section ) != 0;
}

//セクション名の一覧を作成します
bool mInitFile::GetSectionList( WStringDeque& retList )const
{
	retList.clear();

	for( SectionDataMap::const_iterator itr = MySectionDataMap.begin() ; itr != MySectionDataMap.end() ; itr++ )
	{
		retList.push_back( itr->first );
	}
	return true;
}


