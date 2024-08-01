//----------------------------------------------------------------------------
// ドライブ列挙クラス
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MDRIVEENUMERAR_H_INCLUDED
#define MDRIVEENUMERAR_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include "mDeviceEnumerarBase.h"
#include <vector>
#include <SetupAPI.h>
#include <devguid.h>

class mDriveEnumerar : protected mDeviceEnumerarBase
{
public:
	mDriveEnumerar();
	virtual ~mDriveEnumerar();

	struct CatalogEntry
	{
		DWORD index;				//カタログのインデックス
		WString FriendlyName;		//コントロールパネルの「フレンドリ名」と同じ
		WString Description;		//コントロールパネルの「デバイスの説明」と同じ
		WString HardwareId;			//コントロールパネルの「ハードウエアID」と同じ
		WString DevicePath;			//デバイスのパス
		DEVICE_TYPE DeviceType;		//デバイスタイプ(FILE_DEVICE_XXX)
		DWORD DeviceNumber;			//デバイス番号
		DWORD PartitionNumber;		//パーティション番号
		GUID DeviceGuid;			//デバイスのID(ハードウエアから生成される固有ID、ハードウエアから生成できない場合はランダム値)
		bool IsDeviceGuidFromHW;	//DeviceGuidがハードウエアから生成されていれば真。真の場合永続的。偽の場合は再起動するたびDeviceGuidが変わる。
		wchar_t Drive;				//ドライブにマウントされていればドライブレター。いなければ0。
	};
	using Catalog = std::deque< CatalogEntry >;

	//Catalogを再生成する
	bool Reload( void );

	//対象のクラスに存在するデバイスの一覧を取得する
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// retInfo : 結果格納先
	// ret : 成功時真
	bool GetCatalog( Catalog& retCatalog , bool reload = true );

	//対象のクラスに存在するデバイスの一覧を取得する
	// reload : 真の時再読み込みする。偽の時キャッシュがあればそれを使う。
	// ret : 結果への参照
	const Catalog& GetCatalog( bool reload = true );

	//指定したデバイスパスに対応するカタログのエントリを返す。
	// devicepath : 取得したいデバイスパス
	// ret : 得られたカタログへのポインタ。エラーの場合ヌル。
	const CatalogEntry* GetFromDevicePath( const WString DevicePath )const;

private:

	mDriveEnumerar( const mDriveEnumerar& src ) = delete;
	const mDriveEnumerar& operator=( const mDriveEnumerar& src ) = delete;


	//ドライブレターのスキャン
	bool ScanDriveLetter( void );

	//結果格納先
	Catalog MyCatalog;
};


#endif
