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


#ifndef MHTTPREQUEST_H_INCLUDED
#define MHTTPREQUEST_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mFileReadStreamBase.h"
#include "General/mFileWriteStreamBase.h"
#include "General/mCriticalSectionContainer.h"
#include "General/mDateTime.h"
#include "General/mNotifyOption.h"

#include <winhttp.h>

#pragma comment( lib , "winhttp.lib" )

//�Q�l�ɂȂ邩������Ȃ�URL
// http://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa384273(v=vs.85).aspx
// http://eternalwindows.jp/network/winhttp/winhttp01.html

namespace Definitions_HttpRequest
{
	//HTTP����
	enum RequestVerb
	{
		VERB_GET,			//GET���g�p����
		VERB_PUT,			//PUT���g�p����
		VERB_POST,			//POST���g�p����
		VERB_HEADER,		//HEADER���g�p����
	};

	enum HeaderFlag
	{
		ACCEPT = WINHTTP_QUERY_ACCEPT ,
		ACCEPT_CHARSET = WINHTTP_QUERY_ACCEPT_CHARSET ,
		ACCEPT_ENCODING = WINHTTP_QUERY_ACCEPT_ENCODING ,
		ACCEPT_LANGUAGE = WINHTTP_QUERY_ACCEPT_LANGUAGE ,
		ACCEPT_RANGES = WINHTTP_QUERY_ACCEPT_RANGES ,
		AGE = WINHTTP_QUERY_AGE ,
		ALLOW = WINHTTP_QUERY_ALLOW ,
		AUTHENTICATION_INFO = WINHTTP_QUERY_AUTHENTICATION_INFO ,
		AUTHORIZATION = WINHTTP_QUERY_AUTHORIZATION ,
		CACHE_CONTROL = WINHTTP_QUERY_CACHE_CONTROL ,
		CONNECTION = WINHTTP_QUERY_CONNECTION ,
		CONTENT_BASE = WINHTTP_QUERY_CONTENT_BASE ,
		CONTENT_ENCODING = WINHTTP_QUERY_CONTENT_ENCODING ,
		CONTENT_ID = WINHTTP_QUERY_CONTENT_ID ,
		CONTENT_LANGUAGE = WINHTTP_QUERY_CONTENT_LANGUAGE ,
		CONTENT_LENGTH = WINHTTP_QUERY_CONTENT_LENGTH ,
		CONTENT_LOCATION = WINHTTP_QUERY_CONTENT_LOCATION ,
		CONTENT_MD5 = WINHTTP_QUERY_CONTENT_MD5 ,
		CONTENT_RANGE = WINHTTP_QUERY_CONTENT_RANGE ,
		CONTENT_TRANSFER_ENCODING = WINHTTP_QUERY_CONTENT_TRANSFER_ENCODING ,
		CONTENT_TYPE = WINHTTP_QUERY_CONTENT_TYPE ,
		COOKIE = WINHTTP_QUERY_COOKIE ,
		DATE = WINHTTP_QUERY_DATE ,
		ETAG = WINHTTP_QUERY_ETAG ,
		EXPECT = WINHTTP_QUERY_EXPECT ,
		EXPIRES = WINHTTP_QUERY_EXPIRES ,
		FROM = WINHTTP_QUERY_FROM ,
		HOST = WINHTTP_QUERY_HOST ,
		IF_MATCH = WINHTTP_QUERY_IF_MATCH ,
		IF_MODIFIED_SINCE = WINHTTP_QUERY_IF_MODIFIED_SINCE ,
		IF_NONE_MATCH = WINHTTP_QUERY_IF_NONE_MATCH ,
		IF_RANGE = WINHTTP_QUERY_IF_RANGE ,
		IF_UNMODIFIED_SINCE = WINHTTP_QUERY_IF_UNMODIFIED_SINCE ,
		LAST_MODIFIED = WINHTTP_QUERY_LAST_MODIFIED ,
		LOCATION = WINHTTP_QUERY_LOCATION ,
		MAX_FORWARDS = WINHTTP_QUERY_MAX_FORWARDS ,
		MIME_VERSION = WINHTTP_QUERY_MIME_VERSION ,
		PRAGMA = WINHTTP_QUERY_PRAGMA ,
		PROXY_AUTHENTICATE = WINHTTP_QUERY_PROXY_AUTHENTICATE ,
		PROXY_AUTHORIZATION = WINHTTP_QUERY_PROXY_AUTHORIZATION ,
		PROXY_CONNECTION = WINHTTP_QUERY_PROXY_CONNECTION ,
		PROXY_SUPPORT = WINHTTP_QUERY_PROXY_SUPPORT ,
		PUBLIC = WINHTTP_QUERY_PUBLIC ,
		RANGE = WINHTTP_QUERY_RANGE ,
		RAW_HEADERS_CRLF = WINHTTP_QUERY_RAW_HEADERS_CRLF ,
		REFERER = WINHTTP_QUERY_REFERER ,
		REQUEST_METHOD = WINHTTP_QUERY_REQUEST_METHOD ,
		RETRY_AFTER = WINHTTP_QUERY_RETRY_AFTER ,
		SERVER = WINHTTP_QUERY_SERVER ,
		SET_COOKIE = WINHTTP_QUERY_SET_COOKIE ,
		STATUS_CODE = WINHTTP_QUERY_STATUS_CODE ,
		STATUS_TEXT = WINHTTP_QUERY_STATUS_TEXT ,
		TRANSFER_ENCODING = WINHTTP_QUERY_TRANSFER_ENCODING ,
		UNLESS_MODIFIED_SINCE = WINHTTP_QUERY_UNLESS_MODIFIED_SINCE ,
		UPGRADE = WINHTTP_QUERY_UPGRADE ,
		URI = WINHTTP_QUERY_URI ,
		USER_AGENT = WINHTTP_QUERY_USER_AGENT ,
		VARY = WINHTTP_QUERY_VARY ,
		VERSION = WINHTTP_QUERY_VERSION ,
		VIA = WINHTTP_QUERY_VIA ,
		WARNING = WINHTTP_QUERY_WARNING ,
		WWW_AUTHENTICATE = WINHTTP_QUERY_WWW_AUTHENTICATE
	};

