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


#ifndef MHTTPREQUEST_H_INCLUDED
#define MHTTPREQUEST_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mFileReadStreamBase.h"
#include "General/mFileWriteStreamBase.h"
#include "General/mCriticalSectionContainer.h"
#include "General/mDateTime.h"
#include "General/mNotifyOption.h"

#include <winhttp.h>

#pragma comment( lib , "winhttp.lib" )

//参考になるかもしれないURL
// http://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa384273(v=vs.85).aspx
// http://eternalwindows.jp/network/winhttp/winhttp01.html

namespace Definitions_HttpRequest
{
	//HTTP動詞
	enum RequestVerb
	{
		VERB_GET,			//GETを使用する
		VERB_PUT,			//PUTを使用する
		VERB_POST,			//POSTを使用する
		VERB_HEADER,		//HEADERを使用する
	};

	enum HeaderFlag
	{
		ACCEPT = WINHTTP_QUERY_ACCEPT ,
		ACCEPT_CHARSET = WINHTTP_QUERY_ACCEPT_CHARSET ,
		ACCEPT_ENCODING = WINHTTP_QUERY_ACCEPT_ENCODING ,
		ACCEPT_LANGUAGE = WINHTTP_QUERY_ACCEPT_LANGUAGE ,
		ACCEPT_RANGES = WINHTTP_QUERY_ACCEPT_RANGES ,
		AGE = WINHTTP_QUERY_AGE ,
		ALLOW = WINHTTP_QUERY_ALLOW ,
		AUTHENTICATION_INFO = WINHTTP_QUERY_AUTHENTICATION_INFO ,
		AUTHORIZATION = WINHTTP_QUERY_AUTHORIZATION ,
		CACHE_CONTROL = WINHTTP_QUERY_CACHE_CONTROL ,
		CONNECTION = WINHTTP_QUERY_CONNECTION ,
		CONTENT_BASE = WINHTTP_QUERY_CONTENT_BASE ,
		CONTENT_ENCODING = WINHTTP_QUERY_CONTENT_ENCODING ,
		CONTENT_ID = WINHTTP_QUERY_CONTENT_ID ,
		CONTENT_LANGUAGE = WINHTTP_QUERY_CONTENT_LANGUAGE ,
		CONTENT_LENGTH = WINHTTP_QUERY_CONTENT_LENGTH ,
		CONTENT_LOCATION = WINHTTP_QUERY_CONTENT_LOCATION ,
		CONTENT_MD5 = WINHTTP_QUERY_CONTENT_MD5 ,
		CONTENT_RANGE = WINHTTP_QUERY_CONTENT_RANGE ,
		CONTENT_TRANSFER_ENCODING = WINHTTP_QUERY_CONTENT_TRANSFER_ENCODING ,
		CONTENT_TYPE = WINHTTP_QUERY_CONTENT_TYPE ,
		COOKIE = WINHTTP_QUERY_COOKIE ,
		DATE = WINHTTP_QUERY_DATE ,
		ETAG = WINHTTP_QUERY_ETAG ,
		EXPECT = WINHTTP_QUERY_EXPECT ,
		EXPIRES = WINHTTP_QUERY_EXPIRES ,
		FROM = WINHTTP_QUERY_FROM ,
		HOST = WINHTTP_QUERY_HOST ,
		IF_MATCH = WINHTTP_QUERY_IF_MATCH ,
		IF_MODIFIED_SINCE = WINHTTP_QUERY_IF_MODIFIED_SINCE ,
		IF_NONE_MATCH = WINHTTP_QUERY_IF_NONE_MATCH ,
		IF_RANGE = WINHTTP_QUERY_IF_RANGE ,
		IF_UNMODIFIED_SINCE = WINHTTP_QUERY_IF_UNMODIFIED_SINCE ,
		LAST_MODIFIED = WINHTTP_QUERY_LAST_MODIFIED ,
		LOCATION = WINHTTP_QUERY_LOCATION ,
		MAX_FORWARDS = WINHTTP_QUERY_MAX_FORWARDS ,
		MIME_VERSION = WINHTTP_QUERY_MIME_VERSION ,
		PRAGMA = WINHTTP_QUERY_PRAGMA ,
		PROXY_AUTHENTICATE = WINHTTP_QUERY_PROXY_AUTHENTICATE ,
		PROXY_AUTHORIZATION = WINHTTP_QUERY_PROXY_AUTHORIZATION ,
		PROXY_CONNECTION = WINHTTP_QUERY_PROXY_CONNECTION ,
		PROXY_SUPPORT = WINHTTP_QUERY_PROXY_SUPPORT ,
		PUBLIC = WINHTTP_QUERY_PUBLIC ,
		RANGE = WINHTTP_QUERY_RANGE ,
		RAW_HEADERS_CRLF = WINHTTP_QUERY_RAW_HEADERS_CRLF ,
		REFERER = WINHTTP_QUERY_REFERER ,
		REQUEST_METHOD = WINHTTP_QUERY_REQUEST_METHOD ,
		RETRY_AFTER = WINHTTP_QUERY_RETRY_AFTER ,
		SERVER = WINHTTP_QUERY_SERVER ,
		SET_COOKIE = WINHTTP_QUERY_SET_COOKIE ,
		STATUS_CODE = WINHTTP_QUERY_STATUS_CODE ,
		STATUS_TEXT = WINHTTP_QUERY_STATUS_TEXT ,
		TRANSFER_ENCODING = WINHTTP_QUERY_TRANSFER_ENCODING ,
		UNLESS_MODIFIED_SINCE = WINHTTP_QUERY_UNLESS_MODIFIED_SINCE ,
		UPGRADE = WINHTTP_QUERY_UPGRADE ,
		URI = WINHTTP_QUERY_URI ,
		USER_AGENT = WINHTTP_QUERY_USER_AGENT ,
		VARY = WINHTTP_QUERY_VARY ,
		VERSION = WINHTTP_QUERY_VERSION ,
		VIA = WINHTTP_QUERY_VIA ,
		WARNING = WINHTTP_QUERY_WARNING ,
		WWW_AUTHENTICATE = WINHTTP_QUERY_WWW_AUTHENTICATE
	};

