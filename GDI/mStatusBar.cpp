//----------------------------------------------------------------------------
// ウインドウ管理（ステータスバー）
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mStatusBar.h"
#include "General/mErrorLogger.h"
#include "General/mInitFile.h"
#include <commctrl.h>

#pragma comment (lib, "comctl32.lib")

mStatusBar::mStatusBar()
{
	ZeroMemory( MyPartsPos , sizeof( MyPartsPos ) );
	MyRightJustifyIndex = 256;
	MyRightUpper = false;
}

mStatusBar::~mStatusBar()
{
}

//ウインドウクラスの登録をする
bool mStatusBar::WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )
{
	::InitCommonControls();
	return false;	//新たなウインドウクラスの登録はしない
}

//ウインドウを開く
bool mStatusBar::CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )
{
	retSetting.ClassName = STATUSCLASSNAMEW;
	retSetting.Style |= CCS_BOTTOM;	//下部にステータスバーを配置
	retSetting.ProcedureChange = true;

	//オプションの指定がなければココで戻る
	if( opt == nullptr )
	{
		return true;
	}

	//オプションの指定があればそれに従う
	if( ((const Option*)opt)->method == Option::CreateMethod::USEOPTION )
	{
		//通常の方法での作成の場合
		const mStatusBar::Option_UseOption* op = (const mStatusBar::Option_UseOption*)opt;
		retSetting.Style |= ( op->SizeGrip ) ? ( SBARS_SIZEGRIP ) : ( 0 );	//サイズ変更用のグリップを表示？
		retSetting.Style |= ( op->Tooltip ) ? ( SBARS_TOOLTIPS ) : ( 0 );	//ツールチップを表示？
	}
	return true;
}

bool mStatusBar::OnCreate( const void* opt )
{
	//Unicodeモードにする
	::SendMessageW( GetMyHwnd() , SB_SETUNICODEFORMAT , TRUE , 0 );

	//オプションの指定がなければシンプルステータスバーに変更
	if( opt == nullptr )
	{
		::SendMessageW( GetMyHwnd() , SB_SIMPLE , TRUE , 0 );
		return true;
	}

	//オプションの指定があればそれに従う
	if( ( (const Option*)opt )->method == Option::CreateMethod::USEOPTION )
	{
		//通常の作成の場合
		const mStatusBar::Option_UseOption* op = ( const mStatusBar::Option_UseOption* )opt;

		//内部データ用にPartsOptionをコピー
		MyPartsOption = op->Parts;

		//パーツの構成を反映
		if( MyPartsOption.size() == 0 )
		{
			//パーツが無い場合、シンプルステータスバーに変更
			::SendMessageW( GetMyHwnd() , SB_SIMPLE , TRUE , 0 );
		}
		else
		{
			//ノンシンプルステータスバーに変更
			::SendMessageW( GetMyHwnd() , SB_SIMPLE , FALSE , 0 );

			//MyPartsOptionのサイズが過剰である場合はエラーを記録して、後ろを切る。
			//※エラー終了はできないから記録のみ
			if( 255 < MyPartsOption.size() )
			{
				RaiseAssert( g_ErrorLogger , MyPartsOption.size() , L"Too many parts" );
				MyPartsOption.resize( 255 );
			}

			//RightJustifyIndexの正当性をチェックする
			MyRightJustifyIndex = op->RightJustifyIndex;
			if( (INT)MyPartsOption.size() < MyRightJustifyIndex )
			{
				//不正なので全部左詰（値の補正はするが、そう有害でもないので、エラーの記録はしない）
				MyRightJustifyIndex = (INT)MyPartsOption.size();
			}

			//左寄せ、左寄せの優先
			MyRightUpper = op->RightUpper;

			//PartsOptionの要素数と同じになるようにパーツを分割する
			ModifyPartsSize();

			//パーツの内容を更新
			for( INT i = 0 ; i < (INT)MyPartsOption.size() ; i++ )
			{
				ModifyParts( i );
			}
		}
	}
	return true;
}

