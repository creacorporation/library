//----------------------------------------------------------------------------
// ウインドウ管理（メニュー）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
メニューの管理クラスです

・メニューは人間が操作たときだけ駆動するもの。
　つまり人間の操作に付いてこれれば速度は十分ということになるから、処理速度のことは考えてません。
*/

#ifndef MMENU_H_INCLUDED
#define MMENU_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mUniqueValue.h"
#include "mGdiResource.h"
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <functional>

class mMenu
{
public:

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
		//ポップアップなメニューにするならtrue。普通のチェックをつけるならfalse。
		bool IsPopup;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			IsPopup = false;
		}
	};

	//コンストラクタ
	//opt : コントロール生成時のオプション(nullptr可)
	mMenu( const Option* opt );

	//デストラクタ
	virtual ~mMenu();

	//メニューの項目がチェックを持つかどうか。あるいはセパレータかどうか。
	//ラジオボタンタイプ(●印の物)を選択した場合でも、
	//メニュー選択時に勝手に他の項目のチェックが解除されたりはしません。
	//レ印にせよ●印にせよ、チェックの状態を更新するのはプログラマ側で行います。
	enum MenuType
	{
		NORMALCHECK,	//レ印チェック
		RADIOCHECK,		//●印チェック
		SEPARATOR,		//セパレータである
	};
	
	//メニューのデータ
	struct MenuOption
	{
		//非選択状態のビットマップ
		//(ここで指定した文字列をキーにしてmGdiResourceからHBITMAPのハンドルを取得します)
		WString UncheckedBitmap;

		//選択状態のビットマップ
		//(ここで指定した文字列をキーにしてmGdiResourceからHBITMAPのハンドルを取得します)
		WString CheckedBitmap;

		WStringVector Path;		//項目の階層を示すパス
		MenuType Type;			//メニューの体裁

		UINT FunctionId;		//ユーザー定義の機能ID（逆検索するときにも使う）
		ULONG_PTR UserData;		//ユーザー定義の値
		WString OptString;		//ユーザー定義の文字列

		MenuOption()
		{
			FunctionId = 0;
			UserData = 0;
			Type = MenuType::NORMALCHECK;
		}
	};

	//MenuOptionのアレイ
	typedef std::deque<MenuOption> MenuOptionArray;

	//メニューにアイテムを追加する
	//item : 追加する項目
	//ret : 成功したらtrue
	bool AddItem( const MenuOption& item , const mGdiResource* res = nullptr );

	//メニューからアイテムを削除する
	//Path : 削除するアイテム(MenuOption::Pathで指定したもの)
	//ret : 成功したらtrue
	bool RemoveItem( const WStringVector& Path );

	//メニューを有効・無効にする
	//FunctionId : 設定する機能ID(MenuOption::FunctionIdで指定したもの)
	//enable : 真にすると有効にする、偽にすると無効にする。
	//checked : 真にするとチェックする。偽にするとチェックを外す。
	//同一のFunctionIdを持つメニューが複数ある場合、全てに適用されます。
	bool SetState( UINT FunctionId , bool enable , bool checked );

	//ウインドウメッセージから選択されたメニューのデータを取得する
	//wparam : ウインドウメッセージのWPARAMパラメータ
	//retItem : 取得したデータ
	//ret : データを取得したとき真。該当のメニュー項目がなかった場合偽。
	bool QuerySelectedMenu( WPARAM wparam , MenuOption& retItem )const;

	//FunctionIdから選択されたメニューのデータを取得する
	//function_id : 検索したい機能ID
	//retItem : 取得したデータ(該当する項目全てが返る)
	//ret : データを取得したとき真。該当のメニュー項目が1個もなかった場合偽。
	bool QueryItem( UINT function_id , MenuOptionArray& retItem )const;

	//メニューのハンドルを取得する
	HMENU GetMenuHandle( void );

private:
	mMenu( const mMenu& src ) = delete;
	mMenu& operator=( const mMenu& src ) = delete;

protected:
	//-----------------------------------------------------------
	// データ構造とメンバ変数
	//-----------------------------------------------------------
	struct MenuItemEntry;
	typedef std::deque<MenuItemEntry*> MenuItem;

	//1つのサブメニューの情報を格納
	struct MenuHandle
	{
		MenuHandle() = delete;
		MenuHandle( const MenuHandle& src ) = delete;
		MenuHandle& operator=( const MenuHandle& src ) = delete;

		HMENU Handle;		//メニューのハンドル
		MenuItem Items;		//メニューに含まれるアイテム

		MenuHandle( bool IsPopup )
		{
			if( IsPopup )
			{
				Handle = ::CreatePopupMenu();
			}
			else
			{
				Handle = ::CreateMenu();
			}
		}

		~MenuHandle()
		{
			MenuItem::iterator itr;
			for( itr = Items.begin() ; itr != Items.end() ; itr++ )
			{
				mDelete *itr;
			}
			::DestroyMenu( Handle );
		}
	};

	//メニューの各項目についての情報を格納
	struct MenuItemEntry
	{
		MenuItemEntry() = delete;
		MenuItemEntry( MenuItemEntry& src ) = delete;
		MenuItemEntry& operator=( const MenuItemEntry& src ) = delete;

		WString SectionName;		//Iniファイルのセクション名
		WString UnchedkedBitmap;	//非選択状態のビットマップ
		WString CheckedBitmap;		//選択状態のビットマップ

		MenuType Type;				//メニューのタイプ
		mMenu& Root;				//ルートオブジェクト
		MenuHandle& Parent;			//親オブジェクト
		WString Name;				//項目の名称
		USHORT InternalId;			//内部ID（WINAPIに登録するID）
		UINT FunctionId;			//ユーザー定義の機能ID（逆検索するときにも使う）
		ULONG_PTR UserData;			//ユーザー定義の値
		WString OptString;			//ユーザー定義の文字列
		MenuHandle* Child;			//サブメニュー(ない場合はnullptr)

		MenuItemEntry( MenuHandle& parent , mMenu& inst )
			: Parent( parent )
			, Root( inst )
		{
			Type = MenuType::NORMALCHECK;
			InternalId = 0;
			FunctionId = 0;
			UserData = 0;
			Child = nullptr;
		}
		~MenuItemEntry()
		{
			mDelete Child;
			Root.MyInternalIdStock.Return( InternalId );
		}
	};

	//ルートメニュー
	MenuHandle* MyRootMenu;

	//内部IDの使用中の値一覧
	typedef mUniqueValue<USHORT> InternalIdStock;
	InternalIdStock MyInternalIdStock;

