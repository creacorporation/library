//----------------------------------------------------------------------------
// �n�b�V�������N���X
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


#ifndef MHMAC_H_INCLUDED
#define MHMAC_H_INCLUDED

#include "mStandard.h"
#include "mHash.h"
#include "../General/mErrorLogger.h"

/*
<�g����>

int main( int argc , const char** argv )
{
	//key =           "Jefe"
	//key_len =       4
	//data =          "what do ya want for nothing?"
	//data_len =      28
	//digest =        0xeffcdf6ae5eb2fa2d27416d5f184df9c259a7c79

	mHmac hmac;
	BYTE Data1[] = { 'J' , 'e' , 'f' , 'e' };
	BYTE Data2[] = {
		'w' , 'h' , 'a' , 't' , ' ' , 'd' , 'o' , ' ' , 'y' , 'a' ,
		' ' , 'w' , 'a' , 'n' , 't' , ' ' , 'f' , 'o' , 'r' , ' ' ,
		'n' , 'o' , 't' , 'h' , 'i' , 'n' , 'g' , '?'
	};

	hmac.Init( mHash::HashAlgorithm::SHA1 , Data1 , sizeof( Data1 ) );
	hmac.Hash( Data2 , sizeof( Data2 ) );

	AString digest1;
	hmac.GetResult( digest1 );
	return 0;
}
*/

class mHmac
{
public:

	mHmac();
	virtual ~mHmac();

	//������
	//algo : �g�p�������n�b�V���A���S���Y��
	//key : �Í����L�[
	//keylen : key�̃o�C�g��
	//openssl : �^�FOpenSSL�Ɠ���HMAC�𐶐����܂��B
	//			�U�F�L�[����URC4�ŃX�N�����u�����Ă���HMAC�𐶐����܂�(WindowsAPI�̃T���v���Ɠ���)�B
	//ret : �������^
	bool Init( mHash::HashAlgorithm algo , const BYTE* key , DWORD keylen , bool openssl = true );

	//���Z�b�g
	//�I�u�W�F�N�g�����Z�b�g����B�Í����L�[�̓��Z�b�g����Ȃ��B
	//Init�ŏ�������HMAC�v�Z��Reset������HMAC�v�Z�c�̂悤�ɃI�u�W�F�N�g���ė��p�ł���
	bool Reset( void );

	//�n�b�V�������s
	//�傫�ȃf�[�^�̏ꍇ�̓R�}�؂�ɂ��ČĂяo���Ă悢�B
	// len : �f�[�^�̃o�C�g��
	// data : �f�[�^�ւ̃|�C���^
	// ret : �������^
	bool Hash( const BYTE* data , DWORD len );

	//���ʂ̃T�C�Y�𓾂�
	//ret : ���ʂ̃o�C�g��(�G���[�̏ꍇ0)
	DWORD GetResultLen( void )const;

	//�n�b�V���l�o�C�i���̊i�[��
	typedef std::unique_ptr<BYTE> HmacData;

	//���ʂ𓾂�
	//���ʊi�[��̃o�b�t�@�͎����I�Ɋm�ۂ���A�X�}�[�g�|�C���^�ɐݒ肳��܂��B
	//retResult : ���ʂ̊i�[��
	//retLen : ���ʂ̃o�C�g��
	//ret:�������^
	bool GetResult( HmacData& retResult , DWORD& retLen )const;

	//���ʂ𓾂�
	//���ʊi�[��̃o�b�t�@�́A�Ăяo�������m�ۂ��A�܂��A�g�p��͉������K�v������܂��B
	//len:���ʊi�[��̃o�C�g��
	//retResult:���ʊi�[��
	//ret:�������^
	bool GetResult( BYTE* retResult , DWORD len )const;

	//���ʂ𓾂�
	//retResult : �n�b�V���l��16�i������̊i�[��
	//ret : �������^
	bool GetResult( AString& retResult )const;

private:

	mHmac( const mHmac& src );
	const mHmac& operator=( const mHmac& src ) = delete;

protected:

	mHash MyHashObject;

	HCRYPTKEY MyCryptKey;	//HMAC�����p�̃L�[
	HMAC_INFO MyHmacInfo;	//HMAC�����p�̏��i�p�f�B���O���j

	void ReleaseHashObject( void );
};


#endif 