//----------------------------------------------------------------------------
// Mifare Classic UL カードハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSCNTAG.h"
#include <General/mErrorLogger.h>

mSCNTAG::mSCNTAG()
{
}


mSCNTAG::~mSCNTAG()
{
}

bool mSCNTAG::DefaultCommunication( const TransmitData& dt )const
{
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	if( rsp.data.size() != 2 )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカード応答サイズが不正です" );
		return false;
	}
	if( (BYTE)rsp.data[ 0 ] == 0x90 && rsp.data[ 1 ] == 0x00 )
	{
		//成功応答
		return true;
	}
	if( (BYTE)rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
	{
		//失敗応答
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの操作が失敗しました" );
		return false;
	}
	RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
	return false;
}



bool mSCNTAG::ReadSector( DWORD sector , mBinary& retData )const
{
	retData.clear();

	TransmitDataLen dt;
	dt.cla = 0xFFu;
	dt.ins = 0xB0u;
	dt.p1 = 0;
	dt.p2 = (BYTE)( sector * 4 );
	dt.len = 48;

	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//失敗応答
			RaiseError( g_ErrorLogger , 0 , L"スマートカードとの操作が失敗しました" );
			return false;
		}
	}
	if( rsp.data.size() != 50 )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカード応答サイズが不正です" );
		return false;
	}
	if( rsp.data[ 48 ] == 0x90 && rsp.data[ 49 ] == 0x00 )
	{
		//成功応答
		retData = rsp.data.subdata( 0 , 48 );
		return true;
	}
	RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
	return false;

}

bool mSCNTAG::WriteSector( DWORD sector , const mBinary& data )const
{
	TransmitData dt;
	dt.cla = 0xFFu;
	dt.ins = 0xD6u;
	dt.p1 = 0;
	dt.p2 = (BYTE)( sector * 4 );
	dt.data = data.subdata( 0 ,( 48 < data.size() ) ? ( 48 ) : ( data.size() ) );
	while( dt.data.size() < 48 )
	{
		dt.data.push_back( 0 );
	}

	if( !DefaultCommunication( dt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"カードの書き込みが失敗しました" );
		return false;
	}
	return true;
}



bool mSCNTAG::OnConnectCallback( void )
{
	return true;
}
