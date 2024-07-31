//----------------------------------------------------------------------------
// WebView2�n���h���[
// Copyright (C) 2021 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#define MWEBVIWEW2_CPP_COMPILING
#include "mWebView2.h"
#ifndef LIBRARY_ENABLE_WEBVIEW2
// WebView2�̓p�b�P�[�W���C���X�g�[�����Ȃ��Ɨ��p�ł��܂���
// NuGet�p�b�P�[�W�}�l�[�W���[���Microsoft.Web.WebView2���C���X�g�[�����Ă��������B
// �܂��A$(SolutionDir)libconfig.conf��ҏW�܂��͍쐬����WebView2���g�p����悤�ɕύX����K�v������܂��B
// ���s�ɂ̓����^�C�����C�u�������C���X�g�[������K�v������܂�(��Win10�ȉ��̏ꍇ)�B
#pragma message("   *Notice* : WebView2 is disabled")
#else
#include <General/mErrorLogger.h>

//�C�x���g�ʒm�֐�
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
	//�E�C���h�E�N���X�̓o�^���s��Ȃ����߂��̂܂�false�Ŗ߂�
	return false;
}

bool mWebView2::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	//�E�C���h�E�̐��������͂ōs�����߂��̂܂�false�Ŗ߂�
	return false;
}

bool mWebView2::OnCreate( const void* opt )
{
	WString user_data_folder;	//���[�U�[�f�[�^�t�H���_

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
			RaiseError( g_ErrorLogger , 0 , L"WebView2�̏������f�[�^�`�����Ⴂ�܂�" );
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
		RaiseError( g_ErrorLogger , result , L"WebView2�̏����������s���܂���" );
		MyOption.reset();
		return false;
	}
	return true;
}

