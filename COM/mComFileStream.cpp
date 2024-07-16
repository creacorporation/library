//----------------------------------------------------------------------------
// COM�ւ̃X�g���[�~���O�������ݑ���
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MCOMFILESTREAM_CPP_COMPILING
#include "mComFileStream.h"
#include "General/mErrorLogger.h"
#include "General/mFileUtility.h"
#include <shlwapi.h>

mComFileStream::mComFileStream( DWORD buffersize )
	: MAX_BUFFER_SIZE( buffersize )
{
	ResetWriteCache();
	MyStream = nullptr;
}

mComFileStream::~mComFileStream()
{
	MyStream->Release();
}

bool mComFileStream::Close( void )
{
	return false;
}

bool mComFileStream::Write( INT data )
{

	//�L���b�V���̎c�肪����H
	if( MyWriteCacheRemain == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�������݃L���b�V���̎c�ʂ�����܂���" );
		return false;
	}

	MyWriteCacheHead[ MyWriteCacheWritten ] = (BYTE)data;
	MyWriteCacheRemain--;
	MyWriteCacheWritten++;

	if( MyWriteCacheRemain == 0 )
	{
		return FlushCache();
	}
	return true;
}

bool mComFileStream::IsOpen( void )const
{
	if( !MyStream )
	{
		return false;
	}
	return true;
}

INT mComFileStream::Read( void )
{
	if( !IsOpen() )
	{
		return EOF;
	}

	//UnRead���ꂽ����������ꍇ�̓\����Ԃ�
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	//�L���b�V���̎c�ʂ�����΃L���b�V����ǂݍ���
	//�L���b�V���̎c�ʂ��Ȃ��Ȃ�L���[����擾����
	if( MyReadCacheRemain == 0 )
	{
		//�ǂݎ��L���b�V���ɃZ�b�g
		if( MyReadCacheHead.get() == nullptr )
		{
			MyReadCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
		}

		HRESULT hr = MyStream->Read( MyReadCacheHead.get() , MAX_BUFFER_SIZE , &MyReadCacheRemain );
		if( SUCCEEDED( hr ) )
		{
			MyReadCacheCurrent = 0;
			if( MyReadCacheRemain == 0 )
			{
				return EOF;
			}
		}
		else
		{
			MyReadCacheCurrent = 0;
			MyReadCacheRemain = 0;
			return EOF;
		}
	}

	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;
	return result;
}

bool mComFileStream::IsEOF( void )const
{
	return !IsOpen();
}

bool mComFileStream::FlushCache( void )
{
	//�������ނ��̂��Ȃ��Ƃ��͂��̂܂ܖ߂�
	if( MyWriteCacheWritten == 0 )
	{
		return true;
	}

	//�t�@�C���̏������݂��s��
	DWORD written;
	bool result = true;
	HRESULT hr = MyStream->Write( MyWriteCacheHead.get() , MyWriteCacheWritten , &written );

	if( !SUCCEEDED( hr ) || ( written != MyWriteCacheWritten  ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Cannot write" );
		result = false;
	}

	//�o�b�t�@�����Z�b�g
	ResetWriteCache();

	return result;
}

void mComFileStream::ResetWriteCache( void )
{
	//�����L���b�V�����Ȃ��悤�Ȃ�쐬
	if( MyWriteCacheHead.get() == nullptr )
	{
		MyWriteCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
		if( !MyWriteCacheHead )
		{
			MyWriteCacheRemain = 0;
			MyWriteCacheWritten = 0;
			return;
		}
	}
	MyWriteCacheRemain = MAX_BUFFER_SIZE;
	MyWriteCacheWritten = 0;
}


IStream* mComFileStream::Get( void )const
{
	return MyStream;
}

mComFileStream::operator IStream*( void )const
{
	return Get();
}

bool mComFileStream::SetPointer( ULONGLONG pos )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = pos;

	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_SET , nullptr );
	return SUCCEEDED( hr );
}

bool mComFileStream::MovePointer( LONGLONG distance )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = distance;

	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_CUR , nullptr );
	return SUCCEEDED( hr );
}

