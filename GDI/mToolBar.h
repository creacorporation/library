//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�c�[���o�[�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


/*
���p�r
�c�[���o�[�̎����ł�

*/

#ifndef MTOOLBAR_H_INCLUDED
#define MTOOLBAR_H_INCLUDED

#include "mStandard.h"
#include "mWindow.h"
#include "mGdiDualImagelist.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class mToolBar : public mWindow
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
		//�A�C�R���p�C���[�W���X�g�쐬���̃I�v�V����
		mGdiImagelist::Option_UseOption ImgOpt;

		//�t���b�g�^�C�v�̃c�[���o�[�ɂ���Ȃ��true
		bool IsFlat;

		//���X�g�X�^�C��(�����񂪃A�C�R���̉E)�Ȃ�true�A�����łȂ�(�����񂪃A�C�R���̉�)�Ȃ�false
		bool IsListStyle;

		//�c�[���`�b�v�p�̃E�C���h�E���b�Z�[�W�𐶐����邩
		//true�ɂ����WM_NOTIFY/TTN_GETDISPINFO����Ԃ悤�ɂȂ�܂�
		bool IsTooltipReq;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			IsFlat = false;
			IsListStyle = false;
			IsTooltipReq = true;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * opt )throw( )
	{
		if( opt == nullptr )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"Option required" ); 
			return nullptr;
		}
		return mNew mToolBar;
	}

	//�A�C�e���̎��
	enum ItemType
	{
		BUTTONTYPE,		//���������̃{�^��
		CHECKBOXTYPE,	//�������炻�̂܂܂ɂȂ��āA������x�����Ɩ߂�{�^��
		SEPARATOR,		//�Z�p���[�^�ł���
		DROPDOWN,		//�{�^���̉��Ɂ��}�[�N������i�{�^���Ɓ��͓Ɨ��j
		WHOLEDROPDOWN,	//�{�^���̉��Ɂ��}�[�N������i�{�^���Ɓ��͈�̌^�j
	};

	//�c�[���o�[�̃A�C�e��
	struct ItemOptionEntry
	{
		//�A�C�e���𐶐�����Ƃ��ɁA�����Ɏw�肵���Z�N�V�����̓��e���g���B
		//�󕶎���̏ꍇ�͖����i���[�g�Z�N�V�����͎g�p����Ȃ��j
		//ToolbarUpdateHandle::SetInitFile()���g�p����Ini�t�@�C�����w�肵�Ă���ꍇ�̂ݗL���B
		WString SectionName;

		ItemType Type;			//�A�C�e���̎��

		//�c�[���o�[�̃{�^���������ꂽ�Ƃ��ɁA�A�v���ɒʒm�������
		UINT FunctionId;		//���[�U�[��`�̋@�\ID
		ULONG_PTR UserData;		//���[�U�[��`�̒l
		WString OptString;		//���[�U�[��`�̕�����

		//�c�[���o�[�̕\�������\������O���t�B�b�N�ƕ�����
		WString Caption;		//�\��������
		WString ImageId;		//�\������A�C�R����ID(�ʏ�ƑI����Ԃœ���ID���g���܂�)

		ItemOptionEntry()
		{
			Type = ItemType::BUTTONTYPE;
			FunctionId = 0;
			UserData = 0;
		}
	};
	typedef std::vector<ItemOptionEntry> ItemOption;

	//�c�[���o�[���A�b�v�f�[�g���邽�߂̃N���X�B
	//1.ToolbarUpdateHandle�̃R���X�g���N�^�ɁAmToolBar��n���Ă��������B
	//2.Image()�AItem()���g���ĎQ�Ƃ��擾���A�c�[���o�[�̐ݒ��ύX���Ă��������B
	//  img1���ʏ펞�̃A�C�R���Aimg2���I�����̃A�C�R���ɂȂ�܂��B
	//3.ToolbarUpdateHandle�̃C���X�^���X��j������ƁA�c�[���o�[�ɔ��f����܂��B
	class ToolbarUpdateHandle final
	{
	public:
		ToolbarUpdateHandle( mToolBar& toolbar );
		~ToolbarUpdateHandle();

		//�c�[���o�[�̃A�C�R���ɂ���C���[�W���X�g���擾
		//�y���Ӂz�擾�����Q�Ƃ̃|�C���^��ێ�������A������悤�Ƃ����肵�Ȃ��ł��������B
		mGdiDualImagelist& Image()const;

		//�A�C�e���ݒ�̎Q�Ƃ��擾
		//�y���Ӂz�擾�����Q�Ƃ̃|�C���^��ێ�������A������悤�Ƃ����肵�Ȃ��ł��������B
		ItemOption& Item()const;

	protected:
		ToolbarUpdateHandle() = delete;
		ToolbarUpdateHandle& operator=( const ToolbarUpdateHandle& src ) = delete;

		mToolBar* MyParent;				//�e�I�u�W�F�N�g�̎Q��1
		mGdiDualImagelist* MyImgage;	//�X�V���̃C���[�W���X�g
		ItemOption* MyItemOption;		//���j���[�̃A�C�e���ݒ�
	};

	//�A�C�R����L���E�����ɂ���
	//FunctionId : �ݒ肷��@�\ID(ItemOption::FunctionId�Ŏw�肵������)
	//enable : �^�ɂ���ƗL���ɂ���A�U�ɂ���Ɩ����ɂ���B
	//checked : �^�ɂ���ƃ`�F�b�N����B�U�ɂ���ƃ`�F�b�N���O���B
	//�����FunctionId�������j���[����������ꍇ�A�S�ĂɓK�p����܂��B
	bool SetState( UINT FunctionId , bool enable , bool checked );

	//�A�C�e���̏����擾����
	//index : �擾�������A�C�e���̃C���f�b�N�X
	//	�E�������0�x�[�X�̃C���f�b�N�X�ɂȂ�܂��B
	//	�EWM_NOTIFY��NM_CLICK���b�Z�[�W��LPARAM�ANMMOUSE::dwItemSpec�̒l��
	//	�@0�x�[�X�̃C���f�b�N�X�ɂȂ��Ă���̂ŁA���̒l�������Ă�OK�ł��B
	//retInfo : �擾�����A�C�e���̏��
	//ret : �������^
	bool QueryItem( INT index , ItemOptionEntry& retInfo )const;

	//�A�C�e���̏����擾����
	//mouse : WM_NOTIFY��NM_CLICK���b�Z�[�W��LPARAM��LPNMMOUSE�ɃL���X�g��������
	//retInfo : �擾�����A�C�e���̏��
	//ret : �������^
	bool QueryItem( const LPNMMOUSE mouse , ItemOptionEntry& retInfo )const;

