//----------------------------------------------------------------------------
// Mifare Classic 1K カードハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSCMifare.h"
#include <General/mErrorLogger.h>

mSCMifare::mSCMifare()
{
}


mSCMifare::~mSCMifare()
{
}

bool mSCMifare::DefaultCommunication( const TransmitData& dt )const
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


bool mSCMifare::LoadAuthenticationKeys( Key key )const
{
	TransmitData dt;
	dt.cla = 0xFFu;
	dt.ins = 0x82u;
	dt.p1 = 0;
	dt.p2 = 0;
	dt.SetData( key.data() , (DWORD)key.size() );

	if( !DefaultCommunication( dt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"認証キーのダウンロードが失敗しました" );
		return false;
	}
	return true;
}

bool mSCMifare::ExecAuthentication( DWORD sector , bool key_a )const
{
	TransmitData dt;
	dt.cla = 0xFFu;
	dt.ins = 0x86u;
	dt.p1 = 0;
	dt.p2 = 0;
	dt.data.push_back( 0x01u );
	dt.data.push_back( 0x00u );
	dt.data.push_back( (BYTE)( sector * 4 ) );
	dt.data.push_back( ( key_a ) ? ( 0x60u ) : ( 0x61u ) );
	dt.data.push_back( 0x00u );

	if( !DefaultCommunication( dt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"カードの認証が失敗しました" );
		return false;
	}
	return true;
}

bool mSCMifare::ReadSector( DWORD sector , mBinary& retData )const
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

static bool IsValidAccessBits( BYTE byte6 , BYTE byte7 , BYTE byte8 )
{
	//C1
	if( ( ( ( byte6 >> 0 ) ^ ( byte7 >> 4 ) ) & 0x0Fu ) != 0x0Fu )
	{
		return false;
	}
	//C2
	if( ( ( ( byte8 >> 0 ) ^ ( byte6 >> 4 ) ) & 0x0Fu ) != 0x0Fu )
	{
		return false;
	}
	//C3
	if( ( ( ( byte7 >> 0 ) ^ ( byte8 >> 4 ) ) & 0x0Fu ) != 0x0Fu )
	{
		return false;
	}
	return true;
}

static mSCMifare::Permission GetMifarePermission( bool c1 , bool c2 , bool c3 )
{
	DWORD index = 0;
	index += ( c1 ) ? ( 2 ) : ( 0 );
	index += ( c2 ) ? ( 1 ) : ( 0 );
	index += ( c3 ) ? ( 4 ) : ( 0 );

	switch( index )
	{
	case 0:
		return mSCMifare::Permission::Permission0;
	case 1:
		return mSCMifare::Permission::Permission1;
	case 2:
		return mSCMifare::Permission::Permission2;
	case 3:
		return mSCMifare::Permission::Permission3;
	case 4:
		return mSCMifare::Permission::Permission4;
	case 5:
		return mSCMifare::Permission::Permission5;
	case 6:
		return mSCMifare::Permission::Permission6;
	case 7:
		return mSCMifare::Permission::Permission7;
	default:
		RaiseAssert( g_ErrorLogger , index , L"パーミッション設定が不正です" );
		return mSCMifare::Permission::Permission0;
	}
}

static uint8_t MakeMifarePermission( mSCMifare::Permission permission )
{
	switch( permission )
	{
	case mSCMifare::Permission::Permission0:
		//             CCC
		//             123
		return 0b0000'0000;
	case mSCMifare::Permission::Permission1:
		return 0b0000'0010;
	case mSCMifare::Permission::Permission2:
		return 0b0000'0100;
	case mSCMifare::Permission::Permission3:
		return 0b0000'0110;
	case mSCMifare::Permission::Permission4:
		return 0b0000'0001;
	case mSCMifare::Permission::Permission5:
		return 0b0000'0011;
	case mSCMifare::Permission::Permission6:
		return 0b0000'0101;
	case mSCMifare::Permission::Permission7:
		return 0b0000'0111;
	default:
		RaiseError( g_ErrorLogger , permission , L"パーミッション設定が不正です" );
		return 0;
	}
}

