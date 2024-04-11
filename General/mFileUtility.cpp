//----------------------------------------------------------------------------
// �t�@�C�����[�e�B���e�B�֐��Q
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
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

//�g�[�N����Windows�̗\��t�@�C�����ƍ��v���邩����
// token : �`�F�b�N����g�[�N��
// ret : �܂ޏꍇfalse
static bool CheckReservedFileName( const WString& token )
{
	const WCHAR* BadNameList[] =
	{
		L"CON",		//�R���\�[��
		L"AUX",		//�⏕�f�o�C�X
		L"PRN",		//�v�����^�[�O
		L"NUL",		//�r�b�g�o�P�c
		L"CLOCK",	//�����f�o�C�X
		L"CLOCK$",	//�����f�o�C�X
		L"COM1",	//�V���A���|�[�g
		L"COM2",	//�V���A���|�[�g
		L"COM3",	//�V���A���|�[�g
		L"COM4",	//�V���A���|�[�g
		L"COM5",	//�V���A���|�[�g
		L"COM6",	//�V���A���|�[�g
		L"COM7",	//�V���A���|�[�g
		L"COM8",	//�V���A���|�[�g
		L"COM9",	//�V���A���|�[�g
		L"LPT1",	//�p�������|�[�g
		L"LPT2",	//�p�������|�[�g
		L"LPT3",	//�p�������|�[�g
		L"LPT4",	//�p�������|�[�g
		L"LPT5",	//�p�������|�[�g
		L"LPT6",	//�p�������|�[�g
		L"LPT7",	//�p�������|�[�g
		L"LPT8",	//�p�������|�[�g
		L"LPT9",	//�p�������|�[�g
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

//�g�[�N����Windows�̃t�@�C�����Ɏg���Ȃ��������܂ނ�����
// token : �`�F�b�N����g�[�N��
// ret : �܂ޏꍇfalse
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
	//�啶���Ŕ��肷�邽�߂ɁA�啶���ϊ�
	WString fname_upper = ToUpper( filename );

	//�X�� / �� \\ �ɒu��������
	std::replace( fname_upper.begin() , fname_upper.end() , L'/' , L'\\' );

	//�t�@�C�����𕪉����A�f�B���N�g�����ƃt�@�C�����ɂ��Ĕ��肷��
	WString Drive;
	WString Dir;
	WString Filename;
	WString Ext;
	if( !SplitPath( fname_upper , &Drive , &Dir , &Filename , &Ext ) )
	{
		return false;
	}

	//1.�f�B���N�g��
	{
		WStringVector dirs;
		ParseString( Dir , L'\\' , dirs );

		for( WStringVector::const_iterator itr = dirs.begin() ; itr != dirs.end() ; itr++ )
		{
			if( !CheckReservedFileName( *itr ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"�f�B���N�g�����͗\��t�@�C�����ł�" , *itr );
				return false;
			}
			if( !CheckFileNameCharacter( *itr ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"�f�B���N�g�����Ɏg�p�ł��Ȃ��������܂܂�܂�" , *itr );
				return false;
			}
		}
	}

	//�t�@�C����
	if( !CheckReservedFileName( Filename ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�t�@�C�����͗\��t�@�C�����ł�" , Filename );
		return false;
	}
	if( !CheckFileNameCharacter( Filename ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�t�@�C�����Ɏg�p�ł��Ȃ��������܂܂�܂�" , Filename );
		return false;
	}

	//�g���q
	if( !CheckFileNameCharacter( Ext ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�g���q�Ɏg�p�ł��Ȃ��������܂܂�܂�" , Ext );
		return false;
	}

	return true;

}


//�p�X���h���C�u�A�f�B���N�g���A�t�@�C�����A�g���q�ɕ�������
bool mFileUtility::SplitPath( const WString& path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt )
{
	return SplitPath( path.c_str() , retDrive , retDir , retFile , retExt );
}

//�p�X���h���C�u�A�f�B���N�g���A�t�@�C�����A�g���q�ɕ�������
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

	RaiseError( g_ErrorLogger , 0 , L"�p�X�̕��������s���܂���" , ( path ) ? ( path ) : ( L"nullptr" ) );
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
		RaiseError( g_ErrorLogger , 0 , L"�p�X�̍č\�z�����s���܂���" , path );
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
		RaiseError( g_ErrorLogger , 0 , L"�p�X�̍č\�z�����s���܂���" , path.c_str() );
		return false;
	}
	retResult = builtpath;
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
		RaiseError( g_ErrorLogger , 0 , L"�t���p�X�̎擾�Ɏ��s���܂���" , filename );
		return L"";
	}

	WString result( path );
	free( path );
	return result;
}

//�w�肵���p�X���A����p�X�ȉ��ł��邩�𔻒肵�܂�
bool mFileUtility::CheckPathTraversal( const WString& path , const WString& rootpath )noexcept
{
	return CheckPathTraversal( path.c_str() , rootpath.c_str() );
}

//�w�肵���p�X���A����p�X�ȉ��ł��邩�𔻒肵�܂�
bool mFileUtility::CheckPathTraversal( const wchar_t* path , const wchar_t* rootpath )noexcept
{
	WString fullpath_str = GetFullPath( path );
	WString rootpath_str = GetFullPath( rootpath );

	//�s���p�X�H
	if( fullpath_str == L"" || rootpath_str == L"" )
	{
		SetLastError( ERR_BADPATH );
		return false;
	}

	//�f�B���N�g�����ɂ��邽�߂ɁA�Ō��\\�����Ă��Ȃ���΂�������
	if( *fullpath_str.rbegin() != L'\\' )
	{
		fullpath_str += L'\\';
	}
	if( *rootpath_str.rbegin() != L'\\' )
	{
		rootpath_str += L'\\';
	}

	//�t���p�X�̐擪����A���[�g�ƈ�v�����OK
	if( rootpath_str.find( fullpath_str ) == 0 )
	{
		return true;
	}

	SetLastError( ERROR_SUCCESS );
	return false;
}

//�e���|�����t�H���_�̃p�X�𓾂�
WString mFileUtility::GetTempDirPath( void )
{
	//�e���|�����t�@�C���̃p�X�𓾂�
	DWORD path_len = GetTempPathW( 0 , 0 );
	std::unique_ptr<wchar_t> path( mNew wchar_t[ path_len ] );

	if( GetTempPathW( path_len , path.get() ) == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�e���|�����t�H���_���擾�ł��܂���ł���" );
		return L"";
	}
	return path.get();
}

//�e���|�����t�@�C���̃p�X�𓾂�
WString mFileUtility::GetTempFilePath( const WString& folder , const WString& prefix )
{
	WCHAR temp_dll_buff[ MAX_PATH ];

	if( !GetTempFileNameW( folder.c_str() , prefix.c_str() , 0 , temp_dll_buff ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�e���|�����t�@�C�������擾�ł��܂���ł���" );
		return L"";
	}
	return temp_dll_buff;
}

//�p�X��"from"����p�X��"to"�ւ̑��΃p�X��Ԃ��܂�
// from : ���΃p�X�̌�
// to : ���΃p�X�̐�
// ret : from����to�ւ̑��΃p�X�B���΃p�X���v�Z�s�\�̏ꍇ�󕶎���B
WString mFileUtility::GetRelativePath( const WString& from , const WString& to ) noexcept
{
	wchar_t relpath[ MAX_PATH ];

	//from���f�B���N�g�����t�@�C�����𔻒肵�āA�p�X���𒲐�
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

	//to���f�B���N�g�����t�@�C�����𔻒肵�āA�p�X���𒲐�
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

	//���΃p�X�̌v�Z
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
		RaiseError( g_ErrorLogger , 0 , L"�f�B���N�g�������\�z�ł��܂���" );
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

	RaiseError( g_ErrorLogger , 0 , L"�f�B���N�g�����쐬�ł��܂���ł���" );
	return false;
}
