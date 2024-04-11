//----------------------------------------------------------------------------
// Felica認証コード計算機
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mSCFelicaMac.h"
#include <General/mErrorLogger.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#pragma comment(lib,"bcrypt.lib")

mSCFelicaMac::mSCFelicaMac()
{
	MyAlgHandle = 0;
	MyRKeyHandle = 0;
	MyRKeyObject.reset();
	MyWKeyHandle = 0;
	MyWKeyObject.reset();

	Initialize();
}

mSCFelicaMac::~mSCFelicaMac()
{
	//キーのハンドル
	ClearKey();
	//アルゴリズムのハンドル
	if( MyAlgHandle )
	{
		BCryptCloseAlgorithmProvider( MyAlgHandle , 0 );
		MyAlgHandle = 0;
	}
}

bool mSCFelicaMac::Initialize( void )
{
	NTSTATUS stat;
	if( MyAlgHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"二重にハンドルを開こうとしています" );
		return false;
	}

	//2key triple desのハンドルを要求
	stat = BCryptOpenAlgorithmProvider( &MyAlgHandle , BCRYPT_3DES_112_ALGORITHM , nullptr , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"暗号プロバイダのハンドルを取得できません" );
		return false;
	}

	//暗号化モードを指定
	stat = BCryptSetProperty( MyAlgHandle , BCRYPT_CHAINING_MODE , (PUCHAR)BCRYPT_CHAIN_MODE_CBC , sizeof( BCRYPT_CHAIN_MODE_CBC ) , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"暗号化モードを設定できません" );
		return false;
	}
	return true;
}

bool mSCFelicaMac::CheckIsWeakKey( const mSecureBinary& key )
{
	auto Check = []( const mSecureBinary& key , DWORD offset ) -> bool
	{
		unsigned long long keyval = 0;
		static const unsigned long long weak_key_table[] =
		{
			0x0101010101010101u,0xFEFEFEFEFEFEFEFEu,0xE0E0E0E0F1F1F1F1u,0x1F1F1F1F0E0E0E0Eu,	//弱鍵
			0x0000000000000000u,0xFFFFFFFFFFFFFFFFu,0xE1E1E1E1F0F0F0F0u,0x1E1E1E1E0F0F0F0Fu,
			0x011F011F010E010Eu,0x1F011F010E010E01u,0x01E001E001F101F1u,0xE001E001F101F101u,	//順弱鍵
			0x01FE01FE01FE01FEu,0xFE01FE01FE01FE01u,0x1FE01FE00EF10EF1u,0xE01FE01FF10EF10Eu,
			0x1FFE1FFE0EFE0EFEu,0xFE1FFE1FFE0EFE0Eu,0xE0FEE0FEF1FEF1FEu,0xFEE0FEE0FEF1FEF1u,
			0x01011F1F01010E0Eu,0x1F1F01010E0E0101u,0xE0E01F1FF1F10E0Eu,0x0101E0E00101F1F1u,	//弱鍵候補
			0x1F1FE0E00E0EF1F1u,0xE0E0FEFEF1F1FEFEu,0x0101FEFE0101FEFEu,0x1F1FFEFE0E0EFEFEu,
			0xE0FE011FF1FE010Eu,0x011F1F01010E0E01u,0x1FE001FE0EF101FEu,0xE0FE1F01F1FE0E01u,
			0x011FE0FE010EF1FEu,0x1FE0E01F0EF1F10Eu,0xE0FEFEE0F1FEFEF1u,0x011FFEE0010EFEF1u,
			0x1FE0FE010EF1FE01u,0xFE0101FEFE0101FEu,0x01E01FFE01F10EFEu,0x1FFE01E00EFE01F1u,
			0xFE011FE0FE010EF1u,0xFE01E01FFE01F10Eu,0x1FFEE0010EFEF101u,0xFE1F01E0FE0E01F1u,
			0x01E0E00101F1F101u,0x1FFEFE1F0EFEFE0Eu,0xFE1FE001FE0EF101u,0x01E0FE1F01F1FE0Eu,
			0xE00101E0F10101F1u,0xFE1F1FFEFE0E0EFEu,0x01FE1FE001FE0EF1u,0xE0011FFEF1010EFEu,
			0xFEE0011FFEF1010Eu,0x01FEE01F01FEF10Eu,0xE001FE1FF101FE0Eu,0xFEE01F01FEF10E01u,
			0x01FEFE0101FEFE01u,0xE01F01FEF10E01FEu,0xFEE0E0FEFEF1F1FEu,0x1F01011F0E01010Eu,
			0xE01F1FE0F10E0EF1u,0xFEFE0101FEFE0101u,0x1F01E0FE0E01F1FEu,0xE01FFE01F10EFE01u,
			0xFEFE1F1FFEFE0E0Eu,0x1F01FEE00E01FEF1u,0xE0E00101F1F10101u,0xFEFEE0E0FEFEF1F1u,
		};

		for( DWORD i = 0 ; i < 8 ; i++ )
		{
			keyval <<= 8;
			keyval |= key[ ( 7 - i ) + offset ];
		}
		for( DWORD i = 0 ; i < array_count_of( weak_key_table ) ; i++ )
		{
			if( keyval == weak_key_table[ i ] )
			{
				return false;
			}
		}
		SecureZeroMemory( &keyval , sizeof( keyval ) );
		return true;
	};

	//CK1
	if( !Check( key , 0 ) )
	{
		//弱鍵なのでダメ
		return false;
	}
	//CK2
	if( !Check( key , 8 ) )
	{
		//弱鍵なのでダメ
		return false;
	}
	//弱鍵ではない
	return true;
}

