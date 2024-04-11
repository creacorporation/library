//----------------------------------------------------------------------------
// デバイス用レジストリハンドル
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MDEVICEEREGISTRY_H_INCLUDED
#define MDEVICEEREGISTRY_H_INCLUDED

#include <General/mRegistry.h>

class mDeviceRegistry : public mRegistry
{

public:
	mDeviceRegistry();
	virtual ~mDeviceRegistry();

	//レジストリを開く
	bool Open( HDEVINFO devinfo , const SP_DEVINFO_DATA& infodata );

private:
	mDeviceRegistry( const mDeviceRegistry& src );
	const mDeviceRegistry& operator=( const mDeviceRegistry& src );
};


#endif

