//----------------------------------------------------------------------------
// ファイルユーティリティ関数群
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#define MFILEUTILITY_CPP_COMPILING
#include "mFileUtility.h"
#include "mErrorLogger.h"
#include <algorithm>
#include <memory>
#include <shlwapi.h>
#include <Shlobj.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

//トークンがWindowsの予約ファイル名と合致するか判定
// token : チェックするトークン
// ret : 含む場合false
static bool CheckReservedFileName( const WString& token )
{
	const WCHAR* BadNameList[] =
	{
		L"CON",		//コンソール
		L"AUX",		//補助デバイス
		L"PRN",		//プリンター＾
		L"NUL",		//ビットバケツ
		L"CLOCK",	//時刻デバイス
		L"CLOCK$",	//時刻デバイス
		L"COM1",	//シリアルポート
		L"COM2",	//シリアルポート
		L"COM3",	//シリアルポート
		L"COM4",	//シリアルポート
		L"COM5",	//シリアルポート
		L"COM6",	//シリアルポート
		L"COM7",	//シリアルポート
		L"COM8",	//シリアルポート
		L"COM9",	//シリアルポート
		L"COM¹",	//シリアルポート
		L"COM²",	//シリアルポート
		L"COM³",	//シリアルポート
		L"LPT1",	//パラレルポート
		L"LPT2",	//パラレルポート
		L"LPT3",	//パラレルポート
		L"LPT4",	//パラレルポート
		L"LPT5",	//パラレルポート
		L"LPT6",	//パラレルポート
		L"LPT7",	//パラレルポート
		L"LPT8",	//パラレルポート
		L"LPT9",	//パラレルポート
		L"LPT¹",	//パラレルポート
		L"LPT²",	//パラレルポート
		L"LPT³",	//パラレルポート
		nullptr
	};

	DWORD index = 0;
	while( BadNameList[ index ] )
	{
		if( token == BadNameList[ index ] )
		{
			return false;
		}
		index++;
	}

	return true;
}

//トークンがWindowsのファイル名に使えない文字を含むか判定
// token : チェックするトークン
// ret : 含む場合false
static BOOL CheckFileNameCharacter( const WString& token )
{
	const WCHAR BadCharList[] =
	{
		L'/' , L'\\' , L'?' , L'\"' , L'<' , L'>' , L'*' , L'|' , L':' , L'0'
	};

	DWORD index = 0;
	while( BadCharList[ index ] )
	{
		if( token.find( BadCharList[ index ] ) != WString::npos )
		{
			return false;
		}
		index++;
	}
	return true;
}

bool mFileUtility::IsSuitableFilename( const wchar_t* filename )
{
	return IsSuitableFilename( WString( filename ) );
}

