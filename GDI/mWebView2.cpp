//----------------------------------------------------------------------------
// WebView2ハンドラー
// Copyright (C) 2021 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#define MWEBVIWEW2_CPP_COMPILING
#include "mWebView2.h"
#ifndef LIBRARY_ENABLE_WEBVIEW2
// WebView2はパッケージをインストールしないと利用できません
// NuGetパッケージマネージャーよりMicrosoft.Web.WebView2をインストールしてください。
// また、$(SolutionDir)libconfig.confを編集または作成してWebView2を使用するように変更する必要があります。
// 実行にはランタイムライブラリをインストールする必要があります(※Win10以下の場合)。
#pragma message("   *Notice* : WebView2 is disabled")
#else
#include <General/mErrorLogger.h>

//イベント通知関数
static void AsyncEvent( mWebView2& view , const mWebView2::NotifyOption::NotifierInfo& info , mWebView2::NotifyFunctionOptPtr* opt );

mWebView2::mWebView2()
{
	MyOnNavigationStartingToken = { 0 };
	MyOnNavigationCompletedToken = { 0 };
	MyOnWebMessageReceivedToken = { 0 };
	return;
}

mWebView2::~mWebView2()
{
	return;
}

bool mWebView2::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	//ウインドウクラスの登録を行わないためそのままfalseで戻る
	return false;
}

bool mWebView2::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	//ウインドウの生成を自力で行うためそのままfalseで戻る
	return false;
}

bool mWebView2::OnCreate( const void* opt )
{
	WString user_data_folder;	//ユーザーデータフォルダ

	if( opt )
	{
		switch( ((const Option*)opt)->method )
		{
		case Option::CreateMethod::USEOPTION:
		{
			const Option_UseOption* op = (const Option_UseOption*)opt;
			MyOption.reset( mNew Option_UseOption( *op ) );
			user_data_folder = op->UserDataFolder;
			break;
		}
		default:
			RaiseError( g_ErrorLogger , 0 , L"WebView2の初期化データ形式が違います" );
			MyOption.reset( mNew Option_UseOption );
			break;
		}
	}
	else
	{
		MyOption.reset( mNew Option_UseOption );
	}

	using Handler = Microsoft::WRL::ComPtr<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>;
	Handler handler = Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>( this , &mWebView2::OnCreateEnvComplete );

	HRESULT result = CreateCoreWebView2EnvironmentWithOptions( nullptr , user_data_folder.c_str() , nullptr , handler.Get() );
	if( FAILED( result ) )
	{
		RaiseError( g_ErrorLogger , result , L"WebView2の初期化が失敗しました" );
		MyOption.reset();
		return false;
	}
	return true;
}

HRESULT mWebView2::OnCreateEnvComplete( HRESULT callback_result , ICoreWebView2Environment* env )
{
	if( !env )
	{
		RaiseError( g_ErrorLogger , callback_result , L"環境ハンドルが不正です" );
		MyOption.reset();
		return E_FAIL;
	}
	MyEnvironmentHandle = env;


	using Handler = Microsoft::WRL::ComPtr<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>;
	Handler handler = Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>( this , &mWebView2::OnCreateCntComplete );

	HRESULT result = MyEnvironmentHandle->CreateCoreWebView2Controller( GetMyParent() , handler.Get() );
	return S_OK;
}

