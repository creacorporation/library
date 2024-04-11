//----------------------------------------------------------------------------
// Felica�F�؃R�[�h�v�Z�@
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mSCFelicaMac.h"
#include <General/mErrorLogger.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#pragma comment(lib,"bcrypt.lib")

mSCFelicaMac::mSCFelicaMac()
{
	MyAlgHandle = 0;
	MyRKeyHandle = 0;
	MyRKeyObject.reset();
	MyWKeyHandle = 0;
	MyWKeyObject.reset();

	Initialize();
}

mSCFelicaMac::~mSCFelicaMac()
{
	//�L�[�̃n���h��
	ClearKey();
	//�A���S���Y���̃n���h��
	if( MyAlgHandle )
	{
		BCryptCloseAlgorithmProvider( MyAlgHandle , 0 );
		MyAlgHandle = 0;
	}
}

bool mSCFelicaMac::Initialize( void )
{
	NTSTATUS stat;
	if( MyAlgHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"��d�Ƀn���h�����J�����Ƃ��Ă��܂�" );
		return false;
	}

	//2key triple des�̃n���h����v��
	stat = BCryptOpenAlgorithmProvider( &MyAlgHandle , BCRYPT_3DES_112_ALGORITHM , nullptr , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�Í��v���o�C�_�̃n���h�����擾�ł��܂���" );
		return false;
	}

	//�Í������[�h���w��
	stat = BCryptSetProperty( MyAlgHandle , BCRYPT_CHAINING_MODE , (PUCHAR)BCRYPT_CHAIN_MODE_CBC , sizeof( BCRYPT_CHAIN_MODE_CBC ) , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�Í������[�h��ݒ�ł��܂���" );
		return false;
	}
	return true;
}

bool mSCFelicaMac::CheckIsWeakKey( const mSecureBinary& key )
{
	auto Check = []( const mSecureBinary& key , DWORD offset ) -> bool
	{
		unsigned long long keyval = 0;
		static const unsigned long long weak_key_table[] =
		{
			0x0101010101010101u,0xFEFEFEFEFEFEFEFEu,0xE0E0E0E0F1F1F1F1u,0x1F1F1F1F0E0E0E0Eu,	//�㌮
			0x0000000000000000u,0xFFFFFFFFFFFFFFFFu,0xE1E1E1E1F0F0F0F0u,0x1E1E1E1E0F0F0F0Fu,
			0x011F011F010E010Eu,0x1F011F010E010E01u,0x01E001E001F101F1u,0xE001E001F101F101u,	//���㌮
			0x01FE01FE01FE01FEu,0xFE01FE01FE01FE01u,0x1FE01FE00EF10EF1u,0xE01FE01FF10EF10Eu,
			0x1FFE1FFE0EFE0EFEu,0xFE1FFE1FFE0EFE0Eu,0xE0FEE0FEF1FEF1FEu,0xFEE0FEE0FEF1FEF1u,
			0x01011F1F01010E0Eu,0x1F1F01010E0E0101u,0xE0E01F1FF1F10E0Eu,0x0101E0E00101F1F1u,	//�㌮���
			0x1F1FE0E00E0EF1F1u,0xE0E0FEFEF1F1FEFEu,0x0101FEFE0101FEFEu,0x1F1FFEFE0E0EFEFEu,
			0xE0FE011FF1FE010Eu,0x011F1F01010E0E01u,0x1FE001FE0EF101FEu,0xE0FE1F01F1FE0E01u,
			0x011FE0FE010EF1FEu,0x1FE0E01F0EF1F10Eu,0xE0FEFEE0F1FEFEF1u,0x011FFEE0010EFEF1u,
			0x1FE0FE010EF1FE01u,0xFE0101FEFE0101FEu,0x01E01FFE01F10EFEu,0x1FFE01E00EFE01F1u,
			0xFE011FE0FE010EF1u,0xFE01E01FFE01F10Eu,0x1FFEE0010EFEF101u,0xFE1F01E0FE0E01F1u,
			0x01E0E00101F1F101u,0x1FFEFE1F0EFEFE0Eu,0xFE1FE001FE0EF101u,0x01E0FE1F01F1FE0Eu,
			0xE00101E0F10101F1u,0xFE1F1FFEFE0E0EFEu,0x01FE1FE001FE0EF1u,0xE0011FFEF1010EFEu,
			0xFEE0011FFEF1010Eu,0x01FEE01F01FEF10Eu,0xE001FE1FF101FE0Eu,0xFEE01F01FEF10E01u,
			0x01FEFE0101FEFE01u,0xE01F01FEF10E01FEu,0xFEE0E0FEFEF1F1FEu,0x1F01011F0E01010Eu,
			0xE01F1FE0F10E0EF1u,0xFEFE0101FEFE0101u,0x1F01E0FE0E01F1FEu,0xE01FFE01F10EFE01u,
			0xFEFE1F1FFEFE0E0Eu,0x1F01FEE00E01FEF1u,0xE0E00101F1F10101u,0xFEFEE0E0FEFEF1F1u,
		};

		for( DWORD i = 0 ; i < 8 ; i++ )
		{
			keyval <<= 8;
			keyval |= key[ ( 7 - i ) + offset ];
		}
		for( DWORD i = 0 ; i < array_count_of( weak_key_table ) ; i++ )
		{
			if( keyval == weak_key_table[ i ] )
			{
				return false;
			}
		}
		SecureZeroMemory( &keyval , sizeof( keyval ) );
		return true;
	};

	//CK1
	if( !Check( key , 0 ) )
	{
		//�㌮�Ȃ̂Ń_��
		return false;
	}
	//CK2
	if( !Check( key , 8 ) )
	{
		//�㌮�Ȃ̂Ń_��
		return false;
	}
	//�㌮�ł͂Ȃ�
	return true;
}

