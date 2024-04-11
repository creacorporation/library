//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�C���[�W���X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIIMAGELIST_CPP_COMPILING
#include "mGdiImagelist.h"
#include "General/mErrorLogger.h"

mGdiImagelist::mGdiImagelist( const Option* option )throw( mException )
{
	MyHandle = nullptr;

	if( option != nullptr )
	{
		if( option->method == Option::CreateMethod::USEOPTION )
		{
			CreateMyHandle( *(const Option_UseOption*)option );
		}
	}

	//�A�C�R�����ł��ĂȂ�������G���[�ɂ���
	if( MyHandle == nullptr )
	{
		throw EXCEPTION( 0 , L"Creating cursor failed" );
	}
}

mGdiImagelist::~mGdiImagelist()
{
	//�n���h���̉��
	::ImageList_Destroy( MyHandle );
	MyHandle = nullptr;
}

//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
mGdiImagelist::operator HIMAGELIST()const
{
	return MyHandle;
}

//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
HGDIOBJ mGdiImagelist::GetHandle( void )const
{
	return MyHandle;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiImagelist::AddImage( const WString& id , const mGdiBitmap& img , const mGdiBitmap* mask )
{
	//ID���󗓂ł͂Ȃ����`�F�b�N
	if( id == L"" )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not presented" );
		return false;
	}

	//���łɑ��݂���ID�̏ꍇ�̓G���[�ɂ���
	if( MyIdIndexMap.count( id ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id already exist : " + id );
		return false;
	}

	//���݂��Ȃ��̂ŁA�ǉ��������s��
	int index = -1;
	if( mask != nullptr )
	{
		index = ::ImageList_Add( MyHandle , img , *mask );
	}
	else
	{
		index = ::ImageList_Add( MyHandle , img , nullptr );
	}

	//�G���[�������H
	if( index < 0 )
	{
		//�G���[�Ȃ̂ŏI��
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_Add failed" );
		return false;
	}

	//����I���������̂�ID���}�b�v�ɒǉ�
	MyIdIndexMap.insert( IdIndexMap::value_type( id , index ) );
	return true;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiImagelist::AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , const WString& mask )
{
	//mGdiResource����w��̃C���[�W�𒊏o����
	const mGdiBitmap* bmp_img = res.GetItem< mGdiBitmap >( img );
	if( bmp_img == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 0 , L"Image id not found : " + img );
		return false;
	}
	//���l�Ƀ}�X�N���擾����B�}�X�N�͑��݂��Ȃ��Ă��G���[�ɂ͂��Ȃ��B
	const mGdiBitmap* bmp_mask = nullptr;
	if( mask != L"" )
	{
		bmp_mask = res.GetItem< mGdiBitmap >( mask );
	}

	//�ǉ������B
	return AddImage( id , *bmp_img , bmp_mask );
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiImagelist::AddImage( const WString& id , const mGdiBitmap& img , COLORREF mask )
{
	//ID���󗓂ł͂Ȃ����`�F�b�N
	if( id == L"" )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not presented" );
		return false;
	}

	//���łɑ��݂���ID�̏ꍇ�̓G���[�ɂ���
	if( MyIdIndexMap.count( id ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id already exist : " + id );
		return false;
	}

	//���݂��Ȃ��̂ŁA�ǉ��������s��
	int index = -1;
	index = ::ImageList_AddMasked( MyHandle , img , mask );

	//�G���[�������H
	if( index < 0 )
	{
		//�G���[�Ȃ̂ŏI��
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_AddMasked failed" );
		return false;
	}

	//����I���������̂�ID���}�b�v�ɒǉ�
	MyIdIndexMap.insert( IdIndexMap::value_type( id , index ) );
	return true;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiImagelist::AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , COLORREF mask )
{
	//mGdiResource����w��̃C���[�W�𒊏o����
	const mGdiBitmap* bmp_img = res.GetItem< mGdiBitmap >( img );
	if( bmp_img == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 0 , L"Image id not found : " + img );
		return false;
	}

	//�ǉ������B
	return AddImage( id , *bmp_img , mask );
}


