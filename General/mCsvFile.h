//----------------------------------------------------------------------------
// CSV�t�@�C���n���h��
// Copyright (C) 2020-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MCSVFILE_H_INCLUDED
#define MCSVFILE_H_INCLUDED

#include "mStandard.h"
#include "General/mErrorLogger.h"
#include "General/mTCHAR.h"
#include "General/mFileReadStreamBase.h"
#include <deque>

namespace mCsvFile
{
	using WRow = std::deque< WString >; 
	using WTable = std::deque< WRow >;

	using ARow = std::deque< AString >; 
	using ATable = std::deque< ARow >;

	//CSV�t�@�C����ǂݎ���Ĕz��Ɋi�[����
	//�E�ǂݎ��Ƃ��̕����R�[�h�́AretTable�Ɏw�肵���^�ɍ��킹��
	//filename : �ǂݎ��t�@�C����
	//retTable : �i�[��
	//empty_str : �ǂݎ�茋�ʂ������`�ɂȂ��Ă��Ȃ��Ƃ��i�s�^�񐔂��ł��ڂ��ɂȂ��Ă���Ƃ��j
	//�@�@�@�@�@�@�����`�ɂȂ�悤�ɂ��̃f�[�^�����Đ��`����Bnullptr�Ȃ琮�`���Ȃ��B
	//ret : �������^
	bool ReadCsvFile( const WString& filename , WTable& retTable , WString* empty_str = nullptr );

	//CSV�t�@�C����ǂݎ���Ĕz��Ɋi�[����
	//�E�ǂݎ��Ƃ��̕����R�[�h�́AretTable�Ɏw�肵���^�ɍ��킹��
	//filename : �ǂݎ��t�@�C����
	//retTable : �i�[��
	//empty_str : �ǂݎ�茋�ʂ������`�ɂȂ��Ă��Ȃ��Ƃ��i�s�^�񐔂��ł��ڂ��ɂȂ��Ă���Ƃ��j
	//�@�@�@�@�@�@�����`�ɂȂ�悤�ɂ��̃f�[�^�����Đ��`����Bnullptr�Ȃ琮�`���Ȃ��B
	//ret : �������^
	bool ReadCsvFile( const WString& filename , ATable& retTable , AString* empty_str = nullptr );


	//CSV�t�@�C����ǂݎ���Ĕz��Ɋi�[����
	//�E�ǂݎ��Ƃ��̕����R�[�h�́AretTable�Ɏw�肵���^�ɍ��킹��
	//stream : �ǂݎ��X�g���[��
	//retTable : �i�[��
	//empty_str : �ǂݎ�茋�ʂ������`�ɂȂ��Ă��Ȃ��Ƃ��i�s�^�񐔂��ł��ڂ��ɂȂ��Ă���Ƃ��j
	//�@�@�@�@�@�@�����`�ɂȂ�悤�ɂ��̃f�[�^�����Đ��`����Bnullptr�Ȃ琮�`���Ȃ��B
	//ret : �������^
	bool ReadCsvFile( mFileReadStreamBase& stream , WTable& retTable , WString* empty_str = nullptr );

	//CSV�t�@�C����ǂݎ���Ĕz��Ɋi�[����
	//�E�ǂݎ��Ƃ��̕����R�[�h�́AretTable�Ɏw�肵���^�ɍ��킹��
	//stream : �ǂݎ��X�g���[��
	//retTable : �i�[��
	//empty_str : �ǂݎ�茋�ʂ������`�ɂȂ��Ă��Ȃ��Ƃ��i�s�^�񐔂��ł��ڂ��ɂȂ��Ă���Ƃ��j
	//�@�@�@�@�@�@�����`�ɂȂ�悤�ɂ��̃f�[�^�����Đ��`����Bnullptr�Ȃ琮�`���Ȃ��B
	//ret : �������^
	bool ReadCsvFile( mFileReadStreamBase& stream , ATable& retTable , AString* empty_str = nullptr );


};


#endif