bool mFileUtility::IsSuitableFilename( const WString& filename )
{
	//大文字で判定するために、大文字変換
	WString fname_upper = ToUpper( filename );

	//更に / を \\ に置き換える
	std::replace( fname_upper.begin() , fname_upper.end() , L'/' , L'\\' );

	//ファイル名を分解し、ディレクトリ名とファイル名について判定する
	WString Drive;
	WString Dir;
	WString Filename;
	WString Ext;
	if( !SplitPath( fname_upper , &Drive , &Dir , &Filename , &Ext ) )
	{
		return false;
	}

	//1.ディレクトリ
	{
		WStringVector dirs;
		ParseString( Dir , L'\\' , dirs );

		for( WStringVector::const_iterator itr = dirs.begin() ; itr != dirs.end() ; itr++ )
		{
			if( !CheckReservedFileName( *itr ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"ディレクトリ名は予約ファイル名です" , *itr );
				return false;
			}
			if( !CheckFileNameCharacter( *itr ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"ディレクトリ名に使用できない文字が含まれます" , *itr );
				return false;
			}
		}
	}

	//ファイル名
	if( !CheckReservedFileName( Filename ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ファイル名は予約ファイル名です" , Filename );
		return false;
	}
	if( !CheckFileNameCharacter( Filename ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ファイル名に使用できない文字が含まれます" , Filename );
		return false;
	}

	//拡張子
	if( !CheckFileNameCharacter( Ext ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"拡張子に使用できない文字が含まれます" , Ext );
		return false;
	}

	return true;

}


//パスをドライブ、ディレクトリ、ファイル名、拡張子に分割する
bool mFileUtility::SplitPath( const WString& path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt )
{
	return SplitPath( path.c_str() , retDrive , retDir , retFile , retExt );
}

//パスをドライブ、ディレクトリ、ファイル名、拡張子に分割する
bool mFileUtility::SplitPath( const wchar_t* path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt )
{
	WCHAR Drive   [ _MAX_DRIVE ];
	WCHAR Dir     [ _MAX_DIR   ];
	WCHAR FileName[ _MAX_FNAME ];
	WCHAR Ext     [ _MAX_EXT   ];

	if( _wsplitpath_s<_MAX_DRIVE,_MAX_DIR,_MAX_FNAME,_MAX_EXT>( path , Drive , Dir , FileName , Ext ) == 0 )
	{
		if( retDrive )
		{
			*retDrive = Drive;
		}
		if( retDir )
		{
			*retDir = Dir;
		}
		if( retFile )
		{
			*retFile = FileName;
		}
		if( retExt )
		{
			*retExt = Ext;
		}
		return true;
	}

	RaiseError( g_ErrorLogger , 0 , L"パスの分割が失敗しました" , ( path ) ? ( path ) : ( L"nullptr" ) );
	return false;
}

bool mFileUtility::RebuildPath( const WString& path , WString& retResult , bool drive , bool dir , bool filename , bool ext )
{
	return RebuildPath( path.c_str() , retResult , drive , dir , filename , ext );
}

bool mFileUtility::RebuildPath( const wchar_t* path , WString& retResult , bool drive , bool dir , bool filename , bool ext )
{
	WCHAR builtpath[ _MAX_PATH ];
	WString Drive;
	WString Dir;
	WString Filename;
	WString Ext;

	if( !SplitPath( path , &Drive , &Dir , &Filename , &Ext ) )
	{
		return false;
	}

	int result = _wmakepath_s(
		builtpath ,
		_MAX_PATH ,
		( drive )	 ? ( Drive.c_str()    ) : ( nullptr ) ,
		( dir )		 ? ( Dir.c_str()      ) : ( nullptr ) ,
		( filename ) ? ( Filename.c_str() ) : ( nullptr ) ,
		( ext )		 ? ( Ext.c_str()      ) : ( nullptr ) );
	
	if( result != 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"パスの再構築が失敗しました" , path );
		return false;
	}
	retResult = builtpath;
	return true;
}

bool mFileUtility::ReplacePath( const WString& path , WString& retResult , const WString* Drive , const WString* Dir , const WString* File , const WString* Ext )
{
	return ReplacePath( path , retResult ,
		( Drive ) ? ( Drive->c_str() ) : ( nullptr ) ,
		( Dir ) ? ( Dir->c_str() ) : ( nullptr ) ,
		( File ) ? ( File->c_str() ) : ( nullptr ) ,
		( Ext ) ? ( Ext->c_str() ) : ( nullptr ) 
	);
}

bool mFileUtility::ReplacePath( const WString& path , WString& retResult , const wchar_t* Drive , const wchar_t* Dir , const wchar_t* File , const wchar_t* Ext )
{
	WCHAR builtpath[ _MAX_PATH ];
	WString tmp_Drive;
	WString tmp_Dir;
	WString tmp_File;
	WString tmp_Ext;

	if( !SplitPath( path , &tmp_Drive , &tmp_Dir , &tmp_File , &tmp_Ext ) )
	{
		return false;
	}
	if( Drive )
	{
		tmp_Drive = Drive;
	}
	if( Dir )
	{
		tmp_Dir = Dir;
	}
	if( File )
	{
		tmp_File = File;
	}
	if( Ext )
	{
		tmp_Ext = Ext;
	}

	int result = _wmakepath_s(
		builtpath ,
		_MAX_PATH ,
		tmp_Drive.c_str() ,
		tmp_Dir.c_str() ,
		tmp_File.c_str() ,
		tmp_Ext.c_str() 
	);
	
	if( result != 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"パスの再構築が失敗しました" , path.c_str() );
		return false;
	}
	retResult = builtpath;
	return true;
}

