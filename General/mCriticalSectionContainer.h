//----------------------------------------------------------------------------
// クリティカルセクション管理
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
●用途
クリティカルセクションの管理をします。

・mCriticalSectionContainerクラス
　→enter/leaveでクリティカルセクションに出入りします

 ・mCriticalSectionTicketクラス
　→特別のオブジェクトを作り、そのオブジェクトが生存している間だけクリティカルセクションに入ります
*/

//●使い方(1)
#ifdef SAMPLE_CODE
void HowToUse1( void )
{
	mCriticalSectionContainer cs;	//クリティカルセクションを作る
	cs.Enter();	//↓ここからクリティカルセクション
		//なにか処理する
		//なにか処理する
		//なにか処理する
		//なにか処理する
	cs.Leave(); //↑ここまでクリティカルセクション
}
#endif

//●使い方(2)
#ifdef SAMPLE_CODE

//グローバルなクリティカルセクションオブジェクトを作る場合は、libセクションに配置する。
//理由：他のグローバルなオブジェクトが、コンストラクタでクリティカルセクションを使うと、
//　　　初期化順によっては、未初期化のクリティカルセクションオブジェクトをいじってしまう危険があるため。
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
mCriticalSectionContainer cs;	//グローバルなクリティカルセクション

void HowToUse2( void )
{
	while( 1 )	//こっちのループはクリティカルセクションではない
	{
		//何か処理する
		if( some_condition )
		{
			break;
		}
	}

	{
		mCriticalSectionTicket( cs );	//↓ここからクリティカルセクション
		for( DWORD i = 0 ; i < MAX_SIZE ; i++ )
		{
			//何か処理する
			if( some_condition )
			{
				return;	//↑ここまでクリティカルセクション
			}
		}
	}	//↑ここまでクリティカルセクション
	return;
}
#endif

#ifndef MCRITICALSECTIONCONTAINER_H_INCLUDED
#define MCRITICALSECTIONCONTAINER_H_INCLUDED

#include "mStandard.h"

//1つのクリティカルセクションを管理するクラス
class mCriticalSectionContainer
{
public:
	mCriticalSectionContainer()throw();
	virtual ~mCriticalSectionContainer()throw();

	//クリティカルセクションのスピンカウントを設定します
	//Count : スピンカウントの値
	//ret : 以前に設定されていたスピンカウントの値
	DWORD SetSpinCount( DWORD Count );

	//クリティカルセクションに入ります。
	//クリティカルセクションに入れるまで制御を返しません。
	void Enter( void );

	//クリティカルセクションから抜けます。
	void Leave( void );

	//クリティカルセクションに入れれば入ります。入れなかったらそのまま返ります λ....
	//ret : クリティカルセクションを確保できた場合真
	//      クリティカルセクションを確保できなかった場合偽(他のスレッドが持っている場合)
	bool TryEnter( void );

private:

	//コピー禁止
	mCriticalSectionContainer( const mCriticalSectionContainer& source ) = delete;
	const mCriticalSectionContainer& operator=( const mCriticalSectionContainer& source ) = delete;

protected:
	//クリティカルセクションの実物
	CRITICAL_SECTION MyCriticalSection;

};

//クリティカルセクションの範囲＝インスタンスのスコープにするためのクラス
class mCriticalSectionTicket
{
public:
	//コンストラクタ
	//コンストラクタ実行時にクリティカルセクションが確保されます。
	mCriticalSectionTicket( mCriticalSectionContainer& critical_section )throw();

	//デストラクタ
	//デストラクタ実行時にクリティカルセクションが解放されます。
	virtual ~mCriticalSectionTicket()throw();

private:

	//コピー禁止
	mCriticalSectionTicket() = delete;
	mCriticalSectionTicket( const mCriticalSectionTicket& source ) = delete;
	const mCriticalSectionTicket& operator=( const mCriticalSectionTicket& source ) = delete;

	//確保対象のクリティカルセクション
	mCriticalSectionContainer& MySection;
};

//グローバルオブジェクト
#ifndef MCRITICALSECTIONCONTAINER_CPP_COMPILING
extern mCriticalSectionContainer g_CriticalSection;
#else
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
mCriticalSectionContainer g_CriticalSection;
#endif

#endif //MCRITICALSECTIONCONTAINER_H_INCLUDED

