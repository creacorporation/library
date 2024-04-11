//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�q�E�C���h�E�Ǘ��j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MWINDOWCOLLECTION_H_INCLUDED
#define MWINDOWCOLLECTION_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "GDI/mWindow.h"
#include <unordered_map>

class mWindowCollection
{
public:

	//�R���X�g���N�^
	//parent : �e�I�u�W�F�N�g�����݂���ꍇ����̃|�C���^�B
	//         nullptr��n���ƁA�e�Ȃ��ƂȂ�܂��B
	mWindowCollection( mWindow* parent );

	virtual ~mWindowCollection();

	//�t�@�N�g�����\�b�h��typedef�B
	typedef mWindow* (*mWindowFactory)( const void* );

	//�R���g���[�����R���N�V�����ɒǉ�����
	//factory : mWindow*�^�̃C���X�^���X�𐶐����ĕԂ��t�@�N�g�����\�b�h
	//          mNew�ŃC���X�^���X�𐶐����ĕԂ����̂ł��邱�ƁB
	//id : �ǉ�����R���g���[�������ʂ���ID
	//     ID�͔C�ӂ̕�����ł����A�ȉ��̐���������܂��B
	//     (1)�󕶎���̓_��
	//     (2)����mWindowCollection�C���X�^���X���ŏd��������̂̓_��
	//opt : ���������C���X�^���X�Ɉ����n���I�v�V�����i���[�U��`�j
	//ret : �����ɐ��������ꍇ���������R���g���[���B���s�����ꍇnullptr
	template< class T > T* AddControl( const WString& id , const struct T::Option* opt )
	{
		return (T*)mWindowCollection::AddControlInternal( T::Factory , id , opt );
	}
	template< class T > T* AddControl( const WString& id )
	{
		return (T*)mWindowCollection::AddControlInternal( T::Factory , id , nullptr );
	}

	//�w�肵��ID�̃R���g���[�����폜����
	//id : �폜�������R���g���[����ID
	//ret : ����ɍ폜�ł����ꍇtrue
	bool RemoveControl( const WString& id );

	//�w�肵���E�C���h�E�n���h������ID���������܂��B
	//hwnd : ��������E�C���h�E�n���h��
	//ret : ID���B�Y��������̂��Ȃ������ꍇ�͋󕶎���B
	WString QueryId( HWND hwnd )const;

	//ID����R���g���[�����擾���܂��B
	//id : ��������ID
	//ret : ID���猟�������E�C���h�E�n���h���B�Y�����Ȃ������ꍇ��nullptr�B
	mWindow* Query( const WString& id )const;

	//�Ǌ����̃I�u�W�F�N�g�̃T�C�Y�𒲐�����
	//world : �N���C�A���g�G���A�̋�`(�܂��͎q�R���g���[���̕`��̈�)
	bool AdjustSize( const RECT& world );

	//�Ǌ����̃I�u�W�F�N�g�̃T�C�Y�𒲐�����
	//hwnd : �w�肵���E�C���h�E�̃N���C�A���g�̈�ɍ��킹��
	bool AdjustSize( HWND hwnd );

	//�Ǌ����̃I�u�W�F�N�g�ɃE�C���h�E���b�Z�[�W�𑗐M����
	//msg : ���M����E�C���h�E���b�Z�[�W
	//wparam : ���M���郁�b�Z�[�W��WPARAM�p�����[�^
	//lparam : ���M���郁�b�Z�[�W��LPARAM�p�����[�^
	bool ReflectMessage( UINT msg , WPARAM wparam , LPARAM lparam );

private:
	mWindowCollection() = delete;
	mWindowCollection( const mWindowCollection& src ) = delete;
	mWindowCollection& operator=( const mWindowCollection& src ) = delete;

	//�R���g���[�����R���N�V�����ɒǉ�����
	//AddControl�̓�������
	//factory : �t�@�N�g�����\�b�h
	//id : �o�^����ID
	//opt : �t�@�N�g�����\�b�h�Ő��������I�u�W�F�N�g�ɓn���I�v�V����
	//ret : ���������I�u�W�F�N�g�̃|�C���^
	mWindow* AddControlInternal( mWindowFactory factory , const WString& id , const void* opt );

protected:

	//�e�I�u�W�F�N�g
	mWindow* const MyParent;

	//�I�u�W�F�N�g�̃}�b�s���O�P
	//���F�I�u�W�F�N�g��ID
	//�E�F�I�u�W�F�N�g�ւ̃|�C���^
	typedef std::unordered_map<WString,mWindow*> IdMap;
	IdMap MyIdMap;

	//�I�u�W�F�N�g�̃}�b�s���O�Q
	//���F�E�C���h�E�n���h��
	//�E�F�I�u�W�F�N�g�ւ̃|�C���^
	typedef std::unordered_map<HWND,WString> HwndMap;
	HwndMap MyHwndMap;

	//mWindow::WindowPosition�\���̂Œ�`����A���ۂ̍��W�����߂�
	//srcpos : �ϊ�������W
	//retPos : �ϊ���̎��ۂ̍��W
	//world : �e�̃T�C�Y���i�[����RECT�\���̂ւ̃|�C���^�Bnullptr�̏ꍇ���̃E�C���h�E�̃N���C�A���g�G���A�S�̂ɂȂ�B
	//ret : ������true
	static bool CalcAbsolutePosition( const mWindow::WindowPosition& srcpos , RECT& retPos , const RECT& world );

};

#endif

