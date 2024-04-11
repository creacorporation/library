//----------------------------------------------------------------------------
// Felica Light-S �J�[�h�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mSCFelica.h"
#include <General/mErrorLogger.h>
#include <unordered_set>
#include <iterator>

mSCFelica::mSCFelica()
{
}


mSCFelica::~mSCFelica()
{
}

bool mSCFelica::OnConnectCallback( void )
{
	//�|�[�����O���s���A�Z�b�g���ꂽ�J�[�h��Felica Lite-S���ǂ���������s��

	//���M�f�[�^�̍\�z
	TransmitData dt;
	SetDirectCommand( dt );
	dt.data.push_back( 0x06u );	//�f�[�^��(���̃o�C�g���܂�)
	dt.data.push_back( 0x00u );	//polling�R�}���h
	dt.data.push_back( 0xFFu );	//�����ΏۃV�X�e���R�[�h(FFFFH=�S�ẴJ�[�h)�̏��
	dt.data.push_back( 0xFFu );	//�����ΏۃV�X�e���R�[�h(FFFFH=�S�ẴJ�[�h)�̉���
	dt.data.push_back( 0x01u );	//�V�X�e���R�[�h�����N�G�X�g
	dt.data.push_back( 0x00u );	//�����\�ȃ^�C���X���b�g��#0�̂�
	//�ʐM���{
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̒ʐM�����s���܂���" );
		return false;
	}
	//���s�����ꍇ�͂Q�o�C�g�ł���͂�
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//���s����
			RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̑��삪���s���܂���" );
			return false;
		}
	}
	//�f�[�^���̌���
	if( rsp.data.size() != 22 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�����T�C�Y���s���ł�" );
		return false;
	}
	//�Œ�̃f�[�^���z��ʂ肩����
	if( rsp.data[ 0 ] != 0x14u ||	//�z��f�[�^��
		rsp.data[ 1 ] != 0x01u ||	//���X�|���X�R�[�h
		rsp.data[ 20 ] != 0x90u ||	//���������P�o�C�g��
		rsp.data[ 21 ] != 0x00u )	//���������Q�o�C�g��
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�̉������s���ł�" );
		return false;
	}
	//���̃��C�u������Felica Lite-S�݂̂̑Ή��Ȃ̂ŁA����ȊO���͂���
	if( rsp.data[ 18 ] != 0x88u ||	//Felica Lite-S �̃V�X�e���R�[�h�̏�ʃo�C�g
		rsp.data[ 19 ] != 0xB4u )	//Felica Lite-S �̃V�X�e���R�[�h�̉��ʃo�C�g
	{
		RaiseError( g_ErrorLogger , 0 , L"Felica Lite�ł͂���܂���" );
		return false;
	}
	//�����p�����[�^�̃`�F�b�N
	// Felica Lite  ��IC��ʂ� 0xF0
	// Felica Lite-S��IC��ʂ� 0xF1�`0xF7
	if( rsp.data[ 11 ] < 0xF1u || 0xF7u < rsp.data[ 11 ] )
	{
		RaiseError( g_ErrorLogger , 0 , L"Felica Lite-S�ł͂���܂���" );
		return false;
	}
	return true;
}

