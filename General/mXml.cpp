//----------------------------------------------------------------------------
// XML操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MXML_CPP_COMPILING
#include "mXml.h"
#include "General/mErrorLogger.h"

#pragma comment(lib, "XmlLite.lib")

mXml::mXml()
{
}

mXml::~mXml()
{
}

bool mXml::Read( mComStream& stream )
{
	if( !mXmlBase::Read( stream ) )
	{
		MyRoot.reset();
		return false;
	}
	return true;
}

bool mXml::Write( mComStream& stream )const
{
	return mXmlBase::Write( stream );
}

bool mXml::OnReadRoot( std::unique_ptr<mXmlObject_Element_Child>&& obj )
{
	MyRoot = std::move( obj );
	return true;
}

const mXml::mXmlObject_Element_Child* mXml::OnWriteRoot( void )const
{
	return MyRoot.get();
}