HRESULT mWebView2::OnCreateCntComplete( HRESULT callback_result , ICoreWebView2Controller* cnt )
{

	//コントローラーのハンドルを保存
	if( !cnt )
	{
		RaiseError( g_ErrorLogger , callback_result , L"コントローラーのハンドルが不正です" );
		MyOption.reset();
		return E_FAIL;
	}
	MyControllerHandle = cnt;

	//ビューのハンドルを保存
	HRESULT hr = MyControllerHandle->get_CoreWebView2( &MyViewHandle );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"コントローラーのハンドルが不正です" );
		MyOption.reset();
		return E_FAIL;
	}

	//ビューの設定
	ICoreWebView2Settings2* settings;
	MyViewHandle->get_Settings( (ICoreWebView2Settings**)&settings );

	//標準のスクリプトのダイアログを使用するか
	settings->put_AreDefaultScriptDialogsEnabled( true );
	//webmessageを有効にするか
	settings->put_IsWebMessageEnabled( true );

	//オプションの指定があれば、その種類により生成方法を分岐(初期化コールバック前)
	if( MyOption->method == Option::CreateMethod::USEOPTION )
	{
		const mWebView2::Option_UseOption* op = ( const mWebView2::Option_UseOption* )MyOption.get();
		//位置とサイズ
		SetWindowPosition( op->Pos );
		//ユーザーエージェント
		if( op->UserAgent != L"" )
		{
			if( op->IsAppendUserAgent )
			{
				//元のUAに付け足す場合
				WString ua = L"";
				LPWSTR str = nullptr;
				if( SUCCEEDED( settings->get_UserAgent( &str ) ) )
				{
					ua = str;
				}
				CoTaskMemFree( str );

				ua += op->UserAgent;
				settings->put_UserAgent( ua.c_str() );
			}
			else
			{
				//元のUAを置き換える場合
				settings->put_UserAgent( op->UserAgent.c_str() );
			}
		}
		//右クリックメニュー有効・無効スイッチ
		settings->put_AreDefaultContextMenusEnabled( op->IsEnableContextMenu );
		//開発ツールを有効・無効スイッチ
		settings->put_AreDevToolsEnabled( op->IsEnableDevTool );
		//ステータスバー有効・無効スイッチ
		settings->put_IsStatusBarEnabled( op->IsEnableStatusBar );
		//スクリプト有効・無効スイッチ
		settings->put_IsScriptEnabled( op->IsEnableScript );
		//ズーム有効・無効スイッチ
		settings->put_IsZoomControlEnabled( op->IsEnableZoom );
		//ビルトインエラーページの有効・無効スイッチ
		settings->put_IsBuiltInErrorPageEnabled( op->IsEnableBuiltinErrorPage );
	}

	//デリゲートの再構築
	UpdateDelegate();

	//初期化完了通知
	AsyncEvent( *this , MyOption->Notify.OnInitialized , nullptr ); 

	//オプションの指定があれば、その種類により生成方法を分岐(初期化コールバック後)
	if( MyOption->method == Option::CreateMethod::USEOPTION )
	{
		const mWebView2::Option_UseOption* op = ( const mWebView2::Option_UseOption* )MyOption.get();
		//初期ページ
		if( op->OnCreateNavigation != L"" )
		{
			if( op->IsNavigationUrl )
			{
				Navigate( op->OnCreateNavigation );
			}
			else
			{
				PutHTML( op->OnCreateNavigation );
			}
		}
	}
	return S_OK;
}

void mWebView2::MoveWindowPosition( const RECT& pos )
{
	if( !MyControllerHandle )
	{
		return;
	}
	MyControllerHandle->put_Bounds( pos );
}

bool mWebView2::SetVisible( bool newstate )
{
	if( !MyControllerHandle )
	{
		return false;
	}
	if( SUCCEEDED( MyControllerHandle->put_IsVisible( newstate ) ) )
	{
		return true;
	}
	return false;
}
bool mWebView2::SetEnable( bool newstate )
{
	return false;
}
RECT mWebView2::GetRect( void ) const
{
	RECT result;
	if( !MyControllerHandle || FAILED( MyControllerHandle->get_Bounds( &result ) ) )
	{
		result.top = 0;
		result.bottom = 0;
		result.left = 0;
		result.right = 0;
	}
	return result;
}
SIZE mWebView2::GetSize( void ) const
{
	RECT rect = GetRect();
	SIZE size;
	size.cx = rect.right - rect.left;
	size.cy = rect.bottom - rect.top;
	return size;
}
POINT mWebView2::Client2Screen( const POINT& client_pos ) const
{
	POINT result;
	result.x = 0;
	result.y = 0;
	return result;
}
POINT mWebView2::Client2Screen( INT x , INT y ) const
{
	POINT result;
	result.x = 0;
	result.y = 0;
	return result;
}
POINT mWebView2::Screen2Client( const POINT& client_pos ) const
{
	POINT result;
	result.x = 0;
	result.y = 0;
	return result;
}
POINT mWebView2::Screen2Client( INT x , INT y ) const
{
	POINT result;
	result.x = 0;
	result.y = 0;
	return result;
}
bool mWebView2::SetFocus( void )const
{
	if( !MyControllerHandle  )
	{
		return false;
	}
	if( SUCCEEDED( MyControllerHandle->MoveFocus( COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC ) ) )
	{
		return true;
	}
	return false;
}

