//----------------------------------------------------------------------------
// Microsoft Excel�n���h��
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MEXCELCELLREF_H_INCLUDED
#define MEXCELCELLREF_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mExcelCellRef
{
public:
	mExcelCellRef();
	virtual ~mExcelCellRef();
	mExcelCellRef( const mExcelCellRef& src );
	const mExcelCellRef& operator=( const mExcelCellRef& src );

	//�G�N�Z���͈͎̔w����
	struct Position
	{
		//�s�S�̂��w���Ă��邩�H
		//���s�E�񗼕���S�̎w�肷�邱�Ƃ͂ł��܂���B
		//�@�i�w���[�N�V�[�g�S�́x���w�肷�邱�Ƃ͂ł��܂���j
		bool WholeRow;

		//��S�̂��w���Ă��邩�H
		//���s�E�񗼕���S�̎w�肷�邱�Ƃ͂ł��܂���B
		//�@�i�w���[�N�V�[�g�S�́x���w�肷�邱�Ƃ͂ł��܂���j
		bool WholeCol;

		//�s�͐�Ύw��H  �^�Ȃ��ΎQ�ƁA�U�Ȃ瑊�ΎQ��
		// ��ΎQ��		$a$1	R1C1
		// ���ΎQ��		a1		R[1]C[-1]
		bool AbsRow;

		//��͐�Ύw��H  �^�Ȃ��ΎQ�ƁA�U�Ȃ瑊�ΎQ��
		// ��ΎQ��		$a$1	R1C1
		// ���ΎQ��		a1		R[1]C[-1]
		bool AbsCol;

		//�s�ԍ�
		// A1�`���̏ꍇ�AR1C1�`���Ő�ΎQ�Ƃ̏ꍇ��1�J�n
		// R1C1�`���ő��ΎQ�Ƃ̏ꍇ��0�ȉ���OK
		INT Row;

		//��ԍ�
		// A1�`���̏ꍇ�AR1C1�`���Ő�ΎQ�Ƃ̏ꍇ��1�J�n
		// R1C1�`���ő��ΎQ�Ƃ̏ꍇ��0�ȉ���OK
		INT Col;

		//�f�t�H���g�ŏ�����
		//���ǂ̃Z�����w���Ă��Ȃ���ԂɂȂ�܂�
		Position()
		{
			Clear();
		}

		//������
		//whole_row	�s�S�́H
		//whole_col	��S�́H
		//abs_row	�s�͐�Ύw��H  �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
		//abs_col	��͐�Ύw��H  �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
		//row		�s�ԍ�(1�J�n)
		//col		��ԍ�(1�J�n)
		Position( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , INT col )
		{
			Set( whole_row , whole_col , abs_row , abs_col , row , col );
		}

		//������
		//whole_row	�s�S�́H
		//whole_col	��S�́H
		//abs_row	�s�͐�Ύw��H  �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
		//abs_col	��͐�Ύw��H  �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
		//row		�s�ԍ�(1�J�n)
		//col		��ԍ�(A1�`���̃A���t�@�x�b�g)
		Position( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , const WString& col );

		//�L���ȃZ�����w���Ă����Ԃ�
		inline bool IsValid( void )const
		{
			return !WholeRow || !WholeCol;
		}

		//�ʒu�����N���A���܂�
		//���ǂ̃Z�����w���Ă��Ȃ���ԂɂȂ�܂�
		inline void Clear( void )
		{
			WholeRow = true;
			WholeCol = true;
			Row = 0;
			Col = 0;
			AbsRow = false;
			AbsCol = false;
		}

		//�͈͂��w�肵�܂�
		//whole_row	�s�S�́H
		//whole_col	��S�́H
		//abs_row	�s�͐�Ύw��H  �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
		//abs_col	��͐�Ύw��H  �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
		//row		�s�ԍ�(1�J�n)
		//col		��ԍ�(1�J�n)
		inline void Set( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , INT col )
		{
			WholeRow = whole_row;
			WholeCol = whole_col;
			Row = row;
			Col = col;
			AbsRow = abs_row;
			AbsCol = abs_col;
		}
	};

	//�w���1�̃Z���A�s�S�́A��S�̂��w���悤�ɃI�u�W�F�N�g�����������܂�
	// src : �w�肵�����Z��
	mExcelCellRef( const Position& src );

	//�w���2�̃Z���A�s�S�́A��S�̂ɂ��͈͂��w�肳���悤�ɃI�u�W�F�N�g�����������܂�
	// src1 : �w�肵�����͈͂̑Ίp�����Ȃ�����̃Z��
	// src2 : �w�肵�����͈͂̑Ίp�����Ȃ���������̃Z��
	mExcelCellRef( const Position& src1 , const Position& src2 );

	//�w���1�̃Z���A�s�S�́A��S�̂��w���悤�ɃI�u�W�F�N�g���Z�b�g���܂�
	// src : �w�肵�����Z��
	// ret : �������^
	bool SetPosition( const Position& pos );

	//�w���2�̃Z���A�s�S�́A��S�̂ɂ��͈͂��w�肳���悤�ɃI�u�W�F�N�g���Z�b�g���܂�
	// src1 : �w�肵�����͈͂̑Ίp�����Ȃ�����̃Z��
	// src2 : �w�肵�����͈͂̑Ίp�����Ȃ���������̃Z��
	// ret : �������^
	bool SetPosition( const Position& pos1 , const Position& pos2 );

	//�ێ����Ă���ʒu��Ԃ�
	// retPos1 : �͈͂̈���̃Z��(�s�v�Ȃ�nullptr��)
	// retPos2 : �͈͂̂�������̃Z��(�s�v�Ȃ�nullptr��)
	// ret : �������^
	bool GetPosition( Position* retPos1 , Position* retPos2 );

	//�ێ����Ă���ʒu��Ԃ�
	// ret : �������^
	const Position& GetPos1( void )const;

	//�ێ����Ă���ʒu��Ԃ�
	// ret : �������^
	const Position& GetPos2( void )const;

	//�ێ����Ă���ʒu�A�͈͂𕶎���ŕԂ�
	// IsRC : true : R1C1�`��
	//        false : A1�`��
	// ret : �������^
	const WString& GetAddress( bool IsRC )const;

	//�͈͂̍s����Ԃ�
	//pos : ��ʒu
	//	R1C1�`���ŁA��ΎQ�ƂƑ��ΎQ�Ƃ����݂���ꍇ�A���ΎQ�Ƃ���Ƃ���ʒu�ɂ���Č��ʂ��ς��܂�
	//	���̂悤�ȏꍇ�́A��Ƃ���ʒu���w�肵�ĉ������B����ȊO�̏ꍇ�͌��ʂ��ς��Ȃ��̂Ŏw��s�v�ł��B
	//	nullptr���w�肵���ꍇ�́Apos1(�w��͈͂̍���)����ɂȂ�܂��B
	//ret : �͈͂Ɋ܂܂��s���B�G���[�̂Ƃ��O�B
	DWORD GetRowCount( const Position* pos = nullptr )const;

	//�͈̗͂񐔂�Ԃ�
	//pos : ��ʒu
	//	R1C1�`���ŁA��ΎQ�ƂƑ��ΎQ�Ƃ����݂���ꍇ�A���ΎQ�Ƃ���Ƃ���ʒu�ɂ���Č��ʂ��ς��܂�
	//	���̂悤�ȏꍇ�́A��Ƃ���ʒu���w�肵�ĉ������B����ȊO�̏ꍇ�͌��ʂ��ς��Ȃ��̂Ŏw��s�v�ł��B
	//	nullptr���w�肵���ꍇ�́Apos1(�w��͈͂̍���)����ɂȂ�܂��B
	//ret : �͈͂Ɋ܂܂��s���B�G���[�̂Ƃ��O�B
	DWORD GetColCount( const Position* pos = nullptr )const;

	//�����w���Ă��Ȃ���Ԃɂ���
	void Clear( void );

	//�L���ȃZ�����w���Ă��邩�H
	inline bool IsValid( void )const
	{
		return MyPos1.IsValid();
	}

protected:

	Position MyPos1;
	Position MyPos2;

	mutable WString MyAddress;

protected:

	bool UpdateAddress( bool IsRC )const;

	//�G�N�Z���̃��[�N�V�[�g��̈ʒu����Z�����̂𐶐����܂�
	//��F��3�A�s2�Ȃ�R3C2�Ƃ�C2�Ƃ���Ԃ��܂�
	// retCell : ���������Z���̖��O
	// IsRC    : �^�Ȃ�R1C1�`���A�U�Ȃ��ΎQ��
	// IsAbs   : �^�Ȃ��ΎQ��($a$1)�A�U�Ȃ瑊�ΎQ��(a1)
	// row     : �s�ԍ�(1����J�n �G�N�Z����row()�Ɠ����l)
	// col     : ��ԍ�(1����J�n �G�N�Z����col()�Ɠ����l)
	// norow   : �s�͎w�肵�Ȃ��i���̏ꍇrow�͖�������܂��j��S�̂̎w��p
	// nocol   : ��͎w�肵�Ȃ��i���̏ꍇcol�͖�������܂��j�s�S�̂̎w��p
	// ret     : �����ł�����^�A���s������U
	//           ��A1�`����row��-1�Ƃ��w�肵���肷��Ǝ��s���܂�
	static bool BuildCellName( WString& retCell , bool IsRC , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow = false , bool nocol = false );
	//�Z�����̂𐶐�����i�������j
	static bool BuildCellNameA1( WString& retCell , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow , bool nocol );	//A1�`���p
	static bool BuildCellNameRC( WString& retCell , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow , bool nocol );	//R1C1�`���p

};



#endif