void mSCFelicaMac::ClearKey( void )
{
	//キーのハンドル
	if( MyRKeyHandle )
	{
		BCryptDestroyKey( MyRKeyHandle );
		MyRKeyHandle = 0;
	}
	MyRKeyObject.reset();

	if( MyWKeyHandle )
	{
		BCryptDestroyKey( MyWKeyHandle );
		MyWKeyHandle = 0;
	}
	MyWKeyObject.reset();

	//チャレンジ
	MyChallenge.clear();
}


bool mSCFelicaMac::SetKey( const mSecureBinary& key , mBinary& retChallenge )
{
	retChallenge.clear();

	NTSTATUS stat;
	if( !MyAlgHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"初期化されていません" );
		goto badend;
	}
	ClearKey();

	//鍵のサイズを確認
	if( key.size() != 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"鍵のサイズが違います" );
		goto badend;
	}

	//チャレンジを生成
	MyChallenge.resize( 16 );
	stat = BCryptGenRandom( nullptr , (PUCHAR)MyChallenge.data() , (ULONG)MyChallenge.size() , BCRYPT_USE_SYSTEM_PREFERRED_RNG );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"乱数生成エラー" );
		goto badend;
	}
	retChallenge.resize( 16 );
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		retChallenge[ i + 0 ] = MyChallenge[  7 - i ];
		retChallenge[ i + 8 ] = MyChallenge[ 15 - i ];
	}

	//セッション鍵のハンドルを作成する
	if( !CreateSessionKey( key ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"セッション鍵の生成が失敗しました" );
		goto badend;
	}

	//チャレンジの後半は要らんから消す
	SecureZeroMemory( &MyChallenge.data()[ 8 ] , 8 );
	MyChallenge.resize( 8 );
	return true;

badend:
	ClearKey();
	return false;
}

bool mSCFelicaMac::IsKeyExist( void )const
{
	return MyChallenge.size() == 8;
}


