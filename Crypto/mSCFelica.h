//----------------------------------------------------------------------------
// Felica Light-S カードハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCFELICA_H_INCLUDED
#define MSCFELICA_H_INCLUDED

#include "mSCBase.h"
#include <General/mBinary.h>
#include <Crypto/mSCFelicaDefinitions.h>
#include <Crypto/mSCFelicaMac.h>

class mSCFelica : public mSCBase
{
public:
	mSCFelica();
	virtual ~mSCFelica();

	///-----------------------------------------
	/// 一次発行
	///-----------------------------------------

	//１次発行データ
	struct IssuanceParam
	{
		//IDブロック(82H)に存在するユーザーが任意に利用できる値
		//※「個別化カード鍵」を利用する場合は、この値は鍵生成のシードの一部になります。
		BYTE Id[ 6 ];

		//カード鍵バージョン(86H)の値。
		//カード鍵にバージョンを持たせるための値ですが、実質的にはユーザーが任意に利用可能。
		uint16_t KeyVersion;

		//カード鍵CK(87H)
		//・１６バイトのデータを指定したときは、そのバイナリをカード鍵とします
		//  （指定した鍵そのものをカードに書込みます）
		//・２４バイトのデータを指定したときは、そのバイナリを個別化マスター鍵とします
		//  （指定した鍵とIDブロックからソニー標準の方法で生成した鍵をカードに書込みます）
		//・ぴったり１６バイトまたは、２４バイトのデータを保持している状態でないとエラーになります
		mSecureBinary Key;

		//カード鍵の更新を許可するか(真の場合許可)
		//・更新許可にすると、一次発行でシステムブロックを書き換え禁止にした後はMACつき書き込みでカード鍵を更新できます
		//・更新不可にすると、一次発行でシステムブロックを書き換え禁止にした後はカード鍵を更新できなくなります
		//・システムブロックを書き換え禁止にするまでは、この設定に関わらずカード鍵を書き換え可能です
		bool EnableKeyChange;

		//STATEブロックの認証を要する場合は真
		//・要するに外部認証(相互認証)したい場合は真。しない場合は偽。
		//・一次発行時点では、一次発行～二次発行までの間に認証を使いたいかどうかで設定すればよく、
		//　最終的には、二次発行時にユーザーブロックのパーミッションの設定次第で値が決まります。
		bool EnableStateMacReq;

		IssuanceParam()
		{
			ZeroMemory( Id , sizeof( Id ) );
			Key.assign( 16 , 0x00 );
			KeyVersion = 0;
			EnableKeyChange = true;
			EnableStateMacReq = false;
		}
	};

	//一次発行を行います
	// lock : システムブロックをロックし、以降の書き換えを禁止します
	//    ロックを行うとWCNTの値(MAC付き書き込みに使用)がリセットされます。これをするまではわずかな回数しか(MACつきでの)書き込みができません。
	// param : 設定する一次発行のデータ
	// ret : 成功時真
	bool ExecFirstIssuance( bool lock , const IssuanceParam& param );

	///-----------------------------------------
	/// 二次発行
	///-----------------------------------------

	//ユーザーブロックの名前
	using UserBlock = mSCFelicaDefinitions::UserBlock;

	//読み取りのパーミッション
	using ReadPermission = mSCFelicaDefinitions::ReadPermission;

	//書き込みのパーミッション
	using WritePermission = mSCFelicaDefinitions::WritePermission;

	//ユーザーブロックのパーミッション
	struct Permission
	{
		//読み込みパーティション
		//0～13がユーザーブロックのS_PAD0～S_PAD13に対応し、14がREGに対応します。
		ReadPermission Read[ 15 ];
		//書き込みパーティション
		//0～13がユーザーブロックのS_PAD0～S_PAD13に対応し、14がREGに対応します。
		WritePermission Write[ 15 ];
	};

	//二次発行を行います
	// lock : パーミッションをロックし、以降の設定変更を禁止します
	// param : 設定するパーミッション
	// ret : 成功時真
	bool ExecSecondIssuance( bool lock , const Permission& param );

	///-----------------------------------------
	/// 認証
	///-----------------------------------------

