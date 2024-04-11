//----------------------------------------------------------------------------
// ウインドウ管理（ステータスバー）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
ステータスバーの実装です
・ボタンやチェックボックスと同じようなカンジで貼り付けできます。
・ツールバーをいくつかのパーツに区切って、それぞれに文字列とか設定できます。
・左側いくつかをツールバーの左側に配置して、残りを右側に配置できます。
	⇒mStatusBar::Option::RightJustifyIndex
・↑で左側に配置したものと右側に配置したものが被った場合、どちらを上にするか指定できます。
	⇒mStatusBar::Option::RightUpper
*/

//●サンプルコード
#ifdef SAMPLE_CODE
//●使用サンプル（別ファイルにコピペして下さい）
//動作：ステータスバーが付いているウインドウを生成します。
#include "GDI/mStatusBar.h"
class TestWindow : public mWindow
{
public:
	static mWindow* Factory( const void * )throw( )
	{
		return mNew TestWindow;
	}
protected:
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
	{
		switch( msg )
		{
		case WM_DESTROY:
			//ウインドウが閉じたら終了
			PostQuitMessage( 0 );
			return 0;
		case WM_SIZE:
			{
				MyChild->AdjustSize( GetMyHwnd() );

				RECT rect;
				GetClientRect( GetMyHwnd() , &rect );
				WString str;
				sprintf( str , L"%d" , rect.right - rect.left );

				mStatusBar* bar = (mStatusBar*)MyChild->Query( L"STATUS" );
				bar->SetText( L"6" , str );
			}
			break;
		}
		return mWindow::WindowProcedure( msg , wparam , lparam );
	}
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , void* opt )
	{
		retSetting.ClassName = L"TESTCLASS";
		return true;
	}
	virtual void CreateWindowCallback( CreateWindowSetting& retSetting , void* opt )
	{
		retSetting.WindowName = L"TEST WINDOW";
		retSetting.Width = CW_USEDEFAULT;
		retSetting.Style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	}
	virtual void OnCreate( void* option )
	{
		MyChild = mNew mWindowCollection( this );

		mStatusBar::Option opt;
		mStatusBar::PartsOptionEntry entry;
		entry.Width = 100;			//幅100ピクセルのパーツをいくつも作る
		entry.Id = L"1";
		opt.Parts.push_back( entry );
		entry.Id = L"2";
		opt.Parts.push_back( entry );
		entry.Id = L"3";
		opt.Parts.push_back( entry );
		entry.Id = L"4";
		opt.Parts.push_back( entry );
		entry.Id = L"5";
		opt.Parts.push_back( entry );
		entry.Id = L"6";
		opt.Parts.push_back( entry );
		entry.Id = L"7";
		opt.Parts.push_back( entry );

		opt.RightJustifyIndex = 2;	//2個は左寄せ、残りは右寄せ
		opt.RightUpper = true;	//右寄せのパーツを上にする
		MyChild->AddControl<mStatusBar>( L"STATUS" , &opt );	//ステータスバー生成
	}
};

