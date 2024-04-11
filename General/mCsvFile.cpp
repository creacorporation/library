//----------------------------------------------------------------------------
// CSV�t�@�C���n���h��
// Copyright (C) 2020-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#include "mCsvFile.h"
#include <General/mFileReadStream.h>
#include <General/mErrorLogger.h>

static bool OpenCsvFile( const WString& filename , mFileReadStream& retfp )
{
	//�t�@�C�����J���Ƃ��̏��
	mFileReadStream::Option opt;
	opt.AccessRead = true;
	opt.AccessWrite = false;
	opt.Mode = mFileReadStream::CreateMode::OpenExisting;
	opt.ShareRead = true;
	opt.ShareWrite = false;
	opt.Path = filename;

	//�t�@�C�����J��
	if( !retfp.Open( opt ) )
	{
		//�t�@�C�����J���Ȃ�����
		RaiseError( g_ErrorLogger , 0 , L"�t�@�C�����J���܂���" );
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

	//���ꕶ���̌��o���
	bool quote_detect = false;	//�_�u���N�H�[�e�[�V�����̌��o
	bool quoted = false;		//�_�u���N�H�[�e�[�V�����ň͂܂�Ă���
	bool cr_detect = false;		//���s�R�[�h(CR)�̌��o
	Cell< base_type > cell;		//�ǂݎ�蒆�̃Z��
	Row< base_type > row;		//�ǂݎ�蒆�̍s

	//�s���ړ����郉���_
	auto InsertCell = [&row,&cell]( bool noempty ) -> void 
	{
		if( !noempty || !cell.empty() )
		{
			row.push_back( std::move( cell ) );
			cell = Cell< base_type >();
		}
	};

	//�s���ړ����郉���_
	auto InsertRow = [&retTable,&row]( bool noempty ) -> void 
	{
		if( !noempty || !row.empty() )
		{
			retTable.push_back( std::move( row ) );
			row = Row< base_type >();
		}
	};

	//�ǂݎ�胁�C�����[�v
	while( !fp.IsEOF() )
	{
		base_type c;
		if( !fp.ReadSingle( c ) )
		{
			continue;
		}

		//���s����
		if( c == (base_type)'\n' )
		{
			if( !quoted )
			{
				//�s�̋�؂�(LF/CRLF���s)
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
					//�s�̋�؂�(CR���s)�{CR
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
				//�s�̋�؂�(CR���s)
				InsertCell( true );
				InsertRow( false );
			}
			cr_detect = false;
		}

		//�_�u���N�H�[�e�[�V��������
		if( c == (base_type)'"' )
		{
			if( quote_detect )
			{
				//2�A��
				quote_detect = false;
			}
			else
			{
				//�_�u���N�H�[�e�[�V�������o
				quote_detect = true;
				continue;
			}
		}
		else
		{
			if( quote_detect )
			{
				//�͂݊J�n�E�I��
				quoted = !quoted;
				quote_detect = false;
			}
		}

		//�R���}����
		if( c == (base_type)',' )
		{
			if( !quoted )
			{
				//��(�Z��)�̋�؂�
				InsertCell( false );
				continue;
			}
		}

		//���̑�
		cell.push_back( c );
	}

	InsertCell( true );
	InsertRow( true );
	return true;
}

template< class base_type >
static void FillEmptyCell( Table< base_type >& retTable , std::basic_string< base_type >* empty_str )
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

bool mCsvFile::ReadCsvFile( const WString& filename , WTable& retTable , WString* empty_str )
{
	//�t�@�C�����J���i�e���v���[�g�Ɗ֌W�Ȃ�����͊O�ɒǂ��o���j
	mFileReadStream fp;
	if( !OpenCsvFile( filename , fp ) )
	{
		return false;
	}
	//�ǂݎ��
	return ReadCsvFile( fp , retTable , empty_str );
}

bool mCsvFile::ReadCsvFile( const WString& filename , ATable& retTable , AString* empty_str )
{
	//�t�@�C�����J���i�e���v���[�g�Ɗ֌W�Ȃ�����͊O�ɒǂ��o���j
	mFileReadStream fp;
	if( !OpenCsvFile( filename , fp ) )
	{
		return false;
	}
	//�ǂݎ��
	return ReadCsvFile( fp , retTable , empty_str );
}

//CSV�t�@�C����ǂݎ���Ĕz��Ɋi�[����
//�E�ǂݎ��Ƃ��̕����R�[�h�́AretTable�Ɏw�肵���^�ɍ��킹��
//stream : �ǂݎ��X�g���[��
//retTable : �i�[��
//empty_str : �ǂݎ�茋�ʂ������`�ɂȂ��Ă��Ȃ��Ƃ��i�s�^�񐔂��ł��ڂ��ɂȂ��Ă���Ƃ��j
//�@�@�@�@�@�@�����`�ɂȂ�悤�ɂ��̃f�[�^�����Đ��`����Bnullptr�Ȃ琮�`���Ȃ��B
//ret : �������^
bool mCsvFile::ReadCsvFile( mFileReadStreamBase& stream , WTable& retTable , WString* empty_str )
{
	//�ǂݎ��
	if( !ReadCsvFileTemplate( stream , retTable ) )
	{
		return false;
	}
	FillEmptyCell( retTable , empty_str );
	return true;
}

//CSV�t�@�C����ǂݎ���Ĕz��Ɋi�[����
//�E�ǂݎ��Ƃ��̕����R�[�h�́AretTable�Ɏw�肵���^�ɍ��킹��
//stream : �ǂݎ��X�g���[��
//retTable : �i�[��
//empty_str : �ǂݎ�茋�ʂ������`�ɂȂ��Ă��Ȃ��Ƃ��i�s�^�񐔂��ł��ڂ��ɂȂ��Ă���Ƃ��j
//�@�@�@�@�@�@�����`�ɂȂ�悤�ɂ��̃f�[�^�����Đ��`����Bnullptr�Ȃ琮�`���Ȃ��B
//ret : �������^
bool mCsvFile::ReadCsvFile( mFileReadStreamBase& stream , ATable& retTable , AString* empty_str )
{
	//�ǂݎ��
	if( !ReadCsvFileTemplate( stream , retTable ) )
	{
		return false;
	}
	FillEmptyCell( retTable , empty_str );
	return true;
}