static void AsyncEvent( mWebView2& view , const mWebView2::NotifyOption::NotifierInfo& info , mWebView2::NotifyFunctionOptPtr* opt )
{
	if( info.Mode == mWebView2::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		//do nothing
	}
	else if( info.Mode == mWebView2::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
	{
		::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&view , info.Parameter );
	}
	else if( info.Mode == mWebView2::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
	{
		if( info.Notifier.CallbackFunction )
		{
			while( mWebView2::NotifyOption::EnterNotifyEvent( info ) )
			{
				info.Notifier.CallbackFunction( view , info.Parameter , opt );
				if( !mWebView2::NotifyOption::LeaveNotifyEvent( info ) )
				{
					break;
				}
			}
		}
	}
	else if( info.Mode == mWebView2::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
	{
		info.Notifier.CallbackFunction( view , info.Parameter , opt );
	}
	else if( info.Mode == mWebView2::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
	{
		if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
		{
			SetEvent( info.Notifier.Handle );
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"非同期操作の完了通知方法が不正です" , info.Mode );
	}
}

void mWebView2::UpdateDelegate( void )
{
	//ナビゲーションスタート時のデリゲート
	if( MyOption && MyOption->Notify.OnNavigationStarting.Mode != mWebView2::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		if( !MyOnNavigationStartingToken.value )
		{
			MyViewHandle->add_NavigationStarting( 
				Microsoft::WRL::Callback< ICoreWebView2NavigationStartingEventHandler >( this , &mWebView2::OnNavigationStarting ).Get() ,
				&MyOnNavigationStartingToken );
		}
	}
	else
	{
		if( MyOnNavigationStartingToken.value )
		{
			MyViewHandle->remove_NavigationStarting( MyOnNavigationStartingToken );
			MyOnNavigationStartingToken = { 0 };
		}
	}
	//ナビゲーション完了時のデリゲート
	if( MyOption && MyOption->Notify.OnNavigationCompleted.Mode != mWebView2::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		if( !MyOnNavigationCompletedToken.value )
		{
			MyViewHandle->add_NavigationCompleted( 
				Microsoft::WRL::Callback< ICoreWebView2NavigationCompletedEventHandler >( this , &mWebView2::OnNavigationCompleted ).Get() ,
				&MyOnNavigationCompletedToken );
		}
	}
	else
	{
		if( MyOnNavigationCompletedToken.value )
		{
			MyViewHandle->remove_NavigationStarting( MyOnNavigationCompletedToken );
			MyOnNavigationCompletedToken = { 0 };
		}
	}
	//ウェブメッセージ受信時のデリゲート
	if( MyOption && MyOption->Notify.OnWebMessageReceived.Mode != mWebView2::NotifyOption::NotifyMode::NOTIFY_NONE )
	{
		if( !MyOnWebMessageReceivedToken.value )
		{
			MyViewHandle->add_WebMessageReceived( 
				Microsoft::WRL::Callback< ICoreWebView2WebMessageReceivedEventHandler >( this , &mWebView2::OnWebMessageReceived ).Get() ,
				&MyOnWebMessageReceivedToken );
		}
	}
	else
	{
		if( MyOnWebMessageReceivedToken.value )
		{
			MyViewHandle->remove_WebMessageReceived( MyOnWebMessageReceivedToken );
			MyOnWebMessageReceivedToken = { 0 };
		}
	}
}

HRESULT mWebView2::OnNavigationStarting( ICoreWebView2* sender , ICoreWebView2NavigationStartingEventArgs* args )
{
	if( !MyOption )
	{
		return S_OK;
	}
	else if( mWebView2::NotifyOption::IsCallback( MyOption->Notify.OnNavigationStarting.Mode ) )
	{
		Definitions_WebView2::OnNavigationStartingOpt opt;
		//ブール値のパラメータ取得
		{
			BOOL boolval;
			//リダイレクトかどうか
			args->get_IsRedirected( &boolval );
			opt.IsRedirected = boolval;
			//ユーザー開始かどうか
			args->get_IsUserInitiated( &boolval );
			opt.IsUserInitiated = boolval;
		}
		//文字列のパラメータ取得
		{
			LPWSTR str = nullptr;
			//開始するURI
			args->get_Uri( &str );
			if( str )
			{
				opt.Uri = str;
			}
			CoTaskMemFree( str );
		}
		//固定値のパラメータ設定
		{
			//処理許可フラグ
			opt.IsAllowed = true;
		}
		//イベントの呼び出し
		NotifyFunctionOptPtr optptr;
		optptr.OnNavigationStarting = &opt;
		AsyncEvent( *this , MyOption->Notify.OnNavigationStarting , &optptr );

		//処理許可フラグが変更されている場合に処理
		if( !opt.IsAllowed )
		{
			args->put_Cancel( true );
		}
	}
	else
	{
		//イベントの呼び出し
		AsyncEvent( *this , MyOption->Notify.OnNavigationStarting , nullptr );
	}
	return S_OK;
}

HRESULT mWebView2::OnNavigationCompleted( ICoreWebView2* sender , ICoreWebView2NavigationCompletedEventArgs* args )
{
	if( !MyOption )
	{
		return S_OK;
	}
	else if( mWebView2::NotifyOption::IsCallback( MyOption->Notify.OnNavigationCompleted.Mode ) )
	{
		Definitions_WebView2::OnNavigationCompletedOpt opt;
		//ブール値のパラメータ取得
		{
			BOOL boolval;
			//成功かどうか
			args->get_IsSuccess( &boolval );
			opt.IsSuccess = boolval;
		}
		//ステータスコード
		{
			COREWEBVIEW2_WEB_ERROR_STATUS code;
			args->get_WebErrorStatus( &code );
			opt.Status = code;
		}
		//イベントの呼び出し
		NotifyFunctionOptPtr optptr;
		optptr.OnNavigationCompleted = &opt;
		AsyncEvent( *this , MyOption->Notify.OnNavigationCompleted , &optptr );
	}
	else
	{
		//イベントの呼び出し
		AsyncEvent( *this , MyOption->Notify.OnNavigationCompleted , nullptr );
	}
	return S_OK;
}

bool mWebView2::Navigate( const WString& uri )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	HRESULT hr = MyViewHandle->Navigate( uri.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"ナビゲートが失敗しました" , uri );
		return false;
	}
	return true;
}

bool mWebView2::PutHTML( const WString& html )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	HRESULT hr = MyViewHandle->NavigateToString( html.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"ナビゲートが失敗しました" );
		return false;
	}
	return true;
}

