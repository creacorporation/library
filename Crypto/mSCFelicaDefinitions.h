//----------------------------------------------------------------------------
// Felica Light-S �J�[�h�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSCDEFINITIONS_H_INCLUDED
#define MSCDEFINITIONS_H_INCLUDED

#include <mStandard.h>
#include <General/mBinary.h>

namespace mSCFelicaDefinitions
{
	//REG�u���b�N�̃f�[�^�\��
	struct RegData
	{
		DWORD RegA;
		DWORD RegB;
		mBinary RegC;

		RegData()
		{
			RegA = 0;
			RegB = 0;
		}
		bool operator<=( const RegData& r )const
		{
			return ( RegA <= r.RegA ) && ( RegB <= r.RegB );
		}
	};

	//�ǂݏ������s���u���b�N�̃G���g��
	struct DataBlockEntry
	{
		//�u���b�N�̔ԍ�
		DWORD BlockNumber;
		//�f�[�^�i�P�U�o�C�g�j
		mBinary Data;
	};

	//�ǂݏ������s���u���b�N�̃f�[�^
	using DataBlock = std::vector< DataBlockEntry >;

	//�ǂݎ����s�������u���b�N�̔ԍ�
	using ReadRequestBlock = std::vector< DWORD >;

	//�F�؏��
	enum AuthStatus
	{
		//�F�؂͍s���Ă��܂���
		Unauthorized,
		//�����F�؂��s���Ă��܂��B�O���F�؁i���ݔF�؁j�͍s���Ă��܂���B
		//�����[�_�[���A�J�[�h���^���Ȃ��̂ł���ƔF�肵�Ă��܂�
		//���J�[�h�́A���[�_�[���^���Ȃ��̂ł��邩�͖��m�F�ł�
		IntAuthorized,
		//�����F�؁E�O���F�؁i���ݔF�؁j���s���Ă��܂�
		ExtAuthorized,
	};

	//�ǂݎ��̃p�[�~�b�V����
	enum ReadPermission
	{
		//�F�ؕs�v�œǂݏo����������
		//���O���F�؁AMAC�Ƃ��ɂ����Ă��Ȃ��Ă��悢
		ReadEveryone,
		//�ǂݏo���ɂ͊O���F�؂��K�v
		//���O���F�؂͕K�{�����AMAC�͂����Ă��Ȃ��Ă��悢
		ReadAuthUser,
	};

	//�������݂̃p�[�~�b�V����
	enum WritePermission
	{
		//�������݋֎~
		NotWritable,
		//�F�ؕs�v�ŏ������݂�������
		//���O���F�؁AMAC�Ƃ��ɂ����Ă��Ȃ��Ă��悢
		WriteEveryone,
		//�������݂ɂ͊O���F�؂��K�v
		//���O���F�؂͕K�{�����AMAC�͂����Ă��Ȃ��Ă��悢
		WriteAuthUser,
		//�������݂ɂ͊O���F�؂�MAC���K�v
		WriteAuthUserAndMac,
	};

	//���[�U�[�u���b�N�̖��O
	enum UserBlock
	{
		S_PAD0 = 0,
		S_PAD1 = 1,
		S_PAD2 = 2,
		S_PAD3 = 3,
		S_PAD4 = 4,
		S_PAD5 = 5,
		S_PAD6 = 6,
		S_PAD7 = 7,
		S_PAD8 = 8,
		S_PAD9 = 9,
		S_PAD10 = 10,
		S_PAD11 = 11,
		S_PAD12 = 12,
		S_PAD13 = 13,
		REG = 14,
	};

};

#endif