	//リクエストの状態
	enum RequestStatus
	{
		REQUEST_INITIALIZED,
		REQUEST_PREEXEC,		//書き込み受付中(リクエスト送信前)
		REQUEST_POSTEXEC,		//書き込み受付中(リクエスト送信後)
		REQUEST_CLOSED,			//書き込み終了
		REQUEST_RECEIVING,		//応答待ち
		REQUEST_COMPLETED,		//受信終了
		REQUEST_FINISHED,		//通信終了
	};

	//キューの状態
	enum BufferQueueStatus
	{
		BUFFERQUEUE_CREATED,	//作られただけでまだ送信・受信は行われていない
		BUFFERQUEUE_PROCEEDING,	//システム内で送信・受信処理が行われている
		BUFFERQUEUE_COMPLETED,	//送信・受信処理が終わり、結果の確認待ち
		BUFFERQUEUE_ERROR,		//処理がエラーになった
	};
};

class mHttpReadStream : public mFileReadStreamBase
{
public:
	//読み取り側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mHttpWriteStream : public mFileWriteStreamBase
{
public:

	//書き込み側の経路が開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const
	{
		return IsWritable();
	}

protected:
	virtual bool IsWritable( void )const = 0;

};

class mHttpRequest : public mHttpReadStream , public mHttpWriteStream
{
public:
	mHttpRequest();
	virtual ~mHttpRequest();

	//-----------------------------------------------------------------------
	// 初期設定用関数
	//-----------------------------------------------------------------------

	//動詞
	using RequestVerb = Definitions_HttpRequest::RequestVerb;

	//受け付けるメディアタイプのリスト
	using MediaTypeList = WStringDeque;

	//リクエスト設定
	struct RequestOption
	{
		//-----------
		//動作設定
		//-----------
		RequestVerb	Verb;			//HTTP動詞
		bool Reload;				//trueの場合リロードする(キャッシュ無効)
		bool Secure;				//trueの場合SSLを使用する
		WString Referer;			//リファラー
		WString Url;				//取得したいオブジェクト
		MediaTypeList AcceptType;	//受け付けるメディアタイプ(何も指定しないとテキストのみ。後で追加・更新も可能。)

