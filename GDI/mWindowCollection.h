//----------------------------------------------------------------------------
// ウインドウ管理（子ウインドウ管理）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MWINDOWCOLLECTION_H_INCLUDED
#define MWINDOWCOLLECTION_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "GDI/mWindow.h"
#include <unordered_map>

class mWindowCollection
{
public:

	//コンストラクタ
	//parent : 親オブジェクトが存在する場合それのポインタ。
	//         nullptrを渡すと、親なしとなります。
	mWindowCollection( mWindow* parent );

	virtual ~mWindowCollection();

	//ファクトリメソッドのtypedef。
	typedef mWindow* (*mWindowFactory)( const void* );

	//コントロールをコレクションに追加する
	//factory : mWindow*型のインスタンスを生成して返すファクトリメソッド
	//          mNewでインスタンスを生成して返すものであること。
	//id : 追加するコントロールを識別するID
	//     IDは任意の文字列ですが、以下の制限があります。
	//     (1)空文字列はダメ
	//     (2)同じmWindowCollectionインスタンス内で重複するものはダメ
	//opt : 生成したインスタンスに引き渡すオプション（ユーザ定義）
	//ret : 生成に成功した場合生成したコントロール。失敗した場合nullptr
	template< class T > T* AddControl( const WString& id , const struct T::Option* opt )
	{
		return (T*)mWindowCollection::AddControlInternal( T::Factory , id , opt );
	}
	template< class T > T* AddControl( const WString& id )
	{
		return (T*)mWindowCollection::AddControlInternal( T::Factory , id , nullptr );
	}

	//指定したIDのコントロールを削除する
	//id : 削除したいコントロールのID
	//ret : 正常に削除できた場合true
	bool RemoveControl( const WString& id );

	//指定したウインドウハンドルからIDを検索します。
	//hwnd : 検索するウインドウハンドル
	//ret : ID名。該当するものがなかった場合は空文字列。
	WString QueryId( HWND hwnd )const;

	//IDからコントロールを取得します。
	//id : 検索するID
	//ret : IDから検索したウインドウハンドル。該当がなかった場合はnullptr。
	mWindow* Query( const WString& id )const;

	//管轄下のオブジェクトのサイズを調整する
	//world : クライアントエリアの矩形(または子コントロールの描画領域)
	bool AdjustSize( const RECT& world );

	//管轄下のオブジェクトのサイズを調整する
	//hwnd : 指定したウインドウのクライアント領域に合わせる
	bool AdjustSize( HWND hwnd );

	//管轄下のオブジェクトにウインドウメッセージを送信する
	//msg : 送信するウインドウメッセージ
	//wparam : 送信するメッセージのWPARAMパラメータ
	//lparam : 送信するメッセージのLPARAMパラメータ
	bool ReflectMessage( UINT msg , WPARAM wparam , LPARAM lparam );

private:
	mWindowCollection() = delete;
	mWindowCollection( const mWindowCollection& src ) = delete;
	mWindowCollection& operator=( const mWindowCollection& src ) = delete;

	//コントロールをコレクションに追加する
	//AddControlの内部処理
	//factory : ファクトリメソッド
	//id : 登録するID
	//opt : ファクトリメソッドで生成したオブジェクトに渡すオプション
	//ret : 生成したオブジェクトのポインタ
	mWindow* AddControlInternal( mWindowFactory factory , const WString& id , const void* opt );

protected:

	//親オブジェクト
	mWindow* const MyParent;

	//オブジェクトのマッピング１
	//左：オブジェクトのID
	//右：オブジェクトへのポインタ
	typedef std::unordered_map<WString,mWindow*> IdMap;
	IdMap MyIdMap;

	//オブジェクトのマッピング２
	//左：ウインドウハンドル
	//右：オブジェクトへのポインタ
	typedef std::unordered_map<HWND,WString> HwndMap;
	HwndMap MyHwndMap;

	//mWindow::WindowPosition構造体で定義から、実際の座標を求める
	//srcpos : 変換する座標
	//retPos : 変換後の実際の座標
	//world : 親のサイズを格納したRECT構造体へのポインタ。nullptrの場合このウインドウのクライアントエリア全体になる。
	//ret : 成功時true
	static bool CalcAbsolutePosition( const mWindow::WindowPosition& srcpos , RECT& retPos , const RECT& world );

};

#endif

