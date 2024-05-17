//----------------------------------------------------------------------------
// �A�T�[�g�^�G���[����
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2020-2023 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

/*

���p�r
�A�T�[�g�������܂��̓G���[�������̏����ł��B

*/

#ifndef MERRORLOGGER_H_INCLUDED
#define MERRORLOGGER_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "mCriticalSectionContainer.h"
#include <deque>

#ifdef MERRORLOGGER_NO_FILENAME
#define ERROR_LOGGER_CURRENT_FILE (L"N/A")
#else
#define ERROR_LOGGER_CURRENT_FILE mCURRENT_FILE
#endif

class mErrorLogger
{
public:

	//�G���[����������ێ�����ő吔(�f�t�H���g)
	static const DWORD DefaultMaxErrorLogEntry = 512;

	//�R���X�g���N�^
	//max_error : �G���[����������ێ�����ő吔
	mErrorLogger( DWORD max_error = DefaultMaxErrorLogEntry );

	virtual ~mErrorLogger();

	//���O���R���\�[���ɏo�͂��邩
	enum LogOutputMode
	{
		LOG_OUTPUT_CONSOLE,		//�R�}���h���C��(stderr)�ɏo�͂���
		LOG_OUTPUT_DEBUGGER,	//�f�o�b�K�ɏo�͂���
		LOG_OUTPUT_FILE,		//�w��t�@�C���ɏo�͂���
		LOG_OUTPUT_EVENTLOG,	//Windows�̃C�x���g���O�ɏo�͂���(������)
		LOG_OUTPUT_CALLBACK,	//�R�[���o�b�N�֐����Ă�
		LOG_OUTPUT_NONE,		//�������Ȃ�
	};

	//�G���[�̃��x��
	enum ErrorLevel
	{
		//�A�T�[�g�����������Ƃ�(�ʏ푀��Ŕ������Ȃ��z��̃G���[�p)
		//Windows���O�ɋL�^�����ꍇ�́A�u�G���[�C�x���g�v�ƂȂ�܂��B
		LEVEL_ASSERT = 0,
		//��ʓI�G���[�����������Ƃ�(�t�@�C��������������)
		//Windows���O�ɋL�^�����ꍇ�́A�u�x���C�x���g�v�ƂȂ�܂��B
		LEVEL_ERROR = 1,
		//����I���ł��L�^���Ă����������������������Ƃ�(�ڑ������Ȃǂ̃C�x���g)
		//Windows���O�ɋL�^�����ꍇ�́A�u���C�x���g�v�ƂȂ�܂��B
		LEVEL_LOGGING = 2,
		//��O���X���[���ꂽ�Ƃ�(mException���g�p���܂�)
		//Windows���O�ɋL�^�����ꍇ�́A�u�G���[�C�x���g�v�ƂȂ�܂��B
		LEVEL_EXCEPTION = 3,
	};

	//�G���[���O�̃G���g��
	struct LogEntry
	{
		DWORD Id;			//���O�̘A��
		ErrorLevel Level;	//�G���[���x��
		WString File;		//�G���[�����������t�@�C����
		DWORD Line;			//�G���[�����������s
		DWORD Code1;		//�G���[�R�[�h(GetLastError�Ŏ擾������)
		ULONG_PTR Code2;	//�G���[�R�[�h(���[�U�[��`)
		WString Message1;	//���[�U�[��`�̃��b�Z�[�W
		WString Message2;	//���[�U�[��`�̃��b�Z�[�W
		DWORD Time;			//��������
		DWORD ThreadId;		//���O���L�^�����X���b�h��ID
	};
	typedef std::deque<LogEntry> Log;

	//�I�v�V�����\����
	struct LogOutputModeOpt
	{
	public:
		const LogOutputMode Mode;

		//���I�u�W�F�N�g�Ƀ��O���L�^������A�w�肵���I�u�W�F�N�g�����O�̓��e��]������
		//�]���s�v�ł����nullptr
		mErrorLogger* Proxy;

