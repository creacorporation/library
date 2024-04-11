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
#include "mHttpAccess.h"
#include "General/mErrorLogger.h"

mHttpAccess::mHttpAccess()
{
	MySession = 0;
}

mHttpAccess::~mHttpAccess()
{
	if( MySession )
	{
		WinHttpCloseHandle( MySession );
		MySession = 0;
	}
}

bool mHttpAccess::Setup( const AccessOption& opt )
{
	const WCHAR* proxy_server;	
	const WCHAR* proxy_bypass;
	DWORD proxy_type;
	DWORD flag;

	//ユーザエージェントを空にするのはダメ
	if( opt.UserAgent.empty() )
	{
		RaiseError( g_ErrorLogger , 0 , L"ユーザーエージェントを空白にすることはできません" );
		return false;
	}

	//同期モード
	flag = WINHTTP_FLAG_ASYNC;

	//プロキシの使用・不使用
	if( opt.OptionType == OptionType::NOPROXY )
	{
		//プロキシを使用しない場合。
		MyOption.reset( mNew AccessOption_NoProxy( *(AccessOption_NoProxy*)&opt ) );
		const AccessOption_NoProxy* p = (const AccessOption_NoProxy*)MyOption.get();

		proxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
		proxy_server = WINHTTP_NO_PROXY_NAME;
		proxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	}
	else if( opt.OptionType == OptionType::WITHPROXY )
	{
		//プロキシを使用する場合。
		MyOption.reset( mNew AccessOption_WithProxy( *(AccessOption_WithProxy*)&opt ) );
		const AccessOption_WithProxy* p = (const AccessOption_WithProxy*)MyOption.get();

		if( p->ProxyServer == L"" )
		{
			proxy_type = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
			proxy_server = nullptr;
		}
		else
		{
			proxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxy_server = p->ProxyServer.c_str();
		}

		if( p->NoProxyList.size() )
		{
			//プロキシ除外サーバが指定されている場合
			proxy_bypass = p->NoProxyList.c_str();
		}
		else
		{
			//全てプロキシを使用する場合
			proxy_bypass = WINHTTP_NO_PROXY_BYPASS;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"オプション識別子が不正です" );
		return false;
	}

	MySession = WinHttpOpen( MyOption->UserAgent.c_str() , proxy_type , proxy_server , proxy_bypass , flag );
	if( !MySession )
	{
		RaiseError( g_ErrorLogger , 0 , L"セッションハンドルの作成が失敗しました" );
		return false;
	}

	//HTTP/2を使用する場合フラグをセットする
	if( opt.ProtocolVersion == OptionProtocolVersion::HTTP_VERSION_20 )
	{
		DWORD protocol_option = WINHTTP_PROTOCOL_FLAG_HTTP2;
		if( !WinHttpSetOption( MySession , WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL , &protocol_option , sizeof( protocol_option ) ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"HTTPプロトコルバージョンを変更できませんでした" );
			return false;
		}
	}

	return true;
}

mHttpAccess::operator bool() const
{
	return MySession;
}

bool mHttpAccess::NewConnection( const ConnectionOption& info , mHttpConnection& retConn )
{
	//セッションをまだ開いてない場合は失敗
	if( !*this )
	{
		RaiseError( g_ErrorLogger , 0 , L"セッションハンドルが開いていません" );
		return false;
	}

	//すでに開いている場合は失敗
	if( retConn )
	{
		RaiseError( g_ErrorLogger , 0 , L"すでにハンドルが開かれています" );
		return false;
	}

	//URLを分解して、その中からサーバ名の部分を抽出する
	URL_COMPONENTS url;
	ZeroMemory( &url , sizeof( URL_COMPONENTS ) );
	url.dwStructSize = sizeof( URL_COMPONENTS );
	url.dwHostNameLength = (DWORD)-1;
	if( WinHttpCrackUrl( info.ServerName.c_str() , (DWORD)info.ServerName.size() , 0 , &url ) )
	{
		retConn.MyServerAddress = WString( url.lpszHostName ).substr( 0 , url.dwHostNameLength );
	}
	else
	{
		retConn.MyServerAddress = info.ServerName;
	}

	//ポートの設定
	if( info.Port == 0 )
	{
		retConn.MyPort = INTERNET_DEFAULT_PORT;
	}
	else
	{
		retConn.MyPort = info.Port;
	}

	//コネクションを開く
	retConn.MyConnection = WinHttpConnect( MySession , retConn.MyServerAddress.c_str() , retConn.MyPort , 0 );
	if( retConn.MyConnection == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"コネクションハンドルを生成できません" );
		return false;
	}

	return true;
}


