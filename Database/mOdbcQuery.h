//----------------------------------------------------------------------------
// ODBC接続用ライブラリ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MODBCQUERY_H_INCLUDED
#define MODBCQUERY_H_INCLUDED

#include "mStandard.h"
#include "../General/mTCHAR.h"

#include "mOdbc.h"
#include "mOdbcSqlState.h"
#include "mOdbcParams.h"
#include "mOdbcDescription.h"
#include "mOdbcConnectionHandle.h"

#include <vector>
#include <memory>
#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "odbc32.lib")

class mOdbcQuery final
{
public:

	mOdbcQuery();
	virtual ~mOdbcQuery();

	//クエリを開始する
	//※サーバーにより、パラメータがサブクエリやinner/outer joinなどのパラメータになっていると失敗することがあります。
	//  対策はmOdbc.hに記載のコメントを参考にしてください
	// query : クエリの文字列
	// ret : 成功時真
	bool Prepare( const WString& query );

	//クエリを開始する。パラメータの定義は手動で行う。
	//※パラメータがサブクエリやinner/outer joinなどのパラメータになっている場合、
	//  あるいはDBサーバーが対応しない場合、パラメータの設定は手動で行う必要があります。
	// query : クエリの文字列
	// desc : 手動定義するパラメータ情報（このパラメータの内容チェックは行いません。正しいものとして扱います。）
	// ret : 成功時真
	//※サブクエリやinnner joinを含むクエリの仕方(簡単バージョン。もちろん、descを全て手動で設定してもOK)
	//(1)本番のクエリとパラメータが同じだが、サブクエリ等を含まないダミーのクエリを作る
	//(2)Prepare()でダミーのクエリをセット(mOdbcParameterDescriptionが構築される)
	//(3)Prepare()のdescにGetParameterDescription()で得た参照をセットして、本番のクエリをセットする
	bool Prepare( const WString& query , const mOdbcParameterDescription& desc );

	//パラメータクエリを行うときの、パラメータ情報を取得する
	// ret : パラメータの情報
	//※パラメータがない場合も失敗はしません(返された参照のクラスにエントリがない状態になります)
	const mOdbcParameterDescription& GetParameterDescription( void )const;

	//クエリを実行する
	//※パラメータクエリで、パラメータがない場合
	// ret : 成功時真
	bool Execute( void );

	//クエリを実行する
	//※パラメータクエリで、パラメータを使用する場合
	// params : パラメータクエリのパラメータ
	// ret : 成功時真
	bool Execute( const mOdbcQueryParams& params );

	//クエリを実行する
	//※直接SQLを実行する場合
	// query : クエリの文字列
	// ret : 成功時真
	bool Execute( const WString& query );

	//SQL実行結果の列に関する情報を取得する
	// ret : パラメータの情報
	//※実行結果がない場合も失敗はしません(返された参照のクラスにエントリがない状態になります)
	const mOdbcResultDescription& GetResultDescription( void )const;

	//フェッチ処理結果
	enum FetchResult
	{
		FETCH_SUCCEEDED,	//成功
		FETCH_NOMOREDATA,	//成功：もうフェッチするデータがないため何も読み取られていない
		FETCH_TRUNCATED,	//成功：ただし、一部のデータはバッファ不足のため切り捨てられられている
		FETCH_UNKNOWNTYPE,	//エラー：データ型が不明（エラー発生位置までしか読み取られません）
	};

	//結果を取得する
	// retResult : 結果の格納先
	// ・結果セットに含まれる全ての列がセットされます
	// ・すでにretResultに入っているデータは破棄されます
	// ・要らない列まで取得しなくていいんですが→クエリのほうを直してください
	// ret 成功時真
	FetchResult Fetch( mOdbcResultParam& retResult );

