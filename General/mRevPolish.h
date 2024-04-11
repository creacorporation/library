//----------------------------------------------------------------------------
// �t�|�[�����h�ϊ��N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MREVPOLISH_H_INCLUDED
#define MREVPOLISH_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include <General/mErrorLogger.h>
#include <unordered_map>
#include <deque>

namespace mRevPolish
{
	enum ExprKind
	{
		OPERATOR_PLUS,		//���Z�q�{
		OPERATOR_MINUS,		//���Z�q�|
		OPERATOR_MULTIPLY,	//���Z�q�~
		OPERATOR_DIVIDE,	//���Z�q��
		VALUE_INTEGER,		//�����l�̒l
		VALUE_FLOAT,		//���������_�̒l
		VALUE_VARIABLE,		//�ϐ��l
	};

	struct ExprEntry
	{
		//�g�[�N���̎��
		ExprKind kind;			//�g�[�N���̎��

		//�g�[�N���̒l
		//���l�̏ꍇ�́A���l���e�L�X�g�ŕ\������������
		//�ϐ����̏ꍇ�́A�ϐ���
		//���Z�q�̏ꍇ�́A���Z�q���e�L�X�g�ŕ\������������
		WString variable;

		operator int()const
		{
			int val = 0;
			wchar_sscanf( variable.c_str() , L"%d" , &val );
			return val;
		}
		operator unsigned int()const
		{
			unsigned int val = 0;
			wchar_sscanf( variable.c_str() , L"%u" , &val );
			return val;
		}
		operator double()const
		{
			double val = 0.0;
			wchar_sscanf( variable.c_str() , L"%lf" , &val );
			return val;
		}
		operator float()const
		{
			float val = 0.0f;
			wchar_sscanf( variable.c_str() , L"%f" , &val );
			return val;
		}

	};

	//�t�|�[�����h�L�@�ւ̕ϊ����ʊi�[��
	typedef std::deque<ExprEntry> Expr;

	//���ԋL�@�̎����t�|�[�����h�L�@�ɕϊ�����
	// src : ���ԋL�@�̎�
	// retPolish : �t�|�[�����h�L�@�ɕϊ�������
	// ret : ����ɕϊ��o�����ꍇtrue
	//       �v�Z���ɖ�肪����(���ʂ��Ή����Ă��Ȃ��Ȃ�)�ꍇfalse
	bool Convert( const WString& src , Expr& retPolish );

}

#endif