bool mWebView2::Reload( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	HRESULT hr = MyViewHandle->Reload();
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"リロードが失敗しました" );
		return false;
	}
	return true;
}

bool mWebView2::Stop( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	HRESULT hr = MyViewHandle->Stop();
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"リロードが失敗しました" );
		return false;
	}
	return true;
}

//[戻る]が使用可能か確認する
bool mWebView2::IsBackAvailable( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	BOOL result;
	HRESULT hr = MyViewHandle->get_CanGoBack( &result );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"状態取得が失敗しました" );
		return false;
	}
	return result;
}

//[進む]が使用可能か確認する
bool mWebView2::IsForwardAvailable( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	BOOL result;
	HRESULT hr = MyViewHandle->get_CanGoForward( &result );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"状態取得が失敗しました" );
		return false;
	}
	return result;
}

//[戻る]
bool mWebView2::Back( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	HRESULT hr = MyViewHandle->GoBack();
	if( FAILED( hr ) )
	{
		//普通に想定されるのでエラーを記録しない
		return false;
	}
	return true;
}

//[進む]
bool mWebView2::Forward( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}
	HRESULT hr = MyViewHandle->GoForward();
	if( FAILED( hr ) )
	{
		//普通に想定されるのでエラーを記録しない
		return false;
	}
	return true;
}

