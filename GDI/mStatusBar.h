//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�X�e�[�^�X�o�[�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�X�e�[�^�X�o�[�̎����ł�
�E�{�^����`�F�b�N�{�b�N�X�Ɠ����悤�ȃJ���W�œ\��t���ł��܂��B
�E�c�[���o�[���������̃p�[�c�ɋ�؂��āA���ꂼ��ɕ�����Ƃ��ݒ�ł��܂��B
�E�������������c�[���o�[�̍����ɔz�u���āA�c����E���ɔz�u�ł��܂��B
	��mStatusBar::Option::RightJustifyIndex
�E���ō����ɔz�u�������̂ƉE���ɔz�u�������̂�������ꍇ�A�ǂ������ɂ��邩�w��ł��܂��B
	��mStatusBar::Option::RightUpper
*/

//���T���v���R�[�h
#ifdef SAMPLE_CODE
//���g�p�T���v���i�ʃt�@�C���ɃR�s�y���ĉ������j
//����F�X�e�[�^�X�o�[���t���Ă���E�C���h�E�𐶐����܂��B
#include "GDI/mStatusBar.h"
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
		case WM_SIZE:
			{
				MyChild->AdjustSize( GetMyHwnd() );

				RECT rect;
				GetClientRect( GetMyHwnd() , &rect );
				WString str;
				sprintf( str , L"%d" , rect.right - rect.left );

				mStatusBar* bar = (mStatusBar*)MyChild->Query( L"STATUS" );
				bar->SetText( L"6" , str );
			}
			break;
		}
		return mWindow::WindowProcedure( msg , wparam , lparam );
	}
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , void* opt )
	{
		retSetting.ClassName = L"TESTCLASS";
		return true;
	}
	virtual void CreateWindowCallback( CreateWindowSetting& retSetting , void* opt )
	{
		retSetting.WindowName = L"TEST WINDOW";
		retSetting.Width = CW_USEDEFAULT;
		retSetting.Style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	}
	virtual void OnCreate( void* option )
	{
		MyChild = mNew mWindowCollection( this );

		mStatusBar::Option opt;
		mStatusBar::PartsOptionEntry entry;
		entry.Width = 100;			//��100�s�N�Z���̃p�[�c�����������
		entry.Id = L"1";
		opt.Parts.push_back( entry );
		entry.Id = L"2";
		opt.Parts.push_back( entry );
		entry.Id = L"3";
		opt.Parts.push_back( entry );
		entry.Id = L"4";
		opt.Parts.push_back( entry );
		entry.Id = L"5";
		opt.Parts.push_back( entry );
		entry.Id = L"6";
		opt.Parts.push_back( entry );
		entry.Id = L"7";
		opt.Parts.push_back( entry );

		opt.RightJustifyIndex = 2;	//2�͍��񂹁A�c��͉E��
		opt.RightUpper = true;	//�E�񂹂̃p�[�c����ɂ���
		MyChild->AddControl<mStatusBar>( L"STATUS" , &opt );	//�X�e�[�^�X�o�[����
	}
};

int main( int argc , char** argv )
{
	InitializeLibrary();

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

#ifndef MSTATUSBAR_H_INCLUDED
#define MSTATUSBAR_H_INCLUDED

#include "mStandard.h"
#include "mWindow.h"
#include <vector>
#include <unordered_map>
#include <functional>

class mStatusBar : public mWindow
{
public:

	//�{�[�_�[���C���̎��
	enum BorderType
	{
		Normal,		//���ʂ̋��E��
		NoBorder,	//���E���Ȃ�
		Popout,		//����オ�鋫�E��
	};

	//�p�[�c�̐ݒ�
	struct PartsOptionEntry
	{

		WString Id;			//���̃p�[�c�ɂ���ID
		WString Str;		//�ݒ肷�镶����
		UINT Width;			//�p�[�c�̃T�C�Y
		bool Notab;			//�^�u�𖳎�����ꍇtrue
		BorderType Border;	//�{�[�_�[���C���̎��
		PartsOptionEntry()
		{
			Width = 100;
			Notab = true;
			Border = BorderType::Normal;
		}
	};

	//�e�p�[�c�̏����i�[����^
	typedef std::vector<PartsOptionEntry> PartsOption;

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
		//�c�[���o�[�̉E�[�ɁA�T�C�Y�ύX�p�̃}�[�N������
		bool SizeGrip;

		//�c�[���`�b�v���o����悤�ɂ���
		bool Tooltip;				

		//�e�p�[�c�̃v���p�e�B
		PartsOption Parts;
	
		//�E�񂹁E���񂹂Ɋւ�����
		//���̗v�f�̒l��菬�����ʒu�̃p�[�c�����񂹁A����ȏと�E��
		INT RightJustifyIndex;

		//�E�񂹁E���񂹂Ɋւ�����
		//���񂹂ƉE�񂹂̃p�[�c��������Ƃ��Atrue=�E�񂹂���ɂ��� false=���񂹂���ɂ���
		bool RightUpper;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			SizeGrip = true;
			Tooltip = false;
			RightJustifyIndex = 256;
			RightUpper = false;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void* option )throw( )
	{
		return mNew mStatusBar;
	}

	//�p�[�c�ɕ������ݒ肷��
	//id : �ݒ��̃p�[�c(PartsOptionEntry::Id�ɃZ�b�g����������)
	//str : �Z�b�g������������
	//notab : �^�u�𖳎�����ꍇtrue
	//ret : �����ɐ���������true
	//����ID�̍��ڂ������������ꍇ�A�S���ɃZ�b�g����܂��B
	bool SetText( const WString& id , const WString& str , bool notab = true );

	//�{�[�_�[���C���̎�ނ�ύX����
	//id : �ݒ��̃p�[�c(PartsOptionEntry::Id�ɃZ�b�g����������)
	//border : �ݒ肵�����{�[�_�[���C���̎��
	//ret : �����ɐ���������true
	//����ID�̍��ڂ������������ꍇ�A�S���ɃZ�b�g����܂��B
	bool SetBorder( const WString& id , BorderType border );

	//����ύX����
	//id : �ݒ��̃p�[�c(PartsOptionEntry::Id�ɃZ�b�g����������)
	//width : �ݒ肵������
	//ret : �����ɐ���������true
	//����ID�̍��ڂ������������ꍇ�A�S���ɃZ�b�g����܂��B
	bool SetWidth( const WString& id , UINT width );

