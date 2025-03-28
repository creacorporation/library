//----------------------------------------------------------------------------
// ストリーミングファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
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


//１行読み取ります。
//改行文字は、\rまたは\nまたは\r\nです。
//EOFに達したとき、true
//EOFに達しないとき、false
bool mFileReadStreamBase::ReadLine( AString& retResult , OnLineReadError onerr )
{
	//すでにある結果をクリア
	retResult.clear();

	//エンコードが相違している場合は迂回する
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

	//EOF？
	if( IsEOF() )
	{
		return false;
	}

	//１文字ずつ読み取り、１行に達するまで追加
	while( 1 )
	{
		INT readdata = Read();

		if( readdata == EOF )
		{
			//途中でEOFが来た場合
			switch( onerr )
			{
			case OnLineReadError::LINEREADERR_DISCARD:
				//結果をクリアする
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_UNREAD:
				//読んだところまでをバッファに押し戻す
				for( AString::const_reverse_iterator itr = retResult.rbegin() ; itr != retResult.rend() ; itr++ )
				{
					MyUnReadBuffer.Unread< char >( *itr );
				}
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_TRUNCATE:
				//読めたところまでで成功
				if( retResult == "" )
				{
					return false;	//何も読めていないのでfalse
				}
				return true;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"OnLineReadErrorの指定が不正です" , onerr );
				return false;
			}
		}

		switch( readdata )
		{
		case '\r':
			//\rの場合、次の文字が\nだったらCRLF改行
			//そうでなければCR改行
			//さらにその次がEOFかどうかを判定して返す
			readdata = Read();
			if( readdata == '\n' || readdata == EOF )
			{
				//CRLF改行 or CR改行+EOF
				;
			}
			else
			{
				//CR改行
				MyUnReadBuffer.Unread<CHAR>( readdata );
			}
			return true;

		case '\n':
			//LF改行
			return true;

		default:
			//その他の文字は連結
			retResult.push_back( readdata );
			break;
		}
	}
}

//ANSIで１文字読み込みます
bool mFileReadStreamBase::ReadSingle( char& retChar )
{
	INT c = Read();
	retChar = ( (DWORD)c & 0xFF );
	return c != EOF;
}

//UNICODE(LE)で１文字読み込みます
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
	//入力値クリア
	retResult.clear();

	//エンコードが相違している場合は迂回する
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

	//EOF？
	if( IsEOF() )
	{
		return false;
	}

	//読み取りルーチン定義
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

	//１文字ずつ読み取り、１行に達するまで追加
	while( 1 )
	{
		WCHAR readdata;
		if( WcharRead( readdata ) )
		{
			//途中でEOFが来た場合
			switch( onerr )
			{
			case OnLineReadError::LINEREADERR_DISCARD:
				//結果をクリアする
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_UNREAD:
				//読んだところまでをバッファに押し戻す
				for( WString::const_reverse_iterator itr = retResult.rbegin() ; itr != retResult.rend() ; itr++ )
				{
					MyUnReadBuffer.Unread< wchar_t >( *itr );
				}
				retResult.clear();
				return false;
			case OnLineReadError::LINEREADERR_TRUNCATE:
				//読めたところまでで成功
				return true;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"OnLineReadErrorの指定が不正です" , onerr );
				return false;

			}
		}

		switch( readdata )
		{
		case L'\r':
			//\rの場合、次の文字が\nだったらCRLF改行
			//そうでなければCR改行
			//さらにその次がEOFかどうかを判定して返す
			if( !WcharRead( readdata ) )
			{
				if( readdata == L'\n' || readdata == EOF )
				{
					//CRLF改行 or CR改行+EOF
					;
				}
				else
				{
					//CR改行
					MyUnReadBuffer.Unread<WCHAR>( readdata );
				}
			}
			return true;

		case L'\n':
			//LF改行
			return true;

		default:
			//その他の文字は連結
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
			//途中でEOFが来た場合
			if( retReadSize )
			{
				*retReadSize = i;
			}

			switch( onerr )
			{
			case OnLineReadError::LINEREADERR_DISCARD:
				return false;
			case OnLineReadError::LINEREADERR_UNREAD:
				//読んだところまでをバッファに押し戻す
				for( size_t j = 0 ; j < i ; j++ )
				{
					MyUnReadBuffer.Unread< BYTE >( *retResult );
					retResult--;
				}
				return false;
			case OnLineReadError::LINEREADERR_TRUNCATE:
				//読めたところまでで成功
				return true;
			default:
				RaiseAssert( g_ErrorLogger , 0 , L"OnLineReadErrorの指定が不正です" , onerr );
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
		RaiseAssert( g_ErrorLogger , 0 , L"エンコードの指定が不正です" );
		break;
	}
	return false;
}