bool mSCFelicaMac::CreateSessionKey( const mSecureBinary& key )
{
	NTSTATUS stat;

	//キーオブジェクトのサイズを算定
	DWORD key_obj_size = 0;
	DWORD result_size = 0;
	stat = BCryptGetProperty( MyAlgHandle , BCRYPT_OBJECT_LENGTH , (PBYTE)&key_obj_size , sizeof( key_obj_size ) , &result_size , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"キーオブジェクトのサイズを取得できません" );
		return false;
	}
	MyRKeyObject.reset( mNew BYTE[ key_obj_size ] );
	MyWKeyObject.reset( mNew BYTE[ key_obj_size ] );

	//カード鍵のハンドル取得
	BYTE byte_swapped_data[ 16 ];
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		byte_swapped_data[ i + 0 ] = key[  7 - i ];
		byte_swapped_data[ i + 8 ] = key[ 15 - i ];
	}
	stat = BCryptGenerateSymmetricKey( MyAlgHandle , &MyRKeyHandle , MyRKeyObject.get() , key_obj_size , byte_swapped_data , sizeof( byte_swapped_data ) , 0 );
	SecureZeroMemory( byte_swapped_data , sizeof( byte_swapped_data ) );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"カード鍵のハンドルを取得できません" );
		return false;
	}

	//セッション鍵の値を算出
	BYTE session_key[ 16 ];
	ULONG resultsize = 0;
	BYTE init_vector[ 8 ] = { 0 };
	stat = BCryptEncrypt( MyRKeyHandle , &MyChallenge.data()[ 0 ] , 8 , nullptr , init_vector , sizeof( init_vector ) , &session_key[ 0 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"セッション鍵導出エラー１" );
		return false;
	}
	stat = BCryptEncrypt( MyRKeyHandle , &MyChallenge.data()[ 8 ] , 8 , nullptr , init_vector , sizeof( init_vector ) , &session_key[ 8 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"セッション鍵導出エラー２" );
		return false;
	}

	//カード鍵のハンドル削除
	BCryptDestroyKey( MyRKeyHandle );
	MyRKeyHandle = 0;

	//セッション鍵のハンドルを生成(リード用)
	stat = BCryptGenerateSymmetricKey( MyAlgHandle , &MyRKeyHandle , MyRKeyObject.get() , key_obj_size , session_key , sizeof( session_key ) , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"リード用セッション鍵のハンドルを取得できません" );
		return false;
	}

	//ライト用セッション鍵はSK1とSK2を入れ替える
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		BYTE tmp = session_key[ i ];
		session_key[ i ] = session_key[ i + 8 ];
		session_key[ i + 8 ] = tmp;
	}

	//セッション鍵のハンドルを生成(ライト用)
	stat = BCryptGenerateSymmetricKey( MyAlgHandle , &MyWKeyHandle , MyWKeyObject.get() , key_obj_size , session_key , sizeof( session_key ) , 0 );
	SecureZeroMemory( session_key , sizeof( session_key ) );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"ライト用セッション鍵のハンドルを取得できません" );
		return false;
	}

	SecureZeroMemory( session_key , sizeof( session_key ) );
	return true;
}


//読み込み時におけるMAC_Aの体裁をチェックする。以下を満たせば合格。
//・MAC_A(ブロック91h)が末尾についていること
//・MAC_Aが２つ以上あってはいけない
//・MAC_Aを含め、ブロック数は２～４個
//・MAC_Aにデータが８バイト以上格納されている
//・MAC_A以外にデータが１６バイト以上格納されている
static bool CheckDataBlockMacStructure( const mSCFelicaMac::DataBlock& data )
{
	//サイズチェック
	if( data.size() < 2 || 4 < data.size() )
	{
		return false;
	}
	//末尾のブロック番号はMAC_Aか
	if( data.back().BlockNumber != 0x91u )
	{
		return false;
	}
	//MAC_Aが２つ以上存在しないか、MAC_A以外には１６バイトのデータがあるか
	for( DWORD i = 0 ; i < data.size() - 1 ; i++ )
	{
		if( data[ i ].BlockNumber == 0x91u )
		{
			return false;
		}
		if( data[ i ].Data.size() < 16 )
		{
			return false;
		}
	}
	//MAC_Aにデータが入っているか
	if( data.back().Data.size() < 8 )
	{
		return false;
	}
	//合格
	return true;
}

bool mSCFelicaMac::ValidateMacA( const DataBlock& data )
{
	NTSTATUS stat;

	if( !MyRKeyHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"セッション鍵が未生成です" );
		return false;
	}

	if( !CheckDataBlockMacStructure( data ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_Aが正しく格納されていません" );
		return false;
	}

	//平文データの作成
	BYTE plain[ 8 ];
	plain[ 0 ] = ( data.size() < 4 ) ? ( 0xFFu ) : ( 0x00u );
	plain[ 1 ] = ( data.size() < 4 ) ? ( 0xFFu ) : ( (BYTE)data[ 3 ].BlockNumber );
	plain[ 2 ] = ( data.size() < 3 ) ? ( 0xFFu ) : ( 0x00u );
	plain[ 3 ] = ( data.size() < 3 ) ? ( 0xFFu ) : ( (BYTE)data[ 2 ].BlockNumber );
	plain[ 4 ] = 0x00u;
	plain[ 5 ] = (BYTE)data[ 1 ].BlockNumber;
	plain[ 6 ] = 0x00u;
	plain[ 7 ] = (BYTE)data[ 0 ].BlockNumber;

	//IVの作成
	BYTE init_vector[ 8 ];
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		init_vector[ i ] = MyChallenge[ i ];
	}

	//暗号化(1) ブロック番号
	ULONG resultsize;
	BYTE cryptdata[ 8 ];
	stat = BCryptEncrypt( MyRKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A導出エラー1" );
		return false;
	}

	//暗号化(2) ブロックデータ
	for( DWORD i = 0 ; i < data.size() - 1 ; i++ )
	{
		for( DWORD j = 0 ; j < 8 ; j++ )
		{
			plain[ j ] = data[ i ].Data[ 7 - j ];
		}
		stat = BCryptEncrypt( MyRKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
		if( !NT_SUCCESS( stat ) )
		{
			RaiseError( g_ErrorLogger , stat , L"MAC_A導出エラー2" );
			return false;
		}
		for( DWORD j = 0 ; j < 8 ; j++ )
		{
			plain[ j ] = data[ i ].Data[ 15 - j ];
		}
		stat = BCryptEncrypt( MyRKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
		if( !NT_SUCCESS( stat ) )
		{
			RaiseError( g_ErrorLogger , stat , L"MAC_A導出エラー3" );
			return false;
		}
	}

	//得られた暗号化結果がMAC_Aの値と合致するか確認
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		if( data.back().Data[ i ] != cryptdata[ 7 - i ] )
		{
			//合致してないから認証失敗
			RaiseError( g_ErrorLogger , 0 , L"MAC_Aの算出結果が合致しません" );
			return false;
		}
	}

	//認証成功
	return true;
}


