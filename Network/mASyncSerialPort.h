//----------------------------------------------------------------------------
// �V���A���|�[�g�n���h��
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MASYNCSERIALPORT_H_INCLUDED
#define MASYNCSERIALPORT_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include "../General/mCriticalSectionContainer.h"
#include "../General/mNotifyOption.h"
#include "../Thread/mWorkerThreadPool.h"

#include <memory>

/*
�g����

int main( int argc , char** argv )
{
	mASyncSerialPort SerialPort;
	mWorkerThreadPool ThreadPool;

	//---------------
	//����
	//---------------

	//�X���b�h�v�[���쐬
	//�E���̃N���X�̓X���b�h�v�[���ƘA�g���ē����̂ŕK�{
	ThreadPool.Begin( 1 );

	//COM�|�[�g
	{
		mASyncSerialPort::Option opt;				//���e��ݒ�
		mASyncSerialPort::NotifyOption notifier;	//���C�x���g���ɃR�[���o�b�N�����K�v�Ȃ�

		opt.Fileinfo.Path = LR"(\\.\COM5)";
		SerialPort.Open( ThreadPool , opt , notifier );
	}

	//---------------
	//���C�����[�v
	//---------------

	while( 1 )
	{
		//�ǂݏ����֘A�̃C�x���g����������ƁA�ݒ肵�����e�ɉ�����
		//�E�R�[���o�b�N�֐����Ă΂��
		//�E�C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�
		//�E�E�C���h�E���b�Z�[�W���|�X�g�����
		//�Ƃ��������Ƃ��N����̂ŁA������g���K�ɂ��ăA�v���𓮍삳����

		//�C�x���g������������A�ȉ��̂悤�Ȋ����Ńf�[�^��ǂ�
		INT read_one_byte = SerialPort.read();	//1�����P��

		AString read_as_string;
		SerialPort.ReadLine( read_as_string );	//1�s�P��

		//��1�s�P�ʂœǂނƂ��A�ʐM�G���[���ɖ����ɑ҂������Ȃ��悤�ɒ���
		//  ��莞�Ԏ�M���Ȃ��Ƃ���1�������Ƃɓǂ�ł݂�Ƃ����đ΍�
	}

	//---------------
	//�N���[���A�b�v
	//---------------

	//EOF��ݒ�(�V���A���|�[�g�͏I�[���Ȃ��̂Ŗ����I�ɐݒ肵�Ȃ��Ɖi�v�ɓǂ߂Ă��܂�)
	SerialPort.SetEOF();

	//��M�ς݂̃f�[�^��S�ď�������(IsEOF��true�ɂȂ�܂�)
	//�����ł́A�P�Ɏ�M�ς݂̃f�[�^��S���̂ĂĂ���
	while( !SerialPort.IsEOF() )
	{
		//Read()��EOF��Ԃ��Ă��A�u�����_�̍Ō�܂œǂ񂾁v������������Ȃ�(����f�[�^���͂���������Ȃ�)
		//IsEOF��true�̏ꍇ�́A�ȍ~�f�[�^���͂����Ƃ͂Ȃ�

		while( SerialPort.Read() != EOF )
		{
		}
		Sleep( 100 );
	}

	//�X���b�h�v�[���̃^�X�N���͂���̂�҂�
	//���̃T���v���ł̓^�X�N��ǉ����Ă��Ȃ��̂ŁA�����ɔ�����
	while( ThreadPool.GetTaskCount() )
	{
		Sleep( 100 );
	}

	//�X���b�h�v�[���I��
	ThreadPool.End();

	return 0;
}

*/

namespace Definitions_ASyncSerialPort
{
	enum ParityType
	{
		PARITYTYPE_NOPARITY ,	//�p���e�B�Ȃ�
		PARITYTYPE_EVEN ,		//�����p���e�B
		PARITYTYPE_ODD ,		//��p���e�B
	};

	enum StopBitType
	{
		STOPBIT_ONE ,			//1�r�b�g
		STOPBIT_ONEFIVE ,		//1.5�r�b�g
		STOPBIT_TWO ,			//2�r�b�g
	};

	//�G���[�����������^�C�~���O
	enum ErrorAction
	{
		ERROR_ON_CONNECT,	//�ڑ��������̃G���[
		ERROR_ON_READ,		//�ǂݍ��ݒ��̃G���[
		ERROR_ON_WRITE,		//�������ݒ��̃G���[
	};
};

class mSerialReadStream : public mFileReadStreamBase
{
public:
	//�ǂݎ�葤�̌o�H���J���Ă��邩�𔻒肵�܂�
	//�J���Ă���ꍇ�͐^���Ԃ�܂�
	virtual bool IsOpen( void )const
	{
		return !IsEOF();
	}
};

class mSerialWriteStream : public mFileWriteStreamBase
{
public:
	mSerialWriteStream()
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
	mSerialWriteStream( const mSerialWriteStream& src ) = delete;
	const mSerialWriteStream& operator=( const mSerialWriteStream& src ) = delete;

protected:
	bool MyIsClosed;
};

//COM�|�[�g�n���h���̃��b�p�[
class mASyncSerialPort : public mSerialReadStream , public mSerialWriteStream
{
public:
	mASyncSerialPort();
	virtual ~mASyncSerialPort();

	//�G���[�������̗��R�R�[�h
	enum ErrorCode
	{
	};

