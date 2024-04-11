//----------------------------------------------------------------------------
// �p�C�v�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MASYNCNAMEDPIPE_H_INCLUDED
#define MASYNCNAMEDPIPE_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"
#include <memory>

/*
�g����

#include "mStandard.h"
#include "Network/mASyncNamedPipe.h"

mWorkerThreadPool ThreadPool;
mASyncNamedPipe Pipe;

void ReadCallback_Server( mASyncNamedPipe& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	WString str;
	pipe.ReadLine( str );
	wchar_printf( "Recieved(Server):%s\n" , str.c_str() );

	pipe.WriteString( L"Pong" );
	pipe.FlushCache();
	return;
}
void ReadCallback_Client( mASyncNamedPipe& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	WString str;
	pipe.ReadLine( str );

	wchar_printf( "Recieved(Client):%s\n" , str.c_str() );
	return;
}
void ConnectCallback( mASyncNamedPipe& pipe , DWORD_PTR parameter , DWORD_PTR opt )
{
	pipe.SetEncode( mFileReadStreamBase::Encode::ENCODE_UTF16 );
	return;
}

int main( int argc , char** argv )
{

	//---------------
	//����
	//---------------
	//�X���b�h�v�[���쐬
	//�E���̃N���X�̓X���b�h�v�[���ƘA�g���ē����̂ŕK�{
	ThreadPool.Begin( 1 );

	//---------------
	//�T�[�o�[��
	//---------------
	{
		//�ڑ��܂��p�C�v�������̐ݒ�
		mASyncNamedPipe::CreateOption createopt;
		createopt.MaxConn = 1;
		createopt.Timeout = 50;
		createopt.RemoteAccess = false;

		//�p�C�v�ڑ����̐ݒ�
		mASyncNamedPipe::ConnectionOption connopt;

		//�ʒm�֘A�̐ݒ�
		mASyncNamedPipe::NotifyOption notifyopt;
		notifyopt.OnConnect.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnConnect.Notifier.CallbackFunction = ConnectCallback;
		notifyopt.OnConnect.Parameter = parameter;
		notifyopt.OnRead.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnRead.Notifier.CallbackFunction = ReadCallback;
		notifyopt.OnRead.Parameter = parameter;

		Pipe.Create( ThreadPool , createopt , connopt , notifyopt , L"." , "crea\\test)" );
	}

	//---------------
	//�N���C�A���g��
	//---------------
	mASyncNamedPipe client;
	{
		//�p�C�v�ڑ����̐ݒ�
		mASyncNamedPipe::ConnectionOption opt;

		//�ʒm�֘A�̐ݒ�
		mASyncNamedPipe::NotifyOption notifyopt;
		notifyopt.OnConnect.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnConnect.Notifier.CallbackFunction = ConnectCallback;
		notifyopt.OnConnect.Parameter = parameter;
		notifyopt.OnRead.Mode = mASyncNamedPipe::NotifyOption::NotifyMode::NOTIFY_CALLBACK;
		notifyopt.OnRead.Notifier.CallbackFunction = ReadCallback;
		notifyopt.OnRead.Parameter = parameter;

		client.Connect( ThreadPool , opt , notifyopt , L"." , "crea\\test)" );
	}

	for( int i = 0 ; i < 3 ; i++ )
	{

		SleepEx( 1000 , true );
		if( Pipe.IsConnected() )
		{
			bool valid = true;
			valid &= client.WriteString( L"Ping" );
			valid &= client.FlushCache();
			if( !valid )
			{
				//�p�C�v������ł�
				wchar_printf( "pipe died" );
				break;
			}
		}
	}

	//��Ȃ̂ŃT�[�o�[�����������B�{�Ԃł̓N���C�A���g�����l�ɏ�������B
	Pipe.Close();		//�������ݏI��
	Pipe.SetEOF();		//�ǂݍ��ݏI��
	//�������̓ǂݍ��݃L���[�j��
	while( !Pipe.IsEOF() )
	{
		SleepEx( 100 , true );
		while( Pipe.Read() != EOF )
		{
		}
	}
	//�������̏������݃L���[�j��
	Pipe.Cancel();

	//�X���b�h�v�[���I��
	ThreadPool.End();

	return 0;
}


*/

namespace Definitions_ASyncNamedPipe
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
		ERROR_ON_CONNECT,	//�ڑ��������̃G���[
		ERROR_ON_READ,		//�ǂݍ��ݒ��̃G���[
		ERROR_ON_WRITE,		//�������ݒ��̃G���[
	};
};

class mPipeReadStream : public mFileReadStreamBase
{
public:
	//�ǂݎ�葤�̌o�H���J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mPipeWriteStream : public mFileWriteStreamBase
{
public:
	mPipeWriteStream()
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
	mPipeWriteStream( const mPipeWriteStream& src ) = delete;
	const mPipeWriteStream& operator=( const mPipeWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

class mASyncNamedPipe : public mPipeReadStream , public mPipeWriteStream
{
public:
	mASyncNamedPipe();
	virtual ~mASyncNamedPipe();

	//�G���[�������̗��R�R�[�h
	enum ErrorCode
	{
	};

	//���O�t���p�C�v�쐬�p�̃I�v�V�����\����
	struct CreateOption
	{
		DWORD MaxConn;					//�ő哯���ڑ���
		bool RemoteAccess;				//�����[�g����̃A�N�Z�X�������邩
		DWORD Timeout;

		CreateOption()
		{
			MaxConn = 1;
			RemoteAccess = false;
			Timeout = 50;
		};
	};

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
			using ErrorAction = Definitions_ASyncNamedPipe::ErrorAction;
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//�ʒm�ݒ�
	using NotifyFunction = void(*)( mASyncNamedPipe& pipe , DWORD_PTR parameter , const NotifyFunctionOpt& opt );
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

		//-----------
		//����M�ݒ�
		//-----------

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
			ReadPacketSize = 128;
			ReadPacketCount = 2;
			WritePacketSize = 128;
			WritePacketNotifyCount = 0;
			WritePacketLimit = 256;
			ReadBufferTimeout = 100;
		}
	};

	//�����̖��O�t���p�C�v�ɐڑ�����
	// wtp : �o�^��̃��[�J�[�X���b�h�v�[��
	// opt : �ʒm�I�v�V����
	// pipename : ���O�t���p�C�v�̖��O
	bool Connect( mWorkerThreadPool& wtp , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename );

	//�p�C�v��V���ɍ쐬����
	// wtp : �o�^��̃��[�J�[�X���b�h�v�[��
	// opt : �ʒm�I�v�V����
	bool Create( mWorkerThreadPool& wtp , const CreateOption& createopt , const ConnectionOption& opt , const NotifyOption& notifier , const WString& servername , const WString& pipename );

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

	mASyncNamedPipe( const mASyncNamedPipe& src ) = delete;
	const mASyncNamedPipe& operator=( const mASyncNamedPipe& src ) = delete;

protected:
	
	//�p�C�v�̃n���h��
	HANDLE MyHandle;

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
		mASyncNamedPipe* Parent;

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



#endif //MASYNCSERIALPORT_H_INCLUDED



