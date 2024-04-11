//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i1�̃I�u�W�F�N�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MWINDOW_CPP_COMPILING
#include "mWindow.h"
#include "mWindowCollection.h"
#include "mGlobalWindowFunc.h"
#include "General/mErrorLogger.h"


mWindow::mWindow()
{
	MyChild = nullptr;
	MyMenu = nullptr;
	MyWindowClass = L"";
	MyIsWindowClassOriginal = false;
	MyHwnd = 0;
	MyParent = 0;
	MyRoot = 0;
	MyDefWndproc = nullptr;
}

mWindow::~mWindow()
{
	//�q�E�C���h�E��j������
	mDelete MyChild;
	MyChild = nullptr;
	mDelete MyMenu;
	MyMenu = nullptr;

	//�O���[�o���ȃE�C���h�E�t�@���N�V�������玩����o�^��������
	if( !mGlobalWindowFunc::Detach( mGlobalWindowFunc::DetachAccessPermission() , GetMyHwnd() , this ) )
	{
		//�폜�ł��Ȃ��B���ł��B
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)GetMyHwnd() , MyWindowClass );
	}

	//�����œo�^�����E�C���h�E�N���X�ł���ꍇ�̓E�C���h�E�N���X�̍폜�����݂�
	if( MyIsWindowClassOriginal )
	{
		//�����E�C���h�E�N���X���g���Ă���E�C���h�E�����ɂ����邩������Ȃ��̂�
		//�G���[�ɂȂ��Ă��s��ɂ���B
		UnregisterClassW( MyWindowClass.c_str() , GetModuleHandleW( 0 ) );
	}
}

LRESULT mWindow::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( MyDefWndproc != nullptr )
	{
		//�E�C���h�E�v���V�[�W�����u���������Ă���ꍇ�́A�u�������O�̂ɏ���������
		return CallWindowProcW( MyDefWndproc , GetMyHwnd() , msg , wparam , lparam );
	}
	else
	{
		//�E�C���h�E�v���V�[�W�����u���������Ă��Ȃ��ꍇ�́A�f�t�H���g�N�ɔC����B
		return DefWindowProcW( GetMyHwnd() , msg , wparam , lparam );
	}
}

bool mWindow::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	return false;
}

//�E�C���h�E���������������ꍇ�ɃR�[�������
bool mWindow::OnCreate( const void* opt )
{
	return true;
}

//�E�C���h�E���j������悤�Ƃ��Ă���Ƃ��R�[�������
void mWindow::OnDestroy( void )
{
	return;
}

//�������g�̃E�C���h�E�n���h�����擾����
HWND mWindow::GetMyHwnd( void )const
{
	return MyHwnd;
}

//�e�E�C���h�E�̃n���h�����擾����
HWND mWindow::GetMyParent( void )const
{
	return MyParent;
}

//�ŏ�ʃE�C���h�E�̃n���h�����擾����
HWND mWindow::GetMyRoot( void )const
{
	return MyRoot;
}

//�E�C���h�E�̈ʒu���Z�b�g
bool mWindow::SetWindowPosition( const WindowPosition& newpos )
{
	MyPosition = newpos;
	return true;
}

void mWindow::MoveWindowPosition( const RECT& pos )
{
	::SetWindowPos( GetMyHwnd() , nullptr , pos.left , pos.top , pos.right-pos.left , pos.bottom-pos.top , 0 );
}

//�E�C���h�E�̉�/�s����ύX����
bool mWindow::SetVisible( bool newstate )
{
	int cmd = ( newstate ) ? ( SW_SHOW ) : ( SW_HIDE );
	::ShowWindow( GetMyHwnd() , cmd );
	return true;
}

//�E�C���h�E�̗L��/������ύX����
bool mWindow::SetEnable( bool newstate )
{
	::EnableWindow( GetMyHwnd() , newstate );
	return true;
}

//�E�C���h�E�̈ʒu���X�N���[�����W�œ���
RECT mWindow::GetRect( void ) const
{
	RECT rect;
	if( !::GetWindowRect( GetMyHwnd() , &rect ) )
	{
		rect.left = 0;
		rect.right = 0;
		rect.top = 0;
		rect.bottom = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetRect failed" );
	}
	return rect;
}

//�E�C���h�E�̃T�C�Y�𓾂�
SIZE mWindow::GetSize( void ) const
{
	RECT rect;
	SIZE size;
	if( !::GetWindowRect( GetMyHwnd() , &rect ) )
	{
		size.cx = 0;
		size.cy = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetSize failed" );
	}
	else
	{
		size.cx = rect.right - rect.left;
		size.cy = rect.bottom - rect.top;
	}
	return size;
}

//���̃E�C���h�E�̃N���C�A���g���W���X�N���[�����W�ɕϊ�����
POINT mWindow::Client2Screen( const POINT& client_pos ) const
{
	POINT point = client_pos;
	if( !::ClientToScreen( GetMyHwnd() , &point ) )
	{
		point.x = 0;
		point.y = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetRect failed" );
	}
	return point;
}

//���̃E�C���h�E�̃N���C�A���g���W���X�N���[�����W�ɕϊ�����
POINT mWindow::Client2Screen( INT x , INT y ) const
{
	POINT point;
	point.x = x;
	point.y = y;
	return Client2Screen( point );
}

//�X�N���[�����W�����̃E�C���h�E�̃N���C�A���g���W�ɕϊ�����
POINT mWindow::Screen2Client( const POINT& client_pos ) const
{
	POINT point = client_pos;
	if( !::ScreenToClient( GetMyHwnd() , &point ) )
	{
		point.x = 0;
		point.y = 0;
		RaiseAssert( g_ErrorLogger , 0 , L"GetRect failed" );
	}
	return point;
}

//�X�N���[�����W�����̃E�C���h�E�̃N���C�A���g���W�ɕϊ�����
POINT mWindow::Screen2Client( INT x , INT y ) const
{
	POINT point;
	point.x = x;
	point.y = y;
	return Screen2Client( point );
}

bool mWindow::SetFocus( void )const
{
	return ::SetFocus( GetMyHwnd() );
}

