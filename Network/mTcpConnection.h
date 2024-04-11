//----------------------------------------------------------------------------
// TCP�n���h��
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MTCPCONNECTION_H_INCLUDED
#define MTCPCONNECTION_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"
#include <memory>

namespace Definitions_TcpConnection
{
	//�ʒm���[�h
	enum NotifyMode
	{
		NOTIFY_NONE ,
		NOTIFY_WINDOWMESSAGE ,
		NOTIFY_SIGNAL ,
		NOTIFY_CALLBACK ,
	};

	//�G���[�����������^�C�~���O
	enum ErrorAction
	{
		ERROR_ON_DNSRESOLVE,	//���O�������̃G���[
		ERROR_ON_CONNECT,		//�ڑ��������̃G���[
		ERROR_ON_READ,			//�ǂݍ��ݒ��̃G���[
		ERROR_ON_WRITE,			//�������ݒ��̃G���[
	};
	//IPv6�Ή�
	enum IPv6Support
	{
		IPv6_Required,			//IPv6���w��
		IPv6_Auto,				//IPv6���g����Ƃ���IPv6
		IPv4_Required,			//IPv4���w��
	};
};

class mTcpReadStream : public mFileReadStreamBase
{
public:
	//�ǂݎ�葤�̌o�H���J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mTcpWriteStream : public mFileWriteStreamBase
{
public:
	mTcpWriteStream()
	{
		MyIsClosed = false;
	}

	//�������ݑ��̌o�H���J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const
	{
		return !MyIsClosed;
	}

private:
	mTcpWriteStream( const mTcpWriteStream& src ) = delete;
	const mTcpWriteStream& operator=( const mTcpWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

class mTcpConnection : public mTcpReadStream , public mTcpWriteStream
{
public:
	mTcpConnection();
	virtual ~mTcpConnection();

	//�G���[�������̗��R�R�[�h
	enum ErrorCode
	{
	};

	//�ʒm�I�v�V����
	union NotifyFunctionOpt
	{
		struct OnConnectOpt
		{
		}OnConnect;

		struct OnReadOpt
		{
		}OnRead;

		struct OnWriteOpt
		{
		}OnWrite;

		struct OnErrorOpt
		{
			using ErrorAction = Definitions_TcpConnection::ErrorAction;
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//�ʒm�ݒ�
	using NotifyFunction = void(*)( mTcpConnection& conn , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		NotifierInfo OnConnect;
		NotifierInfo OnRead;
		NotifierInfo OnWrite;
		NotifierInfo OnError;
	};

	//�I�v�V�����\����
	struct ConnectionOption
	{
	public:
		using IPv6Support = Definitions_TcpConnection::IPv6Support;

		//-----------
		//����M�ݒ�
		//-----------
		IPv6Support IPv6;				//IPv6�Ή�
		WString HostName;				//�����[�g�̃z�X�g��
		WString Port;					//�|�[�g�ԍ�
		DWORD ConnectTimeout;			//�ڑ����̃^�C���A�E�g�i�~���b�P�ʁj

		DWORD ReadPacketSize;			//�ǂݍ��݃p�P�b�g�̃T�C�Y
		DWORD ReadPacketCount;			//�ǂݍ��݃p�P�b�g���m�ۂ��鐔
		DWORD WritePacketSize;			//�������݃p�P�b�g�̃T�C�Y
		DWORD WritePacketNotifyCount;	//�������ݑ҂��p�P�b�g�������Ŏw�肵��������������ꍇ�ɒʒm����
		DWORD WritePacketLimit;			//�������ݑ҂��p�P�b�g�̐��̏���i������Ə������݃G���[�j
		DWORD ReadBufferTimeout;		//�C�ӂ̃o�C�g�̎�M�Ԋu(�~���b)�����̒l�����������M�ʒm�𐶐�����

		//-----------
		//�����l
		//-----------
		ConnectionOption()
		{
			IPv6 = IPv6Support::IPv6_Auto;
			HostName = L"";
			Port = L"";

			ReadPacketSize = 1500;
			ReadPacketCount = 128;
			WritePacketSize = 1500;
			WritePacketNotifyCount = 8;
			WritePacketLimit = 512;
			ReadBufferTimeout = 100;
		}
	};

	//�w��̃T�[�o�[�ɐڑ�����
	// wtp : �o�^��̃��[�J�[�X���b�h�v�[��
	// opt : �ڑ��I�v�V����
	// notifier : �ʒm�I�v�V����
	bool Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier );

	//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
	//ret : �ǂݎ��������
	//EOF�͌��ݓǂݎ���f�[�^���Ȃ����Ƃ������܂�
	//�i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
	virtual INT Read( void );

	//EOF�ɒB���Ă��邩�𒲂ׂ܂�
	//�ESetEOF()���R�[����A���̎��_�܂łɎ�M�ς݂̃f�[�^��S�ēǂݏo����true�ɂȂ�܂�
	virtual bool IsEOF( void )const;

	//�P������������
	virtual bool Write( INT data );

	//�L���b�V������������
	//������Ă΂Ȃ��Ǝ��ۂ̑��M�͔������܂���
	virtual bool FlushCache( void );

	//�������ݑ��̌o�H����܂�
	virtual bool Close( void );

	//�ǂݍ��ݑ��̌o�H����܂�
	virtual bool SetEOF( void );

	//���M�������̃f�[�^�����邩��Ԃ��܂�
	// ret : ���M�������̃f�[�^�̐�(�L���[�̃G���g���P��)
	DWORD IsWriting( void )const;

	//���M�������̃f�[�^��j�����܂�
	bool Cancel( void );

	//���ݖ������̒ʐM(����M�Ƃ�)��S�Ĕj�����A�ڑ�����܂�
	//�ڑ��҂��̏ꍇ�͑ҋ@���L�����Z�����܂�
	bool Abort( void );

	//�ڑ����Ă��邩�ۂ���Ԃ��܂�
	//���p�C�v�̐�ɒN���Ȃ����Ă���ΐ^
	bool IsConnected( void )const;

	//�ǂݍ��ݗp�̓����o�b�t�@���m�ۂ��܂�
	//�Վ��Ƀo�b�t�@���K�v�ɂȂ�Ƃ��Ɏg�p���܂�
	// count : �����o�b�t�@���w�肵���������ł���΁A���̐��ɂȂ�悤�ɓ����o�b�t�@��V���ɍ쐬���܂�
	bool PrepareReadBuffer( DWORD count );

private:

	mTcpConnection( const mTcpConnection& src ) = delete;
	const mTcpConnection& operator=( const mTcpConnection& src ) = delete;

	//Winsock�̏�����������
	static bool SetupWinsock( void );

	//Winsock�̃N���[���A�b�v������
	static bool CleanupWinsock( void );

	//�ڑ�����
	// pool : �Ăяo����WTP�̎Q��
	// param1 : 0
	// param2 : this�|�C���^
	static void ConnectTask( mWorkerThreadPool& pool , DWORD Param1 , DWORD_PTR Param2 );

protected:

	//�g�p���̃C���X�^���X�̐�
	static LONG MySocketCount;

	//Winsock�̎����ڍ׃f�[�^
	static WSAData MyWSAData;

	//�ڑ��̃n���h��
	SOCKET MySocket;

	//�ڑ��ς݂��H
	bool MyIsConnected;

	//�ݒ�l
	ConnectionOption MyOption;

	//�ʒm�ݒ�l
	NotifyOption MyNotifyOption;

	//�N���e�B�J���Z�N�V����
	mutable mCriticalSectionContainer MyCritical;

	//Notify�Ăяo�����̃C�x���g��
	using NotifyEventToken = std::shared_ptr<int>;
	NotifyEventToken MyNotifyEventToken;

	//�������e�t���O
	enum QueueType
	{
		CONNECT_QUEUE_ENTRY,
		WRITE_QUEUE_ENTRY,
		READ_QUEUE_ENTRY
	};

	//�L���[
	struct BufferQueueEntry
	{
		//�e�I�u�W�F�N�g�ւ̃|�C���^
		//�������A�񓯊�����̊������_�Őe�I�u�W�F�N�g���j������Ă���ꍇ�̓k���|�C���^
		mTcpConnection* Parent;

		//���M�L���[�̃G���g�����A��M�L���[�̃G���g����
		QueueType Type;

		//�����Ώۃo�b�t�@
		BYTE* Buffer;

		//�񓯊��p��OVERLAPPED�\���́iWindows�ɓn���p�j
		OVERLAPPED Ov;

		//�����ς݂Ȃ��true(IO�������ɐݒ�)
		bool Completed;

		//�������̃G���[�R�[�h(IO�������ɐݒ�)
		DWORD ErrorCode;

		//�������̏����ς݃o�C�g��(IO�������ɐݒ�)
		DWORD BytesTransfered;

	};

	typedef std::deque<BufferQueueEntry*> BufferQueue;

	//���C�g�o�b�t�@
	BufferQueue MyWriteQueue;

	//���[�h�o�b�t�@
	BufferQueue MyReadQueue;

	//�ڑ��p
	BufferQueueEntry* MyConnectData;

protected:

	//�������[�`��
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//�ڑ��������̊������[�`��
	static VOID CALLBACK ConnectCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//��M�������̊������[�`��
	static VOID CALLBACK ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//���M�������̊������[�`��
	static VOID CALLBACK WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//�p�C�v��o�^����
	// wtp : �o�^��̃��[�J�[�X���b�h�v�[��
	// opt : �ʒm�I�v�V����
	bool Attach( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier );


};

class mWinsockInitializer final : private mTcpConnection 
{
	mWinsockInitializer();
	virtual ~mWinsockInitializer();
private:
	mWinsockInitializer( const mWinsockInitializer& src ) = delete;
	const mWinsockInitializer& operator=( const mWinsockInitializer& src ) = delete;

};

#endif //MTCPCONNECTION_H_INCLUDED



