//----------------------------------------------------------------------------
// WebView2ハンドラー
// Copyright (C) 2021 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------


#ifndef MWEBVIEW2_H_INCLUDED
#define MWEBVIEW2_H_INCLUDED
#include <mRevision.h>
#if defined(LIBRARY_ENABLE_WEBVIEW2) || !defined(MWEBVIWEW2_CPP_COMPILING)
#include <WinSock2.h>
#include <wrl.h>
#include <WebView2.h>
#include <mStandard.h>

#include "mWindow.h"
#include "../General/mNotifyOption.h"

namespace Definitions_WebView2
{
	//ナビゲーション開始時のパラメータ
	struct OnNavigationStartingOpt
	{
		//リダイレクトにより開始しようとしている場合true
		bool IsRedirected;
		//ユーザーが開始した操作である場合true
		bool IsUserInitiated;
		//処理を開始するURI
		WString Uri;
		//ナビゲーションの開始を許可する
		//※コールバック内でこのパラメータをfalseに書き換えるとナビゲーションはキャンセルされます
		bool IsAllowed;
	};

	//ナビゲーション完了時のパラメータ
	struct OnNavigationCompletedOpt
	{
		//正常終了か？
		bool IsSuccess;
		//エラーコードがあれば
		COREWEBVIEW2_WEB_ERROR_STATUS Status;
	};

	//InjectOnCreateScript()で注入したスクリプト完了時のパラメータ
	struct OnCreateScriptCompletedOpt
	{
		//正常終了か？
		bool IsSuccess;
		//完了したスクリプトID（削除する場合に使う）
		WString ScriptId;
	};

	//WebMessage受信時のパラメータ
	struct OnWebMessageRecievedOpt
	{
		//送信元
		WString Sender;
		//メッセージ(JSONフォーマット)
		WString Message;
		//メッセージがシンプルなテキストとして読める場合に限り、読み取った文字列。
		//読めない場合はnullptr。
		std::unique_ptr<WString> MessageAsString;
	};

	//Javascript実行完了時のパラメータ
	struct OnScriptCompletedOpt
	{
		//実行結果(JSONフォーマット)
		WString Message;
	};

};

class mWebView2 : public mWindow 
{
public:

	//通知時のパラメータ
	union NotifyFunctionOptPtr
	{
		//WebView2の初期化が完了した場合のイベントです
		Definitions_WebView2::OnNavigationStartingOpt* OnNavigationStarting;
		//ページ遷移が完了した場合のイベントです
		Definitions_WebView2::OnNavigationCompletedOpt* OnNavigationCompleted;
		//InjectOnCreateScript()で強制実行するスクリプトを注入している場合、そのスクリプトの実行が完了するとコールバックされるイベントです。
		Definitions_WebView2::OnCreateScriptCompletedOpt* OnCreateScriptCompleted;
		//ウェブメッセージを受信した場合のイベントです
		Definitions_WebView2::OnWebMessageRecievedOpt* OnWebMessageRecievedOpt;
		//ExceScript()で実行したスクリプトが完了した場合のイベントです
		Definitions_WebView2::OnScriptCompletedOpt* OnScriptCompletedOpt;
	};

