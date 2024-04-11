//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�q�E�C���h�E�Ǘ��j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MWINDOWCOLLECTION_CPP_COMPILING
#include "mWindowCollection.h"
#include "mGlobalWindowFunc.h"
#include "General/mErrorLogger.h"

mWindowCollection::mWindowCollection( mWindow* parent )
	: MyParent( parent )
{
}

mWindowCollection::~mWindowCollection()
{
	//�q�I�u�W�F�N�g��S���j������
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		itr->second->OnDestroy();
		mDelete itr->second;
	}
}

mWindow* mWindowCollection::AddControlInternal( mWindowFactory factory , const WString& id , const void* opt )
{
	//�g�p�s�\��ID�łȂ������m�F
	//���łɑ��݂���ID�ł͂Ȃ����AID���󕶎���ł͂Ȃ������m�F����
	if( id == L"" || MyIdMap.count( id ) )
	{
		//�g�p�s�\��ID������
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"ID���g�p�s�\�ł�" + id );
		return nullptr;
	}

	//�C���X�^���X�̃n���h�����擾
	HINSTANCE instance = GetModuleHandleW( 0 );

	//�E�C���h�E�̃C���X�^���X���쐬����
	mWindow* win = factory( opt );
	if( win == nullptr )
	{
		//�t�@�N�g�����\�b�h�����s����
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"�t�@�N�g�����\�b�h�����s���܂���" );
		return nullptr;
	}

	//�E�C���h�E�N���X�̓o�^
	mWindow::WindowClassSetting wndclass;
	bool need_wndcls_register = win->WindowClassSettingCallback( wndclass , opt );
	if( need_wndcls_register )
	{
		//�o�^������ꍇ
		WNDCLASSEXW wc;
		wc.cbSize = sizeof( WNDCLASSEXW );				//�\���̂̃T�C�Y
		wc.style = wndclass.Style;						//�X�^�C���B
		wc.lpfnWndProc = mGlobalWindowFunc::MessageProcedure;	//�E�C���h�E�v���V�[�W��
		wc.cbClsExtra = 0;								//����0
		wc.cbWndExtra = 0;								//����0
		wc.hInstance = instance;						//�C���X�^���X�n���h��
		wc.hIcon = wndclass.Icon;						//�E�C���h�E�̃A�C�R���B��ŕς����B
		wc.hCursor = wndclass.Cursor;					//�J�[�\���B��ŕς����B
		wc.hbrBackground = wndclass.Background;			//�o�b�N�O���E���h�̃u���V�B��ŕς����B
		wc.lpszMenuName = nullptr;						//���j���[�̖��O�B���(ry
		wc.lpszClassName = wndclass.ClassName.c_str();	//�N���X��
		wc.hIconSm = wndclass.IconSmall;				//���������̃A�C�R���B���(ry

		ATOM atom = ::RegisterClassExW( &wc );
		if( atom == 0 )
		{
			//�o�^���s�������A���̗��R�́H
			DWORD err_code = GetLastError();
			//���łɓo�^�ς݂ł��������Ȃ�
			//���̑��Ȃ񂩂̃G���[�ł������G���[
			if( err_code != ERROR_CLASS_ALREADY_EXISTS )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�E�C���h�E�N���X�̓o�^���ł��܂���" );
				mDelete win;
				return nullptr;
			}
		}
	}

	//�E�C���h�E�N���X�̓o�^���ł�������A���̓E�C���h�E�̐������s��
	mWindow::CreateWindowSetting wndsetting;
	wndsetting.ClassName = wndclass.ClassName;
	bool window_create = win->CreateWindowCallback( wndsetting , opt );

	//�e�I�u�W�F�N�g�����݂��邩�ǂ�����WS_CHILD���Z�b�g��������������肷��
	if( MyParent == nullptr )
	{
		wndsetting.Style &= ~WS_CHILD;	//�e�E�C���h�E�͂Ȃ�
	}
	else
	{
		wndsetting.Style |= WS_CHILD;	//�e�E�C���h�E����
	}

	if( window_create )
	{
		//�E�C���h�E����
		HWND hwnd = ::CreateWindowExW(
			wndsetting.ExStyle,				//�g���X�^�C��
			wndsetting.ClassName.c_str(),	//�o�^����Ă���N���X��
			wndsetting.WindowName.c_str(),	//�L���v�V����
			wndsetting.Style ,				//�X�^�C��
			wndsetting.x ,					//X���W
			wndsetting.y ,					//Y���W
			wndsetting.Width ,				//��
			wndsetting.Height ,				//����
			( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyHwnd ),		//�e�E�B���h�E
			nullptr ,						//���j���[�n���h��
			instance ,						//�C���X�^���X�̃n���h��
			0 );							//�E�B���h�E�쐬�f�[�^(MDI) or �C�ӂ̃f�[�^

		//�ł����������E�C���h�E�̃v���C�x�[�g�����o�������Ă��
		win->MyIsWindowClassOriginal = need_wndcls_register;
		win->MyWindowClass = wndsetting.ClassName;
		win->MyParent = ( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyHwnd );
		win->MyRoot = ( MyParent == nullptr ) ? ( hwnd ) : ( MyParent->MyRoot );
		win->MyHwnd = hwnd;
		win->MyDefWndproc = nullptr;

		//�Ƃ���ŁACreateWindow�������Ă��񂾂����H
		if( hwnd == nullptr )
		{
			//���s���Ă���̂Ŕj������
			RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"�E�C���h�E���J���܂���" + wndsetting.WindowName );
			mDelete win;
			return nullptr;
		}

		//�T�u�N���X������H
		if( wndsetting.ProcedureChange )
		{
			//�T�u�N���X������B
			if( need_wndcls_register )
			{
				//�E�C���h�E�N���X�����삵�Ă�ꍇ�A�ŏ����炻���Ȃ��Ă���
				//�ʂɃT�u�N���X�����Ȃ��Ă��A�����ƃ��b�Z�[�W��M�ł����I
				;
			}
			else
			{
				SetLastError( 0 );
				win->MyDefWndproc = (WNDPROC)::SetWindowLongPtrW( hwnd , GWLP_WNDPROC , (LONG_PTR)mGlobalWindowFunc::MessageProcedure );
				if( win->MyDefWndproc == nullptr && GetLastError() != 0 )
				{
					//�u�������Ɏ��s
					RaiseAssert( g_ErrorLogger , 0 , L"�T�u�N���X�������s���܂���" );
					mDelete win;
					return nullptr;
				}
			}
		}

		//�}�b�v�ɓo�^(1)�@�`�O���[�o���E�C���h�E�֐��`
		if( !mGlobalWindowFunc::Attach( mGlobalWindowFunc::AttachAccessPermission() , hwnd , win ) )
		{
			//�o�^�ł��Ȃ�
			RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"�O���[�o���E�C���h�E�֐��ɓo�^�ł��܂���ł���" + wndsetting.WindowName );
			mDelete win;
			return nullptr;
		}

		//�}�b�v�ɓo�^(2)�@�`ID�}�b�v�`
		MyIdMap.insert( IdMap::value_type( id , win ) );
		MyHwndMap.insert( HwndMap::value_type( hwnd , id ) );
	}
	else
	{
		win->MyIsWindowClassOriginal = need_wndcls_register;
		win->MyWindowClass = wndsetting.ClassName;
		win->MyParent = ( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyHwnd );
		win->MyRoot = ( MyParent == nullptr ) ? ( 0 ) : ( MyParent->MyRoot );
		win->MyHwnd = 0;
		win->MyDefWndproc = nullptr;
		MyIdMap.insert( IdMap::value_type( id , win ) );
	}

	//�쐬�����Ȃ̂ŃR�[���o�b�N���Ăяo��
	if( !win->OnCreate( opt ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"OnCreate�����s���܂���" );
		mDelete win;
		return nullptr;
	}

	return win;
}

