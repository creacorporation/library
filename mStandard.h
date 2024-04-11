//----------------------------------------------------------------------------
// ��{�w�b�_
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MSTANDARD_H_INCLUDED
#define MSTANDARD_H_INCLUDED

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif //_DEBUG

#include <winsock2.h>
#include <windows.h>
#include <cstddef>
#include <new>
#include <sal.h>
#pragma warning( disable : 4482 )

//�X���b�h�Z�[�t�ł��邱�Ƃ������}�[�J�[
#define threadsafe /**/

//�f�o�b�O��new�Adelete�̒�`
#ifdef _DEBUG
#include <crtdbg.h>
#define mNew ::new(_NORMAL_BLOCK, __FILE__ ":" __FUNCSIG__ , __LINE__ ) 
#define mPlacementNew(a) ::new( a )
#define mDelete ::delete
#else
#define mNew ::new 
#define mDelete ::delete
#define mPlacementNew(a) ::new( a )
#endif //_DEBUG

//�O���Q�Ƃ̉���
#ifdef DEFINE_EXTERN_VALUE
#define mEXTERN /**/
#define mGLOBAL_INITIAL_VALUE(v) =(v)
#else
#define mEXTERN extern
#define mGLOBAL_INITIAL_VALUE(v)
#endif

//�z��̗v�f�������߂�
template<typename t , size_t s> constexpr size_t array_count_of( const t(&array)[ s ] )
{
	return s;
}

//���C�h�����񉻃}�N��( L"hoge"���ł��Ȃ�__FILE__���̑g�ݍ��݃}�N���p )
#define mWCHAR_STRING_INT(quote) L##quote
#define mWCHAR_STRING(quote) mWCHAR_STRING_INT(quote)

//���݃R���p�C�����̃t�@�C����
//__FILE__����p�X����菜�������ɂȂ�܂��B
#define mCURRENT_FILE ( wcsrchr( mWCHAR_STRING(__FILE__) , L'\\') ? wcsrchr( mWCHAR_STRING(__FILE__)  , L'\\' ) + 1 : mWCHAR_STRING(__FILE__) )
#define mCURRENT_FUNCTION mWCHAR_STRING(__FUNCTION__)

//�C�j�V�����C�Y
//���C�u���������������܂��B�g�p����O�ɏ��������ĉ������B
void InitializeLibrary( void );

//�f�C�j�V�����C�Y
//���C�u�����̏��������������܂��B
void DeinitializeLibrary( void );

#endif //MSTANDARD_H_INCLUDED

