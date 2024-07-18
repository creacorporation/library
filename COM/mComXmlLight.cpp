//----------------------------------------------------------------------------
// XML操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MCOMXMLLIGHT_CPP_COMPILING
#include "mComXmlLight.h"
#include "General/mErrorLogger.h"

#pragma comment(lib, "XmlLite.lib")

mComXmlLight::mComXmlLight()
{
}

mComXmlLight::~mComXmlLight()
{
}

bool mComXmlLight::Read( mComStream& stream )
{
	HRESULT hr;
	IXmlReader* reader = nullptr;
	bool result = false;

	if( FAILED( hr = CreateXmlReader( __uuidof( IXmlReader ) , (void**)&reader , nullptr ) ) )
	{
		wprintf(L"Error creating xml reader, error is %08.8lx", hr);
		return false;
	}

	if( FAILED( hr = reader->SetProperty( XmlReaderProperty_DtdProcessing , DtdProcessing_Prohibit ) ) )
	{
		wprintf(L"Error setting XmlReaderProperty_DtdProcessing, error is %08.8lx", hr);
		return false;
	}

	if( FAILED( hr = reader->SetInput( stream ) ) )
	{
		wprintf(L"Error setting input for reader, error is %08.8lx", hr);
		return false;
	}

	MyRootElement.Child.clear();
	MyRootElement.Name = L"";
	MyRootElement.Prefix = L"";
	if( !ParseMain( MyRootElement , reader ) )
	{
		reader->Release();
		return false;
	}
	reader->Release();
	return true;
}

bool mComXmlLight::ParseMain( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	HRESULT hr;
	XmlNodeType nodetype;

	while( S_OK == ( hr = reader->Read( &nodetype ) ) )
	{
		switch( nodetype )
		{
		case XmlNodeType_XmlDeclaration:
			ParseXmlDeclaration( parent , reader );
			break;
		case XmlNodeType_Element:
			ParseElement( parent , reader );
			break;
		case XmlNodeType_Text:
			ParseText( parent , reader );
			break;
		case XmlNodeType_Whitespace:
			break;
		case XmlNodeType_CDATA:
			ParseCDATA( parent , reader );
			break;
		case XmlNodeType_ProcessingInstruction:
			ParseProcessingInstruction( parent , reader );
			break;
		case XmlNodeType_Comment:
			ParseComment( parent , reader );
			break;
		case XmlNodeType_DocumentType:
			ParseDocumentType( parent , reader );
			break;
		case XmlNodeType_Attribute:
		case XmlNodeType_None:
			return false;
		case XmlNodeType_EndElement:
			return true;
		default:
			return false;
		}
	}
	return true;
}

template< class c >
static bool ReadName( c& ioObject , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	hr = reader->GetLocalName( &ptr , nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}
	ioObject.Name = ptr;
	return true;
}

template< class c >
static bool ReadPrefix( c& ioObject , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	hr = reader->GetPrefix( &ptr , nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}
	ioObject.Prefix = ptr;
	return true;
}

bool mComXmlLight::ParseElement( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	std::unique_ptr< mComXmlLightObject_Element_Child > elm( mNew mComXmlLightObject_Element_Child() );

	//プレフィクス
	if( !ReadPrefix< mComXmlLightObject_Element_Child >( *elm , reader ) )
	{
		return false;
	}
	//名前
	if( !ReadName< mComXmlLightObject_Element_Child >( *elm , reader ) )
	{
		return false;
	}

	//アトリビュート
	if( !ParseAttribute( *elm , reader ) )
	{
		return false;
	}

	//子ノード
	if( !reader->IsEmptyElement() )
	{
		if( !ParseMain( *elm , reader ) )
		{
			return false;
		}
	}
	parent.Child.push_back( std::move( elm ) );
	return true;
}

bool mComXmlLight::ParseAttribute( mComXmlLightObject_WithChildObject& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr = reader->MoveToFirstAttribute();

	if( hr == S_FALSE )
	{
		//アトリビュートなし
		return true;
	}
	else if ( hr == S_OK )
	{
		do
		{
			if( !reader->IsDefault() )
			{
				std::unique_ptr< mComXmlLightObject_Attribute > attr( mNew mComXmlLightObject_Attribute() );
				//プレフィクス
				if( !ReadPrefix< mComXmlLightObject_Attribute >( *attr , reader ) )
				{
					return false;
				}
				//名前
				if( !ReadName< mComXmlLightObject_Attribute >( *attr , reader ) )
				{
					return false;
				}
				//値
				hr = reader->GetValue( &ptr, nullptr );
				if( FAILED( hr ) )
				{
					return false;
				}
				attr->Value = ptr;
				parent.Child.push_back( std::move( attr ) );
			}

			hr = reader->MoveToNextAttribute();
		}
		while( hr == S_OK );
	}
	return hr;

	return true;
}

bool mComXmlLight::ParseText( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mComXmlLightObject_Text > txt( mNew mComXmlLightObject_Text() );

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}
	txt->Text = ptr;
	parent.Child.push_back( std::move( txt ) );
	return true;
}

bool mComXmlLight::ParseCDATA( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mComXmlLightObject_CDATA > txt( mNew mComXmlLightObject_CDATA() );

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}
	txt->Text = ptr;
	parent.Child.push_back( std::move( txt ) );
	return true;
}

bool mComXmlLight::ParseProcessingInstruction( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mComXmlLightObject_ProcessingInstruction > procinst( mNew mComXmlLightObject_ProcessingInstruction() );

	//名前
	if( !ReadName< mComXmlLightObject_ProcessingInstruction >( *procinst , reader ) )
	{
		return false;
	}
	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}
	procinst->Value = ptr;
	parent.Child.push_back( std::move( procinst ) );
	return true;
}

bool mComXmlLight::ParseComment( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mComXmlLightObject_Comment > comment( mNew mComXmlLightObject_Comment() );

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}

	parent.Child.push_back( std::move( comment ) );
	return true;
}

bool mComXmlLight::ParseDocumentType( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	std::unique_ptr< mComXmlLightObject_DocumentType > elm( mNew mComXmlLightObject_DocumentType() );

	//TODO

	parent.Child.push_back( std::move( elm ) );
	return true;
}

bool mComXmlLight::ParseXmlDeclaration( mComXmlLightObject_Element_Child& parent , IXmlReader* reader )
{
	std::unique_ptr< mComXmlLightObject_XmlDeclaration_Child > elm( mNew mComXmlLightObject_XmlDeclaration_Child() );

	//アトリビュート
	if( !ParseAttribute( *elm , reader ) )
	{
		return false;
	}

	parent.Child.push_back( std::move( elm ) );
	return true;
}

std::unique_ptr<mComXmlLightObject>& mComXmlLight::Lookup( const WString& addr , wchar_t delimiter )
{
	WStringDeque name;
	ParseString( addr , delimiter , name , false );


	for( WStringDeque::const_iterator itr = name.begin() ; itr != name.end() ; itr++ )
	{




	}

}