		//-----------
		//バッファ設定
		//-----------
		DWORD SendPacketSize;				//送信時に確保するバッファ１個あたりのサイズ
		DWORD RecievePacketSize;			//受信時に確保するバッファ１個あたりのサイズ
		DWORD RecievePacketMaxActive;		//受信時に確保するバッファの数
		DWORD RecievePacketMaxStock;		//ここで指定した数以上に受信済みで未読み取りのバッファが増えたら受信を停止する

		//-----------
		//初期値
		//-----------

		RequestOption()
		{
			Verb = RequestVerb::VERB_GET;
			Reload = true;
			Secure = false;
			SendPacketSize = 10240;
			RecievePacketSize = 10240;
			RecievePacketMaxActive = 3;
			RecievePacketMaxStock = 128;
		}
	};


	union NotifyFunctionOpt
	{
		struct OnSendOpt
		{
			DWORD BytesSent;
		}OnSend;

		//コールバックでエラー通知を受け取った場合のエラー情報
		struct OnErrorOpt
		{
			DWORD_PTR Api;
			DWORD ErrorCode;
		}OnError;

		//コールバックでSSLエラー通知を受け取った場合のエラー情報
		struct OnSslErrorOpt
		{
			bool IsCertRevocationFailed;	//証明書のチェックそのものができなかった(証明書が不正とは限らない)
			bool IsInvalidCert;				//証明書がニセモノ
			bool IsCertRevoked;				//証明書が失効してる
			bool IsInvalidCA;				//信頼済みCAから発行されたものではない、またはオレオレ証明書
			bool IsInvalidCommonName;		//証明書とURLが一致してない
			bool IsInvalidDate;				//証明書が期限切れ
			bool IsChannelError;			//その他よくわからないエラー。例えばHTTPのポートにHTTPSでつないだ場合とか。
		}OnSslError;

		// struct OnCancelOpt
		// {
		// 	//no member yet
		// }OnCancel;

		// struct OnHeaderAvailableOpt
		// {
		// 	//no member yet
		// }OnHeaderAvailable;

		//static const DWORD OnHeaderAvailable = 0;

		struct OnReceiveOpt
		{
			DWORD BytesReceived;
		}OnReceive;
	};

	//コールバックで通知する場合のプロトタイプ
	//req : コールバックを発生させたオブジェクトの参照
	//parameter : オブジェクト生成時に渡した任意の値(NotifierInfo::Parameterの値)
	//opt : 拡張情報
	using NotifyFunction = void(*)( mHttpRequest& req , DWORD_PTR parameter , const NotifyFunctionOpt& opt );

	//通知設定
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		//データの書き込みが行われた場合の通知
		NotifierInfo OnSend;
		//エラーが発生した場合の通知
		NotifierInfo OnError;
		//エラーが発生した場合の通知
		NotifierInfo OnSslError;
		//処理がキャンセルされた場合の通知
		NotifierInfo OnCancel;
		//ヘッダが利用可能になった場合の通知
		NotifierInfo OnHeaderAvailable;
		//データを受信した場合の通知
		NotifierInfo OnReceive;
	};

	//-----------------------------------------------------------------------
	// 各種オペレーション
	//-----------------------------------------------------------------------

	//追加するヘッダのリスト
	bool SetHeader( const WString& header );

	//リクエストの送信
	// AdditionalPostDataSize ： 追加のデータサイズ
	//  ※Execute実行以降に、Post等で送信するデータのサイズ（不定にしたり後で変更したりすることはできません）
	//  ※GETなど、入力するデータがないリクエストなのに0以外を指定したらエラー
	// ret : 成功時真
	bool Execute( DWORD AdditionalDataSize );

	//読み込み用の内部バッファを確保します
	//臨時にバッファが必要になるときに使用します
	// count : 内部バッファが指定した数未満であれば、その数になるように内部バッファを新たに作成します
	bool PrepareReadBuffer( DWORD count );

	//ハンドルが生成されているかを返す
	operator bool() const;

	//-----------------------------------------------------------------------
	// セキュリティ設定
	//-----------------------------------------------------------------------

