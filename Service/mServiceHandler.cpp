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

#include "mServiceHandler.h"
#include <General/mErrorLogger.h>

 mServiceHandler* mServiceHandler::MyPtr;

mServiceHandler::mServiceHandler()
{
	MyStatusHandle = 0;
	ZeroMemory( &MyServiceStatus , sizeof( MyServiceStatus ) );
	return;
}

mServiceHandler::~mServiceHandler()
{
	return;
}

void mServiceHandler::SetCheckPoint( DWORD WaitHint = 2000 )
{
    MyServiceStatus.dwWaitHint = WaitHint;
    MyServiceStatus.dwCheckPoint++;
	UpdateServiceStatus();
	return;
}

bool mServiceHandler::UpdateServiceStatus()
{
	return SetServiceStatus( MyStatusHandle , &MyServiceStatus );
}

bool mServiceHandler::UpdateServiceStatus( DWORD status )
{
	MyServiceStatus.dwCurrentState = status;
	return UpdateServiceStatus();
}

void __stdcall mServiceHandler::ServiceMainProxy( DWORD argc , LPWSTR* argv )
{
	MyPtr->ServiceMain( argc , argv );
}

DWORD __stdcall mServiceHandler::HandlerProxy( DWORD Control , DWORD EventType , LPVOID EventData , LPVOID Context )
{
	return MyPtr->Handler( Control , EventType , EventData );
}

void mServiceHandler::ServiceMain( DWORD argc , LPTSTR* argv )
{
	ServiceInterfaceInfo info;
	QueryInterface( info );

	//�����̃n���h���͊J�����ςȂ��ɂ��Ă悢(MSDN���)
	MyStatusHandle = RegisterServiceCtrlHandlerExW( info.ServiceName.c_str() , HandlerProxy , this );	
	if( MyStatusHandle == 0 )
	{
		MyServiceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
		MyServiceStatus.dwServiceSpecificExitCode = 0;
		UpdateServiceStatus( SERVICE_STOPPED );
		return;
	}

    MyServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	MyServiceStatus.dwWin32ExitCode = NO_ERROR;
    MyServiceStatus.dwServiceSpecificExitCode = 0;
	UpdateServiceStatus( SERVICE_START_PENDING );

	DWORD rc;
	rc = OnInitialize( argc , argv );

	if( rc == NO_ERROR )
	{
		UpdateServiceStatus( SERVICE_RUNNING );
		rc = Main( argc , argv );
	}
	if( rc != NO_ERROR )
	{
		MyServiceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
		MyServiceStatus.dwServiceSpecificExitCode = rc;
		UpdateServiceStatus( SERVICE_STOPPED );
	}
	return;

}

DWORD mServiceHandler::Handler( DWORD Control , DWORD EventType , LPVOID EventData )
{
	switch( Control )
	{
	case SERVICE_CONTROL_STOP:
		UpdateServiceStatus( SERVICE_STOP_PENDING );
		OnStop();
		UpdateServiceStatus( SERVICE_STOPPED );
		return NO_ERROR;

	case SERVICE_CONTROL_PAUSE:
		UpdateServiceStatus( SERVICE_PAUSE_PENDING );
		OnPause();
		UpdateServiceStatus( SERVICE_PAUSED );
		return NO_ERROR;

	case SERVICE_CONTROL_CONTINUE:
		UpdateServiceStatus( SERVICE_START_PENDING );
		OnContinue();
		UpdateServiceStatus( SERVICE_RUNNING );
		return NO_ERROR;

	case SERVICE_CONTROL_INTERROGATE:
		return NO_ERROR;

	case SERVICE_CONTROL_SHUTDOWN:
		UpdateServiceStatus( SERVICE_STOP_PENDING );
		UpdateServiceStatus();
		OnShutdown();
		UpdateServiceStatus( SERVICE_STOPPED );
		return NO_ERROR;

	case SERVICE_CONTROL_PARAMCHANGE:
		OnParamChange();
		return NO_ERROR;

	case SERVICE_CONTROL_NETBINDADD:
		OnNetBindAdd();
		return NO_ERROR;

	case SERVICE_CONTROL_NETBINDREMOVE:
		OnNetBindRemove();
		return NO_ERROR;

	case SERVICE_CONTROL_NETBINDENABLE:
		OnNetBindEnable();
		return NO_ERROR;

	case SERVICE_CONTROL_NETBINDDISABLE:
		OnNetBindDisable();
		return NO_ERROR;

	case SERVICE_CONTROL_DEVICEEVENT:
		return OnDeviceEvent( EventType , EventData );

	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
		return OnHardwareProfileChange( EventType );

	case SERVICE_CONTROL_POWEREVENT:
		return OnPowerEvent( EventType , *(const POWERBROADCAST_SETTING*)EventData );

	case SERVICE_CONTROL_SESSIONCHANGE:
		OnSessionChange( EventType , *(const WTSSESSION_NOTIFICATION*)EventData );
		return NO_ERROR;

	case SERVICE_CONTROL_PRESHUTDOWN:
		OnPreShutdown();
		return NO_ERROR;

	case SERVICE_CONTROL_TIMECHANGE:
		OnTimeChange( *(const SERVICE_TIMECHANGE_INFO*)EventData );
		return NO_ERROR;

	case SERVICE_CONTROL_TRIGGEREVENT:
		OnTriggerEvent();
		return NO_ERROR;

	default:
		if( Control >= 128 && Control <= 255 )
		{
			return OnUserControlCode( Control , EventType , EventData );
		}
		break;
	}

	return ERROR_CALL_NOT_IMPLEMENTED;
}

