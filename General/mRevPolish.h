//----------------------------------------------------------------------------
// 逆ポーランド変換クラス
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#ifndef MREVPOLISH_H_INCLUDED
#define MREVPOLISH_H_INCLUDED

#include <mStandard.h>
#include <General/mTCHAR.h>
#include <General/mErrorLogger.h>
#include <unordered_map>
#include <deque>

namespace mRevPolish
{
	enum ExprKind
	{
		OPERATOR_PLUS,		//演算子＋
		OPERATOR_MINUS,		//演算子−
		OPERATOR_MULTIPLY,	//演算子×
		OPERATOR_DIVIDE,	//演算子÷
		VALUE_INTEGER,		//整数値の値
		VALUE_FLOAT,		//浮動小数点の値
		VALUE_VARIABLE,		//変数値
	};

	struct ExprEntry
	{
		//トークンの種類
		ExprKind kind;			//トークンの種類

		//トークンの値
		//数値の場合は、数値をテキストで表現した文字列
		//変数名の場合は、変数名
		//演算子の場合は、演算子をテキストで表現した文字列
		WString variable;

		operator int()const
		{
			int val = 0;
			wchar_sscanf( variable.c_str() , L"%d" , &val );
			return val;
		}
		operator unsigned int()const
		{
			unsigned int val = 0;
			wchar_sscanf( variable.c_str() , L"%u" , &val );
			return val;
		}
		operator double()const
		{
			double val = 0.0;
			wchar_sscanf( variable.c_str() , L"%lf" , &val );
			return val;
		}
		operator float()const
		{
			float val = 0.0f;
			wchar_sscanf( variable.c_str() , L"%f" , &val );
			return val;
		}

	};

	//逆ポーランド記法への変換結果格納先
	typedef std::deque<ExprEntry> Expr;

	//中間記法の式を逆ポーランド記法に変換する
	// src : 中間記法の式
	// retPolish : 逆ポーランド記法に変換した式
	// ret : 正常に変換出来た場合true
	//       計算式に問題がある(括弧が対応していないなど)場合false
	bool Convert( const WString& src , Expr& retPolish );

}

#endif