bool mSCFelicaMac::CreateMacA( const DataBlockEntry& data , DataBlockEntry& retMacA , DWORD wcnt )
{
	retMacA.BlockNumber = 0x91u;	//MAC_A
	retMacA.Data.assign( 16 , 0 );	//0x00×16個に初期化

	NTSTATUS stat;
	if( !MyWKeyHandle )
	{
		RaiseError( g_ErrorLogger , 0 , L"セッション鍵が未生成です" );
		return false;
	}
	if( data.Data.size() < 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"MAC_A生成対象のデータがありません" );
		return false;
	}
	if( 0x00FF'FFFFu <= wcnt )
	{
		RaiseError( g_ErrorLogger , 0 , L"WCNTが最大値に到達しています" );
		return false;
	}

	//平文データの作成
	BYTE plain[ 8 ];
	plain[ 0 ] = 0x00u;
	plain[ 1 ] = 0x91u;
	plain[ 2 ] = 0x00u;
	plain[ 3 ] = (BYTE)data.BlockNumber;
	plain[ 4 ] = 0x00u;
	plain[ 5 ] = (BYTE)( ( wcnt >> 16 ) & 0xFFu );
	plain[ 6 ] = (BYTE)( ( wcnt >>  8 ) & 0xFFu );
	plain[ 7 ] = (BYTE)( ( wcnt >>  0 ) & 0xFFu );

	//IVの作成
	BYTE init_vector[ 8 ];
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		init_vector[ i ] = MyChallenge[ i ];
	}	

	//暗号化(1) ブロック番号
	ULONG resultsize;
	BYTE cryptdata[ 8 ];
	stat = BCryptEncrypt( MyWKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A導出エラー1" );
		return false;
	}

	//暗号化(2) ブロックデータ
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		plain[ i ] = data.Data[ 7 - i ];
	}
	stat = BCryptEncrypt( MyWKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A導出エラー2" );
		return false;
	}
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		plain[ i ] = data.Data[ 15 - i ];
	}
	stat = BCryptEncrypt( MyWKeyHandle , plain , 8 , nullptr , init_vector , sizeof( init_vector ) , cryptdata , sizeof( cryptdata ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"MAC_A導出エラー3" );
		return false;
	}

	//結果セット
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		retMacA.Data[ i ] = cryptdata[ 7 - i ];
	}
	retMacA.Data[  8 ] = (BYTE)( ( wcnt >>  0 ) & 0xFFu );
	retMacA.Data[  9 ] = (BYTE)( ( wcnt >>  8 ) & 0xFFu );
	retMacA.Data[ 10 ] = (BYTE)( ( wcnt >> 16 ) & 0xFFu );
	return true;
}


static uint64_t ByteSwap( uint64_t in )
{
	uint64_t result = 0;
	for( DWORD i = 0 ; i < 8 ; i++ )
	{
		result <<= 8;
		result |= ( in & 0xFFu );
		in >>= 8;
	}
	return result;
}

