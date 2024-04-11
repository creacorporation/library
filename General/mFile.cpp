//----------------------------------------------------------------------------
// �t�@�C���Ǘ�
// Copyright (C) 2005,2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MFILE_CPP_COMPILING
#include "mfile.h"
#include "mErrorLogger.h"
#include "mFileUtility.h"

mFile::mFile()
{
	
	MyHandle = INVALID_HANDLE_VALUE;
	return;

}

mFile::~mFile()
{
	Close();
}

bool mFile::Close( void )
{
	bool result = true;
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		result = ( CloseHandle( MyHandle ) );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	MyPath.clear();

	return result;
}



bool mFile::Open( const mFile::Option& opt )
{
	//��d�ɊJ���Ȃ��悤�ɂ���
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		//��d�Ƀt�@�C�����I�[�v�����悤�Ƃ��Ă���
		RaiseError( g_ErrorLogger , 0 , L"Already opened : " + opt.Path );
		return false;
	}

	//�ǂݏ������[�h
	DWORD open_mode = 0;
	open_mode |= ( opt.AccessRead ) ? ( GENERIC_READ ) : ( 0 );
	open_mode |= ( opt.AccessWrite ) ? ( GENERIC_WRITE ) : ( 0 );

	//���L���[�h
	DWORD share_mode = 0;
	share_mode |= ( opt.ShareRead ) ? ( FILE_SHARE_READ ) : ( 0 );
	share_mode |= ( opt.ShareWrite ) ? ( FILE_SHARE_WRITE ) : ( 0 );

	//�쐬���[�h
	DWORD create_mode = 0;
	switch( opt.Mode )
	{
	case CreateMode::CreateNew:			//�V�����t�@�C�����쐬�B���łɂ���ꍇ�̓G���[
		create_mode = CREATE_NEW;
		break;
	case CreateMode::CreateAlways:		//�V�����t�@�C�����쐬�B���łɂ���ꍇ�͏㏑���i���g���̂Ă�j
		create_mode = CREATE_ALWAYS;
		break;
	case CreateMode::OpenExisting:		//���łɂ���t�@�C�����J���B�Ȃ��ꍇ�̓G���[
		create_mode = OPEN_EXISTING;
		break;
	case CreateMode::OpenAlways:		//���łɂ���t�@�C�����J���B�Ȃ��ꍇ�͐V�����t�@�C�����J��
		create_mode = OPEN_ALWAYS;
		break;
	case CreateMode::TruncateExisting:	//���łɂ���t�@�C�����J���Ē��g���̂Ă�B�Ȃ��ꍇ�̓G���[�B
		create_mode = TRUNCATE_EXISTING;
		break;
	case CreateMode::CreateWithDirectory:
		create_mode = CREATE_ALWAYS;
		if( !mFileUtility::CreateMiddleDirectory( opt.Path ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"���Ԃ̃f�B���N�g�����쐬�ł��܂���" + opt.Path );
			return false;
		}
		break;
	default:
		//�s���ȃ��[�h���w�肵�Ă���
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)opt.Mode , L"Invalid creation disposition" );
		break;
	}

	//���X�̐ݒ肪�ł�������A�t�@�C�����J���܂�
	MyHandle = CreateFileW( opt.Path.c_str() , open_mode , share_mode , nullptr , create_mode , FILE_ATTRIBUTE_NORMAL , nullptr );
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		//�c�O�B�t�@�C�����J���̂Ɏ��s���܂����B
		RaiseError( g_ErrorLogger , 0 , L"CreateFileW failed : " + opt.Path );
		return false;
	}
	MyPath = opt.Path;
	return true;

}

WString mFile::GetPath( bool fullpath )const
{
	//�t���p�X�s�v�Ȃ炻�̂܂ܕԂ��ďI���
	if( !fullpath )
	{
		return MyPath;
	}

	//�o�b�t�@�̊m��
	DWORD reqsize = GetFullPathNameW( MyPath.c_str() , 0 , nullptr , nullptr );
	if( reqsize == 0 )
	{
		return MyPath;
	}
	std::unique_ptr< wchar_t > buffer( mNew wchar_t[ reqsize ] );

	//�擾
	DWORD usedsize = GetFullPathNameW( MyPath.c_str() , reqsize , buffer.get() , nullptr );
	if( reqsize != usedsize )
	{
		return MyPath;
	}

	return WString( buffer.get() );
}