protected:
	//-----------------------------------------------------------
	// ユーティリティ関数
	//-----------------------------------------------------------

	//指定したパスを持つアイテムへのポインタを返します
	//Path : 取得したいアイテムのパス
	//Create : true = 指定したアイテムが存在しない場合、それを新規に作成して返します
	//         false = 指定したアイテムが存在しない場合、エラーとなります
	//ret : 成功したときアイテムへのポインタ。エラーのときnullptr。
	//・Createがfalseの場合、内部変数を書き換えることはありません(const_castしてOK)。
	MenuItemEntry* SearchItemEntry( const WStringVector& Path , bool Create );

	//ScanFunctionIdから呼び出すコールバック関数
	//entry : 処理対象の項目
	//ret : 処理が成功した場合真
	typedef std::function< bool( MenuItemEntry* entry ) > ScanItemCallback;

	//指定した項目配下をスキャンし、指定した機能IDを持つ項目に対して、コールバック関数を呼び出します。
	//menu : スキャンする対象(全項目を対象にするならMyRootMenuを渡します)
	//functionid : コールバックを呼び出す対象にする機能ID
	//callback_func : 呼び出すコールバック関数
	//ret : コールバック関数が1度も呼び出されなかった→true
	//      コールバック関数が全部trueを返した→true
	//      コールバック関数がfalseを返したことがある→false
	//・コールバック関数が偽を返した場合でも、ScanFunctionIdの処理は中断されません。
	//  他にも処理対象の項目があると、その項目に対して再びコールされます。
	bool ScanFunctionId( MenuHandle* menu , UINT functionid , ScanItemCallback callback_func )const;

	//指定した項目配下をスキャンし、指定した内部IDを持つ項目のパスを探します。
	//menu : スキャンする対象(全項目を対象にするならMyRootMenuを渡します)
	//internalid : コールバックを呼び出す対象にする内部ID
	//ret : 内部IDが発見された場合真
	bool ScanInternalId( MenuHandle* menu , USHORT internalid , WStringDeque& retPath )const;

	//MenuItemEntryの内容をMENUITEMINFOWに展開する
	//src : 展開元のMenuItemEntry構造体
	//retDst : 展開先のMENUITEMINFOW構造体
	//sec : 追加情報を取得するためのセクション(nullptr可)
	//res : メニューに付与するビットマップを取得するリソースプール(nullptr可)
	//ret : 成功時真
	bool SetMenuItemInfoStruct( const MenuItemEntry& src , MENUITEMINFOW& retDst , const mGdiResource* res )const;

};

#pragma region MMENU_SAMPLE_CODE
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
		case WM_COMMAND:
		{
			if( lparam == 0 )	//メニュー項目が選択されたときはlparam=0
			{
				//選択されたメニューを取得
				mMenu::MenuOption item;
				if( MyMenu->QuerySelectedMenu( wparam , item ) )
				{
					WString str;
					sprintf( str , L"FunctionId=%dが選択されました" , item.FunctionId );
					MessageBoxW( GetMyHwnd() , str.c_str() , L"" , 0 );
				}
			}
			return 0;
		}

		default:
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

		MyChild = mNew mWindowCollection( this );

		{
			MyMenu = mNew mMenu( nullptr );
			::SetMenu( GetMyHwnd() , MyMenu->GetMenuHandle() );

			mMenu::MenuOption opt;
			opt.FunctionId = 1;
			opt.Path.push_back( L"メニュー１" );
			MyMenu->AddItem( opt );

			opt.FunctionId = 2;
			opt.Path[ 0 ] = L"メニュー２";
			MyMenu->AddItem( opt );

			opt.FunctionId = 3;
			opt.Path[ 0 ] = L"メニュー３";
			MyMenu->AddItem( opt );

			opt.FunctionId = 4;
			opt.Path[ 0 ] = L"メニュー３";
			opt.Path.push_back( L"サブメニュー１" );
			MyMenu->AddItem( opt );

			opt.FunctionId = 5;
			opt.Path[ 0 ] = L"メニュー３";
			opt.Path[ 1 ] = L"サブメニュー１";
			opt.Path.push_back( L"サブメニュー２" );
			MyMenu->AddItem( opt );

			DrawMenuBar( GetMyHwnd() );
		}
		MyChild->AdjustSize( GetMyHwnd() );
		UpdateWindow( GetMyHwnd() ); 
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

#endif //MMENU_H_INCLUDED

