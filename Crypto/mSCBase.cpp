//----------------------------------------------------------------------------
// スマートカードハンドラベース
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------


#include "mSCBase.h"
#include <General/mErrorLogger.h>

mSCBase::mSCBase()
{
	MySCardContext = 0;
	MySCard = 0;
	MyActiveProtocol = Protocol::Unknwon;
	MyReaderMaker = ReaderMaker::READER_MAKER_GENERAL;
}

mSCBase::~mSCBase()
{
	if( MySCard )
	{
		SCardDisconnect( MySCard , SCARD_LEAVE_CARD );
		MySCard = 0;
	}

	MyCardID.clear();
	MyActiveProtocol = Protocol::Unknwon;

	if( MySCardContext )
	{
		SCardReleaseContext( MySCardContext );
		MySCardContext = 0;
	}
}


bool mSCBase::Connect( const WString& reader , ReaderMaker maker )
{
	LONG result;

	//初期化済みチェック
	if( MySCard )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"オブジェクトは初期化済みです" );
		return false;
	}

	//コンテキストがなければ取得
	if( !MySCardContext )
	{
		result = SCardEstablishContext( SCARD_SCOPE_USER , nullptr , nullptr , &MySCardContext );
		switch( result )
		{
		case SCARD_S_SUCCESS:
			break;
		case SCARD_E_NO_SERVICE:
			RaiseError( g_ErrorLogger , result , L"スマートカードサービスが起動していません" );
			return false;
		default:
			RaiseError( g_ErrorLogger , result , L"スマートカードの初期化が失敗しました" );
			return false;
		}
	}

	//スマートカードへの接続
	DWORD proto = 0;
	result = SCardConnectW( MySCardContext , reader.c_str() , SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1 , &MySCard , &proto );
	switch( result )
	{
	case SCARD_S_SUCCESS:
		break;
	default:
		RaiseError( g_ErrorLogger , result , L"カードとの接続が失敗しました" );
		return false;
	}

	//プロトコルは？
	switch( proto )
	{
	case SCARD_PROTOCOL_T0:
		MyActiveProtocol = Protocol::T0;
		break;
	case SCARD_PROTOCOL_T1:
		MyActiveProtocol = Protocol::T1;
		break;
	default:
		RaiseError( g_ErrorLogger , result , L"通信プロトコルが不明です" );
		MyActiveProtocol = Protocol::Unknwon;
		break;
	}

	//カードIDを取得する
	{
		TransmitDataLen dt;
		dt.cla = 0xFFu;
		dt.ins = 0xCAu;
		dt.p1 = 0;
		dt.p2 = 0;
		dt.len = 0;

		ResponseData res;
		if( !Communicate( dt , res ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"カードIDが取得できません" );
			return false;
		}
		if( ( res.data.size() == 2 ) &&
			( res.data[ 0 ] == 0x63 && res.data[ 1 ] == 0x00 ) )
		{
			//失敗応答
			RaiseError( g_ErrorLogger , 0 , L"スマートカードとの操作が失敗しました" );
			return false;
		}
		else if( ( 6 <= res.data.size() ) &&
			( res.data[ res.data.size() - 2 ] == 0x90 && res.data[ res.data.size() - 1 ] == 0x00 ) )
		{
			//成功応答
			MyCardID = res.data.subdata( 0 , res.data.size() - 2 );
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
			return false;
		}
	}

	//リーダーのメーカーセット
	if( maker == ReaderMaker::READER_MAKER_GENERAL )
	{
		WString name = ToLower( reader );
		if( name.find( L"sony" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_SONY;	//ソニーの何かっぽい
		}
		else if( name.find( L"pasori" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_SONY;	//パソリっぽい
		}
		else if( name.find( L"acs" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_ACS;	//ACSっぽい
		}
		else if( name.find( L"acr" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_ACS;	//ACRシリーズっぽい
		}
		else
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_GENERAL;	//わかんね
		}
	}
	else
	{
		MyReaderMaker = maker;
	}

	//カード個別の処理を実行
	if( !OnConnectCallback() )
	{
		RaiseError( g_ErrorLogger , 0 , L"内部初期化処理が失敗しました" );
		MyCardID.clear();
		return false;
	}
	return true;
}

bool mSCBase::Communicate( const TransmitData& dt , ResponseData& retResponse )const
{
	retResponse.data.clear();

	//データのチェック
	if( 255 < dt.data.size() )
	{
		//追加データが長すぎる
		RaiseError( g_ErrorLogger , dt.data.size() , L"追加データが長すぎます" );
		return false;
	}

	//送信データの作成
	mSecureBinary senddata;
	LPCSCARD_IO_REQUEST protocol;
	senddata.push_back( dt.cla );
	senddata.push_back( dt.ins );
	senddata.push_back( dt.p1 );
	senddata.push_back( dt.p2 );
	switch( MyActiveProtocol )
	{
	case Protocol::T0:
		if( dt.data.size() )
		{
			senddata.push_back( (BYTE)dt.data.size() );
			senddata.append( dt.data );
		}
		protocol = SCARD_PCI_T0;
		break;
	case Protocol::T1:
		if( dt.data.size() == 0 )
		{
			senddata.push_back( 0 );
		}
		else
		{
			senddata.push_back( (BYTE)dt.data.size() );
			senddata.append( dt.data );
		}
		protocol = SCARD_PCI_T1;
		break;
	default:
		RaiseError( g_ErrorLogger , MyActiveProtocol , L"プロトコルが不明です" );
		return false;
	}

	//送信処理
	BYTE response[ 256 ];	//結果格納バッファ
	DWORD response_size = sizeof( response );
	LONG result = SCardTransmit( MySCard , protocol , senddata.data() , (DWORD)senddata.size() , nullptr , response , &response_size );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , result , L"スマートカードとの通信が失敗しました" );
		SecureZeroMemory( response , sizeof( response ) );
		return false;
	}

	//結果のパース
	retResponse.data.reserve( response_size );
	for( DWORD i = 0 ; i < response_size ; i++ )
	{
		retResponse.data.push_back( response[ i ] );
	}

	SecureZeroMemory( response , response_size );
	return true;
}

