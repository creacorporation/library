//----------------------------------------------------------------------------
// INIファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#define MINITFILESECTION_CPP_COMPINLING
#include "mInitFileSection.h"
#include <unordered_set>

mInitFileSection::mInitFileSection()
{
	MySectionType = SectionType::SECTIONTYPE_NORMAL;
	MyIsModified = false;
}

mInitFileSection::~mInitFileSection()
{
}

mInitFileSection::mInitFileSection( const mInitFileSection& src )
{
	*this = src;
}

mInitFileSection& mInitFileSection::operator=( const mInitFileSection& src )
{
	MyIsModified = src.MyIsModified;
	MyKeyValueMap = src.MyKeyValueMap;
	MySectionName = src.MySectionName;
	MySectionType = src.MySectionType;
	return *this;
}


//指定したキーが存在するか返す
bool mInitFileSection::IsValidKey( const WString& key )const noexcept
{
	return IsValidKey( key , 0 , L"" );
}

//指定したキーが存在するか返す
bool mInitFileSection::IsValidKey( const WString& key , INT index )const noexcept
{
	return IsValidKey( key , index , L"" );
}

//指定したキーが存在するか返す
bool mInitFileSection::IsValidKey( const WString& key , INT index , const WString& subkey )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;
	return MyKeyValueMap.count( tmpkey ) != 0;
}

//指定したキーが存在するか返す
bool mInitFileSection::IsValidKey( const WString& key , const WString& subkey )const noexcept
{
	return IsValidKey( key , 0 , subkey );
}

//キーの値を32ビットINTの値として読み取る
INT mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , INT defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	INT val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%x" , &val ) == 1 )
	{
		//0xで始まっている場合は16進数として読み取る
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%d" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}

//キーの値を32ビットLONGの値として読み取る
//key : 読み取りたいキー
//defvalue : キーを読み取れなかった場合の値
//ret : 読み取った値。読み取れなかった場合はdefvalueの値
LONG mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , LONG defvalue , bool* retIsReadable )const noexcept
{
	static_assert( sizeof( INT ) == sizeof( LONG ) , "sizeof int is not equal to sizeof long" );
	return (LONG)GetValue( key , index , subkey , (INT)defvalue );
}

//キーの値を64ビットINTの値として読み取る
LONGLONG mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , LONGLONG defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	LONGLONG val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%llx" , &val ) == 1 )
	{
		//0xで始まっている場合は16進数として読み取る
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%lld" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

}

//キーの値を32ビットUINTとして読み取る
UINT mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , UINT defvalue , bool* retIsReadable )const noexcept
{
	static_assert( sizeof( UINT ) == sizeof( DWORD ) , "unexpected data size" );
	return GetValue( key , index , subkey , static_cast<DWORD>( defvalue ) );
}

//キーの値を32ビットDWORDとして読み取る
DWORD mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , DWORD defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	DWORD val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%x" , &val ) == 1 )
	{
		//0xで始まっている場合は16進数として読み取る
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%u" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}

//キーの値を64ビットUINTとして読み取る
ULONGLONG mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , ULONGLONG defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	ULONGLONG val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%llx" , &val ) == 1 )
	{
		//0xで始まっている場合は16進数として読み取る
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%llu" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}

//キーの値を64ビットFLOATとして読み取る
FLOAT mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , FLOAT defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	FLOAT val;
	if( wchar_sscanf( itr->second.c_str() , L"%f" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	ResetReadable( retIsReadable );
	return defvalue;
}

//キーの値を64ビットDOUBLEとして読み取る
DOUBLE mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , DOUBLE defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	DOUBLE val;
	if( wchar_sscanf( itr->second.c_str() , L"%lf" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	ResetReadable( retIsReadable );
	return defvalue;
}

//キーの値をUNICODE文字列として読み取る
WString mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const WString& defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	SetReadable( retIsReadable );
	return itr->second;
}

//キーの値をASCII文字列として読み取る
AString mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const AString& defvalue , bool* retIsReadable )const noexcept
{
	//内部的にはUnicodeなので、一旦Unicodeとして読み取ってから、ASCII文字列に変換する。

	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	SetReadable( retIsReadable );
	return WString2AString( itr->second );
}

//キーの値をUNICODE文字列として読み取る
const wchar_t* mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const wchar_t* defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
	SetReadable( retIsReadable );
	return itr->second.c_str();
}

//キーの値を色として読み取る
RGBQUAD mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//設定値があるので、読み取りを試みます。
	DWORD r = 0;	//赤
	DWORD g = 0;	//緑
	DWORD b = 0;	//青
	DWORD a = 0;	//アルファ
	if( wchar_sscanf( itr->second.c_str() , L"#%02X%02X%02X%02X" , &r , &g , &b , &a ) < 3 )
	{
		//読み取れなかったので、エラーにします。
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//読めたので、これが結果。
	RGBQUAD result;
	result.rgbBlue = (BYTE)b;
	result.rgbGreen = (BYTE)g;
	result.rgbRed = (BYTE)r;
	result.rgbReserved = (BYTE)a;

	SetReadable( retIsReadable );
	return result;
}

//ブール値として、キーの値を読み取る
bool mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , bool defvalue , bool* retIsReadable )const noexcept
{
	const mInitFileSection::LookupValues< bool > BooleanValues =
	{
		{ L"yes"	, true  },
		{ L"true"	, true  },
		{ L"no"		, false },
		{ L"false"	, false },
	};
	return GetValue( key , BooleanValues , defvalue , retIsReadable );
}