WString mWebView2::GetDocumentTitle( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return L"";
	}

	LPWSTR str = nullptr;
	HRESULT hr = MyViewHandle->get_DocumentTitle( &str );
	WString result;
	if( SUCCEEDED( hr ) )
	{
		result = str;
	}

	CoTaskMemFree( str );
	return result;
}

WString mWebView2::GetURI( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return L"";
	}

	LPWSTR str = nullptr;
	HRESULT hr = MyViewHandle->get_Source( &str );
	WString result;
	if( SUCCEEDED( hr ) )
	{
		result = str;
	}

	CoTaskMemFree( str );
	return result;
}

bool mWebView2::InjectOnCreateScript( const WString& scr )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return L"";
	}

	HRESULT hr = MyViewHandle->AddScriptToExecuteOnDocumentCreated( scr.c_str() ,
		Microsoft::WRL::Callback< ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler >( this , &mWebView2::OnCreateScriptCompleted ).Get()
	);

	return SUCCEEDED( hr );
}

HRESULT mWebView2::OnCreateScriptCompleted( HRESULT callback_result , LPCWSTR id )
{
	if( !MyOption )
	{
		return S_OK;
	}
	else if( mWebView2::NotifyOption::IsCallback( MyOption->Notify.OnCreateScriptCompleted.Mode ) )
	{
		Definitions_WebView2::OnCreateScriptCompletedOpt opt;
		opt.IsSuccess = SUCCEEDED( callback_result );
		opt.ScriptId = id;
		//イベントの呼び出し
		NotifyFunctionOptPtr optptr;
		optptr.OnCreateScriptCompleted = &opt;
		AsyncEvent( *this , MyOption->Notify.OnCreateScriptCompleted , &optptr );
	}
	else
	{
		//イベントの呼び出し
		AsyncEvent( *this , MyOption->Notify.OnCreateScriptCompleted , nullptr );
	}
	return S_OK;
}

bool mWebView2::RemoveOnCreateScript( const WString& id )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}

	if( FAILED( MyViewHandle->RemoveScriptToExecuteOnDocumentCreated( id.c_str() ) ) )
	{
		return false;
	}
	return true;
}

HRESULT mWebView2::OnWebMessageReceived( ICoreWebView2* sender , ICoreWebView2WebMessageReceivedEventArgs* args )
{
	if( !MyOption )
	{
		return S_OK;
	}
	else if( mWebView2::NotifyOption::IsCallback( MyOption->Notify.OnWebMessageReceived.Mode ) )
	{
		Definitions_WebView2::OnWebMessageRecievedOpt opt;
		{
			LPWSTR str = nullptr;
			HRESULT hr = args->get_Source( &str );
			if( SUCCEEDED( hr ) )
			{
				opt.Sender = str;
			}
			CoTaskMemFree( str );
		}
		{
			LPWSTR str = nullptr;
			HRESULT hr = args->get_WebMessageAsJson( &str );
			if( SUCCEEDED( hr ) )
			{
				opt.Message = str;
			}
			CoTaskMemFree( str );
		}
		{
			LPWSTR str = nullptr;
			HRESULT hr = args->TryGetWebMessageAsString( &str );
			if( SUCCEEDED( hr ) )
			{
				opt.MessageAsString.reset( mNew WString( str ) );
			}
			CoTaskMemFree( str );
		}
		//イベントの呼び出し
		NotifyFunctionOptPtr optptr;
		optptr.OnWebMessageRecievedOpt = &opt;
		AsyncEvent( *this , MyOption->Notify.OnWebMessageReceived , &optptr );
	}
	else
	{
		//イベントの呼び出し
		AsyncEvent( *this , MyOption->Notify.OnWebMessageReceived , nullptr );
	}
	return S_OK;
}

