//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI���\�[�X�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI���\�[�X�̃��b�p�[�ł�
*/


#ifndef MGDIREHANDLE_H_INCLUDED
#define MGDIREHANDLE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mGdiHandle
{
public:
	/*
	���t�@�N�g�����\�b�h�쐬��
	�@�ȉ��R�[�h���p�������N���X�ɃR�s�y���A�umNew mGdiHandle�v�̕������p�������N���X���Œu�������ĉ������B
	static mGdiHandle* Factory( const Option* opt )throw( )
	{
		return mNew mGdiHandle;
	}
	*/

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const = 0;

private:
	mGdiHandle( const mGdiHandle& src ) = delete;
	mGdiHandle& operator=( const mGdiHandle& src ) = delete;

	friend class mGdiResource;

protected:
	mGdiHandle();
	virtual ~mGdiHandle();

};

#endif	//MGDIREHANDLE_H_INCLUDED

