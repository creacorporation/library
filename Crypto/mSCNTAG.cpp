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

bool mSCNTAG::Read( uint8_t start_page , uint8_t end_page , mBinary& retData )const
{
	TransparentSession session( *this );
	return ReadInternal( start_page , end_page , retData , session );
}

bool mSCNTAG::ReadInternal( uint8_t start_page , uint8_t end_page , mBinary& retData , TransparentSession& session )const
{
	mBinary in;
	in.push_back( 0x3Au );		//FastRead
	in.push_back( start_page );
	in.push_back( end_page );

	Sleep( 10 );
	if( !session.Communicate( in , retData ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	return true;
}

bool mSCNTAG::VerifyInternal( uint8_t page , const mBinary& data , TransparentSession& session )const
{
	if( data.size() == 0 )
	{
		return true;
	}
	uint8_t end_page = page + ( ( data.size() + 3 ) / 4 ) - 1;

	mBinary readdata;
	if( !ReadInternal( page , end_page , readdata , session ) )
	{
		return false;
	}
	if( readdata.size() < data.size() )
	{
		RaiseError( g_ErrorLogger , 0 , L"読み取りサイズが不正" );
		return false;
	}
	for( int i = 0 ; i < data.size() ; i++ )
	{
		if( readdata[ i ] != data[ i ] )
		{
			RaiseError( g_ErrorLogger , 0 , L"ベリファイ失敗" );
			return false;
		}
	}
	return true;
}

bool mSCNTAG::Write( uint8_t page , const mBinary& data )const
{
	TransparentSession session( *this );
	return WriteInternal( page , data , session );
}

bool mSCNTAG::Verify( uint8_t page , const mBinary& data )const
{
	TransparentSession session( *this );
	return VerifyInternal( page , data , session );
}

bool mSCNTAG::WriteInternal( uint8_t page , const mBinary& data , TransparentSession& session )const
{
	mBinary in;
	mBinary dummy;

	in.reserve( 6 );
	in.push_back( 0xA2u );	//Write
	in.push_back( 0 );		//access_page
	in.push_back( 0 );		//data[0]
	in.push_back( 0 );		//data[1]
	in.push_back( 0 );		//data[2]
	in.push_back( 0 );		//data[3]

	uint32_t writesize = (uint32_t)data.size();
	uint8_t current_page = page;
	for( uint32_t i = 0 ; i < writesize ; i += 4 )
	{
		in[ 1 ] = current_page;
		if( writesize - i < 4 )
		{
			//残り4バイト未満の場合、端数部分は読み取って上書き
			mBinary tmp;
			Sleep( 10 );
			if( !ReadInternal( current_page , current_page , tmp , session ) || tmp.size() != 4 )
			{
				RaiseError( g_ErrorLogger , 0 , L"最終ページを読み込みできません" );
				return false;
			}
			for( uint32_t j = 0 ; j < 4 ; j++ )
			{
				in[ 2 + j ] = ( j < writesize - i ) ? ( data[ i + j ] ) : ( tmp[ j ] );
			}
		}
		else
		{
			//残り4バイト以上ある場合
			in[ 2 ] = data[ i + 0 ];
			in[ 3 ] = data[ i + 1 ];
			in[ 4 ] = data[ i + 2 ];
			in[ 5 ] = data[ i + 3 ];
		}

		Sleep( 10 );
		if( !session.Communicate( in , dummy ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
			return false;
		}
		current_page++;
	}
	if( !VerifyInternal( page , data , session ) )
	{
		return false;
	}
	return true;
}

bool mSCNTAG::GetUid( int64_t& retId )const
{
	retId = 0;

	mBinary data;
	if( !Read( 0 , 2 , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"UIDの読み取りが失敗しました" );
		return false;
	}
	if( data.size() != 12 )
	{
		RaiseError( g_ErrorLogger , 0 , L"UIDの読み取りサイズが正しくない" );
		return false;
	}
	if( ( ( 0x88u     ^ data[ 0 ] ^ data[ 1 ] ^ data[ 2 ] ) != data[ 3 ] ) ||
		( ( data[ 4 ] ^ data[ 5 ] ^ data[ 6 ] ^ data[ 7 ] ) != data[ 8 ] ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"UIDのチェックバイトが不正" );
		return false;
	}
	retId =
		( (int64_t)data[ 0 ] << 48 ) +
		( (int64_t)data[ 1 ] << 40 ) +
		( (int64_t)data[ 2 ] << 32 ) +
		( (int64_t)data[ 4 ] << 24 ) +
		( (int64_t)data[ 5 ] << 16 ) +
		( (int64_t)data[ 6 ] <<  8 ) +
		( (int64_t)data[ 7 ] <<  0 );
	return true;
}

bool mSCNTAG::GetUid( AString& retId )const
{
	int64_t uid;
	if( !GetUid( uid ) )
	{
		return false;
	}
	sprintf( retId , "%llx" , (uint64_t)uid );
	return true;
}

bool mSCNTAG::GetUid( WString& retId )const
{
	int64_t uid;
	if( !GetUid( uid ) )
	{
		return false;
	}
	sprintf( retId , L"%llx" , (uint64_t)uid );
	return true;
}

int64_t mSCNTAG::GetUidValue( void )const
{
	int64_t uid = 0;
	if( !GetUid( uid ) )
	{
		return -1;
	}
	return uid;
}

AString mSCNTAG::GetUidAString( void )const
{
	AString str;
	if( !GetUid( str ) )
	{
		return "";
	}
	return str;
}

WString mSCNTAG::GetUidWString( void )const
{
	WString str;
	if( !GetUid( str ) )
	{
		return L"";
	}
	return str;
}

int32_t mSCNTAG::GetReadCount( void )const
{
	TransparentSession session( *this );

	mBinary in;
	in.push_back( 0x39u );		//ReadCount
	in.push_back( 0x02u );

	mBinary out;
	if( !session.Communicate( in , out ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return -1;
	}
	if( out.size() != 3 )
	{
		RaiseError( g_ErrorLogger , 0 , L"カウント値のサイズが不正" );
		return -1;
	}
	return ( out[ 2 ] << 16 ) + ( out[ 1 ] << 8 ) + ( out[ 0 ] );

}

bool mSCNTAG::Auth( uint32_t password )const
{
	TransparentSession session( *this );

	uint16_t pack = GetPACK( session );

	mBinary cmd;
	cmd.push_back( 0x1Bu );		//Password Authentication
	cmd.push_back( ( password >> 24 ) & 0xFFu );
	cmd.push_back( ( password >> 16 ) & 0xFFu );
	cmd.push_back( ( password >>  8 ) & 0xFFu );
	cmd.push_back( ( password >>  0 ) & 0xFFu );

	mBinary rsp;
	if( !session.Communicate( cmd , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	if( rsp.size() != 2 )
	{
		RaiseError( g_ErrorLogger , 0 , L"認証が失敗しました" );
		return false;
	}
	if( ( ( rsp[ 0 ] << 8 ) + ( rsp[ 0 ] ) ) != pack )
	{
		RaiseError( g_ErrorLogger , 0 , L"PACKの値が不正です" );
		return false;
	}
	return true;
}

mSCNTAG::PartNum mSCNTAG::GetPartNum( void )const
{
	TransparentSession session( *this );
	return GetPartNum( session );
}

mSCNTAG::PartNum mSCNTAG::GetPartNum( TransparentSession& session )const
{
	uint32_t cc = GetCC( session );
	if( ( cc & 0xFF000000u ) != 0xE1000000u )
	{
		RaiseError( g_ErrorLogger , 0 , L"CCのマジックナンバーが正しくない" );
		return PartNum::Unknown;
	}

	switch( ( cc >> 8 ) & 0xFFu )
	{
	case 0x12u:
		return PartNum::NTAG213;
	case 0x3Eu:
		return PartNum::NTAG215;
	case 0x6Du:
		return PartNum::NTAG216;
	default:
		break;
	}
	return PartNum::Unknown;
}

//PACKの値を取得する
uint16_t mSCNTAG::GetPACK( TransparentSession& session )const
{
	uint8_t packaddr;
	switch( GetPartNum( session ) )
	{
	case PartNum::NTAG213:
		packaddr = 0x2Cu;
		break;
	case PartNum::NTAG215:
		packaddr = 0x86u;
		break;
	case PartNum::NTAG216:
		packaddr = 0xE6u;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"PACKのアドレスを判断できない" );
		return 0;
	}

	mBinary pack;
	if( !ReadInternal( packaddr , packaddr , pack , session ) )
	{
		return 0;
	}
	if( pack.size() != 4 )
	{
		RaiseError( g_ErrorLogger , 0 , L"PACKのサイズが不正" );
	}
	return ( pack[ 0 ] << 8 ) + ( pack[ 1 ] << 0 );
}

//CCの値を取得する
uint32_t mSCNTAG::GetCC( TransparentSession& session )const
{
	mBinary cc;
	if( !ReadInternal( 3 , 3 , cc , session ) )
	{
		return 0;
	}
	if( cc.size() != 4 )
	{
		RaiseError( g_ErrorLogger , 0 , L"CCのサイズが不正" );
		return 0;
	}

	return ( cc[ 0 ] << 24 ) + ( cc[ 1 ] << 16 ) + ( cc[ 2 ] << 8 ) + ( cc[ 3 ] << 0 );
}

bool mSCNTAG::ReadSig( mBinary& retdata )const
{
	TransparentSession session( *this );

	mBinary in;
	in.push_back( 0x3Cu );		//ReadSig
	in.push_back( 0x00u );

	if( !session.Communicate( in , retdata ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	if( retdata.size() != 32 )
	{
		RaiseError( g_ErrorLogger , 0 , L"カウント値のサイズが不正" );
		return false;
	}
	return true;
}

bool mSCNTAG::GetVersion( mBinary& retdata )const
{
	TransparentSession session( *this );

	mBinary in;
	in.push_back( 0x60u );		//GetVersion

	if( !session.Communicate( in , retdata ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	if( retdata.size() != 8 )
	{
		RaiseError( g_ErrorLogger , 0 , L"カウント値のサイズが不正" );
		return false;
	}
	return true;
}

bool mSCNTAG::SetAccessSetting( const AccessSetting& setting )const
{
	TransparentSession session( *this );

	//ACCESSのページ番号取得
	uint8_t access_page;
	uint8_t auth0_page;
	switch( GetPartNum( session ) )
	{
	case PartNum::NTAG213:
		auth0_page = 0x29u;
		access_page = 0x2Au;
		break;
	case PartNum::NTAG215:
		auth0_page = 0x83u;
		access_page = 0x84u;
		break;
	case PartNum::NTAG216:
		auth0_page = 0xE3u;
		access_page = 0xE4u;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"チップの種類が不明" );
		return false;
	}
	
	mBinary data;
	data.resize( 4 );

	//ACCESS
	data[ 0 ] = 
		( ( setting.ReadProtect ) ? ( 0x80u ) : ( 0 ) ) |
		( ( setting.ConfigLock ) ? ( 0x40u ) : ( 0 ) ) |
		( ( setting.EnableCounter ) ? ( 0x10u ) : ( 0 ) ) |
		( ( setting.CounterProtect ) ? ( 0x08u ) : ( 0 ) ) |
		( ( 7 < setting.AuthLimit ) ? ( 7 ) : ( setting.AuthLimit ) );
	data[ 1 ] = 0;
	data[ 2 ] = 0;
	data[ 3 ] = 0;
	if( !Write( access_page , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ACCESSの書込みが失敗しました" );
		return false;
	}

	//PWD
	data[ 0 ] = ( setting.Password >> 24 ) & 0xFFu;
	data[ 1 ] = ( setting.Password >> 16 ) & 0xFFu;
	data[ 2 ] = ( setting.Password >>  8 ) & 0xFFu;
	data[ 3 ] = ( setting.Password >>  0 ) & 0xFFu;
	if( !Write( access_page + 1 , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"PWDの書込みが失敗しました" );
		return false;
	}

	//PACK
	data[ 0 ] = ( setting.Pack << 8 ) & 0xFFu;;
	data[ 1 ] = ( setting.Pack << 0 ) & 0xFFu;;
	data[ 2 ] = 0;
	data[ 3 ] = 0;
	if( !Write( access_page + 2 , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"PACKの書込みが失敗しました" );
		return false;
	}

	//Auth0
	if( !Read( auth0_page , auth0_page , data ) || ( data.size() != 4 ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"Auth0の読み込みが失敗しました" );
		return false;
	}
	if( ( setting.ConfigLock ) && ( access_page < setting.Auth0 ) )
	{
		data[ 3 ] = access_page + 1;
	}
	else
	{
		data[ 3 ] = setting.Auth0;
	}
	if( !Write( auth0_page , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"Auth0の書込みが失敗しました" );
		return false;
	}

	//Capability Container
	if( setting.NoWriteAccess )
	{
		data.resize( 4 );
		data[ 0 ] = 0;
		data[ 1 ] = 0;
		data[ 2 ] = 0;
		data[ 3 ] = 0x0Fu;
		if( !Write( 3 , data ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"CCの書込みが失敗しました" );
			return false;
		}
	}
	return true;
}

mSCNTAG::StaticLock::StaticLock()
{
	CC = StaticLockStatus::Unlocked;
	for( int i = 0 ; i < (int)StaticLock::PageIndex::PageIndexMax ; i++ )
	{
		Page[ i ] = StaticLockStatus::Unlocked;
	}
}


bool mSCNTAG::SetStaticLock( const StaticLock& setting )const
{
	uint16_t pagelock = 0;
	uint16_t settinglock = 0;

	auto UpdatePageLock = [&pagelock,&settinglock]( StaticLockStatus status , uint16_t mask )->void
	{
		pagelock >>= 1;
		switch( status )
		{
		case StaticLockStatus::Unlocked:
			break;
		case StaticLockStatus::ReadOnly:
			pagelock |= 0x8000u;
			break;
		case StaticLockStatus::ReadWrite:
			settinglock |= mask;
			break;
		}
		return;
	};

	UpdatePageLock( setting.CC , 0x0001u );
	for( int i = (int)StaticLock::PageIndex::Page4 ; i <= (int)StaticLock::PageIndex::Page9 ; i++ )
	{
		UpdatePageLock( setting.Page[ i ] , 0x0002u );
	}
	for( int i = (int)StaticLock::PageIndex::Page10 ; i <= (int)StaticLock::PageIndex::Page15 ; i++ )
	{
		UpdatePageLock( setting.Page[ i ] , 0x0004u );
	}

	mBinary data;
	data.push_back( 0 );
	data.push_back( 0 );
	data.push_back( ( pagelock | settinglock ) & 0xFFu );
	data.push_back( ( pagelock >> 8          ) & 0xFFu );

	if( !Write( 2 , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"StaticLockByteの書込みが失敗しました" );
		return false;
	}
	return true;
}

bool mSCNTAG::SetDynamicLock( uint32_t setting )const
{
	TransparentSession session( *this );

	uint8_t addr;
	switch( GetPartNum( session ) )
	{
	case PartNum::NTAG213:
		addr = 0x28u;
		break;
	case PartNum::NTAG215:
		addr = 0x82u;
		break;
	case PartNum::NTAG216:
		addr = 0xE2u;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"DynamicLockByteのアドレスを判断できない" );
		return 0;
	}

	mBinary data;
	data.push_back( ( setting >> 24 ) & 0xFFu );
	data.push_back( ( setting >> 16 ) & 0xFFu );
	data.push_back( ( setting >>  8 ) & 0xFFu );
	data.push_back( ( setting >>  0 ) & 0xFFu );

	if( !Write( addr , data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"DynamicLockByteの書込みが失敗しました" );
		return false;
	}
	return true;

}


bool mSCNTAG::OnConnectCallback( void )
{
	return true;
}

uint16_t mSCNTAG::CalcCrc( const mBinary& data )const
{
	// CRC16/CCITT
	// Polynomial = 0x1021(ビット順が逆の場合0x8408)
	// Initial = 0x6363(ビット順が逆の場合0xc6c6)
	// 最後の排他的論理和なし

	//計算例：以下9バイトのCRC→0xBF05
	// 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39
		
	static const uint16_t Crc16Table[ 256 ] =
	{
		0x0000u,0x1189u,0x2312u,0x329Bu,0x4624u,0x57ADu,0x6536u,0x74BFu,0x8C48u,0x9DC1u,0xAF5Au,0xBED3u,0xCA6Cu,0xDBE5u,0xE97Eu,0xF8F7u,
		0x1081u,0x0108u,0x3393u,0x221Au,0x56A5u,0x472Cu,0x75B7u,0x643Eu,0x9CC9u,0x8D40u,0xBFDBu,0xAE52u,0xDAEDu,0xCB64u,0xF9FFu,0xE876u,
		0x2102u,0x308Bu,0x0210u,0x1399u,0x6726u,0x76AFu,0x4434u,0x55BDu,0xAD4Au,0xBCC3u,0x8E58u,0x9FD1u,0xEB6Eu,0xFAE7u,0xC87Cu,0xD9F5u,
		0x3183u,0x200Au,0x1291u,0x0318u,0x77A7u,0x662Eu,0x54B5u,0x453Cu,0xBDCBu,0xAC42u,0x9ED9u,0x8F50u,0xFBEFu,0xEA66u,0xD8FDu,0xC974u,
		0x4204u,0x538Du,0x6116u,0x709Fu,0x0420u,0x15A9u,0x2732u,0x36BBu,0xCE4Cu,0xDFC5u,0xED5Eu,0xFCD7u,0x8868u,0x99E1u,0xAB7Au,0xBAF3u,
		0x5285u,0x430Cu,0x7197u,0x601Eu,0x14A1u,0x0528u,0x37B3u,0x263Au,0xDECDu,0xCF44u,0xFDDFu,0xEC56u,0x98E9u,0x8960u,0xBBFBu,0xAA72u,
		0x6306u,0x728Fu,0x4014u,0x519Du,0x2522u,0x34ABu,0x0630u,0x17B9u,0xEF4Eu,0xFEC7u,0xCC5Cu,0xDDD5u,0xA96Au,0xB8E3u,0x8A78u,0x9BF1u,
		0x7387u,0x620Eu,0x5095u,0x411Cu,0x35A3u,0x242Au,0x16B1u,0x0738u,0xFFCFu,0xEE46u,0xDCDDu,0xCD54u,0xB9EBu,0xA862u,0x9AF9u,0x8B70u,
		0x8408u,0x9581u,0xA71Au,0xB693u,0xC22Cu,0xD3A5u,0xE13Eu,0xF0B7u,0x0840u,0x19C9u,0x2B52u,0x3ADBu,0x4E64u,0x5FEDu,0x6D76u,0x7CFFu,
		0x9489u,0x8500u,0xB79Bu,0xA612u,0xD2ADu,0xC324u,0xF1BFu,0xE036u,0x18C1u,0x0948u,0x3BD3u,0x2A5Au,0x5EE5u,0x4F6Cu,0x7DF7u,0x6C7Eu,
		0xA50Au,0xB483u,0x8618u,0x9791u,0xE32Eu,0xF2A7u,0xC03Cu,0xD1B5u,0x2942u,0x38CBu,0x0A50u,0x1BD9u,0x6F66u,0x7EEFu,0x4C74u,0x5DFDu,
		0xB58Bu,0xA402u,0x9699u,0x8710u,0xF3AFu,0xE226u,0xD0BDu,0xC134u,0x39C3u,0x284Au,0x1AD1u,0x0B58u,0x7FE7u,0x6E6Eu,0x5CF5u,0x4D7Cu,
		0xC60Cu,0xD785u,0xE51Eu,0xF497u,0x8028u,0x91A1u,0xA33Au,0xB2B3u,0x4A44u,0x5BCDu,0x6956u,0x78DFu,0x0C60u,0x1DE9u,0x2F72u,0x3EFBu,
		0xD68Du,0xC704u,0xF59Fu,0xE416u,0x90A9u,0x8120u,0xB3BBu,0xA232u,0x5AC5u,0x4B4Cu,0x79D7u,0x685Eu,0x1CE1u,0x0D68u,0x3FF3u,0x2E7Au,
		0xE70Eu,0xF687u,0xC41Cu,0xD595u,0xA12Au,0xB0A3u,0x8238u,0x93B1u,0x6B46u,0x7ACFu,0x4854u,0x59DDu,0x2D62u,0x3CEBu,0x0E70u,0x1FF9u,
		0xF78Fu,0xE606u,0xD49Du,0xC514u,0xB1ABu,0xA022u,0x92B9u,0x8330u,0x7BC7u,0x6A4Eu,0x58D5u,0x495Cu,0x3DE3u,0x2C6Au,0x1EF1u,0x0F78u,
	};

	//※テーブル生成方法
	// https://hub.zhovner.com/tools/nfc/
	// https://www.sunshine2k.de/coding/javascript/crc/crc_js.html
	//	for( uint16_t i = 0 ; i < 256 ; i++ )
	//	{
	//		uint16_t c = i;
	//		for( int j = 0 ; j < 8 ; j++ )
	//		{
	//			c = ( c >> 1 ) ^ ( ( c & 0x0001u ) ? 0x8408 : 0 );
	//		}
	//		//※非反転Ver
	//		//	uint16_t c = i << 8;
	//		//	for( int j = 0 ; j < 8 ; j++ )
	//		//	{
	//		//		c = ( c << 1 ) ^ ( ( c & 0x8000u ) ? 0x1021 : 0 );
	//		//	}
	//		Crc16Table[ i ] = c;
	//	}

	uint16_t crc = 0x6363u;
	for( mBinary::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		uint8_t v = *itr;
		crc = ( crc >> 8 ) ^ Crc16Table[ ( ( crc      ) ^ *itr ) & 0xFFu ];
	}
	return crc;
}

