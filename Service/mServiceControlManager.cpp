//----------------------------------------------------------------------------
// サービスハンドラ
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mServiceControlManager.h"
#include "General/mErrorLogger.h"

//サービスの名前からハンドルを取得してコールバックを呼び出すサービスルーチン
//コールバック終了後はハンドルを解放する
// name : 操作を行いたいサービスの名前
// req_access_right : 付与したいアクセス権。コールバック内で行いたい操作に応じて設定する
// opt : ユーザー定義の値
// callback : 呼び出すコールバック
//		第1引数＝nameで指定したサービスのハンドル
//		第2引数＝optに指定したユーザー定義の値
//		ret＝成功時真
// ret : 成功時真
static bool HandlingServiceTemplate( const WString& name , DWORD req_access_right , DWORD_PTR opt , bool(*callback)(SC_HANDLE sv,DWORD_PTR opt) )
{
	SC_HANDLE sc = NULL;
	SC_HANDLE sv = NULL;
	BOOL result = false;

	sc = OpenSCManagerW( 0 , SERVICES_ACTIVE_DATABASE , 0 );
	if( !sc )
	{
		goto cleanup;
	}
	sv = OpenServiceW( sc , name.c_str() , req_access_right );
	if( !sv )
	{
		goto cleanup;
	}
	result = callback( sv , opt);

cleanup:
	if( sv )
	{
		CloseServiceHandle( sv );
	}
	if( sc )
	{
		CloseServiceHandle( sc );
	}
	return result;

}

bool mServiceControlManager::DeleteExistingService( const WString& ServiceName )
{
	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		return DeleteService( sv );
	};

	if( !HandlingServiceTemplate( ServiceName , DELETE , 0 , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスを削除できませんでした" , ServiceName );
		return false;
	}
	return true;
}

//既存のサービスを中断する
bool mServiceControlManager::PauseExistingService( const WString& ServiceName )
{
	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		SERVICE_CONTROL_STATUS_REASON_PARAMS param;
		ZeroMemory( &param , sizeof( param ) );
		return ControlServiceExW( sv , SERVICE_CONTROL_PAUSE , SERVICE_CONTROL_STATUS_REASON_INFO , &param );
	};

	if( !HandlingServiceTemplate( ServiceName , SERVICE_PAUSE_CONTINUE  , 0 , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスを中断できませんでした" , ServiceName );
		return false;
	}
	return true;
}

//既存のサービスを再開する
bool mServiceControlManager::ContinueExistingService( const WString& ServiceName )
{
	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		SERVICE_CONTROL_STATUS_REASON_PARAMS param;
		ZeroMemory( &param , sizeof( param ) );
		return ControlServiceExW( sv , SERVICE_CONTROL_CONTINUE , SERVICE_CONTROL_STATUS_REASON_INFO , &param );
	};

	if( !HandlingServiceTemplate( ServiceName , SERVICE_PAUSE_CONTINUE  , 0 , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスを再開できませんでした" , ServiceName );
		return false;
	}
	return true;
}

//既存のサービスにコントロールコードを送る
bool mServiceControlManager::ControlExistingService( const WString& ServiceName , DWORD code )
{
	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		SERVICE_CONTROL_STATUS_REASON_PARAMS param;
		ZeroMemory( &param , sizeof( param ) );
		return ControlServiceExW( sv , (DWORD)opt , SERVICE_CONTROL_STATUS_REASON_INFO , &param );
	};

	if( ( code < 128 ) || ( 255 < code ) )
	{
		RaiseErrorF( g_ErrorLogger , 0 , L"コントロールコードの値が範囲外です" , L"%d@%s" , code , ServiceName.c_str() );
		return false;
	}

	if( !HandlingServiceTemplate( ServiceName , SERVICE_USER_DEFINED_CONTROL , code , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスにコードを送信できませんでした" , ServiceName );
		return false;
	}
	return true;
}

