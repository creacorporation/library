//----------------------------------------------------------------------------
// �f�o�C�X�񋓃N���X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSERIALPORTENUMERAR_H_INCLUDED
#define MSERIALPORTENUMERAR_H_INCLUDED

#include "mStandard.h"
#include "mDeviceEnumerar.h"

class mSerialPortEnumerar : protected mDeviceEnumerarBase
{
public:
	//IsSetupClass : true=�Z�b�g�A�b�v�N���X false=�C���^�[�t�F�C�X�N���X
	mSerialPortEnumerar( bool IsSetupClass = true );
	virtual ~mSerialPortEnumerar();

	struct SerialPortCatalogEntry
	{
		DWORD index;
		WString FriendlyName;	//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		WString Description;	//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���
		WString HardwareId;		//�R���g���[���p�l���́u�n�[�h�E�G�AID�v�Ɠ��� �����zCOM�|�[�g�̏ꍇUSBID������
		WString DevicePath;		//�C���^�t�F�[�X�N���X���擾�����ꍇ�A�C���X�^���X�̃p�X���Ƃ��
		WString PortName;		//�|�[�g��(COMxx)
	};
	using SerialPortCatalog = std::deque< SerialPortCatalogEntry >;

	//���݂���V���A���|�[�g�̈ꗗ���擾����
	// retInfo : ���ʊi�[��
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : �������^
	bool GetSerialPortCatalog( SerialPortCatalog& retInfo , bool reload = true );

	//���݂���V���A���|�[�g�̈ꗗ���擾����
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : ���ʂւ̎Q��
	const SerialPortCatalog& GetSerialPortCatalog( bool reload = true );


private:
	mSerialPortEnumerar( const mSerialPortEnumerar& src ) = delete;
	const mSerialPortEnumerar& operator=( const mSerialPortEnumerar& src ) = delete;

	bool Reload( void );
	SerialPortCatalog MySerialPortCatalog;
};


#endif
