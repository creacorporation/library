#include "mStdInStream.h"


mStdInStream::mStdInStream()
{
	MyReadCacheHead.reset();
}

mStdInStream::~mStdInStream()
{
}

//１文字（１バイト）読み込みます
//ret : 読み取った文字
//※EOFの場合、現在読み取れるデータがないことを示します
//（時間が経てば再度読み取れるかもしれない）
//※ストリームが完全に終了しているかを知るにはIsEOFを使います
INT mStdInStream::Read( void )
{
	//UnReadされた文字がある場合はソレを返す
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	INT result = fgetc( stdin );
	if( result == EOF )
	{
		MyIsEOF = true;
	}
	return result;
}

//EOFに達しているかを調べます
bool mStdInStream::IsEOF( void )const
{
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return false;
	}
	return MyIsEOF;
}

//ファイルが開いているかを判定します
//開いている場合は真が返ります
bool mStdInStream::IsOpen( void )const
{
	return true;
}

