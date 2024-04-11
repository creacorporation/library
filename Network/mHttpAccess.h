//----------------------------------------------------------------------------
// HTTP�A�N�Z�X
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------


#ifndef MHTTPACCESS_H_INCLUDED
#define MHTTPACCESS_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mHttpConnection.h"
#include "mHttpRequest.h"
#include <winhttp.h>
#include <memory>

#pragma comment( lib , "winhttp.lib" )

//����
//�EPHP�Ōx���E�G���[���ł�ƁAWinHTTP�����G���[�ɂȂ邱�Ƃ�����
//�@- ERROR_WINHTTP_INVALID_SERVER_RESPONSE���Ԃ��Ă���
//  - PHP���Ŗ���`�ϐ����Q�Ƃ��Čx�����o���ꍇ�Ȃ�
//�@- HTTPd�̃X�e�[�^�X�R�[�h��200�ɂȂ��Ă���̂ł킩��ɂ���
//�E�Q�l�ɂȂ邩������Ȃ�URL
// http://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa384273(v=vs.85).aspx
// http://eternalwindows.jp/network/winhttp/winhttp01.html

namespace Definitions_HttpAccess
{
	enum OptionType
	{
		NOPROXY,
		WITHPROXY
	};

	enum OptionProtocolVersion
	{
		//HTTP1.1���g�p����
		HTTP_VERSION_11,
		//HTTP2.0���g�p����
		//�Ȃ��AmHttpRequest::QueryHeader()���g����HTTP�o�[�W������₢���킹��ƁA
		//���ۂɂ�HTTP2.0�ŒʐM���Ă��Ă�1.1�ƕ񍐂��Ă��邱�Ƃ�������ۂ��̂�IsHttp2()�Ŋm�F���邱�ƁB
		HTTP_VERSION_20,
	};
};

class mHttpAccess
{
public:

	mHttpAccess();
	virtual ~mHttpAccess();

	//-----------------------------------------------------------------------
	// �����ݒ�
	//-----------------------------------------------------------------------

	using OptionType = Definitions_HttpAccess::OptionType;
	using OptionProtocolVersion = Definitions_HttpAccess::OptionProtocolVersion;
	struct AccessOption
	{
		const Definitions_HttpAccess::OptionType OptionType;
	protected:
		AccessOption() = delete;
		AccessOption( Definitions_HttpAccess::OptionType type ) : OptionType( type )
		{
			ProtocolVersion = OptionProtocolVersion::HTTP_VERSION_11;
			UserAgent = L"";
		}
	public:
		OptionProtocolVersion ProtocolVersion;	//�g�p����v���g�R���o�[�W����
		WString UserAgent;						//���[�U�G�[�W�F���g
	};

	//�v���L�V���g�p���Ȃ��ꍇ�̃I�v�V����
	struct AccessOption_NoProxy : public AccessOption
	{
		AccessOption_NoProxy() : AccessOption( Definitions_HttpAccess::OptionType::NOPROXY )
		{
		}
	};

	//�v���L�V���g�p����ꍇ�̃I�v�V����
	struct AccessOption_WithProxy : public AccessOption
	{
		AccessOption_WithProxy() : AccessOption( Definitions_HttpAccess::OptionType::WITHPROXY )
		{
			ProxyServer = L"";
			NoProxyList = L"";
		}
		WString ProxyServer;		//�g�p����v���L�V�T�[�o(�󕶎���Ńf�t�H���g)
		WString NoProxyList;		//�v���L�V���g�p���Ȃ�URL(�Z�~�R������؂�)
	};

	//�����ݒ�
	bool Setup( const AccessOption& opt );

	//�Z�b�V�����n���h������������Ă��邩��Ԃ�
	operator bool() const;

	//-----------------------------------------------------------------------
	// �ڑ��̐���
	//-----------------------------------------------------------------------

	using ConnectionOption = mHttpConnection::ConnectionOption;

	//�V�����ڑ��𐶐�����
	//info : �ڑ�����
	//retConn : �ڑ��������ʓ���ꂽ�n���h��
	//ret : �ڑ��������^
	bool NewConnection( const ConnectionOption& info , mHttpConnection& retConn );

private:

