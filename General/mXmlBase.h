//----------------------------------------------------------------------------
// XML����
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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

namespace mXmlBase_Definitions
{
	enum OnReadResult
	{
		Fail,		// �ǂݎ�莸�s�̏ꍇ�i�G���[�I���j
		Next,		// �ǂݎ��n�j�̏ꍇ�i�������s�j
		Skip,		// ���Y�G�������g�ɂ��Ĉȍ~�S�Ė�������ꍇ�i���̃G�������g���瑱�s�j
		Finish,		// �ǂݎ��I���̏ꍇ
	};
}

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

	using OnReadResult = mXmlBase_Definitions::OnReadResult;

	//���[�g�G�������g���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	virtual bool OnReadRoot( std::unique_ptr<mXmlObject_Element_Child>&& obj );

	//���[�g�ȊO�̃G�������g���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadElement( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Element_Child>&& obj );

	//�e�L�X�g�f�[�^���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadText( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Text>&& obj );

	//CDATA���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadCDATA( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_CDATA>&& obj );

	//�������߂��ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadProcessingInstruction( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_ProcessingInstruction>&& obj );

	//�R�����g���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadComment( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Comment>&& obj );

	//DOM���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadDocumentType( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_DocumentType>&& obj );

	//XML�錾���ǂݎ�芮�������Ƃ��ɃR�[���o�b�N����܂�
	//�����̃R�[���o�b�N�֐����ŁAobj��parent��Child�����o�ɒǉ����Ȃ���΁A�ǂݎ�����f�[�^�͔j������邱�ƂɂȂ�܂�
	// path : ���[�g�G�������g����A�Ȃ�G�������g����'\\'�ŘA������������B�擪�Ɩ�����'\\'�B
	// parent : �ǂݎ�����f�[�^�����ڏ������邱�ƂɂȂ�e�G�������g
	// obj : �V���ɓǂݎ�����f�[�^
	// ret : ���̌�̏���������OnReadResult�̒l
	virtual OnReadResult OnReadXmlDeclaration( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_XmlDeclaration_Child>&& obj );

	virtual const mXmlObject_Element_Child* OnWriteRoot( void )const;

private:
	enum OnReadResultEx
	{
		Fail   = OnReadResult::Fail,		// �ǂݎ�莸�s�̏ꍇ�i�G���[�I���j
		Next   = OnReadResult::Next,		// �ǂݎ��n�j�̏ꍇ�i�������s�j
		Skip   = OnReadResult::Skip,		// ���Y�G�������g�ɂ��Ĉȍ~�S�Ė�������ꍇ�i���̃G�������g���瑱�s�j
		Finish = OnReadResult::Finish,	// �ǂݎ��I���̏ꍇ
		Pending,
		SkipPending,
	};

	//�v�f�̓ǂݎ��
	OnReadResultEx ParseMain( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseElement( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseText( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseCDATA( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseProcessingInstruction( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseComment( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseDocumentType( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	OnReadResultEx ParseXmlDeclaration( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader );
	//�A�g���r���[�g�̓ǂݎ��
	bool ParseAttribute( mXmlObject_WithChildObject& parent , IXmlReader* reader );

	//�v�f�̏o��
	bool WriteMain( const mXmlObject_WithChildObject& obj , IXmlWriter* writer )const;
	bool WriteElement( const mXmlObject_Element_Child& obj , IXmlWriter* writer )const;

};

#endif