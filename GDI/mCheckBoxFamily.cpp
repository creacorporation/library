//----------------------------------------------------------------------------
// ウインドウ管理（チェックボックス系）
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
		//チェックされている
		return mCheckboxFamily::CheckStatus::CHECKED;
	}
	else if( rc == BST_UNCHECKED )
	{
		//チェックされていない
		return mCheckboxFamily::CheckStatus::UNCHECKED;
	}
	else if( rc == BST_INDETERMINATE )
	{
		//グレーな状態（3ステート用）
		return mCheckboxFamily::CheckStatus::INDETERMINATE;
	}

	//なんか想定外の値が返ってきた場合
	RaiseAssert( g_ErrorLogger , 0 , L"想定外の値が返りました" , rc );
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