HRESULT mWebView2::OnCreateEnvComplete( HRESULT callback_result , ICoreWebView2Environment* env )
{
	if( !env )
	{
		RaiseError( g_ErrorLogger , callback_result , L"���n���h�����s���ł�" );
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

	//�R���g���[���[�̃n���h����ۑ�
	if( !cnt )
	{
		RaiseError( g_ErrorLogger , callback_result , L"�R���g���[���[�̃n���h�����s���ł�" );
		MyOption.reset();
		return E_FAIL;
	}
	MyControllerHandle = cnt;

	//�r���[�̃n���h����ۑ�
	HRESULT hr = MyControllerHandle->get_CoreWebView2( &MyViewHandle );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�R���g���[���[�̃n���h�����s���ł�" );
		MyOption.reset();
		return E_FAIL;
	}

	//�r���[�̐ݒ�
	ICoreWebView2Settings2* settings;
	MyViewHandle->get_Settings( (ICoreWebView2Settings**)&settings );

	//�W���̃X�N���v�g�̃_�C�A���O���g�p���邩
	settings->put_AreDefaultScriptDialogsEnabled( true );
	//webmessage��L���ɂ��邩
	settings->put_IsWebMessageEnabled( true );

	//�I�v�V�����̎w�肪����΁A���̎�ނɂ�萶�����@�𕪊�(�������R�[���o�b�N�O)
	if( MyOption->method == Option::CreateMethod::USEOPTION )
	{
		const mWebView2::Option_UseOption* op = ( const mWebView2::Option_UseOption* )MyOption.get();
		//�ʒu�ƃT�C�Y
		SetWindowPosition( op->Pos );
		//���[�U�[�G�[�W�F���g
		if( op->UserAgent != L"" )
		{
			if( op->IsAppendUserAgent )
			{
				//����UA�ɕt�������ꍇ
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
				//����UA��u��������ꍇ
				settings->put_UserAgent( op->UserAgent.c_str() );
			}
		}
		//�E�N���b�N���j���[�L���E�����X�C�b�`
		settings->put_AreDefaultContextMenusEnabled( op->IsEnableContextMenu );
		//�J���c�[����L���E�����X�C�b�`
		settings->put_AreDevToolsEnabled( op->IsEnableDevTool );
		//�X�e�[�^�X�o�[�L���E�����X�C�b�`
		settings->put_IsStatusBarEnabled( op->IsEnableStatusBar );
		//�X�N���v�g�L���E�����X�C�b�`
		settings->put_IsScriptEnabled( op->IsEnableScript );
		//�Y�[���L���E�����X�C�b�`
		settings->put_IsZoomControlEnabled( op->IsEnableZoom );
		//�r���g�C���G���[�y�[�W�̗L���E�����X�C�b�`
		settings->put_IsBuiltInErrorPageEnabled( op->IsEnableBuiltinErrorPage );
	}

	//�f���Q�[�g�̍č\�z
	UpdateDelegate();

	//�����������ʒm
	AsyncEvent( *this , MyOption->Notify.OnInitialized , nullptr ); 

	//�I�v�V�����̎w�肪����΁A���̎�ނɂ�萶�����@�𕪊�(�������R�[���o�b�N��)
	if( MyOption->method == Option::CreateMethod::USEOPTION )
	{
		const mWebView2::Option_UseOption* op = ( const mWebView2::Option_UseOption* )MyOption.get();
		//�����y�[�W
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
		RaiseAssert( g_ErrorLogger , 0 , L"�񓯊�����̊����ʒm���@���s���ł�" , info.Mode );
	}
}

void mWebView2::UpdateDelegate( void )
{
	//�i�r�Q�[�V�����X�^�[�g���̃f���Q�[�g
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
	//�i�r�Q�[�V�����������̃f���Q�[�g
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
	//�E�F�u���b�Z�[�W��M���̃f���Q�[�g
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
		//�u�[���l�̃p�����[�^�擾
		{
			BOOL boolval;
			//���_�C���N�g���ǂ���
			args->get_IsRedirected( &boolval );
			opt.IsRedirected = boolval;
			//���[�U�[�J�n���ǂ���
			args->get_IsUserInitiated( &boolval );
			opt.IsUserInitiated = boolval;
		}
		//������̃p�����[�^�擾
		{
			LPWSTR str = nullptr;
			//�J�n����URI
			args->get_Uri( &str );
			if( str )
			{
				opt.Uri = str;
			}
			CoTaskMemFree( str );
		}
		//�Œ�l�̃p�����[�^�ݒ�
		{
			//�������t���O
			opt.IsAllowed = true;
		}
		//�C�x���g�̌Ăяo��
		NotifyFunctionOptPtr optptr;
		optptr.OnNavigationStarting = &opt;
		AsyncEvent( *this , MyOption->Notify.OnNavigationStarting , &optptr );

		//�������t���O���ύX����Ă���ꍇ�ɏ���
		if( !opt.IsAllowed )
		{
			args->put_Cancel( true );
		}
	}
	else
	{
		//�C�x���g�̌Ăяo��
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
		//�u�[���l�̃p�����[�^�擾
		{
			BOOL boolval;
			//�������ǂ���
			args->get_IsSuccess( &boolval );
			opt.IsSuccess = boolval;
		}
		//�X�e�[�^�X�R�[�h
		{
			COREWEBVIEW2_WEB_ERROR_STATUS code;
			args->get_WebErrorStatus( &code );
			opt.Status = code;
		}
		//�C�x���g�̌Ăяo��
		NotifyFunctionOptPtr optptr;
		optptr.OnNavigationCompleted = &opt;
		AsyncEvent( *this , MyOption->Notify.OnNavigationCompleted , &optptr );
	}
	else
	{
		//�C�x���g�̌Ăяo��
		AsyncEvent( *this , MyOption->Notify.OnNavigationCompleted , nullptr );
	}
	return S_OK;
}

bool mWebView2::Navigate( const WString& uri )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	HRESULT hr = MyViewHandle->Navigate( uri.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�i�r�Q�[�g�����s���܂���" , uri );
		return false;
	}
	return true;
}

bool mWebView2::PutHTML( const WString& html )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	HRESULT hr = MyViewHandle->NavigateToString( html.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�i�r�Q�[�g�����s���܂���" );
		return false;
	}
	return true;
}

bool mWebView2::Reload( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	HRESULT hr = MyViewHandle->Reload();
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�����[�h�����s���܂���" );
		return false;
	}
	return true;
}

bool mWebView2::Stop( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	HRESULT hr = MyViewHandle->Stop();
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�����[�h�����s���܂���" );
		return false;
	}
	return true;
}

//[�߂�]���g�p�\���m�F����
bool mWebView2::IsBackAvailable( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	BOOL result;
	HRESULT hr = MyViewHandle->get_CanGoBack( &result );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"��Ԏ擾�����s���܂���" );
		return false;
	}
	return result;
}

//[�i��]���g�p�\���m�F����
bool mWebView2::IsForwardAvailable( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	BOOL result;
	HRESULT hr = MyViewHandle->get_CanGoForward( &result );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"��Ԏ擾�����s���܂���" );
		return false;
	}
	return result;
}

