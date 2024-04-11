//----------------------------------------------------------------------------
// Felica Light-S カードハンドラ
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSCFelica.h"
#include <General/mErrorLogger.h>
#include <unordered_set>
#include <iterator>

mSCFelica::mSCFelica()
{
}


mSCFelica::~mSCFelica()
{
}

bool mSCFelica::OnConnectCallback( void )
{
	//ポーリングを行い、セットされたカードがFelica Lite-Sかどうか同定を行う

	//送信データの構築
	TransmitData dt;
	SetDirectCommand( dt );
	dt.data.push_back( 0x06u );	//データ長(このバイトも含む)
	dt.data.push_back( 0x00u );	//pollingコマンド
	dt.data.push_back( 0xFFu );	//応答対象システムコード(FFFFH=全てのカード)の上位
	dt.data.push_back( 0xFFu );	//応答対象システムコード(FFFFH=全てのカード)の下位
	dt.data.push_back( 0x01u );	//システムコードをリクエスト
	dt.data.push_back( 0x00u );	//応答可能なタイムスロットは#0のみ
	//通信実施
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	//失敗した場合は２バイトでくるはず
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//失敗応答
			RaiseError( g_ErrorLogger , 0 , L"スマートカードとの操作が失敗しました" );
			return false;
		}
	}
	//データ長の検証
	if( rsp.data.size() != 22 )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカード応答サイズが不正です" );
		return false;
	}
	//固定のデータが想定通りか検証
	if( rsp.data[ 0 ] != 0x14u ||	//想定データ長
		rsp.data[ 1 ] != 0x01u ||	//レスポンスコード
		rsp.data[ 20 ] != 0x90u ||	//成功応答１バイト目
		rsp.data[ 21 ] != 0x00u )	//成功応答２バイト目
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
		return false;
	}
	//このライブラリはFelica Lite-Sのみの対応なので、それ以外をはじく
	if( rsp.data[ 18 ] != 0x88u ||	//Felica Lite-S のシステムコードの上位バイト
		rsp.data[ 19 ] != 0xB4u )	//Felica Lite-S のシステムコードの下位バイト
	{
		RaiseError( g_ErrorLogger , 0 , L"Felica Liteではありません" );
		return false;
	}
	//製造パラメータのチェック
	// Felica Lite  のIC種別は 0xF0
	// Felica Lite-SのIC種別は 0xF1～0xF7
	if( rsp.data[ 11 ] < 0xF1u || 0xF7u < rsp.data[ 11 ] )
	{
		RaiseError( g_ErrorLogger , 0 , L"Felica Lite-Sではありません" );
		return false;
	}
	return true;
}

bool mSCFelica::RawRead( const ReadRequestBlock& request , DataBlock& retResponse )const
{
	retResponse.clear();
	if( request.size() == 0 || 4 < request.size() )
	{
		RaiseError( g_ErrorLogger , request.size() , L"読み出し要求のブロック数が正しくありません" );
		return false;
	}

	//送信データの構築
	TransmitData dt;
	SetDirectCommand( dt );
	dt.data.push_back( 0x00u );	//データ長(このバイトも含む) ※仮入力。最後に書き換える。
	dt.data.push_back( 0x06u );	//Read Without Encryptionコマンド
	dt.data.append( MyCardID );	//要求先のカードUID
	dt.data.push_back( 0x01u );	//サービス数。１固定。
	dt.data.push_back( 0x0Bu );	//サービスの１個中１個目（下位バイト）※RWとROブロックにアクセス可能＝0x000B
	dt.data.push_back( 0x00u );	//サービスの１個中１個目（上位バイト）  RWブロックのみにアクセス可能＝0x0009
	dt.data.push_back( (BYTE)request.size() );	//要求するブロックの数
	for( ReadRequestBlock::const_iterator itr = request.begin() ; itr != request.end() ; itr++ )
	{
		dt.data.push_back( 0x80u );	//２バイトエレメント。リスト順とアクセスモードはFeilcaLiteSでは常に0とする。
		if( 0xFFu < *itr )
		{
			RaiseError( g_ErrorLogger , *itr , L"ブロック番号が不正です" );
			return false;
		}
		dt.data.push_back( (BYTE)*itr );	//読み込みたいブロックの番号
	}
	//仮入力したデータ長を実際の値に書き換える
	dt.data[ 0 ] = (BYTE)dt.data.size();

	//通信実施
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		return false;
	}
	//失敗した場合は２バイトでくるはず
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//失敗応答
			RaiseError( g_ErrorLogger , 0 , L"スマートカードとの操作が失敗しました" );
			return false;
		}
	}
	//データ長の検証
	// 1 = データ長１バイト
	// 11 = レスポンスコード１バイト＋IDm８バイト＋ステータスフラグ1/2各１バイト
	// 1 = ブロック数１バイト(成功時のみ)
	// 16n = 実データ１６バイト×読み取ったブロック数(成功時のみ)
	// 2 = PC/SCの成功応答(9000H)２バイト
	DWORD correct_size_success = 1 + 11 + 1 + ( 16 * (DWORD)request.size() ) + 2;
	DWORD correct_size_fail = 1 + 11 + 2;
	if( rsp.data.size() == correct_size_fail )
	{
		RaiseError( g_ErrorLogger , ( rsp.data[ 10 ] << 8 ) + ( rsp.data[ 11 ] ) , L"スマートカードが失敗を返しました" );
		return false;
	}
	else if( rsp.data.size() != correct_size_success )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカード応答サイズが不正です" );
		return false;
	}
	//ヘッダ情報の検証
	if( ( rsp.data[ 0 ] != (BYTE)( correct_size_success - 2 ) ) ||	//データ長
		( rsp.data[ 1 ] != 0x07u ) ||								//レスポンスコード
		( rsp.data[ 12 ] != request.size() ) ||						//ブロック数
		( rsp.data[ rsp.data.size() - 2 ] != 0x90u ) ||				//PC/SCの成功応答
		( rsp.data[ rsp.data.size() - 1 ] != 0x00u ) )				//PC/SCの成功応答
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
		return false;
	}
	//IDmが合致しているか
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		if( MyCardID[ i ] != rsp.data[ i + 2 ] )
		{
			RaiseError( g_ErrorLogger , 0 , L"レスポンスのIDが一致しません" );
			return false;
		}
	}
	//検証ＯＫなのでブロックデータを設定
	retResponse.resize( request.size() );
	for( DWORD i = 0 ; i < request.size() ; i++ )
	{
		retResponse[ i ].BlockNumber = request[ i ];
		retResponse[ i ].Data = rsp.data.subdata( 13 + 16 * i , 16 );
	}
	rsp.data.secure_erase();
	return true;
}

