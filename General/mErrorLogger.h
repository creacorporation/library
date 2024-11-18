//----------------------------------------------------------------------------
// アサート／エラー処理
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2020-2023 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

/*

●用途
アサート発生時またはエラー発生時の処理です。

*/

#ifndef MERRORLOGGER_H_INCLUDED
#define MERRORLOGGER_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "mCriticalSectionContainer.h"
#include <deque>

#ifdef MERRORLOGGER_NO_FILENAME
#define ERROR_LOGGER_CURRENT_FILE (L"N/A")
#else
#define ERROR_LOGGER_CURRENT_FILE mCURRENT_FILE
#endif

class mErrorLogger
{
public:

	//エラー発生履歴を保持する最大数(デフォルト)
	static const DWORD DefaultMaxErrorLogEntry = 512;

	//コンストラクタ
	//max_error : エラー発生履歴を保持する最大数
	mErrorLogger( DWORD max_error = DefaultMaxErrorLogEntry );

	virtual ~mErrorLogger();

	//ログをコンソールに出力するか
	enum LogOutputMode
	{
		LOG_OUTPUT_CONSOLE,			//コマンドライン(stderr)に出力する
		LOG_OUTPUT_DEBUGGER,		//デバッガに出力する
		LOG_OUTPUT_FILE,			//指定ファイルに出力する
		LOG_OUTPUT_FILE_SIMPLE,		//指定ファイルに出力する
		LOG_OUTPUT_EVENTLOG,		//Windowsのイベントログに出力する(未実装)
		LOG_OUTPUT_CALLBACK,		//コールバック関数を呼ぶ
		LOG_OUTPUT_NONE,			//何もしない
	};

	//エラーのレベル
	enum ErrorLevel
	{
		//アサートが発生したとき(通常操作で発生しない想定のエラー用)
		//Windowsログに記録した場合は、「エラーイベント」となります。
		LEVEL_ASSERT = 0,
		//一般的エラーが発生したとき(ファイルが無かった等)
		//Windowsログに記録した場合は、「警告イベント」となります。
		LEVEL_ERROR = 1,
		//正常終了でも記録しておきたい事柄が発生したとき(接続完了などのイベント)
		//Windowsログに記録した場合は、「情報イベント」となります。
		LEVEL_LOGGING = 2,
		//例外がスローされたとき(mExceptionが使用します)
		//Windowsログに記録した場合は、「エラーイベント」となります。
		LEVEL_EXCEPTION = 3,
	};

	//エラーログのエントリ
	struct LogEntry
	{
		DWORD Id;				//ログの連番
		ErrorLevel Level;		//エラーレベル
		WString File;			//エラーが発生したファイル名
		DWORD Line;				//エラーが発生した行
		DWORD Code1;			//エラーコード(GetLastErrorで取得した物)
		ULONG_PTR Code2;		//エラーコード(ユーザー定義)
		WString Message1;		//ユーザー定義のメッセージ
		WString Message2;		//ユーザー定義のメッセージ
		DWORD Time;				//発生時刻（Tick）
		SYSTEMTIME LocalTime;	//発生時刻（ローカル時刻）
		DWORD ThreadId;			//ログを記録したスレッドのID
	};
	typedef std::deque<LogEntry> Log;

	//オプション構造体
	struct LogOutputModeOpt
	{
	public:
		const LogOutputMode Mode;

		//自オブジェクトにログを記録した後、指定したオブジェクトもログの内容を転送する
		//転送不要であればnullptr
		mErrorLogger* Proxy;

		//trueの場合、メモリ内に情報を保持しない
		//このオプションを変更しても、すでにメモリ内に保持している情報には影響しない
		//（追加のオンオフを変更するだけ）
		bool NoTrace;