	//初期状態でのバッファの最大値
	//列の最大長が長い場合(特にVARCHARのような列)、このサイズのバッファしか確保しない
	//※フェッチ用バッファサイズが小さすぎた場合は、フェッチしたデータが途中で切れてしまいます。
	//  この場合、同じデータを再取得することはできないので、全体を取得するには再度クエリからやり直すしかありません。
	const DWORD MAX_FETCH_BUFFER_SIZE = 8192;

	//指定された列のフェッチ用バッファサイズを変更する
	//Executeを行った時点では、フェッチ用バッファは最大でもMAX_FETCH_BUFFER_SIZEしか確保されていない
	//これより大きいサイズの列となる場合は、あらかじめこの関数を呼んで、バッファのサイズを指定します。
	//※フェッチ用バッファサイズが小さすぎた場合は、フェッチしたデータが途中で切れてしまいます。
	//  この場合、同じデータを再取得することはできないので、全体を取得するには再度クエリからやり直すしかありません。
	// colname : 設定対象の列の名前
	// buffsize : 確保するバイト数
	// ret : 成功時真
	bool ResizeFetchBuffer( const WString colname , size_t buffsize );

	//次にフェッチするデータが存在するかをチェックする
	// ret : まだフェッチしていないデータがあれば真
	bool IsDataExist( void )const;

	//同じクエリをパラメータを変更して再実行できるよう準備する
	//・読み取り中の結果は破棄します
	//・このコールを実行後、再度Execute()を呼び出すことができます
	// Prepare()→Execute()→Recycle()→Execute()→Recycle()→Execute()→...
	bool Recycle( void );

private:

	mOdbcQuery( const mOdbcQuery& source );					//コピー禁止クラス
	void operator=( const mOdbcQuery& source ) = delete;	//コピー禁止クラス

	friend class mOdbcConnection;

protected:

	//mOdbcConnection側から生死を判定するためのチケット
	std::shared_ptr< ULONG_PTR > MyDogtag;

	//ステートメントハンドル
	HSTMT MyStmt;

	//パラメータクエリに関する情報
	mOdbcParameterDescription MyParameterDescription;

	//SQLBindParameterのStrLen_or_IndPtrに渡すバッファのアレイ
	std::vector< SQLLEN > MyParameterLenArray;

	//結果セットに関する情報
	mOdbcResultDescription MyResultDescription;

	//フェッチしたデータを一時格納するバッファ
	struct FetchBufferEntry
	{
		SQLLEN size;				//ptrに確保されているバイト数
		SQLLEN value;				//フェッチしたサイズ・またはヌルかどうかを示す(ODBCに渡す用)
		std::unique_ptr<BYTE> ptr;	//フェッチ時の格納先(ODBCに渡す用)
	};
	typedef std::vector<FetchBufferEntry> FetchBuffer;
	FetchBuffer MyFetchBuffer;

	//まだフェッチしていないデータがある？
	//フェッチしていないデータがあれば真
	bool MyIsDataExist;

	//SQLRETURN型の結果コードが正常終了かどうかを判定する
	//追加情報がある場合は、SQLステートメントのメンバ(MyOdbcSqlState)を更新する
	//rc : 結果コード
	//ret : 結果コードが成功を示すものであれば真
	bool SQL_RESULT_CHECK( SQLRETURN rc );

	//指定したmOdbcResultDescriptionのエントリにフィットするように、MyFetchBufferを構築する。
	//また、結果セットにバッファをバインドする
	// desc : 構築元にする結果セットの情報
	// ret : 成功時真
	bool CreateFetchBuffer( const mOdbcResultDescription& desc );

	//descに指定した情報を使って、MyFetchBufferにバッファ情報を設定する。
	//ただし、確保するバイト数はsizeの指定に従う。
	bool SetFetchBuffer( const mOdbcDescriptionEntry& desc , SQLLEN size );

	//パラメータの型をC型に変換する
	SQLSMALLINT ParameterType2CType( mOdbc::ParameterType type )const;
};


#endif