	mHttpAccess( const mHttpAccess& source ) = delete;
	const mHttpAccess& operator=( const mHttpAccess& source ) = delete;

protected:

	//�n���h��
	HINTERNET MySession;

	std::unique_ptr< AccessOption > MyOption;

};

//�g�p��
#if 0

#include <mStandard.h>
#include <Network/mHttpAccess.h>

int main( int argc , char** argv )
{
	InitializeLibrary();

	mHttpAccess http;

	//WinHTTP�̃Z�b�V�����I�u�W�F�N�g���쐬
	mHttpAccess::AccessOption_NoProxy accessopt;
	accessopt.UserAgent = L"TEST";
	accessopt.ProtocolVersion = mHttpAccess::OptionProtocolVersion::HTTP_VERSION_11;

	if( !http.Setup( accessopt ) )
	{
		return 1;
	}

	//�Z�b�V�����I�u�W�F�N�g�ɐڑ�����i�A�h���X�ƃ|�[�g�j��n���Đڑ��I�u�W�F�N�g���쐬
	mHttpConnection conn;
	mHttpConnection::ConnectionOption connopt;
	connopt.ServerName = L"https://192.168.0.1/index.php";
	connopt.Port = 443;

	if( !http.NewConnection( connopt , conn ) )
	{
		return 2;
	}

	//�ڑ��I�u�W�F�N�g�Ƀ��N�G�X�g���i�������URL���j��n���ă��N�G�X�g�I�u�W�F�N�g���쐬
	mHttpRequest req;
	mHttpRequest::RequestOption reqopt;
	reqopt.Url = L"https://192.168.0.1/index.php";
	reqopt.Verb = mHttpRequest::RequestVerb::VERB_GET;
	reqopt.Secure = true;	//https�̏ꍇtrue

	if( !conn.NewRequest( reqopt , req ) )
	{
		return 3;
	}

	//SSL�G���[�𖳎�����ꍇ�͈ȉ��̂悤�Ȑݒ�������
	//mHttpRequest::SslIgnoreErrors sslerr;
	//sslerr.IgnoreInvalidCA = true;
	//sslerr.IgnoreInvalidCommonName = true;
	//if( !req.SetSslIgnoreErrors( sslerr ) )
	//{
	//	return 5;
	//}

	//���N�G�X�g���M�O�ɁA���炩����POST����f�[�^�Ƃ����������ނ��Ƃ��ł���
	//���炩���ߏ�������ł����΁A���N�G�X�g���M���ɏ���ɑ��M����B
	//���炩���ߏ������ރf�[�^�ɂ��ẮAExecute()�̈����Ɏw�肷��f�[�^�T�C�Y�Ɋ܂߂Ȃ��ėǂ�
	//req.WriteString( "value1=123\r\n" );
	//req.WriteString( "value2=456\r\n" );
	//req.WriteString( "value3=789\r\n" );

	//���N�G�X�g�𑗐M
	if( !req.Execute( 0 ) )
	{
		return 4;
	}

	//���N�G�X�g���M��ɁAPOST����f�[�^���������ނ��Ƃ��ł���
	//�������A���̏ꍇ��Execute()�ɏ������ރo�C�g���𐳊m�Ɏw�肷��K�v������
	//�����ς�t�@�C����o�C�i���f�[�^�̑��M�Ɏg��
	//BYTE data[ 1024 ];
	//req.WriteBinary( data , sizeof( data ) );

	//���X�|���X�҂�
	do
	{
		if( req.IsEOF() )
		{
			//�w�b�_������O��IsEOF()���^�ɂȂ�����ʐM�G���[
			printf( "�ʐM�G���[\n" );
			return 6;
		}
		SleepEx( 1000 , true );
	} while( !req.IsHeaderAvailable() );

	//�X�e�[�^�X�R�[�h���擾
	DWORD status_code = req.GetStatusCode();

	//���ʎ擾���R���\�[���ɏo��
	while( !req.IsEOF() )
	{
		AString str;
		if( !req.ReadLine( str ) )
		{
			continue;
		}

		printf( str.c_str() );
		printf( "\n" );
	}


	return 0;
}
#endif

#endif //MHTTPACCESS_H_INCLUDED