//既存のサービスを開始する
bool mServiceControlManager::StartExistingService( const WString& ServiceName , const WStringDeque& args )
{
	//開始処理をするコールバック
	struct OptStruct
	{
		DWORD argc;
		LPCWSTR* argv;
	};

	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		OptStruct* args = (OptStruct*)opt;
		return StartServiceW( sv , args->argc , args->argv );
	};

	//引数の準備
	OptStruct opt;
	opt.argc = 0;
	opt.argv = mNew LPCWSTR[ args.size() ];

	for( WStringDeque::const_iterator itr = args.begin() ; itr != args.end() ; itr++ )
	{
		if( itr->size() && itr->at( 0 ) != '\0' )
		{
			opt.argv[ opt.argc ] = itr->data();
			opt.argc++;
		}
	}

	//開始
	bool result = HandlingServiceTemplate( ServiceName , SERVICE_START , (DWORD_PTR)&opt , Callback );
	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスの開始が失敗しました" , ServiceName );
	}

	mDelete[] opt.argv;
	return result;
}

//既存のサービスを停止する
bool mServiceControlManager::StopExistingService( const WString& ServiceName , bool is_planed , const WString& reason )
{
	//パラメータの準備
	if( 127 < reason.size() )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービス停止時の理由コードが長すぎます" , ServiceName );
		return false;
	}

	SERVICE_CONTROL_STATUS_REASON_PARAMSW param;
	ZeroMemory( &param , sizeof( param ) );
	param.dwReason = ( is_planed ) ? ( SERVICE_STOP_REASON_FLAG_PLANNED ) : ( SERVICE_STOP_REASON_FLAG_UNPLANNED );
	param.pszComment = ( !reason.empty() ) ? ( const_cast<LPWSTR>( reason.data() ) ) : ( nullptr );

	//コールバックを準備
	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		SERVICE_CONTROL_STATUS_REASON_PARAMSW* param = (SERVICE_CONTROL_STATUS_REASON_PARAMSW*)opt;
		return ControlServiceExW( sv , SERVICE_CONTROL_STOP , SERVICE_CONTROL_STATUS_REASON_INFO , param );
	};

	//テンプレートの呼び出し
	if( !HandlingServiceTemplate( ServiceName , SERVICE_STOP , (DWORD_PTR)&param , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスを再開できませんでした" , ServiceName );
		return false;
	}
	return true;
}


bool mServiceControlManager::CreateNewService( const mServiceControlManager::ServiceParam& param )
{
	SC_HANDLE sc = NULL;
	SC_HANDLE sv = NULL;
	BOOL result = false;
	WString UserName;
	WString Password;
	WString DependenciesString;
	DWORD ServiceType = 0;

	sc = OpenSCManagerW( 0 , SERVICES_ACTIVE_DATABASE , SC_MANAGER_CREATE_SERVICE );
	if( !sc )
	{
		RaiseError( g_ErrorLogger , 0 , L"SCMを開けません" );
		goto cleanup;
	}

	//プロセス種別とユーザー名、パスワード
	if( param.method == ServiceParam::ServiceProcessUser::LOGONUSER_PROCESS )
	{
		switch( param.ProcessType )
		{
		case ServiceParam::ServiceProcessType::SERVICE_OWN_PROCESS:
			ServiceType = SERVICE_USER_OWN_PROCESS;
			break;
		case ServiceParam::ServiceProcessType::SERVICE_SHARE_PROCESS:
			ServiceType = SERVICE_USER_SHARE_PROCESS;
			break;
		default:
			break;
		}
	}
	else if( param.method == ServiceParam::ServiceProcessUser::SYSTEM_PROCESS )
	{
		switch( param.ProcessType )
		{
		case ServiceParam::ServiceProcessType::SERVICE_OWN_PROCESS:
			ServiceType = SERVICE_WIN32_OWN_PROCESS;
			break;
		case ServiceParam::ServiceProcessType::SERVICE_SHARE_PROCESS:
			ServiceType = SERVICE_WIN32_SHARE_PROCESS;
			break;
		default:
			break;
		}
		const ServiceParam_System* param_sys = reinterpret_cast<const ServiceParam_System*>( &param );
		UserName = param_sys->UserName;
		Password = param_sys->Password;
	}
	if( ServiceType == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスプロセスの種類指定が間違っています" );
		goto cleanup;
	}

	//ユーザーとの対話許可？
	if( param.Interactive )
	{
		ServiceType |= SERVICE_INTERACTIVE_PROCESS;
	}

	//依存関係
	MakeDoubleNullString( param.Dependencies , DependenciesString );

	//サービス生成
	sv = CreateServiceW( sc ,
		param.ServiceName.c_str() ,
		param.DisplayName.c_str() ,
		SERVICE_CHANGE_CONFIG ,
		ServiceType ,
		param.StartType ,
		param.ErrorControl ,
		param.BinaryPathName.c_str() ,
		0 ,
		0 ,
		DependenciesString.c_str() ,
		UserName.c_str() ,
		Password.c_str() );
	if( !sv )
	{
		RaiseError( g_ErrorLogger , 0 , L"サービスの登録が失敗しました" );
		goto cleanup;
	}

	SERVICE_DESCRIPTIONW desc;
	desc.lpDescription = (LPWSTR)param.Description.c_str();
	result = ChangeServiceConfig2W( sv , SERVICE_CONFIG_DESCRIPTION , &desc );

cleanup:
	if( sv )
	{
		CloseServiceHandle( sv );
	}
	if( sc )
	{
		CloseServiceHandle( sc );
	}

	return result;
}