		//true�̏ꍇ�A���������ɏ���ێ����Ȃ�
		//���̃I�v�V������ύX���Ă��A���łɃ��������ɕێ����Ă�����ɂ͉e�����Ȃ�
		//�i�ǉ��̃I���I�t��ύX���邾���j
		bool NoTrace;

	protected:
		LogOutputModeOpt() = delete;
		LogOutputModeOpt( LogOutputMode mode ) : Mode( mode )
		{
			Proxy = nullptr;
			NoTrace = false;
		}
	};
	//���O���R���\�[���ɏo�͂���ꍇ�̐ݒ�I�u�W�F�N�g
	struct LogOutputModeOpt_Console : public LogOutputModeOpt
	{
		LogOutputModeOpt_Console() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_CONSOLE )
		{
		}
	};
	//���O���f�o�b�K�ɏo�͂���ꍇ�̐ݒ�I�u�W�F�N�g
	struct LogOutputModeOpt_Debugger : public LogOutputModeOpt
	{
		LogOutputModeOpt_Debugger() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_DEBUGGER )
		{
		}
	};
	//���O���t�@�C���ɏo�͂���ꍇ�̐ݒ�I�u�W�F�N�g
	struct LogOutputModeOpt_File : public LogOutputModeOpt
	{
		WString FileName;	//�t�@�C����
		bool IsAppend;		//true=�����̃t�@�C���ɒǋL���� false=�����̃t�@�C���̒��g�͏���
		LogOutputModeOpt_File() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_FILE )
		{
			IsAppend = true;
		}
	};
	//���O��Windows�̃C�x���g�ɏo�͂���ꍇ�̐ݒ�I�u�W�F�N�g
	struct LogOutputModeOpt_EventLog : public LogOutputModeOpt
	{
		WString ServerName;	//�L�^��̃T�[�o���i�󕶎���ɂ���ƃ��[�J���ƌ��Ȃ��܂��j
		WString SourceName;	//�C�x���g�̃\�[�X��

		LogOutputModeOpt_EventLog() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_EVENTLOG )
		{
		}
	};

	//���O���ɌĂяo���R�[���o�b�N
	using LogCallback = void (*)( const LogEntry& entry );

	//���O���ɃR�[���o�b�N����ꍇ�̐ݒ�I�u�W�F�N�g
	struct LogOutputModeOpt_Callback : public LogOutputModeOpt
	{
		LogCallback Callback;
		LogOutputModeOpt_Callback() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_CALLBACK )
		{
			Callback = nullptr;
		}
	};
	//���O���o�͂��Ȃ��ꍇ�̐ݒ�I�u�W�F�N�g
	struct LogOutputModeOpt_None : public LogOutputModeOpt
	{
		LogOutputModeOpt_None() : LogOutputModeOpt( LogOutputMode::LOG_OUTPUT_NONE )
		{
		}
	};

	//���O�o�͂̃��[�h��ύX����
	// setting : �V�����ݒ�
	// ret : �������^
	[[deprecated("Please use LogOutputModeOpt structure instead.")]]
	bool ChangeLogOutputMode( LogOutputMode setting );

	//���O�o�͂̃��[�h��ύX����
	//������Ɍ��炸�A�C�ӂ̃^�C�~���O�ŕύX���邱�Ƃ��ł��܂�
	threadsafe bool ChangeLogOutputMode( const LogOutputModeOpt& setting );

	//�G���[�𔭐����܂�
	//file : �G���[�����������t�@�C����
	//line : �G���[�����������s
	//ec1 : �G���[�R�[�h(GetLastError�Ŏ擾������)
	//ec2 : �G���[�R�[�h(���[�U�[��`)
	//mes : ���[�U�[��`�̃��b�Z�[�W
	//ret : �ǉ������G���[�̃��O�A��(���s�͂���܂���)
	//�@�@�@Proxy��ݒ肵�Ă���ꍇ�̒��ӓ_�F�Ԃ���郍�O�A�Ԃ́A���̃C���X�^���X�ɑ΂���ԍ��ƂȂ�܂��BProxy��̔ԍ��ł͂���܂���B
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 );

	//�G���[�𔭐����܂�
	//file : �G���[�����������t�@�C����
	//line : �G���[�����������s
	//ec1 : �G���[�R�[�h(GetLastError�Ŏ擾������)
	//ec2 : �G���[�R�[�h(���[�U�[��`)
	//mes : ���[�U�[��`�̃��b�Z�[�W
	//ret : �ǉ������G���[�̃��O�A��(���s�͂���܂���)
	//�@�@�@Proxy��ݒ肵�Ă���ꍇ�̒��ӓ_�F�Ԃ���郍�O�A�Ԃ́A���̃C���X�^���X�ɑ΂���ԍ��ƂȂ�܂��BProxy��̔ԍ��ł͂���܂���B
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringDeque& mes2 );

	//�G���[�𔭐����܂�
	//file : �G���[�����������t�@�C����
	//line : �G���[�����������s
	//ec1 : �G���[�R�[�h(GetLastError�Ŏ擾������)
	//ec2 : �G���[�R�[�h(���[�U�[��`)
	//mes : ���[�U�[��`�̃��b�Z�[�W
	//ret : �ǉ������G���[�̃��O�A��(���s�͂���܂���)
	//�@�@�@Proxy��ݒ肵�Ă���ꍇ�̒��ӓ_�F�Ԃ���郍�O�A�Ԃ́A���̃C���X�^���X�ɑ΂���ԍ��ƂȂ�܂��BProxy��̔ԍ��ł͂���܂���B
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringVector& mes2 );

	//�G���[���O��ǉ����܂�
	//toAppend : �ǋL���������O
	//ret : �ǉ������G���[�̃��O�A��(���s�͂���܂���)
	//�E���̕��@�Ń��O��ǉ������ꍇ�A�R���\�[�����ւ̏o�͂͂���܂���B
	//�E���̕��@�Ń��O��ǉ������ꍇ�AProxy��ݒ肵�Ă��Ă��]������܂���B
	threadsafe DWORD AddEntry( const Log& toAppend );

	//���݂̃��O�A�Ԃ̒l�𒲂ׂ�@
	//�����Ƀ��O�̒ǉ����������ꍇ�Ɏg�p�����A�Ԃ̒l
	DWORD GetCurrentId( void )const;

	//���O����������
	//�w�肵��Id�ȍ~�ŁA���̃��\�b�h���Ăяo�����X���b�h���L�^�������O�̈ꗗ��Ԃ�
	// Id : �����͈�(���w�肵��ID�ƈ�v���郍�O���܂݂܂�)
	// file : �����͈�(���O���L�^�����t�@�C���̖��́B�󕶎���Ȃ�ΑS��)
	// retLog : �擾�������O�̊i�[��
	threadsafe void SearchLog( DWORD Id , Log& retLog , const WString& file = L"" )const;

	//���O����������
	//�w�肵��Id�ȍ~�ŁA���̃��\�b�h���Ăяo�����X���b�h���L�^�������O�̒��ɁA�w�肵��code�̃��O�����邩�Ȃ�����Ԃ��܂�
	// Id : �����͈�(���w�肵��ID�ƈ�v���郍�O���܂݂܂�)
	// code : �L���𔻒肷��R�[�h
	// file : �����͈�(���O���L�^�����t�@�C���̖��́B�󕶎���Ȃ�ΑS��)
	// retLog : �擾�������O�̊i�[��
	threadsafe bool SearchLog( DWORD Id , ULONG_PTR code , const WString& file = L"" )const;

	//���O���擾����
	//�w�肵��Id�ȍ~�̃��O���R�s�[����(�L�^�����X���b�h�Ɋ֌W�Ȃ��S�Ď擾���܂�)
	// Id : �����͈�(���w�肵��ID�ƈ�v���郍�O���܂݂܂�)
	// retLog : �R�s�[��
	// file : �����͈�(���O���L�^�����t�@�C���̖��́B�󕶎���Ȃ�ΑS��)
	threadsafe void GetLog( DWORD Id , Log& retLog , const WString& file = L"" )const;

	//���O���擾����
	//�w�肵��Id�ȍ~�̃��O���R�s�[����(�L�^�����X���b�h�Ɋ֌W�Ȃ��S�Ď擾���܂�)
	// Id : �����͈�(���w�肵��ID�ƈ�v���郍�O���܂݂܂�)
	// retLog : �R�s�[��
	// file : �����͈�(���O���L�^�����t�@�C���̖��́B�󕶎���Ȃ�ΑS��)
	threadsafe void GetLog( DWORD Id_from , DWORD Id_to , Log& retLog , const WString& file = L"" )const;

	//�G���[�̔����񐔂��擾����
	DWORD GetErrorCount( ErrorLevel level )const;

	//���O����������
	//�������񐔂���������
	void Clear( void );

	//���O�̋L�^�𖳌�������
	void Disable( void );

	//���O�̋L�^��L��������
	void Enable( void );