bool mWindowCollection::RemoveControl( const WString& id )
{
	//�폜�Ώۂ�����
	IdMap::iterator itr = MyIdMap.find( id );
	if( itr == MyIdMap.end() )
	{
		//������ł����ǁH
		RaiseError( g_ErrorLogger , 0 , L"ID not found : " + id );
		return false;
	}

	HWND del_hwnd = itr->second->GetMyHwnd();

	//�폜����
	itr->second->OnDestroy();
	mDelete itr->second;
	MyIdMap.erase( id );

	if( MyHwndMap.count( del_hwnd ) == 0 )
	{
		//������ł����ǁH
		RaiseError( g_ErrorLogger , 0 , L"Associated hwnd is not found : " + id );
		return false;
	}
	MyHwndMap.erase( del_hwnd );

	return true;
}

WString mWindowCollection::QueryId( HWND hwnd )const
{
	HwndMap::const_iterator itr = MyHwndMap.find( hwnd );
	if( itr == MyHwndMap.end() )
	{
		return L"";
	}
	return itr->second;
}

mWindow* mWindowCollection::Query( const WString& id )const
{
	IdMap::const_iterator itr = MyIdMap.find( id );
	if( itr == MyIdMap.end() )
	{
		return nullptr;
	}
	return itr->second;
}

bool mWindowCollection::AdjustSize( const RECT& world )
{
	//�q�I�u�W�F�N�g�S���ɃT�C�Y�ύX������
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		//�ύX��̈ʒu�����߂�
		RECT abspos;
		CalcAbsolutePosition( itr->second->MyPosition , abspos , world );

		//�ʒu�ύX
		itr->second->MoveWindowPosition( abspos );
	}
	return true;
}

bool mWindowCollection::AdjustSize( HWND hwnd )
{
	RECT rect;
	if( !::GetClientRect( hwnd , &rect ) )
	{
		return false;
	}
	return AdjustSize( rect );
}


bool mWindowCollection::CalcAbsolutePosition( const mWindow::WindowPosition& srcpos , RECT& retPos , const RECT& world )
{
	INT width = world.right - world.left;	//��
	INT height = world.bottom - world.top;	//����

	retPos.left = (LONG)( srcpos.left.rate * width ) + srcpos.left.offset;			//���[�̍��W
	retPos.right = (LONG)( srcpos.right.rate * width ) + srcpos.right.offset;		//�E�[�̍��W
	retPos.top = (LONG)( srcpos.top.rate * height ) + srcpos.top.offset;			//��[�̍��W
	retPos.bottom = (LONG)( srcpos.bottom.rate * height ) + srcpos.bottom.offset;	//���[�̍��W
	return true;
}

bool mWindowCollection::ReflectMessage( UINT msg , WPARAM wparam , LPARAM lparam )
{
	//�q�I�u�W�F�N�g�S���Ƀ��b�Z�[�W�𑗐M����
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		itr->second->WindowProcedure( msg , wparam , lparam );
	}
	return true;
}


