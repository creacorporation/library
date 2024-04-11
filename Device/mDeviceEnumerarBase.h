//----------------------------------------------------------------------------
// �f�o�C�X�񋓃N���X
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MDEVICEENUMERARBASE_H_INCLUDED
#define MDEVICEENUMERARBASE_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include "mDeviceRegistry.h"
#include <vector>
#include <SetupAPI.h>
#include <devguid.h>

class mDeviceEnumerarBase
{
public:
	//�N���X����p���ăf�o�C�X�̈ꗗ���擾����
	//ClassName : �ΏۂƂ���N���X
	//IsSetupClass : true=�Z�b�g�A�b�v�N���X false=�C���^�[�t�F�C�X�N���X
	mDeviceEnumerarBase( const WString& ClassName , bool IsSetupClass = true );

	//GUID��p���ăf�o�C�X�̈ꗗ���擾����
	//�E�w�肷�ׂ�GUID�́A�uGUID_DEVCLASS_xxxx�v�Ƃ����}�N����devguid.h�ɒ�`����Ă���
	//  ��FGUID_DEVCLASS_PORTS���V���A���|�[�g
	//�EBluetooth�ł́Abthdef.h�Abthledef.h�ɂ��C���^�[�t�F�C�X��GUID��`������
	//�@��FGUID_BLUETOOTHLE_DEVICE_INTERFACE��Bluetooth�̃f�o�C�X�C���^�[�t�F�C�X
	//ClassGuid : �ΏۂƂ���N���X
	//IsSetupClass : true=�Z�b�g�A�b�v�N���X false=�C���^�[�t�F�C�X�N���X
	mDeviceEnumerarBase( const GUID& ClassGuid , bool IsSetupClass = true );

	virtual ~mDeviceEnumerarBase();


private:

	mDeviceEnumerarBase() = delete;
	mDeviceEnumerarBase( const mDeviceEnumerarBase& src ) = delete;
	const mDeviceEnumerarBase& operator=( const mDeviceEnumerarBase& src ) = delete;
	
	//���擾�p�̃n���h��
	HDEVINFO MyHandle;

protected:

	//�n���h�����J������
	void FreeDevHandle( void );

	//�Ώۂ̃N���X�ɑ��݂���f�o�C�X�̈ꗗ�����ۂɎ擾����
	// ret : �������^
	bool CreateCatalog( bool reload = true );

	//����ێ����Ă���f�o�C�X�N���X��GUID
	GUID MyClassGuid;

	//MyClassGuid���Z�b�g�A�b�v�N���X�̂��̂��C���^�[�t�F�C�X�N���X�̂��̂�
	bool MyIsSetupClass;

	//�擾�������
	struct DevInfoDataEntry
	{
		SP_DEVINFO_DATA DevInfo;
		WString DevicePath;
	};
	using DevInfoData = std::vector< DevInfoDataEntry >;
	DevInfoData MyDevInfoData;

	//�������(DevInfoData)�\�z�@�Z�b�g�A�b�v�N���X�p
	bool BuildDevInfoData_Setup( void );
	//�������(DevInfoData)�\�z�@�C���^�[�t�F�C�X�N���X�p
	bool BuildDevInfoData_Interface( void );

	//���W�X�g���̎擾
	// index : �J�^���O�̃C���f�b�N�X
	// retReg : �擾�������W�X�g���̃n���h��
	bool GetDeviceRegistry( DWORD index , mDeviceRegistry& retReg )const;

	//�v���p�e�B�̎擾
	// index : �J�^���O�̃C���f�b�N�X
	// prop_id : �擾����v���p�e�B
	// retProp : �擾��������
	bool GetProperty( DWORD index , DWORD prop_id , WString& retProp)const;

	//�v���p�e�B�̎擾
	// info : �f�o�C�X���Z�b�g
	// prop_id : �擾����v���p�e�B
	// retProp : �擾��������
	bool GetProperty( const SP_DEVINFO_DATA& info , DWORD prop_id , WString& retProp)const;

};

#endif
