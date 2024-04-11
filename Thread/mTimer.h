//----------------------------------------------------------------------------
// �^�C�}�[�R���g���[��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MTIMER_H_INCLUDED
#define MTIMER_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"

class mTimer
{
public:
	mTimer();
	virtual ~mTimer();


	//�I�v�V�����\����
	struct NotificationOption
	{
	public:
		//�ʒm���@
		enum NotificationMethod
		{
			METHOD_NONE,				//�������Ȃ�
			METHOD_WINDOW_MESSAGE,		//�E�C���h�E���b�Z�[�W
			METHOD_CALLBACK_FUNCTION,	//�R�[���o�b�N�֐�
			METHOD_SIGNAL_OBJECT,		//�V�O�i���I�u�W�F�N�g
		};
		const NotificationMethod Method;

		DWORD Interval;			//����Ԋu(ms�P��)
		int Count;				//�c���(���̐��̏ꍇ����)
		bool StartImmediate;	//�^�Ȃ炷���J�n����

	protected:
		NotificationOption() = delete;
		NotificationOption( NotificationMethod method ) : Method( method )
		{
			Interval = 1000;
			Count = -1;
			StartImmediate = true;
		}
	};

	//�����ʒm���Ȃ��ꍇ�̃I�v�V����
	struct Option_None : public NotificationOption
	{
		Option_None() : NotificationOption( NotificationMethod::METHOD_NONE )
		{
		}
	};
	//�E�C���h�E���b�Z�[�W���g�p���Ēʒm����ꍇ�̃I�v�V����
	struct Option_WindowMessage : public NotificationOption
	{
		// wparam : �ďo����mTimer�ւ̃|�C���^
		// lparam : Parameter�̒l

		//���b�Z�[�W�𑗂�E�C���h�E
		HWND Sendto;
		//�ڑ��������ɓ�����E�C���h�E���b�Z�[�W�ԍ�
		UINT OnTimer;
		//LPARAM�ɓn���l(�C��)
		LPARAM Parameter;

		Option_WindowMessage() : NotificationOption( NotificationMethod::METHOD_WINDOW_MESSAGE )
		{
			Sendto = 0;
			OnTimer = WM_TIMER;
			Parameter = 0;
		}
	};

	//�f�[�^��M���ɃR�[���o�b�N�֐����Ăяo���Ēʒm����ꍇ�̃I�v�V����
	struct Option_CallbackFunction : public NotificationOption
	{
		using CallbackFunction = void(*)( mTimer& timer , DWORD_PTR parameter , int count );

		CallbackFunction OnTimer;
		DWORD_PTR Parameter;

		Option_CallbackFunction() : NotificationOption( NotificationMethod::METHOD_CALLBACK_FUNCTION )
		{
			OnTimer = nullptr;
			Parameter = 0;
		}
	};

	//�f�[�^��M���ɃV�O�i���I�u�W�F�N�g���g�p���Ēʒm����ꍇ�̃I�v�V����
	struct Option_SignalObject : public NotificationOption
	{
		HANDLE OnTimer;		//�^�C�}�������ɃV�O�i����Ԃɂ���I�u�W�F�N�g

		Option_SignalObject() : NotificationOption( NotificationMethod::METHOD_SIGNAL_OBJECT )
		{
			OnTimer = INVALID_HANDLE_VALUE;
		}
	};

	//�^�C�}�[�̃Z�b�g�A�b�v
	// opt : �ݒ�l
	bool Setup( const NotificationOption& opt );

	//�^�C�}�[���ĊJ�n����
	//�E�c��񐔂����Z�b�g���܂���
	//�E�c��񐔂��[�����ƊJ�n���܂���
	bool Restart( void );

	//�^�C�}�[�����Z�b�g���ĊJ�n����
	//�E�c��񐔂��Z�b�g�A�b�v�����Ƃ��̒l�Ƀ��Z�b�g���Ă���J�n���܂�
	bool Start( void );

	//�^�C�}�[���~����
	bool Stop( void );

	//�^�C�}�[�Ăяo�����s���c��񐔂��Z�b�g����
	// newval : �c���(���̐��������j
	//�E���݂̉񐔂ɑ΂��Đݒ肷��
	//�E�Z�b�g�A�b�v�����Ƃ��̐ݒ���㏑��������̂ł͂Ȃ�
	bool SetCount( int newval );

	//�^�C�}�[�Ăяo�����Z�b�g�A�b�v�����Ƃ��̒l�Ƀ��Z�b�g����
	bool SetCount( void );

	//���݂̎c��񐔂��擾����
	int GetCount( void )const;

private:

	mTimer( const mTimer& src ) = delete;
	mTimer& operator=( const mTimer& src ) = delete;

	//�ݒ�l
	std::unique_ptr< NotificationOption > MyOption;

	//�������[�`��
	static VOID CALLBACK TimerRoutine( PVOID param , BOOLEAN istimer );

	//�c��Ăяo����
	volatile long MyCount;

	//�^�C�}�[�L���[
	HANDLE MyHandle;


};

#endif
