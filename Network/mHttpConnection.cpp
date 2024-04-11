//----------------------------------------------------------------------------
// HTTPアクセス
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019-2020 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mHttpConnection.h"
#include "General/mErrorLogger.h"
#include <vector>

mHttpConnection::mHttpConnection()
{
	MyConnection = 0;
	MyPort = 0;
	MyServerAddress = L"";
}

mHttpConnection::~mHttpConnection()
{
	if( MyConnection )
	{
		WinHttpCloseHandle( MyConnection );
		MyConnection = 0;
	}
}

class MediaTypeArray final
{
public:
	MediaTypeArray( const mHttpRequest::MediaTypeList& ls )
	{
		if( ls.empty() )
		{
			return;
		}

		size_t size = ls.size();
		MyPtr.resize( size + 1 );
		MyPtr[ size ] = 0;

		mHttpRequest::MediaTypeList::const_iterator itr = ls.begin();
		DWORD index = 0;
		while( itr != ls.end() )
		{
			MyPtr[ index ] = mNew WCHAR[ itr->size() + 1 ];
			wchar_strcpy( const_cast<WCHAR*>( MyPtr[ index ] ) , itr->c_str() );
		}
	}
	~MediaTypeArray()
	{
		for( std::vector<WCHAR*>::iterator itr = MyPtr.begin() ; itr != MyPtr.end() ; itr++ )
		{
			mDelete[] *itr;
			*itr = nullptr;
		}
	}
	operator const WCHAR**()
	{
		if( MyPtr.empty() )
		{
			return WINHTTP_DEFAULT_ACCEPT_TYPES;
		}
		return (const WCHAR**)( MyPtr.data() );
	};
private:
	std::vector<WCHAR*> MyPtr;
};

bool mHttpConnection::NewRequest( const RequestOption& opt , const NotifyOption& notifier , mHttpRequest& retReq )
{
	//セッションをまだ開いてない場合は失敗
	if( !*this )
	{
		RaiseError( g_ErrorLogger , 0 , L"コネクションハンドルが開いていません" );
		return false;
	}

	//すでに開いている場合は失敗
	if( retReq )
	{
		RaiseError( g_ErrorLogger , 0 , L"すでにハンドルが開かれています" );
		return false;
	}

	//設定情報のチェック
	if( ( opt.RecievePacketMaxActive < 1 ) ||
		( opt.RecievePacketMaxStock < 1 ) ||
		( opt.RecievePacketSize < 1 ) ||
		( opt.SendPacketSize < 1 ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"バッファサイズの指定が不正です" );
		return false;
	}

	//設定情報のコピー
	retReq.MyOption = opt;
	retReq.MyNotifyOption = notifier;

	//リクエストを開く
	//動詞
	const WCHAR* verb;
	switch( opt.Verb )
	{
	case mHttpRequest::RequestVerb::VERB_GET:
		verb = L"GET";
		break;
	case mHttpRequest::RequestVerb::VERB_PUT:
		verb = L"PUT";
		break;
	case mHttpRequest::RequestVerb::VERB_POST:
		verb = L"POST";
		break;
	case mHttpRequest::RequestVerb::VERB_HEADER:
		verb = L"HEADER";
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"動詞が不正です" );
		return false;
	}

	//バージョン
	const WCHAR* version = nullptr;	//デフォルトにする(1.1 or 2.0)

	//URLパス
	URL_COMPONENTS url;
	const WCHAR* urlpath;
	ZeroMemory( &url , sizeof( URL_COMPONENTS ) );
	url.dwStructSize = sizeof( URL_COMPONENTS );
	url.dwUrlPathLength = 1;
	if( WinHttpCrackUrl( opt.Url.c_str() , (DWORD)opt.Url.size() , 0 , &url ) )
	{
		urlpath = url.lpszUrlPath;
	}
	else
	{
		urlpath = opt.Url.c_str();
	}
	
	//メディアタイプ
	MediaTypeArray mediatype( opt.AcceptType );

	//リファラ
	const WCHAR* ref;
	if( opt.Referer.empty() )
	{
		ref = WINHTTP_NO_REFERER;
	}
	else
	{
		ref = opt.Referer.c_str();
	}

	//フラグ関係
	DWORD flags = 0;
	if( opt.Reload )
	{
		flags |= WINHTTP_FLAG_REFRESH;
	}
	if( opt.Secure )
	{
		flags |= WINHTTP_FLAG_SECURE;
	}

	//リクエスト生成
	retReq.MyRequest = WinHttpOpenRequest( MyConnection , verb , urlpath , version , ref , mediatype , flags );
	if( retReq.MyRequest == 0 )
	{
		return false;
	}
	
	//コールバック関数をインストールする
	DWORD notifyflag = WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS;
	// = ( WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE  \
	//	 | WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE     \
	//	 | WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE        \
	//	 | WINHTTP_CALLBACK_STATUS_READ_COMPLETE         \
	//	 | WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE        \
	//	 | WINHTTP_CALLBACK_STATUS_REQUEST_ERROR         \
	//	 | WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE )
	notifyflag |= WINHTTP_CALLBACK_FLAG_SECURE_FAILURE;			//SSLが失敗したとき
	notifyflag |= WINHTTP_CALLBACK_FLAG_REDIRECT;				//リダイレクトされたとき
	//notifyflag |= WINHTTP_CALLBACK_FLAG_CLOSE_CONNECTION;		//接続が閉じられたとき

	if( WinHttpSetStatusCallback( retReq.MyRequest , mHttpRequest::WinhttpStatusCallback , notifyflag , 0 ) == WINHTTP_INVALID_STATUS_CALLBACK )
	{
		RaiseError( g_ErrorLogger , 0 , L"コールバック関数をインストールできませんでした" );
		return false;
	}

	//メンバ変数をセット
	retReq.MyRequestStatus = mHttpRequest::RequestStatus::REQUEST_PREEXEC;

	return true;
}

mHttpConnection::operator bool() const
{
	return MyConnection;
}