bool mServiceHandler::Start( void )
{
	PVOID prev = InterlockedCompareExchangePointer( ( volatile PVOID*)&MyPtr , this , nullptr );
	if( prev != nullptr )
	{
		return false;
	}

	ServiceInterfaceInfo info;
	QueryInterface( info );
	MyServiceStatus.dwControlsAccepted =
		SERVICE_ACCEPT_STOP                  | //�T�[�r�X���~���悤�Ƃ��Ă���
		SERVICE_ACCEPT_PAUSE_CONTINUE        | //�T�[�r�X���~�E�ĊJ����悤�Ƃ��Ă���
		SERVICE_ACCEPT_SHUTDOWN              ; //�V�X�e���̃V���b�g�_�E��
	MyServiceStatus.dwControlsAccepted |= ( info.HandleParamChange            ) ? ( SERVICE_ACCEPT_PARAMCHANGE           ) : ( 0 ); //�T�[�r�X�̃p�����[�^���ύX���ꂽ
	MyServiceStatus.dwControlsAccepted |= ( info.HandleNetBindChange          ) ? ( SERVICE_ACCEPT_NETBINDCHANGE         ) : ( 0 ); //�l�b�g���[�N�ڑ��̕ύX
	MyServiceStatus.dwControlsAccepted |= ( info.HandleHardwareProfileChange  ) ? ( SERVICE_ACCEPT_HARDWAREPROFILECHANGE ) : ( 0 ); //�V�X�e���̃n�[�h�E�G�A�v���t�@�C�����ύX���ꂽ
	MyServiceStatus.dwControlsAccepted |= ( info.HandlePowerChange            ) ? ( SERVICE_ACCEPT_POWEREVENT            ) : ( 0 ); //�V�X�e���̓d����Ԃ��ύX���ꂽ
	MyServiceStatus.dwControlsAccepted |= ( info.HandleSessionChange          ) ? ( SERVICE_ACCEPT_SESSIONCHANGE         ) : ( 0 ); //�V�X�e���̃Z�b�V�������ύX���ꂽ
	MyServiceStatus.dwControlsAccepted |= ( info.HandlePreShutdown            ) ? ( SERVICE_ACCEPT_PRESHUTDOWN           ) : ( 0 ); //�V�X�e���̃V���b�g�_�E���O�C�x���g������
	MyServiceStatus.dwControlsAccepted |= ( info.HandleTimeChange             ) ? ( SERVICE_ACCEPT_TIMECHANGE            ) : ( 0 ); //�V�X�e���������ύX���ꂽ
	MyServiceStatus.dwControlsAccepted |= ( info.HandleTriggerEvent           ) ? ( SERVICE_ACCEPT_TRIGGEREVENT          ) : ( 0 ); //����ȃC�x���g���󂯎��(�v�ʓr�ݒ�B�������B)
	MyServiceStatus.dwCurrentState = SERVICE_STOPPED;
	
	SERVICE_TABLE_ENTRYW service[ 2 ];
	service[ 0 ].lpServiceName = const_cast< LPWSTR >( info.ServiceName.c_str() );
	service[ 0 ].lpServiceProc = ServiceMainProxy;
	service[ 1 ].lpServiceName = 0;
	service[ 1 ].lpServiceProc = 0;

	bool result = StartServiceCtrlDispatcherW( service );
	InterlockedExchangePointer( ( volatile PVOID*)&MyPtr , nullptr );

	if( !result )
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�T�[�r�X�̃f�B�X�p�b�`���[���N���ł��܂���" );
	}
	return result;
}



void mServiceHandler::OnStop( void )
{
}


void mServiceHandler::OnPause( void )
{
}


void mServiceHandler::OnContinue( void )
{
}


void mServiceHandler::OnShutdown( void )
{
}


void mServiceHandler::OnParamChange( void )
{
}


void mServiceHandler::OnNetBindAdd( void )
{
}


void mServiceHandler::OnNetBindRemove( void )
{
}


void mServiceHandler::OnNetBindEnable( void )
{
}


void mServiceHandler::OnNetBindDisable( void )
{
}


DWORD mServiceHandler::OnDeviceEvent( DWORD type , const void* data)
{
	return NO_ERROR;
}


DWORD mServiceHandler::OnHardwareProfileChange( DWORD type )
{
	return NO_ERROR;
}


DWORD mServiceHandler::OnPowerEvent( DWORD type , const POWERBROADCAST_SETTING& data)
{
	return NO_ERROR;
}


void mServiceHandler::OnSessionChange( DWORD type , const WTSSESSION_NOTIFICATION& data)
{
}


void mServiceHandler::OnPreShutdown( void )
{
}


void mServiceHandler::OnTimeChange( const  SERVICE_TIMECHANGE_INFO& data )
{
}


void mServiceHandler::OnTriggerEvent( void )
{
}


DWORD mServiceHandler::OnUserControlCode( DWORD control , DWORD param1 , void* param2 )
{
	return NO_ERROR;
}

DWORD mServiceHandler::OnInitialize( DWORD argc , LPTSTR* argv )
{
	return NO_ERROR;
}
