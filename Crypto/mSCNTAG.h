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

// PCSC 2.0 パート3 に対応しているリーダーなら動くはず
// 古いカードリーダーなどこの規格に対応しないものもあるので注意

class mSCNTAG : public mSCBase
{
public:
	mSCNTAG();
	virtual ~mSCNTAG();

	enum class PartNum
	{
		NTAG213,	//NXP NTAG213(又は互換品) ユーザメモリ144バイト(ページ04H～27H)
		NTAG215,	//NXP NTAG215(又は互換品) ユーザメモリ504バイト(ページ04H～81H)
		NTAG216,	//NXP NTAG216(又は互換品) ユーザメモリ888バイト(ページ04H～E1H)
		Unknown,
	};

	//指定のページ範囲を読み取ります
	//start_page 読み取る最初のページ
	//end_page 読み取る最後のページ
	//retData 読み取ったデータ
	//・ユーザーエリア、コンフィグにかかわらずどのページでも読めます
	bool Read( uint8_t start_page , uint8_t end_page , mBinary& retData )const;

	//指定のページにデータを書き込みます
	//page 書き込む先頭のページ
	//data 書き込むデータ
	//ret 書込み成功時真
	//・書込み先はユーザーエリアの範囲のみです。ユーザーエリア外に書き込もうとするとエラーになります。
	//・設定の書込みはそれぞれの専用の関数を呼んでください。
	bool Write( uint8_t page , const mBinary& data )const;

	//ベリファイを行います
	//page 先頭のページ
	//data 正解データ
	//・書込み時は内部的にベリファイを行っていますので、ユーザーが行う必要はありません。
	//・書込みを伴わずにデータの確認をしたい場合に使う想定です。
	bool Verify( uint8_t page , const mBinary& data )const;

	//64ビット値でUIDを返します
	//[例]  +0 +1 +2 +3
	//page0	11 22 33 88
	//page1	44 55 66 77
	//→ 返す値は0x0011223344556677u (※88はチェックサムなので無視)
	bool GetUid( int64_t& retId )const;

	//文字列でUIDを返します
	//[例]  +0 +1 +2 +3
	//page0	11 22 33 88
	//page1	44 55 66 77
	//→ 返す値は"11223344556677" (※88はチェックサムなので無視)
	bool GetUid( AString& retId )const;

	//文字列でUIDを返します
	//[例]  +0 +1 +2 +3
	//page0	11 22 33 88
	//page1	44 55 66 77
	//→ 返す値はL"11223344556677" (※88はチェックサムなので無視)
	bool GetUid( WString& retId )const;

	//64ビット値でUIDを返します
	//[例]  +0 +1 +2 +3
	//page0	11 22 33 88
	//page1	44 55 66 77
	//→ 返す値は0x0011223344556677u (※88はチェックサムなので無視)
	//エラーの場合負の値
	int64_t GetUidValue( void )const;

	//文字列でUIDを返します
	//[例]  +0 +1 +2 +3
	//page0	11 22 33 88
	//page1	44 55 66 77
	//→ 返す値は"11223344556677" (※88はチェックサムなので無視)
	//エラーの場合空文字列
	AString GetUidAString( void )const;

	//文字列でUIDを返します
	//[例]  +0 +1 +2 +3
	//page0	11 22 33 88
	//page1	44 55 66 77
	//→ 返す値はL"11223344556677" (※88はチェックサムなので無視)
	//エラーの場合空文字列
	WString GetUidWString( void )const;

	//品番を取得する
	//※CCの値から推定
	PartNum GetPartNum( void )const;

	//読み取り回数カウンタの読み取りをする
	//エラーの場合は負の値が返る
	int32_t GetReadCount( void )const;

	//パスワード認証する
	//成功時真
	bool Auth( uint32_t password , uint16_t pack )const;

	//署名を取得する
	//互換タグだと失敗したり、成功しても全部ゼロ埋めだったりする
	bool ReadSig( mBinary& retdata )const;

	//バージョン番号を取得する
	//互換タグだと失敗したり、成功しても全部ゼロ埋めだったりする
	bool GetVersion( mBinary& retdata )const;

	//アクセス設定
	struct AccessSetting
	{
		//真：読み書きにAUTHが必要、偽：書込みにAUTHが必要
		//※パスワードが無効(0xFFFFFFFFu)のときは、読み書きともAUTH不要
		bool ReadProtect = false;

		//真：コンフィグ領域(ACCESS等)変更禁止、偽：変更可
		//※一度禁止すると二度と戻せない
		bool ConfigLock = false;

