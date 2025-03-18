//----------------------------------------------------------------------------
// ウインドウ管理（ツールバー）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------


/*
●用途
ツールバーの実装です

*/

#ifndef MTOOLBAR_H_INCLUDED
#define MTOOLBAR_H_INCLUDED

#include "mStandard.h"
#include "mWindow.h"
#include "mGdiDualImagelist.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class mToolBar : public mWindow
{
public:

	//コントロール生成時のオプション
	//実際に作成するときは、Option構造体を直接使わずに、シチュエーションに合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct Option : public mWindow::Option
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
		//アイコン用イメージリスト作成時のオプション
		mGdiImagelist::Option_UseOption ImgOpt;

		//フラットタイプのツールバーにするならばtrue
		bool IsFlat;

		//リストスタイル(文字列がアイコンの右)ならtrue、そうでない(文字列がアイコンの下)ならfalse
		bool IsListStyle;

		//ツールチップ用のウインドウメッセージを生成するか
		//trueにするとWM_NOTIFY/TTN_GETDISPINFOが飛ぶようになります
		bool IsTooltipReq;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			IsFlat = false;
			IsListStyle = false;
			IsTooltipReq = true;
		}
	};

	//ファクトリメソッド
	static mWindow* Factory( const void * opt )throw( )
	{
		if( opt == nullptr )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"Option required" ); 
			return nullptr;
		}
		return mNew mToolBar;
	}

	//アイテムの種類
	enum ItemType
	{
		BUTTONTYPE,		//押すだけのボタン
		CHECKBOXTYPE,	//押したらそのままになって、もう一度押すと戻るボタン
		SEPARATOR,		//セパレータである
		DROPDOWN,		//ボタンの横に▼マークがある（ボタンと▼は独立）
		WHOLEDROPDOWN,	//ボタンの横に▼マークがある（ボタンと▼は一体型）
	};

	//ツールバーのアイテム
	struct ItemOptionEntry
	{
		//アイテムを生成するときに、ここに指定したセクションの内容を使う。
		//空文字列の場合は無効（ルートセクションは使用されない）
		//ToolbarUpdateHandle::SetInitFile()を使用してIniファイルを指定している場合のみ有効。
		WString SectionName;

		ItemType Type;			//アイテムの種類

		//ツールバーのボタンが押されたときに、アプリに通知される情報
		UINT FunctionId;		//ユーザー定義の機能ID
		ULONG_PTR UserData;		//ユーザー定義の値
		WString OptString;		//ユーザー定義の文字列

		//ツールバーの表示物を構成するグラフィックと文字列
		WString Caption;		//表示文字列
		WString ImageId;		//表示するアイコンのID(通常と選択状態で同じIDを使います)

		ItemOptionEntry()
		{
			Type = ItemType::BUTTONTYPE;
			FunctionId = 0;
			UserData = 0;
		}
	};
	typedef std::vector<ItemOptionEntry> ItemOption;

	//ツールバーをアップデートするためのクラス。
	//1.ToolbarUpdateHandleのコンストラクタに、mToolBarを渡してください。
	//2.Image()、Item()を使って参照を取得し、ツールバーの設定を変更してください。
	//  img1が通常時のアイコン、img2が選択時のアイコンになります。
	//3.ToolbarUpdateHandleのインスタンスを破棄すると、ツールバーに反映されます。
	class ToolbarUpdateHandle final
	{
	public:
		ToolbarUpdateHandle( mToolBar& toolbar );
		~ToolbarUpdateHandle();

		//ツールバーのアイコンにするイメージリストを取得
		//【注意】取得した参照のポインタを保持したり、解放しようとしたりしないでください。
		mGdiDualImagelist& Image()const;

		//アイテム設定の参照を取得
		//【注意】取得した参照のポインタを保持したり、解放しようとしたりしないでください。
		ItemOption& Item()const;

	protected:
		ToolbarUpdateHandle() = delete;
		ToolbarUpdateHandle& operator=( const ToolbarUpdateHandle& src ) = delete;

		mToolBar* MyParent;				//親オブジェクトの参照1
		mGdiDualImagelist* MyImgage;	//更新中のイメージリスト
		ItemOption* MyItemOption;		//メニューのアイテム設定
	};

	//アイコンを有効・無効にする
	//FunctionId : 設定する機能ID(ItemOption::FunctionIdで指定したもの)
	//enable : 真にすると有効にする、偽にすると無効にする。
	//checked : 真にするとチェックする。偽にするとチェックを外す。
	//同一のFunctionIdを持つメニューが複数ある場合、全てに適用されます。
	bool SetState( UINT FunctionId , bool enable , bool checked );

	//アイテムの情報を取得する
	//index : 取得したいアイテムのインデックス
	//	・左からの0ベースのインデックスになります。
	//	・WM_NOTIFY→NM_CLICKメッセージのLPARAM、NMMOUSE::dwItemSpecの値は
	//	　0ベースのインデックスになっているので、この値をつかってもOKです。
	//retInfo : 取得したアイテムの情報
	//ret : 成功時真
	bool QueryItem( INT index , ItemOptionEntry& retInfo )const;

	//アイテムの情報を取得する
	//mouse : WM_NOTIFY→NM_CLICKメッセージのLPARAMをLPNMMOUSEにキャストしたもの
	//retInfo : 取得したアイテムの情報
	//ret : 成功時真
	bool QueryItem( const LPNMMOUSE mouse , ItemOptionEntry& retInfo )const;

