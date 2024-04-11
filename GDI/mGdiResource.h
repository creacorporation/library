//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��iGDI���\�[�X�v�[���j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
GDI���\�[�X���X�g�b�N���܂�

HBRUSH brush;			//�u���V
HPEN pen;				//�y��
HFONT font;				//�t�H���g
HBITMAP bmp;			//�r�b�g�}�b�v
HIMAGELIST imglist;		//�C���[�W���X�g

*/

#ifndef MGDIRESOURCE_H_INCLUDED
#define MGDIRESOURCE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"
#include "GDI/mGdiHandle.h"
#include <list>
#include <unordered_map>
#include <typeinfo>

class mGdiResource
{
public:
	mGdiResource();
	virtual ~mGdiResource();

	//�t�@�N�g�����\�b�h��typedef�B
	typedef mGdiHandle* (*mGdiHandleFactory)( const void* opt );

	//�A�C�e����o�^����B����œo�^�����I�u�W�F�N�g�́AGetItem�Ŏ擾�ł��܂��B
	//�e���v���[�g�Ɏw�肵���N���X��Factory()���Ăяo����A�����Ő��������|�C���^��
	//id�Ɏw�肵�����O�œ����̃R���e�i�Ɋi�[���܂��BFactory()�ɂ�opt�Ŏw�肵���|�C���^���n����܂��B
	//id : �o�^����A�C�e���ɕt���閼�O
	//item : �o�^����A�C�e��
	//ret : ���������I�u�W�F�N�g(GetItem�Ŏ��镨�Ɠ���)
	template< class T > T* AddItem( const WString& id , const struct T::Option* opt )
	{
		T* obj = (T*)mGdiResource::AddItemInternal( T::Factory , id , opt );
		if( obj == nullptr )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"AddItem failed : " + id );
			return nullptr;
		}
		return obj;
	}

	//�A�C�e�����擾����
	//AddItem�œo�^�����I�u�W�F�N�g��Ԃ��܂��B
	//id�Ŏw�肵���������݂��Ȃ��ꍇ�́Asubid�Ŏw�肵������T���܂��B
	//������Ȃ��ꍇ�́Anullptr��Ԃ��܂��B
	//id : �擾������ID(AddItem�Ŏw�肵������)
	//subid : id���Ȃ������ꍇ�Ɏ擾������ID(�s�v�ȏꍇ�͋󕶎����OK)
	//ret : �擾�����I�u�W�F�N�g�B�Y�������̏ꍇnullptr�B
	template< class T > T* GetItem( const WString& id , const WString& subid = L"" )const
	{
		//�A�C�e���̌���
		IdMap::const_iterator itr = MyIdMap.find( id );
		if( itr == MyIdMap.end() )
		{
			//�Y��ID�̓o�^���Ȃ��ꍇ�́Asubid�Ō���
			itr = MyIdMap.find( subid );
			if( itr == MyIdMap.end() )
			{
				//���̓o�^���Ȃ��ꍇ�͊Y���Ȃ��ɂ��G���[
				RaiseAssert( g_ErrorLogger , 0 , L"Id not found : " + id );
				return nullptr;
			}
		}
		if( dynamic_cast< T* >( itr->second ) == nullptr )
		{
			//�e���v���[�g�Ǝ擾�����I�u�W�F�N�g�̌^���Ⴄ
			RaiseAssert( g_ErrorLogger , 0 , L"Typeinfo mismatch : " + id );
			return nullptr;
		}
		return (T*)itr->second;
	}

	//�A�C�e���̃n���h�����擾����
	//AddItem�œo�^�����I�u�W�F�N�g���������A���̃I�u�W�F�N�g�̃n���h����Ԃ��܂��B
	//�n���h�������݂��Ȃ������ꍇ��nullptr���Ԃ�܂��B
	//id : �擾������ID(AddItem�Ŏw�肵������)
	//subid : id���Ȃ������ꍇ�Ɏ擾������ID(�s�v�ȏꍇ�͋󕶎����OK)
	//ret : �擾�����I�u�W�F�N�g�̃n���h���B�Y�������̏ꍇnullptr�B
	HGDIOBJ GetItem( const WString& id , const WString& subid = L"" )const;

	//�A�C�e�����폜����
	//id : �폜������ID
	//ret�F ������true
	bool RemoveItem( const WString& id );

	//�w�肵��ID�̃I�u�W�F�N�g�����݂��邩��Ԃ��܂�
	//id : ���ׂ���ID
	//ret : ���݂����true�B���݂��Ȃ����false�B
	bool IsExist( const WString& id )const;

private:
	mGdiResource( const mGdiResource& src ) = delete;
	mGdiResource& operator=( const mGdiResource& src ) = delete;

	//�R���g���[�����R���N�V�����ɒǉ�����BAddControl�̓��������B
	//factory : �t�@�N�g�����\�b�h
	//id : �o�^����ID
	//opt : �t�@�N�g�����\�b�h�Ő��������I�u�W�F�N�g�ɓn���I�v�V����
	//ret : ���������I�u�W�F�N�g�̃|�C���^
	mGdiHandle* AddItemInternal( mGdiHandleFactory factory , const WString& id , const void* opt );


protected:

	//�I�u�W�F�N�g�̃}�b�s���O
	//���F�I�u�W�F�N�g��ID
	//�E�F�I�u�W�F�N�g�ւ̃|�C���^
	typedef std::unordered_map<WString,mGdiHandle*> IdMap;
	IdMap MyIdMap;
	
};

#endif	//MGDIRESOURCE_H_INCLUDED