private:
	mErrorLogger( const mErrorLogger& src ) = delete;
	mErrorLogger& operator=( const mErrorLogger& source ) = delete;

protected:

	//�R���\�[���Ƀ��O���o�͂���
	// entry : �o�͂��郍�O
	void OutputLogToConsole( const LogEntry& entry );

	//�f�o�b�K�Ƀ��O���o�͂���
	// entry : �o�͂��郍�O
	void OutputLogToDebugger( const LogEntry& entry );

	//�t�@�C���Ƀ��O���o�͂���
	// entry : �o�͂��郍�O
	void OutputLogToFile( const LogEntry& entry );

	//�G���[�𔭐����܂�
	//public��AddEntry�ɁA����C���X�^���X�̃|�C���^��t�^�������ɂȂ�܂��BProxy�����邮����Ȃ��悤�ɂ��邽�߂̂��̂ł��B
	//origin : Proxy����̌Ăяo���ł͂Ȃ��ꍇ(��ԍŏ��̌Ăяo���̏ꍇ)�́Anullptr
	//         Proxy����̌Ăяo���ł���ꍇ�́A��ԍŏ��ɌĂяo�����I�u�W�F�N�g�̃|�C���^
	//ret : origin��nullptr�̏ꍇ�A�ǉ������G���[�̃��O�A��
	//      origin��nullptr�ł͂Ȃ��ꍇ�A0
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 , const mErrorLogger* origin );

	//�G���[�𔭐����܂�
	//public��AddEntry�ɁA����C���X�^���X�̃|�C���^��t�^�������ɂȂ�܂��BProxy�����邮����Ȃ��悤�ɂ��邽�߂̂��̂ł��B
	//origin : Proxy����̌Ăяo���ł͂Ȃ��ꍇ(��ԍŏ��̌Ăяo���̏ꍇ)�́Anullptr
	//         Proxy����̌Ăяo���ł���ꍇ�́A��ԍŏ��ɌĂяo�����I�u�W�F�N�g�̃|�C���^
	//ret : origin��nullptr�̏ꍇ�A�ǉ������G���[�̃��O�A��
	//      origin��nullptr�ł͂Ȃ��ꍇ�A0
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringDeque& mes2 , const mErrorLogger* origin );

	//�G���[�𔭐����܂�
	//public��AddEntry�ɁA����C���X�^���X�̃|�C���^��t�^�������ɂȂ�܂��BProxy�����邮����Ȃ��悤�ɂ��邽�߂̂��̂ł��B
	//origin : Proxy����̌Ăяo���ł͂Ȃ��ꍇ(��ԍŏ��̌Ăяo���̏ꍇ)�́Anullptr
	//         Proxy����̌Ăяo���ł���ꍇ�́A��ԍŏ��ɌĂяo�����I�u�W�F�N�g�̃|�C���^
	//ret : origin��nullptr�̏ꍇ�A�ǉ������G���[�̃��O�A��
	//      origin��nullptr�ł͂Ȃ��ꍇ�A0
	threadsafe DWORD AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WStringVector& mes2 , const mErrorLogger* origin );