template<class T = AStringVector, class C = CHAR>
static bool ParseLineBase( mFileReadStreamBase& base , C delimiter , T& retParsed , bool noempty , mFileReadStreamBase::OnLineReadError onerr )
{
	T::value_type line;
	if( !base.ReadLine( line , onerr ) )
	{
		return false;
	}
	ParseString( line , delimiter , retParsed , noempty );
	return true;
}

//1行読み取ってその文字列を特定の文字でパースする
bool mFileReadStreamBase::ParseLine( CHAR delimiter , AStringVector& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineBase( *this , delimiter , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を特定の文字でパースする
bool mFileReadStreamBase::ParseLine( CHAR delimiter , AStringDeque& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineBase( *this , delimiter , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を特定の文字でパースする
bool mFileReadStreamBase::ParseLine( WCHAR delimiter , WStringVector& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineBase( *this , delimiter , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を特定の文字でパースする
bool mFileReadStreamBase::ParseLine( WCHAR delimiter , WStringDeque& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineBase( *this , delimiter , retParsed , noempty , onerr );
}

template<class T = AStringVector>
static bool ParseLineSpaceBase( mFileReadStreamBase& base , T& retParsed , bool noempty , mFileReadStreamBase::OnLineReadError onerr )
{
	T::value_type line;
	if( !base.ReadLine( line , onerr ) )
	{
		return false;
	}
	ParseStringSpace( line , retParsed , noempty );
	return true;
}

//1行読み取ってその文字列を空白文字でパースする
bool mFileReadStreamBase::ParseLineSpace( AStringVector& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を空白文字でパースする
bool mFileReadStreamBase::ParseLineSpace( AStringDeque& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を空白文字でパースする
bool mFileReadStreamBase::ParseLineSpace( WStringVector& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を空白文字でパースする
bool mFileReadStreamBase::ParseLineSpace( WStringDeque& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , retParsed , noempty , onerr );
}

template<class T = AStringVector, class C = CHAR>
static bool ParseLineSpaceBase( mFileReadStreamBase& base , C delimiter , T& retParsed , bool noempty , mFileReadStreamBase::OnLineReadError onerr )
{
	T::value_type line;
	if( !base.ReadLine( line , onerr ) )
	{
		return false;
	}
	ParseStringSpace( line , retParsed , noempty );
	return true;
}

//1行読み取ってその文字列を空白文字または特定の文字でパースする
bool mFileReadStreamBase::ParseLineSpace( CHAR delimiter , AStringVector& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , delimiter , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を空白文字または特定の文字でパースする
bool mFileReadStreamBase::ParseLineSpace( CHAR delimiter , AStringDeque& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , delimiter , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を空白文字または特定の文字でパースする
bool mFileReadStreamBase::ParseLineSpace( WCHAR delimiter , WStringVector& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , delimiter , retParsed , noempty , onerr );
}

//1行読み取ってその文字列を空白文字または特定の文字でパースする
bool mFileReadStreamBase::ParseLineSpace( WCHAR delimiter , WStringDeque& retParsed , bool noempty , OnLineReadError onerr )
{
	return ParseLineSpaceBase( *this , delimiter , retParsed , noempty , onerr );
}

//----------------------------------------------------
// ここから子クラス
//----------------------------------------------------

//バッファから1文字取りだし
//バッファが空なのに取り出そうとするとEOFになる(注意！)
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

//バッファは空か？
bool mFileReadStreamBase::UnReadBuffer::IsEmpty( void )const
{
	return MyBuffer.empty();
}

//バッファをクリア
void mFileReadStreamBase::UnReadBuffer::Clear( void )
{
	MyBuffer.clear();
}
