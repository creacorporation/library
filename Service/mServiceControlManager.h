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

#ifndef MSERVICECONTROLMANAGER_H_INCLUDED
#define MSERVICECONTROLMANAGER_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

namespace Definitions_mServiceControlManager
{
	enum ServiceStartType
	{
		STARTTYPE_AUTO = SERVICE_AUTO_START,
		STARTTYPE_ONDEMAND = SERVICE_DEMAND_START,
		STARTTYPE_DISABLED = SERVICE_DISABLED,
	};
	enum ServiceErrorControl
	{
		ONERROR_LOGONLY = SERVICE_ERROR_IGNORE,
		ONERROR_POPUPMESSAGEBOX = SERVICE_ERROR_NORMAL,
		ONERROR_SEVERE = SERVICE_ERROR_SEVERE,
		ONERROR_CRITICAL = SERVICE_ERROR_CRITICAL,
	};
	enum ServiceProcessType
	{
		//�������g�̃v���Z�X�����B
		SERVICE_OWN_PROCESS = 1,
		//�z�X�g�v���Z�X�����B
		SERVICE_SHARE_PROCESS = 2,
	};

	enum ServiceProcessUser
	{
		SYSTEM_PROCESS,
		LOGONUSER_PROCESS,
	};
};

namespace mServiceControlManager
{
	//�I�v�V�����\����
	//���ۂɃt�H���g���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct ServiceParam
	{
		using ServiceProcessUser = Definitions_mServiceControlManager::ServiceProcessUser;
		const ServiceProcessUser method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B

		using ServiceStartType = Definitions_mServiceControlManager::ServiceStartType;
		using ServiceErrorControl = Definitions_mServiceControlManager::ServiceErrorControl;
		using ServiceProcessType = Definitions_mServiceControlManager::ServiceProcessType;

		//�T�[�r�X�̖��́i�T�[�r�X��ID�j
		WString ServiceName;
		//�T�[�r�X�̕\����̖��O
		WString DisplayName;
		//�T�[�r�X�̐�����
		WString Description;
		//�T�[�r�X�̃v���Z�X�̎��
		ServiceProcessType ProcessType;
		//���[�U�[�Ƃ̒��ڑΘb�������邩(LocalSystem�A�J�E���g�Ŏ��s����Ƃ�����)
		bool Interactive;
		//�T�[�r�X�N�����[�h
		ServiceStartType StartType;
		//�T�[�r�X�ɃG���[�����������Ƃ��̋���
		ServiceErrorControl ErrorControl;
		//�T�[�r�X�̃o�C�i��
		WString BinaryPathName;
		//�ˑ���̃T�[�r�X��
		WStringVector Dependencies;

	protected:
		ServiceParam() = delete;
		ServiceParam( ServiceProcessUser method ) : method( method )
		{
			ProcessType = ServiceProcessType::SERVICE_OWN_PROCESS;
			Interactive = false;
			StartType = ServiceStartType::STARTTYPE_DISABLED;
			ErrorControl = ServiceErrorControl::ONERROR_LOGONLY;
		}
	};


	//�T�[�r�X�쐬�p�����[�^�i�ʏ�̃T�[�r�X�j
	//�E�T�[�r�X���g�p����A�J�E���g���w��ł���
	//�E�V�X�e���N���Ɠ����ɋN���ł���
	struct ServiceParam_System : public ServiceParam
	{
		//�T�[�r�X�����s���郆�[�U�[��
		WString UserName;
		//�T�[�r�X�����s���郆�[�U�[�̃p�X���[�h
		WString Password;

		ServiceParam_System() : ServiceParam( ServiceProcessUser::SYSTEM_PROCESS )
		{
		}
	};

	//�T�[�r�X�쐬�p�����[�^
	//�E�T�[�r�X���g�p����A�J�E���g�͌��݃��O�I�����Ă��郆�[�U�[
	//�E���[�U�[�����O�I�����Ă��Ȃ��Ǝg���Ȃ�
	struct ServiceParam_LogonUser : public ServiceParam
	{
		ServiceParam_LogonUser() : ServiceParam( ServiceProcessUser::LOGONUSER_PROCESS )
		{
		}
	};

	//�V�����T�[�r�X���쐬����
	bool CreateNewService( const ServiceParam& param );

	//�����̃T�[�r�X���폜����
	bool DeleteExistingService( const WString& ServiceName );

	//�����̃T�[�r�X���J�n����
	bool StartExistingService( const WString& ServiceName , const WStringDeque& args );

	//�����̃T�[�r�X�𒆒f����
	bool PauseExistingService( const WString& ServiceName );

	//�����̃T�[�r�X���ĊJ����
	bool ContinueExistingService( const WString& ServiceName );

	//�����̃T�[�r�X���~����
	bool StopExistingService( const WString& ServiceName , bool is_planed , const WString& reason );

	//�����̃T�[�r�X�ɃR���g���[���R�[�h�𑗂�
	bool ControlExistingService( const WString& ServiceName , DWORD code );

	//���[�U�[�Z�b�V�����œ��삵�Ă���T�[�r�X�́A���Z�b�V�����ł̃T�[�r�X���𒲂ׂ�
	//�@�����[�U�[�Z�b�V�����œ��삵�Ă���T�[�r�X�́A�^�X�N�}�l�[�W�����ł݂�ƁA
	//�@�@�u<�T�[�r�X��>_xxxxxx�v�̖��O�ɂȂ��Ă��āAxxxxxx�̕����̓��O�C�����邽�тɕς��B
	//�@�@���̕������擾����API�͔���J�̂悤�Ȃ̂ŁA�ȉ������̂��̂���������B
	// (1)���쒆�̃T�[�r�X
	// (2)���[�U�[�Z�b�V�����œ��삵�Ă���T�[�r�X
	// (3)�T�[�r�X���̃A���_�[�o�[���O�̕������A�T���Ă���T�[�r�X���ƈ�v
	// (4)���݂̃v���Z�X�����s���Ă���Z�b�V����ID�ƁA�T�[�r�X�����s���Ă���Z�b�V����ID������
	// ServiceName = �����Ώۂ̃T�[�r�X
	// retFound = ���������T�[�r�X
	// ret = �������^
	bool SearchUserProcessService( const WString& ServiceName , WString& retFound );

};


#endif


