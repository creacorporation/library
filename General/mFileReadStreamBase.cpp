//----------------------------------------------------------------------------
// �X�g���[�~���O�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MFILEREADSTREAMBASE_CPP_COMPILING
#include "mFileReadStreamBase.h"
#include "General/mErrorLogger.h"



mFileReadStreamBase::mFileReadStreamBase()
{
	MyIsEOF = false;
	MyReadCacheCurrent = 0;
	MyReadCacheRemain = 0;
	MyEncode = Encode::ENCODE_ASIS;
}

mFileReadStreamBase::~mFileReadStreamBase()
{
	MyReadCacheHead.reset();
}


//�P�s�ǂݎ��܂��B
//���s�����́A\r�܂���\n�܂���\r\n�ł��B
//EOF�ɒB�����Ƃ��Atrue
//EOF�ɒB���Ȃ��Ƃ��Afalse
bool mFileReadStreamBase::ReadLine( AString& retResult , OnLineReadError onerr )
{
	//���łɂ��錋�ʂ��N���A
	retResult.clear();

	//�G���R�[�h�����Ⴕ�Ă���ꍇ�͉I�񂷂�
	if( MyEncode == Encode::ENCODE_UTF16 )
	{
		WString tmp;
		if( !ReadLine( tmp ) )
		{
			return false;
		}
		retResult = WString2AString( tmp );
		return true;
	}

	//EOF�H
	if( IsEOF() )
	{
		return false;
	}

	//�P�������ǂݎ��A�P�s�ɒB����܂Œǉ�
	while( 1 )
	{
		INT readdata = Read();

		if( readdata == EOF )
		{
			//�r����EOF�������ꍇ
			switch( onerr )
			{
			case OnLineReadError::LINEREADERR_DISCARD:
				//���ʂ��N���A����
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_UNREAD:
				//�ǂ񂾂Ƃ���܂ł��o�b�t�@�ɉ����߂�
				for( AString::const_reverse_iterator itr = retResult.rbegin() ; itr != retResult.rend() ; itr++ )
				{
					MyUnReadBuffer.Unread< char >( *itr );
				}
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_TRUNCATE:
				//�ǂ߂��Ƃ���܂łŐ���
				if( retResult == "" )
				{
					return false;	//�����ǂ߂Ă��Ȃ��̂�false
				}
				return true;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"OnLineReadError�̎w�肪�s���ł�" , onerr );
				return false;
			}
		}

		switch( readdata )
		{
		case '\r':
			//\r�̏ꍇ�A���̕�����\n��������CRLF���s
			//�����łȂ����CR���s
			//����ɂ��̎���EOF���ǂ����𔻒肵�ĕԂ�
			readdata = Read();
			if( readdata == '\n' || readdata == EOF )
			{
				//CRLF���s or CR���s+EOF
				;
			}
			else
			{
				//CR���s
				MyUnReadBuffer.Unread<CHAR>( readdata );
			}
			return true;

		case '\n':
			//LF���s
			return true;

		default:
			//���̑��̕����͘A��
			retResult.push_back( readdata );
			break;
		}
	}
}

//ANSI�łP�����ǂݍ��݂܂�
bool mFileReadStreamBase::ReadSingle( char& retChar )
{
	INT c = Read();
	retChar = ( (DWORD)c & 0xFF );
	return c != EOF;
}

//UNICODE(LE)�łP�����ǂݍ��݂܂�
bool mFileReadStreamBase::ReadSingle( wchar_t& retWchar )
{
	INT lo = Read();
	INT hi = Read();
	if( lo == EOF && hi == EOF )
	{
		return false;
	}
	else if( lo != EOF && hi == EOF )
	{
		MyUnReadBuffer.Unread( (char)lo );
		return false;
	}
	retWchar = (wchar_t)( ( hi << 8 ) + lo );
	return true;
}

