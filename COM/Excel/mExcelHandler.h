//----------------------------------------------------------------------------
// Microsoft Excel�n���h��
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MEXCELHANDLER_H_INCLUDED
#define MEXCELHANDLER_H_INCLUDED

#ifdef _DEBUG
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#endif

//�ȉ��G���[�΍�̂��߁A�C���N���[�h�����Awinsock2��import��mStandard�̏��ɂ��Ă���܂��B
//�Ewinsock2.h���ɃC���N���[�h���Ă����Ȃ��ƃG���[�ɂȂ�
//�EmStandard.h��GetOffset�}�N���������ŃG���[�ɂȂ�
#include <winsock2.h>

// �G�N�Z���̃^�C�v���C�u�������C���|�[�g
#import "progid:Excel.Sheet" auto_search auto_rename rename_search_namespace("Office") \
	raw_method_prefix( "XL" ) \
	exclude( "IFont","IPicture") \
	no_dual_interfaces

//��L�̃C���|�[�g���@���ƁA�C���e���Z���X�������Ȃ��̂ŁA�����I�ɃC���N���[�h����
//���C���N���[�h�f�B���N�g���ɁA���ԃt�@�C���o�͐�(�^�C�v���C�u�����̏o�͐�)��ǉ����Ă����K�v����
#include <Excel.tlh>

// �ȉ��N���X��`
#include <mStandard.h>
#include <COM/mComObject.h>
#include <COM/mVariant.h>
#include "mExcelCellRef.h"

namespace Definitions_mExcelHandler
{
	//�����͈�
	enum SearchAreaEntry
	{
		Worksheet,	//���[�N�V�[�g
		Range		//�w��͈�
	};

	//�ǂ̕����ƈ�v�H
	enum MatchTypeEntry
	{
		Comment,	//�R�����g�ƈ�v
		Formula,	//�����ƈ�v
		Value,		//�l�ƈ�v
	};

	//��������
	enum SearchDirectionEntry
	{
		BYCOLUMN,		//��ԍ��̗񂩂珇�ɏォ�牺�Ɍ���
		BYROW			//��ԏ�̍s���珇�ɍ�����E�Ɍ���
	};
		
	//�ǂݎ�����
	enum ScanOrder
	{
		//��̍s���珇�ɁA������(������E)�ɃX�L����
		//[1] 1��2��3
		//[2] 4��5��6
		//[3] 7��8��9
		SCANORDER_ROW_MAJOR,

		//���̗񂩂珇�ɁA�c����(�ォ�牺)�ɃX�L����
		//[1] [2] [3]
		//1�� 4�� 7��
		//2�� 5�� 8��
		//3�� 6�� 9�� 
		SCANORDER_COL_MAJOR,

		//��̍s���珇�ɁA������(�E���獶)�ɃX�L����
		//���E���̃Z���̈ʒu�ɒ��ӂ��A����ȃ��[�v�����Ȃ��悤�ɂ��Ă�������
		//[1] 3��2��1
		//[2] 6��5��4
		//[3] 9��8��7
		SCANORDER_ROW_REVERSE,

		//���̗񂩂珇�ɁA�c����(�������)�ɃX�L����
		//���E���̃Z���̈ʒu�ɒ��ӂ��A����ȃ��[�v�����Ȃ��悤�ɂ��Ă�������
		//[1] [2] [3]
		//3�� 6�� 9��
		//2�� 5�� 8��
		//1�� 4�� 7�� 
		SCANORDER_COL_REVERSE,

		//���̍s���珇�ɁA������(������E)�ɃX�L����
		//���E���̃Z���̈ʒu�ɒ��ӂ��A����ȃ��[�v�����Ȃ��悤�ɂ��Ă�������
		//[3] 7��8��9
		//[2] 4��5��6
		//[1] 1��2��3
		SCANORDER_ROW_MAJOR_BOTTOMUP,

		//�E�̗񂩂珇�ɁA�c����(�ォ�牺)�ɃX�L����
		//���E���̃Z���̈ʒu�ɒ��ӂ��A����ȃ��[�v�����Ȃ��悤�ɂ��Ă�������
		//[3] [2] [1]
		//7�� 4�� 1��
		//8�� 5�� 2��
		//9�� 6�� 3�� 
		SCANORDER_COL_MAJOR_RIGHTTOLEFT,