//[�߂�]
bool mWebView2::Back( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	HRESULT hr = MyViewHandle->GoBack();
	if( FAILED( hr ) )
	{
		//���ʂɑz�肳���̂ŃG���[���L�^���Ȃ�
		return false;
	}
	return true;
}

//[�i��]
bool mWebView2::Forward( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}
	HRESULT hr = MyViewHandle->GoForward();
	if( FAILED( hr ) )
	{
		//���ʂɑz�肳���̂ŃG���[���L�^���Ȃ�
		return false;
	}
	return true;
}

WString mWebView2::GetDocumentTitle( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
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
		//�C�x���g�̌Ăяo��
		NotifyFunctionOptPtr optptr;
		optptr.OnCreateScriptCompleted = &opt;
		AsyncEvent( *this , MyOption->Notify.OnCreateScriptCompleted , &optptr );
	}
	else
	{
		//�C�x���g�̌Ăяo��
		AsyncEvent( *this , MyOption->Notify.OnCreateScriptCompleted , nullptr );
	}
	return S_OK;
}

bool mWebView2::RemoveOnCreateScript( const WString& id )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
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
		//�C�x���g�̌Ăяo��
		NotifyFunctionOptPtr optptr;
		optptr.OnWebMessageRecievedOpt = &opt;
		AsyncEvent( *this , MyOption->Notify.OnWebMessageReceived , &optptr );
	}
	else
	{
		//�C�x���g�̌Ăяo��
		AsyncEvent( *this , MyOption->Notify.OnWebMessageReceived , nullptr );
	}
	return S_OK;
}

//�E�F�u���b�Z�[�W�𑗐M����iJSON�`���j
bool mWebView2::SendMessageAsJson( const WString& message )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return L"";
	}

	HRESULT hr = MyViewHandle->PostWebMessageAsJson( message.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�E�F�u���b�Z�[�W���M�����s���܂���" );
		return L"";
	}
	return true;
}

//�E�F�u���b�Z�[�W�𑗐M����i������`���j
bool mWebView2::SendMessageAsString( const WString& message )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return L"";
	}

	HRESULT hr = MyViewHandle->PostWebMessageAsString( message.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�E�F�u���b�Z�[�W���M�����s���܂���" );
		return L"";
	}
	return true;
}

bool mWebView2::ExecScript( const WString& scr )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}

	HRESULT hr = MyViewHandle->ExecuteScript( scr.c_str() ,
		Microsoft::WRL::Callback< ICoreWebView2ExecuteScriptCompletedHandler >( this , &mWebView2::OnScriptCompleted ).Get()
	);

	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�X�N���v�g�����s�ł��܂���" );
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
		//�C�x���g�̌Ăяo��
		NotifyFunctionOptPtr optptr;
		optptr.OnScriptCompletedOpt = &opt;
		AsyncEvent( *this , MyOption->Notify.OnScriptCompleted , &optptr );
	}
	else
	{
		//�C�x���g�̌Ăяo��
		AsyncEvent( *this , MyOption->Notify.OnScriptCompleted , nullptr );
	}
	return S_OK;
}

double mWebView2::GetZoomFactor( double errorvalue )
{
	if( !MyControllerHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�R���g���[���[������������Ă��܂���" );
		return errorvalue;
	}

	double result;
	HRESULT hr = MyControllerHandle->get_ZoomFactor( &result );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�\���{�����擾�ł��܂���" );
		return errorvalue;
	}
	return result;
}

bool mWebView2::SetZoomFactor( double factor )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�R���g���[���[������������Ă��܂���" );
		return false;
	}

	HRESULT hr = MyControllerHandle->put_ZoomFactor( factor );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"�\���{����ύX�ł��܂���" );
		return false;
	}
	return true;
}

WString mWebView2::GetUserAgant( void )
{
	if( !MyViewHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
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
		RaiseError( g_ErrorLogger , 0 , L"�r���[������������Ă��܂���" );
		return false;
	}

	ICoreWebView2Settings2* settings;
	MyViewHandle->get_Settings( (ICoreWebView2Settings**)&settings );

	HRESULT hr = settings->put_UserAgent( ua.c_str() );
	if( FAILED( hr ) )
	{
		RaiseError( g_ErrorLogger , hr , L"���[�U�[�G�[�W�F���g��ύX�ł��܂���" );
		return false;
	}
	return true;
}


#endif