//データの書き込み
bool mSCFelica::RawWrite( const DataBlock& data )const
{
	bool result = false;
	if( data.size() == 0 || 4 < data.size() )
	{
		RaiseError( g_ErrorLogger , data.size() , L"書き込み要求のブロック数が正しくありません" );
		return false;
	}

	//送信データの構築
	TransmitData dt;
	SetDirectCommand( dt );
	dt.data.push_back( 0x00u );	//データ長(このバイトも含む) ※仮入力。最後に書き換える。
	dt.data.push_back( 0x08u );	//Write Without Encryptionコマンド
	dt.data.append( MyCardID );	//要求先のカードUID
	dt.data.push_back( 0x01u );	//サービス数。１固定。
	dt.data.push_back( 0x09u );	//サービスの１個中１個目（下位バイト）※RWとROブロックにアクセス可能＝0x000B
	dt.data.push_back( 0x00u );	//サービスの１個中１個目（上位バイト）  RWブロックのみにアクセス可能＝0x0009
	dt.data.push_back( (BYTE)data.size() );	//要求するブロックの数
	for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		dt.data.push_back( 0x80u );	//２バイトエレメント。リスト順とアクセスモードはFeilcaLiteSでは常に0とする。
		if( 0xFFu < itr->BlockNumber )
		{
			RaiseError( g_ErrorLogger , itr->BlockNumber , L"ブロック番号が不正です" );
			return false;
		}
		dt.data.push_back( (BYTE)itr->BlockNumber );	//読み込みたいブロックの番号
	}
	for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		DWORD length = (DWORD)itr->Data.size();
		if( length <= 16 )
		{
			//１６バイト以下の場合は、データをコピーし１６バイトに満たない部分を０で埋める
			dt.data.append( itr->Data );
			for( ; length < 16 ; length++ )
			{
				dt.data.push_back( 0x00 );
			}
		}
		else
		{
			//１６バイトより多い場合は、先頭の１６バイトをコピーする
			dt.data.append( itr->Data.subdata( 0 , 16 ) );
		}
	}
	//仮入力したデータ長を実際の値に書き換える
	dt.data[ 0 ] = (BYTE)dt.data.size();

	//通信実施
	ResponseData rsp;
	if( !Communicate( dt , rsp ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードとの通信が失敗しました" );
		goto ending;
	}
	//失敗した場合は２バイトでくるはず
	if( rsp.data.size() == 2 )
	{
		if( rsp.data[ 0 ] == 0x63 && rsp.data[ 1 ] == 0x00 )
		{
			//失敗応答
			RaiseError( g_ErrorLogger , 0 , L"スマートカードとの操作が失敗しました" );
			goto ending;
		}
	}
	//データ長の検証
	// 1 = データ長１バイト
	// 11 = レスポンスコード１バイト＋IDm８バイト＋ステータスフラグ1/2各１バイト
	// 2 = PC/SCの成功応答(9000H)２バイト
	if( rsp.data.size() != ( 1 + 11 + 2 ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカード応答サイズが不正です" );
		goto ending;
	}
	//ヘッダ情報の検証
	if( ( rsp.data[ 0 ] != 12 ) ||					//データ長
		( rsp.data[ 1 ] != 0x09u ) ||				//レスポンスコード
		( rsp.data[ 12 ] != 0x90u ) ||				//PC/SCの成功応答
		( rsp.data[ 13 ] != 0x00u ) )				//PC/SCの成功応答
	{
		RaiseError( g_ErrorLogger , 0 , L"スマートカードの応答が不正です" );
		goto ending;
	}
	//IDmが合致しているか
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		if( MyCardID[ i ] != rsp.data[ i + 2 ] )
		{
			RaiseError( g_ErrorLogger , 0 , L"レスポンスのIDが一致しません" );
			goto ending;
		}
	}
	//ヘッダ情報の検証
	if( ( rsp.data[ 10 ] != 0x00u ) ||				//ステータスフラグ１
		( rsp.data[ 11 ] != 0x00u ) )				//ステータスフラグ２
	{
		RaiseError( g_ErrorLogger , ( rsp.data[ 10 ] << 8 ) + ( rsp.data[ 11 ] ) , L"スマートカードが失敗を返しました" );
		goto ending;
	}

	result = true;
ending:
	dt.data.secure_erase();
	rsp.data.secure_erase();
	return result;

}