	//通知設定
	using NotifyFunction = void(*)( mWebView2& webview , DWORD_PTR parameter , NotifyFunctionOptPtr* opt );
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		//WebView2の初期化が完了した場合のイベントです
		NotifierInfo OnInitialized;
		//ページ遷移を開始した場合のイベントです
		//・リダイレクトされた場合は、複数回連続で発生することがあります
		NotifierInfo OnNavigationStarting;
		//ページ遷移が完了した場合のイベントです
		//・失敗の場合もあります
		NotifierInfo OnNavigationCompleted;
		//InjectOnCreateScript()で強制実行するスクリプトを注入している場合、
		//そのスクリプトの実行が完了するとコールバックされるイベントです。
		NotifierInfo OnCreateScriptCompleted;
		//ウェブメッセージを受信した場合のイベントです
		// Javascript側から「window.chrome.webview.postMessage("MessageFromJavascript");」みたいな感じでメッセージを送るとそれを受信できます。
		NotifierInfo OnWebMessageReceived;
		//ExceScript()で実行したスクリプトが完了した場合のイベントです
		NotifierInfo OnScriptCompleted;
	};

	//コントロール生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
	public:
		//生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。

		//通知オプション
		NotifyOption Notify;

		virtual ~Option()
		{
			return;
		}
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//コントロール生成時のオプション
	struct Option_UseOption : public Option
	{
	public:
		//表示位置
		WindowPosition Pos;

		//ユーザーデータを配置するパス
		WString UserDataFolder;	

		//ユーザーエージェントの文字列の扱い
		//trueの場合、UserAgentを元のUserAgentの末尾に付け足します
		//falseの場合、UserAgentの値で元のUserAgentを置き換えます
		bool IsAppendUserAgent;

		//ユーザーエージェントの文字列。空文字列の場合はEdgeのデフォルトのUA。
		WString UserAgent;

		//初期化完了時に開くのは、URLかHTMLか
		//trueの場合、OnCreateNavigationはURL
		//falseの場合、OnCreateNavigationはHTML
		bool IsNavigationUrl;

		//初期化完了時に開くURLまたはHTML。空文字列の場合はなにもしない。
		WString OnCreateNavigation;

		//右クリックメニュー有効・無効スイッチ
		//falseにすると、コンテクストメニューが無効になる
		bool IsEnableContextMenu;

		//開発ツールを有効・無効スイッチ
		//falseにすると、開発ツールが無効になる
		bool IsEnableDevTool;

		//ステータスバー有効・無効スイッチ
		//falseにすると、ステータスバーは表示されない。
		bool IsEnableStatusBar;

		//スクリプト有効・無効スイッチ
		//falseにすると、Javascriptは動作しない。ただし、ExecScript()で直接実行した場合は動作する。
		bool IsEnableScript;

		//ズーム有効・無効スイッチ
		//falseにすると、ユーザーによるズームの調整は無効
		bool IsEnableZoom;

		//ビルトインエラーページの有効・無効スイッチ
		//falseにすると、ビルトインエラーページは無効
		bool IsEnableBuiltinErrorPage;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			IsAppendUserAgent = true;
			IsNavigationUrl = true;
			IsEnableContextMenu = true;
			IsEnableDevTool = false;
			IsEnableStatusBar = true;
			IsEnableScript = true;
			IsEnableZoom = true;
			IsEnableBuiltinErrorPage = true;
		}
		virtual ~Option_UseOption()
		{
			return;
		}
	};

	//ウインドウの可視/不可視を変更する
	//newstate : trueで可視、falseで不可視
	//ret : 成功時true
	virtual bool SetVisible( bool newstate )override;

	//ウインドウの有効/無効を変更する
	//newstate : trueで有効、falseで無効
	//ret : 成功時true
	virtual bool SetEnable( bool newstate )override;

	//ウインドウの位置をスクリーン座標で得る
	//ret : スクリーン座標によるウインドウの位置(エラーの場合0,0,0,0)
	virtual RECT GetRect( void ) const override;

	//ウインドウのサイズを得る
	//ret : ウインドウのサイズ(エラーの場合0,0)
	virtual SIZE GetSize( void ) const override;

	//このウインドウのクライアント座標をスクリーン座標に変換する
	//client_pos : クライアント座標
	//ret : スクリーン座標(エラーの場合0,0)
	virtual POINT Client2Screen( const POINT& client_pos ) const override;

	//このウインドウのクライアント座標をスクリーン座標に変換する
	//x : クライアント座標(X座標)
	//x : クライアント座標(Y座標)
	//ret : スクリーン座標(エラーの場合0,0)
	virtual POINT Client2Screen( INT x , INT y ) const override;

	//スクリーン座標をこのウインドウのクライアント座標に変換する
	//client_pos : スクリーン座標
	//ret : クライアント座標(エラーの場合0,0)
	virtual POINT Screen2Client( const POINT& client_pos ) const override;

	//スクリーン座標をこのウインドウのクライアント座標に変換する
	//x : スクリーン座標(X座標)
	//x : スクリーン座標(Y座標)
	//ret : クライアント座標(エラーの場合0,0)
	virtual POINT Screen2Client( INT x , INT y ) const override;

	//フォーカスを設定する
	//ret : 成功時true
	virtual bool SetFocus( void )const override;

	//指定のアドレスにナビゲートする
	bool Navigate( const WString& uri );

	//文字列で指定したＨＴＭＬを表示する
	bool PutHTML( const WString& html );

	//リロードする
	bool Reload( void );

	//読み込みを中止する
	bool Stop( void );

	//[戻る]が使用可能か確認する
	bool IsBackAvailable( void );

	//[進む]が使用可能か確認する
	bool IsForwardAvailable( void );

	//[戻る]
	bool Back( void );

	//[進む]
	bool Forward( void );

	//ドキュメントのタイトルを取得する
	//ret : ドキュメントのタイトル。エラーの場合は空文字列。
	WString GetDocumentTitle( void );

	//現在のURIを取得する
	//ret : 現在のURI。エラーの場合は空文字列。
	WString GetURI( void );

	//ドキュメント読み込み時に強制実行するスクリプトを挿入する
	// scr : 注入するjavascript
	// ret : 成功時真
	bool InjectOnCreateScript( const WString& scr );

	//ドキュメント読み込み時に強制実行するスクリプトを除去する
	//※１回実行が完了し、コールバック関数から得られるＩＤを知らないと削除できない
	// id : 削除するスクリプトのID(コールバック関数のパラメータから得られる)
	// ret : 成功時真
	bool RemoveOnCreateScript( const WString& id );

	//ウェブメッセージを送信する（JSON形式）
	// message : 送信するメッセージ
	// ret : 成功時真
	//HTML側からはこんな感じでリスナーを登録すると、メッセージが届くようになる。
	//  window.chrome.webview.addEventListener('message', handler)
	//  window.chrome.webview.removeEventListener('message', handler)
	bool SendMessageAsJson( const WString& message );

	//ウェブメッセージを送信する（文字列形式）
	// message : 送信するメッセージ
	// ret : 成功時真
	//HTML側からはこんな感じでリスナーを登録すると、メッセージが届くようになる。
	//  window.chrome.webview.addEventListener('message', handler)
	//  window.chrome.webview.removeEventListener('message', handler)
	bool SendMessageAsString( const WString& message );

	//スクリプトを実行する
	// ret : 成功時真
	// scr : 実行するjavascript
	bool ExecScript( const WString& scr );

	//ズーム倍率を取得する
	// errorvalue : エラーの場合に返す値
	// ret : ズーム倍率。エラーの場合errorvalueに指定した値。
	double GetZoomFactor( double errorvalue = 1.0f );

	//ズーム倍率を設定する
	bool SetZoomFactor( double factor );

	//ユーザーエージェントを取得する
	// ret : 取得したUA
	WString GetUserAgant( void );

	//ユーザーエージェントを設定する
	// ua : 新しいua
	// ret : 成功時真
	bool SetUserAgant( const WString& ua );

