//----------------------------------------------------------------------------
// �n�b�V�������N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MHASH_H_INCLUDED
#define MHASH_H_INCLUDED

#include "mStandard.h"
#include <wincrypt.h>
#include <General/mErrorLogger.h>
#include <General/mFileReadStream.h>
#include <memory>

/*
�g�p�@
(1)Init()�ŏ��������܂�
(2)Hash()�Ńn�b�V�����Z�o�������f�[�^��^���܂�
(3)GetResultLen()�Ńn�b�V���l�̃o�C�g���𒲂ׁA���ʊi�[��̃o�b�t�@���m�ۂ��܂�
(4)GetResult()�Ńn�b�V���l���擾���܂�
(5)���������n�b�V�������߂����ꍇ�́A(1)����J��Ԃ��Ă��������B
�@�I�u�W�F�N�g�͍ė��p�\�ł��B
*/

class mHash
{
public:

	mHash();
	virtual ~mHash();

	//�Ή�����n�b�V���A���S���Y��
	enum HashAlgorithm
	{
		MD5,		//MD5   128bit
		SHA1,		//SHA1   160bit
		SHA256,		//SHA2(SHA256)   256bit
		SHA512,		//SHA512   512bit
	};

	//������
	//algo : �g�p�������n�b�V���A���S���Y��
	//ret : �������^
	bool Init( HashAlgorithm algo );

	//�n�b�V�������s
	//�傫�ȃf�[�^�̏ꍇ�̓R�}�؂�ɂ��ČĂяo���Ă悢�B
	// len : �f�[�^�̃o�C�g��
	// data : �f�[�^�ւ̃|�C���^
	// ret : �������^
	bool Hash( const BYTE* data , DWORD len );

	//�n�b�V�������s
	// filename : �n�b�V�����擾����t�@�C����
	//�@�E�ǂݎ��\�ł���K�v������܂�.
	//�@�EEOF�ɂȂ�܂Ő��䂪�Ԃ��Ă��܂���B�p�C�v�̖��O�Ƃ���n���Ȃ��ł��������B
	// ret : �������^
	bool Hash( const WString& filename );

	//�n�b�V�������s
	// opt : �t�@�C���J���Ƃ��̃I�v�V����
	//�@�E�ǂݎ��\�ł���K�v������܂�.
	//�@�EEOF�ɂȂ�܂Ő��䂪�Ԃ��Ă��܂���B�p�C�v�̖��O�Ƃ���n���Ȃ��ł��������B
	// ret : �������^
	bool Hash( mFile::Option opt );

	//�n�b�V�������s
	// fp : �n�b�V�����擾����X�g���[���I�u�W�F�N�g
	//�@�E�n�����Ƃ��̓ǂݎ��ʒu����Ō�܂ł��n�b�V�����܂�
	//�@�EEOF�ɂȂ�܂Ő��䂪�Ԃ��Ă��܂���
	// ret : �������^
	bool Hash( mFileReadStream& fp );

	//�n�b�V�������s
	// fp : �n�b�V�����擾����X�g���[���I�u�W�F�N�g
	//�@�E�n�����Ƃ��̓ǂݎ��ʒu����len�Ŏw�肵�������A�܂��͍Ō�܂ł��n�b�V�����܂�
	//�@�EEOF�ɂȂ�܂Ő��䂪�Ԃ��Ă��܂���
	// len : �n�b�V�����钷��
	// ret : �������^
	bool Hash( mFileReadStream& fp , uint32_t len );

	//���ʂ̃T�C�Y�𓾂�
	//ret : ���ʂ̃o�C�g��(�G���[�̏ꍇ0)
	DWORD GetResultLen( void )const;

	//�n�b�V���֐��̃u���b�N�T�C�Y�𓾂�
	//ret : �u���b�N�T�C�Y(�G���[�̏ꍇ0)
	DWORD GetBlockSize( void )const;

	//�n�b�V���l�o�C�i���̊i�[��
	typedef std::unique_ptr<BYTE> HashData;

	//���ʂ𓾂�
	//���ʊi�[��̃o�b�t�@�͎����I�Ɋm�ۂ���A�X�}�[�g�|�C���^�ɐݒ肳��܂��B
	//retResult:���ʊi�[��
	//retLen:���ʂ̃o�C�g��
	//ret:�������^
	bool GetResult( HashData& retResult , DWORD& retLen )const;

	//���ʂ𓾂�
	//���ʊi�[��̃o�b�t�@�́A�Ăяo�������m�ۂ��A�܂��A�g�p��͉������K�v������܂��B
	//retResult:���ʊi�[��
	//len:���ʊi�[��̃o�C�g��
	//ret:�������^
	bool GetResult( BYTE* retResult , DWORD len )const;

	//���ʂ𓾂�
	//retResult : �n�b�V���l��16�i������̊i�[��
	//ret : �������^
	bool GetResult( AString& retResult )const;

	struct HashResult
	{
		HashData Data;
		DWORD Len;
	};

	//���ʂ𓾂�
	//retResult : �n�b�V���l�̃o�C�i����̊i�[��
	//ret : �������^
	bool GetResult( HashResult& retResult )const;

private:

	mHash( const mHash& src );
	const mHash& operator=( const mHash& src ) = delete;

	friend class mHmac;
	friend class mRSAVerifier;

protected:

	HashAlgorithm MyHashAlgorithm;
	HCRYPTPROV MyCryptProvider;
	HCRYPTHASH MyCryptHash;

	//�n�b�V���I�u�W�F�N�g��j������
	void ReleaseHashObject( void );

	//�n�b�V���A���S���Y����ID���擾����
	ALG_ID HashAlgorithm2AlgId( HashAlgorithm alg )const;

};


#endif 