	struct SslIgnoreErrors
	{
		bool DisableRevocation;				//真の場合、そもそも証明書のチェックをしない
	#ifdef WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE
		bool IgnoreCertRevocationFailed;	//真の場合、証明書のチェックそのものができなかった場合もOK
	#endif
		bool IgnoreInvalidCA;				//真の場合、信頼済みCAから発行されたものではない、またはオレオレ証明書でもOK
		bool IgnoreInvalidCommonName;		//真の場合、証明書とURLが一致してなくてもOK
		bool IgnoreInvalidDate;				//真の場合、証明書が期限切れでもOK

		SslIgnoreErrors()
		{
			DisableRevocation = false;
		#ifdef WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE
			IgnoreCertRevocationFailed = false;
		#endif
			IgnoreInvalidCA = false;
			IgnoreInvalidCommonName = false;
			IgnoreInvalidDate = false;
		}
	};

	//SSLの証明書検証エラーになっても無視する設定をする
	// opt : 無視するエラーの種類を示す構造体
	// ret : 成功時真
	bool SetSslIgnoreErrors( const SslIgnoreErrors opt );

	//-----------------------------------------------------------------------
	// ヘッダとステータスコード
	//-----------------------------------------------------------------------

	//ヘッダ・ステータスコードが取得できるかを調べる
	// ret : 真ならば取得できる
	bool IsHeaderAvailable( void )const;

	//ステータスコードを得ます
	// ret : ステータスコード。エラー時は0
	DWORD GetStatusCode( void )const;

	//レスポンスのヘッダ種別
	using HeaderFlag = Definitions_HttpRequest::HeaderFlag;

	//レスポンスのヘッダを得ます
	// flag : 得たいヘッダ
	// ret : 得られた結果。エラーの場合はerrstrに指定した文字列
	WString QueryHeaderDirect( HeaderFlag flag , const WString& errstr )const;

	//レスポンスのヘッダを得ます
	// flag : 得たいヘッダ
	// retHeader : 得られた結果。エラーの場合は空文字列。
	// ret : 成功時真
	bool QueryHeader( HeaderFlag flag , WString& retHeader )const;

	//レスポンスのヘッダを数値で得ます
	// flag : 得たいヘッダ
	// ret : 得られた結果。エラーの場合はerrvalに指定した値
	DWORD QueryHeaderDirect( HeaderFlag flag , DWORD errval )const;

	//レスポンスのヘッダを数値で得ます
	// flag : 得たいヘッダ
	// retHeader : 得られた結果。エラーの場合は0。
	// ret : 成功時真
	bool QueryHeader( HeaderFlag flag , DWORD& retHeader )const;

	//レスポンスのヘッダを数値で得ます
	// flag : 得たいヘッダ
	// ret : 得られた結果。エラーの場合はerrvalに指定した値
	uint64_t QueryHeaderDirect( HeaderFlag flag , uint64_t errval )const;

	//レスポンスのヘッダを数値で得ます
	// flag : 得たいヘッダ
	// retHeader : 得られた結果。エラーの場合は0。
	// ret : 成功時真
	bool QueryHeader( HeaderFlag flag , uint64_t& retHeader )const;

	//レスポンスのヘッダを時刻で得ます
	// flag : 得たいヘッダ
	// retHeader : 得られた結果。エラーの場合は不定。
	// ret : 成功時真
	bool QueryHeader( HeaderFlag flag , mDateTime::Timestamp& retHeader )const;

	//HTTP Version2で通信していれば真を返す
	// ret : HTTP2であれば真、そうでない、またはエラーの場合は偽
	bool IsHttp2( void )const;

	//暗号化キーの長さを得る
	// ret : 暗号化キーのビット数。暗号化されていないかエラーだと0
	DWORD GetEncryptionKeyLength( void )const;

	//-----------------------------------------------------------------------
	// ストリーム書き込み系
	//-----------------------------------------------------------------------

	//１文字書き込み
	// data : 送信するデータ(1バイト)
	// ret : 成功時真
	//・Exec()呼び出し前に行った書き込みは、Exec()呼び出し後に順次送信します
	//  この場合、Exec()に渡す送信サイズのことは考えなくても良いですが(自動計算されます)、
	//  送信完了までデータのコピーを保持するため、大きなデータを扱う場合は、メモリを浪費します。
	//・Exec()呼び出し後に行った書き込みは、順次送信します
	//  この場合、Exec()に渡した送信サイズとぴったり同じサイズのデータを与えなければなりませんが、
	//  随時メモリを解放するので、メモリには優しくなります。
	//・Exec()呼び出し前のWriteと、呼び出し後のWriteは併用してOK
	//・GETなど、入力するデータがないリクエストなのにこのメソッドを呼んだらエラー
	virtual bool Write( INT data );

