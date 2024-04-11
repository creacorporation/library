//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�`�F�b�N�{�b�N�X�n�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MCHECKBOXFAMILY_CPP_COMPILING
#include "mCheckBoxFamily.h"
#include "General/mErrorLogger.h"

mCheckboxFamily::mCheckboxFamily()
{
}

mCheckboxFamily::~mCheckboxFamily()
{
}


mCheckboxFamily::CheckStatus mCheckboxFamily::GetCheck( void )const
{
	LRESULT rc = ::SendMessageW( GetMyHwnd() , BM_GETCHECK , 0 , 0 );
	if( rc == BST_CHECKED )
	{
		//�`�F�b�N����Ă���
		return mCheckboxFamily::CheckStatus::CHECKED;
	}
	else if( rc == BST_UNCHECKED )
	{
		//�`�F�b�N����Ă��Ȃ�
		return mCheckboxFamily::CheckStatus::UNCHECKED;
	}
	else if( rc == BST_INDETERMINATE )
	{
		//�O���[�ȏ�ԁi3�X�e�[�g�p�j
		return mCheckboxFamily::CheckStatus::INDETERMINATE;
	}

	//�Ȃ񂩑z��O�̒l���Ԃ��Ă����ꍇ
	RaiseAssert( g_ErrorLogger , 0 , L"�z��O�̒l���Ԃ�܂���" , rc );
	return mCheckboxFamily::CheckStatus::UNKNOWN;
}

bool mCheckboxFamily::SetCheck( mCheckboxFamily::CheckStatus state )
{
	WPARAM check;
	switch( state )
	{
	case mCheckboxFamily::CheckStatus::CHECKED:
		check = BST_CHECKED;
		break;
	case mCheckboxFamily::CheckStatus::UNCHECKED:
		check = BST_UNCHECKED;
		break;
	case mCheckboxFamily::CheckStatus::INDETERMINATE:
		check = BST_INDETERMINATE;
		break;
	default:
		return false;
	}
	::SendMessageW( GetMyHwnd() , BM_SETCHECK , check , 0 ); 
	return true;
}