void mSCFelicaMac::ClearKey( void )
{
	//�L�[�̃n���h��
	if( MyRKeyHandle )
	{
		BCryptDestroyKey( MyRKeyHandle );
		MyRKeyHandle = 0;
	}
	MyRKeyObject.reset();

	if( MyWKeyHandle )
	{
		BCryptDestroyKey( MyWKeyHandle );
		MyWKeyHandle = 0;
	}
	MyWKeyObject.reset();

	//�`�������W
	MyChallenge.clear();
}


bool mSCFelicaMac::SetKey( const mSecureBinary& key , mBinary& retChallenge )
{
	retChallenge.clear();

	NTSTATUS stat;
	if( !MyAlgHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"����������Ă��܂���" );
		goto badend;
	}
	ClearKey();

	//���̃T�C�Y���m�F
	if( key.size() != 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"���̃T�C�Y���Ⴂ�܂�" );
		goto badend;
	}

	//�`�������W�𐶐�
	MyChallenge.resize( 16 );
	stat = BCryptGenRandom( nullptr , (PUCHAR)MyChallenge.data() , (ULONG)MyChallenge.size() , BCRYPT_USE_SYSTEM_PREFERRED_RNG );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"���������G���[" );
		goto badend;
	}
	retChallenge.resize( 16 );
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		retChallenge[ i + 0 ] = MyChallenge[  7 - i ];
		retChallenge[ i + 8 ] = MyChallenge[ 15 - i ];
	}

	//�Z�b�V�������̃n���h�����쐬����
	if( !CreateSessionKey( key ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�Z�b�V�������̐��������s���܂���" );
		goto badend;
	}

	//�`�������W�̌㔼�͗v��񂩂����
	SecureZeroMemory( &MyChallenge.data()[ 8 ] , 8 );
	MyChallenge.resize( 8 );
	return true;

badend:
	ClearKey();
	return false;
}

bool mSCFelicaMac::IsKeyExist( void )const
{
	return MyChallenge.size() == 8;
}


