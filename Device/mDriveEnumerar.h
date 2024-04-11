//----------------------------------------------------------------------------
// �h���C�u�񋓃N���X
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MDRIVEENUMERAR_H_INCLUDED
#define MDRIVEENUMERAR_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include "mDeviceEnumerarBase.h"
#include <vector>
#include <SetupAPI.h>
#include <devguid.h>

class mDriveEnumerar : protected mDeviceEnumerarBase
{
public:
	mDriveEnumerar();
	virtual ~mDriveEnumerar();

	struct CatalogEntry
	{
		DWORD index;				//�J�^���O�̃C���f�b�N�X
		WString FriendlyName;		//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		WString Description;		//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���
		WString HardwareId;			//�R���g���[���p�l���́u�n�[�h�E�G�AID�v�Ɠ���
		WString DevicePath;			//�f�o�C�X�̃p�X
		DEVICE_TYPE DeviceType;		//�f�o�C�X�^�C�v(FILE_DEVICE_XXX)
		DWORD DeviceNumber;			//�f�o�C�X�ԍ�
		DWORD PartitionNumber;		//�p�[�e�B�V�����ԍ�
		GUID DeviceGuid;			//�f�o�C�X��ID(�n�[�h�E�G�A���琶�������ŗLID�A�n�[�h�E�G�A���琶���ł��Ȃ��ꍇ�̓����_���l)
		bool IsDeviceGuidFromHW;	//DeviceGuid���n�[�h�E�G�A���琶������Ă���ΐ^�B�^�̏ꍇ�i���I�B�U�̏ꍇ�͍ċN�����邽��DeviceGuid���ς��B
		wchar_t Drive;				//�h���C�u�Ƀ}�E���g����Ă���΃h���C�u���^�[�B���Ȃ����0�B
	};
	using Catalog = std::deque< CatalogEntry >;

	//Catalog���Đ�������
	bool Reload( void );

	//�Ώۂ̃N���X�ɑ��݂���f�o�C�X�̈ꗗ���擾����
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// retInfo : ���ʊi�[��
	// ret : �������^
	bool GetCatalog( Catalog& retCatalog , bool reload = true );

	//�Ώۂ̃N���X�ɑ��݂���f�o�C�X�̈ꗗ���擾����
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : ���ʂւ̎Q��
	const Catalog& GetCatalog( bool reload = true );

	//�w�肵���f�o�C�X�p�X�ɑΉ�����J�^���O�̃G���g����Ԃ��B
	// devicepath : �擾�������f�o�C�X�p�X
	// ret : ����ꂽ�J�^���O�ւ̃|�C���^�B�G���[�̏ꍇ�k���B
	const CatalogEntry* GetFromDevicePath( const WString DevicePath )const;

private:

	mDriveEnumerar( const mDriveEnumerar& src ) = delete;
	const mDriveEnumerar& operator=( const mDriveEnumerar& src ) = delete;


	//�h���C�u���^�[�̃X�L����
	bool ScanDriveLetter( void );

	//���ʊi�[��
	Catalog MyCatalog;
};


#endif
