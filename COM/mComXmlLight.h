//----------------------------------------------------------------------------
// XML操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MCOMMEMSTREAM_H_INCLUDED
#define MCOMMEMSTREAM_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mComFileStream.h"
#include <deque>

class mComXmlLight
{
public:

	mComXmlLight();
	virtual ~mComXmlLight();

	struct Attribute
	{
		WString Name;
		WString Value;
	};

	struct Element
	{
		WString Prefix;
		WString Name;
		
		WStringDeque ChildName;
	};


private:

	mComXmlLight( const mComXmlLight& source ) = delete;
	void operator=( const mComXmlLight& source ) = delete;

protected:


};

#endif