//----------------------------------------------------------------------------
// ウインドウ管理（ウインドウの基底クラス）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
ウインドウ、ボタン、ラジオボタン等のGUIなオブジェクトの基底クラスです。

●注意点
・mWindowを継承したクラスは、それぞれ以下の構造体、メソッドを持つこと
  (mWindowCollection::AddControl()で名前指定で呼び出しているため)
　Factory() …新しいインスタンスを返すファクトリメソッド
  struct Option …そのクラス固有の設定項目を格納する構造体
*/

//●サンプルコード
#ifdef SAMPLE_CODE
//●使用サンプル（別ファイルにコピペして下さい）
//動作：ウインドウを1個作成します。そのウインドウを閉じるとプログラムが終了します。
//前のサンプル→なし
//次のサンプル→mButton.hを参照(ウインドウにボタンを貼り付けるコードがあります)
#include "GDI/mWindow.h"
#include "GDI/mWindowCollection.h"
class TestWindow : public mWindow
{
public:
	//ファクトリメソッド（継承したクラス毎に静的に作って下さい）
	static mWindow* Factory( const void * )throw( )
	{
		return mNew TestWindow;
	}
protected:
	//ウインドウプロシージャ
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
	{
		//WM_DESTROYが来たらメッセージループを終了します。
		if( msg == WM_DESTROY )
		{
			PostQuitMessage( 0 );
			return 0;
		}
		return mWindow::WindowProcedure( msg , wparam , lparam );
	}
	//mWindowCollection::AddControlから呼ばれます。ウインドウクラスの情報を登録します。
	//ここでは、TESTCLASSという名前だけ決めています。
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
	{
		retSetting.ClassName = L"TEST";
		return true;
	}
	//mWindowCollection::AddControlから呼ばれます。作成するウインドウの情報を登録します。
	//ここでは、表示スタイルだけ決めています。
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
	{
		retSetting.Style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
		retSetting.WindowName = L"TEST";
		return true;
	}

};

int main( int argc , char** argv )
{
	InitializeLibrary();	//初期化

	//ウインドウのコレクション。親ウインドウが無いのでnullptrを渡します。
	mWindowCollection root_collection( nullptr );
	//class TestWindowをTESTという名前で登録します。
	root_collection.AddControl<TestWindow>( L"TEST" );

	//メッセージループ
	MSG msg;
	while( GetMessageW( &msg , 0 , 0 , 0 ) )
	{
		TranslateMessage( &msg ); //EDITコントロールなどWM_CHARを受け取るなら必要
		DispatchMessageW( &msg );
	}
	return 0;
}
#endif

#ifndef MWINDOW_H_INCLUDED
#define MWINDOW_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "GDI/mMenu.h"
#include "GDI/mWindowPosition.h"
#include "GDI/mGdiResource.h"

class mWindowCollection;

class mWindow
{
public:
	
	/*
	※ファクトリメソッド作成例
	　以下コードを継承したクラスにコピペし、「mNew mWindow」の部分を継承したクラス名で置き換えて下さい。
	static mWindow* Factory( const void * )throw( )
	{
		return mNew mWindow;
	}
	*/

	//相対的に座標を決定するための情報
	typedef mWindowPosition::POSITION POSITION;

	//ウインドウの相対的な位置設定
	//ウインドウ(あるいはボタンなどのパーツ類)を親ウインドウのクライアントエリア内のどのあたりに配置するかを指定します。
	//上下左右について、親ウインドウの上端・左端からの比率＋オフセットで表します。
	//rate=0.0,offset=10	→左端から10ピクセルの位置
	//rate=1.0,offset=-10	→右端から10ピクセルの位置
	//rate=0.5,offset=10	→中央から右に10ピクセルの位置
	//左端(rate=0.1,offset=  10)+右端(rate=0.1,offset=110) →左寄せ固定幅100ピクセル
	//左端(rate=1.0,offset=-110)+右端(rate=1.0,offset=-10) →右寄せ固定幅100ピクセル
	//左端(rate=0.0,offset=  10)+右端(rate=0.5,offset=-10) →幅は親の50パーセント＋10ピクセルのパディング。
	typedef mWindowPosition::WindowPosition WindowPosition;

	//ウインドウの相対位置を設定する
	//newpos : 新しい位置
	virtual bool SetWindowPosition( const WindowPosition& newpos );

	//ウインドウの可視/不可視を変更する
	//newstate : trueで可視、falseで不可視
	//ret : 成功時true
	virtual bool SetVisible( bool newstate );

	//ウインドウの有効/無効を変更する
	//newstate : trueで有効、falseで無効
	//ret : 成功時true
	virtual bool SetEnable( bool newstate );

	//ウインドウの位置をスクリーン座標で得る
	//ret : スクリーン座標によるウインドウの位置(エラーの場合0,0,0,0)
	virtual RECT GetRect( void ) const;

	//ウインドウのサイズを得る
	//ret : ウインドウのサイズ(エラーの場合0,0)
	virtual SIZE GetSize( void ) const;

	//このウインドウのクライアント座標をスクリーン座標に変換する
	//client_pos : クライアント座標
	//ret : スクリーン座標(エラーの場合0,0)
	virtual POINT Client2Screen( const POINT& client_pos ) const;

	//このウインドウのクライアント座標をスクリーン座標に変換する
	//x : クライアント座標(X座標)
	//x : クライアント座標(Y座標)
	//ret : スクリーン座標(エラーの場合0,0)
	virtual POINT Client2Screen( INT x , INT y ) const;

