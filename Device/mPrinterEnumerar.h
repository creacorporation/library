//----------------------------------------------------------------------------
// �v�����^�񋓃N���X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MPRINTERENUMERAR_H_INCLUDED
#define MPRINTERENUMERAR_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mDeviceEnumerar.h"
#include "mPrinterInfo.h"
#include <vector>

class mPrinterEnumerar : protected mDeviceEnumerarBase
{
public:
	mPrinterEnumerar() noexcept;
	virtual ~mPrinterEnumerar();

	//�f�t�H���g�̃v�����^�[�̖��O�𓾂�
	bool GetDefaultPrinterName( WString& retName ) noexcept;

	//�v�����^�[���
	using PrinterInfo = mPrinterInfo::PrinterInfo;

	//�V�X�e�����猩���Ă���v�����^�̈ꗗ�𓾂�
	//retinfo : ���ʂ̊i�[��
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : �������^
	bool GetPrinterInfo( PrinterInfo& retinfo , bool reload = true ) noexcept;

	//�V�X�e�����猩���Ă���v�����^�̈ꗗ�𓾂�
	// reload : �^�̎��ēǂݍ��݂���B�U�̎��L���b�V��������΂�����g���B
	// ret : ���ʂւ̎Q��
	const PrinterInfo& GetPrinterInfo( bool reload = true ) noexcept;

	//�v�����^�[�ݒ���
	using PrinterProperty = mPrinterInfo::PrinterProperty;

	//�v�����^�[�̐ݒ�����擾����
	// name : �擾�������v�����^�[�̖��O
	// retProperty : �擾��������
	bool GetPrinterProperty( const WString& name , PrinterProperty& retProperty ) noexcept;

private:
	mPrinterEnumerar( const mPrinterEnumerar& src ) = delete;
	const mPrinterEnumerar& operator=( const mPrinterEnumerar& src ) = delete;

protected:

	bool Reload( void );
	PrinterInfo MyPrinterInfo;

};


#endif
