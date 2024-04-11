//----------------------------------------------------------------------------
// �X���b�V����؂�f�[�^�t�H�[�}�b�g
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------


#include "mSlashFormatUtility.h"
#include "mTCHAR.h"

void mSlashFormatUtility::Parse( const AString& src , mSlashFormatA& retData )
{
	retData.Parse( src );
	return;
}

void mSlashFormatUtility::Parse( const WString& src , mSlashFormatW& retData )
{
	retData.Parse( src );
	return;
}

//������̃X���b�V����//�ɒu������
WString mSlashFormatUtility::EscapeSlash( const WString& src )
{
	return ReplaceString( src , L"/" , L"//" , nullptr );
}

//������̃X���b�V����//�ɒu������
AString mSlashFormatUtility::EscapeSlash( const AString& src )
{
	return ReplaceString( src , "/" , "//" , nullptr );
}