	//スクリーン座標をこのウインドウのクライアント座標に変換する
	//client_pos : スクリーン座標
	//ret : クライアント座標(エラーの場合0,0)
	virtual POINT Screen2Client( const POINT& client_pos ) const;

	//スクリーン座標をこのウインドウのクライアント座標に変換する
	//x : スクリーン座標(X座標)
	//x : スクリーン座標(Y座標)
	//ret : クライアント座標(エラーの場合0,0)
	virtual POINT Screen2Client( INT x , INT y ) const;

	//フォーカスを設定する
	//ret : 成功時true
	virtual bool SetFocus( void )const;

	//ウインドウを移動する
	virtual void MoveWindowPosition( const RECT& pos );

	//
	LRESULT MessageSend( UINT msg , WPARAM wparam , LPARAM lparam )const;

	//
	LRESULT MessagePost( UINT msg , WPARAM wparam , LPARAM lparam )const;

protected:

	friend class mGlobalWindowFunc;

	//ウインドウプロシージャ
	//mWindow::WindowProcedure()を呼び出した場合、
	//(1)ウインドウプロシージャが置き換えられていれば、置き換え前のウインドウプロシージャに、
	//(2)ウインドウプロシージャが置き換えられていなければ、DefWindowProcW()に
	//メッセージを処理させます。
	virtual LRESULT WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam );

	//---------------------------------------
	// サブクラスへのコールバック関数
	//---------------------------------------	
	mWindow();
	virtual ~mWindow();
	friend class mWindowCollection;

	//ウインドウクラスの設定用構造体
	struct WindowClassSetting
	{
		WString ClassName;	//登録するクラスの名前。
		UINT Style;			//スタイル
		HICON Icon;			//関連づけるアイコン
		HICON IconSmall;	//アイコンの小さい方
		HCURSOR Cursor;		//カーソル
		HBRUSH Background;	//バックグラウンドを塗りつぶすのに使うブラシ

		//デフォルト値の設定
		WindowClassSetting()
		{
			ClassName = L"";
			Style = 0;
			Icon = nullptr;
			IconSmall = nullptr;
			Cursor = nullptr;	
			Background = nullptr;
		}
	};

	//ウインドウクラスの登録をする
	//retSetting : 設定を返すための構造体
	//opt : ユーザ定義の情報(mWindowFactoryに渡したものがそのまま渡ってくる)
	//ret : ウインドウクラスの登録を行う場合true
	//      ウインドウクラスの登録を行わない場合はfalse
	//      (定義済みコントロールクラスを利用する場合など)
	//※mWindow::WindowClassSettingCallback()は何もせずfalseを返すだけになっています
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt );

	//ウインドウ作成情報
	struct CreateWindowSetting
	{
		WString ClassName;		//ウインドウの作成に使うウインドウクラス
		WString WindowName;		//ウインドウの名前（タイトル）
		DWORD Style;			//スタイル１
		DWORD ExStyle;			//スタイル２
		INT x;					//X座標
		INT y;					//Y座標
		INT Width;				//幅
		INT Height;				//高さ

		//trueにするとウインドウプロシージャを置き換えます
		//ボタンなどコモンコントロールでオリジナルな動作をさせたい場合、
		//これをtrueにすることとサブクラス化され、ウインドウメッセージを取得できるようになります。
		bool ProcedureChange;

		//デフォルト値の設定
		CreateWindowSetting()
		{
			ClassName = L"";
			WindowName = L"";
			Style = WS_VISIBLE;
			ExStyle = 0;
			x = 0;
			y = 0;
			Width = 100;
			Height = 100;
			ProcedureChange = false;
		}
	};

	//ウインドウを開く
	//retSetting : ウインドウ作成のための情報
	//opt : ユーザ定義の情報(mWindowFactoryに渡したものがそのまま渡ってくる)
	//ret : ウインドウを作成する場合はtrue。ウインドウを作成しない場合はfalse。
	//      ※falseを返した場合は、retSettingの内容は全て無視され、自分でウインドウを生成する必要があります。
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt ) = 0;

	//ウインドウ生成を完了した場合にコールされる
	//ret : 最終的に問題なかった場合はtrue
	//      ※falseを返した場合は、ウインドウの生成がキャンセルされる。
	virtual bool OnCreate( const void* opt );

	//ウインドウが破棄されようとしているときコールされる
	virtual void OnDestroy( void );

private:

	mWindow( const mWindow& src ) = delete;
	mWindow& operator=( const mWindow& src ) = delete;

	//このウインドウに対するウインドウクラスの名前
	WString MyWindowClass;

	//ウインドウクラスは自分自身で登録したものか？(Yes=true)
	bool MyIsWindowClassOriginal;

	//このウインドウのハンドル
	HWND MyHwnd;

	//このウインドウの親ウインドウのハンドル
	HWND MyParent;

	//このウインドウの最上位ウインドウのハンドル
	HWND MyRoot;

	//置き換え前のウインドウプロシージャ
	WNDPROC MyDefWndproc;

	//ウインドウの配置情報
	WindowPosition MyPosition;

protected:

	//子ウインドウのコレクション
	mWindowCollection* MyChild;

	//GDIリソースのコレクション
	mGdiResource MyGdiResource;

	//メニューのハンドル
	mMenu* MyMenu;

	//自分自身のウインドウハンドルを取得する
	HWND GetMyHwnd( void )const;

	//親ウインドウのハンドルを取得する
	HWND GetMyParent( void )const;

	//最上位ウインドウのハンドルを取得する
	HWND GetMyRoot( void )const;

};



#endif


