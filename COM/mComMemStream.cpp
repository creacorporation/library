//----------------------------------------------------------------------------
// COM�ւ̃X�g���[�~���O�������ݑ���
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MCOMMEMSTREAM_CPP_COMPILING
#include "mComMemStream.h"
#include "General/mErrorLogger.h"
#include <shlwapi.h>

mComMemStream::mComMemStream( DWORD buffersize )
	: MAX_BUFFER_SIZE( buffersize )
{
	ResetWriteCache();
	MyStream = SHCreateMemStream( nullptr , 0 );
}

mComMemStream::~mComMemStream()
{
	MyStream->Release();
}

bool mComMemStream::Close( void )
{
	return false;
}

bool mComMemStream::Write( INT data )
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

bool mComMemStream::IsOpen( void )const
{
	if( !MyStream )
	{
		return false;
	}
	return true;
}

INT mComMemStream::Read( void )
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

bool mComMemStream::IsEOF( void )const
{
	return false;
}

bool mComMemStream::FlushCache( void )
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

void mComMemStream::ResetWriteCache( void )
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


IStream* mComMemStream::Get( void )const
{
	return MyStream;
}

mComMemStream::operator IStream*( void )const
{
	return Get();
}

bool mComMemStream::SetPointer( ULONGLONG pos )
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

bool mComMemStream::MovePointer( LONGLONG distance )
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

bool mComMemStream::SetPointerToEnd( void )
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

bool mComMemStream::SetPointerToBegin( void )
{
	return SetPointer( 0 );
}

