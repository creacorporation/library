//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�E�C���h�E�̊��N���X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�E�C���h�E�A�{�^���A���W�I�{�^������GUI�ȃI�u�W�F�N�g�̊��N���X�ł��B

�����ӓ_
�EmWindow���p�������N���X�́A���ꂼ��ȉ��̍\���́A���\�b�h��������
  (mWindowCollection::AddControl()�Ŗ��O�w��ŌĂяo���Ă��邽��)
�@Factory() �c�V�����C���X�^���X��Ԃ��t�@�N�g�����\�b�h
  struct Option �c���̃N���X�ŗL�̐ݒ荀�ڂ��i�[����\����
*/

//���T���v���R�[�h
#ifdef SAMPLE_CODE
//���g�p�T���v���i�ʃt�@�C���ɃR�s�y���ĉ������j
//����F�E�C���h�E��1�쐬���܂��B���̃E�C���h�E�����ƃv���O�������I�����܂��B
//�O�̃T���v�����Ȃ�
//���̃T���v����mButton.h���Q��(�E�C���h�E�Ƀ{�^����\��t����R�[�h������܂�)
#include "GDI/mWindow.h"
#include "GDI/mWindowCollection.h"
class TestWindow : public mWindow
{
public:
	//�t�@�N�g�����\�b�h�i�p�������N���X���ɐÓI�ɍ���ĉ������j
	static mWindow* Factory( const void * )throw( )
	{
		return mNew TestWindow;
	}
protected:
	//�E�C���h�E�v���V�[�W��
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
	{
		//WM_DESTROY�������烁�b�Z�[�W���[�v���I�����܂��B
		if( msg == WM_DESTROY )
		{
			PostQuitMessage( 0 );
			return 0;
		}
		return mWindow::WindowProcedure( msg , wparam , lparam );
	}
	//mWindowCollection::AddControl����Ă΂�܂��B�E�C���h�E�N���X�̏���o�^���܂��B
	//�����ł́ATESTCLASS�Ƃ������O�������߂Ă��܂��B
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
	{
		retSetting.ClassName = L"TEST";
		return true;
	}
	//mWindowCollection::AddControl����Ă΂�܂��B�쐬����E�C���h�E�̏���o�^���܂��B
	//�����ł́A�\���X�^�C���������߂Ă��܂��B
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
	{
		retSetting.Style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
		retSetting.WindowName = L"TEST";
		return true;
	}

};

int main( int argc , char** argv )
{
	InitializeLibrary();	//������

	//�E�C���h�E�̃R���N�V�����B�e�E�C���h�E�������̂�nullptr��n���܂��B
	mWindowCollection root_collection( nullptr );
	//class TestWindow��TEST�Ƃ������O�œo�^���܂��B
	root_collection.AddControl<TestWindow>( L"TEST" );

	//���b�Z�[�W���[�v
	MSG msg;
	while( GetMessageW( &msg , 0 , 0 , 0 ) )
	{
		TranslateMessage( &msg ); //EDIT�R���g���[���Ȃ�WM_CHAR���󂯎��Ȃ�K�v
		DispatchMessageW( &msg );
	}
	return 0;
}
#endif

#ifndef MWINDOW_H_INCLUDED
#define MWINDOW_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "GDI/mMenu.h"
#include "GDI/mWindowPosition.h"
#include "GDI/mGdiResource.h"

class mWindowCollection;

class mWindow
{
public:
	
	//�E�C���h�E�v���V�[�W��
	//mWindow::WindowProcedure()���Ăяo�����ꍇ�A
	//(1)�E�C���h�E�v���V�[�W�����u���������Ă���΁A�u�������O�̃E�C���h�E�v���V�[�W���ɁA
	//(2)�E�C���h�E�v���V�[�W�����u���������Ă��Ȃ���΁ADefWindowProcW()��
	//���b�Z�[�W�����������܂��B
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam );

	/*
	���t�@�N�g�����\�b�h�쐬��
	�@�ȉ��R�[�h���p�������N���X�ɃR�s�y���A�umNew mWindow�v�̕������p�������N���X���Œu�������ĉ������B
	static mWindow* Factory( const void * )throw( )
	{
		return mNew mWindow;
	}
	*/

	//���ΓI�ɍ��W�����肷�邽�߂̏��
	typedef mWindowPosition::POSITION POSITION;

	//�E�C���h�E�̑��ΓI�Ȉʒu�ݒ�
	//�E�C���h�E(���邢�̓{�^���Ȃǂ̃p�[�c��)��e�E�C���h�E�̃N���C�A���g�G���A���̂ǂ̂�����ɔz�u���邩���w�肵�܂��B
	//�㉺���E�ɂ��āA�e�E�C���h�E�̏�[�E���[����̔䗦�{�I�t�Z�b�g�ŕ\���܂��B
	//rate=0.0,offset=10	�����[����10�s�N�Z���̈ʒu
	//rate=1.0,offset=-10	���E�[����10�s�N�Z���̈ʒu
	//rate=0.5,offset=10	����������E��10�s�N�Z���̈ʒu
	//���[(rate=0.1,offset=  10)+�E�[(rate=0.1,offset=110) �����񂹌Œ蕝100�s�N�Z��
	//���[(rate=1.0,offset=-110)+�E�[(rate=1.0,offset=-10) ���E�񂹌Œ蕝100�s�N�Z��
	//���[(rate=0.0,offset=  10)+�E�[(rate=0.5,offset=-10) �����͐e��50�p�[�Z���g�{10�s�N�Z���̃p�f�B���O�B
	typedef mWindowPosition::WindowPosition WindowPosition;

	//�E�C���h�E�̑��Έʒu��ݒ肷��
	//newpos : �V�����ʒu
	virtual bool SetWindowPosition( const WindowPosition& newpos );

	//�E�C���h�E�̉�/�s����ύX����
	//newstate : true�ŉ��Afalse�ŕs��
	//ret : ������true
	virtual bool SetVisible( bool newstate );

	//�E�C���h�E�̗L��/������ύX����
	//newstate : true�ŗL���Afalse�Ŗ���
	//ret : ������true
	virtual bool SetEnable( bool newstate );

	//�E�C���h�E�̈ʒu���X�N���[�����W�œ���
	//ret : �X�N���[�����W�ɂ��E�C���h�E�̈ʒu(�G���[�̏ꍇ0,0,0,0)
	virtual RECT GetRect( void ) const;

	//�E�C���h�E�̃T�C�Y�𓾂�
	//ret : �E�C���h�E�̃T�C�Y(�G���[�̏ꍇ0,0)
	virtual SIZE GetSize( void ) const;

	//���̃E�C���h�E�̃N���C�A���g���W���X�N���[�����W�ɕϊ�����
	//client_pos : �N���C�A���g���W
	//ret : �X�N���[�����W(�G���[�̏ꍇ0,0)
	virtual POINT Client2Screen( const POINT& client_pos ) const;

	//���̃E�C���h�E�̃N���C�A���g���W���X�N���[�����W�ɕϊ�����
	//x : �N���C�A���g���W(X���W)
	//x : �N���C�A���g���W(Y���W)
	//ret : �X�N���[�����W(�G���[�̏ꍇ0,0)
	virtual POINT Client2Screen( INT x , INT y ) const;

	//�X�N���[�����W�����̃E�C���h�E�̃N���C�A���g���W�ɕϊ�����
	//client_pos : �X�N���[�����W
	//ret : �N���C�A���g���W(�G���[�̏ꍇ0,0)
	virtual POINT Screen2Client( const POINT& client_pos ) const;

	//�X�N���[�����W�����̃E�C���h�E�̃N���C�A���g���W�ɕϊ�����
	//x : �X�N���[�����W(X���W)
	//x : �X�N���[�����W(Y���W)
	//ret : �N���C�A���g���W(�G���[�̏ꍇ0,0)
	virtual POINT Screen2Client( INT x , INT y ) const;

	//�t�H�[�J�X��ݒ肷��
	//ret : ������true
	virtual bool SetFocus( void )const;

	//�E�C���h�E���ړ�����
	virtual void MoveWindowPosition( const RECT& pos );

protected:
	//---------------------------------------
	// �T�u�N���X�ւ̃R�[���o�b�N�֐�
	//---------------------------------------	
	mWindow();
	virtual ~mWindow();
	friend class mWindowCollection;

	//�E�C���h�E�N���X�̐ݒ�p�\����
	struct WindowClassSetting
	{
		WString ClassName;	//�o�^����N���X�̖��O�B
		UINT Style;			//�X�^�C��
		HICON Icon;			//�֘A�Â���A�C�R��
		HICON IconSmall;	//�A�C�R���̏�������
		HCURSOR Cursor;		//�J�[�\��
		HBRUSH Background;	//�o�b�N�O���E���h��h��Ԃ��̂Ɏg���u���V

		//�f�t�H���g�l�̐ݒ�
		WindowClassSetting()
		{
			ClassName = L"";
			Style = 0;
			Icon = nullptr;
			IconSmall = nullptr;
			Cursor = nullptr;	
			Background = nullptr;
		}
	};

	//�E�C���h�E�N���X�̓o�^������
	//retSetting : �ݒ��Ԃ����߂̍\����
	//opt : ���[�U��`�̏��(mWindowFactory�ɓn�������̂����̂܂ܓn���Ă���)
	//ret : �E�C���h�E�N���X�̓o�^���s���ꍇtrue
	//      �E�C���h�E�N���X�̓o�^���s��Ȃ��ꍇ��false
	//      (��`�ς݃R���g���[���N���X�𗘗p����ꍇ�Ȃ�)
	//��mWindow::WindowClassSettingCallback()�͉�������false��Ԃ������ɂȂ��Ă��܂�
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt );

	//�E�C���h�E�쐬���
	struct CreateWindowSetting
	{
		WString ClassName;		//�E�C���h�E�̍쐬�Ɏg���E�C���h�E�N���X
		WString WindowName;		//�E�C���h�E�̖��O�i�^�C�g���j
		DWORD Style;			//�X�^�C���P
		DWORD ExStyle;			//�X�^�C���Q
		INT x;					//X���W
		INT y;					//Y���W
		INT Width;				//��
		INT Height;				//����

		//true�ɂ���ƃE�C���h�E�v���V�[�W����u�������܂�
		//�{�^���ȂǃR�����R���g���[���ŃI���W�i���ȓ�������������ꍇ�A
		//�����true�ɂ��邱�ƂƃT�u�N���X������A�E�C���h�E���b�Z�[�W���擾�ł���悤�ɂȂ�܂��B
		bool ProcedureChange;

		//�f�t�H���g�l�̐ݒ�
		CreateWindowSetting()
		{
			ClassName = L"";
			WindowName = L"";
			Style = WS_VISIBLE;
			ExStyle = 0;
			x = 0;
			y = 0;
			Width = 100;
			Height = 100;
			ProcedureChange = false;
		}
	};

	//�E�C���h�E���J��
	//retSetting : �E�C���h�E�쐬�̂��߂̏��
	//opt : ���[�U��`�̏��(mWindowFactory�ɓn�������̂����̂܂ܓn���Ă���)
	//ret : �E�C���h�E���쐬����ꍇ��true�B�E�C���h�E���쐬���Ȃ��ꍇ��false�B
	//      ��false��Ԃ����ꍇ�́AretSetting�̓��e�͑S�Ė�������A�����ŃE�C���h�E�𐶐�����K�v������܂��B
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt ) = 0;

	//�E�C���h�E���������������ꍇ�ɃR�[�������
	//ret : �ŏI�I�ɖ��Ȃ������ꍇ��true
	//      ��false��Ԃ����ꍇ�́A�E�C���h�E�̐������L�����Z�������B
	virtual bool OnCreate( const void* opt );

	//�E�C���h�E���j������悤�Ƃ��Ă���Ƃ��R�[�������
	virtual void OnDestroy( void );