//ユーザー権限で動作しているサービスの、現セッションでのサービス名を調べる
bool mServiceControlManager::SearchUserProcessService( const WString& ServiceName , WString& retFound )
{
	retFound.clear();

	bool result = false;
	BYTE* buffer = nullptr;

	SC_HANDLE sc = NULL;
	sc = OpenSCManagerW( 0 , SERVICES_ACTIVE_DATABASE , SC_MANAGER_ENUMERATE_SERVICE );
	if( !sc )
	{
		RaiseError( g_ErrorLogger , 0 , L"SCMを開けません" );
	}


	DWORD my_session_id;								//自分のセッションID
	if( !ProcessIdToSessionId( GetCurrentProcessId() , &my_session_id ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"このプロセスのセッションIDを取得できません" );
	}
	WString search_service_name = ServiceName + L"_";	//サーチするサービス名
	bool continue_required;		//2周目があるか？　EnumServicesStatusExWはまとめて読み出すくせに、一度に全部読めないことがあるらしい。
	DWORD resume_handle = 0;	//読み出し位置のインデックス。

	do
	{
		//バッファの準備
		DWORD service_count = 0;	//読み取ったサービスのエントリ数
		DWORD req_size = 0;			//必要なバッファサイズ
		if( !EnumServicesStatusExW( sc , SC_ENUM_PROCESS_INFO , SERVICE_WIN32 , SERVICE_ACTIVE , 0 , 0 , &req_size , &service_count , &resume_handle , nullptr ) )
		{
			if( GetLastError() != ERROR_MORE_DATA )
			{
				goto cleanup;
			}
		}
		mDelete[] buffer;
		buffer = mNew BYTE[ req_size ];

		//データ取得
		SetLastError( 0 );
		if( !EnumServicesStatusExW( sc , SC_ENUM_PROCESS_INFO , SERVICE_WIN32 , SERVICE_ACTIVE , buffer , req_size , &req_size , &service_count , &resume_handle , nullptr ) )
		{
			if( GetLastError() == ERROR_MORE_DATA )
			{
				continue_required = true;
			}
			else
			{
				goto cleanup;
			}
		}
		else
		{
			continue_required = false;
		}

		//得られた結果を順にスキャンする
		ENUM_SERVICE_STATUS_PROCESS* stats = (ENUM_SERVICE_STATUS_PROCESS*)buffer;
		for( DWORD i = 0 ; i < service_count ; i++ )
		{
			//ユーザー権限で動作しているサービス？
			if( !( stats[ i ].ServiceStatusProcess.dwServiceType & SERVICE_USERSERVICE_INSTANCE ) )
			{
				//ユーザー権限で動作してないサービスだから無視して次
				continue;
			}

			//名前が一致？
			WString service_name = stats[ i ].lpServiceName;
			if( service_name.substr( 0 , search_service_name.size() ) != search_service_name )
			{
				//名前不一致なので次
				continue;
			}

			//セッションIDが一致？
			DWORD session_id;
			if( !ProcessIdToSessionId( stats[ i ].ServiceStatusProcess.dwProcessId , &session_id ) )
			{
				//セッションID取れないから無視
				continue;
			}
			if( session_id != my_session_id )
			{
				//セッションID違うから次
				continue;
			}

			//発見。たぶんこれ。
			retFound = service_name;
			result = true;
			goto cleanup;
		}

	}while( continue_required );

	//見つからなかった
	result = false;

cleanup:
	mDelete[] buffer;
	if( sc )
	{
		CloseServiceHandle( sc );
	}
	return result;
}