	//���N�G�X�g�̏��
	enum RequestStatus
	{
		REQUEST_INITIALIZED,
		REQUEST_PREEXEC,		//�������ݎ�t��(���N�G�X�g���M�O)
		REQUEST_POSTEXEC,		//�������ݎ�t��(���N�G�X�g���M��)
		REQUEST_CLOSED,			//�������ݏI��
		REQUEST_RECEIVING,		//�����҂�
		REQUEST_COMPLETED,		//��M�I��
		REQUEST_FINISHED,		//�ʐM�I��
	};

	//�L���[�̏��
	enum BufferQueueStatus
	{
		BUFFERQUEUE_CREATED,	//���ꂽ�����ł܂����M�E��M�͍s���Ă��Ȃ�
		BUFFERQUEUE_PROCEEDING,	//�V�X�e�����ő��M�E��M�������s���Ă���
		BUFFERQUEUE_COMPLETED,	//���M�E��M�������I���A���ʂ̊m�F�҂�
		BUFFERQUEUE_ERROR,		//�������G���[�ɂȂ���
	};
};

class mHttpReadStream : public mFileReadStreamBase
{
public:
	//�ǂݎ�葤�̌o�H���J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mHttpWriteStream : public mFileWriteStreamBase
{
public:

	//�������ݑ��̌o�H���J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const
	{
		return IsWritable();
	}

protected:
	virtual bool IsWritable( void )const = 0;

};

class mHttpRequest : public mHttpReadStream , public mHttpWriteStream
{
public:
	mHttpRequest();
	virtual ~mHttpRequest();

	//-----------------------------------------------------------------------
	// �����ݒ�p�֐�
	//-----------------------------------------------------------------------

	//����
	using RequestVerb = Definitions_HttpRequest::RequestVerb;

	//�󂯕t���郁�f�B�A�^�C�v�̃��X�g
	using MediaTypeList = WStringDeque;

