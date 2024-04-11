//----------------------------------------------------------------------------
// RSA�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mRSA.h"
#include "../General/mErrorLogger.h"

mRSA::mRSA()
{
	MyCryptProv = 0;
	MyCryptKey = 0;
	MyCryptKeyPub = 0;
}

mRSA::~mRSA()
{
	Clear();
	if( MyCryptProv )
	{
		CryptReleaseContext( MyCryptProv , 0 );
		MyCryptProv = 0;
	}
}

bool mRSA::Init( void )
{
	//RSA���g�p���邪�ACRYPT_VERIFYCONTEXT���g�p���č����x���Ȃ��B
	//�N���C�A���gOS�ɂ����ẮACRYPT_VERIFYCONTEXT���g�p�����ꍇ�͈ꎞ�������Ɍ����ۊǂ����B
	//�����ăn���h���̉���Ɠ����ɍ폜�����B

	//��MSDN��CRYPT_VERIFYCONTEXT�̐�����蔲����
	//The application has no access to the persisted private keys of public/private key pairs. 
	//When this flag is set, temporary public/private key pairs can be created, but they are not persisted.
	//��F�A�v���P�[�V�����́A���y�A�̔閧���ɃA�N�Z�X���邱�Ƃ͂���܂���B
	//    ���̃t���O���Z�b�g���ꂽ�ꍇ�́A�e���|�����Ȍ��y�A���쐬����܂����A�ۑ�����܂���B
	//http://msdn.microsoft.com/en-us/library/aa379886(v=vs.85).aspx ���B

	if( MyCryptProv )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_�͂��łɏ���������Ă��܂�" );
		return false;
	}

	if( !CryptAcquireContext( &MyCryptProv , nullptr , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , CRYPT_VERIFYCONTEXT ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_�̏������Ɏ��s���܂���" );
		return false;
	}
	return true;
}

bool mRSA::Init( const WString& container_name , bool machine_keyset )
{
	if( MyCryptProv )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_�͂��łɏ���������Ă��܂�" );
		return false;
	}

	DWORD flag = ( machine_keyset ) ? ( CRYPT_MACHINE_KEYSET ) : ( 0 );
	if( !CryptAcquireContext( &MyCryptProv , container_name.c_str() , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , flag ) )
	{
		flag |= CRYPT_NEWKEYSET;
		if( !CryptAcquireContext( &MyCryptProv , container_name.c_str() , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , flag ) )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_���������ł��܂���ł���" );
			return false;
		}
		else
		{
			CreateLogEntry( g_ErrorLogger , 0 , L"�V�����L�[�R���e�i���쐬���܂���" , container_name );
		}
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�L�[�R���e�i�����[�h���܂���" , container_name );
	}
	return true;
}