protected:
	mToolBar();
	virtual ~mToolBar()final;

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

	//�E�C���h�E���������������ꍇ�ɃR�[�������
	virtual bool OnCreate( const void* opt )override;

private:

	mToolBar( const mToolBar& src ) = delete;
	mToolBar& operator=( const mToolBar& src ) = delete;

	//���݂̃c�[���o�[�̏�Ԃ�K�p����B�ȉ��̃����o�ϐ��̒l�����ۂ̃I�u�W�F�N�g�ɔ��f�����B
	//�EMyImgage
	//�EMyItemOption
	//ret : ������true
	bool ExecUpdate( void );

	//���݂̃c�[���o�[�̃{�^����S���j������
	//ret : ������true
	bool ExecClear( void );

protected:

	//�c�[���o�[�ɓ\��t����A�C�R��
	mGdiDualImagelist* MyImgage;

	//�A�C�e���ݒ�
	ItemOption MyItemOption;

	//����ID�̎g�p���̒l�ꗗ
	typedef mUniqueValue<INT> InternalIdStock;
	InternalIdStock MyInternalIdStock;

};

#pragma region MTOOLBAR_SAMPLE
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

		case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lparam;
			WString id = MyChild->QueryId( nmhdr->hwndFrom );

			if( nmhdr->code == NM_CLICK && id == L"TOOLBAR" )
			{
				LPNMMOUSE lpnm = (LPNMMOUSE)lparam;
				mToolBar* tb = (mToolBar*)MyChild->Query( id );

				mToolBar::ItemOptionEntry item;
				tb->QueryItem( lpnm , item );

				WString str;
				sprintf( str , L"���j���[�u%ws�v(FunctionId=%d)���I������܂���" , item.Caption.c_str(), item.FunctionId );
				::MessageBoxW( GetMyHwnd() , str.c_str(), L"" , 0);
			}
		}
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

		//�{�^���Ȃǂ�\��t����ꍇ�́AMyChild�����o�Ɏ��̂������Ă��܂��B
		//�i������Ԃł�nullptr�ɂȂ��Ă��܂��B�j
		//main�֐��ł͐e�E�C���h�E���Ȃ�����nullptr��n���Ă��܂����A
		//����͂��̃E�C���h�E���e�ɂȂ�̂ŁAthis��n���܂��B
		//���Ƃ́AMyChild�ɃI�u�W�F�N�g��o�^���Ă��������ł��B
		MyChild = mNew mWindowCollection( this );
		{
			//�c�[���o�[�̐���
			mToolBar::Option_UseOption opt;
			opt.IsFlat = true;
			opt.IsListStyle = true;
			opt.ImgOpt.width = 16;		//�A�C�R���̃T�C�Y
			opt.ImgOpt.height = 16;		//�A�C�R���̃T�C�Y
			mToolBar* tb = MyChild->AddControl<mToolBar>( L"TOOLBAR" , &opt );

			//�c�[���o�[�̏��o�^�I�u�W�F�N�g�𐶐�
			mToolBar::ToolbarUpdateHandle handle( *tb );

			//�c�[���o�[�ɒ���t����A�C�R���𐶐�
			for( DWORD i = 0 ; i < 4 ; i++ )
			{
				WString idstr;
				sprintf( idstr , L"IMAGE_%d" , i + 1 );

				mGdiBitmap::Option_LoadFile img_opt;
				sprintf( img_opt.path , L"%d.bmp" , i + 1 );
				mGdiBitmap bmp1( &img_opt );

				sprintf( img_opt.path , L"%d_h.bmp" , i + 1 );
				mGdiBitmap bmp2( &img_opt );

				handle.Image().AddImage( idstr , bmp1 , bmp2 , RGB( 255 , 255 , 255 ) , RGB( 255 , 255 , 255 ) );
			}

			//�c�[���o�[�̍��ړo�^
			mToolBar::ItemOptionEntry entry;
			entry.ImageId = L"IMAGE_1";
			entry.Caption = L"";
			entry.Type = mToolBar::ItemType::CHECKBOXTYPE;
			entry.FunctionId = 1;
			handle.Item().push_back( entry );

			entry.ImageId = L"IMAGE_2";
			entry.Caption = L"BUTTON2";
			entry.FunctionId = 2;
			entry.Type = mToolBar::ItemType::BUTTONTYPE;
			handle.Item().push_back( entry );

			entry.ImageId = L"IMAGE_3";
			entry.Caption = L"BUTTON3";
			entry.FunctionId = 3;
			entry.Type = mToolBar::ItemType::BUTTONTYPE;
			handle.Item().push_back( entry );

			mToolBar::ItemOptionEntry entry_sep;
			entry_sep.Type = mToolBar::ItemType::SEPARATOR;
			entry_sep.FunctionId = 999;	//�Z�p���[�^���AFunctionId=0��NG
			handle.Item().push_back( entry_sep );

			entry.ImageId = L"IMAGE_4";
			entry.Caption = L"BUTTON4";
			entry.FunctionId = 4;
			entry.Type = mToolBar::ItemType::BUTTONTYPE;
			handle.Item().push_back( entry );
		}
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

#endif //MTOOLBAR_H_INCLUDED