		//読み取りカウンタを有効にするか
		//※読み取りを行った回数をカウントしている24ビットのカウンタ
		//真で有効
		bool EnableCounter = false;

		//読み取りカウンタの読み取り(READ_CNT)にパスワードを必要とするか
		//真で必要
		bool CounterProtect = false;

		//Capacity Containerの書込禁止を設定
		//真：書込禁止
		//※一度禁止すると二度と戻せない
		//※CC(Capability Container)の機能をつかった書込禁止フラグだが、
		//　アプリ解釈のため強制力はなく、アプリに無視されればそれまで
		bool NoWriteAccess = false;

		//パスワードがない場合に書込み禁止とする最初のページ番号
		//※ConfigLockを真にしたとき、Auth0がPWD,PACKのアドレスを含んでいない場合は、
		//  PWD,PACKのアドレスが含まれるように調整されます。
		//  (ConfigLockが真であっても、PWDとPACKは書き換え可能であるため)
		uint8_t Auth0 = 0xFFu;

		//認証成功時のACKの値
		uint16_t Pack = 0;

		//パスワード
		//※パスワードが無効(0xFFFFFFFFu)のときは、読み書きともAUTH不要
		uint32_t Password = 0xFFFFFFFFu;
	};

	//アクセス設定を行う
	bool SetAccessSetting( const AccessSetting& setting )const;

	//静的ロックのモード
	enum class StaticLockStatus
	{
		Unlocked,	//アンロック（データの読み書き可能、将来リードオンリー／読み書きモードへの変更可能）
		ReadOnly,	//リードオンリー（データの書込み不可、モードの変更不可）
		ReadWrite,	//読み書きモード（データの読み書き可能、モードの変更不可）
	};

	//静的ロックのモード
	struct StaticLock
	{
		enum class PageIndex
		{
			Page4 = 0,
			Page5,
			Page6,
			Page7,
			Page8,
			Page9,
			Page10,
			Page11,
			Page12,
			Page13,
			Page14,
			Page15,
			PageIndexMax
		};
		StaticLockStatus CC;
		StaticLockStatus Page[ (uint32_t)PageIndex::PageIndexMax ];

		StaticLock();
	};

	//静的ロックの設定
	//＜制約事項＞
	//・一度リードオンリー／読み書きモードに設定すると、もう変更できない
	//・ページ 4～ 9のいずれかを読み書きモードにする場合、ページ 4～ 9でアンロック状態のページは全て読み書きモードになる。リードオンリーのページは現状維持。
	//・ページ10～15のいずれかを読み書きモードにする場合、ページ10～15でアンロック状態のページは全て読み書きモードになる。リードオンリーのページは現状維持。
	bool SetStaticLock( const StaticLock& setting )const;

	//動的ロックの設定
	//0xAABBCCDD byte0=AA,byte1=BB,byte2=CC,byte3=DDのように書き込む
	//チップの品種によってビットの意味が違うので注意
	//Auth0など別の手段使った方が簡単
	bool SetDynamicLock( uint32_t setting )const;

	//ユーザーエリアの最小
	//※ユーザーエリアの先頭ページ
	static const uint8_t MinUserAreaPage = 4;

	//ユーザーエリアの最大
	//※ユーザーエリアの最後のページ
	uint8_t GetMaxUserAreaPage( void )const;

	//ユーザーエリアのバイト数
	uint32_t GetUserAreaSize( void )const;


protected:

	//接続時のカード個別の処理
	// ret : 処理成功時真
	virtual bool OnConnectCallback( void );

	//CRC計算
	uint16_t CalcCrc( const mBinary& data )const;

	//品番を取得する
	PartNum GetPartNum( TransparentSession& session )const;
	
	//ユーザーエリアの最大
	uint8_t GetMaxUserAreaPage( TransparentSession& session )const;

	//ユーザーエリアのバイト数
	uint32_t GetUserAreaSize( TransparentSession& session )const;

	//CCの値を取得する
	uint32_t GetCC( TransparentSession& session )const;

	//パーツ番号
	mutable PartNum MyPartNum = PartNum::Unknown;

private:
	mSCNTAG( const mSCNTAG& source );
	const mSCNTAG& operator=( const mSCNTAG& source ) = delete;

protected:

	bool ReadInternal( uint8_t start_page , uint8_t end_page , mBinary& retData , TransparentSession& session )const;
	bool WriteInternal( uint8_t page , const mBinary& data , TransparentSession& session , bool isuserarea , bool noverify )const;
	bool VerifyInternal( uint8_t page , const mBinary& data , TransparentSession& session )const;
	bool AuthInternal( uint32_t password , uint16_t pack , TransparentSession& session )const;

};


#endif