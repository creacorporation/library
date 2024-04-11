//----------------------------------------------------------------------------
// �R�}���h���C��������Ǘ��N���X
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MOPTIONPARSER_CPP_COMPILING
#include "mOptionParser.h"
#include "General/mErrorLogger.h"

mOptionParser::mOptionParser()
{
	//�R�}���h���C��������ǎ��
	LPWSTR command_line = GetCommandLineW();
	if( !command_line )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"GetCommandLineW failed" );
		return;
	}

	//argc/argv�`���Ƀp�[�X����
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW( command_line , &argc );
	if( argv == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"CommandLineToArgvW failed" );
		return;
	}

	//�����o�ϐ��Ƃ��ĕۑ�
	MyArgs.resize( argc );
	for( int i = 0 ; i < argc ; i++ )
	{
		MyArgs[ i ] = argv[ i ];
	}
	LocalFree( (HGLOBAL)argv );
	return;

}

bool mOptionParser::Check( WCHAR opt_char )const
{
	return 0 <= GetPosition( opt_char );
}

bool mOptionParser::Check( const WString& opt_string )const
{
	return 0 <= GetPosition( opt_string );
}

bool mOptionParser::Check( WCHAR opt_char , const WString& opt_string )const
{
	return 0 <= GetPosition( opt_char , opt_string );
}

INT mOptionParser::GetPosition( WCHAR opt_char )const
{
	for( DWORD i = 0 ; i < MyArgs.size() ; i++ )
	{
		if( GetArgType( i ) == ArgType::ARGTYPE_OPT_CHAR )
		{
			//�����`��
			if( MyArgs[ i ].find( opt_char ) != WString::npos )
			{
				return i;
			}
		}
	}
	return -1;
}

INT mOptionParser::GetPosition( const WString& opt_string )const
{
	//������`���̏ꍇ�́A���S��v�Ŕ��肷��
	WString req_string = L"--" + opt_string;

	for( DWORD i = 0 ; i < MyArgs.size() ; i++ )
	{
		if( MyArgs[ i ] == req_string )
		{
			return i;
		}
	}
	return -1;
}

INT mOptionParser::GetPosition( WCHAR opt_char , const WString& opt_string )const
{
	//�����`���Ń`�F�b�N
	INT index = GetPosition( opt_string );
	if( 0 <= index )
	{
		return index;
	}

	//������Ȃ��̂ŕ�����`���Ń`�F�b�N
	return GetPosition( opt_char );
}

INT mOptionParser::GetLastOptPosition( void )const
{
	if( MyArgs.size() == 0 )
	{
		return -1;
	}

	//�R�}���h���C�������̐���INT�ł����Ɍ��܂��Ă���̂�OK
	for( INT i = (INT)MyArgs.size() - 1 ; 0 <= i ; i-- )
	{
		if( GetArgType( i ) != ArgType::ARGTYPE_NOTOPTION )
		{
			return i;
		}
	}
	return -1;
}

INT mOptionParser::GetFirstOptPosition( void )const
{
	if( MyArgs.size() == 0 )
	{
		return -1;
	}

	//�R�}���h���C�������̐���INT�ł����Ɍ��܂��Ă���̂�OK
	for( INT i = 0 ; 0 < MyArgs.size() ; i++ )
	{
		if( GetArgType( i ) != ArgType::ARGTYPE_NOTOPTION )
		{
			return i;
		}
	}
	return -1;

}


mOptionParser::ArgType mOptionParser::GetArgType( INT index )const
{
	//�͈͊O�H
	if( index < 0 || MyArgs.size() <= (DWORD)index )
	{
		return ArgType::ARGTYPE_ERROR;
	}

	//1�����ڂ�-��������A�����`����������`��
	if( MyArgs[ index ][ 0 ] == L'-' )
	{
		if( 2 < MyArgs[ index ].size() && MyArgs[ index ][ 1 ] == L'-' )
		{
			//������`��
			return ArgType::ARGTYPE_OPT_STRING;
		}
		else
		{
			//�����`��
			return ArgType::ARGTYPE_OPT_CHAR;
		}
	}
	//1�����ڂ�/�������當���`��
	if( MyArgs[ index ][ 0 ] == L'/' )
	{
		//�����`��
		return ArgType::ARGTYPE_OPT_CHAR;
	}
	//�I�v�V�����ł͂Ȃ�
	return ArgType::ARGTYPE_NOTOPTION;
}

WString mOptionParser::GetString( WCHAR opt_char , const WString& def_string )const
{
	INT index = GetPosition( opt_char );
	if( index < 0 )
	{
		return def_string;
	}
	if( GetArgType( index + 1 ) == ArgType::ARGTYPE_NOTOPTION )
	{
		return MyArgs[ index + 1 ];
	}
	return def_string;
}

WString mOptionParser::GetString( const WString& opt_string , const WString& def_string )const
{
	INT index = GetPosition( opt_string );
	if( index < 0 )
	{
		return def_string;
	}
	if( GetArgType( index + 1 ) == ArgType::ARGTYPE_NOTOPTION )
	{
		return MyArgs[ index + 1 ];
	}
	return def_string;
}

WString mOptionParser::GetString( WCHAR opt_char , const WString& opt_string , const WString& def_string )const
{
	INT index = GetPosition( opt_char , opt_string );
	if( index < 0 )
	{
		return def_string;
	}
	if( GetArgType( index + 1 ) == ArgType::ARGTYPE_NOTOPTION )
	{
		return MyArgs[ index + 1 ];
	}
	return def_string;
}

WString mOptionParser::GetStringIndex( INT index , const WString& def_string )const
{
	if( GetArgType( index ) == ArgType::ARGTYPE_ERROR )
	{
		return def_string;
	}
	return MyArgs[ index ];
}

WStringVector mOptionParser::GetStringArray( WCHAR opt_char , const WString& def_string )const
{
	INT index = GetPosition( opt_char );
	return GetStringArrayIndex( ( 0 <= index ) ? ( index + 1 ) : ( -1 ) , def_string );
}

WStringVector mOptionParser::GetStringArray( const WString& opt_string , const WString& def_string )const
{
	INT index = GetPosition( opt_string );
	return GetStringArrayIndex( ( 0 <= index ) ? ( index + 1 ) : ( -1 ) , def_string );
}

WStringVector mOptionParser::GetStringArray( WCHAR opt_char , const WString& opt_string , const WString& def_string )const
{
	INT index = GetPosition( opt_char , opt_string );
	return GetStringArrayIndex( ( 0 <= index ) ? ( index + 1 ) : ( -1 ) , def_string );
}

WStringVector mOptionParser::GetStringArrayIndex( INT index , const WString& def_string )const
{
	WStringVector result;

	if( 0 <= index )
	{
		for( ; (DWORD)index < MyArgs.size() ; index++ )
		{
			if( GetArgType( index ) != ArgType::ARGTYPE_NOTOPTION )
			{
				break;
			}
			result.push_back( MyArgs[ index ] );
		}
	}

	if( result.size() == 0 )
	{
		result.push_back( def_string );
	}
	return result;

}