bool mSCMifare::ReadTrailer( DWORD sector , Trailer& retTrailer )const
{
	TransmitDataLen dt;
	dt.cla = 0xFFu;
	dt.ins = 0xB0u;
	dt.p1 = 0;
	dt.p2 = (BYTE)( sector * 4 ) + 3;
	dt.len = 16;

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
	if( rsp.data.size() != 18 )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカード応答サイズが不正です" );
		return false;
	}
	if( (BYTE)rsp.data[ 16 ] == 0x90 && rsp.data[ 17 ] == 0x00 )
	{
		//成功応答なので、アクセスビットの検証・設定を行う
		BYTE byte6 = rsp.data[ 6 ];
		BYTE byte7 = rsp.data[ 7 ];
		BYTE byte8 = rsp.data[ 8 ];
		if( !IsValidAccessBits( byte6 , byte7 , byte8 ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"アクセスビットが正しくありません" );
			return false;
		}
		//enumへの変換
		BYTE c1 = ( byte7 >> 4 ) & 0x0Fu;
		BYTE c2 = ( byte8 >> 0 ) & 0x0Fu;
		BYTE c3 = ( byte8 >> 4 ) & 0x0Fu;
		retTrailer.permission0        = GetMifarePermission( ( c1 & 0x01 ) , ( c2 & 0x01 ) , ( c3 & 0x01 ) );
		retTrailer.permission1        = GetMifarePermission( ( c1 & 0x02 ) , ( c2 & 0x02 ) , ( c3 & 0x02 ) );
		retTrailer.permission2        = GetMifarePermission( ( c1 & 0x04 ) , ( c2 & 0x04 ) , ( c3 & 0x04 ) );
		retTrailer.permission_trailer = GetMifarePermission( ( c1 & 0x08 ) , ( c2 & 0x08 ) , ( c3 & 0x08 ) );
		//キーのセット
		for( DWORD i = 0 ; i < 6 ; i++ )
		{
			retTrailer.key_a[ i ] = (BYTE)rsp.data[ i +  0 ];
			retTrailer.key_b[ i ] = (BYTE)rsp.data[ i + 10 ];
		}
		//ユーザーデータのセット
		retTrailer.userdata = (BYTE)rsp.data[ 9 ];
		return true;
	}
	RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
	return false;

}

bool mSCMifare::WriteSector( DWORD sector , const mBinary& data )const
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

bool mSCMifare::WriteTrailer( DWORD sector , const Trailer& trailer )const
{
	TransmitData dt;
	dt.cla = 0xFFu;
	dt.ins = 0xD6u;
	dt.p1 = 0;
	dt.p2 = (BYTE)( sector * 4 ) + 3;

	dt.data.resize( 16 );
	for( DWORD i = 0 ; i < 6 ; i++ )
	{
		dt.data[ i +  0 ] = trailer.key_a[ i ];
		dt.data[ i + 10 ] = trailer.key_b[ i ];
	}

	uint8_t ac_block0 = MakeMifarePermission( trailer.permission0 );
	uint8_t ac_block1 = MakeMifarePermission( trailer.permission1 );
	uint8_t ac_block2 = MakeMifarePermission( trailer.permission2 );
	uint8_t ac_block3 = MakeMifarePermission( trailer.permission_trailer );

	uint8_t byte6 = 0;
	uint8_t byte7 = 0;
	uint8_t byte8 = 0;

	byte7 |= ( ac_block3 & 0b0000'0100 ) ? ( 0b1000'0000 ) : ( 0 );
	byte7 |= ( ac_block2 & 0b0000'0100 ) ? ( 0b0100'0000 ) : ( 0 );
	byte7 |= ( ac_block1 & 0b0000'0100 ) ? ( 0b0010'0000 ) : ( 0 );
	byte7 |= ( ac_block0 & 0b0000'0100 ) ? ( 0b0001'0000 ) : ( 0 );

	byte8 |= ( ac_block3 & 0b0000'0001 ) ? ( 0b1000'0000 ) : ( 0 );
	byte8 |= ( ac_block2 & 0b0000'0001 ) ? ( 0b0100'0000 ) : ( 0 );
	byte8 |= ( ac_block1 & 0b0000'0001 ) ? ( 0b0010'0000 ) : ( 0 );
	byte8 |= ( ac_block0 & 0b0000'0001 ) ? ( 0b0001'0000 ) : ( 0 );

	byte8 |= ( ac_block3 & 0b0000'0010 ) ? ( 0b0000'1000 ) : ( 0 );
	byte8 |= ( ac_block2 & 0b0000'0010 ) ? ( 0b0000'0100 ) : ( 0 );
	byte8 |= ( ac_block1 & 0b0000'0010 ) ? ( 0b0000'0010 ) : ( 0 );
	byte8 |= ( ac_block0 & 0b0000'0010 ) ? ( 0b0000'0001 ) : ( 0 );

	byte7 |= ( ~( byte8 >> 4 ) ) & 0x0Fu;
	byte6 |= ( ~( byte7 >> 4 ) ) & 0x0Fu;
	byte6 |= ( ~( byte8 << 4 ) ) & 0xF0u;

	dt.data[ 6 ] = byte6;
	dt.data[ 7 ] = byte7;
	dt.data[ 8 ] = byte8;
	dt.data[ 9 ] = trailer.userdata;

	if( !DefaultCommunication( dt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"カードの書き込みが失敗しました" );
		return false;
	}
	return true;
}

bool mSCMifare::OnConnectCallback( void )
{
	return true;
}
