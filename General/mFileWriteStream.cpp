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
#include "mFileWriteStream.h"
#include "General/mErrorLogger.h"

mFileWriteStream::mFileWriteStream()
{
	MyWriteCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
}

mFileWriteStream::~mFileWriteStream()
{
	mFileWriteStream::FlushCache();
	MyHandle.Close();
	MyWriteCacheHead.reset();
}

bool mFileWriteStream::FlushCache( void )
{
	//�������ނ��̂��Ȃ��Ƃ��͂��̂܂ܖ߂�
	if( MyWriteCacheWritten == 0 )
	{
		return true;
	}

	//�t�@�C���̏������݂��s��
	DWORD written;
	bool result = true;
	if( !MyHandle.Write( MyWriteCacheHead.get() , MyWriteCacheWritten , written ) || ( written != MyWriteCacheWritten  ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Cannot write" );
		result = false;
	}

	//�o�b�t�@�����Z�b�g
	mFileWriteStream::ResetCache();

	return result;
}

void mFileWriteStream::ResetCache( void )
{
	MyWriteCacheRemain = MAX_BUFFER_SIZE;
	MyWriteCacheWritten = 0;
}

bool mFileWriteStream::Open( const mFile::Option& opt )
{

	//�t�@�C�����I�[�v��
	//����d�ɊJ�����Ƃ���Ƃ��̃��\�b�h�͎��s����
	if( !MyHandle.Open( opt ) )
	{
		return false;
	}

	//�t�@�C�����J�����Ƃ��o�����ꍇ�́A�t�@�C�������R�s�[���āA�L���b�V���𖳌���
	ResetCache();

	return true;
}

bool mFileWriteStream::Close( void )
{
	if( !FlushCache() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"FlushCache failed" );
	}
	return MyHandle.Close();
}

bool mFileWriteStream::Write( INT data )
{
	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}
	return true;
}

bool mFileWriteStream::SetPointer( ULONGLONG pos )
{
	FlushCache();
	if( !MyHandle.SetPointer( pos ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetPointer failed" );
		return false;
	}
	return true;
}

bool mFileWriteStream::MovePointer( LONGLONG distance )
{
	FlushCache();
	return MyHandle.MovePointer( distance );
}

//�t�@�C���|�C���^���t�@�C���̖����Ɉړ�
bool mFileWriteStream::SetPointerToEnd( void )
{
	FlushCache();
	return MyHandle.SetPointerToEnd();
}

//�t�@�C���|�C���^���t�@�C���̐擪�Ɉړ�
bool mFileWriteStream::SetPointerToBegin( void )
{
	FlushCache();
	return MyHandle.SetPointerToBegin();
}

bool mFileWriteStream::IsOpen( void )const
{
	return MyHandle.IsOpen();
}

ULONGLONG mFileWriteStream::GetPointer( void )const
{
	ULONGLONG result = 0;
	if( MyHandle.GetPosition( result ) )
	{
		result += MyWriteCacheWritten;
	}
	return result;
}

WString mFileWriteStream::GetPath( bool fullpath )const
{
	return MyHandle.GetPath( fullpath );
}

bool mFileWriteStream::GetFileSize( ULONGLONG& retSize )const
{
	retSize = 0;
	if( MyHandle.GetFileSize( retSize ) )
	{
		retSize += MyWriteCacheWritten;
		return true;
	}
	return false;
}

bool mFileWriteStream::GetFileSize( DWORD* high , DWORD& low )const
{
	ULONGLONG tmp;
	if( !GetFileSize( tmp ) )
	{
		return 0;
	}
	LARGE_INTEGER li;
	li.QuadPart = (LONGLONG)tmp;

	if( high )
	{
		*high = li.HighPart;
	}
	else if( li.HighPart )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�t�@�C���T�C�Y��4GB�𒴂��Ă��܂�" );
		return false;
	}
	low = li.LowPart;
	return true;
}

ULONGLONG mFileWriteStream::GetFileSize( void )const
{
	ULONGLONG tmp;
	if( !GetFileSize( tmp ) )
	{
		return 0;
	}
	return tmp;
}

bool mFileWriteStream::SetEof( void )
{
	FlushCache();
	return MyHandle.SetEof();
}