	//���N�G�X�g�ݒ�
	struct RequestOption
	{
		//-----------
		//����ݒ�
		//-----------
		RequestVerb	Verb;			//HTTP����
		bool Reload;				//true�̏ꍇ�����[�h����(�L���b�V������)
		bool Secure;				//true�̏ꍇSSL���g�p����
		WString Referer;			//���t�@���[
		WString Url;				//�擾�������I�u�W�F�N�g
		MediaTypeList AcceptType;	//�󂯕t���郁�f�B�A�^�C�v(�����w�肵�Ȃ��ƃe�L�X�g�̂݁B��Œǉ��E�X�V���\�B)

		//-----------
		//�o�b�t�@�ݒ�
		//-----------
		DWORD SendPacketSize;				//���M���Ɋm�ۂ���o�b�t�@�P������̃T�C�Y
		DWORD RecievePacketSize;			//��M���Ɋm�ۂ���o�b�t�@�P������̃T�C�Y
		DWORD RecievePacketMaxActive;		//��M���Ɋm�ۂ���o�b�t�@�̐�
		DWORD RecievePacketMaxStock;		//�����Ŏw�肵�����ȏ�Ɏ�M�ς݂Ŗ��ǂݎ��̃o�b�t�@�����������M���~����

		//-----------
		//�����l
		//-----------

		RequestOption()
		{
			Verb = RequestVerb::VERB_GET;
			Reload = true;
			Secure = false;
			SendPacketSize = 10240;
			RecievePacketSize = 10240;
			RecievePacketMaxActive = 3;
			RecievePacketMaxStock = 128;
		}
	};


	union NotifyFunctionOpt
	{
		struct OnSendOpt
		{
			DWORD BytesSent;
		}OnSend;

		//�R�[���o�b�N�ŃG���[�ʒm���󂯎�����ꍇ�̃G���[���
		struct OnErrorOpt
		{
			DWORD_PTR Api;
			DWORD ErrorCode;
		}OnError;

		//�R�[���o�b�N��SSL�G���[�ʒm���󂯎�����ꍇ�̃G���[���
		struct OnSslErrorOpt
		{
			bool IsCertRevocationFailed;	//�ؖ����̃`�F�b�N���̂��̂��ł��Ȃ�����(�ؖ������s���Ƃ͌���Ȃ�)
			bool IsInvalidCert;				//�ؖ������j�Z���m
			bool IsCertRevoked;				//�ؖ������������Ă�
			bool IsInvalidCA;				//�M���ς�CA���甭�s���ꂽ���̂ł͂Ȃ��A�܂��̓I���I���ؖ���
			bool IsInvalidCommonName;		//�ؖ�����URL����v���ĂȂ�
			bool IsInvalidDate;				//�ؖ����������؂�
			bool IsChannelError;			//���̑��悭�킩��Ȃ��G���[�B�Ⴆ��HTTP�̃|�[�g��HTTPS�łȂ����ꍇ�Ƃ��B
		}OnSslError;

		// struct OnCancelOpt
		// {
		// 	//no member yet
		// }OnCancel;

		// struct OnHeaderAvailableOpt
		// {
		// 	//no member yet
		// }OnHeaderAvailable;

		//static const DWORD OnHeaderAvailable = 0;

		struct OnReceiveOpt
		{
			DWORD BytesReceived;
		}OnReceive;
	};

	//�R�[���o�b�N�Œʒm����ꍇ�̃v���g�^�C�v
	//req : �R�[���o�b�N�𔭐��������I�u�W�F�N�g�̎Q��
	//parameter : �I�u�W�F�N�g�������ɓn�����C�ӂ̒l(NotifierInfo::Parameter�̒l)
	//opt : �g�����
	using NotifyFunction = void(*)( mHttpRequest& req , DWORD_PTR parameter , const NotifyFunctionOpt& opt );

	//�ʒm�ݒ�
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		//�f�[�^�̏������݂��s��ꂽ�ꍇ�̒ʒm
		NotifierInfo OnSend;
		//�G���[�����������ꍇ�̒ʒm
		NotifierInfo OnError;
		//�G���[�����������ꍇ�̒ʒm
		NotifierInfo OnSslError;
		//�������L�����Z�����ꂽ�ꍇ�̒ʒm
		NotifierInfo OnCancel;
		//�w�b�_�����p�\�ɂȂ����ꍇ�̒ʒm
		NotifierInfo OnHeaderAvailable;
		//�f�[�^����M�����ꍇ�̒ʒm
		NotifierInfo OnReceive;
	};