protected:

	DWORD MyErrorCount[ 4 ];	//�G���[������

	Log MyLogError;		//�G���[���O

	//�o�͐�n���h��(�t�@�C���o��/�C�x���g���O�p)
	HANDLE MyHandle;

	//�R�[���o�b�N�̏ꍇ�̃R�[���o�b�N�֐�
	LogCallback MyCallback;

	//�v���N�V��
	mErrorLogger* MyProxy;

	//���O�̋L�^���s����
	bool MyIsEnabled;

	//�������L�^���s����
	bool MyIsNoTrace;

	//���݂̘A��
	DWORD MyCurrentId;

	//�G���[���O�̍ő�T�C�Y
	DWORD MyMaxErrorSize;

	//�����A�N�Z�X�΍��p�N���e�B�J���Z�N�V����
	mutable mCriticalSectionContainer MyCritical;

	//���O�R���\�[���o�̓��[�h
	LogOutputMode MyLogOutputMode;

};

//�O���[�o���I�u�W�F�N�g
#ifndef MERRORLOGGER_CPP_COMPILING
extern mErrorLogger g_ErrorLogger;
#else
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
mErrorLogger g_ErrorLogger;
#endif

//RaiseError��mErrorLogger���|�C���^�œn����Ă��Q�Ƃœn����Ă��ǂ��悤�ɂ��邽�߂̃v���N�V�i�|�C���^Ver�j
inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , DWORD_PTR val )
{
	if( obj == nullptr )
	{
		g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_ASSERT , file , line , 0 , 0 , L"�G���[�o�^��I�u�W�F�N�g��NULL�ł�" , L"" );
		obj = &g_ErrorLogger;
	}

	WString mes2;