bool mRSA::DestroyKeyContainer( const WString& container_name , bool machine_keyset )
{
	HCRYPTPROV dummy_prov = 0;
	DWORD flag = CRYPT_DELETEKEYSET;
	flag |= ( machine_keyset ) ? ( CRYPT_MACHINE_KEYSET ) : ( 0 );

	if( !CryptAcquireContext( &dummy_prov , container_name.c_str() , MS_ENH_RSA_AES_PROV , PROV_RSA_AES , flag ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�L�[�R���e�i�̍폜���ł��܂���ł���" , container_name );
		return false;
	}
	else
	{
		CreateLogEntry( g_ErrorLogger , 0 , L"�L�[�R���e�i���폜���܂���" , container_name );
		return true;
	}
}

bool mRSA::Clear( void )
{
	if( MyCryptKeyPub )
	{
		CryptDestroyKey( MyCryptKeyPub );
		MyCryptKeyPub = 0;
	}
	if( MyCryptKey )
	{
		CryptDestroyKey( MyCryptKey );
		MyCryptKey = 0;
	}
	return true;
}

bool mRSA::GenerateNewKey( KEYLENGTH len )
{
	USHORT bit;
	switch( len )
	{
	case KEYLENGTH::KEYLEN_2048BIT:
		bit = 2048;
		break;
	case KEYLENGTH::KEYLEN_4096BIT:
		bit = 4096;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"RSA�����̎w�肪�s���ł�" );
		return false;
	}

	if( MyCryptProv == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_������������Ă��܂���" );
		return false;
	}
	if( !Clear() )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����̃n���h������Ɏ��s���܂���" );
		return false;
	}

	if( !CryptGenKey( MyCryptProv , AT_KEYEXCHANGE , ( bit << 16 ) | CRYPT_EXPORTABLE , &MyCryptKey ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í������̐��������s���܂���" );
		return false;
	}
	return true;
}

DWORD mRSA::GetKeySize( mRSA::KEYTYPE type )
{
	DWORD size = 0;
	if( !GetKey( type , nullptr , size ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í��������ݒ肳��Ă��܂���" , type );
		return 0;
	}
	return size;
}

//���o�C�i���𓾂�
bool mRSA::ExportKey( mRSA::KEYTYPE type , KeyBinary& retKey , DWORD& retWritten )
{
	retWritten = GetKeySize( type );
	retKey.reset( mNew BYTE[ retWritten ] );

	return GetKey( type , retKey.get() , retWritten );
}

bool mRSA::ImportKey( const BYTE* data , DWORD datalen )
{
	//���̃t�H�[�}�b�g�́A�擪��PUBLICKEYSTRUC�Ȃ̂ŁA
	//��������t�H�[�}�b�g�𔻕ʂ���B

	//http://msdn.microsoft.com/en-us/library/aa375601(v=vs.85).aspx
	//http://msdn.microsoft.com/en-us/library/aa387453(v=vs.85).aspx

	if( data == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�����w��ł�" );
		return false;
	}

	const PUBLICKEYSTRUC* header = (const PUBLICKEYSTRUC*)data;
	if( header->bType == PRIVATEKEYBLOB )
	{
		Clear();
		return ImportKeyInternal( &MyCryptKey , datalen , data );
	}
	else if( ( header->bType == PUBLICKEYBLOB ) || ( header->bType == PUBLICKEYBLOBEX ) )
	{
		Clear();
		return ImportKeyInternal( &MyCryptKeyPub , datalen , data );
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�f�[�^�Ɍ����܂܂�Ă��܂���" );
		return false;
	}
}

bool mRSA::GetKey( mRSA::KEYTYPE type , BYTE* buffer , DWORD& size )
{
	if( MyCryptProv == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Í����v���o�C�_������������Ă��܂���" );
		return false;
	}

	//�l�ϊ� (KEYTYPE �� CryptExportKey��blobtype)
	DWORD blobtype;
	switch( type )
	{
	case KEYTYPE::KEY_PRIVATE:
		blobtype = PRIVATEKEYBLOB;
		break;
	case KEYTYPE::KEY_PUBLIC:
		blobtype = PUBLICKEYBLOB;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�s���Ȍ��̎�ނł�" , type );
		return false;
	}

	//�l�擾
	if( CryptExportKey( MyCryptKey , 0 , blobtype , 0 , buffer , &size ) )
	{
		return true;
	}
	if( CryptExportKey( MyCryptKeyPub , 0 , blobtype , 0 , buffer , &size ) )
	{
		return true;
	}
	RaiseAssert( g_ErrorLogger , 0 , L"���̃G�N�X�|�[�g�Ɏ��s���܂���" );
	return false;

}

bool mRSA::ExtractPublicKey( void )
{
	BOOL result = true;
	DWORD keylen = 0;
	KeyBinary binkey;

	if( MyCryptKeyPub != 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"���J�������łɐݒ肳��Ă��܂�" );
		goto end;
	}

	//���̒������擾���āA���̃T�C�Y�̃o�b�t�@�����
	keylen = GetKeySize( KEYTYPE::KEY_PUBLIC );
	if( keylen == 0 )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���J���̒������擾�ł��܂���ł���" );
		result = false;
		goto end;
	}
	binkey.reset( mNew BYTE[ keylen ] );

	//������o�b�t�@�Ɍ����擾
	if( !ExportKey( KEYTYPE::KEY_PUBLIC , binkey , keylen ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���J���̕ۑ��Ɏ��s���܂���" );
		result = false;
		goto end;
	}

	//�擾�������J�������̂܂܃C���|�[�g���Č��J�������̃n���h�������
	if( !ImportKeyInternal( &MyCryptKeyPub , keylen , binkey.get() ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���J���̃C���|�[�g�Ɏ��s���܂���" );
		result = false;
		goto end;
	}
end:
	if( keylen )
	{
		SecureZeroMemory( binkey.get() , keylen );
	}
	return result;

}

bool mRSA::ImportKeyInternal( HCRYPTKEY* key , DWORD len , const BYTE* buffer )
{
	if( !CryptImportKey( MyCryptProv , buffer , len , 0 , 0 , key ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"���̃C���|�[�g�Ɏ��s���܂���" );
		return false;
	}
	return true;
}