bool mFileUtility::ReplacePath( const WString& path1 , const WString& path2 , WString& retPath , bool drive , bool dir , bool filename , bool ext )
{
	WCHAR builtpath[ _MAX_PATH ];

	WString path1_Drive;
	WString path1_Dir;
	WString path1_File;
	WString path1_Ext;
	if( !SplitPath( path1 , &path1_Drive , &path1_Dir , &path1_File , &path1_Ext ) )
	{
		return false;
	}
	WString path2_Drive;
	WString path2_Dir;
	WString path2_File;
	WString path2_Ext;
	if( !SplitPath( path2 , &path2_Drive , &path2_Dir , &path2_File , &path2_Ext ) )
	{
		return false;
	}
	int result = _wmakepath_s(
		builtpath ,
		_MAX_PATH ,
		( ( drive    ) ? ( path2_Drive.c_str() ) : ( path1_Drive.c_str() ) ),
		( ( dir      ) ? ( path2_Dir.c_str()   ) : ( path1_Dir.c_str()   ) ),
		( ( filename ) ? ( path2_File.c_str()  ) : ( path1_File.c_str()  ) ),
		( ( ext      ) ? ( path2_Ext.c_str()   ) : ( path1_Ext.c_str()   ) )
	);
	if( result != 0 )
	{
		return false;
	}
	retPath = builtpath;
	return true;
}


WString mFileUtility::CombinePath( const WString& dir , const WString& file , const WString& ext )
{
	WString result;
	
	result = dir;
	if( dir != L"" && dir.back() != L'\\' )
	{
		if( file == L"" )
		{
			if( ext == L"" )
			{
				return dir;
			}
			return dir + L"\\." + ext;
		}
		else if( ext == L"" )
		{
			return dir + L"\\" + file;
		}
		else if( file.back() != L'.' )
		{
			return dir + L"\\" + file + L"." + ext;
		}
		else
		{
			return dir + L"\\" + file + ext;
		}
	}
	else
	{
		if( file == L"" )
		{
			if( ext == L"" )
			{
				return dir;
			}
			else if( dir == L"" )
			{
				return L"." + ext;
			}
			else
			{
				return dir + L"\\." + ext;
			}
		}
		else if( ext == L"" )
		{
			return dir + file;
		}
		else if( file.back() != L'.' )
		{
			return dir + file + L"." + ext;
		}
		else
		{
			return dir + file + ext;
		}
	}
}

WString mFileUtility::GetFullPath( const WString& filename )noexcept
{
	return GetFullPath( filename.c_str() );
}

WString mFileUtility::GetFullPath( const wchar_t* filename )noexcept
{
	WCHAR* path = _wfullpath( nullptr , filename , _MAX_PATH );
	if( path == nullptr )
	{
		RaiseError( g_ErrorLogger , 0 , L"フルパスの取得に失敗しました" , filename );
		return L"";
	}

	WString result( path );
	free( path );
	return result;
}

//指定したパスが、あるパス以下であるかを判定します
bool mFileUtility::CheckPathTraversal( const WString& path , const WString& rootpath )noexcept
{
	return CheckPathTraversal( path.c_str() , rootpath.c_str() );
}

