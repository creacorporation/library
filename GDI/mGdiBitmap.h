//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�r�b�g�}�b�v�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI�̃r�b�g�}�b�v�ł��B
�E�쐬����r�b�g�}�b�v�́AOption�\���̂Ŏw�肵�܂��B
*/

/*
���g����
[��1]test.bmp��ǂݍ���ŁAresource�ɓo�^���܂��B
mGdiResource resource;
mGdiBitmap::Option_LoadFile opt;
opt.path = L"d:\\test.bmp";
resource.AddItem<mGdiBitmap>( L"IMAGE", &opt );

[��2]���ڃr�b�g�}�b�v��ǂݍ��݂܂�
    (���ǂݍ��ݎ��s���R���X�g���N�^�����O�𓊂���̂Œ���)
mGdiBitmap::Option_LoadFile opt;
opt.path = L"d:\\test.bmp";
mGdiBitmap bmp( &opt );

*/

#ifndef MGDIBITMAP_H_INCLUDED
#define MGDIBITMAP_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "mGdiDC.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

namespace Definitions_mGdiBitmap
{
	enum Option_CreateMethod
	{
		NEWBITMAP,	//�f�o�C�X�R���e�L�X�g���w�肵�āA�݊��̃r�b�g�}�b�v��V�K�ɍ쐬����
		LOADFILE,	//�t�@�C�������[�h����
		RESOURCE,	//���\�[�X�����[�h����
	};
};

class mGdiBitmap : public mGdiHandle
{
public:


	//�I�v�V�����\����
	//�r�b�g�}�b�v���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_NewBitmap �c �f�o�C�X�R���e�L�X�g�ƌ݊��̃r�b�g�}�b�v���쐬�������Ƃ�
	//�EOption_LoadFile �c �r�b�g�}�b�v�t�@�C����ǂݍ��݂����Ƃ�
	//�EOption_Resource �c ���\�[�X�����[�h�������Ƃ�
	struct Option
	{
		//�r�b�g�}�b�v�����̕��@
		using CreateMethod = Definitions_mGdiBitmap::Option_CreateMethod;
		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//BMP_NEWBITMAP���w�肵�ăr�b�g�}�b�v���쐬����Ƃ��̃I�v�V����
	//srcdc��nullptr�ɂ���ƁA�f�X�N�g�b�v�̌݊��r�b�g�}�b�v�𐶐����܂��B
	struct Option_NewBitmap : public Option
	{
		//�R�R�Ɏw�肵���f�o�C�X�R���e�L�X�g�ƐF���Ȃǂ��݊��ɂȂ�
		const mGdiDC* srcdc;
		//�r�b�g�}�b�v�̕�
		DWORD width;
		//�r�b�g�}�b�v�̍���
		DWORD height;

		Option_NewBitmap() : Option( CreateMethod::NEWBITMAP )
		{
			srcdc = nullptr;
			width = 100;
			height = 100;
		}
	};

	//�t�@�C�������[�h���ăr�b�g�}�b�v���쐬����Ƃ��̃I�v�V����
	struct Option_LoadFile : public Option
	{
		//���[�h����t�@�C����
		WString path;

		Option_LoadFile() : Option( CreateMethod::LOADFILE )
		{
		}
	};

	//���\�[�X�����[�h���ăr�b�g�}�b�v���쐬����Ƃ��̃I�v�V����
	struct Option_Resource : public Option
	{
		//���[�h���郊�\�[�X��
		WString name;

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
			result = mNew mGdiBitmap( (const Option*)opt );
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
	mGdiBitmap( const Option* opt )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiBitmap();
	
	//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
	operator HBITMAP()const;

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const override;

	//�r�b�g�}�b�v�̃T�C�Y���擾����
	bool GetSize( SIZE& retSize )const noexcept;

	//�r�b�g�}�b�v�̏����擾����
	bool GetInfo( BITMAP& retInfo )const noexcept;

private:

	//�ȉ��A�f�t�H���g�n�͎̂g�p�s�Ƃ���
	mGdiBitmap() = delete;
	mGdiBitmap( const mGdiBitmap& src ) = delete;
	mGdiBitmap& operator=( const mGdiBitmap& src ) = delete;

	//�r�b�g�}�b�v�̐���
	//�EOption_NewBitmap���g�p����Ƃ��p
	bool CreateHandle( const Option_NewBitmap& opt );

	//�r�b�g�}�b�v�̐���
	//�EOption_LoadFile���g�p����Ƃ��p
	bool CreateHandle( const Option_LoadFile& opt );

	//�r�b�g�}�b�v�̐���
	//�EOption_Resource���g�p����Ƃ��p
	bool CreateHandle( const Option_Resource& opt );


protected:

	//�n���h���̎���
	HBITMAP MyHandle;

};

#endif	//MGDIBITMAP_H_INCLUDED




