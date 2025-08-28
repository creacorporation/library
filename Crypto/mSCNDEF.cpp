//----------------------------------------------------------------------------
// NDEFエンコード
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSCNDEF.h"
#include <General/mErrorLogger.h>

mSCNDEF::mSCNDEF()
{
}

mSCNDEF::~mSCNDEF()
{
}

bool mSCNDEF::AddTemplate( const Template& t )
{
	std::unique_ptr< Template > entry;
	switch( t.MyTemplateType )
	{
	case Template::TemplateType::Url:
		entry.reset( mNew TemplateUrl( reinterpret_cast< const TemplateUrl& >( t ) ) );
		break;
	default:
		return false;
	}

	MyTemplates.push_back( std::move( entry ) );
	return true;
}

bool mSCNDEF::EncodeUrlPayload( const TemplateUrl& t , mBinary& retpayload )const
{
	retpayload.clear();

	if( t.Url.find( "http://www." ) == 0 )
	{
		retpayload.push_back( 1 );
		appendf( retpayload , "%s" , &t.Url[ 11 ] );
	}
	else if( t.Url.find( "https://www." ) == 0 )
	{
		retpayload.push_back( 2 );
		appendf( retpayload , "%s" , &t.Url[ 12 ] );
	}
	else if( t.Url.find( "http://" ) == 0 )
	{
		retpayload.push_back( 3 );
		appendf( retpayload , "%s" , &t.Url[ 7 ] );
	}
	else if( t.Url.find( "https://" ) == 0 )
	{
		retpayload.push_back( 4 );
		appendf( retpayload , "%s" , &t.Url[ 8 ] );
	}
	else
	{
		if( t.Url.size() == 0 )
		{
			return false;
		}
		retpayload.push_back( 0 );
		appendf( retpayload , "%s" , &t.Url[ 0 ] );
	}

	retpayload.pop_back();	//末尾のヌル文字はNDEFでは不要
	return true;
}

bool mSCNDEF::Encode( mBinary& retdata )const
{
	retdata.clear();
	mBinary data;

	for( int i = 0 ; i < MyTemplates.size() ; i++ )
	{
		const TemplatesEntry& entry = MyTemplates[ i ];

		//データ取得
		uint8_t header_tnf;
		mBinary header_type;
		mBinary payload;
		switch( entry->MyTemplateType )
		{
		case Template::TemplateType::Url:
			header_tnf = 1;
			header_type.push_back( 'U' );
			if( !EncodeUrlPayload( reinterpret_cast< const TemplateUrl& >( *entry.get() ) , payload ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"URL生成が失敗" );
				return false;
			}
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"テンプレートの種類が不正" );
			return false;
		}

		//Header
		data.push_back( 0
			| ( ( i == 0 )                      ? ( 0x80u ) : ( 0x00u ) )   //MB
			| ( ( i == MyTemplates.size() - 1 ) ? ( 0x40u ) : ( 0x00u ) )   //ME
			| ( 0 )	                                                        //CFは対応しない
			| ( ( payload.size() < 256 )        ? ( 0x10u ) : ( 0x00u ) )   //SR
			| ( ( !entry.get()->Id.empty() )    ? ( 0x04u ) : ( 0x00u ) )   //IL
			| ( header_tnf )                                                //TNF
		);
		//Type Length
		if( 255 < header_type.size() )
		{
			RaiseError( g_ErrorLogger , 0 , L"Typeが大きすぎる" );
			return false;
		}
		data.push_back( header_type.size() & 0xFFu );
		//Payload Length
		if( payload.size() < 256 )
		{
			data.push_back( payload.size() & 0xFFu );
		}
		else
		{
			data.push_back( ( payload.size() >> 24 ) & 0xFFu );
			data.push_back( ( payload.size() >> 16 ) & 0xFFu );
			data.push_back( ( payload.size() >>  8 ) & 0xFFu );
			data.push_back( ( payload.size() >>  0 ) & 0xFFu );
		}
		//Id Length
		if( !entry.get()->Id.empty() )
		{
			if( 255 < entry.get()->Id.size() )
			{
				RaiseError( g_ErrorLogger , 0 , L"Idが大きすぎる" );
				return false;
			}
			data.push_back( entry.get()->Id.size() & 0xFFu );
		}
		//Type
		data.append( header_type );
		//Id
		if( !entry.get()->Id.empty() )
		{
			data.append( entry.get()->Id );
		}
		//Payload
		data.append( payload );
	}

	//T
	retdata.push_back( 0x03u );	//NDEF Message TLV
	//L
	if( 65535 < data.size() )
	{
		RaiseError( g_ErrorLogger , 0 , L"ペイロードが大きすぎる" );
		return false;
	}
	else if( 255 <= data.size() )
	{
		retdata.push_back( 0xFFu );
		retdata.push_back( ( data.size() >> 8 ) & 0xFFu );
		retdata.push_back( ( data.size() >> 0 ) & 0xFFu );
	}
	else
	{
		retdata.push_back( data.size() & 0xFFu );
	}
	//V
	retdata.append( data );

	//T
	retdata.push_back( 0xFEu );	//Terminator TLV
	return true;
}

