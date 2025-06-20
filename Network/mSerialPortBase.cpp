//----------------------------------------------------------------------------
// シリアルポートハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSerialPortBase.h"
#include <General/mErrorLogger.h>

mSerialPortBase::mSerialPortBase()
{
	MyHandle = INVALID_HANDLE_VALUE;
}

mSerialPortBase::~mSerialPortBase()
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
}

bool mSerialPortBase::ComPortSetting( HANDLE handle , const Option& setting )
{
	DCB dcb;

	//現在の設定を取得
	if( !GetCommState( handle , &dcb ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートの現在の設定を取得できませんでした" );
		return false;
	}

	//設定の更新

	//パリティ
	switch( setting.Parity )
	{
	case ParityType::PARITYTYPE_NOPARITY:
		dcb.Parity = NOPARITY;
		dcb.fParity = false;
		break;
	case ParityType::PARITYTYPE_EVEN:
		dcb.Parity = EVENPARITY;
		dcb.fParity = true;
		break;
	case ParityType::PARITYTYPE_ODD:
		dcb.Parity = ODDPARITY;
		dcb.fParity = true;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"パリティ設定が不正です" , (int)setting.Parity );
		return false;
	}
	//ストップビット
	switch( setting.StopBit )
	{
	case StopBitType::STOPBIT_ONE:
		dcb.StopBits = ONESTOPBIT;
		break;
	case StopBitType::STOPBIT_ONEFIVE:
		dcb.StopBits = ONE5STOPBITS;
		break;
	case StopBitType::STOPBIT_TWO:
		dcb.StopBits = TWOSTOPBITS;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"ストップビット設定が不正です" , (int)setting.StopBit );
		return false;
	}
	//ボーレート
	dcb.BaudRate = setting.BaudRate;

	//バイトサイズ
	if( 0xffu < setting.ByteSize )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"バイトサイズ設定が不正です" , setting.ByteSize );
		return false;
	}
	dcb.ByteSize = (BYTE)setting.ByteSize;

	//フローコントロール(DTR)
	switch( setting.DTRFlowControl )
	{
	case DTRFlowControlMode::ALWAYS_OFF:
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		break;
	case DTRFlowControlMode::ALWAYS_ON:
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		break;
	case DTRFlowControlMode::HANDSHAKE:
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"DTR設定が不正です" , (int)setting.DTRFlowControl );
		return false;
	}

	//フローコントロール(DSR)
	dcb.fOutxDsrFlow = setting.MonitorDSR;

	//フローコントロール(RTS)
	switch( setting.RTSFlowControl )
	{
	case RTSFlowControlMode::ALWAYS_OFF:
		dcb.fRtsControl = DTR_CONTROL_DISABLE;
		break;
	case RTSFlowControlMode::ALWAYS_ON:
		dcb.fRtsControl = DTR_CONTROL_ENABLE;
		break;
	case RTSFlowControlMode::HANDSHAKE:
		dcb.fRtsControl = DTR_CONTROL_HANDSHAKE;
		break;
	case RTSFlowControlMode::TOGGLE:
		dcb.fRtsControl = RTS_CONTROL_TOGGLE;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"RTS設定が不正です" , (int)setting.RTSFlowControl );
		return false;
	}

	//フローコントロール(CTS)
	dcb.fOutxCtsFlow = setting.MonitorCTS;

	//設定の適用
	if( !SetCommState( handle , &dcb ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートの設定を更新できませんでした" );
	}


	COMMTIMEOUTS timeout;
	GetCommTimeouts( handle , &timeout );
	timeout.ReadIntervalTimeout = setting.ReadBufferTimeout;
	timeout.ReadTotalTimeoutConstant = setting.ReadBufferTimeout;
	timeout.ReadTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = 0;
	timeout.WriteTotalTimeoutMultiplier = 0;
	if( !SetCommTimeouts( handle , &timeout ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"COMポートのタイムアウト設定を更新できませんでした" );
	}

	return true;
}
