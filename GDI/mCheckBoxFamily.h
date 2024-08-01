//----------------------------------------------------------------------------
// ウインドウ管理（チェックボックス系）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
チェックボックス系コントロールの基底クラスです。

以下のコントロールの親となります。
・チェックボックス
・ラジオボックス

*/

#ifndef MCHECKBOXFAMILY_H_INCLUDED
#define MCHECKBOXFAMILY_H_INCLUDED

#include "mWindow.h"

class mCheckboxFamily : public mWindow
{
public:

	//チェック状態
	enum CheckStatus
	{
		CHECKED,		//チェックされている
		UNCHECKED,		//チェックされていない
		INDETERMINATE,	//3ステートのグレーな状態
		UNKNOWN,		//不明（エラー）
	};

	//チェックされているかを取得する
	//ret : 現在のチェックの状態
	virtual CheckStatus GetCheck( void )const;

	//チェックを付けるor外す
	//state : 新しいステータス
	virtual bool SetCheck( CheckStatus state );

protected:
	mCheckboxFamily();
	virtual ~mCheckboxFamily();

private:

	mCheckboxFamily( const mCheckboxFamily& src ) = delete;
	mCheckboxFamily& operator=( const mCheckboxFamily& src ) = delete;

};



#endif	//MCHECKBOXFAMILY_H_INCLUDED


