//----------------------------------------------------------------------------
// コードページハンドラ
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

	//コードページを設定します
	bool SetConsoleInputCodePage( CodePage cp );

	//コードページを取得します
	bool GetConsoleInputCodePage( CodePage& retCp );

	//コードページを設定します
	bool SetConsoleOutputCodePage( CodePage cp );

	//コードページを取得します
	bool GetConsoleOutputCodePage( CodePage& retCp );

};



#endif