bool mSCFelica::MacReadInternal( const ReadRequestBlock& request , DataBlock& retResponse )const
{
	retResponse.clear();
	if( request.size() == 0 || 3 < request.size() )
	{
		RaiseError( g_ErrorLogger , request.size() , L"読み込み要求のブロック数が正しくありません" );
		return false;
	}

	//読み取りの実行
	ReadRequestBlock req = request;
	req.push_back( 0x91 );	//MAC_A
	if( !RawRead( req , retResponse ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"読み取りが失敗しました" );
		return false;
	}

	//MACの検証
	if( !MyFelicaMac.ValidateMacA( retResponse ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_Aの検証が失敗しました" );
		retResponse.pop_back();
		return false;
	}
	retResponse.pop_back();
	return true;
}


bool mSCFelica::GetUserID( mBinary& retVal , bool macauth )const
{
	DataBlock dt;
	retVal.clear();

	//読み込み
	ReadRequestBlock req;
	req.push_back( 0x82 );

	bool result;
	if( macauth )
	{
		result = MacReadInternal( req , dt );
	}
	else
	{
		result = RawRead( req , dt );
	}

	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"IDを読み取れません" );
		return false;
	}
	retVal = dt[ 0 ].Data.subdata( 10 , 6 );
	return true;
}

bool mSCFelica::GetKeyVersion( uint16_t& retVal , bool macauth )const
{
	DataBlock dt;
	retVal = 0;

	//読み込み
	ReadRequestBlock req;
	req.push_back( 0x86 );

	bool result;
	if( macauth )
	{
		result = MacReadInternal( req , dt );
	}
	else
	{
		result = RawRead( req , dt );
	}

	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"鍵バージョンを読み取れません" );
		return false;
	}
	retVal = ( dt[ 0 ].Data[ 1 ] << 8 ) + dt[ 0 ].Data[ 0 ];
	return true;
}