bool mFile::Read(
	void*	Buffer ,							//�ǂݎ�����f�[�^���i�[����o�b�t�@
	ULONGLONG	ReadSize ,						//�ǂݎ��o�C�g��
	ULONGLONG& retReadSize )					//�ǂݎ�����o�C�g��
{
	retReadSize = 0;
	BYTE* tmp_buffer = (BYTE*)Buffer;

	while( ReadSize )
	{
		//�܂������Ă���Ԃɂ͖����Ƃ͎v���񂾂��ǁA�����I��32bit�𒴂���T�C�Y��
		//��C�ɓǂݍ������Ƃ����ꍇ�ɔ����āA32bit�P�ʂɋ�؂��ăA�N�Z�X����B
		DWORD tmp_read_size = ( MAXDWORD < ReadSize ) ? ( MAXDWORD ) : ( (DWORD)ReadSize );
		DWORD tmp_stored_size = 0;
		
		bool result = Read( Buffer , tmp_read_size , tmp_stored_size );
		retReadSize += tmp_stored_size;
		if( !result || tmp_read_size != tmp_stored_size )
		{
			return result;
		}
		ReadSize -= tmp_stored_size;
		tmp_buffer += tmp_stored_size;
	}
	return true;
}

bool mFile::Read(
	void*	Buffer ,							//�ǂݎ�����f�[�^���i�[����o�b�t�@
	DWORD	ReadSize ,							//�ǂݎ��o�C�g��
	DWORD&	retReadSize )						//�ǂݎ�����o�C�g��
{
	//�s�����͂̃`�F�b�N
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"" );
		return false;
	}

	//�ǂݎ�菈��
	if( !ReadFile( MyHandle , Buffer , ReadSize , &retReadSize , nullptr ) )
	{
		//�t�@�C���̓ǂݎ�肪�ł��Ȃ�����
		RaiseError( g_ErrorLogger , ReadSize , L"ReadFile failed" );
		return false;
	}
	return true;
}


bool mFile::Write(
	void*	Buffer ,							//�������ރf�[�^���i�[���Ă���o�b�t�@
	ULONGLONG	WriteSize ,						//�������ރo�C�g��
	ULONGLONG& retWriteSize )					//�������񂾃o�C�g��
{
	retWriteSize = 0;
	BYTE* tmp_buffer = (BYTE*)Buffer;

	while( WriteSize )
	{
		//32bit���𒴂���T�C�Y����C�ɏ����������Ƃ��Ă���ꍇ�A
		//WriteFile��DWORD�ɂȂ��Ă�̂ŁA32bit�P�ʂŋ�؂��ďo�͂��s���B
		//�ł����̑O�Ƀ�����������Ȃ��Ȃ��ďI�����Ǝv���B
		DWORD tmp_write_size = ( MAXDWORD < WriteSize ) ? ( MAXDWORD ) : ( (DWORD)WriteSize );
		DWORD tmp_stored_size = 0;

		bool result = Write( Buffer , tmp_write_size , tmp_stored_size );
		retWriteSize += tmp_stored_size;
		if( !result || tmp_write_size != tmp_stored_size )
		{
			return result;
		}
		WriteSize -= tmp_stored_size;
		tmp_buffer += tmp_stored_size;
	}
	return true;

}

bool mFile::Write(
	void*	Buffer ,							//�������ރf�[�^���i�[���Ă���o�b�t�@
	DWORD	WriteSize ,							//�������ރo�C�g��
	DWORD&  retWriteSize )						//�������񂾃o�C�g��
{
	//�s�����͂̃`�F�b�N
	if( MyHandle == INVALID_HANDLE_VALUE || Buffer == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"" );
		return false;
	}
	//�������ݏ���
	if( !WriteFile( MyHandle , Buffer , WriteSize , &retWriteSize , nullptr ) )
	{
		RaiseError( g_ErrorLogger , WriteSize , L"WriteFile failed" );
		return false;
	}
	return true;

}