private:

	mWindow( const mWindow& src ) = delete;
	mWindow& operator=( const mWindow& src ) = delete;

	//���̃E�C���h�E�ɑ΂���E�C���h�E�N���X�̖��O
	WString MyWindowClass;

	//�E�C���h�E�N���X�͎������g�œo�^�������̂��H(Yes=true)
	bool MyIsWindowClassOriginal;

	//���̃E�C���h�E�̃n���h��
	HWND MyHwnd;

	//���̃E�C���h�E�̐e�E�C���h�E�̃n���h��
	HWND MyParent;

	//���̃E�C���h�E�̍ŏ�ʃE�C���h�E�̃n���h��
	HWND MyRoot;

	//�u�������O�̃E�C���h�E�v���V�[�W��
	WNDPROC MyDefWndproc;

	//�E�C���h�E�̔z�u���
	WindowPosition MyPosition;

protected:

	//�q�E�C���h�E�̃R���N�V����
	mWindowCollection* MyChild;

	//GDI���\�[�X�̃R���N�V����
	mGdiResource MyGdiResource;

	//���j���[�̃n���h��
	mMenu* MyMenu;

	//�������g�̃E�C���h�E�n���h�����擾����
	HWND GetMyHwnd( void )const;

	//�e�E�C���h�E�̃n���h�����擾����
	HWND GetMyParent( void )const;

	//�ŏ�ʃE�C���h�E�̃n���h�����擾����
	HWND GetMyRoot( void )const;

};



#endif


