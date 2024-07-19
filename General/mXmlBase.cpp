//----------------------------------------------------------------------------
// XML操作
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#define MXMLBASE_CPP_COMPILING
#include "mXmlBase.h"
#include "General/mErrorLogger.h"

#pragma comment(lib, "XmlLite.lib")

mXmlBase::mXmlBase()
{
}

mXmlBase::~mXmlBase()
{
}

bool mXmlBase::Read( mComStream& stream )
{
	HRESULT hr;
	IXmlReader* reader = nullptr;
	bool result = false;
	std::unique_ptr< mXmlObject_Element_Child > elm;

	if( FAILED( hr = CreateXmlReader( __uuidof( IXmlReader ) , (void**)&reader , nullptr ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"IXmlReaderの生成に失敗" );
		goto end;
	}

	if( FAILED( hr = reader->SetProperty( XmlReaderProperty_DtdProcessing , DtdProcessing_Prohibit ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"DTD設定が失敗" );
		goto end;
	}

	if( FAILED( hr = reader->SetInput( stream ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"入力ストリームの指定が失敗" );
		goto end;
	}

	elm.reset( mNew mXmlObject_Element_Child() );
	if( ParseMain( L"\\" , *elm , reader ) == OnReadResult::Fail )
	{
		goto end;
	}
	if( !OnReadRoot( std::move( elm ) ) )
	{
		goto end;
	}

	result = true;
end:
	if( reader )
	{
		reader->Release();
	}
	return result;
}

bool mXmlBase::Write( mComStream& stream )const
{
	HRESULT hr;
	IXmlWriter* writer = nullptr;

	const mXmlObject_Element_Child* elm = OnWriteRoot();
	if( !elm )
	{
		RaiseError( g_ErrorLogger , 0 , L"出力対象が指定されませんでした" );
		return false;
	}

	if( FAILED( hr = CreateXmlWriter( __uuidof( IXmlWriter ) , (void**)&writer ,nullptr ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"IXmlWriteの生成に失敗" );
		return false;
	}

	if( FAILED( hr = writer->SetOutput( stream ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"出力先の指定が失敗" );
		return false;
	}

	if( FAILED ( hr = writer->SetProperty( XmlWriterProperty_Indent , TRUE ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"プロパティの設定エラー" );
		return false;
	}

	if( FAILED( hr = writer->WriteStartDocument( XmlStandalone_Omit ) ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ドキュメントの開始が失敗" );
		return false;
	}

	if( !WriteMain( *elm , writer ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"出力が失敗しました" );
		return false;
	}
	return true;
}

bool mXmlBase::WriteMain( const mXmlObject_WithChildObject& obj , IXmlWriter* writer )const
{
	HRESULT hr;

	for( mXmlObject_WithChildObject::ChildObjectArray::const_iterator itr = obj.Child.begin() ; itr != obj.Child.end() ; itr++ )
	{
		switch( (*itr)->Type )
		{
		case mXmlObjectType::XmlObjectType_Element:
		{
			const mXmlObject_Element_Child* elm = reinterpret_cast< const mXmlObject_Element_Child* >( itr->get() );
			if( !WriteElement( *elm , writer ) )
			{
				return false;
			}
			hr = S_OK;
		}
		case mXmlObjectType::XmlObjectType_Attribute:
		{
			const mXmlObject_Attribute* attr = reinterpret_cast< const mXmlObject_Attribute* >( itr->get() );
			hr = writer->WriteAttributeString( attr->Prefix.c_str() , attr->Name.c_str() , attr->NamespaceUri.c_str() , attr->Value.c_str() );
			break;
		}
		case mXmlObjectType::XmlObjectType_Text:
		{
			const mXmlObject_Text* txt = reinterpret_cast< const mXmlObject_Text* >( itr->get() );
			hr = writer->WriteString( txt->Text.c_str() );
			break;
		}
		case mXmlObjectType::XmlObjectType_CDATA:
		{
			const mXmlObject_CDATA* txt = reinterpret_cast< const mXmlObject_CDATA* >( itr->get() );
			hr = writer->WriteCData( txt->Text.c_str() );
			break;
		}
		case mXmlObjectType::XmlObjectType_Comment:
		{
			const mXmlObject_Comment* comment = reinterpret_cast< const mXmlObject_Comment* >( itr->get() );
			hr = writer->WriteComment( comment->Text.c_str() );
			break;
		}
		case mXmlObjectType::XmlObjectType_XmlDeclaration:
		{
			const mXmlObject_XmlDeclaration_Child* decl = reinterpret_cast< const mXmlObject_XmlDeclaration_Child* >( itr->get() );
			break;
		}
		case mXmlObjectType::XmlObjectType_ProcessingInstruction:
		{
			const mXmlObject_ProcessingInstruction* procinst = reinterpret_cast< const mXmlObject_ProcessingInstruction* >( itr->get() );
			hr = writer->WriteProcessingInstruction( procinst->Name.c_str() , procinst->Value.c_str() );
			break;
		}
		case mXmlObjectType::XmlObjectType_DocumentType:
		{
			const mXmlObject_DocumentType* elm = reinterpret_cast< const mXmlObject_DocumentType* >( itr->get() );
			//TODO
			break;
		}
		default:
			RaiseError( g_ErrorLogger , 0 , L"オブジェクトの内部形式が不正" );
			return false;
		}
		if( FAILED( hr ) )
		{
			RaiseErrorF( g_ErrorLogger , 0 , L"エレメントの出力が失敗" , L"type=%d" , (*itr)->Type );
			return false;
		}
	}
	return true;
}

bool mXmlBase::WriteElement( const mXmlObject_Element_Child& obj , IXmlWriter* writer )const
{
	if( FAILED( writer->WriteStartElement( obj.Prefix.c_str() , obj.Name.c_str() , obj.NamespaceUri.c_str() ) ) )
	{
		return false;
	}
	if( !WriteMain( obj , writer ) )
	{
		return false;
	}
	if( FAILED( writer->WriteFullEndElement() ) )
	{
		return false;
	}
	return true;
}


mXmlBase::OnReadResult mXmlBase::ParseMain( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	HRESULT hr;
	XmlNodeType nodetype;
	OnReadResult result = OnReadResult::Next;

	while( S_OK == ( hr = reader->Read( &nodetype ) ) )
	{
		if( nodetype == XmlNodeType_EndElement )
		{
			return OnReadResult::Next;
		}
		if( result == OnReadResult::Skip )
		{
			continue;
		}
		switch( nodetype )
		{
		case XmlNodeType_XmlDeclaration:
			result = ParseXmlDeclaration( path , parent , reader );
			break;
		case XmlNodeType_Element:
			result = ParseElement( path , parent , reader );
			break;
		case XmlNodeType_Text:
			result = ParseText( path , parent , reader );
			break;
		case XmlNodeType_Whitespace:
			break;
		case XmlNodeType_CDATA:
			result = ParseCDATA( path , parent , reader );
			break;
		case XmlNodeType_ProcessingInstruction:
			result = ParseProcessingInstruction( path , parent , reader );
			break;
		case XmlNodeType_Comment:
			result = ParseComment( path , parent , reader );
			break;
		case XmlNodeType_DocumentType:
			result = ParseDocumentType( path , parent , reader );
			break;
		case XmlNodeType_Attribute:
		case XmlNodeType_None:
		default:
			return OnReadResult::Fail;
		}
		switch( result )
		{
		case OnReadResult::Next:
		case OnReadResult::Skip:
			break;
		case OnReadResult::Finish:
		case OnReadResult::Fail:
			return result;
		default:
			return OnReadResult::Fail;
		}
	}
	return OnReadResult::Next;
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

template< class c >
static bool ReadNamespaceUri( c& ioObject , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	hr = reader->GetNamespaceUri( &ptr , nullptr );
	if( FAILED( hr ) )
	{
		return false;
	}
	ioObject.NamespaceUri = ptr;
	return true;
}

mXmlBase::OnReadResult mXmlBase::ParseElement( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	std::unique_ptr< mXmlObject_Element_Child > elm( mNew mXmlObject_Element_Child() );

	//プレフィクス
	if( !ReadPrefix< mXmlObject_Element_Child >( *elm , reader ) )
	{
		return OnReadResult::Fail;
	}
	//名前
	if( !ReadName< mXmlObject_Element_Child >( *elm , reader ) )
	{
		return OnReadResult::Fail;
	}
	//アトリビュート
	if( !ParseAttribute( *elm , reader ) )
	{
		return OnReadResult::Fail;
	}
	//URI
	if( !ReadNamespaceUri< mXmlObject_Element_Child >( *elm , reader ) )
	{
		return OnReadResult::Fail;
	}

	//子ノード
	if( !reader->IsEmptyElement() )
	{
		WString childpath = path + elm->Name + L"\\" ;
		OnReadResult result = ParseMain( childpath , *elm , reader );
		if( result != OnReadResult::Next )
		{
			return result;
		}
	}

	return OnReadElement( path , parent , std::move( elm ) );
}

bool mXmlBase::ParseAttribute( mXmlObject_WithChildObject& parent , IXmlReader* reader )
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
				std::unique_ptr< mXmlObject_Attribute > attr( mNew mXmlObject_Attribute() );
				//プレフィクス
				if( !ReadPrefix< mXmlObject_Attribute >( *attr , reader ) )
				{
					return false;
				}
				//名前
				if( !ReadName< mXmlObject_Attribute >( *attr , reader ) )
				{
					return false;
				}
				//URI
				if( !ReadNamespaceUri< mXmlObject_Attribute >( *attr , reader ) )
				{
					return OnReadResult::Fail;
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
	return true;
}

mXmlBase::OnReadResult mXmlBase::ParseText( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mXmlObject_Text > txt( mNew mXmlObject_Text() );

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return OnReadResult::Fail;
	}
	txt->Text = ptr;

	return OnReadText( path , parent , std::move( txt ) );
}

mXmlBase::OnReadResult mXmlBase::ParseCDATA( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mXmlObject_CDATA > txt( mNew mXmlObject_CDATA() );

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return OnReadResult::Fail;
	}
	txt->Text = ptr;

	return OnReadCDATA( path , parent , std::move( txt ) );
}

mXmlBase::OnReadResult mXmlBase::ParseProcessingInstruction( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mXmlObject_ProcessingInstruction > procinst( mNew mXmlObject_ProcessingInstruction() );

	//名前
	if( !ReadName< mXmlObject_ProcessingInstruction >( *procinst , reader ) )
	{
		return OnReadResult::Fail;
	}
	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return OnReadResult::Fail;
	}
	procinst->Value = ptr;

	return OnReadProcessingInstruction( path , parent , std::move( procinst ) );
}

mXmlBase::OnReadResult mXmlBase::ParseComment( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	std::unique_ptr< mXmlObject_Comment > comment( mNew mXmlObject_Comment() );

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( FAILED( hr ) )
	{
		return OnReadResult::Fail;
	}

	return OnReadComment( path , parent , std::move( comment ) );
}

mXmlBase::OnReadResult mXmlBase::ParseDocumentType( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	std::unique_ptr< mXmlObject_DocumentType > elm( mNew mXmlObject_DocumentType() );

	//TODO

	return OnReadDocumentType( path , parent , std::move( elm ) );
}

mXmlBase::OnReadResult mXmlBase::ParseXmlDeclaration( const WString& path , mXmlObject_Element_Child& parent , IXmlReader* reader )
{
	std::unique_ptr< mXmlObject_XmlDeclaration_Child > elm( mNew mXmlObject_XmlDeclaration_Child() );

	//アトリビュート
	if( !ParseAttribute( *elm , reader ) )
	{
		return OnReadResult::Fail;
	}

	return OnReadXmlDeclaration( path , parent , std::move( elm ) );
}

bool mXmlBase::OnReadRoot( std::unique_ptr<mXmlObject_Element_Child>&& obj )
{
	return true;
}

mXmlBase::OnReadResult mXmlBase::OnReadElement( const WString& path , mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Element_Child>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_Element_Child>>( obj ) );
	return OnReadResult::Next;
}

mXmlBase::OnReadResult mXmlBase::OnReadText( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Text>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_Text>>( obj ) );
	return OnReadResult::Next;
}

mXmlBase::OnReadResult mXmlBase::OnReadCDATA( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_CDATA>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_CDATA>>( obj ) );
	return OnReadResult::Next;
}

mXmlBase::OnReadResult mXmlBase::OnReadProcessingInstruction( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_ProcessingInstruction>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_ProcessingInstruction>>( obj ) );
	return OnReadResult::Next;
}

mXmlBase::OnReadResult mXmlBase::OnReadComment( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_Comment>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_Comment>>( obj ) );
	return OnReadResult::Next;
}

mXmlBase::OnReadResult mXmlBase::OnReadDocumentType( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_DocumentType>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_DocumentType>>( obj ) );
	return OnReadResult::Next;
}

mXmlBase::OnReadResult mXmlBase::OnReadXmlDeclaration( const WString& path ,  mXmlObject_Element_Child& parent , std::unique_ptr<mXmlObject_XmlDeclaration_Child>&& obj )
{
	parent.Child.push_back( std::forward<std::unique_ptr<mXmlObject_XmlDeclaration_Child>>( obj ) );
	return OnReadResult::Next;
}

const mXmlBase::mXmlObject_Element_Child* mXmlBase::OnWriteRoot( void )const
{
	return false;
}

