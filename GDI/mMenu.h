//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���j���[�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
���j���[�̊Ǘ��N���X�ł�

�E���j���[�͐l�Ԃ����삽�Ƃ������쓮������́B
�@�܂�l�Ԃ̑���ɕt���Ă����Α��x�͏\���Ƃ������ƂɂȂ邩��A�������x�̂��Ƃ͍l���Ă܂���B
*/

#ifndef MMENU_H_INCLUDED
#define MMENU_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mUniqueValue.h"
#include "mGdiResource.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>

class mMenu
{
public:

	//�R���g���[���������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B

	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�R���g���[���������̃I�v�V����
	struct Option_UseOption : public Option
	{
		//�|�b�v�A�b�v�ȃ��j���[�ɂ���Ȃ�true�B���ʂ̃`�F�b�N������Ȃ�false�B
		bool IsPopup;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			IsPopup = false;
		}
	};

	//�R���X�g���N�^
	//opt : �R���g���[���������̃I�v�V����(nullptr��)
	mMenu( const Option* opt );

	//�f�X�g���N�^
	virtual ~mMenu();

	//���j���[�̍��ڂ��`�F�b�N�������ǂ����B���邢�̓Z�p���[�^���ǂ����B
	//���W�I�{�^���^�C�v(����̕�)��I�������ꍇ�ł��A
	//���j���[�I�����ɏ���ɑ��̍��ڂ̃`�F�b�N���������ꂽ��͂��܂���B
	//����ɂ��恜��ɂ���A�`�F�b�N�̏�Ԃ��X�V����̂̓v���O���}���ōs���܂��B
	enum MenuType
	{
		NORMALCHECK,	//����`�F�b�N
		RADIOCHECK,		//����`�F�b�N
		SEPARATOR,		//�Z�p���[�^�ł���
	};
	
	//���j���[�̃f�[�^
	struct MenuOption
	{
		//��I����Ԃ̃r�b�g�}�b�v
		//(�����Ŏw�肵����������L�[�ɂ���mGdiResource����HBITMAP�̃n���h�����擾���܂�)
		WString UncheckedBitmap;

		//�I����Ԃ̃r�b�g�}�b�v
		//(�����Ŏw�肵����������L�[�ɂ���mGdiResource����HBITMAP�̃n���h�����擾���܂�)
		WString CheckedBitmap;

		WStringVector Path;		//���ڂ̊K�w�������p�X
		MenuType Type;			//���j���[�̑̍�

		UINT FunctionId;		//���[�U�[��`�̋@�\ID�i�t��������Ƃ��ɂ��g���j
		ULONG_PTR UserData;		//���[�U�[��`�̒l
		WString OptString;		//���[�U�[��`�̕�����

		MenuOption()
		{
			FunctionId = 0;
			UserData = 0;
			Type = MenuType::NORMALCHECK;
		}
	};

	//MenuOption�̃A���C
	typedef std::deque<MenuOption> MenuOptionArray;

	//���j���[�ɃA�C�e����ǉ�����
	//item : �ǉ����鍀��
	//ret : ����������true
	bool AddItem( const MenuOption& item , const mGdiResource* res = nullptr );

	//���j���[����A�C�e�����폜����
	//Path : �폜����A�C�e��(MenuOption::Path�Ŏw�肵������)
	//ret : ����������true
	bool RemoveItem( const WStringVector& Path );

	//���j���[��L���E�����ɂ���
	//FunctionId : �ݒ肷��@�\ID(MenuOption::FunctionId�Ŏw�肵������)
	//enable : �^�ɂ���ƗL���ɂ���A�U�ɂ���Ɩ����ɂ���B
	//checked : �^�ɂ���ƃ`�F�b�N����B�U�ɂ���ƃ`�F�b�N���O���B
	//�����FunctionId�������j���[����������ꍇ�A�S�ĂɓK�p����܂��B
	bool SetState( UINT FunctionId , bool enable , bool checked );

	//�E�C���h�E���b�Z�[�W����I�����ꂽ���j���[�̃f�[�^���擾����
	//wparam : �E�C���h�E���b�Z�[�W��WPARAM�p�����[�^
	//retItem : �擾�����f�[�^
	//ret : �f�[�^���擾�����Ƃ��^�B�Y���̃��j���[���ڂ��Ȃ������ꍇ�U�B
	bool QuerySelectedMenu( WPARAM wparam , MenuOption& retItem )const;

	//FunctionId����I�����ꂽ���j���[�̃f�[�^���擾����
	//function_id : �����������@�\ID
	//retItem : �擾�����f�[�^(�Y�����鍀�ڑS�Ă��Ԃ�)
	//ret : �f�[�^���擾�����Ƃ��^�B�Y���̃��j���[���ڂ�1���Ȃ������ꍇ�U�B
	bool QueryItem( UINT function_id , MenuOptionArray& retItem )const;

	//���j���[�̃n���h�����擾����
	HMENU GetMenuHandle( void );

private:
	mMenu( const mMenu& src ) = delete;
	mMenu& operator=( const mMenu& src ) = delete;

protected:
	//-----------------------------------------------------------
	// �f�[�^�\���ƃ����o�ϐ�
	//-----------------------------------------------------------
	struct MenuItemEntry;
	typedef std::deque<MenuItemEntry*> MenuItem;

	//1�̃T�u���j���[�̏����i�[
	struct MenuHandle
	{
		MenuHandle() = delete;
		MenuHandle( const MenuHandle& src ) = delete;
		MenuHandle& operator=( const MenuHandle& src ) = delete;

		HMENU Handle;		//���j���[�̃n���h��
		MenuItem Items;		//���j���[�Ɋ܂܂��A�C�e��

		MenuHandle( bool IsPopup )
		{
			if( IsPopup )
			{
				Handle = ::CreatePopupMenu();
			}
			else
			{
				Handle = ::CreateMenu();
			}
		}

		~MenuHandle()
		{
			MenuItem::iterator itr;
			for( itr = Items.begin() ; itr != Items.end() ; itr++ )
			{
				mDelete *itr;
			}
			::DestroyMenu( Handle );
		}
	};

	//���j���[�̊e���ڂɂ��Ă̏����i�[
	struct MenuItemEntry
	{
		MenuItemEntry() = delete;
		MenuItemEntry( MenuItemEntry& src ) = delete;
		MenuItemEntry& operator=( const MenuItemEntry& src ) = delete;

		WString SectionName;		//Ini�t�@�C���̃Z�N�V������
		WString UnchedkedBitmap;	//��I����Ԃ̃r�b�g�}�b�v
		WString CheckedBitmap;		//�I����Ԃ̃r�b�g�}�b�v

		MenuType Type;				//���j���[�̃^�C�v
		mMenu& Root;				//���[�g�I�u�W�F�N�g
		MenuHandle& Parent;			//�e�I�u�W�F�N�g
		WString Name;				//���ڂ̖���
		USHORT InternalId;			//����ID�iWINAPI�ɓo�^����ID�j
		UINT FunctionId;			//���[�U�[��`�̋@�\ID�i�t��������Ƃ��ɂ��g���j
		ULONG_PTR UserData;			//���[�U�[��`�̒l
		WString OptString;			//���[�U�[��`�̕�����
		MenuHandle* Child;			//�T�u���j���[(�Ȃ��ꍇ��nullptr)

		MenuItemEntry( MenuHandle& parent , mMenu& inst )
			: Parent( parent )
			, Root( inst )
		{
			Type = MenuType::NORMALCHECK;
			InternalId = 0;
			FunctionId = 0;
			UserData = 0;
			Child = nullptr;
		}
		~MenuItemEntry()
		{
			mDelete Child;
			Root.MyInternalIdStock.Return( InternalId );
		}
	};

	//���[�g���j���[
	MenuHandle* MyRootMenu;

	//����ID�̎g�p���̒l�ꗗ
	typedef mUniqueValue<USHORT> InternalIdStock;
	InternalIdStock MyInternalIdStock;

protected:
	//-----------------------------------------------------------
	// ���[�e�B���e�B�֐�
	//-----------------------------------------------------------

	//�w�肵���p�X�����A�C�e���ւ̃|�C���^��Ԃ��܂�
	//Path : �擾�������A�C�e���̃p�X
	//Create : true = �w�肵���A�C�e�������݂��Ȃ��ꍇ�A�����V�K�ɍ쐬���ĕԂ��܂�
	//         false = �w�肵���A�C�e�������݂��Ȃ��ꍇ�A�G���[�ƂȂ�܂�
	//ret : ���������Ƃ��A�C�e���ւ̃|�C���^�B�G���[�̂Ƃ�nullptr�B
	//�ECreate��false�̏ꍇ�A�����ϐ������������邱�Ƃ͂���܂���(const_cast����OK)�B
	MenuItemEntry* SearchItemEntry( const WStringVector& Path , bool Create );

	//ScanFunctionId����Ăяo���R�[���o�b�N�֐�
	//entry : �����Ώۂ̍���
	//ret : ���������������ꍇ�^
	typedef std::function< bool( MenuItemEntry* entry ) > ScanItemCallback;

	//�w�肵�����ڔz�����X�L�������A�w�肵���@�\ID�������ڂɑ΂��āA�R�[���o�b�N�֐����Ăяo���܂��B
	//menu : �X�L��������Ώ�(�S���ڂ�Ώۂɂ���Ȃ�MyRootMenu��n���܂�)
	//functionid : �R�[���o�b�N���Ăяo���Ώۂɂ���@�\ID
	//callback_func : �Ăяo���R�[���o�b�N�֐�
	//ret : �R�[���o�b�N�֐���1�x���Ăяo����Ȃ�������true
	//      �R�[���o�b�N�֐����S��true��Ԃ�����true
	//      �R�[���o�b�N�֐���false��Ԃ������Ƃ����遨false
	//�E�R�[���o�b�N�֐����U��Ԃ����ꍇ�ł��AScanFunctionId�̏����͒��f����܂���B
	//  ���ɂ������Ώۂ̍��ڂ�����ƁA���̍��ڂɑ΂��čĂуR�[������܂��B
	bool ScanFunctionId( MenuHandle* menu , UINT functionid , ScanItemCallback callback_func )const;

	//�w�肵�����ڔz�����X�L�������A�w�肵������ID�������ڂ̃p�X��T���܂��B
	//menu : �X�L��������Ώ�(�S���ڂ�Ώۂɂ���Ȃ�MyRootMenu��n���܂�)
	//internalid : �R�[���o�b�N���Ăяo���Ώۂɂ������ID
	//ret : ����ID���������ꂽ�ꍇ�^
	bool ScanInternalId( MenuHandle* menu , USHORT internalid , WStringDeque& retPath )const;

	//MenuItemEntry�̓��e��MENUITEMINFOW�ɓW�J����
	//src : �W�J����MenuItemEntry�\����
	//retDst : �W�J���MENUITEMINFOW�\����
	//sec : �ǉ������擾���邽�߂̃Z�N�V����(nullptr��)
	//res : ���j���[�ɕt�^����r�b�g�}�b�v���擾���郊�\�[�X�v�[��(nullptr��)
	//ret : �������^
	bool SetMenuItemInfoStruct( const MenuItemEntry& src , MENUITEMINFOW& retDst , const mGdiResource* res )const;

};

#pragma region MMENU_SAMPLE_CODE
#ifdef SAMPLE_CODE
class TestWindow : public mWindow
{
public:
	static mWindow* Factory( const void * )throw( )
	{
		return mNew TestWindow;
	}

protected:
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
	{
		switch( msg )
		{
		case WM_DESTROY:
			//�E�C���h�E��������I��
			PostQuitMessage( 0 );
			return 0;
		case WM_COMMAND:
		{
			if( lparam == 0 )	//���j���[���ڂ��I�����ꂽ�Ƃ���lparam=0
			{
				//�I�����ꂽ���j���[���擾
				mMenu::MenuOption item;
				if( MyMenu->QuerySelectedMenu( wparam , item ) )
				{
					WString str;
					sprintf( str , L"FunctionId=%d���I������܂���" , item.FunctionId );
					MessageBoxW( GetMyHwnd() , str.c_str() , L"" , 0 );
				}
			}
			return 0;
		}

		default:
			break;
		}
		return mWindow::WindowProcedure( msg , wparam , lparam );
	}
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
	{
		retSetting.ClassName = L"TESTCLASS";
		return true;
	}
	virtual void CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
	{
		retSetting.WindowName = L"TEST WINDOW";
		retSetting.Width = CW_USEDEFAULT;
		retSetting.Style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	}
	virtual void OnCreate( const void* option )
	{

		MyChild = mNew mWindowCollection( this );

		{
			MyMenu = mNew mMenu( nullptr );
			::SetMenu( GetMyHwnd() , MyMenu->GetMenuHandle() );

			mMenu::MenuOption opt;
			opt.FunctionId = 1;
			opt.Path.push_back( L"���j���[�P" );
			MyMenu->AddItem( opt );

			opt.FunctionId = 2;
			opt.Path[ 0 ] = L"���j���[�Q";
			MyMenu->AddItem( opt );

			opt.FunctionId = 3;
			opt.Path[ 0 ] = L"���j���[�R";
			MyMenu->AddItem( opt );

			opt.FunctionId = 4;
			opt.Path[ 0 ] = L"���j���[�R";
			opt.Path.push_back( L"�T�u���j���[�P" );
			MyMenu->AddItem( opt );

			opt.FunctionId = 5;
			opt.Path[ 0 ] = L"���j���[�R";
			opt.Path[ 1 ] = L"�T�u���j���[�P";
			opt.Path.push_back( L"�T�u���j���[�Q" );
			MyMenu->AddItem( opt );

			DrawMenuBar( GetMyHwnd() );
		}
		MyChild->AdjustSize( GetMyHwnd() );
		UpdateWindow( GetMyHwnd() ); 
	}
};

int main( int argc , char** argv )
{
	InitializeLibrary();

	//�E�C���h�E�̐���
	mWindowCollection root_collection( nullptr );
	root_collection.AddControl<TestWindow>( L"TEST" );

	MSG msg;
	while( GetMessageW( &msg , 0 , 0 , 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}
	return 0;
}

#endif
#pragma endregion

#endif //MMENU_H_INCLUDED

