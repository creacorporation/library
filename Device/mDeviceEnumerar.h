//----------------------------------------------------------------------------
// デバイス列挙クラス
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MDEVICEENUMERAR_H_INCLUDED
#define MDEVICEENUMERAR_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include "mDeviceEnumerarBase.h"
#include <vector>
#include <SetupAPI.h>
#include <devguid.h>

class mDeviceEnumerar : protected mDeviceEnumerarBase
{
public:
	//クラス名を用いてデバイスの一覧を取得する
	//ClassName : 対象とするクラス
	//IsSetupClass : true=セットアップクラス false=インターフェイスクラス
	mDeviceEnumerar( const WString& ClassName , bool IsSetupClass = true );

	//GUIDを用いてデバイスの一覧を取得する
	//・指定すべきGUIDは、「GUID_DEVCLASS_xxxx」というマクロでdevguid.hに定義されている
	//  例：GUID_DEVCLASS_PORTS→シリアルポート
	//・Bluetoothでは、bthdef.h、bthledef.hにもインターフェイスのGUID定義がある
	//　例：GUID_BLUETOOTHLE_DEVICE_INTERFACE→Bluetoothのデバイスインターフェイス
	//ClassGuid : 対象とするクラス
	//IsSetupClass : true=セットアップクラス false=インターフェイスクラス
	mDeviceEnumerar( const GUID& ClassGuid , bool IsSetupClass = true );

	virtual ~mDeviceEnumerar();

	struct DeviceCatalogEntry
	{
		DWORD index;			//カタログのインデックス
		WString FriendlyName;	//コントロールパネルの「フレンドリ名」と同じ
		WString Description;	//コントロールパネルの「デバイスの説明」と同じ
		WString HardwareId;		//コントロールパネルの「ハードウエアID」と同じ ※USBIDとかが取れる
		WString DevicePath;		//デバイスのパス(インターフェイスのときのみ)
	};
	using DeviceCatalog = std::deque< DeviceCatalogEntry >;

	//対象のクラスに存在するデバイスの一覧を取得する
	// retInfo : 結果格納先
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 成功時真
	bool GetDeviceCatalog( DeviceCatalog& retInfo , bool reload = true );

	//対象のクラスに存在するデバイスの一覧を取得する
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 成功時真
	const DeviceCatalog& GetDeviceCatalog( bool reload = true );

private:

	mDeviceEnumerar() = delete;
	mDeviceEnumerar( const mDeviceEnumerar& src ) = delete;
	const mDeviceEnumerar& operator=( const mDeviceEnumerar& src ) = delete;

	bool Reload( void );

	DeviceCatalog MyDeviceCatalog;
};


#endif
