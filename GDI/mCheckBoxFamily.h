//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�`�F�b�N�{�b�N�X�n�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�`�F�b�N�{�b�N�X�n�R���g���[���̊��N���X�ł��B

�ȉ��̃R���g���[���̐e�ƂȂ�܂��B
�E�`�F�b�N�{�b�N�X
�E���W�I�{�b�N�X

*/

#ifndef MCHECKBOXFAMILY_H_INCLUDED
#define MCHECKBOXFAMILY_H_INCLUDED

#include "mWindow.h"

class mCheckboxFamily : public mWindow
{
public:

	//�`�F�b�N���
	enum CheckStatus
	{
		CHECKED,		//�`�F�b�N����Ă���
		UNCHECKED,		//�`�F�b�N����Ă��Ȃ�
		INDETERMINATE,	//3�X�e�[�g�̃O���[�ȏ��
		UNKNOWN,		//�s���i�G���[�j
	};

	//�`�F�b�N����Ă��邩���擾����
	//ret : ���݂̃`�F�b�N�̏��
	virtual CheckStatus GetCheck( void )const;

	//�`�F�b�N��t����or�O��
	//state : �V�����X�e�[�^�X
	virtual bool SetCheck( CheckStatus state );

protected:
	mCheckboxFamily();
	virtual ~mCheckboxFamily();

private:

	mCheckboxFamily( const mCheckboxFamily& src ) = delete;
	mCheckboxFamily& operator=( const mCheckboxFamily& src ) = delete;

};



#endif	//MCHECKBOXFAMILY_H_INCLUDED


