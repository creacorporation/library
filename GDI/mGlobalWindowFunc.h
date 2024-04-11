//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�O���[�o���ȃE�C���h�E�֐��j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
�p�r�F
�O���[�o���ȃ��b�Z�[�W�v���V�[�W���B

HWND��mWindow�̑g��o�^
�@��
�S�ẴE�C���h�E���b�Z�[�W�͈�U���̃N���X�ɓ͂�
�@��
�K�؂ȃC���X�^���X��mWindow::WindowProcedure���Ăяo�����
�@��
mWindow::WindowProcedure�Ń��b�Z�[�W������

�Q�l�F
CreateMessageEx�ŌĂяo����鏇��
���������Ă�Ƃ�
(1)WM_GETMINMAXINFO
(2)WM_NCCREATE
(3)WM_NCCALCSIZE
(4)WM_CREATE
�����s���Ă���Ƃ�
(1)WM_GETMINMAXINFO
(2)WM_NCCREATE
(3)WM_NCDESTROY
*/

#ifndef MGLOBALWINDOWFUNC_H_INCLUDED
#define MGLOBALWINDOWFUNC_H_INCLUDED

#include "mStandard.h"
#include "mWindow.h"
#include <unordered_map>

class mGlobalWindowFunc
{
public:

	//���b�Z�[�W�v���V�[�W��
	//�E�C���h�E���b�Z�[�W���󂯎��Ahwnd�̒l����Ɏ󂯎��ׂ��I�u�W�F�N�g�𒲂�
	//���̃I�u�W�F�N�g�̃R�[���o�b�N���Ăяo���܂��B
	//�Y���̃I�u�W�F�N�g�����݂��Ȃ��ꍇ�́ADefWindowProcW()�ɏ��������܂��B
	//hwnd : ����E�C���h�E�n���h��
	//msg : �E�C���h�E���b�Z�[�W
	//wparam : WPARAM�̒l�i���b�Z�[�W�ɂ��Ӗ��͈قȂ�j
	//lparam : LPARAM�̒l�i���b�Z�[�W�ɂ��Ӗ��͈قȂ�j
	//���L�z���AWindows����R�[���o�b�N���邽�߂̊֐��ł��B
	//�@���[�U�A�v�����璼�ڌĂяo����SendMessage()�����s�����݂����Ȍ��ʂɂȂ�͂��B
	static LRESULT __stdcall MessageProcedure( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam );

public:
	//------------------------------------
	//�@�ȉ����������p
	//------------------------------------

	//�A�N�Z�X���ݒ�p�I�u�W�F�N�g
	//���̃N���X�̃R���X�g���N�^���Ăяo����҂ɂ̂݃A�N�Z�X����^����B
	class AttachAccessPermission
	{
	private:
		friend class mWindowCollection;		//�A�N�Z�X������N���X
		AttachAccessPermission()
		{
			return;
		}
	};

	//�E�C���h�E�n���h��(hwnd)�ƁA���̃n���h�����ẴE�C���h�E���b�Z�[�W���󂯎��
	//�I�u�W�F�N�g(win)�̑g��o�^���܂��B���̊֐��́AmWindowCollection����̂݃A�N�Z�X�������܂��B
	//hwnd : �o�^����E�C���h�E�n���h��
	//win : hwnd�Ɋ֘A�Â���I�u�W�F�N�g
	//ret : ����ɓo�^�ł����ꍇtrue
	static bool Attach( const AttachAccessPermission& perm , HWND hwnd , mWindow* win );

	//�w�肷��E�C���h�E�n���h���Ɋ֘A�Â����Ă���I�u�W�F�N�g���擾���܂��B
	//���̊֐��́AmWindowCollection����̂݃A�N�Z�X�������܂��B
	//hwnd : ��������E�C���h�E�n���h��
	//ret : �֘A�Â����Ă���I�u�W�F�N�g�B���݂��Ȃ��ꍇ��nullptr
	static mWindow* Query( const AttachAccessPermission& perm , HWND hwnd );

	//�A�N�Z�X���ݒ�p�I�u�W�F�N�g
	//���̃N���X�̃R���X�g���N�^���Ăяo����҂ɂ̂݃A�N�Z�X����^����B
	class DetachAccessPermission
	{
	private:
		friend class mWindow;	//�A�N�Z�X������N���X
		DetachAccessPermission()
		{
			return;
		}
	};

	//�w�肳�ꂽ�E�C���h�E�n���h���ƃI�u�W�F�N�g�̑g��o�^��������B
	//���̊֐��́AmWindow����̂݃A�N�Z�X�������܂��B
	//hwnd : �o�^��������E�C���h�E�n���h��
	//win : hwnd�Ɋ֘A�Â����Ă���I�u�W�F�N�g
	//ret : �w�肳�ꂽ�g�����݂��A�o�^�����ł����ꍇ��true�B
	static bool Detach( const DetachAccessPermission& perm , HWND hwnd , mWindow* win );

private:

	//HWND-mWindow�̃}�b�v
	typedef std::unordered_map<HWND,mWindow*> HandleObjMap;
	static HandleObjMap MyHandleObjMap;

};



#endif	//MGLOBALWINDOWFUNC_H_INCLUDED

