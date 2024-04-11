//----------------------------------------------------------------------------
// �X�}�[�g�J�[�h���[�_�[�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#include "mSCReader.h"
#include <General/mErrorLogger.h>

mSCReader::mSCReader()
{
	MyServiceAvailableSignal = SCardAccessStartedEvent();
	MySCardContext = 0;
	MyThread = nullptr;
}

mSCReader::~mSCReader()
{
	//���[�_�[�̊Ď����ł���΂�߂�����
	EndWatch();
	//���\�[�X�̊J��
	if( MyServiceAvailableSignal != NULL )
	{
		SCardReleaseStartedEvent();
		MyServiceAvailableSignal = NULL;
	}
	if( MySCardContext )
	{
		SCardReleaseContext( MySCardContext );
		MySCardContext = 0;
	}
}

bool mSCReader::WaitForServiceAvailable( DWORD waittime )
{
	if( MyServiceAvailableSignal == NULL )
	{
		return false;
	}
	if( WaitForSingleObject( MyServiceAvailableSignal , waittime ) == WAIT_OBJECT_0 )
	{
		return true;
	}
	return false;
}


bool mSCReader::OpenContext( void )const
{
	//�������ς݃`�F�b�N
	if( MySCardContext )
	{
		return true;
	}

	//�ڑ�
	LONG result = SCardEstablishContext( SCARD_SCOPE_USER , nullptr , nullptr , &MySCardContext );
	switch( result )
	{
	case SCARD_S_SUCCESS:
		break;
	case SCARD_E_NO_SERVICE:
		RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�T�[�r�X���N�����Ă��܂���" );
		return false;
	default:
		RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�̏����������s���܂���" );
		return false;
	}
	return true;
}

bool mSCReader::GetCardReaderList( WStringVector& retReader )const
{
	retReader.clear();

	//�������ς݃`�F�b�N
	if( !OpenContext() )
	{
		return false;
	}

	//�J�[�h���[�_�[�̈ꗗ���擾����
	BYTE* buffer = nullptr;
	DWORD length = SCARD_AUTOALLOCATE;
	LONG result = SCardListReadersW( MySCardContext , nullptr , (LPWSTR)&buffer, &length);
	switch( result )
	{
	case SCARD_S_SUCCESS:
		break;
	case SCARD_E_NO_READERS_AVAILABLE:
		RaiseError( g_ErrorLogger , result , L"�J�[�h���[�_�[���ڑ�����Ă��܂���" );
		return false;
	case SCARD_E_READER_UNAVAILABLE:
		RaiseError( g_ErrorLogger , result , L"�J�[�h���[�_�[�����p�ł��܂���" );
		return false;
	default:
		RaiseError( g_ErrorLogger , result , L"�J�[�h���[�_�[�̖��O���擾�ł��܂���" );
		return false;
	}

	//�_�u���k���̕�������p�[�X
	ReadDoubleNullString( (const WCHAR*)buffer , retReader );
	SCardFreeMemory( MySCardContext , buffer );
	return true;
}

//�Ď��X���b�h
class SCReaderWatchThread : public mThread
{
public:
	SCReaderWatchThread( mSCReader& reader , SCARDCONTEXT context , const WString& readername , const mSCReader::NotifyOption& opt )
		: MyReader( reader )
		, MySCardContext( context )
		, MyReaderName( readername )
		, MyOption( opt )
	{
	}
	virtual ~SCReaderWatchThread()
	{
	}
	virtual unsigned int TaskFunction() override
	{
		DWORD waittime = 0;

		SCARD_READERSTATEW stat = { 0 };
		stat.szReader = MyReaderName.c_str();
		stat.dwCurrentState = SCARD_STATE_UNAWARE;

		while( 1 )
		{
			//�I���v���̃`�F�b�N
			DWORD code = WaitForSingleObject( MyTerminateSignal , 0 );
			switch( code )
			{
			case WAIT_OBJECT_0:	//�I���v������
				return 0;
			case WAIT_TIMEOUT:	//�I���v���Ȃ�
				break;
			default:			//�G���[
				RaiseError( g_ErrorLogger , 0 , L"�X���b�h��Ԃ��ُ�ł�" );
				return GetLastError();
			}

			//�X�}�[�g�J�[�h���[�_�[�̏�Ԃ��`�F�b�N����
			LONG result = SCardGetStatusChangeW( MySCardContext , waittime , &stat , 1 );
			switch( result )
			{
			case SCARD_S_SUCCESS:
				StatusChangeHandler( *this , stat );
				break;
			case SCARD_E_CANCELLED:
				break;
			default:
				RaiseError( g_ErrorLogger , result , L"�X�}�[�g�J�[�h�̊Ď������s���܂���" );
				return result;
			}

			stat.dwCurrentState = stat.dwEventState;
			waittime = INFINITE;
		}
	}

private:
	SCReaderWatchThread();
	SCReaderWatchThread( const SCReaderWatchThread& source );
	const SCReaderWatchThread& operator=( const SCReaderWatchThread& source ) = delete;

protected:
	mSCReader& MyReader;
	const SCARDCONTEXT MySCardContext;
	const WString MyReaderName;
	const mSCReader::NotifyOption MyOption;

private:

	using CardKind = Definitions_SCReader::CardKind;
	
	//�J�[�h�̎�ނ𔻕ʂ���
	static CardKind GetCardKind( const mBinary& atr )
	{
		//                                    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F   10   11   12   13
		static const BYTE AtrMifare1K[] = {0x3B,0x8F,0x80,0x01,0x80,0x4F,0x0C,0xA0,0x00,0x00,0x03,0x06,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x6A};
		static const BYTE AtrFelica[]   = {0x3B,0x8F,0x80,0x01,0x80,0x4F,0x0C,0xA0,0x00,0x00,0x03,0x06,0x11,0x00,0x3B,0x00,0x00,0x00,0x00,0x42};

		auto Compare = []( const mBinary& atr1 , const BYTE* atr2 , DWORD atrlen ) -> bool
		{
			if( atrlen != atr1.size() )
			{
				return false;
			}
			for( DWORD i = 0 ; i < atrlen ; i++ )
			{
				if( atr1.at( i ) != atr2[ i ] )
				{
					return false;
				}
			}
			return true;
		};

		//Mifare1K���H
		if( Compare( atr , AtrMifare1K , sizeof( AtrMifare1K ) ) )
		{
			return CardKind::MIFARE_CLASSIC_1K;
		}
		//Felica���H
		if( Compare( atr , AtrFelica , sizeof( AtrFelica ) ) )
		{
			return CardKind::FELICA;
		}
		//���̑��̃J�[�h
		return CardKind::UNKNOWN;
	}

	static void StatusChangeHandler( SCReaderWatchThread& me , SCARD_READERSTATEW& stat )
	{
		auto AtrCopy = []( DWORD size , const BYTE* bindata , mBinary& retAtr ) -> void
		{
			retAtr.clear();
			for( DWORD i = 0 ; i < size ; i++ )
			{
				retAtr.push_back( *bindata );
				bindata++;
			}
		};

		if( ( stat.dwEventState & SCARD_STATE_PRESENT ) && !( stat.dwCurrentState & SCARD_STATE_PRESENT ) )
		{
			Definitions_SCReader::OnCardPresent info;
			info.Status = stat.dwEventState;
			AtrCopy( stat.cbAtr , stat.rgbAtr , info.Atr );
			info.Kind = GetCardKind( info.Atr );

			mSCReader::NotifyFunctionOptPtr ptr;
			ptr.OnCardPresent = &info;
			AsyncEvent( me.MyReader , me.MyOption.OnCardPresent , &ptr );
		}
		else if( !( stat.dwEventState & SCARD_STATE_PRESENT ) && ( stat.dwCurrentState & SCARD_STATE_PRESENT ) )
		{
			Definitions_SCReader::OnCardRemoved info;
			info.Status = stat.dwEventState;
			AtrCopy( stat.cbAtr , stat.rgbAtr , info.Atr );

			mSCReader::NotifyFunctionOptPtr ptr;
			ptr.OnCardRemoved = &info;
			AsyncEvent( me.MyReader , me.MyOption.OnCardRemoved , &ptr );
		}
		if( 
			( stat.dwEventState & SCARD_STATE_IGNORE ) ||
			( stat.dwEventState & SCARD_STATE_UNAVAILABLE ) )
		{
			Definitions_SCReader::OnReaderGone info;
			info.Status = stat.dwEventState;

			mSCReader::NotifyFunctionOptPtr ptr;
			ptr.OnReaderGone = &info;
			AsyncEvent( me.MyReader , me.MyOption.OnReaderGone , &ptr );
		}
		return;
	}

	//�R�[���o�b�N
	static void AsyncEvent( mSCReader& view , const mSCReader::NotifyOption::NotifierInfo& info , mSCReader::NotifyFunctionOptPtr* opt )
	{
		if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_NONE )
		{
			//do nothing
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_WINDOWMESSAGE )
		{
			::PostMessageW( info.Notifier.Message.Hwnd , info.Notifier.Message.Message , (WPARAM)&view , info.Parameter );
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK )
		{
			if( info.Notifier.CallbackFunction )
			{
				while( mSCReader::NotifyOption::EnterNotifyEvent( info ) )
				{
					info.Notifier.CallbackFunction( view , info.Parameter , opt );
					if( !mSCReader::NotifyOption::LeaveNotifyEvent( info ) )
					{
						break;
					}
				}
			}
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_CALLBACK_PARALLEL )
		{
			info.Notifier.CallbackFunction( view , info.Parameter , opt );
		}
		else if( info.Mode == mSCReader::NotifyOption::NotifyMode::NOTIFY_SIGNAL )
		{
			if( info.Notifier.Handle != INVALID_HANDLE_VALUE )
			{
				SetEvent( info.Notifier.Handle );
			}
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�񓯊�����̊����ʒm���@���s���ł�" , info.Mode );
		}
		return;
	}
};

