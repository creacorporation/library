//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���X�g�{�b�N�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
���X�g�{�b�N�X�̃N���X�ł��B�E�C���h�E�ɓ\��t���邱�Ƃ��ł��܂��B

*/

#ifndef MLISTBOX_H_INCLUDED
#define MLISTBOX_H_INCLUDED

#include "mWindow.h"
#include "mListboxFamily.h"

class mListBox : public mListboxFamily
{
public:


	//���X�g�{�b�N�X�������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//���X�g�{�b�N�X�����̕��@
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

	//�I�v�V����
	struct Option_UseOption : public Option
	{
		WindowPosition Pos;		//�\���ʒu
		bool Enable;			//�L��/����
		bool AutoSort;			//�����\�[�g�L��
		bool Border;			//�{�[�_�[����
		bool ScrollBar;			//�X�N���[���o�[����
		bool Multiple;			//�����I���\�H
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			Enable = true;
			AutoSort = false;
			Border = true;
			ScrollBar = true;
			Multiple = false;
		}
	};

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mListBox;
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
	virtual bool MoveItem( INT index , INT moveto )override;

protected:

	mListBox();
	virtual ~mListBox();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

private:

	mListBox( const mListBox& src ) = delete;
	mListBox& operator=( const mListBox& src ) = delete;

};



#endif	//MLISTBOX_H_INCLUDED