protected:
	mStatusBar();
	virtual ~mStatusBar();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

	//�E�C���h�E���������������ꍇ�ɃR�[�������
	virtual bool OnCreate( const void* opt )override;

	//�E�C���h�E�v���V�[�W��
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )override;

private:

	mStatusBar( const mStatusBar& src ) = delete;
	mStatusBar& operator=( const mStatusBar& src ) = delete;

protected:

	//�e�p�[�c�̏��
	PartsOption MyPartsOption;

	//�p�[�c��(�E�[��)�ʒu
	INT MyPartsPos[ 255 ];

	//���񂹂ƉE�񂹂̃p�[�c��������Ƃ��A�ǂ���̃p�[�c����ɂ��邩
	bool MyRightUpper;

	//�p�[�c�T�C�Y���X�V����
	//MyPartsSize�̒l���X�V���܂��B
	//ret : ���������Ƃ�true
	bool ModifyPartsSize( void );

	//���Ԗڂ̃p�[�c����E�񂹂ɂ��邩
	INT MyRightJustifyIndex;

	//ScanItem����Ăяo�����R�[���o�b�N�֐�
	//index : ���Ԗڂ̃p�[�c�ɑ΂���R�[���o�b�N�̌Ăяo���ł��邩
	//ret : ����I���ł����true
	typedef std::function< bool( INT index ) > ScanItemCallback;

	//�S�p�[�c���X�L�������AID����v���镨�ɑ΂��ăR�[���o�b�N���Ăяo��
	//id : �R�[���o�b�N�֐��Ăяo���Ώۂ�ID
	//ret : �R�[���o�b�N�֐���1�x���Ăяo����Ȃ�������true
	//      �R�[���o�b�N�֐����S��true��Ԃ�����true
	//      �R�[���o�b�N�֐���false��Ԃ������Ƃ����遨false
	//�E�R�[���o�b�N�֐���false��Ԃ����ꍇ�ł��AScanItem�̏����͒��f����܂���B
	//  ���ɂ������Ώۂ̍��ڂ�����ƁA���̍��ڂɑ΂��čĂуR�[������܂��B
	bool ScanItem( const WString& id , ScanItemCallback callback );

	//�w�肵���C���f�b�N�X�̃p�[�c���Đݒ肷��
	//index : �Đݒ�Ώۂ̃p�[�c
	//ret : �������^
	bool ModifyParts( INT index );

};

#endif	//MSTATUSBAR_H_INCLUDED

