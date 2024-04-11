//----------------------------------------------------------------------------
// �R�[�h�y�[�W�n���h��
// Copyright (C) 2020 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MCODEPAGE_H_INCLUDED
#define MCODEPAGE_H_INCLUDED

#include "mStandard.h"

namespace mCodePage
{
	enum CodePage
	{
		SHIFTJIS,
		UTF8,
		UTF16,
		EUCJP,
		UNKNOWN
	};

	//�R�[�h�y�[�W��ݒ肵�܂�
	bool SetConsoleInputCodePage( CodePage cp );

	//�R�[�h�y�[�W���擾���܂�
	bool GetConsoleInputCodePage( CodePage& retCp );

	//�R�[�h�y�[�W��ݒ肵�܂�
	bool SetConsoleOutputCodePage( CodePage cp );

	//�R�[�h�y�[�W���擾���܂�
	bool GetConsoleOutputCodePage( CodePage& retCp );

};



#endif
