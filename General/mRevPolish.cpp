//----------------------------------------------------------------------------
// �t�|�[�����h�ϊ��N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------


#define MREVPOLISH_CPP_COMPILING
#include "mRevPolish.h"

using namespace mRevPolish;

//���̃g�[�N�������o��
// src : ���̕�����
// index : �g�[�N���̐擪�ʒu
// retExpr : �ǂݎ�����g�[�N��
// retNext : ���̃g�[�N���̐擪�ʒu
// ret : �g�[�N����ǂݎ�����ꍇtrue
//       �����g�[�N�����Ȃ��ꍇfalse
static bool NextToken( const WString& src , size_t index , WString& retExpr , size_t& retNext )
{
	WCHAR c = L' ';

	//�󔒂�ǂݔ�΂�
	while( index < src.size() )
	{
		c = src[ index ];
		if( !wchar_isblank( c ) )
		{
			break;
		}
		index++;
	}

	//���ʂ����Z�b�g
	retExpr = L"";

	//�ŏ��̕��������ł��邩�ɂ��g�[�N���𔻕�
	if( wchar_isblank( c ) )
	{
		//�󔒁������g�[�N���͂Ȃ�
		return false;
	}
	else if( wchar_isdigit( c ) )
	{
		//���������l
		bool is_hex = false;		//16�i���H
		bool is_float = false;		//���������_�H

		for( ; index < src.size() ; index++ )
		{
			c = src[ index ];
			if( !wchar_isdigit( c ) )
			{
				if( is_float )
				{
					break;
				}
				else if( is_hex && !wchar_isxdigit( c ) )
				{
					break;
				}
				else if( c == L'x' && retExpr == L"0" )
				{
					is_hex = true;
				}
				else if( c == L'.' )
				{
					is_float = true;
				}
				else
				{
					break;
				}
			}
			retExpr += c;
		}
		retNext = index;

		if( src.back() == L'.' || src.back() == L'x' )
		{
			//�����_��0x�ŏI����Ă���ꍇ(�G���[)
			retExpr = L"";
			return true;
		}
		return true;
	}
	else if( wchar_isalpha( c ) || c == L'@' || c == L'$' )
	{
		//�A���t�@�x�b�g�Ɓ����ϐ���
		for( ; index < src.size() ; index++ )
		{
			c = src[ index ];
			if( !wchar_isalnum( c ) )
			{
				break;
			}
			retExpr += c;
		}
		retNext = index;
		return true;
	}
	else
	{
		//���̑������Z�q�H
		switch( c )
		{
		case L'+': case L'-': case L'*': case L'/':
		case L'(': case L')':
			retExpr = c;
			retNext = index + 1;
			return true;

		default:
			retExpr = L"";
			retNext = index + 1;
			return true;
		}
	}
}

//���Z�q�̗D�揇�ʂ�Ԃ�
// oper : �D�揇�ʂ��m�F�������I�y���[�^�[
// ret : �D�揇�ʁi�D�揇�ʂ��������قǐ������傫���j
//       �G���[�̎�0
static INT QueryOperatorPriority( ExprKind oper )
{
	switch( oper )
	{
	case ExprKind::OPERATOR_PLUS:		//���Z�q�{
		return 2;
	case ExprKind::OPERATOR_MINUS:		//���Z�q�|
		return 1;
	case ExprKind::OPERATOR_MULTIPLY:	//���Z�q�~
		return 4;
	case ExprKind::OPERATOR_DIVIDE:	//���Z�q��
		return 3;
	default:
		break;
	}
	return 0;
}