	//�p���e�B�̐ݒ�
	using ParityType = Definitions_ASyncSerialPort::ParityType;

	//�X�g�b�v�r�b�g��
	using StopBitType = Definitions_ASyncSerialPort::StopBitType;

	//�t�@�C���ݒ�
	using FileOption = mFile::Option;

	//�t�@�C�����J���Ƃ��̃��[�h
	using CreateMode = mFile::CreateMode;

	//�I�v�V�����\����
	struct Option
	{
		//-----------
		//����M�ݒ�
		//-----------
		FileOption Fileinfo;
		DWORD BaudRate;			//�{�[���[�g
		ParityType Parity;		//�p���e�B
		StopBitType StopBit;	//�X�g�b�v�r�b�g
		DWORD ByteSize;			//1�o�C�g�����r�b�g���B���ʂ�8�B

		//-----------
		//�o�b�t�@�����O�ݒ�
		//-----------
		DWORD ReadPacketSize;			//�ǂݍ��݃p�P�b�g�̃T�C�Y
		DWORD ReadPacketCount;			//�ǂݍ��݃p�P�b�g���m�ۂ��鐔
		DWORD WritePacketSize;			//�������݃p�P�b�g�̃T�C�Y
		DWORD WritePacketNotifyCount;	//�������ݑ҂��p�P�b�g�����̎q�Ŏw�肵��������������ꍇ�ɒʒm����
		DWORD WritePacketLimit;			//�������ݑ҂��p�P�b�g�̐��̏���i������Ə������݃G���[�j
		DWORD ReadBufferTimeout;		//�C�ӂ̃o�C�g�̎�M�Ԋu(�~���b)�����̒l�����������M�ʒm�𐶐�����

		//-----------
		//�����l
		//-----------
		Option()
		{
			BaudRate = 9600;
			Parity = mASyncSerialPort::ParityType::PARITYTYPE_NOPARITY;
			StopBit = mASyncSerialPort::StopBitType::STOPBIT_ONE;
			ByteSize = 8;
			ReadPacketSize = 128;
			ReadPacketCount = 2;
			WritePacketSize = 128;
			WritePacketNotifyCount = 0;
			WritePacketLimit = 256;
			ReadBufferTimeout = 100;
		}
	};

	union NotifyFunctionOpt
	{
		struct OnReadOpt
		{
		}OnRead;

		struct OnWriteOpt
		{
		}OnWrite;

		struct OnErrorOpt
		{
			using ErrorAction = Definitions_ASyncSerialPort::ErrorAction;
			ErrorAction Action;
			DWORD ErrorCode;
		}OnError;
	};

	//�R�[���o�b�N�Œʒm����ꍇ�̃v���g�^�C�v
	//req : �R�[���o�b�N�𔭐��������I�u�W�F�N�g�̎Q��
	//parameter : �I�u�W�F�N�g�������ɓn�����C�ӂ̒l(NotifierInfo::Parameter�̒l)
	//opt : �g�����
	using NotifyFunction = void(*)( mASyncSerialPort& port , DWORD_PTR parameter , const NotifyFunctionOpt& opt );

	//�ʒm�ݒ�
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		NotifierInfo OnRead;
		NotifierInfo OnWrite;
		NotifierInfo OnError;
	};

	//�V���A���|�[�g���J��
	bool Open( mWorkerThreadPool& wtp , const Option& opt , const NotifyOption& notifier );

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

	//���M�������̃f�[�^��j�����܂�
	bool Cancel( void );

	//���ݖ������̒ʐM(����M�Ƃ�)��S�Ĕj�����A�ڑ�����܂�
	bool Abort( void );

	//�ǂݍ��ݑ��̌o�H����܂�
	virtual bool SetEOF( void );

	//���M�������̃f�[�^�����邩��Ԃ��܂�
	// ret : ���M�������̃f�[�^�̐�(�L���[�̃G���g���P��)
	DWORD IsWriting( void )const;

	//�ǂݍ��ݗp�̓����o�b�t�@���m�ۂ��܂�
	//�Վ��Ƀo�b�t�@���K�v�ɂȂ�Ƃ��Ɏg�p���܂�
	// count : �����o�b�t�@���w�肵���������ł���΁A���̐��ɂȂ�悤�ɓ����o�b�t�@��V���ɍ쐬���܂�
	bool PrepareReadBuffer( DWORD count );

private:

	mASyncSerialPort( const mASyncSerialPort& src ) = delete;
	const mASyncSerialPort& operator=( const mASyncSerialPort& src ) = delete;

protected:
	
	//COM�|�[�g�̃n���h��
	HANDLE MyHandle;

	//�ݒ�l
	Option MyOption;

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
		WRITE_QUEUE_ENTRY,
		READ_QUEUE_ENTRY
	};

	//�L���[
	struct BufferQueueEntry
	{
		//�e�I�u�W�F�N�g�ւ̃|�C���^
		//�������A�񓯊�����̊������_�Őe�I�u�W�F�N�g���j������Ă���ꍇ�̓k���|�C���^
		mASyncSerialPort* Parent;

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

protected:

	//�������[�`��
	static VOID CALLBACK CompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//��M�������̊������[�`��
	static VOID CALLBACK ReadCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

	//���M�������̊������[�`��
	static VOID CALLBACK WriteCompleteRoutine( DWORD ec , DWORD len , LPOVERLAPPED ov );

};



#endif //MASYNCSERIALPORT_H_INCLUDED



