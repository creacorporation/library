//----------------------------------------------------------------------------
// デバイス列挙クラス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSERIALPORTENUMERAR_H_INCLUDED
#define MSERIALPORTENUMERAR_H_INCLUDED

#include "mStandard.h"
#include "mDeviceEnumerar.h"

class mSerialPortEnumerar : protected mDeviceEnumerarBase
{
public:
	//IsSetupClass : true=セットアップクラス false=インターフェイスクラス
	mSerialPortEnumerar( bool IsSetupClass = true );
	virtual ~mSerialPortEnumerar();

	struct SerialPortCatalogEntry
	{
		DWORD index;
		WString FriendlyName;	//コントロールパネルの「フレンドリ名」と同じ
		WString Description;	//コントロールパネルの「デバイスの説明」と同じ
		WString HardwareId;		//コントロールパネルの「ハードウエアID」と同じ ※仮想COMポートの場合USBIDが取れる
		WString DevicePath;		//インタフェースクラスを取得した場合、インスタンスのパスがとれる
		WString PortName;		//ポート名(COMxx)
	};
	using SerialPortCatalog = std::deque< SerialPortCatalogEntry >;

	//存在するシリアルポートの一覧を取得する
	// retInfo : 結果格納先
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 成功時真
	bool GetSerialPortCatalog( SerialPortCatalog& retInfo , bool reload = true );

	//存在するシリアルポートの一覧を取得する
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 結果への参照
	const SerialPortCatalog& GetSerialPortCatalog( bool reload = true );


private:
	mSerialPortEnumerar( const mSerialPortEnumerar& src ) = delete;
	const mSerialPortEnumerar& operator=( const mSerialPortEnumerar& src ) = delete;

	bool Reload( void );
	SerialPortCatalog MySerialPortCatalog;
};


#endif
