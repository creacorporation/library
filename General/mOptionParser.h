//----------------------------------------------------------------------------
// �R�}���h���C��������Ǘ��N���X
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MOPTIONPARSER_H_INCLUDED
#define MOPTIONPARSER_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"

/*
���p�r
�R�}���h���C���̕��������͂��āA�ǂ̃I�v�V����(/h�Ƃ�--hoge�Ƃ�)���w�肳��Ă��邩
���̃I�v�V�����ɕt�����Ă��镶���񂪂Ȃɂ��Ƃ��𓾂܂��B

���g������
mOptionParser opt;	//����ɃR�}���h���C����������擾���ɍs���܂�argc,argv��
					//�n���K�v�͂Ȃ��ł��B

if( opt.Check( L'?' ) ){...}	//�u/?�v�܂��́u-?�v�Ƃ����I�v�V���������邩�𒲂ׂ܂�

if( opt.Check( L"hoge" ) ){...}	//�u--hoge�v�Ƃ����I�v�V���������邩�𒲂ׂ܂�

�����ӓ_
�R�}���h���C��������̍ŏ��̗v�f�́A���s�t�@�C�����ɂȂ�܂����A
�p�X���S���t���Ă���Ƃ͌���܂���(WinAPI�̎d�l)�B

���F���ł���I�v�V����
	/a			�I�v�V����a���w��
	-a			�I�v�V����a���w��
	/abc		�I�v�V����a��b��c���w��
	-abc		�I�v�V����a��b��c���w��
	--abc		�I�v�V����abc���w��(abc�Ɗ��S��v)

	-a xyz		�I�v�V����a�ɑ΂��ĕ�����xyz���w��
	/a xyz		�I�v�V����a�ɑ΂��ĕ�����xyz���w��
	--abc xyz	�I�v�V����abc�ɑ΂��ĕ�����xyz���w��(abc�Ɗ��S��v)

	-abc xyz	�I�v�V����a��b��c�ɑ΂��ĕ�����xyz���w��
	/abc xyz	�I�v�V����a��b��c�ɑ΂��ĕ�����xyz���w��
				(�I�v�V����a,b,c�ǂ�ɑ΂��Ă�xyz���擾�����)
	�E�啶���Ə������͋�ʂ���
*/

class mOptionParser
{
public:

	//�R���X�g���N�^
	//��Window�A�v����argv[0]�����s�t�@�C�����ł͂Ȃ����A
	//  �����I�ɕ₢�܂��B
	mOptionParser();

	virtual ~mOptionParser() = default;

	//�w��̃I�v�V�������w�肳��Ă��邩�`�F�b�N
	//opt_char ���݂��邩�`�F�b�N����I�v�V����(1����)
	//opt_string ���݂��邩�`�F�b�N����I�v�V����(������)
	//ret : �w�肪����ꍇ�^
	bool Check( WCHAR opt_char )const;
	bool Check( const WString& opt_string )const;
	bool Check( WCHAR opt_char , const WString& opt_string )const;

	//�w��̃I�v�V�����̈ʒu(���Ԗڂ̃I�v�V�����Ƃ��đ��݂��邩)��Ԃ�
	//opt_char ���݂��邩�`�F�b�N����I�v�V����(1����)
	//opt_string ���݂��邩�`�F�b�N����I�v�V����(������)
	//ret : �������ꍇ�͂��̈ʒu(0�J�n)�B������Ȃ��ꍇ�͕��̒l�B
	//�����E������̗����w�肵�A�������������ꍇ�͕�����̈ʒu���Ԃ�
	INT GetPosition( WCHAR opt_char )const;
	INT GetPosition( const WString& opt_string )const;
	INT GetPosition( WCHAR opt_char , const WString& opt_string )const;


	//�ŏ��̃I�v�V�����̎w��ʒu�𓾂܂�
	//ret : �ŏ��ɂ���I�v�V�����̈ʒu
	//      ���������I�v�V������1���Ȃ��ꍇ�͕��̐�
	INT GetFirstOptPosition( void )const;