		//���̍s���珇�ɁA������(�E���獶)�ɃX�L����
		//���E���̃Z���̈ʒu�ɒ��ӂ��A����ȃ��[�v�����Ȃ��悤�ɂ��Ă�������
		//[3] 9��8��7
		//[2] 6��5��4
		//[1] 3��2��1
		SCANORDER_ROW_REVERSE_BOTTOMUP,

		//�E�̗񂩂珇�ɁA�c����(�������)�ɃX�L����
		//���E���̃Z���̈ʒu�ɒ��ӂ��A����ȃ��[�v�����Ȃ��悤�ɂ��Ă�������
		//[3] [2] [1]
		//9�� 6�� 3��
		//8�� 5�� 2��
		//7�� 4�� 1�� 
		SCANORDER_COL_REVERSE_RIGHTTOLEFT,

	};

	//�ǂݎ��t���O
	enum ScanContinue
	{
		SCAN_FINISH_TRUE = 0,	//�ǂݎ����I�����AReadArray��true��Ԃ��܂�
		SCAN_FINISH_FALSE,		//�ǂݎ����I�����AReadArray��false��Ԃ��܂�
		SCAN_NEXTCELL,			//���̃Z����ǂݎ��܂�
		SCAN_NEXTGROUP,			//���݂̍s�܂��͗�̓ǂݎ��͏I�����A���̍s�܂��͗񂩂�ǂݎ��܂�
	};
};

class mExcelHandler : public mComObject
{
public:
	mExcelHandler();
	virtual ~mExcelHandler();

	//-------------------------------------------------------
	// �V�X�e��
	//-------------------------------------------------------

	//������
	//���̃��\�b�h���R�[������ƁA���ۂ�Excel���N�����܂�
	//true : ����������
	//false: ���������s
	bool Initialize( void );

	//�������ς݂��H
	// ret : �^�Ȃ珉�����ς�
	bool IsInitialized( void )const;

	//�������ς݂��H
	// ret : �^�Ȃ珉�����ς�
	operator bool()const;

	//�N���[���A�b�v
	//���̃��\�b�h���R�[������ƁAExcel���I�����܂�
	//true : ����
	//false: ���s
	bool UnInitialize( void );

	//���E�s���̕ύX
	//newstate true : ��
	//         false: �s���i�f�t�H���g�j
	bool SetVisible( bool newstate );

	//�A���[�g�𔭐����邩�ۂ���ݒ肷��
	//���u�t�@�C����ۑ����܂����v�̂悤�ȃ��b�Z�[�W
	//newval : true : �\������
	//         false : �\�����Ȃ�
	bool SetAlertEnable( bool newval );

	//�A���[�g�𔭐����邩�ۂ��̏�Ԃ��擾����
	//���u�t�@�C����ۑ����܂����v�̂悤�ȃ��b�Z�[�W
	//ret : true : �\������
	//      false : �\�����Ȃ�
	bool GetAlertEnable( void );

	//�����Čv�Z���[�h
	typedef Excel::XlCalculation XlsCalculation;

	//�����Čv�Z���[�h��ݒ肷��
	// newval : �V�����ݒ�l
	// ret : �������^
	bool SetCalculation( XlsCalculation newval );

	//���݂̎����Čv�Z���[�h���擾����
	// ret : ���݂̐ݒ�l
	XlsCalculation GetCalculation( void );

	//�蓮�Čv�Z���s��(�S���[�N�u�b�N)
	// ret : �������^
	bool CalcAll( void );

	//�蓮�Čv�Z���s��(���݂̃��[�N�V�[�g)
	// ret : �������^
	bool CalcWorksheet( void );

	//�C�x���g�̗L���E�����ݒ�
	//�������ɂ����Workbook_Open�v���V�[�W�������s����Ȃ��Ȃ�܂�
	//newval : true : �L���ɂ���
	//      false : �����ɂ���
	//ret : �������^
	bool SetEventEnable( bool newval );

	//�C�x���g�̗L���E������Ԃ��擾����
	//�������̏ꍇWorkbook_Open�v���V�[�W�������s����܂���
	//ret : true : �L���ł���
	//      false : �����ł���
	bool SetEventEnable( void );


	//-------------------------------------------------------
	// ���[�N�u�b�N�֌W�|�t�@�C������n
	//-------------------------------------------------------

	//���[�N�u�b�N���J���Ƃ��̃I�v�V����
	struct OpenWorkbookOption
	{
		bool IsReadOnly;	//true�ɂ���Ɠǂݎ���p�ŊJ��
		bool IsSelect;		//�J�����瑁���I������

