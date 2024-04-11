//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI�r�b�g�}�b�v�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIBITMAP_CPP_COMPILING
#include "mGdiBitmap.h"
#include "General/mErrorLogger.h"
#include "mGdiMemDC.h"

mGdiBitmap::mGdiBitmap( const Option* opt )throw( mException )
{
	MyHandle = nullptr;

	if( opt != nullptr )
	{
		//�쐬����r�b�g�}�b�v�̎�ނɉ���������
		if( opt->method == Option::CreateMethod::NEWBITMAP )
		{
			//�f�o�C�X�R���e�L�X�g���琶������ꍇ
			const Option_NewBitmap* opt_bitmap = (const Option_NewBitmap*)opt;
			CreateHandle( *opt_bitmap );
		}
		else if( opt->method == Option::CreateMethod::LOADFILE )
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

	//�r�b�g�}�b�v���ł��ĂȂ�������G���[�ɂ���
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"Creating bitmap failed" );
	}
}

mGdiBitmap::~mGdiBitmap()
{
	//�n���h���̉��
	DeleteObject( MyHandle );
	MyHandle = nullptr;
}

//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
mGdiBitmap::operator HBITMAP()const
{
	return MyHandle;
}

//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
HGDIOBJ mGdiBitmap::GetHandle( void )const
{
	return MyHandle;
}

//�r�b�g�}�b�v�̐���
bool mGdiBitmap::CreateHandle( const Option_NewBitmap& opt )
{
	if( opt.srcdc == nullptr )
	{
		//srcdc��nullptr�̏ꍇ�́A�f�X�N�g�b�v�ƌ݊��̃r�b�g�}�b�v�����
		HWND hwnd = ::GetDesktopWindow();
		HDC hdc = ::GetDC( hwnd );
		MyHandle = ::CreateCompatibleBitmap( hdc , opt.width , opt.height );
		::ReleaseDC( hwnd , hdc );
		//GetDesktopWindow�Ŏ擾�����n���h���͉�����Ȃ��Ă���
	}
	else
	{
		//�S���w�肪����̂ŁA��������ɍ��
		MyHandle = ::CreateCompatibleBitmap( opt.srcdc->MyHdc , opt.width , opt.height );
	}
	return ( MyHandle != nullptr );
}

//�r�b�g�}�b�v�̐���
bool mGdiBitmap::CreateHandle( const Option_LoadFile& opt )
{

	MyHandle = (HBITMAP)::LoadImageW( 
		nullptr ,			//�C���X�^���X�n���h���B���\�[�X�ǂݍ��ނ킯����Ȃ�����k��
		opt.path.c_str() ,		//�ǂݍ��ރt�@�C����
		IMAGE_BITMAP ,		//���[�h����C���[�W�̃^�C�v�i�r�b�g�}�b�v�j
		0 ,					//���B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		0 ,					//�����B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		LR_LOADFROMFILE );	//�t�@�C������ǂݍ��ނƂ��Ɏg���t���O

	return ( MyHandle != nullptr );
}

//�r�b�g�}�b�v�̐���
bool mGdiBitmap::CreateHandle( const Option_Resource& opt )
{
	//���̃��W���[���̃n���h���擾
	HINSTANCE module = ::GetModuleHandleW( nullptr );	//������Ȃ��ėǂ�

	MyHandle = (HBITMAP)::LoadImageW( 
		module ,			//�C���X�^���X�n���h���B
		opt.name.c_str() ,		//�ǂݍ��ރ��\�[�X��
		IMAGE_BITMAP ,		//���[�h����C���[�W�̃^�C�v�i�r�b�g�}�b�v�j
		0 ,					//���B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		0 ,					//�����B0�̏ꍇ�A�C���[�W�T�C�Y�ɍ��킹��B
		LR_DEFAULTCOLOR );	//���m�N���ł͂Ȃ��ꍇ�Ɏw��B

	return ( MyHandle != nullptr );
}

bool mGdiBitmap::GetSize( SIZE& retSize )const noexcept
{
	BITMAP bmp;
	if( !GetInfo( bmp ) )
	{
		return false;
	}
	retSize.cx = bmp.bmWidth;
	retSize.cy = bmp.bmHeight;
	return true;

}

bool mGdiBitmap::GetInfo( BITMAP& retInfo )const noexcept
{
	if( !GetObject( MyHandle , sizeof( BITMAP ) , &retInfo ) )
	{
		return false;
	}
	return true;
}