//指定したパスが、あるパス以下であるかを判定します
bool mFileUtility::CheckPathTraversal( const wchar_t* path , const wchar_t* rootpath )noexcept
{
	WString fullpath_str = GetFullPath( path );
	WString rootpath_str = GetFullPath( rootpath );

	//不正パス？
	if( fullpath_str == L"" || rootpath_str == L"" )
	{
		SetLastError( ERR_BADPATH );
		return false;
	}

	//ディレクトリ名にするために、最後に\\がついていなければくっつける
	if( *fullpath_str.rbegin() != L'\\' )
	{
		fullpath_str += L'\\';
	}
	if( *rootpath_str.rbegin() != L'\\' )
	{
		rootpath_str += L'\\';
	}

	//フルパスの先頭から、ルートと一致すればOK
	if( rootpath_str.find( fullpath_str ) == 0 )
	{
		return true;
	}

	SetLastError( ERROR_SUCCESS );
	return false;
}

//テンポラリフォルダのパスを得る
WString mFileUtility::GetTempDirPath( void )
{
	//テンポラリファイルのパスを得る
	DWORD path_len = GetTempPathW( 0 , 0 );
	std::unique_ptr<wchar_t> path( mNew wchar_t[ path_len ] );

	if( GetTempPathW( path_len , path.get() ) == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"テンポラリフォルダを取得できませんでした" );
		return L"";
	}
	return path.get();
}

//テンポラリファイルのパスを得る
WString mFileUtility::GetTempFilePath( const WString& folder , const WString& prefix )
{
	WCHAR temp_dll_buff[ MAX_PATH ];

	if( !GetTempFileNameW( folder.c_str() , prefix.c_str() , 0 , temp_dll_buff ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"テンポラリファイル名を取得できませんでした" );
		return L"";
	}
	return temp_dll_buff;
}

//パス名"from"からパス名"to"への相対パスを返します
// from : 相対パスの元
// to : 相対パスの先
// ret : fromからtoへの相対パス。相対パスが計算不能の場合空文字列。
WString mFileUtility::GetRelativePath( const WString& from , const WString& to ) noexcept
{
	wchar_t relpath[ MAX_PATH ];

	//fromがディレクトリかファイルかを判定して、パス名を調整
	WString from_str;
	DWORD attrfrom;
	if( !from.size() )
	{
		return L"";
	}
	else if( from.back() == L'\\' )
	{
		attrfrom = FILE_ATTRIBUTE_DIRECTORY;
		from_str = from.substr( 0 , from.size() - 1 );
	}
	else
	{
		attrfrom = 0;
		from_str = from;
	}

	//toがディレクトリかファイルかを判定して、パス名を調整
	WString to_str;
	DWORD attrto;
	if( !to.size() )
	{
		return L"";
	}
	else if( to.back() == L'\\' )
	{
		attrto = FILE_ATTRIBUTE_DIRECTORY;
		to_str = to.substr( 0 , to.size() - 1 );
	}
	else
	{
		attrto = 0;
		to_str = to;
	}

	//相対パスの計算
	if( PathRelativePathToW( relpath , from_str.c_str() , attrfrom , to.c_str() , attrto ) )
	{
		return WString( relpath );
	}
	return L"";
}

bool mFileUtility::CreateMiddleDirectory( const WString& path )noexcept
{
	WString directory;
	if( !mFileUtility::RebuildPath( path , directory , true , true , false , false ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ディレクトリ名を構築できません" );
		return false;
	}
	directory = GetFullPath( directory );

	switch( SHCreateDirectoryExW( nullptr , directory.c_str() , nullptr ) )
	{
	case ERROR_SUCCESS:
	case ERROR_FILE_EXISTS:
	case ERROR_ALREADY_EXISTS:
		return true;
	default:
		break;
	}

	RaiseError( g_ErrorLogger , 0 , L"ディレクトリを作成できませんでした" );
	return false;
}