		OpenWorkbookOption()
		{
			IsReadOnly = false;
			IsSelect = false;
		}
	};

	//�����̃��[�N�u�b�N���J��
	// filename : �J���t�@�C����
	// opt      : �t�@�C�����J���Ƃ��̃I�v�V����
	// ret : �������^
	bool OpenWorkbook( const WString& filename , const OpenWorkbookOption* opt = 0 );

	//�㏑���ۑ�
	// ret : �������^
	bool Save( void );

	//�d���������t�@�C���̃t�H�[�}�b�g
	typedef Excel::XlFileFormat XlsFileFormat;

	//���O��t���ĕۑ�
	//filename : �ۑ�����t�@�C����
	//format : �d���������t�@�C���̃t�H�[�}�b�g
	// ret : �������^
	bool SaveAs( const WString& filename , XlsFileFormat format );

	//����
	// force : true  �ύX������ꍇ���A�ύX��j�����ĕ���
	//         false �ύX������ꍇ�́A�_�C�A���O��\�����ă��[�U�Ɏw�������߂�
	// bookname : �t�@�C�������w�肵���ꍇ�A���̃t�@�C���Bnullptr���w�肵���ꍇ�A���݂̃t�@�C���B
	// ret : �������^
	bool Close( bool force , const WString* bookname = 0 );

	//���[�N�u�b�N���X�V����Ă��邩�𓾂�
	//�X�V����Ă����true
	bool IsModified( void )const;

	//-------------------------------------------------------
	// ���[�N�u�b�N�֌W
	//-------------------------------------------------------

	//���[�N�u�b�N�̐V�K�쐬
	// ret : �������^
	bool AddNewWorkbook( void );

	//�J���Ă��郏�[�N�u�b�N���������邩��Ԃ�
	// ret : �J���Ă��郏�[�N�u�b�N�̐�
	DWORD GetWorkbookCount( void );

	//���݂̃��[�N�u�b�N��ύX���܂�
	// index : �I�����郏�[�N�u�b�N�̃C���f�b�N�X
	// ret : �ύX�ɐ��������ꍇ��true�B���݂��Ȃ����[�N�u�b�N���w�肵���ꍇ�ȂǁA���s������false�B
	bool SetCurrentWorkbook( DWORD index );

	//���݂̃��[�N�u�b�N��ύX���܂�
	// name  : �I�����郏�[�N�u�b�N�̖��O
	// ret : �ύX�ɐ��������ꍇ��true�B���݂��Ȃ����[�N�u�b�N���w�肵���ꍇ�ȂǁA���s������false�B
	bool SetCurrentWorkbook( const WString& name );

	//���݂̃��[�N�u�b�N�𓾂�
	// ret : ���݂̃��[�N�u�b�N���B�w�肳��Ă��Ȃ��ꍇ�͋󕶎���
	WString GetWorkbookName( void )const;

	//���[�N�u�b�N�̖��O�𓾂�
	// index : ���O��m�肽�����[�N�u�b�N�̃C���f�b�N�X
	// retName : index�Ŏw�肵�����[�N�u�b�N�̖��O
	//  ��index�Ŏw�肵�����[�N�u�b�N���Ȃ��ꍇ�͋󕶎���
	WString GetWorkbookName( DWORD index );

	//���[�N�u�b�N�̖��O�𓾂�
	// retNames : �S�Ẵ��[�N�u�b�N�̖��O
	// ret : �������^
	bool GetWorkbookNames( WStringDeque& retNames );

	//-------------------------------------------------------
	// ���[�N�V�[�g�֌W
	//-------------------------------------------------------

	//���[�N�V�[�g�̐V�K�쐬
	// ret : �������^
	bool AddNewWorksheet( void );

	//���݂̃��[�N�u�b�N�ɃV�[�g���������邩��Ԃ�
	// ret : ���݂̃��[�N�u�b�N�ɑ��݂���V�[�g�̐�
	DWORD GetWorksheetCount( void );

	//���݂̃��[�N�V�[�g��ύX����
	// index : �I�����郏�[�N�V�[�g�̃C���f�b�N�X(1�J�n)
	// ret : �ύX�ɐ��������ꍇ��true�B���݂��Ȃ����[�N�V�[�g���w�肵���ꍇ�ȂǁA���s������false�B
	bool SetCurrentWorksheet( DWORD index );

	//���݂̃��[�N�V�[�g��ύX����
	// name  : �I�����郏�[�N�V�[�g�̖��O
	// ret : �ύX�ɐ��������ꍇ��true�B���݂��Ȃ����[�N�V�[�g���w�肵���ꍇ�ȂǁA���s������false�B
	bool SetCurrentWorksheet( const WString& name );

