//----------------------------------------------------------------------------
// HTTP�A�N�Z�X
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019-2020 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mHttpAccess.h"
#include "General/mErrorLogger.h"

mHttpAccess::mHttpAccess()
{
	MySession = 0;
}

mHttpAccess::~mHttpAccess()
{
	if( MySession )
	{
		WinHttpCloseHandle( MySession );
		MySession = 0;
	}
}

bool mHttpAccess::Setup( const AccessOption& opt )
{
	const WCHAR* proxy_server;	
	const WCHAR* proxy_bypass;
	DWORD proxy_type;
	DWORD flag;

	//���[�U�G�[�W�F���g����ɂ���̂̓_��
	if( opt.UserAgent.empty() )
	{
		RaiseError( g_ErrorLogger , 0 , L"���[�U�[�G�[�W�F���g���󔒂ɂ��邱�Ƃ͂ł��܂���" );
		return false;
	}

	//�������[�h
	flag = WINHTTP_FLAG_ASYNC;

	//�v���L�V�̎g�p�E�s�g�p
	if( opt.OptionType == OptionType::NOPROXY )
	{
		//�v���L�V���g�p���Ȃ��ꍇ�B
		MyOption.reset( mNew AccessOption_NoProxy( *(AccessOption_NoProxy*)&opt ) );
		const AccessOption_NoProxy* p = (const AccessOption_NoProxy*)MyOption.get();

		proxy_type = WINHTTP_ACCESS_TYPE_NO_PROXY;
		proxy_server = WINHTTP_NO_PROXY_NAME;
		proxy_bypass = WINHTTP_NO_PROXY_BYPASS;
	}
	else if( opt.OptionType == OptionType::WITHPROXY )
	{
		//�v���L�V���g�p����ꍇ�B
		MyOption.reset( mNew AccessOption_WithProxy( *(AccessOption_WithProxy*)&opt ) );
		const AccessOption_WithProxy* p = (const AccessOption_WithProxy*)MyOption.get();

		if( p->ProxyServer == L"" )
		{
			proxy_type = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY;
			proxy_server = nullptr;
		}
		else
		{
			proxy_type = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxy_server = p->ProxyServer.c_str();
		}

		if( p->NoProxyList.size() )
		{
			//�v���L�V���O�T�[�o���w�肳��Ă���ꍇ
			proxy_bypass = p->NoProxyList.c_str();
		}
		else
		{
			//�S�ăv���L�V���g�p����ꍇ
			proxy_bypass = WINHTTP_NO_PROXY_BYPASS;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"�I�v�V�������ʎq���s���ł�" );
		return false;
	}

	MySession = WinHttpOpen( MyOption->UserAgent.c_str() , proxy_type , proxy_server , proxy_bypass , flag );
	if( !MySession )
	{
		RaiseError( g_ErrorLogger , 0 , L"�Z�b�V�����n���h���̍쐬�����s���܂���" );
		return false;
	}

	//HTTP/2���g�p����ꍇ�t���O���Z�b�g����
	if( opt.ProtocolVersion == OptionProtocolVersion::HTTP_VERSION_20 )
	{
		DWORD protocol_option = WINHTTP_PROTOCOL_FLAG_HTTP2;
		if( !WinHttpSetOption( MySession , WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL , &protocol_option , sizeof( protocol_option ) ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"HTTP�v���g�R���o�[�W������ύX�ł��܂���ł���" );
			return false;
		}
	}

	return true;
}

mHttpAccess::operator bool() const
{
	return MySession;
}

bool mHttpAccess::NewConnection( const ConnectionOption& info , mHttpConnection& retConn )
{
	//�Z�b�V�������܂��J���ĂȂ��ꍇ�͎��s
	if( !*this )
	{
		RaiseError( g_ErrorLogger , 0 , L"�Z�b�V�����n���h�����J���Ă��܂���" );
		return false;
	}

	//���łɊJ���Ă���ꍇ�͎��s
	if( retConn )
	{
		RaiseError( g_ErrorLogger , 0 , L"���łɃn���h�����J����Ă��܂�" );
		return false;
	}

	//URL�𕪉����āA���̒�����T�[�o���̕����𒊏o����
	URL_COMPONENTS url;
	ZeroMemory( &url , sizeof( URL_COMPONENTS ) );
	url.dwStructSize = sizeof( URL_COMPONENTS );
	url.dwHostNameLength = (DWORD)-1;
	if( WinHttpCrackUrl( info.ServerName.c_str() , (DWORD)info.ServerName.size() , 0 , &url ) )
	{
		retConn.MyServerAddress = WString( url.lpszHostName ).substr( 0 , url.dwHostNameLength );
	}
	else
	{
		retConn.MyServerAddress = info.ServerName;
	}

	//�|�[�g�̐ݒ�
	if( info.Port == 0 )
	{
		retConn.MyPort = INTERNET_DEFAULT_PORT;
	}
	else
	{
		retConn.MyPort = info.Port;
	}

	//�R�l�N�V�������J��
	retConn.MyConnection = WinHttpConnect( MySession , retConn.MyServerAddress.c_str() , retConn.MyPort , 0 );
	if( retConn.MyConnection == 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�R�l�N�V�����n���h���𐶐��ł��܂���" );
		return false;
	}

	return true;
}


