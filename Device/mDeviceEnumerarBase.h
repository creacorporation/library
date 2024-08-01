//----------------------------------------------------------------------------
// デバイス列挙クラス
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MDEVICEENUMERARBASE_H_INCLUDED
#define MDEVICEENUMERARBASE_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include "mDeviceRegistry.h"
#include <vector>
#include <SetupAPI.h>
#include <devguid.h>

class mDeviceEnumerarBase
{
public:
	//クラス名を用いてデバイスの一覧を取得する
	//ClassName : 対象とするクラス
	//IsSetupClass : true=セットアップクラス false=インターフェイスクラス
	mDeviceEnumerarBase( const WString& ClassName , bool IsSetupClass = true );

	//GUIDを用いてデバイスの一覧を取得する
	//・指定すべきGUIDは、「GUID_DEVCLASS_xxxx」というマクロでdevguid.hに定義されている
	//  例：GUID_DEVCLASS_PORTS→シリアルポート
	//・Bluetoothでは、bthdef.h、bthledef.hにもインターフェイスのGUID定義がある
	//　例：GUID_BLUETOOTHLE_DEVICE_INTERFACE→Bluetoothのデバイスインターフェイス
	//ClassGuid : 対象とするクラス
	//IsSetupClass : true=セットアップクラス false=インターフェイスクラス
	mDeviceEnumerarBase( const GUID& ClassGuid , bool IsSetupClass = true );

	virtual ~mDeviceEnumerarBase();


private:

	mDeviceEnumerarBase() = delete;
	mDeviceEnumerarBase( const mDeviceEnumerarBase& src ) = delete;
	const mDeviceEnumerarBase& operator=( const mDeviceEnumerarBase& src ) = delete;
	
	//情報取得用のハンドル
	HDEVINFO MyHandle;

protected:

	//ハンドルを開放する
	void FreeDevHandle( void );

	//対象のクラスに存在するデバイスの一覧を実際に取得する
	// ret : 成功時真
	bool CreateCatalog( bool reload = true );

	//情報を保持しているデバイスクラスのGUID
	GUID MyClassGuid;

	//MyClassGuidがセットアップクラスのものかインターフェイスクラスのものか
	bool MyIsSetupClass;

	//取得した情報
	struct DevInfoDataEntry
	{
		SP_DEVINFO_DATA DevInfo;
		WString DevicePath;
	};
	using DevInfoData = std::vector< DevInfoDataEntry >;
	DevInfoData MyDevInfoData;

	//内部情報(DevInfoData)構築　セットアップクラス用
	bool BuildDevInfoData_Setup( void );
	//内部情報(DevInfoData)構築　インターフェイスクラス用
	bool BuildDevInfoData_Interface( void );

	//レジストリの取得
	// index : カタログのインデックス
	// retReg : 取得したレジストリのハンドル
	bool GetDeviceRegistry( DWORD index , mDeviceRegistry& retReg )const;

	//プロパティの取得
	// index : カタログのインデックス
	// prop_id : 取得するプロパティ
	// retProp : 取得した結果
	bool GetProperty( DWORD index , DWORD prop_id , WString& retProp)const;

	//プロパティの取得
	// info : デバイス情報セット
	// prop_id : 取得するプロパティ
	// retProp : 取得した結果
	bool GetProperty( const SP_DEVINFO_DATA& info , DWORD prop_id , WString& retProp)const;

};

#endif
