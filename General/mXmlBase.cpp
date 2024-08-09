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
	MyStackStatus = OnReadResultEx::Next;
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


mXmlBase::OnReadResultEx mXmlBase::ParseMain( IXmlReader* reader )
{
	HRESULT hr;
	XmlNodeType nodetype;
//	OnReadResultEx result = OnReadResultEx::Next;

	ElementStack::reverse_iterator element = MyElementStack.rbegin();
	switch( element->LastResult )
	{
	case OnReadResultEx::PendingElementSkip:
		hr = reader->Read( &nodetype );
		result = OnReadResultEx::Skip;
		break;
	case OnReadResultEx::PendingText:
		nodetype = XmlNodeType_Text;
		hr = S_OK;
		break;
	case OnReadResultEx::PendingCDATA:
		nodetype = XmlNodeType_CDATA;
		hr = S_OK;
		break;
	case OnReadResultEx::PendingProcessingInstruction:
		nodetype = XmlNodeType_ProcessingInstruction;
		hr = S_OK;
		break;
	case OnReadResultEx::PendingComment:
		nodetype = XmlNodeType_Comment;
		hr = S_OK;
		break;
	case OnReadResultEx::PendingDeclAttributeTag:
	case OnReadResultEx::PendingDeclAttributeData:
		nodetype = XmlNodeType_XmlDeclaration;
		hr = S_OK;
		break;
	case OnReadResultEx::PendingElementAttributeTag:
	case OnReadResultEx::PendingElementAttributeData:
		nodetype = XmlNodeType_Element;
		hr = S_OK;
		break;
	default:
		hr = reader->Read( &nodetype );
		break;
	}

	while( hr == S_OK )
	{
		if( nodetype == XmlNodeType_EndElement )
		{
			return OnReadResultEx::Next;
		}
		if( result != OnReadResult::Skip )
		{
			switch( nodetype )
			{
			case XmlNodeType_XmlDeclaration:
				ParseXmlDeclaration( *element , reader );
				break;
			case XmlNodeType_Element:
				result = ParseElement( *element , reader );
				break;
			case XmlNodeType_Text:
				result = ParseText( element->Path , *element->Ptr , reader );
				break;
			case XmlNodeType_Whitespace:
				break;
			case XmlNodeType_CDATA:
				result = ParseCDATA( element->Path , *element->Ptr , reader );
				break;
			case XmlNodeType_ProcessingInstruction:
				result = ParseProcessingInstruction( element->Path , *element->Ptr , reader );
				break;
			case XmlNodeType_Comment:
				result = ParseComment( element->Path , *element->Ptr , reader );
				break;
			case XmlNodeType_DocumentType:
				result = ParseDocumentType( element->Path , *element->Ptr , reader );
				break;
			case XmlNodeType_Attribute:
			case XmlNodeType_None:
			default:
				return OnReadResultEx::Fail;
			}

			switch( result )
			{
			case OnReadResultEx::Next:
			case OnReadResultEx::Skip:
				break;
			case OnReadResultEx::Finish:
			case OnReadResultEx::Fail:
				return result;
			case OnReadResultEx::PendingElementNext:
			case OnReadResultEx::PendingElementSkip:
			case OnReadResultEx::PendingText:
			case OnReadResultEx::PendingCDATA:
			case OnReadResultEx::PendingProcessingInstruction:
			case OnReadResultEx::PendingComment:
			case OnReadResultEx::PendingElementAttributeTag:
			case OnReadResultEx::PendingElementAttributeData:
			case OnReadResultEx::PendingDeclAttributeTag:
			case OnReadResultEx::PendingDeclAttributeData:
				element->LastResult = result;
				return result;
			default:
				return OnReadResultEx::Fail;
			}
		}
	}
	if( hr == E_PENDING )
	{
		return OnReadResultEx::PendingElementNext;
	}
	if( FAILED( hr ) )
	{
		return OnReadResultEx::Fail;
	}
	return OnReadResultEx::Next;
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

void mXmlBase::ParseElement( ElementStackEntry& entry , IXmlReader* reader )
{
	if( !entry.Ptr )
	{
		entry.Ptr.reset( mNew mXmlObject_Element_Child() );
		entry.IsEmpty = reader->IsEmptyElement();
	}

	//アトリビュート
	ParseAttributeResult parse_result;
	switch( entry.LastResult )
	{
	case OnReadResultEx::PendingElementAttributeTag:
		parse_result = ParseAttributeResult::PendingAttributeTag;
		break;
	case OnReadResultEx::PendingElementAttributeData:
		parse_result = ParseAttributeResult::PendingAttributeData;
		break;
	default:
		parse_result = ParseAttributeResult::Next;
	}
	switch( ParseAttribute( parse_result , *entry.Ptr , reader ) )
	{
	case ParseAttributeResult::PendingAttributeTag:
		entry.LastResult = OnReadResultEx::PendingElementAttributeTag;
		return;
	case ParseAttributeResult::PendingAttributeData:
		entry.LastResult = OnReadResultEx::PendingElementAttributeData;
		return;
	case ParseAttributeResult::Next:
		break;
	default:
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	//プレフィクス
	if( !ReadPrefix< mXmlObject_Element_Child >( *entry.Ptr , reader ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}
	//名前
	if( !ReadName< mXmlObject_Element_Child >( *entry.Ptr , reader ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}
	//URI
	if( !ReadNamespaceUri< mXmlObject_Element_Child >( *entry.Ptr , reader ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	//子ノード
	if( !entry.IsEmpty )
	{
		WString childpath = path + elm->Name + L"\\" ;
		OnReadResultEx result = ParseMain( childpath , *elm , reader );
		if( result != OnReadResultEx::Next )
		{
			return result;
		}
	}

	return (OnReadResultEx)OnReadElement( path , parent , std::move( elm ) );
}

mXmlBase::ParseAttributeResult mXmlBase::ParseAttribute( ParseAttributeResult prevresult , mXmlObject_WithChildObject& parent , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	switch( prevresult )
	{
	case ParseAttributeResult::PendingAttributeTag:
		if( parent.GetAttrCount() == 0 )
		{
			hr = reader->MoveToFirstAttribute();
		}
		else
		{
			hr = reader->MoveToNextAttribute();
		}
		break;
	case ParseAttributeResult::PendingAttributeData:
		break;
	default:
		hr = reader->MoveToFirstAttribute();
		break;
	}

	while( hr == S_OK )
	{
		if( !reader->IsDefault() )
		{
			//値
			hr = reader->GetValue( &ptr, nullptr );
			if( hr == E_PENDING )
			{
				return ParseAttributeResult::PendingAttributeData;
			}
			if( FAILED( hr ) )
			{
				return ParseAttributeResult::Fail;
			}

			std::unique_ptr< mXmlObject_Attribute > attr( mNew mXmlObject_Attribute() );
			attr->Value = ptr;

			//プレフィクス
			if( !ReadPrefix< mXmlObject_Attribute >( *attr , reader ) )
			{
				return ParseAttributeResult::Fail;
			}
			//名前
			if( !ReadName< mXmlObject_Attribute >( *attr , reader ) )
			{
				return ParseAttributeResult::Fail;
			}
			//URI
			if( !ReadNamespaceUri< mXmlObject_Attribute >( *attr , reader ) )
			{
				return ParseAttributeResult::Fail;
			}
			parent.Child.push_back( std::move( attr ) );
		}

		hr = reader->MoveToNextAttribute();
	}

	if( hr == E_PENDING )
	{
		return ParseAttributeResult::PendingAttributeTag;
	}
	if( FAILED( hr ) )
	{
		return ParseAttributeResult::Fail;
	}
	return ParseAttributeResult::Next;
}

void mXmlBase::ParseText( ElementStackEntry& entry , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( hr == E_PENDING )
	{
		entry.LastResult = OnReadResultEx::PendingText;
		return;
	}
	if( FAILED( hr ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	std::unique_ptr< mXmlObject_Text > txt( mNew mXmlObject_Text() );
	txt->Text = ptr;
	entry.LastResult = (OnReadResultEx)OnReadText( entry.Path , *entry.Ptr , std::move( txt ) );
	return;
}

void mXmlBase::ParseCDATA( ElementStackEntry& entry , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( hr == E_PENDING )
	{
		entry.LastResult = OnReadResultEx::PendingCDATA;
		return;
	}
	if( FAILED( hr ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	std::unique_ptr< mXmlObject_CDATA > txt( mNew mXmlObject_CDATA() );
	txt->Text = ptr;
	entry.LastResult = (OnReadResultEx)OnReadCDATA( entry.Path , *entry.Ptr , std::move( txt ) );
	return;
}

void mXmlBase::ParseProcessingInstruction( ElementStackEntry& entry , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;
	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( hr == E_PENDING )
	{
		entry.LastResult = OnReadResultEx::PendingProcessingInstruction;
		return;
	}
	if( FAILED( hr ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	std::unique_ptr< mXmlObject_ProcessingInstruction > procinst( mNew mXmlObject_ProcessingInstruction() );
	procinst->Value = ptr;

	//名前
	if( !ReadName< mXmlObject_ProcessingInstruction >( *procinst , reader ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	entry.LastResult = (OnReadResultEx)OnReadProcessingInstruction( entry.Path , *entry.Ptr , std::move( procinst ) );
	return;
}

void mXmlBase::ParseComment( ElementStackEntry& entry , IXmlReader* reader )
{
	LPCWSTR ptr;
	HRESULT hr;

	//値
	hr = reader->GetValue( &ptr, nullptr );
	if( hr == E_PENDING )
	{
		entry.LastResult = OnReadResultEx::PendingComment;
		return;
	}
	if( FAILED( hr ) )
	{
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	std::unique_ptr< mXmlObject_Comment > comment( mNew mXmlObject_Comment() );
	comment->Text = ptr;
	entry.LastResult = (OnReadResultEx)OnReadComment( entry.Path , *entry.Ptr , std::move( comment ) );
	return;
}

void mXmlBase::ParseDocumentType( ElementStackEntry& entry , IXmlReader* reader )
{
	std::unique_ptr< mXmlObject_DocumentType > elm( mNew mXmlObject_DocumentType() );

	//TODO

	entry.LastResult = (OnReadResultEx)OnReadDocumentType( entry.Path , *entry.Ptr , std::move( elm ) );
	return;
}

void mXmlBase::ParseXmlDeclaration( ElementStackEntry& entry , IXmlReader* reader )
{
	std::unique_ptr< mXmlObject_XmlDeclaration_Child > elm( mNew mXmlObject_XmlDeclaration_Child() );

	//アトリビュート
	ParseAttributeResult parse_result;
	switch( entry.LastResult )
	{
	case OnReadResultEx::PendingDeclAttributeTag:
		parse_result = ParseAttributeResult::PendingAttributeTag;
		break;
	case OnReadResultEx::PendingDeclAttributeData:
		parse_result = ParseAttributeResult::PendingAttributeData;
		break;
	default:
		parse_result = ParseAttributeResult::Next;
	}

	switch( ParseAttribute( parse_result , *elm , reader ) )
	{
	case ParseAttributeResult::PendingAttributeTag:
		entry.LastResult = OnReadResultEx::PendingDeclAttributeTag;
		return;
	case ParseAttributeResult::PendingAttributeData:
		entry.LastResult = OnReadResultEx::PendingDeclAttributeData;
		return;
	case ParseAttributeResult::Next:
		break;
	default:
		entry.LastResult = OnReadResultEx::Fail;
		return;
	}

	entry.LastResult = (OnReadResultEx)OnReadXmlDeclaration( entry.Path , *entry.Ptr , std::move( elm ) );
	return;
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

mXmlBase::mXmlObject_WithChildObject::AttrMap mXmlBase::mXmlObject_WithChildObject::GetAttrMap( const WString& prefix )const
{
	AttrMap attrmap;
	for( ChildObjectArray::const_iterator itr = Child.begin() ; itr != Child.end() ; itr++ )
	{
		if( itr->get()->Type == mXmlObjectType::XmlObjectType_Attribute )
		{
			const mXmlObject_Attribute* ptr = reinterpret_cast< const mXmlObject_Attribute* >( itr->get() );
			if( ptr->Prefix == prefix )
			{
				attrmap[ ptr->Name ] = ptr->Value;
			}
		}
	}
	return std::move( attrmap );
}

DWORD mXmlBase::mXmlObject_WithChildObject::GetAttrCount( void )const
{
	DWORD result = 0;
	for( ChildObjectArray::const_iterator itr = Child.begin() ; itr != Child.end() ; itr++ )
	{
		if( itr->get()->Type == mXmlObjectType::XmlObjectType_Attribute )
		{
			result++;
		}
	}
	return result;
}

WString mXmlBase::mXmlObject_WithChildObject::GetText( void )const
{
	for( ChildObjectArray::const_iterator itr = Child.begin() ; itr != Child.end() ; itr++ )
	{
		switch( itr->get()->Type )
		{
		case mXmlObjectType::XmlObjectType_CDATA:
		{
			const mXmlObject_CDATA* ptr = reinterpret_cast< const mXmlObject_CDATA* >( itr->get() );			
			WString result = ptr->Text;
			return std::move( result );
		}
		case mXmlObjectType::XmlObjectType_Text:
		{
			const mXmlObject_Text* ptr = reinterpret_cast< const mXmlObject_Text* >( itr->get() );
			WString result = ptr->Text;
			return std::move( result );
		}
		default:
			break;
		}
	}
	return WString();
}

void mXmlBase::ResetStack( void )
{
	//スタックの全データを消去
	MyElementStack.clear();
	MyStackStatus = OnReadResultEx::Next;
}