bool mSCFelica::RawRead( const ReadRequestBlock& request , DataBlock& retResponse )const
{
	retResponse.clear();
	if( request.size() == 0 || 4 < request.size() )
	{
		RaiseError( g_ErrorLogger , request.size() , L"�ǂݏo���v���̃u���b�N��������������܂���" );
		return false;
	}

	//���M�f�[�^�̍\�z
	TransmitData dt;
	SetDirectCommand( dt );
	dt.data.push_back( 0x00u );	//�f�[�^��(���̃o�C�g���܂�) �������́B�Ō�ɏ���������B
	dt.data.push_back( 0x06u );	//Read Without Encryption�R�}���h
	dt.data.append( MyCardID );	//�v����̃J�[�hUID
	dt.data.push_back( 0x01u );	//�T�[�r�X���B�P�Œ�B
	dt.data.push_back( 0x0Bu );	//�T�[�r�X�̂P���P�ځi���ʃo�C�g�j��RW��RO�u���b�N�ɃA�N�Z�X�\��0x000B
	dt.data.push_back( 0x00u );	//�T�[�r�X�̂P���P�ځi��ʃo�C�g�j  RW�u���b�N�݂̂ɃA�N�Z�X�\��0x0009
	dt.data.push_back( (BYTE)request.size() );	//�v������u���b�N�̐�
	for( ReadRequestBlock::const_iterator itr = request.begin() ; itr != request.end() ; itr++ )
	{
		dt.data.push_back( 0x80u );	//�Q�o�C�g�G�������g�B���X�g���ƃA�N�Z�X���[�h��FeilcaLiteS�ł͏��0�Ƃ���B
		if( 0xFFu < *itr )
		{
			RaiseError( g_ErrorLogger , *itr , L"�u���b�N�ԍ����s���ł�" );
			return false;
		}
		dt.data.push_back( (BYTE)*itr );	//�ǂݍ��݂����u���b�N�̔ԍ�
	}
	//�����͂����f�[�^�������ۂ̒l�ɏ���������
	dt.data[ 0 ] = (BYTE)dt.data.size();

	//�ʐM���{
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̒ʐM�����s���܂���" );
		return false;
	}
	//���s�����ꍇ�͂Q�o�C�g�ł���͂�
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//���s����
			RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̑��삪���s���܂���" );
			return false;
		}
	}
	//�f�[�^���̌���
	// 1 = �f�[�^���P�o�C�g
	// 11 = ���X�|���X�R�[�h�P�o�C�g�{IDm�W�o�C�g�{�X�e�[�^�X�t���O1/2�e�P�o�C�g
	// 1 = �u���b�N���P�o�C�g(�������̂�)
	// 16n = ���f�[�^�P�U�o�C�g�~�ǂݎ�����u���b�N��(�������̂�)
	// 2 = PC/SC�̐�������(9000H)�Q�o�C�g
	DWORD correct_size_success = 1 + 11 + 1 + ( 16 * (DWORD)request.size() ) + 2;
	DWORD correct_size_fail = 1 + 11 + 2;
	if( rsp.data.size() == correct_size_fail )
	{
		RaiseError( g_ErrorLogger , ( rsp.data[ 10 ] << 8 ) + ( rsp.data[ 11 ] ) , L"�X�}�[�g�J�[�h�����s��Ԃ��܂���" );
		return false;
	}
	else if( rsp.data.size() != correct_size_success )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�����T�C�Y���s���ł�" );
		return false;
	}
	//�w�b�_���̌���
	if( ( rsp.data[ 0 ] != (BYTE)( correct_size_success - 2 ) ) ||	//�f�[�^��
		( rsp.data[ 1 ] != 0x07u ) ||								//���X�|���X�R�[�h
		( rsp.data[ 12 ] != request.size() ) ||						//�u���b�N��
		( rsp.data[ rsp.data.size() - 2 ] != 0x90u ) ||				//PC/SC�̐�������
		( rsp.data[ rsp.data.size() - 1 ] != 0x00u ) )				//PC/SC�̐�������
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�̉������s���ł�" );
		return false;
	}
	//IDm�����v���Ă��邩
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		if( MyCardID[ i ] != rsp.data[ i + 2 ] )
		{
			RaiseError( g_ErrorLogger , 0 , L"���X�|���X��ID����v���܂���" );
			return false;
		}
	}
	//���؂n�j�Ȃ̂Ńu���b�N�f�[�^��ݒ�
	retResponse.resize( request.size() );
	for( DWORD i = 0 ; i < request.size() ; i++ )
	{
		retResponse[ i ].BlockNumber = request[ i ];
		retResponse[ i ].Data = rsp.data.subdata( 13 + 16 * i , 16 );
	}
	rsp.data.secure_erase();
	return true;
}