	//���[�N�V�[�g�̖��O�𓾂�
	// ret : ���݂̃��[�N�V�[�g���B�w�肳��Ă��Ȃ��ꍇ�͋󕶎���
	WString GetWorksheetName( void );

	//���[�N�V�[�g�̖��O�𓾂�
	// index : ���O��m�肽�����[�N�V�[�g�̃C���f�b�N�X(1�J�n)
	// ret : ���[�N�V�[�g�̖��O
	// ��index�Ŏw�肵�����[�N�V�[�g���Ȃ��ꍇ�͋󕶎���
	WString GetWorksheetName( DWORD index );

	//���݂̃��[�N�u�b�N�ɂ��郏�[�N�V�[�g�̖��O�̈ꗗ�𓾂�
	// retNames : �S�Ẵ��[�N�V�[�g�̖��O
	// ret : �������^
	bool GetWorksheetNames( WStringDeque& retNames );

	//���[�N�V�[�g�̌��݂̃T�C�Y�𓾂�
	//���[�N�V�[�g�̃f�[�^�����͂���Ă���͈͂�Ԃ�
	// ActiveSheet.Cells.SpecialCells(xlLastCell).Row
	// ActiveSheet.Cells.SpecialCells(xlLastCell).Columns
	//�œ�����l��Ԃ��܂��B
	// retRow : (ret)�s���@�l���s�v�Ȃ�nullptr��
	// retCol : (ret)�񐔁@�l���s�v�Ȃ�nullptr��
	// ret : �������^
	bool GetLastCell( DWORD* retRow , DWORD* retCol );

	//-------------------------------------------------------
	// �͈͊֌W
	//-------------------------------------------------------

	//���݂̃��[�N�V�[�g�͈̔͂��w�肷��
	//range : �w�肵�����͈�
	// ret : �������^
	bool SetCurrentRange( const mExcelCellRef& range );

	//���݂̃��[�N�V�[�g�͈̔͂��w�肷��
	//range : �w�肵�����͈�
	// ret : �������^
	bool SetCurrentRange( const WString& range );

	//���݂͈̔͂̍s���𓾂�
	//ret : ���݂͈̔͂Ɋ܂ލs��
	DWORD GetRangeRow( void );

	//���݂͈̗̔͂񐔂𓾂�
	//ret : ���݂͈̔͂Ɋ܂ޗ�
	DWORD GetRangeCol( void );

	//�����Z�����𓾂�
	// ret : �����Z���ł���ΐ^
	bool IsMerged( void )const;

	//�����Z���͈̔͂𓾂�
	// retrange : �����Z���͈̔�
	// ret : �������^
	bool GetMergeRange( mExcelCellRef& retrange );

	//-------------------------------------------------------
	// �����֌W
	//-------------------------------------------------------

	//Search�ɓn���R�[���o�b�N�֐�
	// dataptr : Search�ɓn�����l�i�����ɓǂݎ�茋�ʂ��i�[�j
	// row,col : Search�ɓn�����͈͂̍��ォ��̃I�t�Z�b�g(����̃Z����row=0,col=0)
	// ret     : �����𑱍s����Ƃ�true�A�I������Ƃ�false
	typedef bool (*fpSearchFindCallback)( void* dataptr , DWORD row , DWORD col );

	//Search�ɓn����������
	struct SearchOption
	{
		WString What;	//�������镶����

		//�����͈�
		using SearchAreaEntry = Definitions_mExcelHandler::SearchAreaEntry;
		SearchAreaEntry SearchArea;

		//�ǂ̕����ƈ�v�H
		using MatchTypeEntry = Definitions_mExcelHandler::MatchTypeEntry;
		MatchTypeEntry MatchType;

		//true = �啶���Ə���������ʂ���
		//false= �啶���Ə���������ʂ��Ȃ�
		bool MatchCase;

		//true = ���S��v
		//false= ������v
		bool MatchWhole;

		//��������
		using SearchDirectionEntry = Definitions_mExcelHandler::SearchDirectionEntry;
		SearchDirectionEntry SearchDirection;

	};

