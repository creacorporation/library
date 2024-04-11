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

#ifndef MSERVICEHANDLER_H_INCLUDED
#define MSERVICEHANDLER_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mServiceHandler
{
public:

	mServiceHandler();
	virtual ~mServiceHandler();

	//�T�[�r�X���J�n����
	//���T�[�r�X���I������܂Ő����Ԃ��Ȃ�
	//ret : �T�[�r�X�̋N���Ɏ��s�����false�B�T�[�r�X������I�������ꍇ��true�B
	bool Start( void );

private:

	mServiceHandler( const mServiceHandler& source ) = delete;
	void operator=( const mServiceHandler& source ) = delete;

	SERVICE_STATUS_HANDLE MyStatusHandle;
	SERVICE_STATUS MyServiceStatus;

	//�ȉ�Windows����Ă΂�郁���o�B���[�U���Ăяo�����Ƃ͂Ȃ��B
	static mServiceHandler* MyPtr;

	static void __stdcall  ServiceMainProxy( DWORD argc , LPWSTR* argv );
	void ServiceMain( DWORD argc , LPTSTR* argv );

	static DWORD __stdcall HandlerProxy( DWORD Control , DWORD EventType , LPVOID EventData , LPVOID Context );
	DWORD Handler( DWORD Control , DWORD EventType , LPVOID EventData );

	bool UpdateServiceStatus();
	bool UpdateServiceStatus( DWORD status );

protected:

	//�T�[�r�X�̏��
	struct ServiceInterfaceInfo
	{
		WString ServiceName;
		bool HandleParamChange;				//�T�[�r�X�̃p�����[�^���ύX���ꂽ
		bool HandleNetBindChange;			//�l�b�g���[�N�ڑ��̕ύX
		bool HandleHardwareProfileChange;	//�V�X�e���̃n�[�h�E�G�A�v���t�@�C�����ύX���ꂽ
		bool HandlePowerChange;				//�V�X�e���̓d����Ԃ��ύX���ꂽ
		bool HandleSessionChange;			//�V�X�e���̃Z�b�V�������ύX���ꂽ
		bool HandlePreShutdown;				//�V�X�e���̃V���b�g�_�E���O�C�x���g������
		bool HandleTimeChange;				//�V�X�e���������ύX���ꂽ
		bool HandleTriggerEvent;			//����ȃC�x���g���󂯎��B�Ȃ��AmServiceControlManager�ɂ����āA���̐ݒ菈���͖������B

		ServiceInterfaceInfo()
		{
			Reset();
		}
		void Reset( void )
		{
			ServiceName = L"";
			HandleParamChange = false;
			HandleNetBindChange = false;
			HandleHardwareProfileChange = false;
			HandlePowerChange = false;
			HandleSessionChange = false;
			HandlePreShutdown = false;
			HandleTimeChange = false;
			HandleTriggerEvent = false;
		}
	};

	//�T�[�r�X�̏���Ԃ��܂�
	virtual void QueryInterface( ServiceInterfaceInfo& retInfo ) = 0;

	//���Ԃ������鏈�����s���ꍇ�̐i����
	void SetCheckPoint( DWORD WaitHint );

	//�T�[�r�X�̏��������ɌĂяo���܂�
	// ret : �G���[�Ȃ��������ł����ꍇ��NO_ERROR( = 0 )��Ԃ��܂�
	//       �G���[�����������ꍇ�́ANO_ERROR�ȊO�̒l��Ԃ��܂�
	virtual DWORD OnInitialize( DWORD argc , LPTSTR* argv );

	//�T�[�r�X�̃��C�����[�`��
	// ret : �G���[�Ȃ��������ł����ꍇ��NO_ERROR( = 0 )��Ԃ��܂�
	//       �G���[�����������ꍇ�́ANO_ERROR�ȊO�̒l��Ԃ��܂�
	virtual DWORD Main( DWORD argc , LPTSTR* argv ) = 0;

	//�T�[�r�X�̒�~���v�����ꂽ
	virtual void OnStop( void );

	//�T�[�r�X�̈ꎞ��~���v�����ꂽ
	virtual void OnPause( void );

	//�ꎞ��~���̃T�[�r�X�ĊJ��v�����ꂽ
	virtual void OnContinue( void );

	//�V�X�e�����V���b�g�_�E�����悤�Ƃ��Ă���
	virtual void OnShutdown( void );

	//�T�[�r�X�̃p�����[�^���ύX���ꂽ
	virtual void OnParamChange( void );

	//�V�����l�b�g���[�N�ڑ������o���ꂽ
	//��Plug&Play�̎g�p������
	[[deprecated]]
	virtual void OnNetBindAdd( void );

	//�L���ł������l�b�g���[�N�ڑ��̂��������ꂩ�������ɂȂ���
	//��Plug&Play�̎g�p������
	[[deprecated]]
	virtual void OnNetBindRemove( void );

	//�����ł������l�b�g���[�N�ڑ��̂��������ꂩ���L���ɂȂ���
	//��Plug&Play�̎g�p������
	[[deprecated]]
	virtual void OnNetBindEnable( void );

	//���݂��Ă����l�b�g���[�N�ڑ����폜���ꂽ
	//��Plug&Play�̎g�p������
	[[deprecated]]
	virtual void OnNetBindDisable( void );

	//�f�o�C�X�C�x���g����������
	//�����̃C�x���g���擾����ɂ́A���炩����RegisterDeviceNotification()�œo�^���K�v
	virtual DWORD OnDeviceEvent( DWORD type , const void* data);

	//�n�[�h�E�G�A�̃v���t�@�C�����ς����
	virtual DWORD OnHardwareProfileChange( DWORD type );

	//�d����Ԃ̕ύX
	virtual DWORD OnPowerEvent( DWORD type , const POWERBROADCAST_SETTING& data);

	//�Z�b�V�������ύX���ꂽ
	virtual void OnSessionChange( DWORD type , const WTSSESSION_NOTIFICATION& data);

	//�V�X�e���̃V���b�g�_�E���O�C�x���g������
	//�����̃C�x���g���n���h�����邱�ƂŁA�T�[�r�X�I���܂ŃV���b�g�_�E�����u���b�N�ł���
	virtual void OnPreShutdown( void );

	//�����ύX
	virtual void OnTimeChange( const  SERVICE_TIMECHANGE_INFO& data );

	//"Service Trigger Events"������
	//�����̃C�x���g���擾����ɂ́A���炩����ChangeServiceConfig2()�œo�^���K�v
	virtual void OnTriggerEvent( void );

	//���[�U�[��`
	virtual DWORD OnUserControlCode( DWORD control , DWORD param1 , void* param2 );


};

#endif //MSERVICEHANDLER_H_INCLUDED