bool mSCFelicaMac::CreateSessionKey( const mSecureBinary& key )
{
	NTSTATUS stat;

	//�L�[�I�u�W�F�N�g�̃T�C�Y���Z��
	DWORD key_obj_size = 0;
	DWORD result_size = 0;
	stat = BCryptGetProperty( MyAlgHandle , BCRYPT_OBJECT_LENGTH , (PBYTE)&key_obj_size , sizeof( key_obj_size ) , &result_size , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�L�[�I�u�W�F�N�g�̃T�C�Y���擾�ł��܂���" );
		return false;
	}
	MyRKeyObject.reset( mNew BYTE[ key_obj_size ] );
	MyWKeyObject.reset( mNew BYTE[ key_obj_size ] );

	//�J�[�h���̃n���h���擾
	BYTE byte_swapped_data[ 16 ];
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		byte_swapped_data[ i + 0 ] = key[  7 - i ];
		byte_swapped_data[ i + 8 ] = key[ 15 - i ];
	}
	stat = BCryptGenerateSymmetricKey( MyAlgHandle , &MyRKeyHandle , MyRKeyObject.get() , key_obj_size , byte_swapped_data , sizeof( byte_swapped_data ) , 0 );
	SecureZeroMemory( byte_swapped_data , sizeof( byte_swapped_data ) );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�J�[�h���̃n���h�����擾�ł��܂���" );
		return false;
	}

	//�Z�b�V�������̒l���Z�o
	BYTE session_key[ 16 ];
	ULONG resultsize = 0;
	BYTE init_vector[ 8 ] = { 0 };
	stat = BCryptEncrypt( MyRKeyHandle , &MyChallenge.data()[ 0 ] , 8 , nullptr , init_vector , sizeof( init_vector ) , &session_key[ 0 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�Z�b�V���������o�G���[�P" );
		return false;
	}
	stat = BCryptEncrypt( MyRKeyHandle , &MyChallenge.data()[ 8 ] , 8 , nullptr , init_vector , sizeof( init_vector ) , &session_key[ 8 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�Z�b�V���������o�G���[�Q" );
		return false;
	}

	//�J�[�h���̃n���h���폜
	BCryptDestroyKey( MyRKeyHandle );
	MyRKeyHandle = 0;

	//�Z�b�V�������̃n���h���𐶐�(���[�h�p)
	stat = BCryptGenerateSymmetricKey( MyAlgHandle , &MyRKeyHandle , MyRKeyObject.get() , key_obj_size , session_key , sizeof( session_key ) , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"���[�h�p�Z�b�V�������̃n���h�����擾�ł��܂���" );
		return false;
	}

	//���C�g�p�Z�b�V��������SK1��SK2�����ւ���
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		BYTE tmp = session_key[ i ];
		session_key[ i ] = session_key[ i + 8 ];
		session_key[ i + 8 ] = tmp;
	}

	//�Z�b�V�������̃n���h���𐶐�(���C�g�p)
	stat = BCryptGenerateSymmetricKey( MyAlgHandle , &MyWKeyHandle , MyWKeyObject.get() , key_obj_size , session_key , sizeof( session_key ) , 0 );
	SecureZeroMemory( session_key , sizeof( session_key ) );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"���C�g�p�Z�b�V�������̃n���h�����擾�ł��܂���" );
		return false;
	}

	SecureZeroMemory( session_key , sizeof( session_key ) );
	return true;
}


