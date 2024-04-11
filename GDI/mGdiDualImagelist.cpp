//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�Q�C���[�W�̃C���[�W���X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIDUALIMAGELIST_CPP_COMPILING
#include "mGdiDualImagelist.h"
#include "General/mErrorLogger.h"

mGdiDualImagelist::mGdiDualImagelist( const Option* opt )throw( mException )
{
	MyHandle = mNew mGdiImagelist( opt );
	MyHandle2 = ImageList_Duplicate( (HIMAGELIST)MyHandle->GetHandle() );
	if( MyHandle2 == nullptr )
	{
		throw EXCEPTION( 0 , L"ImageList_Duplicate failed" );
	}
	return;
}

mGdiDualImagelist::~mGdiDualImagelist()
{
	//�n���h���̉��
	::ImageList_Destroy( MyHandle2 );
	MyHandle2 = nullptr;
	mDelete MyHandle;
	MyHandle = nullptr;
}

//�n���h���̒l���擾����i�P�߂̃C���[�W���X�g�̃n���h�����擾����j
HGDIOBJ mGdiDualImagelist::GetHandle( void )const
{
	return MyHandle->GetHandle();
}

//�n���h���̒l���擾����i�Q�߂̃C���[�W���X�g�̃n���h�����擾����j
HGDIOBJ mGdiDualImagelist::GetHandle2( void )const
{
	return MyHandle2;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiDualImagelist::AddImage( const WString& id ,
	const mGdiBitmap& img1 ,
	const mGdiBitmap& img2 ,
	const mGdiBitmap* mask1 ,
	const mGdiBitmap* mask2 )
{
	//�܂��A�P�Ԗڂ̃C���[�W���X�g�ɓo�^
	if( !MyHandle->AddImage( id , img1 , mask1 ) )
	{
		//�G���[���b�Z�[�W�͂P�Ԗڂ̕��œo�^���Ă��邩��R�R�ł͉������Ȃ��B
		return false;
	}

	//�P�Ԗڂ����������̂ŁA�Q�Ԗڂɓo�^
	INT index = -1;
	if( mask2 != nullptr )
	{
		//�}�X�N������ꍇ
		index = ::ImageList_Add( MyHandle2 , img2 , *mask2 );
	}
	else
	{
		//�}�X�N���Ȃ��ꍇ
		index = ::ImageList_Add( MyHandle2 , img2 , nullptr );
	}

	//�Q�Ԗڂ̓o�^�͐������������H
	if( index < 0 )
	{
		//���s���Ă���̂ŁA�P�Ԗڂ̓o�^��������
		MyHandle->RemoveImage( id );
		//�G���[���b�Z�[�W��o�^
		RaiseAssert( g_ErrorLogger , index , L"ImageList_Add(2) failed" );
		return false;
	}
	return true;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������(mGdiResource����C���[�W�𒊏o����Ver)
bool mGdiDualImagelist::AddImageBitmap( const mGdiResource& res ,
	const WString& id ,
	const WString& img1 ,
	const WString& img2 ,
	const WString& mask1 ,
	const WString& mask2 )
{
	//mGdiResource����w��̃C���[�W�𒊏o����
	//���P��
	const mGdiBitmap* bmp_img1 = res.GetItem< mGdiBitmap >( img1 );
	if( bmp_img1 == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 1 , L"Image id not found : " + img1 );
		return false;
	}
	//�Q��
	const mGdiBitmap* bmp_img2 = res.GetItem< mGdiBitmap >( img2 );
	if( bmp_img2 == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 2 , L"Image id not found : " + img2 );
		return false;
	}

	//�C���[�W��2�Ƃ���ꂽ����A���x�̓}�X�N���擾����B�}�X�N�͑��݂��Ȃ��Ă��G���[�ɂ͂��Ȃ��B
	//�P��
	const mGdiBitmap* bmp_mask1 = nullptr;
	if( mask1 != L"" )
	{
		bmp_mask1 = res.GetItem< mGdiBitmap >( mask1 );
	}
	//�Q��
	const mGdiBitmap* bmp_mask2 = nullptr;
	if( mask2 != L"" )
	{
		bmp_mask2 = res.GetItem< mGdiBitmap >( mask2 );
	}

	//�ǉ������B
	return AddImage( id , *bmp_img1 , *bmp_img2 , bmp_mask1 , bmp_mask2 );
}


//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiDualImagelist::AddImage( const WString& id ,
	const mGdiBitmap& img1 ,
	const mGdiBitmap& img2 ,
	COLORREF mask1 ,
	COLORREF mask2 )
{
	//�܂��A�P�Ԗڂ̃C���[�W���X�g�ɓo�^
	if( !MyHandle->AddImage( id , img1 , mask1 ) )
	{
		//�G���[���b�Z�[�W�͂P�Ԗڂ̕��œo�^���Ă��邩��R�R�ł͉������Ȃ��B
		return false;
	}

	//�Q�Ԗڂ̃C���[�W���X�g�ɒǉ��������s��
	int index = -1;
	index = ::ImageList_AddMasked( MyHandle2 , img2 , mask2 );

	//�Q�Ԗڂ̓o�^�͐������������H
	if( index < 0 )
	{
		//���s���Ă���̂ŁA�P�Ԗڂ̓o�^��������
		MyHandle->RemoveImage( id );
		//�G���[���b�Z�[�W��o�^
		RaiseAssert( g_ErrorLogger , index , L"ImageList_AddMasked(2) failed" );
		return false;
	}
	return true;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������(mGdiResource����C���[�W�𒊏o����Ver)
bool mGdiDualImagelist::AddImageBitmap( const mGdiResource& res ,
	const WString& id ,
	const WString& img1 ,
	const WString& img2 ,
	COLORREF mask1 ,
	COLORREF mask2 )
{
	//mGdiResource����w��̃C���[�W�𒊏o����
	//���P��
	const mGdiBitmap* bmp_img1 = res.GetItem< mGdiBitmap >( img1 );
	if( bmp_img1 == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 1 , L"Image id not found : " + img1 );
		return false;
	}
	//�Q��
	const mGdiBitmap* bmp_img2 = res.GetItem< mGdiBitmap >( img2 );
	if( bmp_img2 == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 2 , L"Image id not found : " + img2 );
		return false;
	}
	return AddImage( id , *bmp_img1 , *bmp_img2 );
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiDualImagelist::AddImage( const WString& id ,
	const mGdiIcon& img1 ,
	const mGdiIcon& img2 )
{
	//�܂��A�P�Ԗڂ̃C���[�W���X�g�ɓo�^
	if( !MyHandle->AddImage( id , img1 ) )
	{
		//�G���[���b�Z�[�W�͂P�Ԗڂ̕��œo�^���Ă��邩��R�R�ł͉������Ȃ��B
		return false;
	}
	//�Q�Ԗڂ̃C���[�W���X�g�ɒǉ��������s��
	int index = -1;
	index = ::ImageList_AddIcon( MyHandle2 , img2 );

	//�Q�Ԗڂ̓o�^�͐������������H
	if( index < 0 )
	{
		//���s���Ă���̂ŁA�P�Ԗڂ̓o�^��������
		MyHandle->RemoveImage( id );
		//�G���[���b�Z�[�W��o�^
		RaiseAssert( g_ErrorLogger , index , L"ImageList_AddIcon(2) failed" );
		return false;
	}
	return true;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������(mGdiResource����C���[�W�𒊏o����Ver)
bool mGdiDualImagelist::AddImageIcon( const mGdiResource& res ,
	const WString& id ,
	const WString& img1 ,
	const WString& img2 )
{
	//mGdiResource����w��̃C���[�W�𒊏o����
	//���P��
	const mGdiIcon* icon_img1 = res.GetItem< mGdiIcon >( img1 );
	if( icon_img1 == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 1 , L"Image id not found : " + img1 );
		return false;
	}
	//�Q��
	const mGdiIcon* icon_img2 = res.GetItem< mGdiIcon >( img2 );
	if( icon_img2 == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 2 , L"Image id not found : " + img2 );
		return false;
	}
	return AddImage( id , *icon_img1 , *icon_img2 );
}

//�C���[�W���X�g����C���[�W���폜����
bool mGdiDualImagelist::RemoveImage( const WString& id )
{
	//��������C���[�W�̃C���f�b�N�X���擾
	INT index = MyHandle->GetIndex( id );
	if( index < 0 )
	{
		RaiseAssert( g_ErrorLogger , index , L"Id not found : " + id );
		return false;
	}

	//�P�Ԗڂ������
	if( !MyHandle->RemoveImage( id ) )
	{
		//�G���[���b�Z�[�W�͂P�Ԗڂ̒��g�œo�^���Ă��邩��R�R�ł͉������Ȃ��B
		return false;
	}

	//�Q�Ԗڂ̃C���[�W���X�g����폜
	if( !::ImageList_Remove( MyHandle2 , index ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_Remove(2) failed" );
		return false;
	}
	return true;
}

//ID����C���f�b�N�X���擾����
INT mGdiDualImagelist::GetIndex( const WString& id )const
{
	return MyHandle->GetIndex( id );
}


