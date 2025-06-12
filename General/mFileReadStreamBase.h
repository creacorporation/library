//----------------------------------------------------------------------------
// ストリーミングファイル読み込み操作
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2019-2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------


#ifndef MFILEREADSTREAMBASE_H_INCLUDED
#define MFILEREADSTREAMBASE_H_INCLUDED

#include "mStandard.h"
#include "mFile.h"
#include "mTCHAR.h"
#include <deque>
#include <memory>
#include <General/mBinary.h>

class mFileReadStreamBase
{
public:
	mFileReadStreamBase();
	virtual ~mFileReadStreamBase();

	//１文字（１バイト）読み込みます
	//ret : 読み取った文字
	//※EOFの場合、このリードストリームが同期アクセスのものかどうかで意味が違います
	// 同期アクセスの場合   : ファイルの終端（それ以上のデータはない）
	// 非同期アクセスの場合 : 現在読み取れるデータがない（時間が経てば再度読み取れるかもしれない）
	virtual INT Read( void ) = 0;

	//ANSIで１文字読み込みます
	//retChar : 読み取った文字
	//ret : 真の場合 : EOFに到達していない
	//      偽で同期アクセスの場合   : ファイルの終端（それ以上のデータはない）
	//      偽で非同期アクセスの場合 : 現在読み取れるデータがない（時間が経てば再度読み取れるかもしれない）
	bool ReadSingle( char& retChar );

	//UNICODE(LE)で１文字読み込みます
	//retWchar : 読み取った文字
	//ret : 真の場合 : EOFに到達していない
	//      偽で同期アクセスの場合   : ファイルの終端（それ以上のデータはない）
	//      偽で非同期アクセスの場合 : 現在読み取れるデータがない（時間が経てば再度読み取れるかもしれない）
	bool ReadSingle( wchar_t& retWchar );

	//読み取り時のエンコード指定
	enum Encode
	{
		ENCODE_ASIS,		//読み取るファイルのエンコードについて考慮しない（あるがままに読み込む）
		ENCODE_SHIFT_JIS,	//読み取るファイルのエンコードはSHIFT JISである
		ENCODE_UTF16,		//読み取るファイルのエンコードはUTF16(BOMなし)である
	};

	//1行読み取り・指定サイズ読み取り失敗時の動作指定
	enum OnLineReadError
	{
		//読めなかった部分は破棄する
		// 結果→
		// ・ReadLine:空が返る
		// ・ReadBinary:エラー発生までに読んでいたデータが返る
		// エラー発生までに読んでいたデータ→読み取り済みになる
		// 関数の戻り値→失敗
		LINEREADERR_DISCARD,
		
		//未読み取り状態に戻す
		// 結果→
		// ・ReadLine:空が返る
		// ・ReadBinary:中身は保証されない
		// エラー発生までに読んでいたデータ→未読状態に戻る
		// 関数の戻り値→失敗
		LINEREADERR_UNREAD,

		//先読みしたことにする(途中まで読んだデータを破棄しないこと以外はLINEREADERR_UNREADと同じ)
		// 結果→
		// ・ReadLine:エラー発生までに読んでいたデータが返る
		// ・ReadBinary:エラー発生までに読んでいたデータが返る
		// エラー発生までに読んでいたデータ→未読状態に戻る
		// 関数の戻り値→失敗
		LINEREADERR_PEEK,

		//読めた位置までを結果とする
		// 結果→
		// ・ReadLine:エラー発生までに読んでいたデータが返る
		// ・ReadBinary:エラー発生までに読んでいたデータが返る
		// エラー発生までに読んでいたデータ→読み取り済みになる
		// 関数の戻り値→成功
		LINEREADERR_TRUNCATE,
	};

	//読み取り時のファイルのエンコードが何であるかを指定します。
	//　例： SetEncode( ENCODE_SHIFT_JIS )	・・・ファイルのエンコードがSHIFT JISと指定
	//     　↓
	//       WString str
	//       ReadLine( str ) ・・・ファイルをSHIFT JISで読み取って、UTF16に変換してstrに格納
	// encode : ファイルのエンコード
	// ret : 成功時真
	// ※この関数を呼び出さない（エンコード無指定）場合は、ENCODE_ASISになります。
	bool SetEncode( Encode encode );