bool mSCBase::Communicate( const TransmitDataLen& dt , ResponseData& retResponse )const
{
	retResponse.data.clear();

	//送信データの作成
	mSecureBinary senddata;
	LPCSCARD_IO_REQUEST protocol;
	senddata.push_back( dt.cla );
	senddata.push_back( dt.ins );
	senddata.push_back( dt.p1 );
	senddata.push_back( dt.p2 );
	senddata.push_back( dt.len );
	switch( MyActiveProtocol )
	{
	case Protocol::T0:
		protocol = SCARD_PCI_T0;
		break;
	case Protocol::T1:
		protocol = SCARD_PCI_T1;
		break;
	default:
		return false;
	}

	//送信処理
	BYTE response[ 256 ];	//結果格納バッファ
	DWORD response_size = sizeof( response );
	LONG result = SCardTransmit( MySCard , protocol , senddata.data() , (DWORD)senddata.size() , nullptr , response , &response_size );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , result , L"スマートカードとの通信が失敗しました" );
		SecureZeroMemory( response , sizeof( response ) );
		return false;
	}

	//結果のパース
	retResponse.data.reserve( response_size );
	for( DWORD i = 0 ; i < response_size ; i++ )
	{
		retResponse.data.push_back( response[ i ] );
	}
	SecureZeroMemory( response , response_size );
	return true;
}

bool mSCBase::Control( DWORD controlcode , const mBinary& senddata , mBinary* retResponse )const
{
	
	//送信処理
	BYTE response[ 256 ];	//結果格納バッファ
	DWORD response_size = 0;
	LONG result = SCardControl( MySCard , SCARD_CTL_CODE( controlcode ) , senddata.data() , (DWORD)senddata.size() , response , sizeof( response ) , &response_size );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , result , L"スマートカードとの通信が失敗しました" );
		return false;
	}

	//結果のパース
	if( retResponse )
	{
		retResponse->clear();
		retResponse->reserve( response_size );
		for( DWORD i = 0 ; i < response_size ; i++ )
		{
			retResponse->push_back( response[ i ] );
		}
	}
	return true;

}