//�J�[�h���[�_�[�̊Ď����J�n����
bool mSCReader::StartWatch( const WString& readername , const NotifyOption& opt )
{
	//��d�N���`�F�b�N
	if( MyThread )
	{
		RaiseError( g_ErrorLogger , 0 , L"���łɃJ�[�h���[�_�[�̊Ď����ł�" );
		return false;
	}

	//�������ς݃`�F�b�N
	if( !OpenContext() )
	{
		return false;
	}

	//�X���b�h�̋N��
	MyThread = mNew SCReaderWatchThread( *this , MySCardContext , readername , opt );
	if( !MyThread->Begin() )
	{
		goto errorend;
	}
	if( !MyThread->Resume() )
	{
		goto errorend;
	}

	MyCardReaderName = readername;
	return true;

errorend:
	RaiseError( g_ErrorLogger , 0 , L"�Ď��X���b�h���N���ł��܂���ł���" );
	mDelete MyThread;
	MyThread = nullptr;
	MyCardReaderName = L"";
	return false;
}

//�J�[�h���[�_�[�̊Ď����I������
bool mSCReader::EndWatch( void )
{
	//�N���`�F�b�N
	if( !MyThread )
	{
		return true;
	}

	//�I���V�O�i���𑗐M
	if( !MyThread->FinishRequest() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�I���v�������s���܂���" );
		return false;
	}

	//�ҋ@���̕ύX�ʒm���L�����Z��
	//���I���V�O�i���Ƃ̏������Ԃɒ���
	LONG result = SCardCancel( MySCardContext );
	if( result != SCARD_S_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X�}�[�g�J�[�h�̑ҋ@���L�����Z���ł��܂���" );
		return false;
	}

	//�X���b�h�̏I���ҋ@
	if( !MyThread->End() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�X���b�h�̏I�����ł��܂���" );
		return false;
	}

	mDelete MyThread;
	MyThread = nullptr;
	MyCardReaderName = L"";
	return true;
}

const WString& mSCReader::GetCurrentCardReaderName( void )const
{
	return MyCardReaderName;
}