bool mStatusBar::ModifyPartsSize( void )
{
	//サイズが0の場合(シンプルステータスバーになっていると思われる)はエラーを返す
	if( MyPartsOption.size() == 0 )
	{
		return false;
	}

	//クライアントエリアの幅を取得
	RECT client_rect;
	::GetClientRect( GetMyHwnd() , &client_rect );

	//左寄せのパーツの位置を設定
	//左側から順番に幅を確保して、確保できなくなったら残りは全部重ねます。
	auto LeftJustifier = [this]( UINT position_max ) -> UINT
	{
		UINT position_l = 0;
		for( INT i = 0 ; i < this->MyRightJustifyIndex ; i++ )
		{
			position_l += MyPartsOption[ i ].Width;
			if( position_max < position_l )
			{
				position_l = position_max;
			}
			MyPartsPos[ i ] = position_l;
		}
		return position_l;
	};
	
	//右寄せのパーツの位置を設定
	//右側から順番に幅を確保していき、確保できなくなったら残りを全部重ねます
	auto RightJustifier = [this,client_rect]( UINT position_min )-> UINT
	{
		UINT position_r = client_rect.right - client_rect.left;
		for( INT i = (INT)MyPartsOption.size() - 1 ; MyRightJustifyIndex <= i ; i-- )
		{
			MyPartsPos[ i ] = position_r;
			if( MyPartsOption[ i ].Width < position_r )
			{
				position_r -= MyPartsOption[ i ].Width;
			}
			else
			{
				position_r = 0;
			}
			if( position_r < position_min )
			{
				position_r = position_min;
			}
		}
		return position_r;
	};

	//優先する側から値を設定する
	if( MyRightUpper )
	{
		//右側が優先の場合、右側から設定する
		UINT position_min = RightJustifier( 0 );
		LeftJustifier( position_min );
	}
	else
	{
		//左側が優先の場合、左側から設定する
		UINT position_max = client_rect.right - client_rect.left;
		position_max = LeftJustifier( position_max );
		RightJustifier( position_max );
	}

	//API呼んで、パーツを分割する
	if( !::SendMessageW( GetMyHwnd() , SB_SETPARTS , MyPartsOption.size() + 1 , (LPARAM)MyPartsPos ) )
	{
		RaiseAssert( g_ErrorLogger , MyPartsOption.size() , L"SB_SETPARTS failed" );
		return false;
	}
	return true;

}

LRESULT mStatusBar::WindowProcedure( UINT msg , WPARAM wparam , LPARAM lparam )
{
	if( msg == WM_SIZE )
	{
		ModifyPartsSize();
	}
	return __super::WindowProcedure( msg , wparam , lparam );
}

//全パーツをスキャンし、IDが一致する物に対してコールバックを呼び出す
bool mStatusBar::ScanItem( const WString& id , ScanItemCallback callback )
{
	bool result = true;
	for( INT i = 0 ; i < (INT)MyPartsOption.size() ; i++ )
	{
		if( MyPartsOption[ i ].Id == id )
		{
			result &= callback( i );
		}
	}
	return result;
}

//指定したインデックスのパーツを再設定する
bool mStatusBar::ModifyParts( INT index )
{
	//プロパティを設定値に変換
	WORD form = 0;
	form |= ( MyPartsOption[ index ].Notab ) ? ( SBT_NOTABPARSING ) : ( 0 );
	switch( MyPartsOption[ index ].Border )
	{
	case BorderType::Popout:
		form |= SBT_POPOUT;
		break;
	case BorderType::NoBorder:
		form |= SBT_NOBORDERS;
		break;
	case BorderType::Normal:
	default:
		break;
	}

	//API呼んで、パーツを分割する
	if( !::SendMessageW( GetMyHwnd() , SB_SETTEXT , ( form | LOBYTE(index) ) , (LPARAM)MyPartsOption[ index ].Str.c_str() ) )
	{
		RaiseAssert( g_ErrorLogger , index , L"SB_SETTEXT failed" );
		return false;
	}
	return true;
}

//パーツに文字列を設定する
bool mStatusBar::SetText( const WString& id , const WString& str , bool notab )
{
	//コールバック関数の定義
	auto SetStringCallback = [ this , str , notab ]( INT index ) -> bool
	{
		//プロパティの更新
		MyPartsOption[ index ].Notab = notab;
		MyPartsOption[ index ].Str = str;

		return ModifyParts( index );
	};

	//IDを検索し、文字列の更新をする
	return ScanItem( id , SetStringCallback );
}

//ボーダーラインの種類を変更する
bool mStatusBar::SetBorder( const WString& id , BorderType border )
{
	//コールバック関数の定義
	auto SetBorderCallback = [ this , border ]( INT index ) -> bool
	{
		//プロパティの更新
		MyPartsOption[ index ].Border = border;

		return ModifyParts( index );
	};

	//IDを検索し、文字列の更新をする
	return ScanItem( id , SetBorderCallback );
}

//幅を変更する
bool mStatusBar::SetWidth( const WString& id , UINT width )
{
	//コールバック関数の定義
	auto SetWidthCallback = [ this , width ]( INT index ) -> bool
	{
		//プロパティの更新
		MyPartsOption[ index ].Width = width;

		return ModifyParts( index );
	};

	//IDを検索し、文字列の更新をする
	return ScanItem( id , SetWidthCallback );
}