bool mSCFelicaMac::CalcDiversifiedKey( const mSecureBinary& master , const mBinary& id , mSecureBinary& retcardkey )
{
	//ソニーの仕様書にはエンディアンがビッグともリトルとも書いてないが、
	//参考にしたソースはビッグエンディアンのようなので、「最上位ビット」とか「１ビットシフト」とかはビッグエンディアンに合わせて処理する。
	// ttps://github.com/ode1022/esp32_felica_lite

	NTSTATUS stat;
	BCRYPT_ALG_HANDLE alghandle = 0;
	BYTE* keyobject = nullptr;
	BCRYPT_KEY_HANDLE keyhandle = 0;

	retcardkey.clear();
	retcardkey.assign( 16 , 0 );

	//サイズチェック
	if( master.size() != 24 || id.size() != 16 )
	{
		RaiseError( g_ErrorLogger , 0 , L"鍵またはIDブロックのサイズが違います" );
		return false;
	}

	//3DESのハンドルを要求
	stat = BCryptOpenAlgorithmProvider( &alghandle , BCRYPT_3DES_ALGORITHM , nullptr , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"暗号プロバイダのハンドルを取得できません" );
		goto ending;
	}

	//暗号化モードを指定
	stat = BCryptSetProperty( alghandle , BCRYPT_CHAINING_MODE , (PUCHAR)BCRYPT_CHAIN_MODE_CBC , sizeof( BCRYPT_CHAIN_MODE_CBC ) , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"暗号化モードを設定できません" );
		goto ending;
	}

	//キーオブジェクトのサイズを算定
	DWORD key_obj_size;
	DWORD result_size;
	stat = BCryptGetProperty( alghandle , BCRYPT_OBJECT_LENGTH , (PBYTE)&key_obj_size , sizeof( key_obj_size ) , &result_size , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"キーオブジェクトのサイズを取得できません" );
		goto ending;
	}
	keyobject = mNew BYTE[ key_obj_size ];

	//カード鍵のハンドル取得
	stat = BCryptGenerateSymmetricKey( alghandle , &keyhandle , keyobject , key_obj_size , (PUCHAR)master.data() , (ULONG)master.size() , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"カード鍵のハンドルを取得できません" );
		goto ending;
	}

	//(2)Lの導出
	uint64_t ivector = 0;
	uint64_t plain = 0;
	uint64_t value_L;
	ULONG resultsize;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&value_L , sizeof( value_L ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"L値導出エラー" );
		goto ending;
	}

	//(3)ビットシフト
	uint64_t value_K1 = ByteSwap( value_L );	//ここからK1＝リトルエンディアンの並び
	value_K1 <<= 1;
	if( value_L & 0x0000'0000'0000'0080u )		//←ビッグエンディアンの最上位ビットの位置
	{
		value_K1 ^= 0x0000'0000'0000'001Bu;
	}
	value_K1 = ByteSwap( value_K1 );			//ここまでK1＝リトルエンディアンの並び

	//(4)Mの分割、(5)M2の導出
	uint64_t value_M1 = ( *(const uint64_t*)( &id[ 0 ] ) ) ^ ( 0        );
	uint64_t value_M2 = ( *(const uint64_t*)( &id[ 8 ] ) ) ^ ( value_K1 );

	//(6)C1の導出
	ivector = 0;
	plain = value_M1;
	uint64_t value_C1a;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&value_C1a , sizeof( value_C1a ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"C1値導出エラー" );
		goto ending;
	}

	//(7)Tの導出
	ivector = 0;
	plain = value_C1a ^ value_M2;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&retcardkey[ 0 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"T値導出エラー" );
		goto ending;
	}

	//(8)M1'、(9)C1'の導出
	ivector = 0;
	plain = value_M1 ^ 0x0000'0000'0000'0080u;	//←ビッグエンディアンの最上位ビットの位置
	uint64_t value_C1b;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&value_C1b , sizeof( value_C1b ) , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"C1'値導出エラー" );
		goto ending;
	}

	//(10)T'の導出
	ivector = 0;
	plain = value_C1b ^ value_M2;
	stat = BCryptEncrypt( keyhandle , (PUCHAR)&plain , sizeof( plain ) , nullptr , (PUCHAR)&ivector , sizeof( ivector ) , (PUCHAR)&retcardkey[ 8 ] , 8 , &resultsize , 0 );
	if( !NT_SUCCESS( stat ) )
	{
		RaiseError( g_ErrorLogger , stat , L"T'値導出エラー" );
		goto ending;
	}

ending:
	bool result = NT_SUCCESS( stat );
	//アルゴリズムのハンドル
	if( keyhandle )
	{
		BCryptDestroyKey( keyhandle );
	}
	if( keyobject )
	{
		mDelete[] keyobject;
	}
	if( alghandle )
	{
		BCryptCloseAlgorithmProvider( alghandle , 0 );
	}
	return result;
}

