//----------------------------------------------------------------------------
// Mifare Classic UL カードハンドラ
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCNTAG_H_INCLUDED
#define MSCNTAG_H_INCLUDED

#include "mSCBase.h"
#include <array>
#include <General/mBinary.h>

class mSCNTAG : public mSCBase
{
public:
	mSCNTAG();
	virtual ~mSCNTAG();

	bool Read( uint8_t start_page , uint8_t end_page , mBinary& retData )const;
	bool Write( uint8_t page , const mBinary& data )const;

protected:

	//接続時のカード個別の処理
	// ret : 処理成功時真
	virtual bool OnConnectCallback( void );

	//CRC計算
	uint16_t CalcCrc( const mBinary& data )const;

private:
	mSCNTAG( const mSCNTAG& source );
	const mSCNTAG& operator=( const mSCNTAG& source ) = delete;

	bool ReadInternal( uint8_t start_page , uint8_t end_page , mBinary& retData , TransparentSession& session )const;
	bool WriteInternal( uint8_t page , const mBinary& data , TransparentSession& session )const;
};


#endif