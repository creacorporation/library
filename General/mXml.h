//----------------------------------------------------------------------------
// XML����
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MXML_H_INCLUDED
#define MXML_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mXmlBase.h"
#include <deque>
#include <xmllite.h>

class mXml : public mXmlBase
{
public:
	mXml();
	virtual ~mXml();

	virtual bool Read( mComStream& stream );
	virtual bool Write( mComStream& stream )const;

private:

	mXml( const mXml& source ) = delete;
	void operator=( const mXml& source ) = delete;

protected:

	virtual bool OnReadRoot( std::unique_ptr<mXmlObject_Element_Child>&& obj );
	virtual const mXmlObject_Element_Child* OnWriteRoot( void )const;

	std::unique_ptr<mXmlObject_Element_Child> MyRoot;

};

#endif