//----------------------------------------------------------------------------
// 逆ポーランド変換クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------


#define MREVPOLISH_CPP_COMPILING
#include "mRevPolish.h"

using namespace mRevPolish;

//次のトークンを取り出す
// src : 式の文字列
// index : トークンの先頭位置
// retExpr : 読み取ったトークン
// retNext : 次のトークンの先頭位置
// ret : トークンを読み取った場合true
//       もうトークンがない場合false
static bool NextToken( const WString& src , size_t index , WString& retExpr , size_t& retNext )
{
	WCHAR c = L' ';

	//空白を読み飛ばす
	while( index < src.size() )
	{
		c = src[ index ];
		if( !wchar_isblank( c ) )
		{
			break;
		}
		index++;
	}

	//結果をリセット
	retExpr = L"";

	//最初の文字が何であるかによりトークンを判別
	if( wchar_isblank( c ) )
	{
		//空白→もうトークンはない
		return false;
	}
	else if( wchar_isdigit( c ) )
	{
		//数字→数値
		bool is_hex = false;		//16進数？
		bool is_float = false;		//浮動小数点？

		for( ; index < src.size() ; index++ )
		{
			c = src[ index ];
			if( !wchar_isdigit( c ) )
			{
				if( is_float )
				{
					break;
				}
				else if( is_hex && !wchar_isxdigit( c ) )
				{
					break;
				}
				else if( c == L'x' && retExpr == L"0" )
				{
					is_hex = true;
				}
				else if( c == L'.' )
				{
					is_float = true;
				}
				else
				{
					break;
				}
			}
			retExpr += c;
		}
		retNext = index;

		if( src.back() == L'.' || src.back() == L'x' )
		{
			//小数点や0xで終わっている場合(エラー)
			retExpr = L"";
			return true;
		}
		return true;
	}
	else if( wchar_isalpha( c ) || c == L'@' || c == L'$' )
	{
		//アルファベットと＠→変数名
		for( ; index < src.size() ; index++ )
		{
			c = src[ index ];
			if( !wchar_isalnum( c ) )
			{
				break;
			}
			retExpr += c;
		}
		retNext = index;
		return true;
	}
	else
	{
		//その他→演算子？
		switch( c )
		{
		case L'+': case L'-': case L'*': case L'/':
		case L'(': case L')':
			retExpr = c;
			retNext = index + 1;
			return true;

		default:
			retExpr = L"";
			retNext = index + 1;
			return true;
		}
	}
}

//演算子の優先順位を返す
// oper : 優先順位を確認したいオペレーター
// ret : 優先順位（優先順位が高い物ほど数字が大きい）
//       エラーの時0
static INT QueryOperatorPriority( ExprKind oper )
{
	switch( oper )
	{
	case ExprKind::OPERATOR_PLUS:		//演算子＋
		return 2;
	case ExprKind::OPERATOR_MINUS:		//演算子−
		return 1;
	case ExprKind::OPERATOR_MULTIPLY:	//演算子×
		return 4;
	case ExprKind::OPERATOR_DIVIDE:	//演算子÷
		return 3;
	default:
		break;
	}
	return 0;
}