//ウェブメッセージを送信する（JSON形式）
bool mWebView2::SendMessageAsJson( const WString& message )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return L"";
	}

	HRESULT hr = MyViewHandle->PostWebMessageAsJson( message.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ウェブメッセージ送信が失敗しました" );
		return L"";
	}
	return true;
}

//ウェブメッセージを送信する（文字列形式）
bool mWebView2::SendMessageAsString( const WString& message )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return L"";
	}

	HRESULT hr = MyViewHandle->PostWebMessageAsString( message.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"ウェブメッセージ送信が失敗しました" );
		return L"";
	}
	return true;
}

bool mWebView2::ExecScript( const WString& scr )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}

	HRESULT hr = MyViewHandle->ExecuteScript( scr.c_str() ,
		Microsoft::WRL::Callback< ICoreWebView2ExecuteScriptCompletedHandler >( this , &mWebView2::OnScriptCompleted ).Get()
	);

	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"スクリプトを実行できません" );
		return false;
	}
	return true;
}

HRESULT mWebView2::OnScriptCompleted( HRESULT callback_result , LPCWSTR response )
{

	if( !MyOption )
	{
		return S_OK;
	}
	else if( mWebView2::NotifyOption::IsCallback( MyOption->Notify.OnScriptCompleted.Mode ) )
	{
		Definitions_WebView2::OnScriptCompletedOpt opt;
		opt.Message = response;
		//イベントの呼び出し
		NotifyFunctionOptPtr optptr;
		optptr.OnScriptCompletedOpt = &opt;
		AsyncEvent( *this , MyOption->Notify.OnScriptCompleted , &optptr );
	}
	else
	{
		//イベントの呼び出し
		AsyncEvent( *this , MyOption->Notify.OnScriptCompleted , nullptr );
	}
	return S_OK;
}

double mWebView2::GetZoomFactor( double errorvalue )
{
	if( !MyControllerHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"コントローラーが初期化されていません" );
		return errorvalue;
	}

	double result;
	HRESULT hr = MyControllerHandle->get_ZoomFactor( &result );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"表示倍率を取得できません" );
		return errorvalue;
	}
	return result;
}

bool mWebView2::SetZoomFactor( double factor )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"コントローラーが初期化されていません" );
		return false;
	}

	HRESULT hr = MyControllerHandle->put_ZoomFactor( factor );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"表示倍率を変更できません" );
		return false;
	}
	return true;
}

WString mWebView2::GetUserAgant( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return L"";
	}

	ICoreWebView2Settings2* settings;
	MyViewHandle->get_Settings( (ICoreWebView2Settings**)&settings );

	LPWSTR str = nullptr;
	HRESULT hr = settings->get_UserAgent( &str );
	WString result;
	if( SUCCEEDED( hr ) )
	{
		result = str;
	}

	CoTaskMemFree( str );
	return result;
}

bool mWebView2::SetUserAgant( const WString& ua )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"ビューが初期化されていません" );
		return false;
	}

	ICoreWebView2Settings2* settings;
	MyViewHandle->get_Settings( (ICoreWebView2Settings**)&settings );

	HRESULT hr = settings->put_UserAgent( ua.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"ユーザーエージェントを変更できません" );
		return false;
	}
	return true;
}


#endif
