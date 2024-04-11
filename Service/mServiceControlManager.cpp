//----------------------------------------------------------------------------
// �T�[�r�X�n���h��
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mServiceControlManager.h"
#include "General/mErrorLogger.h"

//�T�[�r�X�̖��O����n���h�����擾���ăR�[���o�b�N���Ăяo���T�[�r�X���[�`��
//�R�[���o�b�N�I����̓n���h�����������
// name : ������s�������T�[�r�X�̖��O
// req_access_right : �t�^�������A�N�Z�X���B�R�[���o�b�N���ōs����������ɉ����Đݒ肷��
// opt : ���[�U�[��`�̒l
// callback : �Ăяo���R�[���o�b�N
//		��1������name�Ŏw�肵���T�[�r�X�̃n���h��
//		��2������opt�Ɏw�肵�����[�U�[��`�̒l
//		ret���������^
// ret : �������^
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
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X���폜�ł��܂���ł���" , ServiceName );
		return false;
	}
	return true;
}

//�����̃T�[�r�X�𒆒f����
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
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X�𒆒f�ł��܂���ł���" , ServiceName );
		return false;
	}
	return true;
}

//�����̃T�[�r�X���ĊJ����
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
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X���ĊJ�ł��܂���ł���" , ServiceName );
		return false;
	}
	return true;
}

//�����̃T�[�r�X�ɃR���g���[���R�[�h�𑗂�
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
		RaiseErrorF( g_ErrorLogger , 0 , L"�R���g���[���R�[�h�̒l���͈͊O�ł�" , L"%d@%s" , code , ServiceName.c_str() );
		return false;
	}

	if( !HandlingServiceTemplate( ServiceName , SERVICE_USER_DEFINED_CONTROL , code , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X�ɃR�[�h�𑗐M�ł��܂���ł���" , ServiceName );
		return false;
	}
	return true;
}

//�����̃T�[�r�X���J�n����
bool mServiceControlManager::StartExistingService( const WString& ServiceName , const WStringDeque& args )
{
	//�J�n����������R�[���o�b�N
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

	//�����̏���
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

	//�J�n
	bool result = HandlingServiceTemplate( ServiceName , SERVICE_START , (DWORD_PTR)&opt , Callback );
	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X�̊J�n�����s���܂���" , ServiceName );
	}

	mDelete[] opt.argv;
	return result;
}