	//-----------------------------------------------------------------------
	// �e��I�y���[�V����
	//-----------------------------------------------------------------------

	//�ǉ�����w�b�_�̃��X�g
	bool SetHeader( const WString& header );

	//���N�G�X�g�̑��M
	// AdditionalPostDataSize �F �ǉ��̃f�[�^�T�C�Y
	//  ��Execute���s�ȍ~�ɁAPost���ő��M����f�[�^�̃T�C�Y�i�s��ɂ������ŕύX�����肷�邱�Ƃ͂ł��܂���j
	//  ��GET�ȂǁA���͂���f�[�^���Ȃ����N�G�X�g�Ȃ̂�0�ȊO���w�肵����G���[
	// ret : �������^
	bool Execute( DWORD AdditionalDataSize );

	//�ǂݍ��ݗp�̓����o�b�t�@���m�ۂ��܂�
	//�Վ��Ƀo�b�t�@���K�v�ɂȂ�Ƃ��Ɏg�p���܂�
	// count : �����o�b�t�@���w�肵���������ł���΁A���̐��ɂȂ�悤�ɓ����o�b�t�@��V���ɍ쐬���܂�
	bool PrepareReadBuffer( DWORD count );

	//�n���h������������Ă��邩��Ԃ�
	operator bool() const;

	//-----------------------------------------------------------------------
	// �Z�L�����e�B�ݒ�
	//-----------------------------------------------------------------------

	struct SslIgnoreErrors
	{
		bool DisableRevocation;				//�^�̏ꍇ�A���������ؖ����̃`�F�b�N�����Ȃ�
	#ifdef WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE
		bool IgnoreCertRevocationFailed;	//�^�̏ꍇ�A�ؖ����̃`�F�b�N���̂��̂��ł��Ȃ������ꍇ��OK
	#endif
		bool IgnoreInvalidCA;				//�^�̏ꍇ�A�M���ς�CA���甭�s���ꂽ���̂ł͂Ȃ��A�܂��̓I���I���ؖ����ł�OK
		bool IgnoreInvalidCommonName;		//�^�̏ꍇ�A�ؖ�����URL����v���ĂȂ��Ă�OK
		bool IgnoreInvalidDate;				//�^�̏ꍇ�A�ؖ����������؂�ł�OK

		SslIgnoreErrors()
		{
			DisableRevocation = false;
		#ifdef WINHTTP_OPTION_IGNORE_CERT_REVOCATION_OFFLINE
			IgnoreCertRevocationFailed = false;
		#endif
			IgnoreInvalidCA = false;
			IgnoreInvalidCommonName = false;
			IgnoreInvalidDate = false;
		}
	};

	//SSL�̏ؖ������؃G���[�ɂȂ��Ă���������ݒ������
	// opt : ��������G���[�̎�ނ������\����
	// ret : �������^
	bool SetSslIgnoreErrors( const SslIgnoreErrors opt );

	//-----------------------------------------------------------------------
	// �w�b�_�ƃX�e�[�^�X�R�[�h
	//-----------------------------------------------------------------------

	//�w�b�_�E�X�e�[�^�X�R�[�h���擾�ł��邩�𒲂ׂ�
	// ret : �^�Ȃ�Ύ擾�ł���
	bool IsHeaderAvailable( void )const;

	//�X�e�[�^�X�R�[�h�𓾂܂�
	// ret : �X�e�[�^�X�R�[�h�B�G���[����0
	DWORD GetStatusCode( void )const;

	//���X�|���X�̃w�b�_���
	using HeaderFlag = Definitions_HttpRequest::HeaderFlag;

	//���X�|���X�̃w�b�_�𓾂܂�
	// flag : �������w�b�_
	// ret : ����ꂽ���ʁB�G���[�̏ꍇ��errstr�Ɏw�肵��������
	WString QueryHeaderDirect( HeaderFlag flag , const WString& errstr )const;

