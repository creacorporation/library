//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�O���[�o���ȃE�C���h�E�֐��j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGLOBALWINDOWFUNC_CPP_COMPILING
#include "mGlobalWindowFunc.h"
#include "General/mErrorLogger.h"

//�ÓI�ȃ����o�ϐ��̒�`
mGlobalWindowFunc::HandleObjMap mGlobalWindowFunc::MyHandleObjMap;

LRESULT __stdcall mGlobalWindowFunc::MessageProcedure( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam )
{
	//hwnd���L�[�Ɍ������āA���������I�u�W�F�N�g�Ƀ��b�Z�[�W������������B
	//������Ȃ������ꍇ�̓f�t�H���g�N�ɏ���������B

	HandleObjMap::iterator itr = MyHandleObjMap.find( hwnd );
	if( itr == MyHandleObjMap.end() )
	{
		//�s���ȃI�u�W�F�N�g
		return DefWindowProcW( hwnd , msg , wparam , lparam );
	}
	else
	{
		//�Y���I�u�W�F�N�g����
		return itr->second->WindowProcedure( msg , wparam , lparam );
	}
}

bool mGlobalWindowFunc::Attach( const mGlobalWindowFunc::AttachAccessPermission& , HWND hwnd , mWindow* win )
{
	//���łɓo�^�ς݁H
	if( MyHandleObjMap.count( hwnd ) )
	{
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)hwnd , L"Duplicate attach" );
		return false;
	}
	//�o�^
	MyHandleObjMap.insert( HandleObjMap::value_type( hwnd , win ) );
	return true;
}

mWindow* mGlobalWindowFunc::Query( const mGlobalWindowFunc::AttachAccessPermission& , HWND hwnd )
{
	HandleObjMap::iterator itr = MyHandleObjMap.find( hwnd );
	if( itr == MyHandleObjMap.end() )
	{
		//�s���ȃI�u�W�F�N�g
		return nullptr;
	}
	else
	{
		//�Y���I�u�W�F�N�g����
		return itr->second;
	}
}

bool mGlobalWindowFunc::Detach( const mGlobalWindowFunc::DetachAccessPermission& , HWND hwnd , mWindow* win )
{
	//�o�^����H
	HandleObjMap::iterator itr = MyHandleObjMap.find( hwnd );
	if( itr == MyHandleObjMap.end() )
	{
		//�o�^�Ȃ�
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)hwnd , L"Object not found" );
		return false;
	}
	if( ( itr->first != hwnd ) || ( itr->second != win ) )
	{
		//�o�^����Ă�����̂ƁA�폜�������̂��قȂ�ꍇ�B
		//���̏ꍇ�ł��A���̊֐���Ԃ���win���w���Ă���A�h���X����������\������Ȃ̂ŁA
		//�G���[�I�������ɁA���̂܂܍폜���Ă��܂��B��ʂ͕���邩������Ȃ����B
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)hwnd , L"HWND/mWindow mismatch" );
	}

	//�폜
	MyHandleObjMap.erase( hwnd );
	return true;
}