	//内部認証または、外部（相互）認証を行います
	// key : カード鍵のバイナリ(ぴったり16or24バイトのデータを保持していること)
	//  16バイトの時そのバイナリをカード鍵、24バイトのとき個別化カード鍵のマスター鍵として処理します
	// int_auth_only : 真の場合、内部認証のみ行います
	//                    内部認証＝リーダーがカードを真正なもの(正しい鍵を持っている)と認めている状態
	//                 偽の場合、相互認証を行います(内部認証、外部認証の両方を行います)
	//                    相互認証＝リーダーとカードがお互いに真正なもの(正しい鍵を持っている)と認めている状態
	// ret : 成功時真
	bool ExecAuthentication( const mSecureBinary& key , bool int_auth_only );

	//認証状態
	using AuthStatus = mSCFelicaDefinitions::AuthStatus;

	//現在認証が行われているかを返します
	// ret : 現在の認証状態
	AuthStatus GetAuthStatus( void )const;

	//指定のバイナリを実際にカードにハッシュさせ、その結果を得ます
	//・カードが未認証状態のときしか使用できません
	//・返される値は、inをチャレンジとし、IDを取得したときのMACの値です
	// in : ハッシュするデータ
	//      16バイトに満たない場合は足りない分は後ろに00hを埋め、16バイトを超えるときは先頭の16バイトを使用します。
	// retHash : 結果格納先
	// ret : 成功時真
	bool CalcHash( const mBinary& in , mBinary& rethash );

	//カード鍵を更新する
	//・あらかじめ外部認証（相互認証）が成功している必要があります
	//・一次発行時に、カード鍵の更新が許可されている必要があります
	// key : カード鍵のバイナリ(ぴったり16or24バイトのデータを保持していること)
	//  16バイトの時そのバイナリをカード鍵、24バイトのとき個別化カード鍵のマスター鍵として処理します
	// keyver : 新しい鍵バージョン
	// ret : 成功時真
	bool UpdateKey( const mSecureBinary& key , uint16_t keyver );

	///-----------------------------------------
	/// 設定情報取得
	///-----------------------------------------

	//IDブロック(82H)に存在するユーザーが任意に利用できる値を取得します
	//※「個別化カード鍵」を利用する場合は、この値は鍵生成のシードの一部になります。
	// retVal : 取得した値
	// macauth : 真の場合、MAC_Aによる認証を行います
	//   ※真の場合、あらかじめ内部認証または、外部（相互）認証が完了している必要があります
	// ret : 成功時真
	bool GetUserID( mBinary& retVal , bool macauth = false )const;

	//鍵バージョンを取得します
	// retVal : 取得した値
	// macauth : 真の場合、MAC_Aによる認証を行います
	//   ※真の場合、あらかじめ内部認証または、外部（相互）認証が完了している必要があります
	// ret : 成功時真
	bool GetKeyVersion( uint16_t& retVal , bool macauth = false )const;

	//カード内部のROMが破損していないかを返します
	//・CRCによる不揮発性メモリの検証結果を返します。これはカード起動時に１回だけ行われるチェックです。
	// macauth : 真の場合、MAC_Aによる認証を行います
	//   ※真の場合、あらかじめ内部認証または、外部（相互）認証が完了している必要があります
	// ret : チェック合格の場合真、チェック不合格の場合偽
	bool GetChecksumResult( bool macauth = false )const;

	///-----------------------------------------
	/// 読み書き関連
	///-----------------------------------------

	//読み書きを行うブロックのエントリ
	using DataBlockEntry = mSCFelicaDefinitions::DataBlockEntry;
	//読み書きを行うブロックのデータ
	using DataBlock = mSCFelicaDefinitions::DataBlock;
	//読み取りを行いたいブロックの番号
	using ReadRequestBlock = mSCFelicaDefinitions::ReadRequestBlock;
	//ユーザーブロックの名前
	using UserBlock = mSCFelicaDefinitions::UserBlock;
	//REGデータ
	using RegData = mSCFelicaDefinitions::RegData;

	//データの読み取り
	//・ユーザーブロックのみ読み取りできます
	//  ブロック番号S_PAD0(00h)～REG(0Eh)の範囲。
	//・１度に通信できるサイズを超えて読み取り対象を指定した場合は、複数回に分割してこれを行います
	//・同じブロックを２回以上読み取ろうとするのは禁止です
	// request : 読み取りたいブロック番号の一覧
	// retResponse : 読み取り結果
	//   成功時、requestに指定したブロックの並び順に読み取ったデータが格納されることを保証します
	// macauth : 真の場合、MAC_Aによる認証を行います
	//   ※真の場合、あらかじめ内部認証または、外部（相互）認証が完了している必要があります
	// ret : 成功時真
	bool Read( const ReadRequestBlock& request , DataBlock& retResponse , bool macauth = false )const;

