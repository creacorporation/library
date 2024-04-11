//----------------------------------------------------------------------------
// �񓯊����슮���ʒm�I�u�W�F�N�g
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MNOTIFYOPTION_H_INCLUDED
#define MNOTIFYOPTION_H_INCLUDED

#include "mStandard.h"

namespace Definitions_NotifyOption
{
	//�ʒm���[�h
	enum NotifyMode
	{
		NOTIFY_NONE ,
		NOTIFY_WINDOWMESSAGE ,
		NOTIFY_SIGNAL ,
		NOTIFY_CALLBACK ,
		NOTIFY_CALLBACK_PARALLEL ,
	};
};


template< typename fn > class mNotifyOption
{
public:
	using NotifyMode = Definitions_NotifyOption::NotifyMode;

	//�E�C���h�E���b�Z�[�W�Œʒm����ꍇ�̃p�����[�^
	struct NotifyMessage
	{
		HWND Hwnd;		//�ʒm��̃E�C���h�E�̃n���h��
		UINT Message;	//�ʒm���郁�b�Z�[�WID
	};

	//�ʒm���
	struct NotifierInfo
	{
		//�C�x���g�������̒ʒm���@
		//�����Ɏw�肵���l�ɉ����āANotifier�ɌĂяo���擙�̐ݒ�����Ă�������
		NotifyMode Mode;

		//�ʒm���@���E�C���h�E���b�Z�[�W�܂��̓R�[���o�b�N�֐��̎��Ɏg���郆�[�U�[��`�̒l
		//���V�O�i���I�u�W�F�N�g�Œʒm����ꍇ�͎g�p����܂���
		DWORD_PTR Parameter;

		//�C�x���g�������̒ʒm���e
		//Mode�Ɏw�肵�����e�Ɩ������Ȃ��悤�ɂ��Ă�������
		union Notifiers
		{
			fn CallbackFunction;				//�R�[���o�b�N�֐��Œʒm����ꍇ
			NotifyMessage Message;				//�E�C���h�E���b�Z�[�W�Œʒm����ꍇ
			HANDLE Handle;						//�V�O�i���I�u�W�F�N�g�Œʒm����ꍇ
		}Notifier;

		NotifierInfo()
		{
			Mode = NotifyMode::NOTIFY_NONE;
			Parameter = 0;
			Notifier.CallbackFunction = nullptr;
			MyNotifyEventCounter = 0;
		};

	private:
		friend static bool mNotifyOption<fn>::EnterNotifyEvent( const NotifierInfo& info );
		friend static bool mNotifyOption<fn>::LeaveNotifyEvent( const NotifierInfo& info );

		mutable volatile LONG MyNotifyEventCounter;

	};

	static bool EnterNotifyEvent( const NotifierInfo& info )
	{
		LONG val = InterlockedIncrement( &info.MyNotifyEventCounter );
		if( val == 1 )
		{
			return true;
		}
		return false;
	}

	static bool LeaveNotifyEvent( const NotifierInfo& info )
	{
		LONG val = InterlockedExchange( &info.MyNotifyEventCounter , 0 );
		if( val == 1 )
		{
			return false;
		}
		return true;
	}

	static bool IsCallback( NotifyMode mode )
	{
		return ( mode == NotifyMode::NOTIFY_CALLBACK ) || ( mode == NotifyMode::NOTIFY_CALLBACK_PARALLEL );
	}
};



#endif