//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiImagelist::AddImage( const WString& id , const mGdiIcon& img )
{
	//ID���󗓂ł͂Ȃ����`�F�b�N
	if( id == L"" )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not presented" );
		return false;
	}

	//���łɑ��݂���ID�̏ꍇ�̓G���[�ɂ���
	if( MyIdIndexMap.count( id ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id already exist : " + id );
		return false;
	}

	//���݂��Ȃ��̂ŁA�ǉ��������s��
	int index = -1;
	index = ::ImageList_AddIcon( MyHandle , img );

	//�G���[�������H
	if( index < 0 )
	{
		//�G���[�Ȃ̂ŏI��
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_AddIcon failed" );
		return false;
	}

	//����I���������̂�ID���}�b�v�ɒǉ�
	MyIdIndexMap.insert( IdIndexMap::value_type( id , index ) );
	return true;
}

//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
bool mGdiImagelist::AddImageIcon( const mGdiResource& res , const WString& id , const WString& img )
{
	//mGdiResource����w��̃C���[�W�𒊏o����
	const mGdiIcon* icon_img = res.GetItem< mGdiIcon >( img );
	if( icon_img == nullptr )
	{
		//���o���s�B�C���[�W���擾�ł��Ȃ��ꍇ�̓G���[�ŏI��
		RaiseError( g_ErrorLogger , 0 , L"Icon id not found : " + img );
		return false;
	}

	//�ǉ������B
	return AddImage( id , *icon_img );
}

//�C���[�W���X�g����C���[�W���폜����
bool mGdiImagelist::RemoveImage( const WString& id )
{
	//�폜�Ώۂ�T��
	IdIndexMap::iterator itr = MyIdIndexMap.find( id );
	if( itr == MyIdIndexMap.end() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Id not found : " + id );
		return false;
	}

	//�폜����C���[�W���X�g�̃C���f�b�N�X
	INT index = itr->second;

	//�C���[�W���X�g����폜
	if( !::ImageList_Remove( MyHandle , index ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"ImageList_Remove failed" );
		return false;
	}

	//������ID--�C���f�b�N�X�̃}�b�v����Y���̂��폜
	MyIdIndexMap.erase( itr );

	//�폜����Ɩ����Ȃ������̃C���f�b�N�X��1�������̂ł��̕␳������
	//�C���[�W���X�g���ē����I�ɂ�vector�I�ȍ\���Ȃ�ł����ˁH
	for( itr = MyIdIndexMap.begin() ; itr != MyIdIndexMap.end() ; itr++ )
	{
		if( index < itr->second )
		{
			itr->second--;
		}
	}
	return true;
}

//ID����C���f�b�N�X���擾����
INT mGdiImagelist::GetIndex( const WString& id )const
{
	//ID���󕶎���̏ꍇ�́A�G���[�ɂ���
	if( id == L"" )
	{
		RaiseError( g_ErrorLogger , 0 , L"Id not specified" );
		return -1;
	}

	//�Ώۂ�T��
	IdIndexMap::const_iterator itr = MyIdIndexMap.find( id );
	if( itr == MyIdIndexMap.end() )
	{
		//�L��܂���ł���
		RaiseError( g_ErrorLogger , 0 , L"Id not found : " + id );
		return -1;
	}
	return itr->second;
}

//Option�Ɏw�肵�����e�Ɍ������I�u�W�F�N�g�𐶐����AMyHandle�ɓo�^����
//�R���X�g���N�^����Ăяo�����z��
bool mGdiImagelist::CreateMyHandle( const Option_UseOption& opt )
{
	UINT flag = 0;
	//�g�p����F���ɉ����āA�t���O��ݒ�
	switch( opt.color )
	{
	case Option::ColorDepth::COLOR4:
		flag |= ILC_COLOR4;
		break;
	case Option::ColorDepth::COLOR8:
		flag |= ILC_COLOR8;
		break;
	case Option::ColorDepth::COLOR16:
		flag |= ILC_COLOR16;
		break;
	case Option::ColorDepth::COLOR24:
		flag |= ILC_COLOR24;
		break;
	default:
		//����`�̏ꍇ�A�f�t�H���g�����߂�͍̂�����Ȃ̂ŃG���[�ɂ���
		RaiseAssert( g_ErrorLogger , 0 , L"Invalid color depth" );
		return false;
	}
	//�}�X�N���g�p���邽�߃t���O�𗧂Ă�
	flag |= ILC_MASK;

	//�n���h���̐���
	MyHandle = ::ImageList_Create( opt.width , opt.height , flag , opt.initial_size , opt.grow_size );

	return MyHandle != nullptr;
}