mInitFileSection::Hexdecimal mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , Hexdecimal defvalue , bool* retIsReadable )const noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//キーに対応する文字列を取得
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//取得した文字列を数値に変換する
	DWORD val;
	if( wchar_sscanf( itr->second.c_str() , L"%x" , &val ) == 1 )
	{
		//0xで始まっている場合は16進数として読み取る
		SetReadable( retIsReadable );
		return Hexdecimal( val );
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}


//指定のキーに32ビットINTの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , INT newval )noexcept
{
	WString value;
	sprintf( value , L"%d" , newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーに32ビットLONGの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , LONG newval )noexcept
{
	return SetValue( key , index , subkey , (INT)newval );
}

//指定のキーに64ビットINTの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , LONGLONG newval )noexcept
{
	WString value;
	sprintf( value , L"%lld" , newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーに32ビットUINTの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , DWORD newval )noexcept
{
	WString value;
	sprintf( value , L"%u" , newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーに64ビットUINTの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , ULONGLONG newval )noexcept
{
	WString value;
	sprintf( value , L"%llu" , newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーに32ビットFLOATの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , FLOAT newval )noexcept
{
	WString value;
	sprintf( value , L"%f" , newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーに64ビットDOUBLEの値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , DOUBLE newval )noexcept
{
	WString value;
	sprintf( value , L"%lf" , newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーにUNICODE文字列を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const WString& newval )noexcept
{
	//設定値を取得
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	MyKeyValueMap[ tmpkey ] = newval;
	MyIsModified = true;
	return true;
}

//指定のキーにASCII文字列を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const AString& newval )noexcept
{
	WString value = AString2WString( newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーにUNICODE文字列を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const wchar_t* newval )noexcept
{
	WString value( newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーにASCII文字列を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const char* newval )noexcept
{
	AString value( newval );
	return SetValue( key , index , subkey , value );
}

//指定のキーに輝度値を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& newval )noexcept
{
	WString value;
	sprintf( value , L"#%02X%02X%02X%02X" , newval.rgbRed , newval.rgbGreen , newval.rgbBlue , newval.rgbReserved );
	return SetValue( key , index , subkey , value );
}

//指定のキーにGDI座標を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const mWindowPosition::POSITION& newval )noexcept
{
	WString value;
	if( newval.offset == 0 )
	{
		sprintf( value , L"%0.0f" , newval.rate );
	}
	else
	{
		sprintf( value , L"%0.0f+%d" , newval.rate , newval.offset );
	}
	return SetValue( key , index , subkey , value );
}

//指定のキーにWindow座標を設定する
bool mInitFileSection::SetValue( const WString& key , const mWindowPosition::WindowPosition& newval )noexcept
{
	bool result = true;
	result &= SetValue( key , L"left" , newval.left );
	result &= SetValue( key , L"right" , newval.right );
	result &= SetValue( key , L"top" , newval.top );
	result &= SetValue( key , L"bottom" , newval.bottom );

	return result;
}

//指定のキーにWindow座標を設定する
bool mInitFileSection::SetValue( const WString& key , INT index , const mWindowPosition::WindowPosition& newval )noexcept
{
	bool result = true;
	result &= SetValue( key , index , L"left" , newval.left );
	result &= SetValue( key , index , L"right" , newval.right );
	result &= SetValue( key , index , L"top" , newval.top );
	result &= SetValue( key , index , L"bottom" , newval.bottom );

	return result;
}

//指定のキーにブール値を書き込む
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , bool newval )noexcept
{
	WString value = ( newval ) ? ( L"true" ) : ( L"false" );
	return SetValue( key , index , subkey , value );
}

//存在するキーの一覧を返す
bool mInitFileSection::GetKeyList( WStringDeque& retList )const
{
	retList.clear();

	//すでに出現しているキーの一覧
	std::unordered_set< WString > keytable;

	//キーのスキャン
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( keytable.count( itr->first.key ) == 0 )
		{
			retList.push_back( itr->first.key );
			keytable.insert( itr->first.key );
		}
	}

	//並べ替え
	std::sort( retList.begin() , retList.end() );
	return true;
}

//存在するキーの一覧を返す
bool mInitFileSection::GetKeyList( KeyList& retList )const
{
	retList.clear();

	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		retList.push_back( itr->first );
	}

	std::sort( retList.begin() , retList.end() );
	return true;
}

//存在するキーの一覧を返す
//※この関数は、指定したキーのサブキーを返します
bool mInitFileSection::GetKeyList( const WString& key , KeyList& retList )const
{
	retList.clear();

	//キーのスキャン
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( key == itr->first.key )
		{
			retList.push_back( itr->first );
		}
	}

	//並べ替え
	std::sort( retList.begin() , retList.end() );
	return true;
}

bool mInitFileSection::GetIndexRange( const WString& key , INT& ret_min_index , INT& ret_max_index )const
{
	ret_min_index = 0;
	ret_max_index = 0;
	bool result = false;

	//キーのスキャン
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( key == itr->first.key )
		{
			if( itr->first.index < ret_min_index )
			{
				ret_min_index = itr->first.index;
			}
			else if( ret_max_index < itr->first.index )
			{
				ret_max_index = itr->first.index;
			}
			result = true;
		}
	}

	return true;
}

//存在するキーに含まれるインデックスのリストを返す
//retList : インデックスの一覧を格納する
//ret : 成功時真、失敗時偽(指定したキーが存在しない場合など)
bool mInitFileSection::GetIndexList( const WString& key , IndexList& retList )const
{
	retList.clear();
	bool result = false;

	//キーのスキャン
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( key == itr->first.key )
		{
			retList.push_back( itr->first.index );
			result = true;
		}
	}
	return true;
}

//ファイルにセクションの内容を出力する
bool mInitFileSection::Write( mFileWriteStream& fp )const
{
	bool result = true;
	WString str;

	//タイトルを出力
	switch( MySectionType )
	{
	case SectionType::SECTIONTYPE_NORMAL:
		sprintf( str , L"[%ls]\r\n" , MySectionName.c_str() );
		break;
	case SectionType::SECTIONTYPE_ROWBASE:
		sprintf( str , L"<%ls>\r\n" , MySectionName.c_str() );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"セクション形式が不正です" );
		sprintf( str , L"[%ls] ;section type error\r\n" , MySectionName.c_str() );
		break;
	}
	fp.WriteString( str );

	//キーの一覧を作成。キーをソートするため、GetKeyListを呼ぶ。
	KeyList keylist;
	if( !GetKeyList( keylist ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"キーのリストが取得できませんでした" );
		return false;
	}

	//各項目を出力
	for( KeyList::const_iterator itr = keylist.begin() ; itr != keylist.end() ; itr++ )
	{
		KeyValueMap::const_iterator entry = MyKeyValueMap.find( *itr );
		if( entry == MyKeyValueMap.end() )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"設定値が行方不明になりました" , itr->key );
			result = false;
		}
		else if( MySectionType == SectionType::SECTIONTYPE_NORMAL )
		{
			if( itr->subkey == L"" )
			{
				//サブキーではない場合
				if( itr->index != 0 )
				{
					sprintf( str , L"%ls[%d] = %ls\r\n" , itr->key.c_str() , itr->index , entry->second.c_str() );
				}
				else
				{
					sprintf( str , L"%ls = %ls\r\n" , itr->key.c_str() , entry->second.c_str() );
				}
			}
			else
			{
				//サブキーの場合
				sprintf( str , L".%ls = %ls\r\n" , itr->subkey.c_str() , entry->second.c_str() );
			}

			if( !fp.WriteString( str ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"ファイルへの書き込みが失敗しました" , itr->key );
				result = false;
			}
		}
		else if( MySectionType == SectionType::SECTIONTYPE_ROWBASE )
		{
			if( itr->subkey == L"" )
			{
				sprintf( str , L"%ls\r\n" , entry->second.c_str() );
			}
			else
			{
				sprintf( str , L".%ls = %ls\r\n" , itr->subkey.c_str() , entry->second.c_str() );
			}

			if( !fp.WriteString( str ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"ファイルへの書き込みが失敗しました" , itr->key );
				result = false;
			}
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"セクション形式が不正です" );
			return false;
		}
	}

	//出力ができたので、更新済みフラグをクリア
	//インスタンスはconstになっているが更新する
	const_cast< mInitFileSection* >( this )->MyIsModified = false;

	return result;
}

