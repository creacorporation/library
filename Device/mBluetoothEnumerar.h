//----------------------------------------------------------------------------
// Bluetoothハンドラクラス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MBLUETOOTHENUMERAR_H_INCLUDED
#define MBLUETOOTHENUMERAR_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mBluetoothEnumerar
{
public:
	mBluetoothEnumerar();
	virtual ~mBluetoothEnumerar();

	struct BluetoothCatalogEntryBase
	{
		ULONGLONG Address;	//Bluetoothアドレス(48bit値です)
		WString Name;		//デバイス名
	};

	typedef BluetoothCatalogEntryBase BluetoothDeviceCatalogEntry;
	using BluetoothDeviceCatalog = std::deque< BluetoothDeviceCatalogEntry >;

	//指定した条件に合致するBluetoothのデバイス一覧を返します
	//※周辺機器の一覧が返ります
	// retCatalog : 結果の格納先
	// authed : 
	// remembered :
	// unknown : 
	// connected :
	// issue_inquiry : 
	// timeout_ms : 情報取得のタイムアウト(ms単位)。128ms単位のため、端数は切り上げます。また、最大6144msとなりそれ以上の値は最大値に補正されます。
	// ret : 成功時真
	static bool SearchDevice( BluetoothDeviceCatalog& retCatalog , bool authed , bool remembered , bool unknown , bool connected , bool issue_inquiry , DWORD timeout_ms );

	typedef BluetoothCatalogEntryBase BluetoothRadioCatalogEntry;
	using BluetoothRadioCatalog = std::deque< BluetoothRadioCatalogEntry >;

	//指定した条件に合致するBluetoothのデバイス一覧を返します
	//※本体に装着されているデバイスの一覧が返ります
	// retCatalog : 結果の格納先
	// ret : 成功時真
	static bool SearchRadio( BluetoothRadioCatalog& retCatalog );

	//
	struct BluetoothLECatalogEntryBase : public BluetoothCatalogEntryBase
	{
		WString DevicePath;
	};

private:
	mBluetoothEnumerar( const mBluetoothEnumerar& src ) = delete;
	const mBluetoothEnumerar& operator=( const mBluetoothEnumerar& src ) = delete;



};


#endif
