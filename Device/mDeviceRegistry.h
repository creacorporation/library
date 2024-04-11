//----------------------------------------------------------------------------
// �f�o�C�X�p���W�X�g���n���h��
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MDEVICEEREGISTRY_H_INCLUDED
#define MDEVICEEREGISTRY_H_INCLUDED

#include <General/mRegistry.h>

class mDeviceRegistry : public mRegistry
{

public:
	mDeviceRegistry();
	virtual ~mDeviceRegistry();

	//���W�X�g�����J��
	bool Open( HDEVINFO devinfo , const SP_DEVINFO_DATA& infodata );

private:
	mDeviceRegistry( const mDeviceRegistry& src );
	const mDeviceRegistry& operator=( const mDeviceRegistry& src );
};


#endif