	//データの書き込み
	//・ユーザーブロックのみ書き込みできます
	//  ブロック番号S_PAD0(00h)～REG(0Eh)の範囲。enum UserBlockで名称を定義してあります。
	//・複数の書き込み対象を指定した場合は、１ブロックずつ分割して書き込みを行います。
	//  このとき、書込み処理が行われる順番は、dataの配列の並び順となります。
	//・同じブロックを２回以上書込もうとするのは禁止です
	// data : 書込むデータ
	// macauth : 真の場合、MAC_Aによる認証を行います
	//   ※真の場合、あらかじめ内部認証または、外部（相互）認証が完了している必要があります
	// ret : 成功時真
	bool Write( const DataBlock& data , bool macauth = false )const;

	//DataBlockEntryをRegDataに変換
	//・Readメソッドで取得したDataBlockをREG(減算)ブロックとして読み取ります
	// src : 読み取るデータブロック(ブロック番号がREGレジスタのものでなくても読み取ります)
	// ret : 読み取り結果
	RegData DataBlockEntryToRegData( const DataBlockEntry& src );

	//DataBlockEntryをRegDataに変換
	//・Readメソッドで取得したDataBlockをREG(減算)ブロックとして読み取ります
	// src : 読み取るデータブロック(ブロック番号がREGレジスタのものでなくても読み取ります)
	// ret : 読み取り結果
	void DataBlockEntryToRegData( const DataBlockEntry& src , RegData& retRegData );

	//RegDataをDataBlockEntryに変換
	//・REG(減算)ブロックを書き込み用にDataBlockEntry形式にエンコードします
	// src : 減算ブロック
	// ret : エンコード結果
	DataBlockEntry RegDataToDataBlockEntry( const RegData& src );

	//RegDataをDataBlockEntryに変換
	//・REG(減算)ブロックを書き込み用にDataBlockEntry形式にエンコードします
	// src : 減算ブロック
	// ret : エンコード結果
	void RegDataToDataBlockEntry( const RegData& src , DataBlockEntry& retDataBlockEntry );

private:

	mSCFelica( const mSCFelica& source );
	const mSCFelica& operator=( const mSCFelica& source ) = delete;

protected:

	//認証オブジェクト
	mutable mSCFelicaMac MyFelicaMac;

	//接続時のカード個別の処理
	// ret : 処理成功時真
	virtual bool OnConnectCallback( void );

	//データの読み取り
	// request : 読み取りたいブロック番号の一覧(最大4ブロック)
	// retResponse : 読み取り結果
	//   成功時、requestに指定したブロックの並び順に読み取ったデータが格納されることを保証します
	// ret : 成功時真
	bool RawRead( const ReadRequestBlock& request , DataBlock& retResponse )const;

	//データの書き込み
	// data : 書込むデータ
	//   成功時、dataに指定したブロックの並び順に書き込み処理を行います
	//   各エントリのDataが16バイトに満たない場合は、足りない分は00Hを補います。16バイトより多い場合は、先頭の16バイトのみ書込みます。
	// ret : 成功時真
	bool RawWrite( const DataBlock& data )const;

	//データの読み取り(MAC認証有効)
	//※あらかじめ内部認証または、外部（相互）認証が完了している必要があります
	// request : 読み取りたいブロック番号の一覧(最大3ブロック)
	// retResponse : 読み取り結果
	//   成功時、requestに指定したブロックの並び順に読み取ったデータが格納されることを保証します
	// ret : 成功時真
	bool MacReadInternal( const ReadRequestBlock& request , DataBlock& retResponse )const;

	//データの書き込み(MAC認証有効)
	//※あらかじめ外部（相互）認証が完了している必要があります
	// data : 書込むデータ
	// ret : 成功時真
	bool MacWriteInternal( const DataBlockEntry& data , DWORD wcnt )const;

	//WCNT値(90h)を取得します
	// ret : 成功時WCNT値。失敗時0xFFFFFFFFu。
	DWORD GetWCNT( void )const;

	//内部認証または、外部（相互）認証を行います
	// key : カード鍵のバイナリ(ぴったり16バイトのデータを保持していること)
	// int_auth_only : 真の場合、内部認証のみ行います
	//                 偽の場合、相互認証を行います(内部認証、外部認証の両方を行います)
	// ret : 成功時真
	bool ExecAuthenticationInternal( const mSecureBinary& key , bool int_auth_only );

};

#endif