//�f�[�^�̏�������
bool mSCFelica::RawWrite( const DataBlock& data )const
{
	bool result = false;
	if( data.size() == 0 || 4 < data.size() )
	{
		RaiseError( g_ErrorLogger , data.size() , L"�������ݗv���̃u���b�N��������������܂���" );
		return false;
	}

	//���M�f�[�^�̍\�z
	TransmitData dt;
	SetDirectCommand( dt );
	dt.data.push_back( 0x00u );	//�f�[�^��(���̃o�C�g���܂�) �������́B�Ō�ɏ���������B
	dt.data.push_back( 0x08u );	//Write Without Encryption�R�}���h
	dt.data.append( MyCardID );	//�v����̃J�[�hUID
	dt.data.push_back( 0x01u );	//�T�[�r�X���B�P�Œ�B
	dt.data.push_back( 0x09u );	//�T�[�r�X�̂P���P�ځi���ʃo�C�g�j��RW��RO�u���b�N�ɃA�N�Z�X�\��0x000B
	dt.data.push_back( 0x00u );	//�T�[�r�X�̂P���P�ځi��ʃo�C�g�j  RW�u���b�N�݂̂ɃA�N�Z�X�\��0x0009
	dt.data.push_back( (BYTE)data.size() );	//�v������u���b�N�̐�
	for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		dt.data.push_back( 0x80u );	//�Q�o�C�g�G�������g�B���X�g���ƃA�N�Z�X���[�h��FeilcaLiteS�ł͏��0�Ƃ���B
		if( 0xFFu < itr->BlockNumber )
		{
			RaiseError( g_ErrorLogger , itr->BlockNumber , L"�u���b�N�ԍ����s���ł�" );
			return false;
		}
		dt.data.push_back( (BYTE)itr->BlockNumber );	//�ǂݍ��݂����u���b�N�̔ԍ�
	}
	for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		DWORD length = (DWORD)itr->Data.size();
		if( length <= 16 )
		{
			//�P�U�o�C�g�ȉ��̏ꍇ�́A�f�[�^���R�s�[���P�U�o�C�g�ɖ����Ȃ��������O�Ŗ��߂�
			dt.data.append( itr->Data );
			for( ; length < 16 ; length++ )
			{
				dt.data.push_back( 0x00 );
			}
		}
		else
		{
			//�P�U�o�C�g��葽���ꍇ�́A�擪�̂P�U�o�C�g���R�s�[����
			dt.data.append( itr->Data.subdata( 0 , 16 ) );
		}
	}
	//�����͂����f�[�^�������ۂ̒l�ɏ���������
	dt.data[ 0 ] = (BYTE)dt.data.size();

	//�ʐM���{
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̒ʐM�����s���܂���" );
		goto ending;
	}
	//���s�����ꍇ�͂Q�o�C�g�ł���͂�
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//���s����
			RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�Ƃ̑��삪���s���܂���" );
			goto ending;
		}
	}
	//�f�[�^���̌���
	// 1 = �f�[�^���P�o�C�g
	// 11 = ���X�|���X�R�[�h�P�o�C�g�{IDm�W�o�C�g�{�X�e�[�^�X�t���O1/2�e�P�o�C�g
	// 2 = PC/SC�̐�������(9000H)�Q�o�C�g
	if( rsp.data.size() != ( 1 + 11 + 2 ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�����T�C�Y���s���ł�" );
		goto ending;
	}
	//�w�b�_���̌���
	if( ( rsp.data[ 0 ] != 12 ) ||					//�f�[�^��
		( rsp.data[ 1 ] != 0x09u ) ||				//���X�|���X�R�[�h
		( rsp.data[ 12 ] != 0x90u ) ||				//PC/SC�̐�������
		( rsp.data[ 13 ] != 0x00u ) )				//PC/SC�̐�������
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�̉������s���ł�" );
		goto ending;
	}
	//IDm�����v���Ă��邩
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		if( MyCardID[ i ] != rsp.data[ i + 2 ] )
		{
			RaiseError( g_ErrorLogger , 0 , L"���X�|���X��ID����v���܂���" );
			goto ending;
		}
	}
	//�w�b�_���̌���
	if( ( rsp.data[ 10 ] != 0x00u ) ||				//�X�e�[�^�X�t���O�P
		( rsp.data[ 11 ] != 0x00u ) )				//�X�e�[�^�X�t���O�Q
	{
		RaiseError( g_ErrorLogger , ( rsp.data[ 10 ] << 8 ) + ( rsp.data[ 11 ] ) , L"�X�}�[�g�J�[�h�����s��Ԃ��܂���" );
		goto ending;
	}

	result = true;
ending:
	dt.data.secure_erase();
	rsp.data.secure_erase();
	return result;

}



bool mSCFelica::MacReadInternal( const ReadRequestBlock& request , DataBlock& retResponse )const
{
	retResponse.clear();
	if( request.size() == 0 || 3 < request.size() )
	{
		RaiseError( g_ErrorLogger , request.size() , L"�ǂݍ��ݗv���̃u���b�N��������������܂���" );
		return false;
	}

	//�ǂݎ��̎��s
	ReadRequestBlock req = request;
	req.push_back( 0x91 );	//MAC_A
	if( !RawRead( req , retResponse ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�ǂݎ�肪���s���܂���" );
		return false;
	}

	//MAC�̌���
	if( !MyFelicaMac.ValidateMacA( retResponse ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_A�̌��؂����s���܂���" );
		retResponse.pop_back();
		return false;
	}
	retResponse.pop_back();
	return true;
}


bool mSCFelica::GetUserID( mBinary& retVal , bool macauth )const
{
	DataBlock dt;
	retVal.clear();

	//�ǂݍ���
	ReadRequestBlock req;
	req.push_back( 0x82 );

	bool result;
	if( macauth )
	{
		result = MacReadInternal( req , dt );
	}
	else
	{
		result = RawRead( req , dt );
	}

	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"ID��ǂݎ��܂���" );
		return false;
	}
	retVal = dt[ 0 ].Data.subdata( 10 , 6 );
	return true;
}

bool mSCFelica::GetKeyVersion( uint16_t& retVal , bool macauth )const
{
	DataBlock dt;
	retVal = 0;

	//�ǂݍ���
	ReadRequestBlock req;
	req.push_back( 0x86 );

	bool result;
	if( macauth )
	{
		result = MacReadInternal( req , dt );
	}
	else
	{
		result = RawRead( req , dt );
	}

	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"���o�[�W������ǂݎ��܂���" );
		return false;
	}
	retVal = ( dt[ 0 ].Data[ 1 ] << 8 ) + dt[ 0 ].Data[ 0 ];
	return true;
}

