//----------------------------------------------------------------------------
// �f�o�C�X�񋓃N���X
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MDEVICEENUMERAR_H_INCLUDED
#define MDEVICEENUMERAR_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include "mDeviceEnumerarBase.h"
#include <vector>
#include <SetupAPI.h>
#include <devguid.h>

class mDeviceEnumerar : protected mDeviceEnumerarBase
{
public:
	//�N���X����p���ăf�o�C�X�̈ꗗ���擾����
	//ClassName : �ΏۂƂ���N���X
	//IsSetupClass : true=�Z�b�g�A�b�v�N���X false=�C���^�[�t�F�C�X�N���X
	mDeviceEnumerar( const WString& ClassName , bool IsSetupClass = true );

	//GUID��p���ăf�o�C�X�̈ꗗ���擾����
	//�E�w�肷�ׂ�GUID�́A�uGUID_DEVCLASS_xxxx�v�Ƃ����}�N����devguid.h�ɒ�`����Ă���
	//  ��FGUID_DEVCLASS_PORTS���V���A���|�[�g
	//�EBluetooth�ł́Abthdef.h�Abthledef.h�ɂ��C���^�[�t�F�C�X��GUID��`������
	//�@��FGUID_BLUETOOTHLE_DEVICE_INTERFACE��Bluetooth�̃f�o�C�X�C���^�[�t�F�C�X
	//ClassGuid : �ΏۂƂ���N���X
	//IsSetupClass : true=�Z�b�g�A�b�v�N���X false=�C���^�[�t�F�C�X�N���X
	mDeviceEnumerar( const GUID& ClassGuid , bool IsSetupClass = true );

	virtual ~mDeviceEnumerar();

	struct DeviceCatalogEntry
	{
		DWORD index;			//�J�^���O�̃C���f�b�N�X
		WString FriendlyName;	//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		WString Description;	//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���
		WString HardwareId;		//�R���g���[���p�l���́u�n�[�h�E�G�AID�v�Ɠ��� ��USBID�Ƃ�������
		WString DevicePath;		//�f�o�C�X�̃p�X(�C���^�[�t�F�C�X�̂Ƃ��̂�)
	};
	using DeviceCatalog = std::deque< DeviceCatalogEntry >;

	//�Ώۂ̃N���X�ɑ��݂���f�o�C�X�̈ꗗ���擾����
	// retInfo : ���ʊi�[��
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : �������^
	bool GetDeviceCatalog( DeviceCatalog& retInfo , bool reload = true );

	//�Ώۂ̃N���X�ɑ��݂���f�o�C�X�̈ꗗ���擾����
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : �������^
	const DeviceCatalog& GetDeviceCatalog( bool reload = true );

private:

	mDeviceEnumerar() = delete;
	mDeviceEnumerar( const mDeviceEnumerar& src ) = delete;
	const mDeviceEnumerar& operator=( const mDeviceEnumerar& src ) = delete;

	bool Reload( void );

	DeviceCatalog MyDeviceCatalog;
};


#endif
