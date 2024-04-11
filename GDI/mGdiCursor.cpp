//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�J�[�\���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#define MGDICURSOR_CPP_COMPILING
#include "mGdiCursor.h"
#include "General/mErrorLogger.h"

mGdiCursor::mGdiCursor( const Option* opt )throw( mException )
{
	MyHandle = nullptr;

	if( opt != nullptr )
	{
		//�쐬����J�[�\���̍쐬���@�ɉ���������
		if( opt->method == Option::CreateMethod::LOADFILE )
		{
			//�t�@�C����ǂݍ��ޏꍇ
			const Option_LoadFile* opt_loadfile = (const Option_LoadFile*)opt;
			CreateHandle( *opt_loadfile );
		}
		else if( opt->method == Option::CreateMethod::RESOURCE )
		{
			//�t�@�C����ǂݍ��ޏꍇ
			const Option_Resource* opt_resource = (const Option_Resource*)opt;
			CreateHandle( *opt_resource );
		}
		else
		{
			throw EXCEPTION( opt->method , L"Unknown create method" );
		}
	}

	//�J�[�\�����ł��ĂȂ�������G���[�ɂ���
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"Creating cursor failed" );
	}
}

mGdiCursor::~mGdiCursor()
{
	//�n���h���̉��
	::DestroyCursor( MyHandle );
	MyHandle = nullptr;
}

//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
mGdiCursor::operator HCURSOR()const
{
	return MyHandle;
}

//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
HGDIOBJ mGdiCursor::GetHandle( void )const
{
	return MyHandle;
}

//�J�[�\���̐���
bool mGdiCursor::CreateHandle( const Option_LoadFile& opt )
{
	//�n���h���̍쐬
	MyHandle = (HCURSOR)::LoadImageW( 
		nullptr ,			//�C���X�^���X�n���h���B���\�[�X�ǂݍ��ނ킯����Ȃ�����k��
		opt.path.c_str() ,		//�ǂݍ��ރt�@�C����
		IMAGE_CURSOR ,		//���[�h����C���[�W�̃^�C�v�i�J�[�\���j
		0 ,					//���B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		0 ,					//�����B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		LR_LOADFROMFILE );	//�t�@�C������ǂݍ��ނƂ��Ɏg���t���O

	return ( MyHandle != nullptr );
}

//�J�[�\���̐���
bool mGdiCursor::CreateHandle( const Option_Resource& opt )
{
	//���̃��W���[���̃n���h���擾
	HINSTANCE module = ::GetModuleHandleW( nullptr );	//������Ȃ��ėǂ�

	//�n���h���̍쐬
	MyHandle = (HCURSOR)::LoadImageW( 
		module ,			//�C���X�^���X�n���h���B
		opt.name.c_str() ,		//�ǂݍ��ރ��\�[�X��
		IMAGE_CURSOR ,		//���[�h����C���[�W�̃^�C�v�i�J�[�\���j
		0 ,					//���B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		0 ,					//�����B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		LR_DEFAULTCOLOR );	//���m�N���ł͂Ȃ��ꍇ�Ɏw��B

	return ( MyHandle != nullptr );
}