bool mComFileStream::SetPointerToEnd( void )
{
	if( !IsOpen() )
	{
		return false;
	}

	LARGE_INTEGER v;
	v.QuadPart = 0;

	HRESULT hr = MyStream->Seek( v , STREAM_SEEK_END , nullptr );
	return SUCCEEDED( hr );
}

bool mComFileStream::SetPointerToBegin( void )
{
	return SetPointer( 0 );
}

//�t�@�C�����J���܂��B
bool mComFileStream::Open( const mFile::Option& opt )
{
	if( IsOpen() )
	{
		return false;
	}

	DWORD mode = 0;
	BOOL create = FALSE;
	//�A�N�Z�X���[�h
	if( opt.AccessWrite )
	{
		mode |= ( opt.AccessRead ) ? ( STGM_READWRITE ) : ( STGM_WRITE );
	}
	else
	{
		mode |= STGM_READ;
	}
	//���L
	if( opt.ShareRead && opt.ShareWrite )
	{
		mode |= STGM_SHARE_DENY_NONE;
	}
	else if( opt.ShareRead && !opt.ShareWrite )
	{
		mode |= STGM_SHARE_DENY_WRITE;
	}
	else if( !opt.ShareRead && opt.ShareWrite )
	{
		mode |= STGM_SHARE_DENY_READ;
	}
	else
	{
		mode |= STGM_SHARE_EXCLUSIVE;
	}
	//�J����
	switch( opt.Mode )
	{
	case mFile::CreateMode::CreateNew:				//�V�����t�@�C�����쐬�B���łɂ���ꍇ�̓G���[
		mode |= STGM_FAILIFTHERE;
		create = TRUE;
		break;
	case mFile::CreateMode::CreateAlways:			//�V�����t�@�C�����쐬�B���łɂ���ꍇ�͏㏑���i���g���̂Ă�j
		mode |= STGM_CREATE;
		create = FALSE;
		break;
	case mFile::CreateMode::OpenExisting:			//���łɂ���t�@�C�����J���B�Ȃ��ꍇ�̓G���[
		mode |= STGM_FAILIFTHERE;
		create = FALSE;
		break;
	case mFile::CreateMode::OpenAlways:				//�����łɂ���t�@�C�����J���B�Ȃ��ꍇ�͐V�����t�@�C�����J��
		if( PathFileExistsW( opt.Path.c_str() ) )
		{
			mode |= STGM_FAILIFTHERE;
			create = FALSE;
		}
		else
		{
			mode |= STGM_CREATE;
			create = FALSE;
		}
		break;
	case mFile::CreateMode::TruncateExisting:		//�����łɂ���t�@�C�����J���Ē��g���̂Ă�B�Ȃ��ꍇ�̓G���[�B
		mode |= STGM_FAILIFTHERE;
		create = FALSE;
		break;
	case mFile::CreateMode::CreateWithDirectory:	//�V�����t�@�C�����쐬�B�f�B���N�g�����Ȃ��ꍇ�̓f�B���N�g�����쐬����B���łɂ���ꍇ�͏㏑���i���g���̂Ă�j�B
		if( !mFileUtility::CreateMiddleDirectory( opt.Path ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"���Ԃ̃f�B���N�g�����쐬�ł��܂���" + opt.Path );
			return false;
		}
		mode |= STGM_CREATE;
		create = FALSE;
		break;
	default:
		return false;
	}

	HRESULT hr = SHCreateStreamOnFileEx( opt.Path.c_str() , mode , 0 , create , nullptr , &MyStream );
	if( FAILED( hr ) )
	{
		SetLastError( hr );
		RaiseError( g_ErrorLogger , 0 , L"�t�@�C���̃I�[�v�������s���܂���" + opt.Path );
		return false;
	}
	if( opt.Mode == mFile::CreateMode::TruncateExisting )
	{
		ULARGE_INTEGER li;
		li.QuadPart = 0;
		MyStream->SetSize( li );
	}
	return true;
}
