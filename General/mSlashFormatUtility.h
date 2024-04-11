//----------------------------------------------------------------------------
// �X���b�V����؂�f�[�^�t�H�[�}�b�g
// Copyright (C) 2020- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef SLASHFORMATUTILITY_H_INCLUDED
#define SLASHFORMATUTILITY_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <map>
#include "mSlashFormat.h"

/*

�ȉ��̌`���̃e�L�X�g�t�@�C��
<command>/<key1>=<val1>/<key2>=<val2>/..../<keyn>=<valn>

command = �R�}���h�B::�ŋ�؂��ĊK�w�\���ɂ��邱�Ƃ��ł���B
key = �R�}���h�ɕt������ݒ�l�̃L�[�B�ŏ��ɔ�������=�̈ʒu�܂ł�key�ɂȂ�B
val = �R�}���h�ɕt������ݒ�l�̒l�B�ŏ��ɔ�������=�̈ʒu�ȍ~��val�ɂȂ�B

�E������ɂ�\r,\n,\0���܂߂邱�Ƃ͏o���Ȃ�
�Ekey��=���܂ނ��Ƃ͏o���Ȃ��B
�Ecommand,key,val�ɃX���b�V�����܂߂����ꍇ�́A//�ɂ���B

��1�F
Info::Price::Current/Id=12345/Name=�R�c���Y/Price=200
��2�F
Set::Date/Date=2020//03//26
	�����t�̃X���b�V����//�ɂȂ��Ă��邱�Ƃɒ���

*/

namespace mSlashFormatUtility
{

	//�������SlashFormat�`���Ƀp�[�X����
	// src : �p�[�X���镶����
	// retData : �p�[�X��������
	void Parse( const AString& src , mSlashFormatA& retData );

	//�������SlashFormat�`���Ƀp�[�X����
	// src : �p�[�X���镶����
	// retData : �p�[�X��������
	void Parse( const WString& src , mSlashFormatW& retData );

	//������̃X���b�V����//�ɒu������
	WString EscapeSlash( const WString& src );

	//������̃X���b�V����//�ɒu������
	AString EscapeSlash( const AString& src );

	//SlashFormat�𕶎���ɃG���R�[�h����
	// dt : �G���R�[�h�Ώۂ̃f�[�^
	// retStr : ���ʊi�[��
	// ret : �������^(�U�̏ꍇ�͎g���Ȃ��������܂�ł���\���������j
	bool Encode( const mSlashFormatA& dt , AString& retStr );

	//SlashFormat�𕶎���ɃG���R�[�h����
	// dt : �G���R�[�h�Ώۂ̃f�[�^
	// retStr : ���ʊi�[��
	// ret : �������^(�U�̏ꍇ�͎g���Ȃ��������܂�ł���\���������j
	bool Encode( const mSlashFormatW& dt , WString& retStr );

};

#endif