bool mSCFelica::ExecFirstIssuance( bool lock , const IssuanceParam& param )
{
	//鍵のチェック
	if( !MyFelicaMac.CheckIsWeakKey( param.Key ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"指定した鍵は脆弱鍵のリストに掲載されています" );
		return false;
	}

	//IDの設定を行う
	mBinary IdBlockData;
	{
		//書き込み
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x82;
		writedata[ 0 ].Data.assign( 16 , 0x00u );
		for( DWORD i = 0 ; i < 8 ; i++ )
		{
			writedata[ 0 ].Data[ i ] = MyCardID[ i ];
		}
		for( DWORD i = 0 ; i < 6 ; i++ )
		{
			writedata[ 0 ].Data[ 10 + i ] = param.Id[ i ];
		}
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"IDを書き込めません" );
			return false;
		}
		//鍵の書き込みに使うので保存
		IdBlockData = writedata[ 0 ].Data;
		//ベリファイ
		ReadRequestBlock req;
		req.push_back( 0x82 );	//ID
		if( !RawRead( req , writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"IDを読み込めません" );
			return false;
		}
		for( DWORD i = 0 ; i < 16 ; i++ )
		{
			if( IdBlockData[ i ] != writedata[ 0 ].Data[ i ] )
			{
				RaiseError( g_ErrorLogger , 0 , L"IDベリファイエラー" );
				return false;
			}
		}
	}
	//鍵の書き込みを行う
	{
		//書き込み
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x87;	//CK
		if( param.Key.size() == 16 )
		{
			//カード鍵
			writedata[ 0 ].Data.assign( 16 , 0x00u );
			for( DWORD i = 0 ; i < 16 ; i++ )
			{
				writedata[ 0 ].Data[ i ] = param.Key[ i ];
			}
		}
		else if( param.Key.size() == 24 )
		{
			//マスター鍵
			mSecureBinary ck;
			if( !mSCFelicaMac::CalcDiversifiedKey( param.Key , IdBlockData , ck ) )
			{
				RaiseError( g_ErrorLogger , 0 , L"個別化カード鍵の生成が失敗しました" );
				return false;
			}
			for( DWORD i = 0 ; i < 16 ; i++ )
			{
				writedata[ 0 ].Data[ i ] = ck[ i ];
			}
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"キーの長さが違います" );
			return false;
		}
		if( !RawWrite( writedata ) )
		{
			writedata[ 0 ].Data.secure_erase();
			RaiseError( g_ErrorLogger , 0 , L"キーを書き込めません" );
			return false;
		}
		writedata[ 0 ].Data.secure_erase();
		//ベリファイを行う
		if( !ExecAuthenticationInternal( param.Key , true ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"キーベリファイエラー" );
			return false;
		}
	}
	//カード鍵のバージョンを書込む
	{
		//書き込み
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x86;	//CK
		writedata[ 0 ].Data.push_back( ( param.KeyVersion >> 0 ) & 0xFFu );
		writedata[ 0 ].Data.push_back( ( param.KeyVersion >> 8 ) & 0xFFu );
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"キーバージョンを書き込めません" );
			return false;
		}
		uint16_t readversion;
		if( !GetKeyVersion( readversion , false ) ||
			( readversion != param.KeyVersion ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"キーバージョンベリファイエラー" );
			return false;
		}
	}
	//メモリコンフィグレーションブロック
	{
		//・メモリコンフィグレーションブロックは、現在値を更新する形で書込むので、まずは現在の設定を読み込む
		DataBlock writedata;
		ReadRequestBlock req;
		req.push_back( 0x88 );	//メモリコンフィグレーションブロック
		if( !RawRead( req , writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションを読み取れません" );
			return false;
		}
		//メモリコンフィグレーションブロックのデータ更新
		//byte12 MemoryConfig
		writedata[ 0 ].Data[ 12 ] = ( param.EnableStateMacReq ) ? ( 1 ) : ( 0 );
		//byte5 CK、CKVへの書き込み可否
		writedata[ 0 ].Data[ 5 ] = ( param.EnableKeyChange ) ? ( 1 ) : ( 0 );
		//byte4 RFパラメータ
		//※書き込み時は必ず07Hを書込めと書いてある
		writedata[ 0 ].Data[ 4 ] = 7;
		//byte3 NDEF設定
		//※対応なら01H、非対応なら00H
		writedata[ 0 ].Data[ 3 ] = 0;
		//byte2 アクセス権設定
		writedata[ 0 ].Data[ 2 ] = ( lock ) ? ( 0x00u ) : ( 0xFFu );

		//書き込み
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションを書き込めません" );
			return false;
		}
		//ベリファイを行う
		DataBlock response;
		if( !RawRead( req , response ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションを読み込めません" );
			return false;
		}
		for( DWORD i = 0 ; i < 16 ; i++ )
		{
			if( response[ 0 ].Data[ i ] != writedata[ 0 ].Data[ i ] )
			{
				RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションベリファイエラー" );
				return false;
			}
		}
	}
	return true;
}

