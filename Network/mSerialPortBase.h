//----------------------------------------------------------------------------
// シリアルポートハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSERIALPORTBASE_H_INCLUDED
#define MSERIALPORTBASE_H_INCLUDED

#include "mStandard.h"
#include "../General/mFileReadStreamBase.h"
#include "../General/mFileWriteStreamBase.h"
#include <memory>

namespace Definitions_SerialPortBase
{
enum class ParityType
{
	PARITYTYPE_NOPARITY ,	//パリティなし
	PARITYTYPE_EVEN ,		//偶数パリティ
	PARITYTYPE_ODD ,		//奇数パリティ
};

enum class StopBitType
{
	STOPBIT_ONE ,			//1ビット
	STOPBIT_ONEFIVE ,		//1.5ビット
	STOPBIT_TWO ,			//2ビット
};

//エラーが発生したタイミング
enum class ErrorAction
{
	ERROR_ON_CONNECT,	//接続処理中のエラー
	ERROR_ON_READ,		//読み込み中のエラー
	ERROR_ON_WRITE,		//書き込み中のエラー
};

//フローコントロール(DTR-DSR)
enum class DTRFlowControlMode
{
	ALWAYS_OFF,					//自分のDTRから常にオフを送信する
	ALWAYS_ON,					//自分のDTRから常にオンを送信する
	HANDSHAKE					//DTR-DSRのハンドシェイクを行う
};

//フローコントロール(RTS-CTS)
enum class RTSFlowControlMode
{
	ALWAYS_OFF,					//自分のRTSから常にオフを送信する
	ALWAYS_ON,					//自分のRTSから常にオンを送信する
	HANDSHAKE,					//RTS-CTSのハンドシェイクを行う(送信バッファにデータが50%たまるとオン/25%以下でオフ)
	TOGGLE,						//RTS-CTSのハンドシェイクを行う(送信バッファに1バイトでもデータがあればオン/空でオフ)
};

};

class mSerialReadStream : public mFileReadStreamBase
{
public:
	//読み取り側の経路が開いているかを判定します
	//開いている場合は真が返ります
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

	//書き込み側の経路が開いているかを判定します
	//開いている場合は真が返ります
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

class mSerialPortBase
{
public:
	mSerialPortBase();
	virtual ~mSerialPortBase();

	//パリティの設定
	using ParityType = Definitions_SerialPortBase::ParityType;

	//ストップビット長
	using StopBitType = Definitions_SerialPortBase::StopBitType;

	//ファイル設定
	using FileOption = mFile::Option;

	//ファイルを開くときのモード
	using CreateMode = mFile::CreateMode;

	//フローコントロール
	using DTRFlowControlMode = Definitions_SerialPortBase::DTRFlowControlMode;

	//フローコントロール
	using RTSFlowControlMode = Definitions_SerialPortBase::RTSFlowControlMode;

	//オプション構造体
	struct Option
	{
		//-----------
		//送受信設定
		//-----------
		FileOption Fileinfo;
		DWORD BaudRate;			//ボーレート
		ParityType Parity;		//パリティ
		StopBitType StopBit;	//ストップビット
		DWORD ByteSize;			//1バイトが何ビットか。普通は8。

		//-----------
		//バッファリング設定
		//-----------
		DWORD ReadPacketSize;			//読み込みパケットのサイズ
		DWORD ReadBufferTimeout;		//任意のバイトの受信間隔(ミリ秒)がこの値を上回ったら受信通知を生成する
		DWORD WritePacketSize;			//書き込みパケットのサイズ

		//-----------
		//フローコントロール
		//-----------
		DTRFlowControlMode DTRFlowControl;	//DTR信号を制御するか
		bool MonitorDSR;					//相手から送られてくるDSRを利用するか(true=利用する/false=無視する)
		RTSFlowControlMode RTSFlowControl;	//RTS信号を制御するか
		bool MonitorCTS;					//相手から送られてくるCTSを利用するか(true=利用する/false=無視する)

		//-----------
		//初期値
		//-----------
		Option()
		{
			BaudRate = 9600;
			Parity = ParityType::PARITYTYPE_NOPARITY;
			StopBit = StopBitType::STOPBIT_ONE;
			ByteSize = 8;
			ReadPacketSize = 128;
			ReadBufferTimeout = 100;
			WritePacketSize = 128;
			DTRFlowControl = DTRFlowControlMode::ALWAYS_ON;
			MonitorDSR = false;
			RTSFlowControl = RTSFlowControlMode::ALWAYS_ON;
			MonitorCTS = false;
		}
	};

	//通信対象のポートの名前を得ます
	virtual WString GetPortName( void )const = 0;

private:
	mSerialPortBase( const mSerialPortBase& src ) = delete;
	const mSerialPortBase& operator=( const mSerialPortBase& src ) = delete;

protected:

	//COMポートのハンドル
	HANDLE MyHandle;

	//COMポートの設定を行う
	bool ComPortSetting( HANDLE handle , const Option& setting );

};


#endif