const mBinary& mSCBase::GetCardId( void )const
{
	return MyCardID;
}

bool mSCBase::OnConnectCallback( void )
{
	return true;
}

mSCBase::ReaderMaker mSCBase::QueryMaker( void )const
{
	return MyReaderMaker;
}

//スマートカードリーダーの種類に対応した直接通信コマンドを設定する
void mSCBase::SetDirectCommand( TransmitData& retPacket ) const
{
	switch( QueryMaker() )
	{
	case ReaderMaker::READER_MAKER_SONY:
		retPacket.cla = 0xFFu;
		retPacket.ins = 0xFEu;
		retPacket.p1  = 0x01u;
		retPacket.p2  = 0x00u;
		break;
	case ReaderMaker::READER_MAKER_ACS:
	default:
		retPacket.cla = 0xFFu;
		retPacket.ins = 0x00u;
		retPacket.p1  = 0x00u;
		retPacket.p2  = 0x00u;
		break;
	}
}

mSCBase::TransparentSession::TransparentSession( const mSCBase& base ) :
	MyCard( base )
{
	TransmitData dt;
	dt.cla = 0xFFu;
	dt.ins = 0xC2u;
	dt.p1 = 0;
	dt.p2 = 0;
	dt.data.clear();
	dt.data.push_back( 0x81u );
	dt.data.push_back( 0x00u );

	ResponseData rsp;
	if( !MyCard.Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
	}

	MyIsValid = true;
	CreateLogEntry( g_ErrorLogger , 0 , L"スマートカードとのセッションを開始" );
	return;
}

mSCBase::TransparentSession::~TransparentSession()
{
	if( !MyIsValid )
	{
		return;
	}

	TransmitData dt;
	dt.cla = 0xFFu;
	dt.ins = 0xC2u;
	dt.p1 = 0;
	dt.p2 = 0;
	dt.data.clear();
	dt.data.push_back( 0x82u );
	dt.data.push_back( 0x00u );

	ResponseData rsp;
	if( !MyCard.Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
	}
	CreateLogEntry( g_ErrorLogger , 0 , L"スマートカードとのセッションを終了" );
	return;
}

