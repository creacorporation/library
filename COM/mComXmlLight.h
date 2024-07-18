//----------------------------------------------------------------------------
// XML操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MCOMXMLLIGHT_H_INCLUDED
#define MCOMXMLLIGHT_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mComStream.h"
#include <deque>
#include <xmllite.h>

enum mComXmlLightObjectType
{
	XmlLightObjectType_Element,
	XmlLightObjectType_Attribute,
	XmlLightObjectType_Text,
	XmlLightObjectType_CDATA,
	XmlLightObjectType_Comment,
	XmlLightObjectType_XmlDeclaration,
	XmlLightObjectType_ProcessingInstruction,
	XmlLightObjectType_DocumentType,
};

class mComXmlLightObject
{
protected:
	mComXmlLightObject( mComXmlLightObjectType type )
		: Type( type )
	{
	}
public:
	virtual ~mComXmlLightObject(){};
public:
	const mComXmlLightObjectType Type;
};


class mComXmlLightObject_Element : public mComXmlLightObject
{
public:
	mComXmlLightObject_Element()
		: mComXmlLightObject( XmlLightObjectType_Element )
	{
	}
	mComXmlLightObject_Element( const WString& prefix , const WString& name )
		: mComXmlLightObject( XmlLightObjectType_Element )
	{
		Prefix = prefix;
		Name = name;
	}
	virtual ~mComXmlLightObject_Element(){};
public:
	WString Prefix;
	WString Name;
};

class mComXmlLightObject_Attribute : public mComXmlLightObject
{
public:
	mComXmlLightObject_Attribute()
		: mComXmlLightObject( XmlLightObjectType_Attribute )
	{
	}
	mComXmlLightObject_Attribute( const WString& prefix , const WString& name , const WString& value )
		: mComXmlLightObject( XmlLightObjectType_Attribute )
	{
		Prefix = prefix;
		Name = name;
		Value = value;
	}
	virtual ~mComXmlLightObject_Attribute(){};
public:

	WString Prefix;
	WString Name;
	WString Value;
};

class mComXmlLightObject_Text : public mComXmlLightObject
{
public:
	mComXmlLightObject_Text()
		: mComXmlLightObject( XmlLightObjectType_Text )
	{
	}
	mComXmlLightObject_Text( const WString& text )
		: mComXmlLightObject( XmlLightObjectType_Text )
	{
		Text = text;
	}
	virtual ~mComXmlLightObject_Text(){};
public:
	WString Text;
};

class mComXmlLightObject_CDATA : public mComXmlLightObject
{
public:
	mComXmlLightObject_CDATA()
		: mComXmlLightObject( XmlLightObjectType_CDATA )
	{
	}
	mComXmlLightObject_CDATA( const WString& text )
		: mComXmlLightObject( XmlLightObjectType_CDATA )
	{
		Text = text;
	}
	virtual ~mComXmlLightObject_CDATA(){};
public:
	WString Text;
};

class mComXmlLightObject_Comment : public mComXmlLightObject
{
public:
	mComXmlLightObject_Comment()
		: mComXmlLightObject( XmlLightObjectType_Comment )
	{
	}
	mComXmlLightObject_Comment( const WString& text )
		: mComXmlLightObject( XmlLightObjectType_Comment )
	{
		Text = text;
	}
	virtual ~mComXmlLightObject_Comment(){};
public:
	WString Text;
};

class mComXmlLightObject_XmlDeclaration : public mComXmlLightObject
{
public:
	mComXmlLightObject_XmlDeclaration()
		: mComXmlLightObject( XmlLightObjectType_XmlDeclaration )
	{
	}
	virtual ~mComXmlLightObject_XmlDeclaration(){};
public:
};

class mComXmlLightObject_ProcessingInstruction : public mComXmlLightObject
{
public:
	mComXmlLightObject_ProcessingInstruction()
		: mComXmlLightObject( XmlLightObjectType_ProcessingInstruction )
	{
	}
	mComXmlLightObject_ProcessingInstruction( const WString& name , const WString& value )
		: mComXmlLightObject( XmlLightObjectType_ProcessingInstruction )
	{
		Name = name;
		Value = value;
	}
	virtual ~mComXmlLightObject_ProcessingInstruction(){};
public:
	WString Name;
	WString Value;
};

class mComXmlLightObject_DocumentType : public mComXmlLightObject
{
public:
	mComXmlLightObject_DocumentType()
		: mComXmlLightObject( XmlLightObjectType_DocumentType )
	{
	}
	virtual ~mComXmlLightObject_DocumentType(){};
public:
};

class mComXmlLight
{
public:
	mComXmlLight();
	virtual ~mComXmlLight();

	bool Read( mComStream& stream );

private:

	mComXmlLight( const mComXmlLight& source ) = delete;
	void operator=( const mComXmlLight& source ) = delete;

protected:

	class mComXmlLightObject_WithChildObject
	{
	public:
		using ChildObject = std::unique_ptr<mComXmlLightObject>;
		using ChildObjectArray = std::deque< ChildObject >;
		ChildObjectArray Child;
	};

	class mComXmlLightObject_Element_Child : public mComXmlLightObject_Element , public mComXmlLightObject_WithChildObject
	{
	};
	class mComXmlLightObject_XmlDeclaration_Child : public mComXmlLightObject_XmlDeclaration , public mComXmlLightObject_WithChildObject
	{
	};

private:

	//要素たち
	bool ParseMain( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseElement( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseText( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseCDATA( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseProcessingInstruction( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseComment( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseDocumentType( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	bool ParseXmlDeclaration( mComXmlLightObject_Element_Child& parent , IXmlReader* reader );
	//アトリビュート
	bool ParseAttribute( mComXmlLightObject_WithChildObject& parent , IXmlReader* reader );

	//ルート
	mComXmlLightObject_Element_Child MyRootElement;

	std::unique_ptr<mComXmlLightObject>& Lookup( const WString& addr , wchar_t delimiter = L'\\' ); 

};

#endif