//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i���X�g�{�b�N�X�n�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
���X�g�{�b�N�X�n�̃R���g���[���̊��N���X�ł��B
*/

#ifndef MLISTBOXFAMILY_H_INCLUDED
#define MLISTBOXFAMILY_H_INCLUDED

#include "mWindow.h"
#include <unordered_map>
#include <vector>

class mListboxFamily : public mWindow
{
public:

	//���X�g�{�b�N�X�̍��ڂɊ֘A�Â����Ă���f�[�^
	struct ItemDataEntry
	{
		ULONG_PTR data;		//���[�U��`�̒l
		WString str;		//���[�U��`�̕�����(���X�g�{�b�N�X�ɂ͕\������Ȃ�)
	};

	//�A�C�e����ǉ�����
	//caption : ���X�g�{�b�N�X���ɕ\�����镶����
	//data : ���[�U��`�̃f�[�^
	//index : �}������ʒu�i�w�肷��Ǝ����\�[�g�͓������A�w��ʒu�ɑ}������܂��B-1�ɂ���Ɩ����B�j
	//ret : ����I���̏ꍇ�^
	//�E���łɂ��鍀��(caption)�𕡐��o�^����ƁA�R���{�{�b�N�X�ɂ͓������ڂ��������o�����܂����A
	//  �擾�ł���ItemDataEntry�͋��p�ƂȂ�A�Ō�ɓo�^�������̂ɂȂ�܂��B
	virtual bool AddItem( const WString& caption ) = 0;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data ) = 0;
	virtual bool AddItem( const WString& caption , INT index ) = 0;
	virtual bool AddItem( const WString& caption , const ItemDataEntry& data , INT index ) = 0;

	//�A�C�e�����폜����
	//index : �폜�Ώ�
	//ret : �폜���ꂽ�ꍇ�͐^�B�폜����Ȃ������ꍇ�͋U�B
	virtual bool RemoveItem( INT index ) = 0;

	//�I������Ă���C���f�b�N�X�̈ꗗ���擾����
	typedef std::vector<INT> SelectedItems;
	virtual SelectedItems GetSelected( void )const = 0;

	//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
	//items : �I����Ԃɂ���A�C�e���̈ꗗ
	//ret : �������^�A���s���U�B
	virtual bool SetSelected( const SelectedItems& items ) = 0;

	//�w�肵���C���f�b�N�X�̃A�C�e����I����Ԃɂ���
	//item : �I����Ԃɂ���A�C�e��
	//ret : �������^�A���s���U�B
	virtual bool SetSelected( INT item ) = 0;

	//�w�肵���L���v�V���������A�C�e���̃C���f�b�N�X��Ԃ�
	//caption : �T�������A�C�e���̃L���v�V����
	//ret : �w��L���v�V���������A�C�e���̃C���f�b�N�X�B������Ȃ��ꍇ���̐��B
	//�����L���v�V����������������������ꍇ�A�ŏ��Ɍ��������C���f�b�N�X��Ԃ��܂��B
	virtual INT SearchItem( const WString& caption ) = 0;

	//�w��C���f�b�N�X�̃L���v�V�������擾����
	//index : �擾�������ʒu
	//ret : �w�肵���ʒu�ɂ���A�C�e���̃L���v�V�����B�A�C�e�����Ȃ��ꍇ�͋󕶎���B
	virtual WString GetItemCaption( INT index )const = 0;

	//�w��C���f�b�N�X�Ɋ֘A�Â����Ă���f�[�^���擾����
	//index : �擾�������ʒu
	//retdata : �w�肵���ʒu�ɂ���A�C�e���̃f�[�^�̊i�[��
	//ret : �������^�A���s���U�B
	//���w��C���f�b�N�X�Ɋ֘A�t���Ă���f�[�^�����݂��Ȃ��ꍇ�A�U���Ԃ�
	virtual bool GetItemData( INT index , ItemDataEntry& retdata )const = 0;

	//�����A�C�e�������邩���J�E���g����
	//ret : ���݂���A�C�e���̐�
	//�G���[�̏ꍇ�͕��̐��B
	virtual INT GetItemCount( void )const = 0;

	//�A�C�e�����ړ�����
	//index�Ŏw�肵���A�C�e�����Amoveto�Ŏw�肵���ʒu�Ɉړ����܂�
	//ret : �������^�A���s���U�B
	virtual bool MoveItem( INT index , INT moveto ) = 0;

protected:
	mListboxFamily();
	virtual ~mListboxFamily();

	//�A�C�e���̕�����Ɗ֘A�Â����Ă���f�[�^�̃}�b�v
	typedef std::unordered_map<WString,ItemDataEntry> ItemData;
	ItemData MyItemData;

private:

	mListboxFamily( const mListboxFamily& src ) = delete;
	mListboxFamily& operator=( const mListboxFamily& src ) = delete;

};



#endif	//MLISTBOXFAMILY_H_INCLUDED


