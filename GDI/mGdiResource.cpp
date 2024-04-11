//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI���\�[�X�v�[���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIRESOURCE_CPP_COMPILING
#include "mGdiResource.h"
#include "General/mErrorLogger.h"

mGdiResource::mGdiResource()
{
}

mGdiResource::~mGdiResource()
{
	for( IdMap::iterator itr = MyIdMap.begin() ; itr != MyIdMap.end() ; itr++ )
	{
		mDelete itr->second;
	}
}

//�R���g���[�����R���N�V�����ɒǉ�����
mGdiHandle* mGdiResource::AddItemInternal( mGdiHandleFactory factory , const WString& id , const void* opt )
{
	//�g�p�s�\��ID�łȂ������m�F
	//���łɑ��݂���ID�ł͂Ȃ����AID���󕶎���ł͂Ȃ������m�F����
	if( id == L"" || MyIdMap.count( id ) )
	{
		//�g�p�s�\��ID������
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"Bad id : " + id );
		return nullptr;
	}

	//�I�u�W�F�N�g�̃C���X�^���X���쐬����
	mGdiHandle* obj = factory( opt );
	if( obj == nullptr )
	{
		//�t�@�N�g�����\�b�h�����s����
		RaiseAssert( g_ErrorLogger , (ULONG_PTR)factory , L"Factory method returned nullptr : " + id );
		return nullptr;
	}

	//���������I�u�W�F�N�g��o�^����
	MyIdMap.insert( IdMap::value_type( id , obj ) );

	return obj;
}

HGDIOBJ mGdiResource::GetItem( const WString& id , const WString& subid )const
{
	IdMap::const_iterator itr;
	
	//�܂��͍ŏ���id�Ō���
	itr = MyIdMap.find( id );
	if( itr != MyIdMap.end() )
	{
		return itr->second->GetHandle();
	}
	//�Y��ID�̓o�^���Ȃ��ꍇ�́Asubid�Ō���
	itr = MyIdMap.find( subid ); 
	if( itr != MyIdMap.end() )
	{
		return itr->second->GetHandle();
	}
	//���̓o�^���Ȃ��ꍇ�͊Y�������G���[
	RaiseAssert( g_ErrorLogger , 0 , L"Id not found : " + id );
	return nullptr;
}

bool mGdiResource::RemoveItem( const WString& id )
{
	//�A�C�e�����폜����
	IdMap::iterator itr = MyIdMap.find( id );
	if( itr == MyIdMap.end() )
	{
		return false;
	}
	mDelete itr->second;
	MyIdMap.erase( itr );
	return true;
}

bool mGdiResource::IsExist( const WString& id )const
{
	return MyIdMap.count( id ) != 0;
}