#ifdef _WIN64
	sprintf( mes2 , L"0x%llX(%lld)" , val , val );
#else
	sprintf( mes2 , L"0x%lX(%ld)" , val , val );
#endif
	return obj->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , DWORD_PTR val )
{
	return RaiseErrorInternal( obj , level , file , line , ec1 , ec2 , AString2WString( mes1 ) , val );
}

inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 = L"" )
{
	if( obj == nullptr )
	{
		g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_ASSERT , L"" , 0 , 0 , 0 , L"�G���[�o�^��I�u�W�F�N�g��NULL�ł�" , L"" );
		obj = &g_ErrorLogger;
	}
	return obj->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString& mes2 = "" )
{
	return RaiseErrorInternal( obj , level , file , line , ec1 , ec2 , AString2WString( mes1 ) , AString2WString( mes2 ) );
}

DWORD RaiseErrorInternalF( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString mes2 , ... );
DWORD RaiseErrorInternalF( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString mes2 , ... );

//RaiseError��mErrorLogger���|�C���^�œn����Ă��Q�Ƃœn����Ă��ǂ��悤�ɂ��邽�߂̃v���N�V�i�Q��Ver�j
inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , DWORD_PTR val )
{
	WString mes2;
#ifdef _WIN64
	sprintf( mes2 , L"0x%llX(%lld)" , val , val );
#else
	sprintf( mes2 , L"0x%lX(%ld)" , val , val );
#endif
	return obj.AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , DWORD_PTR val )
{
	return RaiseErrorInternal( obj , level , file , line , ec1 , ec2 , AString2WString( mes1 ) , val );
}

inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 = L"" )
{
	return obj.AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 );
}

inline DWORD RaiseErrorInternal( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString& mes2 = "" )
{
	return obj.AddEntry( level , file , line , ec1 , ec2 , AString2WString( mes1 ) , AString2WString( mes2 ) );
}

DWORD RaiseErrorInternalF( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString mes2 , ... );
DWORD RaiseErrorInternalF( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString mes2 , ... );