//�����̃T�[�r�X���~����
bool mServiceControlManager::StopExistingService( const WString& ServiceName , bool is_planed , const WString& reason )
{
	//�p�����[�^�̏���
	if( 127 < reason.size() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X��~���̗��R�R�[�h���������܂�" , ServiceName );
		return false;
	}

	SERVICE_CONTROL_STATUS_REASON_PARAMSW param;
	ZeroMemory( &param , sizeof( param ) );
	param.dwReason = ( is_planed ) ? ( SERVICE_STOP_REASON_FLAG_PLANNED ) : ( SERVICE_STOP_REASON_FLAG_UNPLANNED );
	param.pszComment = ( !reason.empty() ) ? ( const_cast<LPWSTR>( reason.data() ) ) : ( nullptr );

	//�R�[���o�b�N������
	auto Callback = []( SC_HANDLE sv , DWORD_PTR opt )-> bool
	{
		SERVICE_CONTROL_STATUS_REASON_PARAMSW* param = (SERVICE_CONTROL_STATUS_REASON_PARAMSW*)opt;
		return ControlServiceExW( sv , SERVICE_CONTROL_STOP , SERVICE_CONTROL_STATUS_REASON_INFO , param );
	};

	//�e���v���[�g�̌Ăяo��
	if( !HandlingServiceTemplate( ServiceName , SERVICE_STOP , (DWORD_PTR)&param , Callback ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X���ĊJ�ł��܂���ł���" , ServiceName );
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
		RaiseError( g_ErrorLogger , 0 , L"SCM���J���܂���" );
		goto cleanup;
	}

	//�v���Z�X��ʂƃ��[�U�[���A�p�X���[�h
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
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X�v���Z�X�̎�ގw�肪�Ԉ���Ă��܂�" );
		goto cleanup;
	}

	//���[�U�[�Ƃ̑Θb���H
	if( param.Interactive )
	{
		ServiceType |= SERVICE_INTERACTIVE_PROCESS;
	}

	//�ˑ��֌W
	MakeDoubleNullString( param.Dependencies , DependenciesString );

	//�T�[�r�X����
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
		RaiseError( g_ErrorLogger , 0 , L"�T�[�r�X�̓o�^�����s���܂���" );
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

//���[�U�[�����œ��삵�Ă���T�[�r�X�́A���Z�b�V�����ł̃T�[�r�X���𒲂ׂ�
bool mServiceControlManager::SearchUserProcessService( const WString& ServiceName , WString& retFound )
{
	retFound.clear();

	bool result = false;
	BYTE* buffer = nullptr;

	SC_HANDLE sc = NULL;
	sc = OpenSCManagerW( 0 , SERVICES_ACTIVE_DATABASE , SC_MANAGER_ENUMERATE_SERVICE );
	if( !sc )
	{
		RaiseError( g_ErrorLogger , 0 , L"SCM���J���܂���" );
	}


	DWORD my_session_id;								//�����̃Z�b�V����ID
	if( !ProcessIdToSessionId( GetCurrentProcessId() , &my_session_id ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���̃v���Z�X�̃Z�b�V����ID���擾�ł��܂���" );
	}
	WString search_service_name = ServiceName + L"_";	//�T�[�`����T�[�r�X��
	bool continue_required;		//2���ڂ����邩�H�@EnumServicesStatusExW�͂܂Ƃ߂ēǂݏo�������ɁA��x�ɑS���ǂ߂Ȃ����Ƃ�����炵���B
	DWORD resume_handle = 0;	//�ǂݏo���ʒu�̃C���f�b�N�X�B

	do
	{
		//�o�b�t�@�̏���
		DWORD service_count = 0;	//�ǂݎ�����T�[�r�X�̃G���g����
		DWORD req_size = 0;			//�K�v�ȃo�b�t�@�T�C�Y
		if( !EnumServicesStatusExW( sc , SC_ENUM_PROCESS_INFO , SERVICE_WIN32 , SERVICE_ACTIVE , 0 , 0 , &req_size , &service_count , &resume_handle , nullptr ) )
		{
			if( GetLastError() != ERROR_MORE_DATA )
			{
				goto cleanup;
			}
		}
		mDelete[] buffer;
		buffer = mNew BYTE[ req_size ];

		//�f�[�^�擾
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

		//����ꂽ���ʂ����ɃX�L��������
		ENUM_SERVICE_STATUS_PROCESS* stats = (ENUM_SERVICE_STATUS_PROCESS*)buffer;
		for( DWORD i = 0 ; i < service_count ; i++ )
		{
			//���[�U�[�����œ��삵�Ă���T�[�r�X�H
			if( !( stats[ i ].ServiceStatusProcess.dwServiceType & SERVICE_USERSERVICE_INSTANCE ) )
			{
				//���[�U�[�����œ��삵�ĂȂ��T�[�r�X�����疳�����Ď�
				continue;
			}

			//���O����v�H
			WString service_name = stats[ i ].lpServiceName;
			if( service_name.substr( 0 , search_service_name.size() ) != search_service_name )
			{
				//���O�s��v�Ȃ̂Ŏ�
				continue;
			}

			//�Z�b�V����ID����v�H
			DWORD session_id;
			if( !ProcessIdToSessionId( stats[ i ].ServiceStatusProcess.dwProcessId , &session_id ) )
			{
				//�Z�b�V����ID���Ȃ����疳��
				continue;
			}
			if( session_id != my_session_id )
			{
				//�Z�b�V����ID�Ⴄ���玟
				continue;
			}

			//�����B���Ԃ񂱂�B
			retFound = service_name;
			result = true;
			goto cleanup;
		}

	}while( continue_required );

	//������Ȃ�����
	result = false;

cleanup:
	mDelete[] buffer;
	if( sc )
	{
		CloseServiceHandle( sc );
	}
	return result;
}