	//キャッシュを書き込み
	// ※GETなど、入力するデータがないリクエストなのにこのメソッドを呼んだらエラー
	// ret : 成功時真
	virtual bool FlushCache( void );

	//書き込み可能かを返す
	// ret : 書き込み可能であれば真
	virtual bool IsWritable( void )const;

	//-----------------------------------------------------------------------
	// ストリーム読み込み系
	//-----------------------------------------------------------------------

	//１文字（１バイト）読み込みます
	virtual INT Read( void );

	//EOFに達しているかを調べます
	virtual bool IsEOF( void )const;

private:
	mHttpRequest( const mHttpRequest& source ) = delete;
	const mHttpRequest& operator=( const mHttpRequest& source ) = delete;

	friend class mHttpConnection;

protected:

	//------------------------
	// ハンドルと状態変数関係
	//------------------------

	//リクエストのハンドル
	HINTERNET MyRequest;

	//クリティカルセクション
	mutable mCriticalSectionContainer MyCritical;

	//生成時のオプション
	RequestOption MyOption;

	//通知オプション
	NotifyOption MyNotifyOption;

	//内部状態
	using RequestStatus = Definitions_HttpRequest::RequestStatus;
	RequestStatus MyRequestStatus;

	//レスポンスのヘッダーを読み取り可能かどうか
	bool MyIsResponseHeaderAvailable;

	//------------------------
	// キュー関係
	//------------------------

	//キューの状態
	using BufferQueueStatus = Definitions_HttpRequest::BufferQueueStatus;

	//キュー
	struct BufferQueueEntry
	{
		//送受信するデータ
		BYTE* Buffer;

		//データのサイズ
		DWORD Size;

		//キューの状態
		BufferQueueStatus Status;

		//完了時のエラーコード(IO完了時に設定)
		DWORD ErrorCode;

		//完了時の処理済みバイト数(IO完了時に設定)
		DWORD BytesTransfered;
	};

	typedef std::deque<BufferQueueEntry> BufferQueue;

	//ライトバッファ（ユーザープログラムの書き込み中・未送信）
	BufferQueue MyWriteQueue;

	//ライトバッファ（送信中）
	BufferQueue MyWriteActiveQueue;

	//リードバッファ（受信中）
	BufferQueue MyReadActiveQueue;

	//リードバッファ（受信済み・ユーザープログラムの読み取り待ち）
	BufferQueue MyReadQueue;

	//WinHttpからのコールバック関数
	static void __stdcall WinhttpStatusCallback(
		IN HINTERNET hInternet ,
		IN DWORD_PTR dwContext ,
		IN DWORD dwInternetStatus ,
		IN LPVOID lpvStatusInformation ,
		IN DWORD dwStatusInformationLength
	);

	//キューの送信を行う
	// ret : 成功時真
	bool ExecWriteQueue( void );

	//WinHTTPからのコールバックから得た書き込み済みサイズを元に、完了したバッファをパージする
	// written_size : WinHTTPから得た書き込み完了したバイト数
	// ret : 成功時真
	bool CompleteWriteQueue( DWORD written_size );

	//一つのBufferQueueEntryを完了させる
	// buffer : WinHTTPのコールバックから渡された完了ずみの受信バッファ
	// read_size : WinHTTPのコールバックから渡された読み取ったサイズ
	// ret : 新たにユーザーに引き渡し可能なデータが発生した場合はtrue
	// ※完了したBufferQueueEntryがキューの先頭であればtrue
	//   先頭でない場合でも、先頭のキューが完了済みになっている場合(イレギュラー)もtrue
	//   それ以外はfalse
	bool CompleteReadQueue( BYTE* buffer , DWORD read_size );

	//------------------------
	// その他のオペレーション
	//------------------------

	//受信を開始する
	// ret : 成功時真
	bool StartReceiveResponse( void );

	//書き込みの完了
	//HTTPでは不要
	virtual bool Close( void );

};

#endif //MHTTPACCESS_H_INCLUDED