protected:
	mToolBar();
	virtual ~mToolBar()final;

	//ウインドウクラスの登録をする
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//ウインドウを開く
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;

	//ウインドウ生成を完了した場合にコールされる
	virtual bool OnCreate( const void* opt )override;

private:

	mToolBar( const mToolBar& src ) = delete;
	mToolBar& operator=( const mToolBar& src ) = delete;

	//現在のツールバーの状態を適用する。以下のメンバ変数の値が実際のオブジェクトに反映される。
	//・MyImgage
	//・MyItemOption
	//ret : 成功時true
	bool ExecUpdate( void );

	//現在のツールバーのボタンを全部破棄する
	//ret : 成功時true
	bool ExecClear( void );

protected:

	//ツールバーに貼り付けるアイコン
	mGdiDualImagelist* MyImgage;

	//アイテム設定
	ItemOption MyItemOption;

	//内部IDの使用中の値一覧
	typedef mUniqueValue<INT> InternalIdStock;
	InternalIdStock MyInternalIdStock;

};

#pragma region MTOOLBAR_SAMPLE
#ifdef SAMPLE_CODE
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

		case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lparam;
			WString id = MyChild->QueryId( nmhdr->hwndFrom );

			if( nmhdr->code == NM_CLICK && id == L"TOOLBAR" )
			{
				LPNMMOUSE lpnm = (LPNMMOUSE)lparam;
				mToolBar* tb = (mToolBar*)MyChild->Query( id );

				mToolBar::ItemOptionEntry item;
				tb->QueryItem( lpnm , item );

				WString str;
				sprintf( str , L"メニュー「%ws」(FunctionId=%d)が選択されました" , item.Caption.c_str(), item.FunctionId );
				::MessageBoxW( GetMyHwnd() , str.c_str(), L"" , 0);
			}
		}
		break;
		}
		return mWindow::WindowProcedure( msg , wparam , lparam );
	}
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
	{
		retSetting.ClassName = L"TESTCLASS";
		return true;
	}
	virtual void CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
	{
		retSetting.WindowName = L"TEST WINDOW";
		retSetting.Width = CW_USEDEFAULT;
		retSetting.Style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
	}
	virtual void OnCreate( const void* option )
	{

		//ボタンなどを貼り付ける場合は、MyChildメンバに実体をつくってやります。
		//（初期状態ではnullptrになっています。）
		//main関数では親ウインドウがないからnullptrを渡していますが、
		//今回はこのウインドウが親になるので、thisを渡します。
		//あとは、MyChildにオブジェクトを登録していくだけです。
		MyChild = mNew mWindowCollection( this );
		{
			//ツールバーの生成
			mToolBar::Option_UseOption opt;
			opt.IsFlat = true;
			opt.IsListStyle = true;
			opt.ImgOpt.width = 16;		//アイコンのサイズ
			opt.ImgOpt.height = 16;		//アイコンのサイズ
			mToolBar* tb = MyChild->AddControl<mToolBar>( L"TOOLBAR" , &opt );

			//ツールバーの情報登録オブジェクトを生成
			mToolBar::ToolbarUpdateHandle handle( *tb );

			//ツールバーに張り付けるアイコンを生成
			for( DWORD i = 0 ; i < 4 ; i++ )
			{
				WString idstr;
				sprintf( idstr , L"IMAGE_%d" , i + 1 );

				mGdiBitmap::Option_LoadFile img_opt;
				sprintf( img_opt.path , L"%d.bmp" , i + 1 );
				mGdiBitmap bmp1( &img_opt );

				sprintf( img_opt.path , L"%d_h.bmp" , i + 1 );
				mGdiBitmap bmp2( &img_opt );

				handle.Image().AddImage( idstr , bmp1 , bmp2 , RGB( 255 , 255 , 255 ) , RGB( 255 , 255 , 255 ) );
			}

			//ツールバーの項目登録
			mToolBar::ItemOptionEntry entry;
			entry.ImageId = L"IMAGE_1";
			entry.Caption = L"";
			entry.Type = mToolBar::ItemType::CHECKBOXTYPE;
			entry.FunctionId = 1;
			handle.Item().push_back( entry );

			entry.ImageId = L"IMAGE_2";
			entry.Caption = L"BUTTON2";
			entry.FunctionId = 2;
			entry.Type = mToolBar::ItemType::BUTTONTYPE;
			handle.Item().push_back( entry );

			entry.ImageId = L"IMAGE_3";
			entry.Caption = L"BUTTON3";
			entry.FunctionId = 3;
			entry.Type = mToolBar::ItemType::BUTTONTYPE;
			handle.Item().push_back( entry );

			mToolBar::ItemOptionEntry entry_sep;
			entry_sep.Type = mToolBar::ItemType::SEPARATOR;
			entry_sep.FunctionId = 999;	//セパレータも、FunctionId=0はNG
			handle.Item().push_back( entry_sep );

			entry.ImageId = L"IMAGE_4";
			entry.Caption = L"BUTTON4";
			entry.FunctionId = 4;
			entry.Type = mToolBar::ItemType::BUTTONTYPE;
			handle.Item().push_back( entry );
		}
	}
};

int main( int argc , char** argv )
{
	InitializeLibrary();

	//ウインドウの生成
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
#pragma endregion

#endif //MTOOLBAR_H_INCLUDED