	//�w��͈͓̔�����������
	// opt      : ��������
	// callback : �������邽�тɌĂяo���R�[���o�b�N�֐�
	// dataptr  : �R�[���o�b�N�֐��ɁA�q�b�g�����Z���ƂƂ��Ɉ����n���|�C���^�i���e�͔C�Ӂj
	// retCount : �R�[���o�b�N�֐����Ăяo������(�s�v�Ȃ�nullptr��)
	// ret      : ����I����true
	bool Search( const SearchOption& opt , fpSearchFindCallback callback , void* dataptr , DWORD* retCount = nullptr );

	//���������Z���̃A�h���X�ꗗ
	typedef std::deque<mExcelCellRef::Position> PositionArray;

	//�w��͈͓̔����������A���������Z���̃A�h���X�ꗗ��Ԃ�
	// opt       : ��������
	// retPos    : ���������Z���̃A�h���X�i�[��
	// max_found : �ő傢���܂ŒT�����邩
	// ret       : �������^
	bool Search( const SearchOption& opt , PositionArray& retPos , DWORD max_found = MAXDWORD32 );

	//-------------------------------------------------------
	// �l�ǂݏ����n�i�P�i�u�����j
	//-------------------------------------------------------

	//���݂̃��[�N�V�[�g�̎w��Z���̒l�𕶎���œ���
	//�����Z����ݒ肷��ƒl����������Ȃ�
	// ret : ����ꂽ�Z���̒l
	WString GetValue( void );

	//���݂̃��[�N�V�[�g�̎w��Z���̒l��variant�^�œ���
	// retResult : �w��Z���̒l
	// ret : �������^
	bool GetValue( _variant_t& retResult );

	//���݂͈̔͂ɒl��ݒ肷��
	// newval : �ݒ肵�����l
	// ret : �������^
	bool SetValue( const _variant_t& newval );

	//-------------------------------------------------------
	// �l�ǂݏ����n�i�܂Ƃ߂Ău�����j
	//-------------------------------------------------------

	//�ǂݎ�����
	using ScanOrder = Definitions_mExcelHandler::ScanOrder;

	//�ǂݎ��t���O
	using ScanContinue = Definitions_mExcelHandler::ScanContinue;

	//ReadArray�ɓn���R�[���o�b�N�֐�
	// value   : �e�Z���̒l(�����ɓn���ꂽ�l��ǂݎ��̂����̊֐��̖��)
	// dataptr : ReadArray�ɓn�����l�i�����ɓǂݎ�茋�ʂ��i�[�j
	// row,col : ReadArray�ɓn�����͈͂̍��ォ��̃I�t�Z�b�g(����̃Z����row=0,col=0)
	// ret     : �Ԃ����l�ɉ����āA���ȍ~�̃Z���̃X�L�����𑱂��邩�����܂�܂�
	//�����̃R�[���o�b�N�֐����ŗ�O�𔭐����Ă����S�ł�(���ɒ��ӂ������Ă��܂�)
	typedef ScanContinue (*fpCellReadCallback)( const _variant_t& value , void* dataptr , DWORD row , DWORD col , DWORD max_row , DWORD max_col );

	//ReadArray�ɓn���R�[���o�b�N�֐�
	// value   : �e�Z���̒l(�����ɓn���ꂽ�l��ǂݎ��̂����̊֐��̖��)
	// dataptr : ReadArray�ɓn�����l�i�����ɓǂݎ�茋�ʂ��i�[�j
	// row,col : ReadArray�ɓn�����͈͂̍��ォ��̃I�t�Z�b�g(����̃Z����row=0,col=0)
	// ret     : �Ԃ����l�ɉ����āA���ȍ~�̃Z���̃X�L�����𑱂��邩�����܂�܂�
	//�����̃R�[���o�b�N�֐����ŗ�O�𔭐����Ă����S�ł�(���ɒ��ӂ������Ă��܂�)
	typedef ScanContinue (*fpCellReadCallbackV)( const mVariant& value , void* dataptr , DWORD row , DWORD col , DWORD max_row , DWORD max_col );