	protected:
		LogOutputModeOpt() = delete;
		LogOutputModeOpt( LogOutputMode mode ) : Mode( mode )
		{
			Proxy = nullptr;
			NoTrace = false;
		}
	};
	//ログをコンソールに出力する場合の設定オブジェクト
	struct LogOutputModeOpt_Console : public LogOutputModeOpt
	{
		LogOutputModeOpt_Console() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_CONSOLE )
		{
		}
	};
	//ログをデバッガに出力する場合の設定オブジェクト
	struct LogOutputModeOpt_Debugger : public LogOutputModeOpt
	{
		LogOutputModeOpt_Debugger() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_DEBUGGER )
		{
		}
	};
	//ログをファイルに出力する場合の設定オブジェクト
	struct LogOutputModeOpt_File : public LogOutputModeOpt
	{
		WString FileName;	//ファイル名
		bool IsAppend;		//true=既存のファイルに追記する false=既存のファイルの中身は消す
		LogOutputModeOpt_File() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_FILE )
		{
			IsAppend = true;
		}
	};
	//ログをファイルに出力する場合の設定オブジェクト
	struct LogOutputModeOpt_FileSimple : public LogOutputModeOpt
	{
		WString FileName;	//ファイル名
		bool IsAppend;		//true=既存のファイルに追記する false=既存のファイルの中身は消す
		LogOutputModeOpt_FileSimple() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_FILE_SIMPLE )
		{
			IsAppend = true;
		}
	};
	//ログをWindowsのイベントに出力する場合の設定オブジェクト
	struct LogOutputModeOpt_EventLog : public LogOutputModeOpt
	{
		WString ServerName;	//記録先のサーバ名（空文字列にするとローカルと見なします）
		WString SourceName;	//イベントのソース名

		LogOutputModeOpt_EventLog() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_EVENTLOG )
		{
		}
	};

	//ログ時に呼び出すコールバック
	using LogCallback = void (*)( const LogEntry& entry );

	//ログ時にコールバックする場合の設定オブジェクト
	struct LogOutputModeOpt_Callback : public LogOutputModeOpt
	{
		LogCallback Callback;
		LogOutputModeOpt_Callback() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_CALLBACK )
		{
			Callback = nullptr;
		}
	};
	//ログを出力しない場合の設定オブジェクト
	struct LogOutputModeOpt_None : public LogOutputModeOpt
	{
		LogOutputModeOpt_None() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_NONE )
		{
		}
	};

	//ログ出力のモードを変更する
	// setting : 新しい設定
	// ret : 成功時真
	[[deprecated("Please use LogOutputModeOpt structure instead.")]]
	bool ChangeLogOutputMode( LogOutputMode setting );

	//ログ出力のモードを変更する
	//※初回に限らず、任意のタイミングで変更することができます
	threadsafe bool ChangeLogOutputMode( const LogOutputModeOpt& setting );

	//エラーを発生します
	//file : エラーが発生したファイル名
	//line : エラーが発生した行
	//ec1 : エラーコード(GetLastErrorで取得した物)
	//ec2 : エラーコード(ユーザー定義)
	//mes : ユーザー定義のメッセージ
	//ret : 追加したエラーのログ連番(失敗はありません)
	//　　　Proxyを設定している場合の注意点：返されるログ連番は、このインスタンスに対する番号となります。Proxy先の番号ではありません。
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 );

	//エラーを発生します
	//file : エラーが発生したファイル名
	//line : エラーが発生した行
	//ec1 : エラーコード(GetLastErrorで取得した物)
	//ec2 : エラーコード(ユーザー定義)
	//mes : ユーザー定義のメッセージ
	//ret : 追加したエラーのログ連番(失敗はありません)
	//　　　Proxyを設定している場合の注意点：返されるログ連番は、このインスタンスに対する番号となります。Proxy先の番号ではありません。
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringDeque& mes2 );

	//エラーを発生します
	//file : エラーが発生したファイル名
	//line : エラーが発生した行
	//ec1 : エラーコード(GetLastErrorで取得した物)
	//ec2 : エラーコード(ユーザー定義)
	//mes : ユーザー定義のメッセージ
	//ret : 追加したエラーのログ連番(失敗はありません)
	//　　　Proxyを設定している場合の注意点：返されるログ連番は、このインスタンスに対する番号となります。Proxy先の番号ではありません。
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringVector& mes2 );

	//エラーログを追加します
	//toAppend : 追記したいログ
	//ret : 追加したエラーのログ連番(失敗はありません)
	//・この方法でログを追加した場合、コンソール等への出力はありません。
	//・この方法でログを追加した場合、Proxyを設定していても転送されません。
	threadsafe DWORD AddEntry( const Log& toAppend );

	//現在のログ連番の値を調べる　
	//※次にログの追加があった場合に使用される連番の値
	DWORD GetCurrentId( void )const;

	//ログを検索する
	//指定したId以降で、このメソッドを呼び出したスレッドが記録したログの一覧を返す
	// Id : 検索範囲(※指定したIDと一致するログも含みます)
	// file : 検索範囲(ログを記録したファイルの名称。空文字列ならば全部)
	// retLog : 取得したログの格納先
	threadsafe void SearchLog( DWORD Id , Log& retLog , const WString& file = L"" )const;

	//ログを検索する
	//指定したId以降で、このメソッドを呼び出したスレッドが記録したログの中に、指定したcodeのログがあるかないかを返します
	// Id : 検索範囲(※指定したIDと一致するログも含みます)
	// code : 有無を判定するコード
	// file : 検索範囲(ログを記録したファイルの名称。空文字列ならば全部)
	// retLog : 取得したログの格納先
	threadsafe bool SearchLog( DWORD Id , ULONG_PTR code , const WString& file = L"" )const;

	//ログを取得する
	//指定したId以降のログをコピーする(記録したスレッドに関係なく全て取得します)
	// Id : 検索範囲(※指定したIDと一致するログも含みます)
	// retLog : コピー先
	// file : 検索範囲(ログを記録したファイルの名称。空文字列ならば全部)
	threadsafe void GetLog( DWORD Id , Log& retLog , const WString& file = L"" )const;

	//ログを取得する
	//指定したId以降のログをコピーする(記録したスレッドに関係なく全て取得します)
	// Id : 検索範囲(※指定したIDと一致するログも含みます)
	// retLog : コピー先
	// file : 検索範囲(ログを記録したファイルの名称。空文字列ならば全部)
	threadsafe void GetLog( DWORD Id_from , DWORD Id_to , Log& retLog , const WString& file = L"" )const;

	//エラーの発生回数を取得する
	DWORD GetErrorCount( ErrorLevel level )const;

	//ログを消去する
	//※発生回数も消去する
	void Clear( void );

	//ログの記録を無効化する
	void Disable( void );

	//ログの記録を有効化する
	void Enable( void );