int main( int argc , char** argv )
{
	InitializeLibrary();

	mWindowCollection root_collection( nullptr );
	root_collection.AddControl<TestWindow>( L"TEST" );

	MSG msg;
	while( GetMessageW( &msg , 0 , 0 , 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}
	return 0;
}
#endif

#ifndef MSTATUSBAR_H_INCLUDED
#define MSTATUSBAR_H_INCLUDED

#include "mStandard.h"
#include "mWindow.h"
#include <vector>
#include <unordered_map>
#include <functional>

class mStatusBar : public mWindow
{
public:

	//ボーダーラインの種類
	enum BorderType
	{
		Normal,		//普通の境界線
		NoBorder,	//境界線なし
		Popout,		//盛り上がる境界線
	};

	//パーツの設定
	struct PartsOptionEntry
	{

		WString Id;			//このパーツにつけるID
		WString Str;		//設定する文字列
		UINT Width;			//パーツのサイズ
		bool Notab;			//タブを無視する場合true
		BorderType Border;	//ボーダーラインの種類
		PartsOptionEntry()
		{
			Width = 100;
			Notab = true;
			Border = BorderType::Normal;
		}
	};

	//各パーツの情報を格納する型
	typedef std::vector<PartsOptionEntry> PartsOption;

	//コントロール生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option
	{
		//生成の方法
		enum CreateMethod
		{
			USEOPTION,		//通常の方法
		};
		const CreateMethod method;	//RTTIの代用です。変更の必要はありません。

	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//コントロール生成時のオプション
	struct Option_UseOption : public Option
	{
		//ツールバーの右端に、サイズ変更用のマークをつける
		bool SizeGrip;

		//ツールチップを出せるようにする
		bool Tooltip;				

		//各パーツのプロパティ
		PartsOption Parts;
	
		//右寄せ・左寄せに関する情報
		//この要素の値より小さい位置のパーツ→左寄せ、これ以上→右寄せ
		INT RightJustifyIndex;

		//右寄せ・左寄せに関する情報
		//左寄せと右寄せのパーツが被ったとき、true=右寄せを上にする false=左寄せを上にする
		bool RightUpper;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			SizeGrip = true;
			Tooltip = false;
			RightJustifyIndex = 256;
			RightUpper = false;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void* option )throw( )
	{
		return mNew mStatusBar;
	}

	//パーツに文字列を設定する
	//id : 設定先のパーツ(PartsOptionEntry::Idにセットした文字列)
	//str : セットしたい文字列
	//notab : タブを無視する場合true
	//ret : 処理に成功したらtrue
	//同一IDの項目が複数あった場合、全部にセットされます。
	bool SetText( const WString& id , const WString& str , bool notab = true );

	//ボーダーラインの種類を変更する
	//id : 設定先のパーツ(PartsOptionEntry::Idにセットした文字列)
	//border : 設定したいボーダーラインの種類
	//ret : 処理に成功したらtrue
	//同一IDの項目が複数あった場合、全部にセットされます。
	bool SetBorder( const WString& id , BorderType border );

	//幅を変更する
	//id : 設定先のパーツ(PartsOptionEntry::Idにセットした文字列)
	//width : 設定したい幅
	//ret : 処理に成功したらtrue
	//同一IDの項目が複数あった場合、全部にセットされます。
	bool SetWidth( const WString& id , UINT width );

protected:
	mStatusBar();
	virtual ~mStatusBar();

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

	//ウインドウ生成を完了した場合にコールされる
	virtual bool OnCreate( const void* opt )override;

	//ウインドウプロシージャ
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )override;

private:

	mStatusBar( const mStatusBar& src ) = delete;
	mStatusBar& operator=( const mStatusBar& src ) = delete;

protected:

	//各パーツの情報
	PartsOption MyPartsOption;

	//パーツの(右端の)位置
	INT MyPartsPos[ 255 ];

	//左寄せと右寄せのパーツが被ったとき、どちらのパーツを上にするか
	bool MyRightUpper;

	//パーツサイズを更新する
	//MyPartsSizeの値を更新します。
	//ret : 成功したときtrue
	bool ModifyPartsSize( void );

	//何番目のパーツから右寄せにするか
	INT MyRightJustifyIndex;

	//ScanItemから呼び出されるコールバック関数
	//index : 何番目のパーツに対するコールバックの呼び出しであるか
	//ret : 正常終了であればtrue
	typedef std::function< bool( INT index ) > ScanItemCallback;

	//全パーツをスキャンし、IDが一致する物に対してコールバックを呼び出す
	//id : コールバック関数呼び出し対象のID
	//ret : コールバック関数が1度も呼び出されなかった→true
	//      コールバック関数が全部trueを返した→true
	//      コールバック関数がfalseを返したことがある→false
	//・コールバック関数がfalseを返した場合でも、ScanItemの処理は中断されません。
	//  他にも処理対象の項目があると、その項目に対して再びコールされます。
	bool ScanItem( const WString& id , ScanItemCallback callback );

	//指定したインデックスのパーツを再設定する
	//index : 再設定対象のパーツ
	//ret : 成功時真
	bool ModifyParts( INT index );

};

#endif	//MSTATUSBAR_H_INCLUDED