bool mRevPolish::Convert( const WString& src , Expr& retPolish )
{
	//すでに格納されている結果があれば破棄
	retPolish.clear();

	//テンポラリ変数たち
	size_t index = 0;		//現在のトークンの開始位置
	size_t next_index = 0;	//次のトークンの開始位置
	WString NextExpr;		//次のトークン
	Expr Stack;				//逆ポーランド記法変換用のスタック
	WString ErrorStr;		//エラー発生時の文面
	bool is_unary = true;	//単項演算子出現のタイミングか？

	//閉じ括弧のトークンが来た場合の処理
	auto PopStack = [&retPolish,&Stack]() -> bool 
	{
		while( Stack.size() )
		{
			ExprEntry entry = Stack.back();
			Stack.pop_back();
			if( entry.variable == L"(" )
			{
				return true;
			}
			retPolish.push_back( entry );
		}
		//括弧が対応していない（エラー）
		return false;
	};

	//演算子の評価をする処理
	auto ExprPriority = [&retPolish,&Stack]( const WString& Next ) -> bool
	{
		//今回のトークンに対するエントリと優先順位を生成する
		ExprEntry current_entry;
		current_entry.variable = Next;
		if( Next== L"+" )
		{
			current_entry.kind = ExprKind::OPERATOR_PLUS;
		}
		else if( Next == L"-" )
		{
			current_entry.kind = ExprKind::OPERATOR_MINUS;
		}
		else if( Next == L"*" )
		{
			current_entry.kind = ExprKind::OPERATOR_MULTIPLY;
		}
		else if( Next == L"/" )
		{
			current_entry.kind = ExprKind::OPERATOR_DIVIDE;
		}
		else
		{
			return false;
		}
		INT current_priority = QueryOperatorPriority( current_entry.kind );

		//スタックを確認して、優先順位の低いものを吐き出す
		while( !Stack.empty() )
		{
			if( current_priority < QueryOperatorPriority( Stack.back().kind ) )
			{
				retPolish.push_back( Stack.back() );
				Stack.pop_back();
			}
			else
			{
				break;
			}
		}

		//今回のトークンをスタックに積む
		Stack.push_back( current_entry );
		return true;
	};

	//メインルーチン
	while( NextToken( src , index , NextExpr , next_index ) )
	{
		if( NextExpr == L"" )
		{
			//読み取れないトークン
			sprintf( ErrorStr , L"式'%ls'に不明なトークンがあります" , src.c_str() );
			goto onerror;
		}
		else if( wchar_isdigit( NextExpr[ 0 ] ) )
		{
			//数値
			ExprEntry entry;
			entry.variable = NextExpr;
			if( NextExpr.find( L'.' ) == WString::npos )
			{
				//小数点がないので整数
				entry.kind = ExprKind::VALUE_INTEGER;
			}
			else
			{
				//小数点があるので浮動小数
				entry.kind = ExprKind::VALUE_FLOAT;
			}
			retPolish.push_back( entry );

			is_unary = false;
		}
		else if( wchar_isalpha( NextExpr[ 0 ] ) )
		{
			//変数名
			ExprEntry entry;
			entry.variable = NextExpr;
			entry.kind = ExprKind::VALUE_VARIABLE;
			retPolish.push_back( entry );

			is_unary = false;
		}
		else if( NextExpr[ 0 ] == L')' )
		{
			//閉じ括弧
			if( is_unary )
			{
				sprintf( ErrorStr , L"式'%ls'において、括弧')'の前に二項演算子が不足しています" , src.c_str() );
				goto onerror;
			}
			if( !PopStack() )
			{
				sprintf( ErrorStr , L"式'%ls'において括弧'('の数が足りません" , src.c_str() );
				goto onerror;
			}

			is_unary = false;
		}
		else if( NextExpr[ 0 ] == L'(' )
		{
			//開き括弧
			if( !is_unary )
			{
				sprintf( ErrorStr , L"式'%ls'において、括弧'('の前に二項演算子が不足しています" , src.c_str() );
				goto onerror;
			}
			ExprEntry entry;
			entry.kind = ExprKind::VALUE_VARIABLE;
			entry.variable = L'(';
			Stack.push_back( entry );

			is_unary = true;
		}
		else
		{
			//その他演算子
			if( is_unary )
			{
				//単項演算子の場合
				if( NextExpr== L"+" )
				{
					//＋符号は無視する
					;
				}
				else if( NextExpr == L"-" )
				{
					//−符号は式に−１を乗じる
					ExprEntry entry;
					entry.kind = ExprKind::VALUE_INTEGER;
					entry.variable = L"-1";
					retPolish.push_back( entry );

					entry.kind = ExprKind::OPERATOR_MULTIPLY;
					entry.variable = L"*";
					Stack.push_back( entry );
				}
				else
				{
					//それ以外は未対応
					sprintf( ErrorStr , L"式'%ls'において不正な単項演算子があります" , src.c_str() );
					goto onerror;
				}
			}
			else
			{
				//二項演算子の場合
				if( !ExprPriority( NextExpr ) )
				{
					sprintf( ErrorStr , L"式'%ls'において演算子の処理が失敗しました" , src.c_str() );
					goto onerror;
				}
			}
			is_unary = true;
		}
		index = next_index;
	}

	//全部のトークンをスタックから移動する
	while( !Stack.empty() )
	{
		ExprEntry entry = Stack.back();
		Stack.pop_back();
		if( entry.variable == L"(" )
		{
			sprintf( ErrorStr , L"式'%ls'において括弧')'の数が足りません" , src.c_str() );
			goto onerror;
		}
		retPolish.push_back( entry );
	}
	return true;

onerror:
	RaiseError( g_ErrorLogger , 0 , ErrorStr.c_str() , index );
	return false;
}