	//�Ō�̃I�v�V�����̎w��ʒu�𓾂܂�
	//ret : �Ō�ɂ���I�v�V�����̈ʒu
	//      ���������I�v�V������1���Ȃ��ꍇ�͕��̐�
	//�g�����q���g�F���̊֐��̌��ʂ�GetStringArray()�ɓn���ƁA�Ō�̃I�v�V�����ȍ~�S���̔z��𓾂��܂��B
	INT GetLastOptPosition( void )const;

	//�w��I�v�V�����ɕR�t���Ă��镶������ŏ���1�����擾���܂�
	//opt_char : ���݂��邩�`�F�b�N����I�v�V����(1����)
	//opt_string : ���݂��邩�`�F�b�N����I�v�V����(������)
	//def_string : �G���[�̏ꍇ�ɕԂ�����
	//ret : �������ꍇ�͂���Ɋ֘A�Â����Ă��镶����̔z��B
	//      �ȉ��̏ꍇ�Adef_string�Ɏw�肵��������B
	//			�E�I�v�V������������Ȃ��ꍇ
	//			�E�I�v�V�����͂��邪�֘A���镶���񂪂Ȃ��ꍇ
	WString GetString( WCHAR opt_char , const WString& def_string = L"" )const;
	WString GetString( const WString& opt_string , const WString& def_string = L"" )const;
	WString GetString( WCHAR opt_char , const WString& opt_string , const WString& def_string = L"" )const;

	//�w��ʒu�̕������1�擾���܂�
	//���̃��\�b�h�́A���ꂪ������ł���I�v�V�����̈ꕔ�ł��ꋭ���I�Ɏ擾���܂��B
	//index : �擾�������ʒu
	//def_string : �G���[�̏ꍇ�ɕԂ�����
	//ret : �������ꍇ�͂���Ɋ֘A�Â����Ă��镶����̔z��B
	//      �G���[�̏ꍇ�Adef_string�Ɏw�肵��������B
	WString GetStringIndex( INT index , const WString& def_string = L"" )const;


	//�w��I�v�V�����ɕR�t���Ă��镶������擾���܂�
	//opt_char : ���݂��邩�`�F�b�N����I�v�V����(1����)
	//opt_string : ���݂��邩�`�F�b�N����I�v�V����(������)
	//def_string : �G���[�̏ꍇ�ɕԂ�����
	//ret : �������ꍇ�͂���Ɋ֘A�Â����Ă��镶����̔z��B
	//      �ȉ��̏ꍇ�Adef_string�Ɏw�肵��������B
	//			�E�I�v�V������������Ȃ��ꍇ
	//			�E�I�v�V�����͂��邪�֘A���镶���񂪂Ȃ��ꍇ
	WStringVector GetStringArray( WCHAR opt_char , const WString& def_string = L"" )const;
	WStringVector GetStringArray( const WString& opt_string , const WString& def_string = L"" )const;
	WStringVector GetStringArray( WCHAR opt_char , const WString& opt_string , const WString& def_string = L"" )const;

	//�w��ʒu����A�ŏ��ɃI�v�V�����łȂ��Ƃ���܂ŕ�������擾���܂�
	//index : �J�n�ʒu
	//def_string : �G���[�̏ꍇ�ɕԂ�����
	//ret : �擾����������̔z��
	//      �ȉ��̏ꍇ�Adef_string�Ɏw�肵��������B
	//			�Eindex���z��O�̏ꍇ
	//			�E1���i�[�ł��镶���񂪖��������ꍇ
	WStringVector GetStringArrayIndex( INT index , const WString& def_string = L"" )const;


private:

	//�R�s�[�֎~
	mOptionParser( const mOptionParser& source ) = delete;
	void operator=( const mOptionParser& source ) = delete;

protected:

	//�w��ʒu�̕����񂪉��Ȃ̂����擾���܂�
	enum ArgType
	{
		ARGTYPE_NOTOPTION,	//�I�v�V�������������̂ł͂Ȃ�
		ARGTYPE_OPT_CHAR,	//1�����I�v�V����
		ARGTYPE_OPT_STRING,	//������I�v�V����
		ARGTYPE_ERROR,		//�G���[(�͈͊O�Ȃ�)
	};
	ArgType GetArgType( INT index )const;

protected:

	//�R�}���h���C��������̔z��
	WStringVector MyArgs;

};

#endif	//MOPTIONPARSER_H_INCLUDED