//存在するキーの数を取得する
DWORD mInitFileSection::GetCount( void )const
{
	return static_cast<DWORD>( MyKeyValueMap.size() );
}

bool mInitFileSection::SetSectionName( const WString& newname )
{
	MySectionName = newname;
	return true;
}

const WString& mInitFileSection::GetSectionName( void )const
{
	return MySectionName;
}

//セクション形式を変更する
// newtype : 新しい形式
// ret : 成功時真
bool mInitFileSection::SetSectionType( SectionType newtype )
{
	switch( newtype )
	{
	case SECTIONTYPE_NORMAL:
	case SECTIONTYPE_ROWBASE:
		MySectionType = newtype;
		return true;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"セクション形式が不正です" );
		break;
	}

	return false;
}

//セクション形式を取得する
// ret : セクション形式
mInitFileSection::SectionType mInitFileSection::GetSectionType( void )const
{
	switch( MySectionType )
	{
	case SECTIONTYPE_NORMAL:
	case SECTIONTYPE_ROWBASE:
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"セクション形式が不正であったので強制的に補正されました" );
		const_cast<SectionType>( MySectionType ) = SECTIONTYPE_NORMAL;
		break;
	}

	return MySectionType;
}

//更新されているか否かを返す
bool mInitFileSection::IsModified( void )const
{
	return MyIsModified;
}

