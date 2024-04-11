//----------------------------------------------------------------------------
// AES�Í����N���X
// Copyright (C) 2013-2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 2013/10/09�`
//----------------------------------------------------------------------------

#include "mAESDecrypter.h"
#include "../General/mErrorLogger.h"

mAESDecrypter::mAESDecrypter()
{

}

mAESDecrypter::~mAESDecrypter()
{

}

bool mAESDecrypter::Decrypt( bool isfinal , const BYTE* data , DWORD datalen , EncryptData& retData , DWORD& retWritten )const
{
	//���̓`�F�b�N
	if( !data )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�o�b�t�@�����w��ł�" );
		return false;
	}
	if( !MyCryptKey )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�����ݒ肳��Ă��܂���" );
		return false;
	}

	//�o�b�t�@�̊m��
	retData.reset( mNew BYTE[ datalen ] );
	MoveMemory( retData.get() , data , datalen );

	//�Í�������
	retWritten = datalen;
	if( !CryptDecrypt( MyCryptKey , 0 , isfinal , 0 , retData.get() , &retWritten ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"��ǂɎ��s���܂���" );
		return false;
	}
	return true;
}