	//１行読み取ります。
	//ret : 読み取りを行ったときtrue。何も読み取らなかったときfalse。
	//・改行文字は、\rまたは\nまたは\r\nです。
	//・読み取った文字列に、改行コードは含まれません。
	//・文字エンコードはSHIFT_JISとして処理されます。
	bool ReadLine( AString& retResult , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//１行読み取ります。
	//ret : 読み取りを行ったときtrue。何も読み取らなかったときfalse。
	//・改行文字は、\rまたは\nまたは\r\nです。
	//・読み取った文字列に、改行コードは含まれません。
	//・文字エンコードはUNICODE(BOMなし16bitリトルエンディアン)として処理されます。
	bool ReadLine( WString& retResult , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//指定サイズを読み取ります
	//retReadSize : 正常終了するか、エラーが発生するまでに読み取ったバイト数。不要ならnullptrで可。
	//ret : 指定サイズ読み取り成功時true、失敗時false
	//・指定サイズを読み取る前にEOFになると失敗します
	bool ReadBinary( BYTE* retResult , size_t ReadSize , size_t* retReadSize = nullptr , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//指定サイズを読み取ります
	//retReadSize : 正常終了するか、エラーが発生するまでに読み取ったバイト数。不要ならnullptrで可。
	//ret : 指定サイズ読み取り成功時true、失敗時false
	//・指定サイズを読み取る前にEOFになると失敗します
	bool ReadBinary( mBinary& retResult , size_t ReadSize , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );


	//EOFに達しているかを調べます
	//※trueの場合、このリードストリームが同期アクセスのものかどうかで意味が違います
	// 同期アクセスの場合   : ファイルの終端（それ以上のデータはない）
	// 非同期アクセスの場合 : 現在読み取れるデータがない（時間が経てば再度読み取れるかもしれない）
	//・エンコードに"ENCODE_UTF16"を指定している場合、バイト単位の端数が存在すると、
	//  その最後のバイトについてもEOFと見なします。
	virtual bool IsEOF( void )const = 0;

	//ファイルが開いているかを判定します
	//開いている場合は真が返ります
	virtual bool IsOpen( void )const = 0;

	//1行読み取ってその文字列を特定の文字でパースする
	bool ParseLine( CHAR delimiter , AStringVector& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を特定の文字でパースする
	bool ParseLine( CHAR delimiter , AStringDeque& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を特定の文字でパースする
	bool ParseLine( WCHAR delimiter , WStringVector& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を特定の文字でパースする
	bool ParseLine( WCHAR delimiter , WStringDeque& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//1行読み取ってその文字列を空白文字でパースする
	bool ParseLineSpace( AStringVector& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を空白文字でパースする
	bool ParseLineSpace( AStringDeque& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を空白文字でパースする
	bool ParseLineSpace( WStringVector& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を空白文字でパースする
	bool ParseLineSpace( WStringDeque& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );

	//1行読み取ってその文字列を空白文字または特定の文字でパースする
	bool ParseLineSpace( CHAR delimiter , AStringVector& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を空白文字または特定の文字でパースする
	bool ParseLineSpace( CHAR delimiter , AStringDeque& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を空白文字または特定の文字でパースする
	bool ParseLineSpace( WCHAR delimiter , WStringVector& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );
	//1行読み取ってその文字列を空白文字または特定の文字でパースする
	bool ParseLineSpace( WCHAR delimiter , WStringDeque& retParsed , bool noempty = false , OnLineReadError onerr = OnLineReadError::LINEREADERR_TRUNCATE );


protected:

	mFileReadStreamBase( const mFileReadStreamBase& source ) = delete;
	void operator=( const mFileReadStreamBase& source ) = delete;

protected:

	Encode MyEncode;		//読み取り時のエンコード

	//読み込んだのをやっぱ止めたとバッファに押し戻した場合に、
	//次に読むときまで取っておくためのバッファ
	class UnReadBuffer : public std::deque<BYTE>
	{
	public:
		//型の分だけキャッシュに押し返します
		//val : 押し返すデータ
		//・リトルエンディアンと見なします
		template<class T> void Unread( T val )
		{
			for( size_t i = 0 ; i < sizeof( T ) ; i++ )
			{
				size_t offset = ( sizeof( T ) - 1 - i ) * 8;
				push_front( ( val >> offset ) & 0xFF );
			}
		}

		//バッファから1文字取りだし
		//バッファが空なのに取り出そうとするとEOFになる(注意！)
		//ret : バッファから取り出したデータ。バッファが空の場合EOF。
		INT Read( void );

		//バッファは空か？
		//ret : 空であればtrue
		bool IsEmpty( void )const;

		//バッファをクリア
		void Clear( void );
	};

	UnReadBuffer MyUnReadBuffer;

	std::unique_ptr<BYTE[]> MyReadCacheHead;	//キャッシュしているデータの先頭バイト
	DWORD MyReadCacheCurrent;					//次のReadで返す位置
	DWORD MyReadCacheRemain;					//未読のキャッシュのサイズ

	//EOFとなったか？
	bool MyIsEOF;

	//CRLF無視フラグを参照して、このバイトを無視するべきか判断する
	// true : このバイトは無視して次のバイトを読み込む必要がある
	bool ProcLFIgnore( INT c );

	//現在受信済みのデータを破棄します
	void ClearBuffer( void );

private:

	//CRLF無視フラグ
	// 非同期ストリームでReadLineでCR→EOF→LFの順に読み込んだ場合、ReadLineはEOFの時点で行区切りにするが、
	// その後にLFを受信した場合に無視するためのフラグ
	enum class LFIgnoreState
	{
		None,
		AsciiLF,		//ASCIIのLF
		UnicodeLF1,		//UnicodeのLF(1バイト目)
		UnicodeLF2,		//UnicodeのLF(2バイト目)
	};
	LFIgnoreState MyLFIgnoreState;

};

#endif //MFILEREADSTREAMBASE_H_INCLUDED

