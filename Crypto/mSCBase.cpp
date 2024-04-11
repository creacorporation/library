//----------------------------------------------------------------------------
// �X�}�[�g�J�[�h�n���h���x�[�X
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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

	//�������ς݃`�F�b�N
	if( MySCard )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�I�u�W�F�N�g�͏������ς݂ł�" );
		return false;
	}

	//�R���e�L�X�g���Ȃ���Ύ擾
	if( !MySCardContext )
	{
		result = SCardEstablishContext( SCARD_SCOPE_USER , nullptr , nullptr , &MySCardContext );
		switch( result )
		{
		case SCARD_S_SUCCESS:
			break;
		case SCARD_E_NO_SERVICE:
			RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�T�[�r�X���N�����Ă��܂���" );
			return false;
		default:
			RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�̏����������s���܂���" );
			return false;
		}
	}

	//�X�}�[�g�J�[�h�ւ̐ڑ�
	DWORD proto = 0;
	result = SCardConnectW( MySCardContext , reader.c_str() , SCARD_SHARE_SHARED , SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1 , &MySCard , &proto );
	switch( result )
	{
	case SCARD_S_SUCCESS:
		break;
	default:
		RaiseError( g_ErrorLogger , result , L"�J�[�h�Ƃ̐ڑ������s���܂���" );
		return false;
	}

	//�v���g�R���́H
	switch( proto )
	{
	case SCARD_PROTOCOL_T0:
		MyActiveProtocol = Protocol::T0;
		break;
	case SCARD_PROTOCOL_T1:
		MyActiveProtocol = Protocol::T1;
		break;
	default:
		RaiseError( g_ErrorLogger , result , L"�ʐM�v���g�R�����s���ł�" );
		MyActiveProtocol = Protocol::Unknwon;
		break;
	}

	//�J�[�hID���擾����
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
			RaiseError( g_ErrorLogger , 0 , L"�J�[�hID���擾�ł��܂���" );
			return false;
		}
		if( ( res.data.size() == 2 ) &&
			( res.data[ 0 ] == 0x63 && res.data[ 1 ] == 0x00 ) )
		{
			//���s����
			RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̑��삪���s���܂���" );
			return false;
		}
		else if( ( 6 <= res.data.size() ) &&
			( res.data[ res.data.size() - 2 ] == 0x90 && res.data[ res.data.size() - 1 ] == 0x00 ) )
		{
			//��������
			MyCardID = res.data.subdata( 0 , res.data.size() - 2 );
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�̉������s���ł�" );
			return false;
		}
	}

	//���[�_�[�̃��[�J�[�Z�b�g
	if( maker == ReaderMaker::READER_MAKER_GENERAL )
	{
		WString name = ToLower( reader );
		if( name.find( L"sony" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_SONY;	//�\�j�[�̉������ۂ�
		}
		else if( name.find( L"pasori" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_SONY;	//�p�\�����ۂ�
		}
		else if( name.find( L"acs" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_ACS;	//ACS���ۂ�
		}
		else if( name.find( L"acr" ) != WString::npos )
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_ACS;	//ACR�V���[�Y���ۂ�
		}
		else
		{
			MyReaderMaker = ReaderMaker::READER_MAKER_GENERAL;	//�킩���
		}
	}
	else
	{
		MyReaderMaker = maker;
	}

	//�J�[�h�ʂ̏��������s
	if( !OnConnectCallback() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�������������������s���܂���" );
		MyCardID.clear();
		return false;
	}
	return true;
}

bool mSCBase::Communicate( const TransmitData& dt , ResponseData& retResponse )const
{
	retResponse.data.clear();

	//�f�[�^�̃`�F�b�N
	if( 255 < dt.data.size() )
	{
		//�ǉ��f�[�^����������
		RaiseError( g_ErrorLogger , dt.data.size() , L"�ǉ��f�[�^���������܂�" );
		return false;
	}

	//���M�f�[�^�̍쐬
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
		RaiseError( g_ErrorLogger , MyActiveProtocol , L"�v���g�R�����s���ł�" );
		return false;
	}

	//���M����
	BYTE response[ 256 ];	//���ʊi�[�o�b�t�@
	DWORD response_size = sizeof( response );
	LONG result = SCardTransmit( MySCard , protocol , senddata.data() , (DWORD)senddata.size() , nullptr , response , &response_size );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�Ƃ̒ʐM�����s���܂���" );
		SecureZeroMemory( response , sizeof( response ) );
		return false;
	}

	//���ʂ̃p�[�X
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

	//���M�f�[�^�̍쐬
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

	//���M����
	BYTE response[ 256 ];	//���ʊi�[�o�b�t�@
	DWORD response_size = sizeof( response );
	LONG result = SCardTransmit( MySCard , protocol , senddata.data() , (DWORD)senddata.size() , nullptr , response , &response_size );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�Ƃ̒ʐM�����s���܂���" );
		SecureZeroMemory( response , sizeof( response ) );
		return false;
	}

	//���ʂ̃p�[�X
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
	
	//���M����
	BYTE response[ 256 ];	//���ʊi�[�o�b�t�@
	DWORD response_size = 0;
	LONG result = SCardControl( MySCard , SCARD_CTL_CODE( controlcode ) , senddata.data() , (DWORD)senddata.size() , response , sizeof( response ) , &response_size );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�Ƃ̒ʐM�����s���܂���" );
		return false;
	}

	//���ʂ̃p�[�X
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

//�X�}�[�g�J�[�h���[�_�[�̎�ނɑΉ��������ڒʐM�R�}���h��ݒ肷��
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

//�X�}�[�g�J�[�h���[�_�[�̎�ނɑΉ��������ڒʐM�R�}���h��ݒ肷��
void mSCBase::SetDirectCommand( TransmitDataLen& retPacket ) const
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

/*
//�y�J�[�h���[�_�[�̋@��ˑ��z
//�����\�Ȃ�X�}�[�g�J�[�h���[�_�[����u�U�[��炵�܂�
// activate_time : �u�U�[��炷����(1ms�P��)
//  �����Ԃ͓K�X�ۂ߂邱�Ƃ�����܂�
// ret : �炷���Ƃ��ł�����^
bool mSCBase::Beep( DWORD activate_time )
{
	mBinary dt;
	dt.push_back( 0xE0u );
	dt.push_back( 0x00u );
	dt.push_back( 0x00u );
	dt.push_back( 0x28u );
	dt.push_back( 0x01u );
	if( activate_time != 0 )
	{
		activate_time /= 10;
		if( activate_time == 0 )
		{
			activate_time = 1;
		}
		else if( 255 < activate_time )
		{
			activate_time = 255;
		}
	}
	dt.push_back( (BYTE)activate_time );
	return Control( 3500 , dt );
}
*/


