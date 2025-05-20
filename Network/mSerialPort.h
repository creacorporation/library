//----------------------------------------------------------------------------
// シリアルポートハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSERIALPORT_H_INCLUDED
#define MSERIALPORT_H_INCLUDED

#include "mStandard.h"
#include <General/mCriticalSectionContainer.h>
#include <General/mNotifyOption.h>
#include <Thread/mWorkerThreadPool.h>
#include "mSerialPortBase.h"
#include <memory>

//COMポートハンドルのラッパー
class mSerialPort : public mSerialPortBase , public mSerialWriteStream
{
public:
	mSerialPort();
	virtual ~mSerialPort();

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

	//シリアルポートを開く
	bool Open( const Option& opt );

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//EOFは現在読み取れるデータがないことを示します
	//（時間が経てば再度読み取れるかもしれない）
	INT Read( void );

	//１文字書き込み
	virtual bool Write( INT data )override;

	//キャッシュを書き込み
	//これを呼ばないと実際の送信は発生しません
	virtual bool FlushCache( void )override;

	//接続を閉じます
	bool Abort( void );

	//通信対象のポートの名前を得ます
	virtual WString GetPortName( void )const override;

private:

	mSerialPort( const mSerialPort& src ) = delete;
	const mSerialPort& operator=( const mSerialPort& src ) = delete;

protected:

	//設定値
	Option MyOption;

};

#endif //MSERIALPORT_H_INCLUDED



