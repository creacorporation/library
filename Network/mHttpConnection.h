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


#ifndef MHTTPCONNECTION_H_INCLUDED
#define MHTTPCONNECTION_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mHttpRequest.h"
#include <winhttp.h>
#include <list>

#pragma comment( lib , "winhttp.lib" )

//参考になるかもしれないURL
// http://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa384273(v=vs.85).aspx
// http://eternalwindows.jp/network/winhttp/winhttp01.html

namespace Definitions_HttpConnection
{
};

class mHttpConnection
{
public:
	mHttpConnection();
	virtual ~mHttpConnection();

	//-----------------------------------------------------------------------
	// 初期設定用関数
	//-----------------------------------------------------------------------

	struct ConnectionOption
	{
		//接続先サーバの名前
		WString ServerName;
		//ポート番号
		USHORT Port;
	};

	//リクエスト設定
	using RequestOption = mHttpRequest::RequestOption;

	//通知設定
	using NotifyOption = mHttpRequest::NotifyOption;

	//リクエストを生成
	bool NewRequest( const RequestOption& opt , const NotifyOption& notifier , mHttpRequest& retReq );

	//ハンドルが生成されているかを返す
	operator bool() const;

private:
	mHttpConnection( const mHttpConnection& source );
	const mHttpConnection& operator=( const mHttpConnection& source );

	friend class mHttpAccess;

protected:

	//ハンドル
	HINTERNET MyConnection;

	//現在接続しているサーバのアドレス
	WString MyServerAddress;

	//接続先のポート
	USHORT MyPort;

};

#endif