bool mSCBase::TransparentSession::Communicate( const mBinary& in , mBinary& retout , TransarentResponse* retresponse )const
{
	if( !MyIsValid )
	{
		RaiseError( g_ErrorLogger , 0 , L"透過セッションが有効ではありません" );
		return false;
	}
	retout.clear();

	TransmitData dt;
	dt.cla = 0xFFu;	//Transparent Exchange(FF-C2-00-01)
	dt.ins = 0xC2u;
	dt.p1 = 0;
	dt.p2 = 1;
	dt.data.push_back( 0x95u );	//Transceive Data Object
	dt.data.push_back( (uint8_t)in.size() );
	dt.data.append( in );       //Card native command & data

	ResponseData rsp;
	bool result = MyCard.Communicate( dt , rsp );
	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
	}

	auto ParseErrorStatus = []( const mBinary& data , mBinary::const_iterator& itr , TransarentResponse* r )->bool
	{
		++itr;
		if( itr == data.end() ){ return false; }
		if( *itr != 0x03 ){ return false; }

		++itr;
		if( itr == data.end() ){ return false; }
		uint8_t code = *itr;

		++itr;
		if( itr == data.end() ){ return false; }
		uint8_t sw1 = *itr;

		++itr;
		if( itr == data.end() ){ return false; }
		uint8_t sw2 = *itr;
		if( r )
		{
			r->ErrorCode = ( code << 16 ) + ( sw1 << 8 ) + sw2;
			switch( ( sw1 << 8 ) + sw2 )
			{
			case 0x9000u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::NoError;
				break;
			case 0x6282u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::InformationNotAvailable;
				break;
			case 0x6300u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::NoInformation;
				break;
			case 0x6301u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::ExcecutionStopped;
				break;
			case 0x6A81u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::NotSupported;
				break;
			case 0x6700u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::UnexpectedLength;
				break;
			case 0x6A80u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::UnexpectedValue;
				break;
			case 0x6400u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::IFDNoResponse;
				break;
			case 0x6401u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::ICCNoResponse;
				break;
			case 0x6F00u:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::NoPreciseDiagnosis;
				break;
			default:
				r->ErrorDescription = TransarentResponse::ErrorDescriptionCode::Unknown;
				break;
			}
		}
		return true;
	};

	auto ParseFraming = []( const mBinary& data , mBinary::const_iterator& itr , TransarentResponse* r )->bool
	{
		++itr;
		if( itr == data.end() ){ return false; }
		if( *itr != 0x01 ){ return false; }

		++itr;
		if( itr == data.end() ){ return false; }
		if( r )
		{
			r->ResponseBitFraming = *itr;
		}
		return true;
	};

	auto ParseStatus = []( const mBinary& data , mBinary::const_iterator& itr , TransarentResponse* r )->bool
	{
		++itr;
		if( itr == data.end() ){ return false; }
		if( *itr != 0x02 ){ return false; }

		++itr;
		if( itr == data.end() ){ return false; }
		uint8_t v1 = *itr;

		++itr;
		if( itr == data.end() ){ return false; }
		uint8_t v2 = *itr;

		if( r )
		{
			r->IsCrcOK = ( ( v1 & 0x01u ) == 0 );
			r->IsCollisionOK = ( ( v1 & 0x02u ) == 0 );
			r->IsParityOK = ( ( v1 & 0x04u ) == 0 );
			r->IsFramingOK = ( ( v1 & 0x08u ) == 0 );
			r->CollisionPos = v2;
		}
		return true;
	};

	auto ParseData = []( const mBinary& data , mBinary::const_iterator& itr , mBinary& out )->bool
	{
		++itr;
		if( itr == data.end() ){ return false; }
		uint8_t size = *itr;

		for( uint16_t i = 0 ; i < size ; i++ )
		{
			++itr;
			if( itr == data.end() )
			{
				return false;
			}
			out.push_back( *itr );
		}
		return true;
	};

	auto ParseResult = []( const mBinary& data , mBinary::const_iterator& itr )->bool
	{
		++itr;
		if( itr == data.end() ){ return false; }
		if( *itr != 0x00 ){ return false; }

		return true;
	};

	for( mBinary::const_iterator itr = rsp.data.begin() ; itr != rsp.data.end() ; itr++ )
	{
		bool lambdarsp = false;
		switch( *itr )
		{
		case 0xC0u:
			lambdarsp = ParseErrorStatus( rsp.data , itr , retresponse );
			break;
		case 0x92u:
			lambdarsp = ParseFraming( rsp.data , itr , retresponse );
			break;
		case 0x96u:
			lambdarsp = ParseStatus( rsp.data , itr , retresponse );
			break;
		case 0x97u:
			lambdarsp = ParseData( rsp.data , itr , retout );
			break;
		case 0x90u:
			lambdarsp = ParseResult( rsp.data , itr );
			break;
		default:
			lambdarsp = false;
		}
		if( !lambdarsp )
		{
			RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が読み取れません" );
			return false;
		}
	}
	return true;
}


/*
//【カードリーダーの機種依存】
//もし可能ならスマートカードリーダーからブザーを鳴らします
bool mSCBase::Beep( void )const
{
	mBinary dt;
	dt.push_back( 0xE0u );
	dt.push_back( 0x00u );
	dt.push_back( 0x00u );
	dt.push_back( 0x28u );
	dt.push_back( 0x01u );
	dt.push_back( 0x0au );
	Control( 3500 , dt );
	return true;
}
*/


