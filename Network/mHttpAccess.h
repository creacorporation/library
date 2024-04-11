//----------------------------------------------------------------------------
// HTTPアクセス
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------


#ifndef MHTTPACCESS_H_INCLUDED
#define MHTTPACCESS_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mHttpConnection.h"
#include "mHttpRequest.h"
#include <winhttp.h>
#include <memory>

#pragma comment( lib , "winhttp.lib" )

//メモ
//・PHPで警告・エラーがでると、WinHTTP側がエラーになることがある
//　- ERROR_WINHTTP_INVALID_SERVER_RESPONSEが返ってくる
//  - PHP側で未定義変数を参照して警告が出た場合など
//　- HTTPdのステータスコードは200になっているのでわかりにくい
//・参考になるかもしれないURL
// http://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa384273(v=vs.85).aspx
// http://eternalwindows.jp/network/winhttp/winhttp01.html

namespace Definitions_HttpAccess
{
	enum OptionType
	{
		NOPROXY,
		WITHPROXY
	};

	enum OptionProtocolVersion
	{
		//HTTP1.1を使用する
		HTTP_VERSION_11,
		//HTTP2.0を使用する
		//なお、mHttpRequest::QueryHeader()を使ってHTTPバージョンを問い合わせると、
		//実際にはHTTP2.0で通信していても1.1と報告してくることがあるっぽいのでIsHttp2()で確認すること。
		HTTP_VERSION_20,
	};
};

class mHttpAccess
{
public:

	mHttpAccess();
	virtual ~mHttpAccess();

	//-----------------------------------------------------------------------
	// 初期設定
	//-----------------------------------------------------------------------

	using OptionType = Definitions_HttpAccess::OptionType;
	using OptionProtocolVersion = Definitions_HttpAccess::OptionProtocolVersion;
	struct AccessOption
	{
		const Definitions_HttpAccess::OptionType OptionType;
	protected:
		AccessOption() = delete;
		AccessOption( Definitions_HttpAccess::OptionType type ) : OptionType( type )
		{
			ProtocolVersion = OptionProtocolVersion::HTTP_VERSION_11;
			UserAgent = L"";
		}
	public:
		OptionProtocolVersion ProtocolVersion;	//使用するプロトコルバージョン
		WString UserAgent;						//ユーザエージェント
	};

	//プロキシを使用しない場合のオプション
	struct AccessOption_NoProxy : public AccessOption
	{
		AccessOption_NoProxy() : AccessOption( Definitions_HttpAccess::OptionType::NOPROXY )
		{
		}
	};

	//プロキシを使用する場合のオプション
	struct AccessOption_WithProxy : public AccessOption
	{
		AccessOption_WithProxy() : AccessOption( Definitions_HttpAccess::OptionType::WITHPROXY )
		{
			ProxyServer = L"";
			NoProxyList = L"";
		}
		WString ProxyServer;		//使用するプロキシサーバ(空文字列でデフォルト)
		WString NoProxyList;		//プロキシを使用しないURL(セミコロン区切り)
	};

	//初期設定
	bool Setup( const AccessOption& opt );

	//セッションハンドルが生成されているかを返す
	operator bool() const;

	//-----------------------------------------------------------------------
	// 接続の生成
	//-----------------------------------------------------------------------

	using ConnectionOption = mHttpConnection::ConnectionOption;

	//新しい接続を生成する
	//info : 接続先情報
	//retConn : 接続した結果得られたハンドル
	//ret : 接続成功時真
	bool NewConnection( const ConnectionOption& info , mHttpConnection& retConn );

private:

	mHttpAccess( const mHttpAccess& source ) = delete;
	const mHttpAccess& operator=( const mHttpAccess& source ) = delete;

protected:

	//ハンドル
	HINTERNET MySession;

	std::unique_ptr< AccessOption > MyOption;

};

//使用例
#if 0

#include <mStandard.h>
#include <Network/mHttpAccess.h>

int main( int argc , char** argv )
{
	InitializeLibrary();

	mHttpAccess http;

	//WinHTTPのセッションオブジェクトを作成
	mHttpAccess::AccessOption_NoProxy accessopt;
	accessopt.UserAgent = L"TEST";
	accessopt.ProtocolVersion = mHttpAccess::OptionProtocolVersion::HTTP_VERSION_11;

	if( !http.Setup( accessopt ) )
	{
		return 1;
	}

	//セッションオブジェクトに接続先情報（アドレスとポート）を渡して接続オブジェクトを作成
	mHttpConnection conn;
	mHttpConnection::ConnectionOption connopt;
	connopt.ServerName = L"https://192.168.0.1/index.php";
	connopt.Port = 443;

	if( !http.NewConnection( connopt , conn ) )
	{
		return 2;
	}

	//接続オブジェクトにリクエスト情報（動詞やらURLやら）を渡してリクエストオブジェクトを作成
	mHttpRequest req;
	mHttpRequest::RequestOption reqopt;
	reqopt.Url = L"https://192.168.0.1/index.php";
	reqopt.Verb = mHttpRequest::RequestVerb::VERB_GET;
	reqopt.Secure = true;	//httpsの場合true

	if( !conn.NewRequest( reqopt , req ) )
	{
		return 3;
	}

	//SSLエラーを無視する場合は以下のような設定をいれる
	//mHttpRequest::SslIgnoreErrors sslerr;
	//sslerr.IgnoreInvalidCA = true;
	//sslerr.IgnoreInvalidCommonName = true;
	//if( !req.SetSslIgnoreErrors( sslerr ) )
	//{
	//	return 5;
	//}

	//リクエスト送信前に、あらかじめPOSTするデータとかを書き込むこともできる
	//あらかじめ書き込んでおけば、リクエスト送信時に勝手に送信する。
	//あらかじめ書き込むデータについては、Execute()の引数に指定するデータサイズに含めなくて良い
	//req.WriteString( "value1=123\r\n" );
	//req.WriteString( "value2=456\r\n" );
	//req.WriteString( "value3=789\r\n" );

	//リクエストを送信
	if( !req.Execute( 0 ) )
	{
		return 4;
	}

	//リクエスト送信後に、POSTするデータを書き込むこともできる
	//ただし、この場合はExecute()に書き込むバイト数を正確に指定する必要がある
	//もっぱらファイルやバイナリデータの送信に使う
	//BYTE data[ 1024 ];
	//req.WriteBinary( data , sizeof( data ) );

	//レスポンス待ち
	do
	{
		if( req.IsEOF() )
		{
			//ヘッダが来る前にIsEOF()が真になったら通信エラー
			printf( "通信エラー\n" );
			return 6;
		}
		SleepEx( 1000 , true );
	} while( !req.IsHeaderAvailable() );

	//ステータスコードを取得
	DWORD status_code = req.GetStatusCode();

	//結果取得しコンソールに出力
	while( !req.IsEOF() )
	{
		AString str;
		if( !req.ReadLine( str ) )
		{
			continue;
		}

		printf( str.c_str() );
		printf( "\n" );
	}


	return 0;
}
#endif

#endif //MHTTPACCESS_H_INCLUDED
