//----------------------------------------------------------------------------
// �v�����^���
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MPRINTERINFO_H_INCLUDED
#define MPRINTERINFO_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <vector>

namespace mPrinterInfo
{
	//�V�X�e���ɑ��݂���v�����^�̈ꗗ���
	struct PrinterInfoEntry
	{
		//�v�����^�̖��O
		//�v�����^�̃n���h�����J���ȂǁA�I������Ƃ��͂��̕�������g�p
		WString Name;

		//�v�����^�̐���
		WString Description;

		//�f�t�H���g�v�����^�Ȃ�true
		bool IsDefault;
	};

	//�V�X�e���ɑ��݂���v�����^�̈ꗗ���
	using PrinterInfo = std::vector< PrinterInfoEntry >;

	//�p���̎��ID
	using PaperId = DWORD;

	//�p���̏��
	struct PaperInfoEntry
	{
		//�p���̖��O�i�ǂ̂��́j
		WString FriendlyName;
		//�p���h�c
		//�E�v�����^�̗p���ݒ�ł��̒l���g��
		//�����Ԃ�wingdi.h�Œ�`����Ă���"DMPAPER_"�Ŏn�܂�}�N���̒l
		PaperId Id;
		//�p���̕�(0.1�~���P�� / 1cm = 100)
		DWORD Width;
		//�p���̍���(0.1�~���P�� / 1cm = 100)
		DWORD Height;
	};

	//�p���̈ꗗ���
	using PaperInfo = std::vector< PaperInfoEntry >;

	//�v�����^�̏��
	struct PrinterProperty
	{
		//�J���[�v�����^�ł���ΐ^
		bool IsColor;
		//���ʃv�����^�[�ł���ΐ^
		bool IsDuplex;
		//�𑜓x
		DWORD Dpi;
		//��t�\�ȗp���̎��
		PaperInfo AcceptablePaper;
	};
};


#endif