bool mSCFelica::ExecSecondIssuance( bool lock , const Permission& param )
{

	uint16_t ReadWriteAccess = 0;	//byte0-1 読み取り専用
	uint16_t ReqAuthR = 0;			//byte6-7 読み取りに外部認証が必要
	uint16_t ReqAuthW = 0;			//byte8-9 書き込みに外部認証が必要
	uint16_t ReqMacW = 0;			//byte10-11 書き込みにMACつきアクセスが必要
	bool ReqMacStateBlock = false;	//byte12 STATEブロックへの書き込みにMACつきアクセスが必要

	//書込み用データのスキャン
	for( int i = 14 ; 0 <= i ; i-- )
	{
		ReqAuthR <<= 1;
		switch( param.Read[ i ] )
		{
		case ReadPermission::ReadEveryone:
			break;
		case ReadPermission::ReadAuthUser:
			ReqAuthR |= 0x0001u;
			break;
		default:
			RaiseError( g_ErrorLogger , i , L"読み取りアクセス権の指定が正しくありません" );
			return false;
		}

		ReadWriteAccess <<= 1;
		ReqAuthW <<= 1;
		ReqMacW <<= 1;
		switch( param.Write[ i ] )
		{
		case WritePermission::NotWritable:
			break;
		case WritePermission::WriteEveryone:
			ReadWriteAccess |= 0x0001u;
			break;
		case WritePermission::WriteAuthUser:
			ReadWriteAccess |= 0x0001u;
			ReqAuthW |= 0x0001u;
			break;
		case WritePermission::WriteAuthUserAndMac:
			ReadWriteAccess |= 0x0001u;
			ReqAuthW |= 0x0001u;
			ReqMacW |= 0x0001u;
			break;
		default:
			RaiseError( g_ErrorLogger , i , L"書き込みアクセス権の指定が正しくありません" );
			return false;
		}
	}

	//MCブロック自体のアクセス権(ロックなしならRWとして1を立てる)
	if( !lock )
	{
		ReadWriteAccess |= 0x8000u;
	}

	//STATEブロックにMACが必要か判定
	if( ReqAuthR || ReqAuthW )
	{
		ReqMacStateBlock = true;
	}
	else
	{
		ReqMacStateBlock = false;
	}

	//メモリコンフィグレーションブロックの更新
	{
		//・メモリコンフィグレーションブロックは、現在値を更新する形で書込むので、まずは現在の設定を読み込む
		DataBlock writedata;
		ReadRequestBlock req;
		req.push_back( 0x88 );	//メモリコンフィグレーションブロック
		if( !RawRead( req , writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションを読み取れません" );
			return false;
		}
		//メモリコンフィグレーションブロックのデータ更新
		//byte12 MemoryConfig
		writedata[ 0 ].Data[ 12 ] = ( ReqMacStateBlock ) ? ( 1 ) : ( 0 );
		writedata[ 0 ].Data[ 11 ] = (BYTE)( ( ReqMacW >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[ 10 ] = (BYTE)( ( ReqMacW >> 0 ) & 0xFFu );
		writedata[ 0 ].Data[  9 ] = (BYTE)( ( ReqAuthW >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[  8 ] = (BYTE)( ( ReqAuthW >> 0 ) & 0xFFu );
		writedata[ 0 ].Data[  7 ] = (BYTE)( ( ReqAuthR >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[  6 ] = (BYTE)( ( ReqAuthR >> 0 ) & 0xFFu );
		writedata[ 0 ].Data[  1 ] = (BYTE)( ( ReadWriteAccess >> 8 ) & 0xFFu );
		writedata[ 0 ].Data[  0 ] = (BYTE)( ( ReadWriteAccess >> 0 ) & 0xFFu );

		//書き込み
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションを書き込めません" );
			return false;
		}
		//ベリファイを行う
		DataBlock response;
		if( !RawRead( req , response ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションを読み込めません" );
			return false;
		}
		for( DWORD i = 0 ; i < 16 ; i++ )
		{
			if( response[ 0 ].Data[ i ] != writedata[ 0 ].Data[ i ] )
			{
				RaiseError( g_ErrorLogger , 0 , L"メモリコンフィグレーションベリファイエラー" );
				return false;
			}
		}
	}
	return true;
}


bool mSCFelica::ExecAuthentication( const mSecureBinary& key , bool int_auth_only )
{
	if( key.size() == 16 )
	{
		//カード鍵なのでそのまま使う
		return ExecAuthenticationInternal( key , int_auth_only );
	}
	else if( key.size() == 24 )
	{
		//マスター鍵なのでカード鍵を導出
		ReadRequestBlock idreq;
		DataBlock idrsp;
		idreq.push_back( 0x82u );
		if( !RawRead( idreq , idrsp ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"IDを読み取れません" );
			MyFelicaMac.ClearKey();
			return false;
		}

		mSecureBinary cardkey;
		if( !mSCFelicaMac::CalcDiversifiedKey( key , idrsp[ 0 ].Data , cardkey ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"個別化カード鍵を導出できません" );
			MyFelicaMac.ClearKey();
			return false;
		}
		return ExecAuthenticationInternal( cardkey , int_auth_only );
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"鍵のサイズが違います" );
		MyFelicaMac.ClearKey();
		return false;
	}
}

bool mSCFelica::ExecAuthenticationInternal( const mSecureBinary& key , bool int_auth_only )
{

	if( key.size() != 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"鍵のサイズが違います" );
		MyFelicaMac.ClearKey();
		return false;
	}

	//鍵の設定と、カードに送るチャレンジの取得
	mBinary challenge;
	MyFelicaMac.SetKey( key , challenge );

	//チャレンジの書き込み
	{
		DataBlock writedata;
		writedata.resize( 1 );
		writedata[ 0 ].BlockNumber = 0x80;	//RC(ランダムチャレンジ)
		writedata[ 0 ].Data = challenge;
		if( !RawWrite( writedata ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"チャレンジ値を書き込めません" );
			MyFelicaMac.ClearKey();
			return false;
		}
	}

	//検証データとMAC_Aを取得して内部認証を行う。ついでにWCNTを取得する。
	DWORD wcnt = 0;
	{
		ReadRequestBlock req;
		req.push_back( 0x82u );	//ID
		req.push_back( 0x86u );	//CKV
		if( !int_auth_only )
		{
			req.push_back( 0x90u );	//WCNT（外部認証の場合のみ必要）
		}
		DataBlock response;
		if( !MacReadInternal( req , response ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"内部認証が失敗しました" );
			MyFelicaMac.ClearKey();
			return false;
		}

		//内部認証のみ行う場合はここでおしまい
		if( int_auth_only )
		{
			//内部認証のみ成功
			return true;
		}

		//WCNTの値
		wcnt = ( response[ 2 ].Data[ 2 ] << 16 ) + ( response[ 2 ].Data[ 1 ] << 8 ) + ( response[ 2 ].Data[ 0 ] << 0 );
	}

	//STATEのEXT_AUTHに01Hを書込む。書き込み成功で外部認証成功（＝相互認証成立）
	{
		DataBlockEntry extdata;
		extdata.BlockNumber = 0x92u; //STATE
		extdata.Data.assign( 16 , 0 );
		extdata.Data[ 0 ] = 1;
		if( !MacWriteInternal( extdata , wcnt ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"外部認証が失敗しました" );
			MyFelicaMac.ClearKey();
			return false;
		}
	}

	//相互認証成立
	return true;
}

bool mSCFelica::CalcHash( const mBinary& in , mBinary& rethash )
{
	rethash.clear();

	//未認証状態？
	if( MyFelicaMac.IsKeyExist() )
	{
		RaiseError( g_ErrorLogger , 0 , L"認証済みです" );
		return false;
	}

	//チャレンジの書き込み
	DataBlock writedata;
	writedata.resize( 1 );
	writedata[ 0 ].BlockNumber = 0x80;	//RC(ランダムチャレンジ)
	writedata[ 0 ].Data = in;
	if( !RawWrite( writedata ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ハッシュデータを書き込めません" );
		MyFelicaMac.ClearKey();
		return false;
	}

	//結果の取得
	ReadRequestBlock req;
	req.push_back( 0x82u );	//ID
	req.push_back( 0x81u );	//MAC
	DataBlock response;
	if( !RawRead( req , response ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"ハッシュ値取得が失敗しました" );
		return false;
	}

	//結果の格納
	rethash = response[ 1 ].Data.subdata( 0 , 8 );
	return true;
}

bool mSCFelica::MacWriteInternal( const DataBlockEntry& data , DWORD wcnt )const
{
	DataBlockEntry mac;
	if( !MyFelicaMac.CreateMacA( data , mac , wcnt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_Aの生成が失敗しました" );
		return false;
	}

	DataBlock datablock;
	datablock.push_back( data );	//←STATE
	datablock.push_back( mac );		//←算出したMAC

	if( !RawWrite( datablock ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MACつき書き込みが失敗しました" );
		return false;
	}
	return true;
}

bool mSCFelica::GetChecksumResult( bool macauth )const
{
	//読み込み
	ReadRequestBlock req;
	DataBlock dt;
	req.push_back( 0xA0u );	//CRC_CHECK

	bool result;
	if( macauth )
	{
		result = MacReadInternal( req , dt );
	}
	else
	{
		result = RawRead( req , dt );
	}

	if( !result )
	{
		RaiseError( g_ErrorLogger , 0 , L"CRC検証結果を読み取れません" );
		return false;
	}

	return ( dt[ 0 ].Data[ 0 ] == 0x00u );
}

mSCFelica::AuthStatus mSCFelica::GetAuthStatus( void )const
{
	//読み込み
	ReadRequestBlock req;
	DataBlock dt;
	req.push_back( 0x92u );	//STATE

	if( !MacReadInternal( req , dt ) )
	{
		//Macつき読み込みが失敗する場合、認証されてない
		return mSCFelica::AuthStatus::Unauthorized;
	}
	if( dt[ 0 ].Data[ 0 ] == 0x01u )
	{
		//Macつき読み込み（＝内部認証済みの要件）が成功し、EXT_AUTHフラグが１なら外部認証済み
		return mSCFelica::AuthStatus::ExtAuthorized;
	}

	//それ以外は内部認証のみが済み
	return mSCFelica::AuthStatus::IntAuthorized;
}

//データの読み取り
bool mSCFelica::Read( const ReadRequestBlock& request , DataBlock& retResponse , bool macauth )const
{
	//前のデータを消す
	retResponse.clear();

	//同一のブロックを何度も読もうとしていないかチェックする
	//ユーザーブロック以外を読もうとしていないかもチェックする
	{
		using Blocks = std::unordered_set<DWORD>;
		Blocks blocks;
		for( ReadRequestBlock::const_iterator itr = request.begin() ; itr != request.end() ; itr++ )
		{
			if( blocks.count( *itr ) )
			{
				RaiseError( g_ErrorLogger , *itr , L"同一ブロックを複数回読み出そうとしました" );
				return false;
			}
			if( 0x0Eu < *itr )
			{
				RaiseError( g_ErrorLogger , *itr , L"ユーザーブロック以外を読み取ろうとしました" );
				return false;
			}
			blocks.insert( *itr );
		}
	}

	//順次読み込み
	{
		auto execread = [&retResponse,macauth,this]( const ReadRequestBlock& req )->bool
		{
			DataBlock rsp;
			bool result;

			if( macauth )
			{
				result = MacReadInternal( req , rsp );
			}
			else
			{
				result = RawRead( req , rsp );
			}
			if( !result )
			{
				RaiseError( g_ErrorLogger , 0 , L"データを読み取れません" );
				return false;
			}

			std::move( rsp.begin() , rsp.end() , std::back_inserter( retResponse ) );
			return true;
		};

		ReadRequestBlock req;
		DWORD max_blocks = ( macauth ) ? ( 3 ) : ( 4 );	//MAC付きの場合は一度に３ブロック、ついてない場合は４ブロック読める

		for( ReadRequestBlock::const_iterator itr = request.begin() ; itr != request.end() ; itr++ )
		{
			req.push_back( *itr );
			if( req.size() == max_blocks )
			{
				if( !execread( req ) )
				{
					return false;
				}
				req.clear();
			}
		}
		if( req.size() )
		{
			if( !execread( req ) )
			{
				return false;
			}
		}
	}
	return true;
}

DWORD mSCFelica::GetWCNT( void )const
{
	//読み込み
	ReadRequestBlock req;
	req.push_back( 0x90u );

	DataBlock dt;
	if( !RawRead( req , dt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"WCNTを読み取れません" );
		return 0xFFFF'FFFFu;
	}

	DWORD result = 0;
	result |= ( (DWORD)dt[ 0 ].Data[ 0 ] <<  0 );
	result |= ( (DWORD)dt[ 0 ].Data[ 1 ] <<  8 );
	result |= ( (DWORD)dt[ 0 ].Data[ 2 ] << 16 );
	return result;
}


//データの書き込み
bool mSCFelica::Write( const DataBlock& data , bool macauth )const
{
	//同一のブロックを何度も書こうとしていないかチェックする
	//ユーザーブロック以外を書こうとしていないかもチェックする
	{
		using Blocks = std::unordered_set<DWORD>;
		Blocks blocks;
		for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
		{
			if( blocks.count( itr->BlockNumber ) )
			{
				RaiseError( g_ErrorLogger , itr->BlockNumber , L"同一ブロックを複数回書込もうとしました" );
				return false;
			}
			if( 0x0Eu < itr->BlockNumber )
			{
				RaiseError( g_ErrorLogger , itr->BlockNumber , L"ユーザーブロック以外を書込もうとしました" );
				return false;
			}
			blocks.insert( itr->BlockNumber );
		}
	}

	//順次書き込み
	DWORD wcnt = ( macauth ) ? ( GetWCNT() ) : ( 0 );	//現在のWCNT値
	for( DataBlock::const_iterator itr = data.begin() ; itr != data.end() ; itr++ )
	{
		bool result;
		if( macauth )
		{
			result = MacWriteInternal( *itr , wcnt );
			wcnt++;
		}
		else
		{
			DataBlock dt;
			dt.push_back( *itr );
			result = RawWrite( dt );
		}
		if( !result )
		{
			RaiseError( g_ErrorLogger , itr->BlockNumber , L"書き込みが失敗しました" );
			return false;
		}
	}
	return true;
}

bool mSCFelica::UpdateKey( const mSecureBinary& key , uint16_t keyver )
{
	//鍵の導出
	mSecureBinary cardkey;
	if( key.size() == 16 )
	{
		//カード鍵なのでそのまま使う
		cardkey = key;
	}
	else if( key.size() == 24 )
	{
		//マスター鍵なのでカード鍵を導出
		ReadRequestBlock idreq;
		DataBlock idrsp;
		idreq.push_back( 0x82u );
		if( !RawRead( idreq , idrsp ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"IDを読み取れません" );
			MyFelicaMac.ClearKey();
			return false;
		}
		if( !mSCFelicaMac::CalcDiversifiedKey( key , idrsp[ 0 ].Data , cardkey ) )
		{
			RaiseError( g_ErrorLogger , 0 , L"個別化カード鍵を導出できません" );
			MyFelicaMac.ClearKey();
			return false;
		}
	}
	else
	{
		RaiseError( g_ErrorLogger , 0 , L"鍵のサイズが違います" );
		MyFelicaMac.ClearKey();
		return false;
	}

	//カード鍵の書き込み
	DWORD wcnt = GetWCNT();	//現在のWCNT値
	DataBlockEntry writedata;
	writedata.BlockNumber = 0x87;	//CK
	writedata.Data.assign( 16 , 0x00u );
	for( DWORD i = 0 ; i < 16 ; i++ )
	{
		writedata.Data[ i ] = cardkey[ i ];
	}
	if( !MacWriteInternal( writedata , wcnt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"キーの更新が失敗しました" );
		writedata.Data.secure_erase();
		return false;
	}
	writedata.Data.secure_erase();

	//新しいカード鍵で外部認証を通す
	if( !ExecAuthenticationInternal( cardkey , false ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"新しいキーでの認証が成功しませんでした" );
		return false;
	}

	//カード鍵バージョンの書き込み
	wcnt = GetWCNT();
	writedata.BlockNumber = 0x86;	//CKV
	writedata.Data.assign( 16 , 0x00u );
	writedata.Data[ 0 ] = (BYTE)( ( keyver >> 0 ) & 0xFFu );
	writedata.Data[ 1 ] = (BYTE)( ( keyver >> 8 ) & 0xFFu );

	if( !MacWriteInternal( writedata , wcnt ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"キーバージョンの更新が失敗しました" );
		return false;
	}

	return true;
}

//DataBlockEntryをRegDataに変換
mSCFelica::RegData mSCFelica::DataBlockEntryToRegData( const DataBlockEntry& src )
{
	RegData result;
	int max_index;

	//REGA
	max_index = ( src.Data.size() < 4 ) ? ( (int)src.Data.size() ) : ( 4 );
	for( int i = max_index - 1 ; 0 <= i ; i-- )
	{
		result.RegA <<= 8;
		result.RegA  += src.Data[ i ];
	}
	//REGB
	max_index = ( src.Data.size() < 8 ) ? ( (int)src.Data.size() ) : ( 8 );
	for( int i = max_index - 1 ; 4 <= i ; i-- )
	{
		result.RegB <<= 8;
		result.RegB  += src.Data[ i ];
	}
	//REGC
	result.RegC.assign( 8 , 0 );
	max_index = ( src.Data.size() < 16 ) ? ( (int)src.Data.size() ) : ( 16 );
	for( int i = 8 ; i < max_index ; i++ )
	{
		result.RegC[ i - 8 ] = src.Data[ i ];
	}
	return result;
}

//DataBlockEntryをRegDataに変換
void mSCFelica::DataBlockEntryToRegData( const DataBlockEntry& src , RegData& retRegData )
{
	retRegData = DataBlockEntryToRegData( src );
}

//RegDataをDataBlockEntryに変換
mSCFelica::DataBlockEntry mSCFelica::RegDataToDataBlockEntry( const RegData& src )
{
	DataBlockEntry result;
	result.BlockNumber = 0x0Eu;

	result.Data.assign( 16 , 0 );
	result.Data[ 0 ] = (BYTE)( ( src.RegA >>  0 ) & 0xFFu );
	result.Data[ 1 ] = (BYTE)( ( src.RegA >>  8 ) & 0xFFu );
	result.Data[ 2 ] = (BYTE)( ( src.RegA >> 16 ) & 0xFFu );
	result.Data[ 3 ] = (BYTE)( ( src.RegA >> 24 ) & 0xFFu );
	result.Data[ 4 ] = (BYTE)( ( src.RegB >>  0 ) & 0xFFu );
	result.Data[ 5 ] = (BYTE)( ( src.RegB >>  8 ) & 0xFFu );
	result.Data[ 6 ] = (BYTE)( ( src.RegB >> 16 ) & 0xFFu );
	result.Data[ 7 ] = (BYTE)( ( src.RegB >> 24 ) & 0xFFu );
	
	DWORD max_index = ( 8 < src.RegC.size() ) ? ( 8 ) : ( (DWORD)src.RegC.size() );
	for( DWORD i = 0 ; i < max_index ; i++ )
	{
		result.Data[ i + 8 ] = src.RegC[ i ];
	}

	return result;
}

//RegDataをDataBlockEntryに変換
void mSCFelica::RegDataToDataBlockEntry( const RegData& src , DataBlockEntry& retDataBlockEntry )
{
	retDataBlockEntry = RegDataToDataBlockEntry( src );
}