private:
	mErrorLogger( const mErrorLogger& src ) = delete;
	mErrorLogger& operator=( const mErrorLogger& source ) = delete;

protected:

	//コンソールにログを出力する
	// entry : 出力するログ
	void OutputLogToConsole( const LogEntry& entry );

	//デバッガにログを出力する
	// entry : 出力するログ
	void OutputLogToDebugger( const LogEntry& entry );

	//ファイルにログを出力する
	// entry : 出力するログ
	void OutputLogToFile( const LogEntry& entry );

	//ファイルにログを出力する（シンプル）
	// entry : 出力するログ
	void OutputLogToFileSimple( const LogEntry& entry );

	//エラーを発生します
	//publicのAddEntryに、初回インスタンスのポインタを付与した物になります。Proxyがぐるぐる回らないようにするためのものです。
	//origin : Proxyからの呼び出しではない場合(一番最初の呼び出しの場合)は、nullptr
	//         Proxyからの呼び出しである場合は、一番最初に呼び出したオブジェクトのポインタ
	//ret : originがnullptrの場合、追加したエラーのログ連番
	//      originがnullptrではない場合、0
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 , const mErrorLogger* origin );

	//エラーを発生します
	//publicのAddEntryに、初回インスタンスのポインタを付与した物になります。Proxyがぐるぐる回らないようにするためのものです。
	//origin : Proxyからの呼び出しではない場合(一番最初の呼び出しの場合)は、nullptr
	//         Proxyからの呼び出しである場合は、一番最初に呼び出したオブジェクトのポインタ
	//ret : originがnullptrの場合、追加したエラーのログ連番
	//      originがnullptrではない場合、0
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringDeque& mes2 , const mErrorLogger* origin );

	//エラーを発生します
	//publicのAddEntryに、初回インスタンスのポインタを付与した物になります。Proxyがぐるぐる回らないようにするためのものです。
	//origin : Proxyからの呼び出しではない場合(一番最初の呼び出しの場合)は、nullptr
	//         Proxyからの呼び出しである場合は、一番最初に呼び出したオブジェクトのポインタ
	//ret : originがnullptrの場合、追加したエラーのログ連番
	//      originがnullptrではない場合、0
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringVector& mes2 , const mErrorLogger* origin );

protected:

	DWORD MyErrorCount[ 4 ];	//エラー発生回数

	Log MyLogError;		//エラーログ

	//出力先ハンドル(ファイル出力/イベントログ用)
	HANDLE MyHandle;

	//コールバックの場合のコールバック関数
	LogCallback MyCallback;

	//プロクシ先
	mErrorLogger* MyProxy;

	//ログの記録を行うか
	bool MyIsEnabled;

	//メモリ記録を行うか
	bool MyIsNoTrace;

	//現在の連番
	DWORD MyCurrentId;

	//エラーログの最大サイズ
	DWORD MyMaxErrorSize;

	//同時アクセス対策用クリティカルセクション
	mutable mCriticalSectionContainer MyCritical;

	//ログコンソール出力モード
	LogOutputMode MyLogOutputMode;

};

//グローバルオブジェクト
#ifndef MERRORLOGGER_CPP_COMPILING
extern mErrorLogger g_ErrorLogger;
#else
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
mErrorLogger g_ErrorLogger;
#endif

//RaiseErrorにmErrorLoggerがポインタで渡されても参照で渡されても良いようにするためのプロクシ（ポインタVer）
inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , DWORD_PTR val )
{
	if( obj == nullptr )
	{
		g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_ASSERT , file , line , 0 , 0 , L"エラー登録先オブジェクトがNULLです" , L"" );
		obj = &g_ErrorLogger;
	}

	WString mes2;
