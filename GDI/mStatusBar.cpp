//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�X�e�[�^�X�o�[�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mStatusBar.h"
#include "General/mErrorLogger.h"
#include "General/mInitFile.h"
#include <commctrl.h>

#pragma comment (lib, "comctl32.lib")

mStatusBar::mStatusBar()
{
	ZeroMemory( MyPartsPos , sizeof( MyPartsPos ) );
	MyRightJustifyIndex = 256;
	MyRightUpper = false;
}

mStatusBar::~mStatusBar()
{
}

//�E�C���h�E�N���X�̓o�^������
bool mStatusBar::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	::InitCommonControls();
	return false;	//�V���ȃE�C���h�E�N���X�̓o�^�͂��Ȃ�
}

//�E�C���h�E���J��
bool mStatusBar::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = STATUSCLASSNAMEW;
	retSetting.Style |= CCS_BOTTOM;	//�����ɃX�e�[�^�X�o�[��z�u
	retSetting.ProcedureChange = true;

	//�I�v�V�����̎w�肪�Ȃ���΃R�R�Ŗ߂�
	if( opt == nullptr )
	{
		return true;
	}

	//�I�v�V�����̎w�肪����΂���ɏ]��
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		//�ʏ�̕��@�ł̍쐬�̏ꍇ
		const mStatusBar::Option_UseOption* op = (const mStatusBar::Option_UseOption*)opt;
		retSetting.Style |= ( op->SizeGrip ) ? ( SBARS_SIZEGRIP ) : ( 0 );	//�T�C�Y�ύX�p�̃O���b�v��\���H
		retSetting.Style |= ( op->Tooltip ) ? ( SBARS_TOOLTIPS ) : ( 0 );	//�c�[���`�b�v��\���H
	}
	return true;
}

bool mStatusBar::OnCreate( const void* opt )
{
	//Unicode���[�h�ɂ���
	::SendMessageW( GetMyHwnd() , SB_SETUNICODEFORMAT , TRUE , 0 );

	//�I�v�V�����̎w�肪�Ȃ���΃V���v���X�e�[�^�X�o�[�ɕύX
	if( opt == nullptr )
	{
		::SendMessageW( GetMyHwnd() , SB_SIMPLE , TRUE , 0 );
		return true;
	}

	//�I�v�V�����̎w�肪����΂���ɏ]��
	if( ( (const Option*)opt )->method == Option::CreateMethod::USEOPTION )
	{
		//�ʏ�̍쐬�̏ꍇ
		const mStatusBar::Option_UseOption* op = ( const mStatusBar::Option_UseOption* )opt;

		//�����f�[�^�p��PartsOption���R�s�[
		MyPartsOption = op->Parts;

		//�p�[�c�̍\���𔽉f
		if( MyPartsOption.size() == 0 )
		{
			//�p�[�c�������ꍇ�A�V���v���X�e�[�^�X�o�[�ɕύX
			::SendMessageW( GetMyHwnd() , SB_SIMPLE , TRUE , 0 );
		}
		else
		{
			//�m���V���v���X�e�[�^�X�o�[�ɕύX
			::SendMessageW( GetMyHwnd() , SB_SIMPLE , FALSE , 0 );

			//MyPartsOption�̃T�C�Y���ߏ�ł���ꍇ�̓G���[���L�^���āA����؂�B
			//���G���[�I���͂ł��Ȃ�����L�^�̂�
			if( 255 < MyPartsOption.size() )
			{
				RaiseAssert( g_ErrorLogger , MyPartsOption.size() , L"Too many parts" );
				MyPartsOption.resize( 255 );
			}

			//RightJustifyIndex�̐��������`�F�b�N����
			MyRightJustifyIndex = op->RightJustifyIndex;
			if( (INT)MyPartsOption.size() < MyRightJustifyIndex )
			{
				//�s���Ȃ̂őS�����l�i�l�̕␳�͂��邪�A�����L�Q�ł��Ȃ��̂ŁA�G���[�̋L�^�͂��Ȃ��j
				MyRightJustifyIndex = (INT)MyPartsOption.size();
			}

			//���񂹁A���񂹂̗D��
			MyRightUpper = op->RightUpper;

			//PartsOption�̗v�f���Ɠ����ɂȂ�悤�Ƀp�[�c�𕪊�����
			ModifyPartsSize();

			//�p�[�c�̓��e���X�V
			for( INT i = 0 ; i < (INT)MyPartsOption.size() ; i++ )
			{
				ModifyParts( i );
			}
		}
	}
	return true;
}

