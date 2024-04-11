//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�C���[�W���X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�C���[�W���X�g�̊Ǘ������܂��B
�A�C�R�����X���܂Ƃ߂ĊǗ����A�c�[���o�[�ɂԂ����񂾂�ł��܂��B

mGdiHandle���p�����Ă͂��܂����A�����mGdiResource�ł܂Ƃ߂ĊǗ��ł���悤�ɂ���̂��ړI�ŁA
�C���[�W���X�g�́A�u���V��y���A�t�H���g�Ɠ����悤�Ȏg�������͂ł��܂���B
(���Ƃ��΁AmGdiDC::Select()�Ńf�o�C�X�R���e�L�X�g�Ɋ֘A�Â��ł��܂���B)

�C���[�W���X�g�ɓo�^�����r�b�g�}�b�v��A�C�R���𒼐ډ����ɕ`�悷����@�͒񋟂��Ă��܂���B
(mGdiDC�ɗ�O�I�ȏ����������A��₱�����Ȃ��Ă��܂����߂ł��B)
�����ς�A�c�[���o�[�̃A�C�R����o�^���邽�߂̃N���X�Ƃ��Ďg���ĉ������B
���ڕ`�悵�����Ȃ�AmGdiBitmap���g���ĉ������B
*/

#ifndef MGDIIMAGELIST_H_INCLUDED
#define MGDIIMAGELIST_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "mGdiBitmap.h"
#include "mGdiIcon.h"
#include "mGdiResource.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"
#include <CommCtrl.h>
#include <unordered_map>

class mGdiImagelist : public mGdiHandle
{
public:

	//�I�v�V�����\����
	//���ۂɃC���[�W���X�g���쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA��肽�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		//�C���[�W���X�g�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		//���F�g���邩�H
		enum ColorDepth
		{
			COLOR4,		//4�r�b�g�J���[		16�F
			COLOR8,		//8�r�b�g�J���[		256�F
			COLOR16,	//16�r�b�g�J���[	65536�F
			COLOR24,	//24�r�b�g�J���[	1677���F
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�I�v�V�����\����
	struct Option_UseOption : public Option
	{
		INT width;			//�C���[�W�̕�
		INT height;			//�C���[�W�̍���
		INT initial_size;	//�ŏ��ɃI�u�W�F�N�g��������Ƃ��̃L���p�V�e�B(�����C���[�W���i�[�ł��邩)
		INT grow_size;		//�L���p�V�e�B������Ȃ��Ȃ����Ƃ��ɉ��������L���p���g������邩
		ColorDepth color;	//�C���[�W�̃r�b�g��
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			width = 32;
			height = 32;
			initial_size = 32;
			grow_size = 16;
			color = ColorDepth::COLOR16;
		}
	};

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//id : �ǉ�����C���[�W�ɕt�^����ID(����̃C���[�W���X�g���ŏd���s��)
	//img : �ǉ�����C���[�W
	//mask : �C���[�W�̓��߃}�X�N(null�̏ꍇ�̓}�X�N�Ȃ�)
	//ret : �������^
	bool AddImage( const WString& id , const mGdiBitmap& img , const mGdiBitmap* mask = nullptr );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//mGdiResource����mGdiBitmap�𒊏o���Ēǉ�����
	//res : �C���[�W���o��
	//id : �ǉ�����C���[�W�ɕt�^����ID
	//img : �ǉ�����C���[�W(mGdiResource�ɓo�^����Ă���ID)
	//mask : �C���[�W�̓��߃}�X�N(mGdiResource�ɓo�^����Ă���ID)�󕶎���̂Ƃ��̓}�X�N�Ȃ��B
	//ret : �������^
	bool AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , const WString& mask = L"" );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//id : �ǉ�����C���[�W�ɕt�^����ID(����̃C���[�W���X�g���ŏd���s��)
	//img : �ǉ�����C���[�W
	//mask : �C���[�W�̓��߃}�X�N(�F���w��)
	//ret : �������^
	bool AddImage( const WString& id , const mGdiBitmap& img , COLORREF mask );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//mGdiResource����mGdiBitmap�𒊏o���Ēǉ�����
	//res : �C���[�W���o��
	//id : �ǉ�����C���[�W�ɕt�^����ID
	//img : �ǉ�����C���[�W(mGdiResource�ɓo�^����Ă���ID)
	//mask : �C���[�W�̓��߃}�X�N(�F���w��)
	//ret : �������^
	bool AddImageBitmap( const mGdiResource& res , const WString& id , const WString& img , COLORREF mask );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//id : �ǉ�����C���[�W�ɕt�^����ID(����̃C���[�W���X�g���ŏd���s��)
	//img : �ǉ�����C���[�W
	//ret : �������^
	bool AddImage( const WString& id , const mGdiIcon& img );

	//�C���[�W���X�g�ɃC���[�W�̒ǉ�������
	//mGdiResource����mGdiIcon�𒊏o���Ēǉ�����
	//res : �C���[�W���o��
	//id : �ǉ�����C���[�W�ɕt�^����ID
	//img : �ǉ�����C���[�W(mGdiResource�ɓo�^����Ă���ID)
	//ret : �������^
	bool AddImageIcon( const mGdiResource& res , const WString& id , const WString& img );

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
		mGdiImagelist* result;
		try
		{
			result = mNew mGdiImagelist( (const Option*)opt );
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
	mGdiImagelist( const Option* option )throw( mException );

	//�f�X�g���N�^
	virtual ~mGdiImagelist();
	
	//�n���h���̒l���擾����(�L���X�g���Z�q�o�[�W����)
	operator HIMAGELIST()const;

	//�n���h���̒l���擾����(���ʂ̊֐��o�[�W����)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//�ȉ��A�f�t�H���g�n�͎̂g�p�s�Ƃ���
	mGdiImagelist() = delete;
	mGdiImagelist( const mGdiImagelist& src ) = delete;
	mGdiImagelist& operator=( const mGdiImagelist& src ) = delete;

	//Option�Ɏw�肵�����e�Ɍ������I�u�W�F�N�g�𐶐����AMyHandle�ɓo�^����
	//�R���X�g���N�^����Ăяo�����z��
	bool CreateMyHandle( const Option_UseOption& opt );

protected:

	//�n���h���̎���
	HIMAGELIST MyHandle;

	//������ID��ImageList�̃C���f�b�N�X�̊֘A�Â�
	typedef std::unordered_map<WString,INT> IdIndexMap;
	IdIndexMap MyIdIndexMap;

};

#endif	//MGDIIMAGELIST_H_INCLUDED