#ifdef _WIN64
	sprintf( mes2 , L"0x%llX(%lld)" , val , val );
#else
	sprintf( mes2 , L"0x%lX(%ld)" , val , val );
#endif
	return obj->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , DWORD_PTR val )
{
	return RaiseErrorInternal( obj , level , file , line , ec1 , ec2 , AString2WString( mes1 ) , val );
}

inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 = L"" )
{
	if( obj == nullptr )
	{
		g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_ASSERT , L"" , 0 , 0 , 0 , L"エラー登録先オブジェクトがNULLです" , L"" );
		obj = &g_ErrorLogger;
	}
	return obj->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString& mes2 = "" )
{
	return RaiseErrorInternal( obj , level , file , line , ec1 , ec2 , AString2WString( mes1 ) , AString2WString( mes2 ) );
}

DWORD RaiseErrorInternalF( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString mes2 , ... );
DWORD RaiseErrorInternalF( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString mes2 , ... );

//RaiseErrorにmErrorLoggerがポインタで渡されても参照で渡されても良いようにするためのプロクシ（参照Ver）
inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , DWORD_PTR val )
{
	WString mes2;
#ifdef _WIN64
	sprintf( mes2 , L"0x%llX(%lld)" , val , val );
#else
	sprintf( mes2 , L"0x%lX(%ld)" , val , val );
#endif
	return obj.AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , DWORD_PTR val )
{
	return RaiseErrorInternal( obj , level , file , line , ec1 , ec2 , AString2WString( mes1 ) , val );
}

inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 = L"" )
{
	return obj.AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString& mes2 = "" )
{
	return obj.AddEntry( level , file , line , ec1 , ec2 , AString2WString( mes1 ) , AString2WString( mes2 ) );
}

DWORD RaiseErrorInternalF( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString mes2 , ... );
DWORD RaiseErrorInternalF( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString mes2 , ... );

//動作ログ
//obj : エラー情報の登録先
//error_code : エラーコード(ユーザー定義)
//mes : ユーザー定義のメッセージ
//・ロギングは正常動作でも記録しておきたいイベント（接続完了とか）に対して生成します
#define CreateLogEntry(obj,error_code,...)			\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternal(								\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_LOGGING,	\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*CreateLogEntry*/

//動作ログ
//obj : エラー情報の登録先
//error_code : エラーコード(ユーザー定義)
// ...(可変長引数)は以下の通りです
//  1要素目 メッセージ(書式指定文字使用不可)
//  2要素目 書式指定文字列
//  3要素目以降 書式指定文字列に埋め込む値
//・ロギングは正常動作でも記録しておきたいイベント（接続完了とか）に対して生成します
#define CreateLogEntryF(obj,error_code,...)			\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternalF(							\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_LOGGING,	\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*CreateLogEntryF*/

//エラー発生
//obj : エラー情報の登録先
//error_code : エラーコード(ユーザー定義)
//mes : ユーザー定義のメッセージ
//・エラーは通常操作で起こりうるエラー（ファイルがなかったとか）に対して生成します。
#define RaiseError(obj,error_code,...)				\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternal(								\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ERROR,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseError*/

//エラー発生
//obj : エラー情報の登録先
//error_code : エラーコード(ユーザー定義)
// ...(可変長引数)は以下の通りです
//  1要素目 メッセージ(書式指定文字使用不可)
//  2要素目 書式指定文字列
//  3要素目以降 書式指定文字列に埋め込む値
//・エラーは通常操作で起こりうるエラー（ファイルがなかったとか）に対して生成します。
#define RaiseErrorF(obj,error_code,...)				\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternalF(							\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ERROR,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseErrorF*/


//アサート発生
//obj : アサート情報の登録先
//error_code : エラーコード(ユーザー定義)
//mes : ユーザー定義のメッセージ
//・アサートは通常操作では起こりえないはずのエラー（バグ濃厚のもの）に対して生成します。
#define RaiseAssert(obj,error_code,...)				\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternal(								\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ASSERT,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseAssert*/


//アサート発生
//obj : アサート情報の登録先
//error_code : エラーコード(ユーザー定義)
// ...(可変長引数)は以下の通りです
//  1要素目 メッセージ(書式指定文字使用不可)
//  2要素目 書式指定文字列
//  3要素目以降 書式指定文字列に埋め込む値
//・アサートは通常操作では起こりえないはずのエラー（バグ濃厚のもの）に対して生成します。
#define RaiseAssertF(obj,error_code,...)			\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternalF(							\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ASSERT,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseAssert*/

#endif	//MERRORLOGGER_H_INCLUDED