bool mStatusBar::ModifyPartsSize( void )
{
	//�T�C�Y��0�̏ꍇ(�V���v���X�e�[�^�X�o�[�ɂȂ��Ă���Ǝv����)�̓G���[��Ԃ�
	if( MyPartsOption.size() == 0 )
	{
		return false;
	}

	//�N���C�A���g�G���A�̕����擾
	RECT client_rect;
	::GetClientRect( GetMyHwnd() , &client_rect );

	//���񂹂̃p�[�c�̈ʒu��ݒ�
	//�������珇�Ԃɕ����m�ۂ��āA�m�ۂł��Ȃ��Ȃ�����c��͑S���d�˂܂��B
	auto LeftJustifier = [this]( UINT position_max ) -> UINT
	{
		UINT position_l = 0;
		for( INT i = 0 ; i < this->MyRightJustifyIndex ; i++ )
		{
			position_l += MyPartsOption[ i ].Width;
			if( position_max < position_l )
			{
				position_l = position_max;
			}
			MyPartsPos[ i ] = position_l;
		}
		return position_l;
	};
	
	//�E�񂹂̃p�[�c�̈ʒu��ݒ�
	//�E�����珇�Ԃɕ����m�ۂ��Ă����A�m�ۂł��Ȃ��Ȃ�����c���S���d�˂܂�
	auto RightJustifier = [this,client_rect]( UINT position_min )-> UINT
	{
		UINT position_r = client_rect.right - client_rect.left;
		for( INT i = (INT)MyPartsOption.size() - 1 ; MyRightJustifyIndex <= i ; i-- )
		{
			MyPartsPos[ i ] = position_r;
			if( MyPartsOption[ i ].Width < position_r )
			{
				position_r -= MyPartsOption[ i ].Width;
			}
			else
			{
				position_r = 0;
			}
			if( position_r < position_min )
			{
				position_r = position_min;
			}
		}
		return position_r;
	};

	//�D�悷�鑤����l��ݒ肷��
	if( MyRightUpper )
	{
		//�E�����D��̏ꍇ�A�E������ݒ肷��
		UINT position_min = RightJustifier( 0 );
		LeftJustifier( position_min );
	}
	else
	{
		//�������D��̏ꍇ�A��������ݒ肷��
		UINT position_max = client_rect.right - client_rect.left;
		position_max = LeftJustifier( position_max );
		RightJustifier( position_max );
	}

	//API�Ă�ŁA�p�[�c�𕪊�����
	if( !::SendMessageW( GetMyHwnd() , SB_SETPARTS , MyPartsOption.size() + 1 , (LPARAM)MyPartsPos ) )
	{
		RaiseAssert( g_ErrorLogger , MyPartsOption.size() , L"SB_SETPARTS failed" );
		return false;
	}
	return true;

}

LRESULT mStatusBar::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( msg == WM_SIZE )
	{
		ModifyPartsSize();
	}
	return __super::WindowProcedure( msg , wparam , lparam );
}

//�S�p�[�c���X�L�������AID����v���镨�ɑ΂��ăR�[���o�b�N���Ăяo��
bool mStatusBar::ScanItem( const WString& id , ScanItemCallback callback )
{
	bool result = true;
	for( INT i = 0 ; i < (INT)MyPartsOption.size() ; i++ )
	{
		if( MyPartsOption[ i ].Id == id )
		{
			result &= callback( i );
		}
	}
	return result;
}

//�w�肵���C���f�b�N�X�̃p�[�c���Đݒ肷��
bool mStatusBar::ModifyParts( INT index )
{
	//�v���p�e�B��ݒ�l�ɕϊ�
	WORD form = 0;
	form |= ( MyPartsOption[ index ].Notab ) ? ( SBT_NOTABPARSING ) : ( 0 );
	switch( MyPartsOption[ index ].Border )
	{
	case BorderType::Popout:
		form |= SBT_POPOUT;
		break;
	case BorderType::NoBorder:
		form |= SBT_NOBORDERS;
		break;
	case BorderType::Normal:
	default:
		break;
	}

	//API�Ă�ŁA�p�[�c�𕪊�����
	if( !::SendMessageW( GetMyHwnd() , SB_SETTEXT , ( form | LOBYTE(index) ) , (LPARAM)MyPartsOption[ index ].Str.c_str() ) )
	{
		RaiseAssert( g_ErrorLogger , index , L"SB_SETTEXT failed" );
		return false;
	}
	return true;
}

//�p�[�c�ɕ������ݒ肷��
bool mStatusBar::SetText( const WString& id , const WString& str , bool notab )
{
	//�R�[���o�b�N�֐��̒�`
	auto SetStringCallback = [ this , str , notab ]( INT index ) -> bool
	{
		//�v���p�e�B�̍X�V
		MyPartsOption[ index ].Notab = notab;
		MyPartsOption[ index ].Str = str;

		return ModifyParts( index );
	};

	//ID���������A������̍X�V������
	return ScanItem( id , SetStringCallback );
}

//�{�[�_�[���C���̎�ނ�ύX����
bool mStatusBar::SetBorder( const WString& id , BorderType border )
{
	//�R�[���o�b�N�֐��̒�`
	auto SetBorderCallback = [ this , border ]( INT index ) -> bool
	{
		//�v���p�e�B�̍X�V
		MyPartsOption[ index ].Border = border;

		return ModifyParts( index );
	};

	//ID���������A������̍X�V������
	return ScanItem( id , SetBorderCallback );
}

//����ύX����
bool mStatusBar::SetWidth( const WString& id , UINT width )
{
	//�R�[���o�b�N�֐��̒�`
	auto SetWidthCallback = [ this , width ]( INT index ) -> bool
	{
		//�v���p�e�B�̍X�V
		MyPartsOption[ index ].Width = width;

		return ModifyParts( index );
	};

	//ID���������A������̍X�V������
	return ScanItem( id , SetWidthCallback );
}
