//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�Q�C���[�W�̃C���[�W���X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�C���[�W���X�g���Q�Ǘ����܂��B
*/

#ifndef MGDIDUALIMAGELIST_H_INCLUDED
#define MGDIDUALIMAGELIST_H_INCLUDED

#include "mGdiImagelist.h"
#include "mGdiResource.h"
#include "General/mException.h"

class mGdiDualImagelist : public mGdiHandle
{
public:

	//�I�v�V�����\����
	typedef mGdiImagelist::Option Option;

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//id : �ǉ�����C���[�W�ɕt�^����ID(����̃C���[�W���X�g���ŏd���s��)
	//img1,img2 : �ǉ�����C���[�W
	//mask1,mask2 : �C���[�W�̓��߃}�X�N(nullptr�܂��͏ȗ��̏ꍇ�̓}�X�N�Ȃ�)
	//ret : �������^
	bool AddImage( const WString& id ,
		const mGdiBitmap& img1 ,
		const mGdiBitmap& img2 , 
		const mGdiBitmap* mask1 = nullptr ,
		const mGdiBitmap* mask2 = nullptr );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������(mGdiResource����C���[�W�𒊏o����Ver)
	//res : �C���[�W���o��
	//id : �ǉ�����C���[�W�ɕt�^����ID(�C���[�W���X�g�ɓo�^����Ƃ���ID)
	//img1,img2 : �ǉ�����C���[�W��ID(mGdiResource���璊�o����Ƃ���ID)
	//mask1,mask2 : �C���[�W�̓��߃}�X�N��ID(mGdiResource���璊�o����Ƃ���ID�B�󕶎���̏ꍇ�̓}�X�N�Ȃ�)
	//ret : �������^
	bool AddImageBitmap( const mGdiResource& res , 
		const WString& id ,
		const WString& img1 ,
		const WString& img2 , 
		const WString& mask1 = L"" ,
		const WString& mask2 = L"" );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//id : �ǉ�����C���[�W�ɕt�^����ID(����̃C���[�W���X�g���ŏd���s��)
	//img1,img2 : �ǉ�����C���[�W
	//mask1,img2 : �C���[�W�̓��߃}�X�N(�F���w��) ���ȗ��ł��܂���
	//ret : �������^
	bool AddImage( const WString& id ,
		const mGdiBitmap& img1 ,
		const mGdiBitmap& img2 ,
		COLORREF mask1 ,
		COLORREF mask2 );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������(mGdiResource����C���[�W�𒊏o����Ver)
	//res : �C���[�W���o��
	//id : �ǉ�����C���[�W�ɕt�^����ID(�C���[�W���X�g�ɓo�^����Ƃ���ID)
	//img1,img2 : �ǉ�����C���[�W��ID(mGdiResource���璊�o����Ƃ���ID)
	//mask1,img2 : �C���[�W�̓��߃}�X�N(�F���w��) ���ȗ��ł��܂���
	//ret : �������^
	bool AddImageBitmap( const mGdiResource& res , 
		const WString& id ,
		const WString& img1 ,
		const WString& img2 ,
		COLORREF mask1 ,
		COLORREF mask2 );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//id : �ǉ�����C���[�W�ɕt�^����ID(����̃C���[�W���X�g���ŏd���s��)
	//img1,img2 : �ǉ�����C���[�W
	//ret : �������^
	bool AddImage( const WString& id , 
		const mGdiIcon& img1 ,
		const mGdiIcon& img2 );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������(mGdiResource����C���[�W�𒊏o����Ver)
	//res : �C���[�W���o��
	//id : �ǉ�����C���[�W�ɕt�^����ID(�C���[�W���X�g�ɓo�^����Ƃ���ID)
	//img1,img2 : �ǉ�����C���[�W��ID(mGdiResource���璊�o����Ƃ���ID)
	//ret : �������^
	bool AddImageIcon( const mGdiResource& res , 
		const WString& id , 
		const WString& img1 ,
		const WString& img2 );

	//�C���[�W���X�g����C���[�W���폜����
	//id : �폜����C���[�W��ID
	//ret : �������^
	bool RemoveImage( const WString& id );

	//ID����C���f�b�N�X���擾����
	//id : �擾������ID
	//ret : �C���f�b�N�X�B�G���[�̏ꍇ���̐�
	INT GetIndex( const WString& id )const;

public:

	//�t�@�N�g�����\�b�h
	//opt�͕K���w�肵�Ă��������B�G���[�ɂȂ�nullptr��Ԃ��܂��B
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiDualImagelist* result;
		try
		{
			result = mNew mGdiDualImagelist( (const Option*)opt );
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
	mGdiDualImagelist( const Option* opt )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiDualImagelist();
	
	//�n���h���̒l���擾����i�P�߂̃C���[�W���X�g�̃n���h�����擾����j
	virtual HGDIOBJ GetHandle( void )const override;

	//�n���h���̒l���擾����i�Q�߂̃C���[�W���X�g�̃n���h�����擾����j
	HGDIOBJ GetHandle2( void )const;

private:

	//�ȉ��A�f�t�H���g�n�͎̂g�p�s�Ƃ���
	mGdiDualImagelist() = delete;
	mGdiDualImagelist( const mGdiDualImagelist& src ) = delete;
	mGdiDualImagelist& operator=( const mGdiDualImagelist& src ) = delete;

protected:

	//�n���h���̎��́i�m�[�}���j
	mGdiImagelist* MyHandle;

	//�n���h���̎��́i�z�b�g�j
	HIMAGELIST MyHandle2;

};

#endif	//MGDIDUALIMAGELIST_H_INCLUDED