bool mFile::SetPointer( ULONGLONG newpos )
{
	//�s�����͂̃`�F�b�N
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	//�t�@�C���|�C���^�̈ړ�(SetPointer�ł̓t�@�C���̐擪����)
	LARGE_INTEGER pos;
	pos.QuadPart = (LONGLONG)( newpos & 0x7FFF'FFFF'FFFF'FFFFULL );	//API�̈����ɂ͕���������̂ŏ���
	if( !SetFilePointerEx( MyHandle , pos , nullptr , FILE_BEGIN ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
		return false;
	}
	if( newpos & 0x8000'0000'0000'0000ULL )
	{
		//�w��ʒu���t�@�C���̐擪����8�G�N�T�o�C�g�ȏ�̏ꍇ�i�����͂��蓾�Ȃ��j�B
		bool result = true;
		pos.QuadPart = (LONGLONG)( 0x7FFF'FFFF'FFFF'FFFFULL );
		result &= SetFilePointerEx( MyHandle , pos , nullptr , FILE_CURRENT ) != FALSE;
		pos.QuadPart = (LONGLONG)( 0x0000'0000'0000'0001ULL );
		result &= SetFilePointerEx( MyHandle , pos , nullptr , FILE_CURRENT ) != FALSE;
		if( !result )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
			return false;
		}
	}
	return true;
}

bool mFile::MovePointer( LONGLONG distance )
{
	//�s�����͂̃`�F�b�N
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	LARGE_INTEGER pos;
	pos.QuadPart = distance;
	if( !SetFilePointerEx( MyHandle , pos , nullptr , FILE_CURRENT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
		return false;
	}
	return true;

}

bool mFile::SetPointerToEnd( void )
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	if( !SetFilePointer( MyHandle , 0 , nullptr , FILE_END ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointer (FILE_END) failed" );
		return false;
	}
	return true;
}

bool mFile::SetPointerToBegin( void )
{

	return SetPointer( 0 );

}

bool mFile::FlushBuffer( void )
{

	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	if( !FlushFileBuffers( MyHandle ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"FlushFileBuffers failed" );
		return false;
	}
	return true;

}

bool mFile::GetFileSize( ULONGLONG& retSize )const
{
	retSize = 0;
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	//�t�@�C���T�C�Y�̎擾
	LARGE_INTEGER result;
	if( !GetFileSizeEx( MyHandle , &result ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"GetFileSizeEx failed" );
		return false;
	}
	retSize = (ULONGLONG)( result.QuadPart );
	return true;
}

bool mFile::GetFileSize( DWORD* high , DWORD& low )const
{
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	//�t�@�C���T�C�Y�̎擾
	LARGE_INTEGER result;
	if( !GetFileSizeEx( MyHandle , &result ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"GetFileSizeEx failed" );
		return false;
	}
	if( high )
	{
		*high = result.HighPart;
	}
	else if( result.HighPart )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�t�@�C���T�C�Y��4GB�𒴂��Ă��܂�" );
		return false;
	}
	low = result.LowPart;
	return true;
}

ULONGLONG mFile::GetFileSize( void )const
{
	ULONGLONG tmp;
	if( !GetFileSize( tmp ) )
	{
		return 0;
	}
	return tmp;
}

bool mFile::GetPosition( ULONGLONG& retPos )const
{
	retPos = 0;
	if( MyHandle == INVALID_HANDLE_VALUE )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid handle" );
		return false;
	}

	LARGE_INTEGER dummy_pos;
	dummy_pos.QuadPart = 0;
	LARGE_INTEGER result;
	if( !SetFilePointerEx( MyHandle , dummy_pos , &result , FILE_CURRENT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"SetFilePointerEx failed" );
		return false;
	}

	retPos = (ULONGLONG)( result.QuadPart );
	return true;

}

bool mFile::IsOpen( void )const
{
	return MyHandle != INVALID_HANDLE_VALUE;
}

bool mFile::SetEof( void )
{
	return SetEndOfFile( MyHandle );
}

bool mFile::ExecIoControl( DWORD code , const mBinary* in , mBinary* retResult )
{
	mBinary in_dummy;
	mBinary out_dummy;
	mBinary* in_ptr;
	mBinary* out_ptr;

	in_ptr = ( in ) ? ( const_cast< mBinary* >( in ) ) : ( &in_dummy );
	out_ptr = ( retResult ) ? ( retResult ) : ( &out_dummy );

	//�K�v�T�C�Y�̒���
	DWORD response_size = 0;
	while( !DeviceIoControl( MyHandle , code , in_ptr->data() , in_ptr->size() , out_ptr->data() , out_ptr->size() , &response_size , nullptr ) )
	{
		DWORD errcode = GetLastError();
		if( ( errcode == ERROR_INSUFFICIENT_BUFFER ) ||
			( errcode == ERROR_MORE_DATA ) )
		{
			if( !retResult )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"���ʂ��Ԃ���Ă��邪�󂯎��悤�ɂȂ��Ă��Ȃ�" );
				return false;
			}
			out_ptr->resize( out_ptr->capacity() * 2 + 32 );
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"DeviceIoControl�����s" );
			return false;
		}
	}
	out_ptr->resize( response_size );
	return true;
}




