//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�R���{�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�R���{�{�b�N�X�̃N���X�ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B

�E�����\�[�g�A�X�^�C���̕ύX�͏o���܂���i���䂪�ς���Ă��܂����߁j
*/

#ifndef MCOMBOBOX_H_INCLUDED
#define MCOMBOBOX_H_INCLUDED

#include "mWindow.h"
#include "mListboxFamily.h"

class mComboBox : public mListboxFamily
{
public:

	//�R���{�{�b�N�X�������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
	//�R���{�{�b�N�X�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		//�X�^�C��
		enum ControlStyle
		{
			SIMPLE,			//�y�V���v���z�G�f�B�b�g�R���g���[���{��ɕ\������Ă��郊�X�g
			DROPDOWN,		//�y�h���b�v�_�E���z�G�f�B�b�g�R���g���[���{�h���b�v�_�E�����郊�X�g
			DROPDOWNLIST,	//�y�h���b�v�_�E�����X�g�z�ҏW�s�\�̃e�L�X�g�{�h���b�v�_�E�����郊�X�g
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�I�v�V����
	struct Option_UseOption : public Option
	{
		ControlStyle Style;		//�X�^�C��
		WindowPosition Pos;		//�\���ʒu
		bool Enable;			//�L��/����
		bool AutoSort;			//�����\�[�g�L��
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Style = ControlStyle::DROPDOWNLIST;
			Enable = true;
			AutoSort = false;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mComboBox;
	}

	//�A�C�e����ǉ�����
	virtual bool AddItem( const WString& caption )override;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data )override;
	virtual bool AddItem( const WString& caption , INT index )override;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data , INT index )override;

	//�A�C�e�����폜����
	virtual bool RemoveItem( INT index )override;

	//�I������Ă���C���f�b�N�X�̈ꗗ���擾����
	virtual SelectedItems GetSelected( void )const override;

	//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
	virtual bool SetSelected( const SelectedItems& items )override;

	//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
	virtual bool SetSelected( INT item )override;

	//�w�肵���L���v�V���������A�C�e���̃C���f�b�N�X��Ԃ�
	virtual INT SearchItem( const WString& caption )override;

	//�w��C���f�b�N�X�̃L���v�V�������擾����
	virtual WString GetItemCaption( INT index )const override;

	//�w��C���f�b�N�X�Ɋ֘A�Â����Ă���f�[�^���擾����
	virtual bool GetItemData( INT index , ItemDataEntry& retdata )const override;

	//�����A�C�e�������邩���J�E���g����
	virtual INT GetItemCount( void )const override;

	//�A�C�e�����ړ�����
	virtual bool MoveItem( INT index , INT moveto ) override;

protected:

	mComboBox();
	virtual ~mComboBox();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mComboBox( const mComboBox& src ) = delete;
	mComboBox& operator=( const mComboBox& src ) = delete;

};



#endif	//MCOMBOBOX_H_INCLUDED
