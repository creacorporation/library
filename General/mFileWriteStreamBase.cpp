//----------------------------------------------------------------------------
// �X�g���[�~���O�t�@�C���������ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MFILEWRITESTREAM_CPP_COMPILING
#include "mFileWriteStreamBase.h"
#include "General/mErrorLogger.h"

mFileWriteStreamBase::mFileWriteStreamBase()
{
	MyWriteCacheRemain = 0;
	MyWriteCacheWritten = 0;
}

mFileWriteStreamBase::~mFileWriteStreamBase()
{
}


bool mFileWriteStreamBase::WriteString( const AString& line )	//ANSI���������������
{

	for( DWORD i = 0 ; i < line.size() ; i++ )
	{
		if( !Write( line.at( i ) ) )
		{
			WString error_str = AString2WString( line );
			RaiseAssert( g_ErrorLogger , i , L"Write ANSI failed : " + error_str );
			return false;
		}
	}
	return true;
}


bool mFileWriteStreamBase::WriteString( const WString& line )	//UNICODE���������������
{

	for( DWORD i = 0 ; i < line.size() ; i++ )
	{
		wchar_t dt =  line.at( i );

		if( !Write( ( dt >> 0 ) & 0x000000FF ) )
		{
			RaiseAssert( g_ErrorLogger , i * 2 + 0 , L"Write UNICODE failed : " + line );
			return false;
		}
		if( !Write( ( dt >> 8 ) & 0x000000FF ) )
		{
			RaiseAssert( g_ErrorLogger , i * 2 + 1 , L"Write UNICODE failed : " + line );
			return false;
		}
	}
	return true;

}

bool mFileWriteStreamBase::WriteString( const char* line )	//ANSI���������������
{
	if( !line )
	{
		return false;
	}
	return WriteString( AString( line ) );
}

bool mFileWriteStreamBase::WriteString( const wchar_t* line )	//UNICODE���������������
{
	if( line == 0 )
	{
		return true;
	}
	return WriteString( WString( line ) );
}

//�w��T�C�Y�������݂܂�
bool mFileWriteStreamBase::WriteBinary( const BYTE* buffer , size_t size )
{
	for( size_t i = 0 ; i < size ; i++ )
	{
		if( !Write( *buffer ) )
		{
			RaiseAssert( g_ErrorLogger , i , L"write binary failed" );
			return false;
		}
		buffer++;
	}
	return true;
}

//�w��X�g���[������ǂݎ���Ă��̂܂܏������݂܂�
//���w��o�C�g�������ނ��A�ǂݍ��݌��X�g���[����EOF�ɂȂ�܂ŏ������݂܂�
//fp : �������݌�
//sz : �������ރo�C�g��
//retWritten : �������񂾃T�C�Y(�s�v�Ȃ�nullptr�ŉ�)
//ret : ������true
bool mFileWriteStreamBase::WriteStream( mFileReadStreamBase& fp , size_t sz , size_t* retWritten )
{
	BYTE buffer[ 65536 ];
	while( !fp.IsEOF() && sz )
	{
		size_t readsize = ( sz < sizeof( buffer ) ) ? ( sz ) : ( sizeof( buffer ) );
		if( !fp.ReadBinary( buffer , readsize , &readsize ) )
		{
			if( readsize == 0 )
			{
				continue;
			}
		}
		if( !WriteBinary( buffer , readsize ) )
		{
			return false;
		}
		sz -= readsize;
		if( retWritten )
		{
			*retWritten += readsize;
		}
	}
	return true;
}

//�w��X�g���[������ǂݎ���Ă��̂܂܏������݂܂�
//���ǂݍ��݌��X�g���[����EOF�ɂȂ�܂ŏ������݂܂�
//fp : �������݌�
//retWritten : �������񂾃T�C�Y(�s�v�Ȃ�nullptr�ŉ�)
//ret : ������true
bool mFileWriteStreamBase::WriteStream( mFileReadStreamBase& fp , size_t* retWritten )
{
	BYTE buffer[ 65536 ];
	while( !fp.IsEOF() )
	{
		size_t readsize;
		if( !fp.ReadBinary( buffer , sizeof( buffer ) , &readsize ) )
		{
			if( readsize == 0 )
			{
				continue;
			}
		}
		if( !WriteBinary( buffer , readsize ) )
		{
			return false;
		}
		if( retWritten )
		{
			*retWritten += readsize;
		}
	}
	return true;
}