bool mSCFelica::ExecFirstIssuance( bool lock , const IssuanceParam& param )
{
	//���̃`�F�b�N
	if( !MyFelicaMac.CheckIsWeakKey( param.Key ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�w�肵�����͐Ǝ㌮�̃��X�g�Ɍf�ڂ���Ă��܂�" );
		return false;
	}

	//ID�̐ݒ���s��
	mBinary IdBlockData;
	{
		//��������
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x82;
		writedata[ 0 ].Data.assign( 16 , 0x00u );
		for( DWORD i = 0 ; i < 8 ; i++ )
		{
			writedata[ 0 ].Data[ i ] = MyCardID[ i ];
		}
		for( DWORD i = 0 ; i < 6 ; i++ )
		{
			writedata[ 0 ].Data[ 10 + i ] = param.Id[ i ];
		}
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"ID���������߂܂���" );
			return false;
		}
		//���̏������݂Ɏg���̂ŕۑ�
		IdBlockData = writedata[ 0 ].Data;
		//�x���t�@�C
		ReadRequestBlock req;
		req.push_back( 0x82 );	//ID
		if( !RawRead( req , writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"ID��ǂݍ��߂܂���" );
			return false;
		}
		for( DWORD i = 0 ; i < 16 ; i++ )
		{
			if( IdBlockData[ i ] != writedata[ 0 ].Data[ i ] )
			{
				RaiseError( g_ErrorLogger , 0 , L"ID�x���t�@�C�G���[" );
				return false;
			}
		}
	}
	//���̏������݂��s��
	{
		//��������
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x87;	//CK
		if( param.Key.size() == 16 )
		{
			//�J�[�h��
			writedata[ 0 ].Data.assign( 16 , 0x00u );
			for( DWORD i = 0 ; i < 16 ; i++ )
			{
				writedata[ 0 ].Data[ i ] = param.Key[ i ];
			}
		}
		else if( param.Key.size() == 24 )
		{
			//�}�X�^�[��
			mSecureBinary ck;
			if( !mSCFelicaMac::CalcDiversifiedKey( param.Key , IdBlockData , ck ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"�ʉ��J�[�h���̐��������s���܂���" );
				return false;
			}
			for( DWORD i = 0 ; i < 16 ; i++ )
			{
				writedata[ 0 ].Data[ i ] = ck[ i ];
			}
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"�L�[�̒������Ⴂ�܂�" );
			return false;
		}
		if( !RawWrite( writedata ) )
		{
			writedata[ 0 ].Data.secure_erase();
			RaiseError( g_ErrorLogger , 0 , L"�L�[���������߂܂���" );
			return false;
		}
		writedata[ 0 ].Data.secure_erase();
		//�x���t�@�C���s��
		if( !ExecAuthenticationInternal( param.Key , true ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�L�[�x���t�@�C�G���[" );
			return false;
		}
	}
	//�J�[�h���̃o�[�W������������
	{
		//��������
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x86;	//CK
		writedata[ 0 ].Data.push_back( ( param.KeyVersion >> 0 ) & 0xFFu );
		writedata[ 0 ].Data.push_back( ( param.KeyVersion >> 8 ) & 0xFFu );
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�L�[�o�[�W�������������߂܂���" );
			return false;
		}
		uint16_t readversion;
		if( !GetKeyVersion( readversion , false ) ||
			( readversion != param.KeyVersion ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�L�[�o�[�W�����x���t�@�C�G���[" );
			return false;
		}
	}
	//�������R���t�B�O���[�V�����u���b�N
	{
		//�E�������R���t�B�O���[�V�����u���b�N�́A���ݒl���X�V����`�ŏ����ނ̂ŁA�܂��͌��݂̐ݒ��ǂݍ���
		DataBlock writedata;
		ReadRequestBlock req;
		req.push_back( 0x88 );	//�������R���t�B�O���[�V�����u���b�N
		if( !RawRead( req , writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V������ǂݎ��܂���" );
			return false;
		}
		//�������R���t�B�O���[�V�����u���b�N�̃f�[�^�X�V
		//byte12 MemoryConfig
		writedata[ 0 ].Data[ 12 ] = ( param.EnableStateMacReq ) ? ( 1 ) : ( 0 );
		//byte5 CK�ACKV�ւ̏������݉�
		writedata[ 0 ].Data[ 5 ] = ( param.EnableKeyChange ) ? ( 1 ) : ( 0 );
		//byte4 RF�p�����[�^
		//���������ݎ��͕K��07H�������߂Ə����Ă���
		writedata[ 0 ].Data[ 4 ] = 7;
		//byte3 NDEF�ݒ�
		//���Ή��Ȃ�01H�A��Ή��Ȃ�00H
		writedata[ 0 ].Data[ 3 ] = 0;
		//byte2 �A�N�Z�X���ݒ�
		writedata[ 0 ].Data[ 2 ] = ( lock ) ? ( 0x00u ) : ( 0xFFu );

		//��������
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V�������������߂܂���" );
			return false;
		}
		//�x���t�@�C���s��
		DataBlock response;
		if( !RawRead( req , response ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V������ǂݍ��߂܂���" );
			return false;
		}
		for( DWORD i = 0 ; i < 16 ; i++ )
		{
			if( response[ 0 ].Data[ i ] != writedata[ 0 ].Data[ i ] )
			{
				RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V�����x���t�@�C�G���[" );
				return false;
			}
		}
	}
	return true;
}

bool mSCFelica::ExecSecondIssuance( bool lock , const Permission& param )
{

	uint16_t ReadWriteAccess = 0;	//byte0-1 �ǂݎ���p
	uint16_t ReqAuthR = 0;			//byte6-7 �ǂݎ��ɊO���F�؂��K�v
	uint16_t ReqAuthW = 0;			//byte8-9 �������݂ɊO���F�؂��K�v
	uint16_t ReqMacW = 0;			//byte10-11 �������݂�MAC���A�N�Z�X���K�v
	bool ReqMacStateBlock = false;	//byte12 STATE�u���b�N�ւ̏������݂�MAC���A�N�Z�X���K�v

	//�����ݗp�f�[�^�̃X�L����
	for( int i = 14 ; 0 <= i ; i-- )
	{
		ReqAuthR <<= 1;
		switch( param.Read[ i ] )
		{
		case ReadPermission::ReadEveryone:
			break;
		case ReadPermission::ReadAuthUser:
			ReqAuthR |= 0x0001u;
			break;
		default:
			RaiseError( g_ErrorLogger , i , L"�ǂݎ��A�N�Z�X���̎w�肪����������܂���" );
			return false;
		}

		ReadWriteAccess <<= 1;
		ReqAuthW <<= 1;
		ReqMacW <<= 1;
		switch( param.Write[ i ] )
		{
		case WritePermission::NotWritable:
			break;
		case WritePermission::WriteEveryone:
			ReadWriteAccess |= 0x0001u;
			break;
		case WritePermission::WriteAuthUser:
			ReadWriteAccess |= 0x0001u;
			ReqAuthW |= 0x0001u;
			break;
		case WritePermission::WriteAuthUserAndMac:
			ReadWriteAccess |= 0x0001u;
			ReqAuthW |= 0x0001u;
			ReqMacW |= 0x0001u;
			break;
		default:
			RaiseError( g_ErrorLogger , i , L"�������݃A�N�Z�X���̎w�肪����������܂���" );
			return false;
		}
	}

	//MC�u���b�N���̂̃A�N�Z�X��(���b�N�Ȃ��Ȃ�RW�Ƃ���1�𗧂Ă�)
	if( !lock )
	{
		ReadWriteAccess |= 0x8000u;
	}

	//STATE�u���b�N��MAC���K�v������
	if( ReqAuthR || ReqAuthW )
	{
		ReqMacStateBlock = true;
	}
	else
	{
		ReqMacStateBlock = false;
	}

	//�������R���t�B�O���[�V�����u���b�N�̍X�V
	{
		//�E�������R���t�B�O���[�V�����u���b�N�́A���ݒl���X�V����`�ŏ����ނ̂ŁA�܂��͌��݂̐ݒ��ǂݍ���
		DataBlock writedata;
		ReadRequestBlock req;
		req.push_back( 0x88 );	//�������R���t�B�O���[�V�����u���b�N
		if( !RawRead( req , writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V������ǂݎ��܂���" );
			return false;
		}
		//�������R���t�B�O���[�V�����u���b�N�̃f�[�^�X�V
		//byte12 MemoryConfig
		writedata[ 0 ].Data[ 12 ] = ( ReqMacStateBlock ) ? ( 1 ) : ( 0 );
		writedata[ 0 ].Data[ 11 ] = (BYTE)( ( ReqMacW >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[ 10 ] = (BYTE)( ( ReqMacW >> 0 ) & 0xFFu );
		writedata[ 0 ].Data[  9 ] = (BYTE)( ( ReqAuthW >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[  8 ] = (BYTE)( ( ReqAuthW >> 0 ) & 0xFFu );
		writedata[ 0 ].Data[  7 ] = (BYTE)( ( ReqAuthR >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[  6 ] = (BYTE)( ( ReqAuthR >> 0 ) & 0xFFu );
		writedata[ 0 ].Data[  1 ] = (BYTE)( ( ReadWriteAccess >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[  0 ] = (BYTE)( ( ReadWriteAccess >> 0 ) & 0xFFu );

		//��������
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V�������������߂܂���" );
			return false;
		}
		//�x���t�@�C���s��
		DataBlock response;
		if( !RawRead( req , response ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V������ǂݍ��߂܂���" );
			return false;
		}
		for( DWORD i = 0 ; i < 16 ; i++ )
		{
			if( response[ 0 ].Data[ i ] != writedata[ 0 ].Data[ i ] )
			{
				RaiseError( g_ErrorLogger , 0 , L"�������R���t�B�O���[�V�����x���t�@�C�G���[" );
				return false;
			}
		}
	}
	return true;
}


bool mSCFelica::ExecAuthentication( const mSecureBinary& key , bool int_auth_only )
{
	if( key.size() == 16 )
	{
		//�J�[�h���Ȃ̂ł��̂܂܎g��
		return ExecAuthenticationInternal( key , int_auth_only );
	}
	else if( key.size() == 24 )
	{
		//�}�X�^�[���Ȃ̂ŃJ�[�h���𓱏o
		ReadRequestBlock idreq;
		DataBlock idrsp;
		idreq.push_back( 0x82u );
		if( !RawRead( idreq , idrsp ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"ID��ǂݎ��܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}

		mSecureBinary cardkey;
		if( !mSCFelicaMac::CalcDiversifiedKey( key , idrsp[ 0 ].Data , cardkey ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�ʉ��J�[�h���𓱏o�ł��܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}
		return ExecAuthenticationInternal( cardkey , int_auth_only );
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"���̃T�C�Y���Ⴂ�܂�" );
		MyFelicaMac.ClearKey();
		return false;
	}
}

bool mSCFelica::ExecAuthenticationInternal( const mSecureBinary& key , bool int_auth_only )
{

	if( key.size() != 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"���̃T�C�Y���Ⴂ�܂�" );
		MyFelicaMac.ClearKey();
		return false;
	}

	//���̐ݒ�ƁA�J�[�h�ɑ���`�������W�̎擾
	mBinary challenge;
	MyFelicaMac.SetKey( key , challenge );

	//�`�������W�̏�������
	{
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x80;	//RC(�����_���`�������W)
		writedata[ 0 ].Data = challenge;
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�`�������W�l���������߂܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}
	}

	//���؃f�[�^��MAC_A���擾���ē����F�؂��s���B���ł�WCNT���擾����B
	DWORD wcnt = 0;
	{
		ReadRequestBlock req;
		req.push_back( 0x82u );	//ID
		req.push_back( 0x86u );	//CKV
		if( !int_auth_only )
		{
			req.push_back( 0x90u );	//WCNT�i�O���F�؂̏ꍇ�̂ݕK�v�j
		}
		DataBlock response;
		if( !MacReadInternal( req , response ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�����F�؂����s���܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}

		//�����F�؂̂ݍs���ꍇ�͂����ł����܂�
		if( int_auth_only )
		{
			//�����F�؂̂ݐ���
			return true;
		}

		//WCNT�̒l
		wcnt = ( response[ 2 ].Data[ 2 ] << 16 ) + ( response[ 2 ].Data[ 1 ] << 8 ) + ( response[ 2 ].Data[ 0 ] << 0 );
	}

	//STATE��EXT_AUTH��01H�������ށB�������ݐ����ŊO���F�ؐ����i�����ݔF�ؐ����j
	{
		DataBlockEntry extdata;
		extdata.BlockNumber = 0x92u; //STATE
		extdata.Data.assign( 16 , 0 );
		extdata.Data[ 0 ] = 1;
		if( !MacWriteInternal( extdata , wcnt ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�O���F�؂����s���܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}
	}

	//���ݔF�ؐ���
	return true;
}

bool mSCFelica::CalcHash( const mBinary& in , mBinary& rethash )
{
	rethash.clear();

	//���F�؏�ԁH
	if( MyFelicaMac.IsKeyExist() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�F�؍ς݂ł�" );
		return false;
	}

	//�`�������W�̏�������
	DataBlock writedata;
	writedata.resize( 1 );
	writedata[ 0 ].BlockNumber = 0x80;	//RC(�����_���`�������W)
	writedata[ 0 ].Data = in;
	if( !RawWrite( writedata ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�n�b�V���f�[�^���������߂܂���" );
		MyFelicaMac.ClearKey();
		return false;
	}

	//���ʂ̎擾
	ReadRequestBlock req;
	req.push_back( 0x82u );	//ID
	req.push_back( 0x81u );	//MAC
	DataBlock response;
	if( !RawRead( req , response ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�n�b�V���l�擾�����s���܂���" );
		return false;
	}

	//���ʂ̊i�[
	rethash = response[ 1 ].Data.subdata( 0 , 8 );
	return true;
}

bool mSCFelica::MacWriteInternal( const DataBlockEntry& data , DWORD wcnt )const
{
	DataBlockEntry mac;
	if( !MyFelicaMac.CreateMacA( data , mac , wcnt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_A�̐��������s���܂���" );
		return false;
	}

	DataBlock datablock;
	datablock.push_back( data );	//��STATE
	datablock.push_back( mac );		//���Z�o����MAC

	if( !RawWrite( datablock ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC���������݂����s���܂���" );
		return false;
	}
	return true;
}

bool mSCFelica::GetChecksumResult( bool macauth )const
{
	//�ǂݍ���
	ReadRequestBlock req;
	DataBlock dt;
	req.push_back( 0xA0u );	//CRC_CHECK

	bool result;
	if( macauth )
	{
		result = MacReadInternal( req , dt );
	}
	else
	{
		result = RawRead( req , dt );
	}

	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"CRC���،��ʂ�ǂݎ��܂���" );
		return false;
	}

	return ( dt[ 0 ].Data[ 0 ] == 0x00u );
}

mSCFelica::AuthStatus mSCFelica::GetAuthStatus( void )const
{
	//�ǂݍ���
	ReadRequestBlock req;
	DataBlock dt;
	req.push_back( 0x92u );	//STATE

	if( !MacReadInternal( req , dt ) )
	{
		//Mac���ǂݍ��݂����s����ꍇ�A�F�؂���ĂȂ�
		return mSCFelica::AuthStatus::Unauthorized;
	}
	if( dt[ 0 ].Data[ 0 ] == 0x01u )
	{
		//Mac���ǂݍ��݁i�������F�؍ς݂̗v���j���������AEXT_AUTH�t���O���P�Ȃ�O���F�؍ς�
		return mSCFelica::AuthStatus::ExtAuthorized;
	}

	//����ȊO�͓����F�؂݂̂��ς�
	return mSCFelica::AuthStatus::IntAuthorized;
}

//�f�[�^�̓ǂݎ��
bool mSCFelica::Read( const ReadRequestBlock& request , DataBlock& retResponse , bool macauth )const
{
	//�O�̃f�[�^������
	retResponse.clear();

	//����̃u���b�N�����x���ǂ����Ƃ��Ă��Ȃ����`�F�b�N����
	//���[�U�[�u���b�N�ȊO��ǂ����Ƃ��Ă��Ȃ������`�F�b�N����
	{
		using Blocks = std::unordered_set<DWORD>;
		Blocks blocks;
		for( ReadRequestBlock::const_iterator itr = request.begin() ; itr != request.end() ; itr++ )
		{
			if( blocks.count( *itr ) )
			{
				RaiseError( g_ErrorLogger , *itr , L"����u���b�N�𕡐���ǂݏo�����Ƃ��܂���" );
				return false;
			}
			if( 0x0Eu < *itr )
			{
				RaiseError( g_ErrorLogger , *itr , L"���[�U�[�u���b�N�ȊO��ǂݎ�낤�Ƃ��܂���" );
				return false;
			}
			blocks.insert( *itr );
		}
	}

	//�����ǂݍ���
	{
		auto execread = [&retResponse,macauth,this]( const ReadRequestBlock& req )->bool
		{
			DataBlock rsp;
			bool result;

			if( macauth )
			{
				result = MacReadInternal( req , rsp );
			}
			else
			{
				result = RawRead( req , rsp );
			}
			if( !result )
			{
				RaiseError( g_ErrorLogger , 0 , L"�f�[�^��ǂݎ��܂���" );
				return false;
			}

			std::move( rsp.begin() , rsp.end() , std::back_inserter( retResponse ) );
			return true;
		};

		ReadRequestBlock req;
		DWORD max_blocks = ( macauth ) ? ( 3 ) : ( 4 );	//MAC�t���̏ꍇ�͈�x�ɂR�u���b�N�A���ĂȂ��ꍇ�͂S�u���b�N�ǂ߂�

		for( ReadRequestBlock::const_iterator itr = request.begin() ; itr != request.end() ; itr++ )
		{
			req.push_back( *itr );
			if( req.size() == max_blocks )
			{
				if( !execread( req ) )
				{
					return false;
				}
				req.clear();
			}
		}
		if( req.size() )
		{
			if( !execread( req ) )
			{
				return false;
			}
		}
	}
	return true;
}

DWORD mSCFelica::GetWCNT( void )const
{
	//�ǂݍ���
	ReadRequestBlock req;
	req.push_back( 0x90u );

	DataBlock dt;
	if( !RawRead( req , dt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"WCNT��ǂݎ��܂���" );
		return 0xFFFF'FFFFu;
	}

	DWORD result = 0;
	result |= ( (DWORD)dt[ 0 ].Data[ 0 ] <<  0 );
	result |= ( (DWORD)dt[ 0 ].Data[ 1 ] <<  8 );
	result |= ( (DWORD)dt[ 0 ].Data[ 2 ] << 16 );
	return result;
}


//�f�[�^�̏�������
bool mSCFelica::Write( const DataBlock& data , bool macauth )const
{
	//����̃u���b�N�����x���������Ƃ��Ă��Ȃ����`�F�b�N����
	//���[�U�[�u���b�N�ȊO���������Ƃ��Ă��Ȃ������`�F�b�N����
	{
		using Blocks = std::unordered_set<DWORD>;
		Blocks blocks;
		for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
		{
			if( blocks.count( itr->BlockNumber ) )
			{
				RaiseError( g_ErrorLogger , itr->BlockNumber , L"����u���b�N�𕡐��񏑍������Ƃ��܂���" );
				return false;
			}
			if( 0x0Eu < itr->BlockNumber )
			{
				RaiseError( g_ErrorLogger , itr->BlockNumber , L"���[�U�[�u���b�N�ȊO�����������Ƃ��܂���" );
				return false;
			}
			blocks.insert( itr->BlockNumber );
		}
	}

	//������������
	DWORD wcnt = ( macauth ) ? ( GetWCNT() ) : ( 0 );	//���݂�WCNT�l
	for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		bool result;
		if( macauth )
		{
			result = MacWriteInternal( *itr , wcnt );
			wcnt++;
		}
		else
		{
			DataBlock dt;
			dt.push_back( *itr );
			result = RawWrite( dt );
		}
		if( !result )
		{
			RaiseError( g_ErrorLogger , itr->BlockNumber , L"�������݂����s���܂���" );
			return false;
		}
	}
	return true;
}

bool mSCFelica::UpdateKey( const mSecureBinary& key , uint16_t keyver )
{
	//���̓��o
	mSecureBinary cardkey;
	if( key.size() == 16 )
	{
		//�J�[�h���Ȃ̂ł��̂܂܎g��
		cardkey = key;
	}
	else if( key.size() == 24 )
	{
		//�}�X�^�[���Ȃ̂ŃJ�[�h���𓱏o
		ReadRequestBlock idreq;
		DataBlock idrsp;
		idreq.push_back( 0x82u );
		if( !RawRead( idreq , idrsp ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"ID��ǂݎ��܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}
		if( !mSCFelicaMac::CalcDiversifiedKey( key , idrsp[ 0 ].Data , cardkey ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"�ʉ��J�[�h���𓱏o�ł��܂���" );
			MyFelicaMac.ClearKey();
			return false;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"���̃T�C�Y���Ⴂ�܂�" );
		MyFelicaMac.ClearKey();
		return false;
	}

	//�J�[�h���̏�������
	DWORD wcnt = GetWCNT();	//���݂�WCNT�l
	DataBlockEntry writedata;
	writedata.BlockNumber = 0x87;	//CK
	writedata.Data.assign( 16 , 0x00u );
	for( DWORD i = 0 ; i < 16 ; i++ )
	{
		writedata.Data[ i ] = cardkey[ i ];
	}
	if( !MacWriteInternal( writedata , wcnt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�L�[�̍X�V�����s���܂���" );
		writedata.Data.secure_erase();
		return false;
	}
	writedata.Data.secure_erase();

	//�V�����J�[�h���ŊO���F�؂�ʂ�
	if( !ExecAuthenticationInternal( cardkey , false ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�V�����L�[�ł̔F�؂��������܂���ł���" );
		return false;
	}

	//�J�[�h���o�[�W�����̏�������
	wcnt = GetWCNT();
	writedata.BlockNumber = 0x86;	//CKV
	writedata.Data.assign( 16 , 0x00u );
	writedata.Data[ 0 ] = (BYTE)( ( keyver >> 0 ) & 0xFFu );
	writedata.Data[ 1 ] = (BYTE)( ( keyver >> 8 ) & 0xFFu );

	if( !MacWriteInternal( writedata , wcnt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�L�[�o�[�W�����̍X�V�����s���܂���" );
		return false;
	}

	return true;
}

//DataBlockEntry��RegData�ɕϊ�
mSCFelica::RegData mSCFelica::DataBlockEntryToRegData( const DataBlockEntry& src )
{
	RegData result;
	int max_index;

	//REGA
	max_index = ( src.Data.size() < 4 ) ? ( (int)src.Data.size() ) : ( 4 );
	for( int i = max_index - 1 ; 0 <= i ; i-- )
	{
		result.RegA <<= 8;
		result.RegA  += src.Data[ i ];
	}
	//REGB
	max_index = ( src.Data.size() < 8 ) ? ( (int)src.Data.size() ) : ( 8 );
	for( int i = max_index - 1 ; 4 <= i ; i-- )
	{
		result.RegB <<= 8;
		result.RegB  += src.Data[ i ];
	}
	//REGC
	result.RegC.assign( 8 , 0 );
	max_index = ( src.Data.size() < 16 ) ? ( (int)src.Data.size() ) : ( 16 );
	for( int i = 8 ; i < max_index ; i++ )
	{
		result.RegC[ i - 8 ] = src.Data[ i ];
	}
	return result;
}

//DataBlockEntry��RegData�ɕϊ�
void mSCFelica::DataBlockEntryToRegData( const DataBlockEntry& src , RegData& retRegData )
{
	retRegData = DataBlockEntryToRegData( src );
}

//RegData��DataBlockEntry�ɕϊ�
mSCFelica::DataBlockEntry mSCFelica::RegDataToDataBlockEntry( const RegData& src )
{
	DataBlockEntry result;
	result.BlockNumber = 0x0Eu;

	result.Data.assign( 16 , 0 );
	result.Data[ 0 ] = (BYTE)( ( src.RegA >>  0 ) & 0xFFu );
	result.Data[ 1 ] = (BYTE)( ( src.RegA >>  8 ) & 0xFFu );
	result.Data[ 2 ] = (BYTE)( ( src.RegA >> 16 ) & 0xFFu );
	result.Data[ 3 ] = (BYTE)( ( src.RegA >> 24 ) & 0xFFu );
	result.Data[ 4 ] = (BYTE)( ( src.RegB >>  0 ) & 0xFFu );
	result.Data[ 5 ] = (BYTE)( ( src.RegB >>  8 ) & 0xFFu );
	result.Data[ 6 ] = (BYTE)( ( src.RegB >> 16 ) & 0xFFu );
	result.Data[ 7 ] = (BYTE)( ( src.RegB >> 24 ) & 0xFFu );
	
	DWORD max_index = ( 8 < src.RegC.size() ) ? ( 8 ) : ( (DWORD)src.RegC.size() );
	for( DWORD i = 0 ; i < max_index ; i++ )
	{
		result.Data[ i + 8 ] = src.RegC[ i ];
	}

	return result;
}

//RegData��DataBlockEntry�ɕϊ�
void mSCFelica::RegDataToDataBlockEntry( const RegData& src , DataBlockEntry& retDataBlockEntry )
{
	retDataBlockEntry = RegDataToDataBlockEntry( src );
}