bool mFileReadStreamBase::ReadLine( WString& retResult , OnLineReadError onerr )
{
	//���͒l�N���A
	retResult.clear();

	//�G���R�[�h�����Ⴕ�Ă���ꍇ�͉I�񂷂�
	if( MyEncode == Encode::ENCODE_SHIFT_JIS )
	{
		AString tmp;
		if( !ReadLine( tmp ) )
		{
			return false;
		}
		retResult = AString2WString( tmp );
		return true;
	}

	//EOF�H
	if( IsEOF() )
	{
		return false;
	}

	//�ǂݎ�胋�[�`����`
	auto WcharRead = [this,onerr]( wchar_t& retRead ) -> bool
	{
		INT lo = Read();
		INT hi = Read();
		if( lo == EOF && hi == EOF )
		{
			return true;
		}
		else if( lo != EOF && hi == EOF )
		{
			if( onerr == OnLineReadError::LINEREADERR_UNREAD )
			{
				MyUnReadBuffer.Unread( (char)lo );
			}
			return true;
		}
		retRead = (wchar_t)( ( hi << 8 ) + lo );
		return false;
	};

	//�P�������ǂݎ��A�P�s�ɒB����܂Œǉ�
	while( 1 )
	{
		WCHAR readdata;
		if( WcharRead( readdata ) )
		{
			//�r����EOF�������ꍇ
			switch( onerr )
			{
			case OnLineReadError::LINEREADERR_DISCARD:
				//���ʂ��N���A����
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_UNREAD:
				//�ǂ񂾂Ƃ���܂ł��o�b�t�@�ɉ����߂�
				for( WString::const_reverse_iterator itr = retResult.rbegin() ; itr != retResult.rend() ; itr++ )
				{
					MyUnReadBuffer.Unread< wchar_t >( *itr );
				}
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_TRUNCATE:
				//�ǂ߂��Ƃ���܂łŐ���
				return true;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"OnLineReadError�̎w�肪�s���ł�" , onerr );
				return false;

			}
		}

		switch( readdata )
		{
		case L'\r':
			//\r�̏ꍇ�A���̕�����\n��������CRLF���s
			//�����łȂ����CR���s
			//����ɂ��̎���EOF���ǂ����𔻒肵�ĕԂ�
			if( !WcharRead( readdata ) )
			{
				if( readdata == L'\n' || readdata == EOF )
				{
					//CRLF���s or CR���s+EOF
					;
				}
				else
				{
					//CR���s
					MyUnReadBuffer.Unread<WCHAR>( readdata );
				}
			}
			return true;

		case L'\n':
			//LF���s
			return true;

		default:
			//���̑��̕����͘A��
			retResult.push_back( (WCHAR)readdata );
			break;
		}
	}
}

bool mFileReadStreamBase::ReadBinary( BYTE* retResult , size_t ReadSize , size_t* retReadSize , OnLineReadError onerr )
{
	size_t i;
	for( i = 0 ; i < ReadSize ; i++ )
	{
		INT readdata = Read();

		if( readdata == EOF )
		{
			//�r����EOF�������ꍇ
			if( retReadSize )
			{
				*retReadSize = i;
			}

			switch( onerr )
			{
			case OnLineReadError::LINEREADERR_DISCARD:
				return false;
			case OnLineReadError::LINEREADERR_UNREAD:
				//�ǂ񂾂Ƃ���܂ł��o�b�t�@�ɉ����߂�
				for( size_t j = 0 ; j < i ; j++ )
				{
					MyUnReadBuffer.Unread< BYTE >( *retResult );
					retResult--;
				}
				return false;
			case OnLineReadError::LINEREADERR_TRUNCATE:
				//�ǂ߂��Ƃ���܂łŐ���
				return true;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"OnLineReadError�̎w�肪�s���ł�" , onerr );
				return false;
			}
		}
		else
		{
			*retResult = (BYTE)readdata;
			retResult++;
		}
	}

	if( retReadSize )
	{
		*retReadSize = i;
	}
	return true;
}


bool mFileReadStreamBase::SetEncode( Encode encode )
{
	switch( encode )
	{
	case Encode::ENCODE_ASIS:
	case Encode::ENCODE_SHIFT_JIS:
	case Encode::ENCODE_UTF16:
		MyEncode = encode;
		return true;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�G���R�[�h�̎w�肪�s���ł�" );
		break;
	}
	return false;
}

//----------------------------------------------------
// ��������q�N���X
//----------------------------------------------------

//�o�b�t�@����1������肾��
//�o�b�t�@����Ȃ̂Ɏ��o�����Ƃ����EOF�ɂȂ�(���ӁI)
INT mFileReadStreamBase::UnReadBuffer::Read( void )
{
	if( IsEmpty() )
	{
		return EOF;
	}
	INT result = MyBuffer.front();
	MyBuffer.pop_front();
	return result;
}

//�o�b�t�@�͋󂩁H
bool mFileReadStreamBase::UnReadBuffer::IsEmpty( void )const
{
	return MyBuffer.empty();
}

//�o�b�t�@���N���A
void mFileReadStreamBase::UnReadBuffer::Clear( void )
{
	MyBuffer.clear();
}