//�ǂݍ��ݎ��ɂ�����MAC_A�̑̍ق��`�F�b�N����B�ȉ��𖞂����΍��i�B
//�EMAC_A(�u���b�N91h)�������ɂ��Ă��邱��
//�EMAC_A���Q�ȏ゠���Ă͂����Ȃ�
//�EMAC_A���܂߁A�u���b�N���͂Q�`�S��
//�EMAC_A�Ƀf�[�^���W�o�C�g�ȏ�i�[����Ă���
//�EMAC_A�ȊO�Ƀf�[�^���P�U�o�C�g�ȏ�i�[����Ă���
static bool CheckDataBlockMacStructure( const mSCFelicaMac::DataBlock& data )
{
	//�T�C�Y�`�F�b�N
	if( data.size() < 2 || 4 < data.size() )
	{
		return false;
	}
	//�����̃u���b�N�ԍ���MAC_A��
	if( data.back().BlockNumber != 0x91u )
	{
		return false;
	}
	//MAC_A���Q�ȏ㑶�݂��Ȃ����AMAC_A�ȊO�ɂ͂P�U�o�C�g�̃f�[�^�����邩
	for( DWORD i = 0 ; i < data.size() - 1 ; i++ )
	{
		if( data[ i ].BlockNumber == 0x91u )
		{
			return false;
		}
		if( data[ i ].Data.size() < 16 )
		{
			return false;
		}
	}
	//MAC_A�Ƀf�[�^�������Ă��邩
	if( data.back().Data.size() < 8 )
	{
		return false;
	}
	//���i
	return true;
}

