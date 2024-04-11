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
#include "mHttpConnection.h"
#include "General/mErrorLogger.h"
#include <vector>

mHttpConnection::mHttpConnection()
{
	MyConnection = 0;
	MyPort = 0;
	MyServerAddress = L"";
}

mHttpConnection::~mHttpConnection()
{
	if( MyConnection )
	{
		WinHttpCloseHandle( MyConnection );
		MyConnection = 0;
	}
}

class MediaTypeArray final
{
public:
	MediaTypeArray( const mHttpRequest::MediaTypeList& ls )
	{
		if( ls.empty() )
		{
			return;
		}

		size_t size = ls.size();
		MyPtr.resize( size + 1 );
		MyPtr[ size ] = 0;

		mHttpRequest::MediaTypeList::const_iterator itr = ls.begin();
		DWORD index = 0;
		while( itr != ls.end() )
		{
			MyPtr[ index ] = mNew WCHAR[ itr->size() + 1 ];
			wchar_strcpy( const_cast<WCHAR*>( MyPtr[ index ] ) , itr->c_str() );
		}
	}
	~MediaTypeArray()
	{
		for( std::vector<WCHAR*>::iterator itr = MyPtr.begin() ; itr != MyPtr.end() ; itr++ )
		{
			mDelete[] *itr;
			*itr = nullptr;
		}
	}
	operator const WCHAR**()
	{
		if( MyPtr.empty() )
		{
			return WINHTTP_DEFAULT_ACCEPT_TYPES;
		}
		return (const WCHAR**)( MyPtr.data() );
	};
private:
	std::vector<WCHAR*> MyPtr;
};

bool mHttpConnection::NewRequest( const RequestOption& opt , const NotifyOption& notifier , mHttpRequest& retReq )
{
	//�Z�b�V�������܂��J���ĂȂ��ꍇ�͎��s
	if( !*this )
	{
		RaiseError( g_ErrorLogger , 0 , L"�R�l�N�V�����n���h�����J���Ă��܂���" );
		return false;
	}

	//���łɊJ���Ă���ꍇ�͎��s
	if( retReq )
	{
		RaiseError( g_ErrorLogger , 0 , L"���łɃn���h�����J����Ă��܂�" );
		return false;
	}

	//�ݒ���̃`�F�b�N
	if( ( opt.RecievePacketMaxActive < 1 ) ||
		( opt.RecievePacketMaxStock < 1 ) ||
		( opt.RecievePacketSize < 1 ) ||
		( opt.SendPacketSize < 1 ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�o�b�t�@�T�C�Y�̎w�肪�s���ł�" );
		return false;
	}

	//�ݒ���̃R�s�[
	retReq.MyOption = opt;
	retReq.MyNotifyOption = notifier;

	//���N�G�X�g���J��
	//����
	const WCHAR* verb;
	switch( opt.Verb )
	{
	case mHttpRequest::RequestVerb::VERB_GET:
		verb = L"GET";
		break;
	case mHttpRequest::RequestVerb::VERB_PUT:
		verb = L"PUT";
		break;
	case mHttpRequest::RequestVerb::VERB_POST:
		verb = L"POST";
		break;
	case mHttpRequest::RequestVerb::VERB_HEADER:
		verb = L"HEADER";
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"�������s���ł�" );
		return false;
	}

	//�o�[�W����
	const WCHAR* version = nullptr;	//�f�t�H���g�ɂ���(1.1 or 2.0)

	//URL�p�X
	URL_COMPONENTS url;
	const WCHAR* urlpath;
	ZeroMemory( &url , sizeof( URL_COMPONENTS ) );
	url.dwStructSize = sizeof( URL_COMPONENTS );
	url.dwUrlPathLength = 1;
	if( WinHttpCrackUrl( opt.Url.c_str() , (DWORD)opt.Url.size() , 0 , &url ) )
	{
		urlpath = url.lpszUrlPath;
	}
	else
	{
		urlpath = opt.Url.c_str();
	}
	
	//���f�B�A�^�C�v
	MediaTypeArray mediatype( opt.AcceptType );

	//���t�@��
	const WCHAR* ref;
	if( opt.Referer.empty() )
	{
		ref = WINHTTP_NO_REFERER;
	}
	else
	{
		ref = opt.Referer.c_str();
	}

	//�t���O�֌W
	DWORD flags = 0;
	if( opt.Reload )
	{
		flags |= WINHTTP_FLAG_REFRESH;
	}
	if( opt.Secure )
	{
		flags |= WINHTTP_FLAG_SECURE;
	}

	//���N�G�X�g����
	retReq.MyRequest = WinHttpOpenRequest( MyConnection , verb , urlpath , version , ref , mediatype , flags );
	if( retReq.MyRequest == 0 )
	{
		return false;
	}
	
	//�R�[���o�b�N�֐����C���X�g�[������
	DWORD notifyflag = WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS;
	// = ( WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE  \
	//	 | WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE     \
	//	 | WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE        \
	//	 | WINHTTP_CALLBACK_STATUS_READ_COMPLETE         \
	//	 | WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE        \
	//	 | WINHTTP_CALLBACK_STATUS_REQUEST_ERROR         \
	//	 | WINHTTP_CALLBACK_STATUS_GETPROXYFORURL_COMPLETE )
	notifyflag |= WINHTTP_CALLBACK_FLAG_SECURE_FAILURE;			//SSL�����s�����Ƃ�
	notifyflag |= WINHTTP_CALLBACK_FLAG_REDIRECT;				//���_�C���N�g���ꂽ�Ƃ�
	//notifyflag |= WINHTTP_CALLBACK_FLAG_CLOSE_CONNECTION;		//�ڑ�������ꂽ�Ƃ�

	if( WinHttpSetStatusCallback( retReq.MyRequest , mHttpRequest::WinhttpStatusCallback , notifyflag , 0 ) == WINHTTP_INVALID_STATUS_CALLBACK )
	{
		RaiseError( g_ErrorLogger , 0 , L"�R�[���o�b�N�֐����C���X�g�[���ł��܂���ł���" );
		return false;
	}

	//�����o�ϐ����Z�b�g
	retReq.MyRequestStatus = mHttpRequest::RequestStatus::REQUEST_PREEXEC;

	return true;
}

mHttpConnection::operator bool() const
{
	return MyConnection;
}


