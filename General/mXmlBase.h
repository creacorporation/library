//----------------------------------------------------------------------------
// XML操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MXMLBASE_H_INCLUDED
#define MXMLBASE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "COM/mComStream.h"
#include <deque>
#include <xmllite.h>

enum mXmlObjectType
{
	XmlObjectType_Element,
	XmlObjectType_Attribute,
	XmlObjectType_Text,
	XmlObjectType_CDATA,
	XmlObjectType_Comment,
	XmlObjectType_XmlDeclaration,
	XmlObjectType_ProcessingInstruction,
	XmlObjectType_DocumentType,
};

class mXmlObject
{
protected:
	mXmlObject( mXmlObjectType type )
		: Type( type )
	{
	}
public:
	virtual ~mXmlObject(){};
public:
	const mXmlObjectType Type;
};


class mXmlObject_Element : public mXmlObject
{
public:
	mXmlObject_Element()
		: mXmlObject( XmlObjectType_Element )
	{
	}
	mXmlObject_Element( const WString& prefix , const WString& name )
		: mXmlObject( XmlObjectType_Element )
	{
		Prefix = prefix;
		Name = name;
	}
	virtual ~mXmlObject_Element(){};
public:
	WString Prefix;
	WString Name;
	WString NamespaceUri;
};

class mXmlObject_Attribute : public mXmlObject
{
public:
	mXmlObject_Attribute()
		: mXmlObject( XmlObjectType_Attribute )
	{
	}
	mXmlObject_Attribute( const WString& prefix , const WString& name , const WString& value )
		: mXmlObject( XmlObjectType_Attribute )
	{
		Prefix = prefix;
		Name = name;
		Value = value;
	}
	virtual ~mXmlObject_Attribute(){};
public:

	WString Prefix;
	WString Name;
	WString Value;
	WString NamespaceUri;
};

class mXmlObject_Text : public mXmlObject
{
public:
	mXmlObject_Text()
		: mXmlObject( XmlObjectType_Text )
	{
	}
	mXmlObject_Text( const WString& text )
		: mXmlObject( XmlObjectType_Text )
	{
		Text = text;
	}
	virtual ~mXmlObject_Text(){};
public:
	WString Text;
};

class mXmlObject_CDATA : public mXmlObject
{
public:
	mXmlObject_CDATA()
		: mXmlObject( XmlObjectType_CDATA )
	{
	}
	mXmlObject_CDATA( const WString& text )
		: mXmlObject( XmlObjectType_CDATA )
	{
		Text = text;
	}
	virtual ~mXmlObject_CDATA(){};
public:
	WString Text;
};

class mXmlObject_Comment : public mXmlObject
{
public:
	mXmlObject_Comment()
		: mXmlObject( XmlObjectType_Comment )
	{
	}
	mXmlObject_Comment( const WString& text )
		: mXmlObject( XmlObjectType_Comment )
	{
		Text = text;
	}
	virtual ~mXmlObject_Comment(){};
public:
	WString Text;
};

class mXmlObject_XmlDeclaration : public mXmlObject
{
public:
	mXmlObject_XmlDeclaration()
		: mXmlObject( XmlObjectType_XmlDeclaration )
	{
	}
	virtual ~mXmlObject_XmlDeclaration(){};
public:
};

class mXmlObject_ProcessingInstruction : public mXmlObject
{
public:
	mXmlObject_ProcessingInstruction()
		: mXmlObject( XmlObjectType_ProcessingInstruction )
	{
	}
	mXmlObject_ProcessingInstruction( const WString& name , const WString& value )
		: mXmlObject( XmlObjectType_ProcessingInstruction )
	{
		Name = name;
		Value = value;
	}
	virtual ~mXmlObject_ProcessingInstruction(){};
public:
	WString Name;
	WString Value;
};

class mXmlObject_DocumentType : public mXmlObject
{
public:
	mXmlObject_DocumentType()
		: mXmlObject( XmlObjectType_DocumentType )
	{
	}
	virtual ~mXmlObject_DocumentType(){};
public:
};

class mXmlBase
{
public:
	mXmlBase();
	virtual ~mXmlBase();

	virtual bool Read( mComStream& stream );
	virtual bool Write( mComStream& stream )const;

private:

	mXmlBase( const mXmlBase& source ) = delete;
	void operator=( const mXmlBase& source ) = delete;

protected:

	class mXmlObject_WithChildObject
	{
	public:
		using ChildObject = std::unique_ptr<mXmlObject>;
		using ChildObjectArray = std::deque< ChildObject >;
		ChildObjectArray Child;
	};

	class mXmlObject_Element_Child : public mXmlObject_Element , public mXmlObject_WithChildObject
	{
	};
	class mXmlObject_XmlDeclaration_Child : public mXmlObject_XmlDeclaration , public mXmlObject_WithChildObject
	{
	};

	enum OnReadResult
	{
		Fail,		//
		Next,		//
		Skip,		//
		Finish,		//
	};

	//
	virtual bool OnReadRoot( std::unique_ptr<mXmlObject_Element_Child>&& obj );
	virtual OnReadResult OnReadElement( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Element_Child>&& obj );
	virtual OnReadResult OnReadText( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Text>&& obj );
	virtual OnReadResult OnReadCDATA( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_CDATA>&& obj );
	virtual OnReadResult OnReadProcessingInstruction( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_ProcessingInstruction>&& obj );
	virtual OnReadResult OnReadComment( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Comment>&& obj );
	virtual OnReadResult OnReadDocumentType( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_DocumentType>&& obj );
	virtual OnReadResult OnReadXmlDeclaration( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_XmlDeclaration_Child>&& obj );

	virtual const mXmlObject_Element_Child* OnWriteRoot( void )const;

private:

	//要素の読み取り
	OnReadResult ParseMain( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseElement( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseText( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseCDATA( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseProcessingInstruction( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseComment( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseDocumentType( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResult ParseXmlDeclaration( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	//アトリビュートの読み取り
	bool ParseAttribute( mXmlObject_WithChildObject& parent , IXmlReader* reader );

	//要素の出力
	bool WriteMain( const mXmlObject_WithChildObject& obj , IXmlWriter* writer )const;
	bool WriteElement( const mXmlObject_Element_Child& obj , IXmlWriter* writer )const;

};

#endif