bool mSCFelicaMac::ValidateMacA( const DataBlock& data )
{
	NTSTATUS stat;

	if( !MyRKeyHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�Z�b�V���������������ł�" );
		return false;
	}

	if( !CheckDataBlockMacStructure( data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_A���������i�[����Ă��܂���" );
		return false;
	}

	//�����f�[�^�̍쐬
	BYTE plain[ 8 ];
	plain[ 0 ] = ( data.size() < 4 ) ? ( 0xFFu ) : ( 0x00u );
	plain[ 1 ] = ( data.size() < 4 ) ? ( 0xFFu ) : ( (BYTE)data[ 3 ].BlockNumber );
	plain[ 2 ] = ( data.size() < 3 ) ? ( 0xFFu ) : ( 0x00u );
	plain[ 3 ] = ( data.size() < 3 ) ? ( 0xFFu ) : ( (BYTE)data[ 2 ].BlockNumber );
	plain[ 4 ] = 0x00u;
	plain[ 5 ] = (BYTE)data[ 1 ].BlockNumber;
	plain[ 6 ] = 0x00u;
	plain[ 7 ] = (BYTE)data[ 0 ].BlockNumber;

	//IV�̍쐬
	BYTE init_vector[ 8 ];
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		init_vector[ i ] = MyChallenge[ i ];
	}

	//�Í���(1) �u���b�N�ԍ�
	ULONG resultsize;
	BYTE cryptdata[ 8 ];
	stat = BCryptEncrypt( MyRKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A���o�G���[1" );
		return false;
	}

	//�Í���(2) �u���b�N�f�[�^
	for( DWORD i = 0 ; i < data.size() - 1 ; i++ )
	{
		for( DWORD j = 0 ; j < 8 ; j++ )
		{
			plain[ j ] = data[ i ].Data[ 7 - j ];
		}
		stat = BCryptEncrypt( MyRKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
		if( !NT_SUCCESS( stat ) )
		{
			RaiseError( g_ErrorLogger , stat , L"MAC_A���o�G���[2" );
			return false;
		}
		for( DWORD j = 0 ; j < 8 ; j++ )
		{
			plain[ j ] = data[ i ].Data[ 15 - j ];
		}
		stat = BCryptEncrypt( MyRKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
		if( !NT_SUCCESS( stat ) )
		{
			RaiseError( g_ErrorLogger , stat , L"MAC_A���o�G���[3" );
			return false;
		}
	}

	//����ꂽ�Í������ʂ�MAC_A�̒l�ƍ��v���邩�m�F
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		if( data.back().Data[ i ] != cryptdata[ 7 - i ] )
		{
			//���v���ĂȂ�����F�؎��s
			RaiseError( g_ErrorLogger , 0 , L"MAC_A�̎Z�o���ʂ����v���܂���" );
			return false;
		}
	}

	//�F�ؐ���
	return true;
}


bool mSCFelicaMac::CreateMacA( const DataBlockEntry& data , DataBlockEntry& retMacA , DWORD wcnt )
{
	retMacA.BlockNumber = 0x91u;	//MAC_A
	retMacA.Data.assign( 16 , 0 );	//0x00�~16�ɏ�����

	NTSTATUS stat;
	if( !MyWKeyHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"�Z�b�V���������������ł�" );
		return false;
	}
	if( data.Data.size() < 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_A�����Ώۂ̃f�[�^������܂���" );
		return false;
	}
	if( 0x00FF'FFFFu <= wcnt )
	{
		RaiseError( g_ErrorLogger , 0 , L"WCNT���ő�l�ɓ��B���Ă��܂�" );
		return false;
	}

	//�����f�[�^�̍쐬
	BYTE plain[ 8 ];
	plain[ 0 ] = 0x00u;
	plain[ 1 ] = 0x91u;
	plain[ 2 ] = 0x00u;
	plain[ 3 ] = (BYTE)data.BlockNumber;
	plain[ 4 ] = 0x00u;
	plain[ 5 ] = (BYTE)( ( wcnt >> 16 ) & 0xFFu );
	plain[ 6 ] = (BYTE)( ( wcnt >>  8 ) & 0xFFu );
	plain[ 7 ] = (BYTE)( ( wcnt >>  0 ) & 0xFFu );

	//IV�̍쐬
	BYTE init_vector[ 8 ];
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		init_vector[ i ] = MyChallenge[ i ];
	}	

	//�Í���(1) �u���b�N�ԍ�
	ULONG resultsize;
	BYTE cryptdata[ 8 ];
	stat = BCryptEncrypt( MyWKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A���o�G���[1" );
		return false;
	}

	//�Í���(2) �u���b�N�f�[�^
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		plain[ i ] = data.Data[ 7 - i ];
	}
	stat = BCryptEncrypt( MyWKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A���o�G���[2" );
		return false;
	}
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		plain[ i ] = data.Data[ 15 - i ];
	}
	stat = BCryptEncrypt( MyWKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A���o�G���[3" );
		return false;
	}

	//���ʃZ�b�g
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		retMacA.Data[ i ] = cryptdata[ 7 - i ];
	}
	retMacA.Data[  8 ] = (BYTE)( ( wcnt >>  0 ) & 0xFFu );
	retMacA.Data[  9 ] = (BYTE)( ( wcnt >>  8 ) & 0xFFu );
	retMacA.Data[ 10 ] = (BYTE)( ( wcnt >> 16 ) & 0xFFu );
	return true;
}


static uint64_t ByteSwap( uint64_t in )
{
	uint64_t result = 0;
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		result <<= 8;
		result |= ( in & 0xFFu );
		in >>= 8;
	}
	return result;
}

bool mSCFelicaMac::CalcDiversifiedKey( const mSecureBinary& master , const mBinary& id , mSecureBinary& retcardkey )
{
	//�\�j�[�̎d�l���ɂ̓G���f�B�A�����r�b�O�Ƃ����g���Ƃ������ĂȂ����A
	//�Q�l�ɂ����\�[�X�̓r�b�O�G���f�B�A���̂悤�Ȃ̂ŁA�u�ŏ�ʃr�b�g�v�Ƃ��u�P�r�b�g�V�t�g�v�Ƃ��̓r�b�O�G���f�B�A���ɍ��킹�ď�������B
	// ttps://github.com/ode1022/esp32_felica_lite

	NTSTATUS stat;
	BCRYPT_ALG_HANDLE alghandle = 0;
	BYTE* keyobject = nullptr;
	BCRYPT_KEY_HANDLE keyhandle = 0;

	retcardkey.clear();
	retcardkey.assign( 16 , 0 );

	//�T�C�Y�`�F�b�N
	if( master.size() != 24 || id.size() != 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"���܂���ID�u���b�N�̃T�C�Y���Ⴂ�܂�" );
		return false;
	}

	//3DES�̃n���h����v��
	stat = BCryptOpenAlgorithmProvider( &alghandle , BCRYPT_3DES_ALGORITHM , nullptr , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�Í��v���o�C�_�̃n���h�����擾�ł��܂���" );
		goto ending;
	}

	//�Í������[�h���w��
	stat = BCryptSetProperty( alghandle , BCRYPT_CHAINING_MODE , (PUCHAR)BCRYPT_CHAIN_MODE_CBC , sizeof( BCRYPT_CHAIN_MODE_CBC ) , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�Í������[�h��ݒ�ł��܂���" );
		goto ending;
	}

	//�L�[�I�u�W�F�N�g�̃T�C�Y���Z��
	DWORD key_obj_size;
	DWORD result_size;
	stat = BCryptGetProperty( alghandle , BCRYPT_OBJECT_LENGTH , (PBYTE)&key_obj_size , sizeof( key_obj_size ) , &result_size , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�L�[�I�u�W�F�N�g�̃T�C�Y���擾�ł��܂���" );
		goto ending;
	}
	keyobject = mNew BYTE[ key_obj_size ];

	//�J�[�h���̃n���h���擾
	stat = BCryptGenerateSymmetricKey( alghandle , &keyhandle , keyobject , key_obj_size , (PUCHAR)master.data() , (ULONG)master.size() , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"�J�[�h���̃n���h�����擾�ł��܂���" );
		goto ending;
	}

	//(2)L�̓��o
	uint64_t ivector = 0;
	uint64_t plain = 0;
	uint64_t value_L;
	ULONG resultsize;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&value_L , sizeof( value_L ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"L�l���o�G���[" );
		goto ending;
	}

	//(3)�r�b�g�V�t�g
	uint64_t value_K1 = ByteSwap( value_L );	//��������K1�����g���G���f�B�A���̕���
	value_K1 <<= 1;
	if( value_L & 0x0000'0000'0000'0080u )		//���r�b�O�G���f�B�A���̍ŏ�ʃr�b�g�̈ʒu
	{
		value_K1 ^= 0x0000'0000'0000'001Bu;
	}
	value_K1 = ByteSwap( value_K1 );			//�����܂�K1�����g���G���f�B�A���̕���

	//(4)M�̕����A(5)M2�̓��o
	uint64_t value_M1 = ( *(const uint64_t*)( &id[ 0 ] ) ) ^ ( 0        );
	uint64_t value_M2 = ( *(const uint64_t*)( &id[ 8 ] ) ) ^ ( value_K1 );

	//(6)C1�̓��o
	ivector = 0;
	plain = value_M1;
	uint64_t value_C1a;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&value_C1a , sizeof( value_C1a ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"C1�l���o�G���[" );
		goto ending;
	}

	//(7)T�̓��o
	ivector = 0;
	plain = value_C1a ^ value_M2;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&retcardkey[ 0 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"T�l���o�G���[" );
		goto ending;
	}

	//(8)M1'�A(9)C1'�̓��o
	ivector = 0;
	plain = value_M1 ^ 0x0000'0000'0000'0080u;	//���r�b�O�G���f�B�A���̍ŏ�ʃr�b�g�̈ʒu
	uint64_t value_C1b;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&value_C1b , sizeof( value_C1b ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"C1'�l���o�G���[" );
		goto ending;
	}

	//(10)T'�̓��o
	ivector = 0;
	plain = value_C1b ^ value_M2;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&retcardkey[ 8 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"T'�l���o�G���[" );
		goto ending;
	}

ending:
	bool result = NT_SUCCESS( stat );
	//�A���S���Y���̃n���h��
	if( keyhandle )
	{
		BCryptDestroyKey( keyhandle );
	}
	if( keyobject )
	{
		mDelete[] keyobject;
	}
	if( alghandle )
	{
		BCryptCloseAlgorithmProvider( alghandle , 0 );
	}
	return result;
}

