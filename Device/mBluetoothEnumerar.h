//----------------------------------------------------------------------------
// Bluetooth�n���h���N���X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MBLUETOOTHENUMERAR_H_INCLUDED
#define MBLUETOOTHENUMERAR_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mBluetoothEnumerar
{
public:
	mBluetoothEnumerar();
	virtual ~mBluetoothEnumerar();

	struct BluetoothCatalogEntryBase
	{
		ULONGLONG Address;	//Bluetooth�A�h���X(48bit�l�ł�)
		WString Name;		//�f�o�C�X��
	};

	typedef BluetoothCatalogEntryBase BluetoothDeviceCatalogEntry;
	using BluetoothDeviceCatalog = std::deque< BluetoothDeviceCatalogEntry >;

	//�w�肵�������ɍ��v����Bluetooth�̃f�o�C�X�ꗗ��Ԃ��܂�
	//�����Ӌ@��̈ꗗ���Ԃ�܂�
	// retCatalog : ���ʂ̊i�[��
	// authed : 
	// remembered :
	// unknown : 
	// connected :
	// issue_inquiry : 
	// timeout_ms : ���擾�̃^�C���A�E�g(ms�P��)�B128ms�P�ʂ̂��߁A�[���͐؂�グ�܂��B�܂��A�ő�6144ms�ƂȂ肻��ȏ�̒l�͍ő�l�ɕ␳����܂��B
	// ret : �������^
	static bool SearchDevice( BluetoothDeviceCatalog& retCatalog , bool authed , bool remembered , bool unknown , bool connected , bool issue_inquiry , DWORD timeout_ms );

	typedef BluetoothCatalogEntryBase BluetoothRadioCatalogEntry;
	using BluetoothRadioCatalog = std::deque< BluetoothRadioCatalogEntry >;

	//�w�肵�������ɍ��v����Bluetooth�̃f�o�C�X�ꗗ��Ԃ��܂�
	//���{�̂ɑ�������Ă���f�o�C�X�̈ꗗ���Ԃ�܂�
	// retCatalog : ���ʂ̊i�[��
	// ret : �������^
	static bool SearchRadio( BluetoothRadioCatalog& retCatalog );

	//
	struct BluetoothLECatalogEntryBase : public BluetoothCatalogEntryBase
	{
		WString DevicePath;
	};

private:
	mBluetoothEnumerar( const mBluetoothEnumerar& src ) = delete;
	const mBluetoothEnumerar& operator=( const mBluetoothEnumerar& src ) = delete;



};


#endif
