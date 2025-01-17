//----------------------------------------------------------------------------
// ウインドウ管理（GDIフォント）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
GDIのフォントです。
*/

#ifndef MGDIFONT_H_INCLUDED
#define MGDIFONT_H_INCLUDED

#include "mStandard.h"
#include "GDI/mGdiHandle.h"
#include "General/mException.h"
#include "General/mInitFileSection.h"

/*
https://msdn.microsoft.com/ja-jp/library/cc428368.aspx
http://chokuto.ifdef.jp/urawaza/api/CreateFont.html

int nHeight,               // フォントの高さ
int nWidth,                // 平均文字幅
int nEscapement,           // 文字送り方向の角度
int nOrientation,          // ベースラインの角度
int fnWeight,              // フォントの太さ
DWORD fdwItalic,           // 斜体にするかどうか
DWORD fdwUnderline,        // 下線を付けるかどうか
DWORD fdwStrikeOut,        // 取り消し線を付けるかどうか
DWORD fdwCharSet,          // キャラクタセット
DWORD fdwOutputPrecision,  // 出力精度
DWORD fdwClipPrecision,    // クリッピング精度
DWORD fdwQuality,          // 出力品質
DWORD fdwPitchAndFamily,   // ピッチとファミリ
LPCTSTR lpszFace           // フォント名
*/

namespace mGdiFont_Definitions
{
	//フォントを作成するときに使用するキャラクタセット
	typedef enum FontCharSet
	{
		ANSI,			//英語
		BALTIC,			//バルト三国の文字(リトアニア語、ラトビア語等)
		GB2312,			//簡体字中国語(中国、シンガポール)
		CHINESEBIG5,	//繁体字中国語(中華民国、香港、マカオ)
		EASTEUROPE,		//東欧系言語の文字
		GREEK,			//ギリシャ語
		HANGUL,			//韓国語
		SHIFTJIS,		//日本語
		RUSSIAN,		//ロシア語
		TURKISH,		//トルコ語
		HEBREW,			//ヘブライ語
		ARABIC,			//アラビア語
		THAI,			//タイ語
		LOCALE,			//現在のロケール(言語設定)に基づいて決める
		OEM,			//何語版ウインドウズかで決める
		eMAX_FontCharSet,
	}FontCharSet;

	//フォントを作成するときの綺麗さ
	enum PrintQuality
	{
		NORMAL ,		//デフォルト。コントロールパネルの設定に従う。
		ANTIALIAESED ,	//標準のアンチエイリアスをする。細い部分の表現がイマイチ。
		CLEARTYPE ,		//ClearTypeを使う。XP以降のみ。綺麗。
	};

	//フォントを印字するときのピッチ
	enum PrintPitch
	{
		DEFAULT ,		//フォント任せにする
		FIXED ,			//固定幅にする
		VARIABLE ,		//可変幅にする
	};
}

class mGdiFont : public mGdiHandle
{
public:


	//オプション構造体
	//実際にフォントを作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//フォント生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};

		//インテリセンスに値がわらわら出てきて邪魔なので外に追い出してあります
		using FontCharSet = mGdiFont_Definitions::FontCharSet;
		using PrintQuality = mGdiFont_Definitions::PrintQuality;
		using PrintPitch = mGdiFont_Definitions::PrintPitch;

		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//フォント生成のオプション
	//どんなフォントを生成するかをココで指定します。
	struct Option_UseOption : public Option
	{
		WString name;			//使用したいフォントの名前
		LONG height;			//フォントの高さ
		LONG width;				//フォントの幅(0で高さから自動設定)
		bool isbold;			//太字にしたい場合はtrue
		bool isitalic;			//イタリックにしたい場合はtrue
		bool isunderline;		//アンダーラインが欲しい場合はtrue
		bool isstrikeout;		//打ち消し線を付けたい場合はtrue
		FontCharSet charset;	//使用するキャラクタセット
		PrintQuality quality;	//印字品質
		PrintPitch pitch;		//文字幅
		LONG orientation;		//文字の回転角度(反時計回り0.1度単位) 横書きの場合0、縦書きの場合-900が推奨値です。

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			name = L"";	//フォント名が空文字列の場合、Windowsが適当に決めてくれる
			height = 9;
			width = 0;							//高さから自動設定
			isbold = false;
			isitalic = false;
			isunderline = false;
			isstrikeout = false;
			charset = FontCharSet::LOCALE;		//現在のロケールで決める
			quality = PrintQuality::NORMAL;		//デフォルト。アンチエイリアスはしない
			pitch = PrintPitch::DEFAULT;		//幅はフォント任せで決める
			orientation = 0;					//回転角度
		}
	};

	//ファクトリメソッド
	static mGdiHandle* Factory( const void* opt )throw( )
	{
		mGdiHandle* result;
		try
		{
			//新しいフォントのインスタンスを作成する
			result = mNew mGdiFont( (const Option*)opt );
		}
		catch( mException )
		{
			//フォントの生成に失敗した場合はヌルポインタを返す
			result = nullptr;
		}
		return result;
	}

	//コンストラクタ
	//このコンストラクタは、MyHandleに格納するブラシの生成失敗時に例外を投げます。
	mGdiFont( const Option* opt )noexcept( false );

	//デストラクタ
	virtual ~mGdiFont();
	
	//ハンドルの値を取得する(キャスト演算子バージョン)
	operator HFONT()const;

	//ハンドルの値を取得する(普通の関数バージョン)
	virtual HGDIOBJ GetHandle( void )const override;

private:

	//以下、デフォルト系のはコピーされると面倒くさいので使用禁止

	mGdiFont() = delete;
	mGdiFont( const mGdiFont& src ) = delete;
	mGdiFont& operator=( const mGdiFont& src ) = delete;

private:

	//Optionに指定した内容に見合うフォントを生成し、MyHandleに登録する
	//コンストラクタから呼び出される想定
	bool MakeFont( const Option_UseOption* opt );


protected:
	//ハンドルの実体
	HFONT MyHandle;
};

#endif	//MGDIFONT_H_INCLUDED

