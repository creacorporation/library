//----------------------------------------------------------------------------
// Microsoft Excelハンドラ
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MEXCELCELLREF_H_INCLUDED
#define MEXCELCELLREF_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mExcelCellRef
{
public:
	mExcelCellRef();
	virtual ~mExcelCellRef();
	mExcelCellRef( const mExcelCellRef& src );
	const mExcelCellRef& operator=( const mExcelCellRef& src );

	//エクセルの範囲指定情報
	struct Position
	{
		//行全体を指しているか？
		//※行・列両方を全体指定することはできません。
		//　（『ワークシート全体』を指定することはできません）
		bool WholeRow;

		//列全体を指しているか？
		//※行・列両方を全体指定することはできません。
		//　（『ワークシート全体』を指定することはできません）
		bool WholeCol;

		//行は絶対指定？  真なら絶対参照、偽なら相対参照
		// 絶対参照		$a$1	R1C1
		// 相対参照		a1		R[1]C[-1]
		bool AbsRow;

		//列は絶対指定？  真なら絶対参照、偽なら相対参照
		// 絶対参照		$a$1	R1C1
		// 相対参照		a1		R[1]C[-1]
		bool AbsCol;

		//行番号
		// A1形式の場合、R1C1形式で絶対参照の場合は1開始
		// R1C1形式で相対参照の場合は0以下もOK
		INT Row;

		//列番号
		// A1形式の場合、R1C1形式で絶対参照の場合は1開始
		// R1C1形式で相対参照の場合は0以下もOK
		INT Col;

		//デフォルトで初期化
		//※どのセルも指していない状態になります
		Position()
		{
			Clear();
		}

		//初期化
		//whole_row	行全体？
		//whole_col	列全体？
		//abs_row	行は絶対指定？  真なら絶対参照($a$1)、偽なら相対参照(a1)
		//abs_col	列は絶対指定？  真なら絶対参照($a$1)、偽なら相対参照(a1)
		//row		行番号(1開始)
		//col		列番号(1開始)
		Position( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , INT col )
		{
			Set( whole_row , whole_col , abs_row , abs_col , row , col );
		}

		//初期化
		//whole_row	行全体？
		//whole_col	列全体？
		//abs_row	行は絶対指定？  真なら絶対参照($a$1)、偽なら相対参照(a1)
		//abs_col	列は絶対指定？  真なら絶対参照($a$1)、偽なら相対参照(a1)
		//row		行番号(1開始)
		//col		列番号(A1形式のアルファベット)
		Position( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , const WString& col );

		//有効なセルを指している状態か
		inline bool IsValid( void )const
		{
			return !WholeRow || !WholeCol;
		}

		//位置情報をクリアします
		//※どのセルも指していない状態になります
		inline void Clear( void )
		{
			WholeRow = true;
			WholeCol = true;
			Row = 0;
			Col = 0;
			AbsRow = false;
			AbsCol = false;
		}

		//範囲を指定します
		//whole_row	行全体？
		//whole_col	列全体？
		//abs_row	行は絶対指定？  真なら絶対参照($a$1)、偽なら相対参照(a1)
		//abs_col	列は絶対指定？  真なら絶対参照($a$1)、偽なら相対参照(a1)
		//row		行番号(1開始)
		//col		列番号(1開始)
		inline void Set( bool whole_row , bool whole_col , bool abs_row , bool abs_col , INT row , INT col )
		{
			WholeRow = whole_row;
			WholeCol = whole_col;
			Row = row;
			Col = col;
			AbsRow = abs_row;
			AbsCol = abs_col;
		}
	};

	//指定の1つのセル、行全体、列全体を指すようにオブジェクトを初期化します
	// src : 指定したいセル
	mExcelCellRef( const Position& src );

	//指定の2つのセル、行全体、列全体により範囲が指定されるようにオブジェクトを初期化します
	// src1 : 指定したい範囲の対角線をなす一方のセル
	// src2 : 指定したい範囲の対角線をなすもう一方のセル
	mExcelCellRef( const Position& src1 , const Position& src2 );

	//指定の1つのセル、行全体、列全体を指すようにオブジェクトをセットします
	// src : 指定したいセル
	// ret : 成功時真
	bool SetPosition( const Position& pos );

	//指定の2つのセル、行全体、列全体により範囲が指定されるようにオブジェクトをセットします
	// src1 : 指定したい範囲の対角線をなす一方のセル
	// src2 : 指定したい範囲の対角線をなすもう一方のセル
	// ret : 成功時真
	bool SetPosition( const Position& pos1 , const Position& pos2 );

	//保持している位置を返す
	// retPos1 : 範囲の一方のセル(不要ならnullptr可)
	// retPos2 : 範囲のもう一方のセル(不要ならnullptr可)
	// ret : 成功時真
	bool GetPosition( Position* retPos1 , Position* retPos2 );

	//保持している位置を返す
	// ret : 成功時真
	const Position& GetPos1( void )const;

	//保持している位置を返す
	// ret : 成功時真
	const Position& GetPos2( void )const;

	//保持している位置、範囲を文字列で返す
	// IsRC : true : R1C1形式
	//        false : A1形式
	// ret : 成功時真
	const WString& GetAddress( bool IsRC )const;

	//範囲の行数を返す
	//pos : 基準位置
	//	R1C1形式で、絶対参照と相対参照が混在する場合、相対参照が基準とする位置によって結果が変わります
	//	このような場合は、基準とする位置を指定して下さい。それ以外の場合は結果が変わらないので指定不要です。
	//	nullptrを指定した場合は、pos1(指定範囲の左上)が基準になります。
	//ret : 範囲に含まれる行数。エラーのとき０。
	DWORD GetRowCount( const Position* pos = nullptr )const;

	//範囲の列数を返す
	//pos : 基準位置
	//	R1C1形式で、絶対参照と相対参照が混在する場合、相対参照が基準とする位置によって結果が変わります
	//	このような場合は、基準とする位置を指定して下さい。それ以外の場合は結果が変わらないので指定不要です。
	//	nullptrを指定した場合は、pos1(指定範囲の左上)が基準になります。
	//ret : 範囲に含まれる行数。エラーのとき０。
	DWORD GetColCount( const Position* pos = nullptr )const;

	//何も指していない状態にする
	void Clear( void );

	//有効なセルを指しているか？
	inline bool IsValid( void )const
	{
		return MyPos1.IsValid();
	}

protected:

	Position MyPos1;
	Position MyPos2;

	mutable WString MyAddress;

protected:

	bool UpdateAddress( bool IsRC )const;

	//エクセルのワークシート上の位置からセル名称を生成します
	//例：列3、行2ならR3C2とかC2とかを返します
	// retCell : 生成したセルの名前
	// IsRC    : 真ならR1C1形式、偽なら絶対参照
	// IsAbs   : 真なら絶対参照($a$1)、偽なら相対参照(a1)
	// row     : 行番号(1から開始 エクセルのrow()と同じ値)
	// col     : 列番号(1から開始 エクセルのcol()と同じ値)
	// norow   : 行は指定しない（この場合rowは無視されます）列全体の指定用
	// nocol   : 列は指定しない（この場合colは無視されます）行全体の指定用
	// ret     : 生成できたら真、失敗したら偽
	//           ※A1形式でrowに-1とか指定したりすると失敗します
	static bool BuildCellName( WString& retCell , bool IsRC , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow = false , bool nocol = false );
	//セル名称を生成する（下請け）
	static bool BuildCellNameA1( WString& retCell , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow , bool nocol );	//A1形式用
	static bool BuildCellNameRC( WString& retCell , bool IsAbsRow , bool IsAbsCol , INT row , INT col , bool norow , bool nocol );	//R1C1形式用

};



#endif

