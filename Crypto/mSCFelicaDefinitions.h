//----------------------------------------------------------------------------
// Felica Light-S カードハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCDEFINITIONS_H_INCLUDED
#define MSCDEFINITIONS_H_INCLUDED

#include <mStandard.h>
#include <General/mBinary.h>

namespace mSCFelicaDefinitions
{
	//REGブロックのデータ構造
	struct RegData
	{
		DWORD RegA;
		DWORD RegB;
		mBinary RegC;

		RegData()
		{
			RegA = 0;
			RegB = 0;
		}
		bool operator<=( const RegData& r )const
		{
			return ( RegA <= r.RegA ) && ( RegB <= r.RegB );
		}
	};

	//読み書きを行うブロックのエントリ
	struct DataBlockEntry
	{
		//ブロックの番号
		DWORD BlockNumber;
		//データ（１６バイト）
		mBinary Data;
	};

	//読み書きを行うブロックのデータ
	using DataBlock = std::vector< DataBlockEntry >;

	//読み取りを行いたいブロックの番号
	using ReadRequestBlock = std::vector< DWORD >;

	//認証状態
	enum AuthStatus
	{
		//認証は行われていません
		Unauthorized,
		//内部認証が行われています。外部認証（相互認証）は行われていません。
		//※リーダーが、カードが真正なものであると認定しています
		//※カードは、リーダーが真正なものであるかは未確認です
		IntAuthorized,
		//内部認証・外部認証（相互認証）が行われています
		ExtAuthorized,
	};

	//読み取りのパーミッション
	enum ReadPermission
	{
		//認証不要で読み出しを許可する
		//※外部認証、MACともにあってもなくてもよい
		ReadEveryone,
		//読み出しには外部認証が必要
		//※外部認証は必須だが、MACはあってもなくてもよい
		ReadAuthUser,
	};

	//書き込みのパーミッション
	enum WritePermission
	{
		//書き込み禁止
		NotWritable,
		//認証不要で書き込みを許可する
		//※外部認証、MACともにあってもなくてもよい
		WriteEveryone,
		//書き込みには外部認証が必要
		//※外部認証は必須だが、MACはあってもなくてもよい
		WriteAuthUser,
		//書き込みには外部認証とMACが必要
		WriteAuthUserAndMac,
	};

	//ユーザーブロックの名前
	enum UserBlock
	{
		S_PAD0 = 0,
		S_PAD1 = 1,
		S_PAD2 = 2,
		S_PAD3 = 3,
		S_PAD4 = 4,
		S_PAD5 = 5,
		S_PAD6 = 6,
		S_PAD7 = 7,
		S_PAD8 = 8,
		S_PAD9 = 9,
		S_PAD10 = 10,
		S_PAD11 = 11,
		S_PAD12 = 12,
		S_PAD13 = 13,
		REG = 14,
	};

};

#endif