	//���X�|���X�̃w�b�_�𓾂܂�
	// flag : �������w�b�_
	// retHeader : ����ꂽ���ʁB�G���[�̏ꍇ�͋󕶎���B
	// ret : �������^
	bool QueryHeader( HeaderFlag flag , WString& retHeader )const;

	//���X�|���X�̃w�b�_�𐔒l�œ��܂�
	// flag : �������w�b�_
	// ret : ����ꂽ���ʁB�G���[�̏ꍇ��errval�Ɏw�肵���l
	DWORD QueryHeaderDirect( HeaderFlag flag , DWORD errval )const;

	//���X�|���X�̃w�b�_�𐔒l�œ��܂�
	// flag : �������w�b�_
	// retHeader : ����ꂽ���ʁB�G���[�̏ꍇ��0�B
	// ret : �������^
	bool QueryHeader( HeaderFlag flag , DWORD& retHeader )const;

	//���X�|���X�̃w�b�_�𐔒l�œ��܂�
	// flag : �������w�b�_
	// ret : ����ꂽ���ʁB�G���[�̏ꍇ��errval�Ɏw�肵���l
	uint64_t QueryHeaderDirect( HeaderFlag flag , uint64_t errval )const;

	//���X�|���X�̃w�b�_�𐔒l�œ��܂�
	// flag : �������w�b�_
	// retHeader : ����ꂽ���ʁB�G���[�̏ꍇ��0�B
	// ret : �������^
	bool QueryHeader( HeaderFlag flag , uint64_t& retHeader )const;

	//���X�|���X�̃w�b�_�������œ��܂�
	// flag : �������w�b�_
	// retHeader : ����ꂽ���ʁB�G���[�̏ꍇ�͕s��B
	// ret : �������^
	bool QueryHeader( HeaderFlag flag , mDateTime::Timestamp& retHeader )const;

	//HTTP Version2�ŒʐM���Ă���ΐ^��Ԃ�
	// ret : HTTP2�ł���ΐ^�A�����łȂ��A�܂��̓G���[�̏ꍇ�͋U
	bool IsHttp2( void )const;

	//�Í����L�[�̒����𓾂�
	// ret : �Í����L�[�̃r�b�g���B�Í�������Ă��Ȃ����G���[����0
	DWORD GetEncryptionKeyLength( void )const;

	//-----------------------------------------------------------------------
	// �X�g���[���������݌n
	//-----------------------------------------------------------------------

	//�P������������
	// data : ���M����f�[�^(1�o�C�g)
	// ret : �������^
	//�EExec()�Ăяo���O�ɍs�����������݂́AExec()�Ăяo����ɏ������M���܂�
	//  ���̏ꍇ�AExec()�ɓn�����M�T�C�Y�̂��Ƃ͍l���Ȃ��Ă��ǂ��ł���(�����v�Z����܂�)�A
	//  ���M�����܂Ńf�[�^�̃R�s�[��ێ����邽�߁A�傫�ȃf�[�^�������ꍇ�́A��������Q��܂��B
	//�EExec()�Ăяo����ɍs�����������݂́A�������M���܂�
	//  ���̏ꍇ�AExec()�ɓn�������M�T�C�Y�Ƃ҂����蓯���T�C�Y�̃f�[�^��^���Ȃ���΂Ȃ�܂��񂪁A
	//  �������������������̂ŁA�������ɂ͗D�����Ȃ�܂��B
	//�EExec()�Ăяo���O��Write�ƁA�Ăяo�����Write�͕��p����OK
	//�EGET�ȂǁA���͂���f�[�^���Ȃ����N�G�X�g�Ȃ̂ɂ��̃��\�b�h���Ă񂾂�G���[
	virtual bool Write( INT data );

	//�L���b�V������������
	// ��GET�ȂǁA���͂���f�[�^���Ȃ����N�G�X�g�Ȃ̂ɂ��̃��\�b�h���Ă񂾂�G���[
	// ret : �������^
	virtual bool FlushCache( void );

	//�������݉\����Ԃ�
	// ret : �������݉\�ł���ΐ^
	virtual bool IsWritable( void )const;

	//-----------------------------------------------------------------------
	// �X�g���[���ǂݍ��݌n
	//-----------------------------------------------------------------------