	//arr�Ɏw�肵��SAFEARRAY�̊e�Z���ɑ΂���fpCellReadCallback���Ăяo��
	// arr     : SAFEARRAY���܂�variant�^�̕ϐ�
	// reader  : arr�̊e�Z���ɑ΂��Ă��̃R�[���o�b�N�֐����Ă΂�܂�
	//           reader��1��ł�false��Ԃ��ƁA�����őł��؂��܂�
	// dataptr : reader�Ɏw�肵���֐��ɓn����܂�
	// ret     : reader���S�Z���ɑ΂���true��Ԃ����Ƃ��Atrue
	//           reader��1��ł�false��Ԃ����Ƃ��Afalse
	//��reader�ɓn���R�[���o�b�N�֐����ŗ�O�𔭐����Ă����S�ł�(���ɒ��ӂ������Ă��܂�)
	bool ReadArray( const _variant_t& arr , fpCellReadCallback reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//arr�Ɏw�肵��SAFEARRAY�̊e�Z���ɑ΂���fpCellReadCallback���Ăяo��
	// arr     : SAFEARRAY���܂�variant�^�̕ϐ�
	// reader  : arr�̊e�Z���ɑ΂��Ă��̃R�[���o�b�N�֐����Ă΂�܂�
	//           reader��1��ł�false��Ԃ��ƁA�����őł��؂��܂�
	// dataptr : reader�Ɏw�肵���֐��ɓn����܂�
	// ret     : reader���S�Z���ɑ΂���true��Ԃ����Ƃ��Atrue
	//           reader��1��ł�false��Ԃ����Ƃ��Afalse
	//��reader�ɓn���R�[���o�b�N�֐����ŗ�O�𔭐����Ă����S�ł�(���ɒ��ӂ������Ă��܂�)
	bool ReadArray( const _variant_t& arr , fpCellReadCallbackV reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//���ݑI�������͈͂̊e�Z���ɑ΂���fpCellReadCallback���Ăяo��
	// reader  : arr�̊e�Z���ɑ΂��Ă��̃R�[���o�b�N�֐����Ă΂�܂�
	//           reader��1��ł�false��Ԃ��ƁA�����őł��؂��܂�
	// dataptr : reader�Ɏw�肵���֐��ɓn����܂�
	// ret     : reader���S�Z���ɑ΂���true��Ԃ����Ƃ��Atrue
	//           reader��1��ł�false��Ԃ����Ƃ��Afalse
	//��reader�ɓn���R�[���o�b�N�֐����ŗ�O�𔭐����Ă����S�ł�(���ɒ��ӂ������Ă��܂�)
	bool ReadArray( fpCellReadCallback reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//���ݑI�������͈͂̊e�Z���ɑ΂���fpCellReadCallback���Ăяo��
	// reader  : arr�̊e�Z���ɑ΂��Ă��̃R�[���o�b�N�֐����Ă΂�܂�
	//           reader��1��ł�false��Ԃ��ƁA�����őł��؂��܂�
	// dataptr : reader�Ɏw�肵���֐��ɓn����܂�
	// ret     : reader���S�Z���ɑ΂���true��Ԃ����Ƃ��Atrue
	//           reader��1��ł�false��Ԃ����Ƃ��Afalse
	//��reader�ɓn���R�[���o�b�N�֐����ŗ�O�𔭐����Ă����S�ł�(���ɒ��ӂ������Ă��܂�)
	bool ReadArray( fpCellReadCallbackV reader , void* dataptr , ScanOrder order = ScanOrder::SCANORDER_ROW_MAJOR );

	//-------------------------------------------------------
	// ����
	//-------------------------------------------------------

	//�w��Z���̔w�i�F��RGBQUAD�l�Ŏ擾����
	// ret : �Y���Z���̔w�i�F�B�G���[�̏ꍇ��0xFFFFFFFF�ɂȂ�܂��B
	//       ��ARGB��A�̕�����0�ȊO���ǂ����Ŕ��f�ł��܂�
	RGBQUAD GetBackgroundColor( void );

private:

	mExcelHandler( const mExcelHandler& src );
	const mExcelHandler& operator=( const mExcelHandler& src );

	//�G�N�Z���ŗL�̌^�ɖ��O�t��
	typedef Excel::_ApplicationPtr XlsApplication;		//�A�v���P�[�V����
	typedef Excel::WorkbooksPtr XlsWorkbooks;			//���[�N�u�b�N
	typedef Excel::_WorkbookPtr XlsWorkbook;			//���[�N�u�b�N
	typedef Excel::SheetsPtr XlsWorksheets;				//���[�N�V�[�g
	typedef Excel::_WorksheetPtr XlsWorksheet;			//���[�N�V�[�g
	typedef Excel::RangePtr XlsRange;					//�͈�

	XlsApplication MyApplication;		//�A�v���P�[�V�����n���h��
	XlsWorkbook MyCurrentWorkbook;		//���݂̃��[�N�u�b�N
	XlsWorksheet MyCurrentWorksheet;	//���݂̃��[�N�V�[�g
	XlsRange MyCurrentRange;			//���݂͈̔�

};



#endif

