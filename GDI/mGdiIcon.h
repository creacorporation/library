//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�A�C�R���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MGDIICON_H_INCLUDED
#define MGDIICON_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

class mGdiIcon : public mGdiHandle
{
public:


	//�I�v�V�����\����
	//�A�C�R���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_LoadFile �c �r�b�g�}�b�v�t�@�C����ǂݍ��݂����Ƃ�
	//�EOption_Resource �c ���\�[�X�����[�h�������Ƃ�
	struct Option
	{
		//�A�C�R�������̕��@
		enum CreateMethod
		{
			LOADFILE,	//�t�@�C�������[�h����
			RESOURCE,	//���\�[�X�����[�h����
		};
		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�t�@�C�������[�h���ăA�C�R���쐬����Ƃ��̃I�v�V����
	struct Option_LoadFile : public Option
	{
		WString path;			//���[�h����t�@�C����
		Option_LoadFile() : Option( CreateMethod::LOADFILE )
		{
		}
	};

	//���\�[�X�����[�h���ăA�C�R���쐬����Ƃ��̃I�v�V����
	struct Option_Resource : public Option
	{
		WString name;			//���[�h���郊�\�[�X��
		Option_Resource() : Option( CreateMethod::RESOURCE )
		{
		}
	};

public:

	//�t�@�N�g�����\�b�h
	//opt�͕K���w�肵�Ă��������B�G���[�ɂȂ�nullptr��Ԃ��܂��B
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			result = mNew mGdiIcon( (const Option*)opt );
		}
		catch( mException )
		{
			//nullptr��Ԃ��ƁA�t�@�N�g�����\�b�h�̌Ăяo���������s����
			result = nullptr;
		}
		return result;
	}

	//�R���X�g���N�^
	//���̃R���X�g���N�^�́AMyHandle�Ɋi�[����r�b�g�}�b�v�̐������s���ɗ�O�𓊂��܂��B
	//�Eopt�͕K���w�肵�ĉ������Bnullptr��n���Ɨ�O�𓊂��܂��B
	mGdiIcon( const Option* opt )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiIcon();
	
	//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
	operator HICON()const;

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//�ȉ��A�f�t�H���g�n�͎̂g�p�s�Ƃ���
	mGdiIcon() = delete;
	mGdiIcon( const mGdiIcon& src ) = delete;
	mGdiIcon& operator=( const mGdiIcon& src ) = delete;

	//�J�[�\������
	//�EOption_LoadFile���g�p����Ƃ��p
	bool CreateHandle( const Option_LoadFile& opt );

	//�J�[�\������
	//�EOption_Resource���g�p����Ƃ��p
	bool CreateHandle( const Option_Resource& opt );

protected:

	//�n���h���̎���
	HICON MyHandle;

};

#endif	//MGDIICON_H_INCLUDED