	//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
	virtual INT Read( void );

	//EOF�ɒB���Ă��邩�𒲂ׂ܂�
	virtual bool IsEOF( void )const;

private:
	mHttpRequest( const mHttpRequest& source ) = delete;
	const mHttpRequest& operator=( const mHttpRequest& source ) = delete;

	friend class mHttpConnection;

protected:

	//------------------------
	// �n���h���Ə�ԕϐ��֌W
	//------------------------

	//���N�G�X�g�̃n���h��
	HINTERNET MyRequest;

	//�N���e�B�J���Z�N�V����
	mutable mCriticalSectionContainer MyCritical;

	//�������̃I�v�V����
	RequestOption MyOption;

	//�ʒm�I�v�V����
	NotifyOption MyNotifyOption;

	//�������
	using RequestStatus = Definitions_HttpRequest::RequestStatus;
	RequestStatus MyRequestStatus;

	//���X�|���X�̃w�b�_�[��ǂݎ��\���ǂ���
	bool MyIsResponseHeaderAvailable;

	//------------------------
	// �L���[�֌W
	//------------------------

	//�L���[�̏��
	using BufferQueueStatus = Definitions_HttpRequest::BufferQueueStatus;

	//�L���[
	struct BufferQueueEntry
	{
		//����M����f�[�^
		BYTE* Buffer;

		//�f�[�^�̃T�C�Y
		DWORD Size;

		//�L���[�̏��
		BufferQueueStatus Status;

		//�������̃G���[�R�[�h(IO�������ɐݒ�)
		DWORD ErrorCode;

		//�������̏����ς݃o�C�g��(IO�������ɐݒ�)
		DWORD BytesTransfered;
	};

	typedef std::deque<BufferQueueEntry> BufferQueue;

	//���C�g�o�b�t�@�i���[�U�[�v���O�����̏������ݒ��E�����M�j
	BufferQueue MyWriteQueue;

	//���C�g�o�b�t�@�i���M���j
	BufferQueue MyWriteActiveQueue;

	//���[�h�o�b�t�@�i��M���j
	BufferQueue MyReadActiveQueue;

	//���[�h�o�b�t�@�i��M�ς݁E���[�U�[�v���O�����̓ǂݎ��҂��j
	BufferQueue MyReadQueue;

	//WinHttp����̃R�[���o�b�N�֐�
	static void __stdcall WinhttpStatusCallback(
		IN HINTERNET hInternet ,
		IN DWORD_PTR dwContext ,
		IN DWORD dwInternetStatus ,
		IN LPVOID lpvStatusInformation ,
		IN DWORD dwStatusInformationLength
	);

	//�L���[�̑��M���s��
	// ret : �������^
	bool ExecWriteQueue( void );

	//WinHTTP����̃R�[���o�b�N���瓾���������ݍς݃T�C�Y�����ɁA���������o�b�t�@���p�[�W����
	// written_size : WinHTTP���瓾���������݊��������o�C�g��
	// ret : �������^
	bool CompleteWriteQueue( DWORD written_size );

	//���BufferQueueEntry������������
	// buffer : WinHTTP�̃R�[���o�b�N����n���ꂽ�������݂̎�M�o�b�t�@
	// read_size : WinHTTP�̃R�[���o�b�N����n���ꂽ�ǂݎ�����T�C�Y
	// ret : �V���Ƀ��[�U�[�Ɉ����n���\�ȃf�[�^�����������ꍇ��true
	// ����������BufferQueueEntry���L���[�̐擪�ł����true
	//   �擪�łȂ��ꍇ�ł��A�擪�̃L���[�������ς݂ɂȂ��Ă���ꍇ(�C���M�����[)��true
	//   ����ȊO��false
	bool CompleteReadQueue( BYTE* buffer , DWORD read_size );

	//------------------------
	// ���̑��̃I�y���[�V����
	//------------------------

	//��M���J�n����
	// ret : �������^
	bool StartReceiveResponse( void );

	//�������݂̊���
	//HTTP�ł͕s�v
	virtual bool Close( void );

};

#endif //MHTTPACCESS_H_INCLUDED
