//----------------------------------------------------------------------------
// �X�g���[�~���O�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MFILEREADSTREAM_CPP_COMPILING
#include "mFileReadStream.h"
#include "General/mErrorLogger.h"

mFileReadStream::mFileReadStream()
{
	MyReadCacheHead.reset( mNew BYTE[ MAX_BUFFER_SIZE ] );
}

mFileReadStream::~mFileReadStream()
{
	MyHandle.Close();
}

bool mFileReadStream::Open( const mFile::Option& opt )
{
	//�ǂݎ��A�N�Z�X���w�肵�Ă��邩�`�F�b�N
	if( !opt.AccessRead )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Insafficient access right : " + opt.Path );
		return false;
	}

	//�t�@�C�����I�[�v��
	//����d�ɊJ�����Ƃ���Ƃ��̃��\�b�h�͎��s����
	if( !MyHandle.Open( opt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"Cannot open : " + opt.Path );
		return false;
	}

	//�t�@�C�����J�����Ƃ��o�����ꍇ�́A�ǂݎ��L���b�V������
	InvalidateCache();
	return true;
}

//�t�@�C�����J���܂�
bool mFileReadStream::Open( const WString& filename )
{
	Option opt;
	opt.AccessRead = true;
	opt.AccessWrite = false;
	opt.Mode = CreateMode::OpenExisting;
	opt.Path = filename;
	opt.ShareRead = true;
	opt.ShareWrite = false;

	return Open( opt );
}

bool mFileReadStream::Close( void )
{
	//�N���[�Y��͎��̖h�~�̂���EOF�����ǂ߂Ȃ��悤�ɂ��Ă���
	MyIsEOF = true;
	return MyHandle.Close();
}

void mFileReadStream::InvalidateCache( void )
{
	//�L���b�V���𖳌��ɁB
	MyReadCacheRemain = 0;
	MyUnReadBuffer.Clear();
	return;
}

INT mFileReadStream::Read()
{
	//UnRead���ꂽ����������ꍇ�̓\����Ԃ�
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	//�Ō�ɓǂ񂾌��ʂ�EOF�Ȃ獡���EOF
	if( MyIsEOF )
	{
		return EOF;
	}

	//�L���b�V���̎c�ʂ��O�Ȃ�A����ǂ�
	if( MyReadCacheRemain == 0 )
	{
		if( !ReadNextBlock() )
		{
			MyIsEOF = true;
			return EOF;
		}
	}

	//�L���b�V���̎c�ʂ�����̂ŁA�L���b�V������P�����Ԃ�
	INT result = MyReadCacheHead[ MyReadCacheCurrent ];
	MyReadCacheCurrent++;
	MyReadCacheRemain--;

	return result;
}

bool mFileReadStream::ReadNextBlock( void )
{
	//�t�@�C���̓ǂݎ��
	if( !MyHandle.Read( MyReadCacheHead.get() , MAX_BUFFER_SIZE , MyReadCacheRemain ) || MyReadCacheRemain == 0 )
	{
		//�ǂ߂Ȃ�������EOF
		//���邢�͓ǂݎ�����T�C�Y���O�ł�EOF
		return false;
	}

	//�L���b�V���̂����܂œǂ񂾃|�C���^��擪�Ƀ��Z�b�g
	MyReadCacheCurrent = 0;
	return true;
}

bool mFileReadStream::SetPointer( ULONGLONG position )
{
	InvalidateCache();
	if( MyHandle.SetPointer( position ) )
	{
		MyIsEOF = false;
		return true;
	}
	return false;
}

bool mFileReadStream::IsOpen( void )const
{
	return MyHandle.IsOpen();
}

//EOF�ɒB���Ă��邩�𒲂ׂ܂�
bool mFileReadStream::IsEOF( void )const
{
	return MyIsEOF;
}

WString mFileReadStream::GetPath( bool fullpath )const
{
	return MyHandle.GetPath( fullpath );
}

//�t�@�C���̃T�C�Y���擾
bool mFileReadStream::GetFileSize( ULONGLONG& retSize )const
{
	return MyHandle.GetFileSize( retSize );
}
