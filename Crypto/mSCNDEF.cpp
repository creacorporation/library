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

bool mSCNDEF::CreateTlvHeaderType3( int index , uint8_t header_tnf , const AString& header_type , size_t payload_size , mBinary& retheader )const
{
	const TemplateWellknown& entry = reinterpret_cast< const TemplateWellknown& >( *MyTemplates[ index ].get() );

	//Header
	retheader.push_back( 0
		| ( ( index == 0 )                      ? ( 0x80u ) : ( 0x00u ) )   //MB
		| ( ( index == MyTemplates.size() - 1 ) ? ( 0x40u ) : ( 0x00u ) )   //ME
		| ( 0 )	                                                            //CFは対応しない
		| ( ( payload_size < 256 )              ? ( 0x10u ) : ( 0x00u ) )   //SR
		| ( ( !entry.Id.empty() )               ? ( 0x04u ) : ( 0x00u ) )   //IL
		| ( header_tnf )                                                    //TNF
	);
	//Type Length
	if( 255 < header_type.size() )
	{
		RaiseError( g_ErrorLogger , 0 , L"Typeが大きすぎる" );
		return false;
	}
	retheader.push_back( header_type.size() & 0xFFu );
	//Payload Length
	if( payload_size < 256 )
	{
		retheader.push_back( payload_size & 0xFFu );
	}
	else
	{
		retheader.push_back( ( payload_size >> 24 ) & 0xFFu );
		retheader.push_back( ( payload_size >> 16 ) & 0xFFu );
		retheader.push_back( ( payload_size >>  8 ) & 0xFFu );
		retheader.push_back( ( payload_size >>  0 ) & 0xFFu );
	}
	//Id Length
	if( !entry.Id.empty() )
	{
		if( 255 < entry.Id.size() )
		{
			RaiseError( g_ErrorLogger , 0 , L"Idが大きすぎる" );
			return false;
		}
		retheader.push_back( entry.Id.size() & 0xFFu );
	}
	//Type
	for( char c : header_type )
	{
		retheader.push_back( c );
	}
	//Id
	if( !entry.Id.empty() )
	{
		retheader.append( entry.Id );
	}
	return true;
}

bool mSCNDEF::Encode( mBinary& retdata )const
{
	retdata.clear();
	
	auto WriteTlv = [this,&retdata]( uint8_t tag , const mBinary& data )->bool
	{
		//T
		retdata.push_back( tag );
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
		return true;
	};

	mBinary data;
	uint8_t stocked_tag = 0xffu;
	for( int i = 0 ; i < MyTemplates.size() ; i++ )
	{
		const TemplatesEntry& entry = MyTemplates[ i ];

		//データ取得
		uint8_t current_tag;
		mBinary header;
		mBinary payload;

		switch( entry->MyTemplateType )
		{
		case Template::TemplateType::Url:
		{
			current_tag = 3;	//NDEF Message TLV
			if( !EncodeUrlPayload( reinterpret_cast< const TemplateUrl& >( *entry.get() ) , payload ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"URL生成が失敗" );
				return false;
			}
			if( !CreateTlvHeaderType3( i , 1 , "U" , payload.size() , header ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"URLのヘッダー生成が失敗" );
				return false;
			}
			break;
		}
		default:
			RaiseError( g_ErrorLogger , 0 , L"テンプレートの種類が不正" );
			return false;
		}

		//現在保持しているタグと今回のタグが違う場合、ここまでのデータを書き出し
		if( current_tag != stocked_tag )
		{
			if( stocked_tag != 0xFFu && !data.empty() )
			{
				if( !WriteTlv( stocked_tag , data ) )
				{
					return false;
				}
				data.clear();
			}
			stocked_tag = current_tag;
		}

		//今回のタグの内容を書き出し
		data.append( header );
		data.append( payload );
	}

	//最後のタグの書き出し
	if( !data.empty() )
	{
		if( !WriteTlv( stocked_tag , data ) )
		{
			return false;
		}
	}

	//Terminator TLV
	retdata.push_back( 0xFEu );
	return true;
}