bool mRevPolish::Convert( const WString& src , Expr& retPolish )
{
	//���łɊi�[����Ă��錋�ʂ�����Δj��
	retPolish.clear();

	//�e���|�����ϐ�����
	size_t index = 0;		//���݂̃g�[�N���̊J�n�ʒu
	size_t next_index = 0;	//���̃g�[�N���̊J�n�ʒu
	WString NextExpr;		//���̃g�[�N��
	Expr Stack;				//�t�|�[�����h�L�@�ϊ��p�̃X�^�b�N
	WString ErrorStr;		//�G���[�������̕���
	bool is_unary = true;	//�P�����Z�q�o���̃^�C�~���O���H

	//�����ʂ̃g�[�N���������ꍇ�̏���
	auto PopStack = [&retPolish,&Stack]() -> bool 
	{
		while( Stack.size() )
		{
			ExprEntry entry = Stack.back();
			Stack.pop_back();
			if( entry.variable == L"(" )
			{
				return true;
			}
			retPolish.push_back( entry );
		}
		//���ʂ��Ή����Ă��Ȃ��i�G���[�j
		return false;
	};

	//���Z�q�̕]�������鏈��
	auto ExprPriority = [&retPolish,&Stack]( const WString& Next ) -> bool
	{
		//����̃g�[�N���ɑ΂���G���g���ƗD�揇�ʂ𐶐�����
		ExprEntry current_entry;
		current_entry.variable = Next;
		if( Next== L"+" )
		{
			current_entry.kind = ExprKind::OPERATOR_PLUS;
		}
		else if( Next == L"-" )
		{
			current_entry.kind = ExprKind::OPERATOR_MINUS;
		}
		else if( Next == L"*" )
		{
			current_entry.kind = ExprKind::OPERATOR_MULTIPLY;
		}
		else if( Next == L"/" )
		{
			current_entry.kind = ExprKind::OPERATOR_DIVIDE;
		}
		else
		{
			return false;
		}
		INT current_priority = QueryOperatorPriority( current_entry.kind );

		//�X�^�b�N���m�F���āA�D�揇�ʂ̒Ⴂ���̂�f���o��
		while( !Stack.empty() )
		{
			if( current_priority < QueryOperatorPriority( Stack.back().kind ) )
			{
				retPolish.push_back( Stack.back() );
				Stack.pop_back();
			}
			else
			{
				break;
			}
		}

		//����̃g�[�N�����X�^�b�N�ɐς�
		Stack.push_back( current_entry );
		return true;
	};

	//���C�����[�`��
	while( NextToken( src , index , NextExpr , next_index ) )
	{
		if( NextExpr == L"" )
		{
			//�ǂݎ��Ȃ��g�[�N��
			sprintf( ErrorStr , L"��'%ls'�ɕs���ȃg�[�N��������܂�" , src.c_str() );
			goto onerror;
		}
		else if( wchar_isdigit( NextExpr[ 0 ] ) )
		{
			//���l
			ExprEntry entry;
			entry.variable = NextExpr;
			if( NextExpr.find( L'.' ) == WString::npos )
			{
				//�����_���Ȃ��̂Ő���
				entry.kind = ExprKind::VALUE_INTEGER;
			}
			else
			{
				//�����_������̂ŕ�������
				entry.kind = ExprKind::VALUE_FLOAT;
			}
			retPolish.push_back( entry );

			is_unary = false;
		}
		else if( wchar_isalpha( NextExpr[ 0 ] ) )
		{
			//�ϐ���
			ExprEntry entry;
			entry.variable = NextExpr;
			entry.kind = ExprKind::VALUE_VARIABLE;
			retPolish.push_back( entry );

			is_unary = false;
		}
		else if( NextExpr[ 0 ] == L')' )
		{
			//������
			if( is_unary )
			{
				sprintf( ErrorStr , L"��'%ls'�ɂ����āA����')'�̑O�ɓ񍀉��Z�q���s�����Ă��܂�" , src.c_str() );
				goto onerror;
			}
			if( !PopStack() )
			{
				sprintf( ErrorStr , L"��'%ls'�ɂ����Ċ���'('�̐�������܂���" , src.c_str() );
				goto onerror;
			}

			is_unary = false;
		}
		else if( NextExpr[ 0 ] == L'(' )
		{
			//�J������
			if( !is_unary )
			{
				sprintf( ErrorStr , L"��'%ls'�ɂ����āA����'('�̑O�ɓ񍀉��Z�q���s�����Ă��܂�" , src.c_str() );
				goto onerror;
			}
			ExprEntry entry;
			entry.kind = ExprKind::VALUE_VARIABLE;
			entry.variable = L'(';
			Stack.push_back( entry );

			is_unary = true;
		}
		else
		{
			//���̑����Z�q
			if( is_unary )
			{
				//�P�����Z�q�̏ꍇ
				if( NextExpr== L"+" )
				{
					//�{�����͖�������
					;
				}
				else if( NextExpr == L"-" )
				{
					//�|�����͎��Ɂ|�P���悶��
					ExprEntry entry;
					entry.kind = ExprKind::VALUE_INTEGER;
					entry.variable = L"-1";
					retPolish.push_back( entry );

					entry.kind = ExprKind::OPERATOR_MULTIPLY;
					entry.variable = L"*";
					Stack.push_back( entry );
				}
				else
				{
					//����ȊO�͖��Ή�
					sprintf( ErrorStr , L"��'%ls'�ɂ����ĕs���ȒP�����Z�q������܂�" , src.c_str() );
					goto onerror;
				}
			}
			else
			{
				//�񍀉��Z�q�̏ꍇ
				if( !ExprPriority( NextExpr ) )
				{
					sprintf( ErrorStr , L"��'%ls'�ɂ����ĉ��Z�q�̏��������s���܂���" , src.c_str() );
					goto onerror;
				}
			}
			is_unary = true;
		}
		index = next_index;
	}

	//�S���̃g�[�N�����X�^�b�N����ړ�����
	while( !Stack.empty() )
	{
		ExprEntry entry = Stack.back();
		Stack.pop_back();
		if( entry.variable == L"(" )
		{
			sprintf( ErrorStr , L"��'%ls'�ɂ����Ċ���')'�̐�������܂���" , src.c_str() );
			goto onerror;
		}
		retPolish.push_back( entry );
	}
	return true;

onerror:
	RaiseError( g_ErrorLogger , 0 , ErrorStr.c_str() , index );
	return false;
}