//���샍�O
//obj : �G���[���̓o�^��
//error_code : �G���[�R�[�h(���[�U�[��`)
//mes : ���[�U�[��`�̃��b�Z�[�W
//�E���M���O�͐��퓮��ł��L�^���Ă��������C�x���g�i�ڑ������Ƃ��j�ɑ΂��Đ������܂�
#define CreateLogEntry(obj,error_code,...)			\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternal(								\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_LOGGING,	\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*CreateLogEntry*/

//���샍�O
//obj : �G���[���̓o�^��
//error_code : �G���[�R�[�h(���[�U�[��`)
// ...(�ϒ�����)�͈ȉ��̒ʂ�ł�
//  1�v�f�� ���b�Z�[�W(�����w�蕶���g�p�s��)
//  2�v�f�� �����w�蕶����
//  3�v�f�ڈȍ~ �����w�蕶����ɖ��ߍ��ޒl
//�E���M���O�͐��퓮��ł��L�^���Ă��������C�x���g�i�ڑ������Ƃ��j�ɑ΂��Đ������܂�
#define CreateLogEntryF(obj,error_code,...)			\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternalF(							\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_LOGGING,	\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*CreateLogEntryF*/

//�G���[����
//obj : �G���[���̓o�^��
//error_code : �G���[�R�[�h(���[�U�[��`)
//mes : ���[�U�[��`�̃��b�Z�[�W
//�E�G���[�͒ʏ푀��ŋN���肤��G���[�i�t�@�C�����Ȃ������Ƃ��j�ɑ΂��Đ������܂��B
#define RaiseError(obj,error_code,...)				\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternal(								\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ERROR,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseError*/

//�G���[����
//obj : �G���[���̓o�^��
//error_code : �G���[�R�[�h(���[�U�[��`)
// ...(�ϒ�����)�͈ȉ��̒ʂ�ł�
//  1�v�f�� ���b�Z�[�W(�����w�蕶���g�p�s��)
//  2�v�f�� �����w�蕶����
//  3�v�f�ڈȍ~ �����w�蕶����ɖ��ߍ��ޒl
//�E�G���[�͒ʏ푀��ŋN���肤��G���[�i�t�@�C�����Ȃ������Ƃ��j�ɑ΂��Đ������܂��B
#define RaiseErrorF(obj,error_code,...)				\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternalF(							\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ERROR,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseErrorF*/


//�A�T�[�g����
//obj : �A�T�[�g���̓o�^��
//error_code : �G���[�R�[�h(���[�U�[��`)
//mes : ���[�U�[��`�̃��b�Z�[�W
//�E�A�T�[�g�͒ʏ푀��ł͋N���肦�Ȃ��͂��̃G���[�i�o�O�Z���̂��́j�ɑ΂��Đ������܂��B
#define RaiseAssert(obj,error_code,...)				\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternal(								\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ASSERT,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseAssert*/


//�A�T�[�g����
//obj : �A�T�[�g���̓o�^��
//error_code : �G���[�R�[�h(���[�U�[��`)
// ...(�ϒ�����)�͈ȉ��̒ʂ�ł�
//  1�v�f�� ���b�Z�[�W(�����w�蕶���g�p�s��)
//  2�v�f�� �����w�蕶����
//  3�v�f�ڈȍ~ �����w�蕶����ɖ��ߍ��ޒl
//�E�A�T�[�g�͒ʏ푀��ł͋N���肦�Ȃ��͂��̃G���[�i�o�O�Z���̂��́j�ɑ΂��Đ������܂��B
#define RaiseAssertF(obj,error_code,...)			\
{													\
	DWORD tmp_error_code = GetLastError();			\
	RaiseErrorInternalF(							\
		obj,										\
		mErrorLogger::ErrorLevel::LEVEL_ASSERT,		\
		ERROR_LOGGER_CURRENT_FILE ,					\
		__LINE__ ,									\
		tmp_error_code ,							\
		error_code ,								\
		__VA_ARGS__ );								\
}													\
/*RaiseAssert*/

#endif	//MERRORLOGGER_H_INCLUDED