public:

	//ファクトリメソッド
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mWebView2;
	}

protected:
	mWebView2();
	virtual ~mWebView2();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;
	
	//ウインドウ生成を完了した場合にコールされる
	virtual bool OnCreate( const void* opt )override;

	//ウインドウを移動する
	virtual void MoveWindowPosition( const RECT& pos )override;

	//WebView環境ハンドル
	using EnvironmentHandle = Microsoft::WRL::ComPtr<ICoreWebView2Environment>;
	EnvironmentHandle MyEnvironmentHandle;

	//WebViewコントローラーのハンドル
	using ControllerHandle = Microsoft::WRL::ComPtr<ICoreWebView2Controller>;
	ControllerHandle MyControllerHandle;

	//WebViewのハンドル
	using ViewHandle = Microsoft::WRL::ComPtr<ICoreWebView2>;
	ViewHandle MyViewHandle;

	//デリゲートの再構築
	//実行により以下のイベント定義で使用するデリゲートを再構築する
	void UpdateDelegate( void );
	//デリゲート定義＠ナビゲーションスタート
	EventRegistrationToken MyOnNavigationStartingToken;
	//デリゲート定義＠ナビゲーション完了
	EventRegistrationToken MyOnNavigationCompletedToken;
	//デリゲート定義＠ウェブメッセージ受信
	EventRegistrationToken MyOnWebMessageReceivedToken;

	//コールバック関数
	HRESULT OnCreateEnvComplete( HRESULT callback_result , ICoreWebView2Environment* env );
	HRESULT OnCreateCntComplete( HRESULT callback_result , ICoreWebView2Controller* cnt );
	HRESULT OnNavigationStarting( ICoreWebView2* sender , ICoreWebView2NavigationStartingEventArgs* args );
	HRESULT OnNavigationCompleted( ICoreWebView2* sender , ICoreWebView2NavigationCompletedEventArgs* args );
	HRESULT OnCreateScriptCompleted( HRESULT callback_result , LPCWSTR id );
	HRESULT OnWebMessageReceived( ICoreWebView2* sender , ICoreWebView2WebMessageReceivedEventArgs* args );
	HRESULT OnScriptCompleted( HRESULT callback_result , LPCWSTR response );

private:

	mWebView2( const mWebView2& src ) = delete;
	mWebView2& operator=( const mWebView2& src ) = delete;

	//オプションデータを一時保存する
	std::unique_ptr<Option> MyOption;

};

#endif
#endif
