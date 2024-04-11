//----------------------------------------------------------------------------
// INI�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

/*
���p�r
	INI�t�@�C����1���̃Z�N�V������ێ����܂��B

���Z�N�V�����Ƃ�
	�u�Z�N�V�����v�Ƃ́A

	[BASIC]			���^�C�g��
	HOGE=1
	FUGA=c:\windows
	PIYO=0xFFFF		���L�[�ƒl�̑g

	�̂悤�ɁA[]�ł�����ꂽ�^�C�g���ƁA�L�[�ƒl�̑g����Ȃ�J�^�}���ł��B
	�܂��A[]�̑����<>�Ń^�C�g����������ƁA�s�P�ʂł̓ǂݍ��݂ɂȂ�܂�(�u���������̃L�[�ɂ��āv���Q��)

	����[]�܂���<>���o�Ă���܂ł��P�̃Z�N�V�����ɂȂ�܂��B
	����̃Z�N�V�������œ����̃L�[���o������ƁA�ォ���`�������̂��g���܂��B
	�����Z�N�V��������2�x�g���ƁA�ォ��o�Ă����ق��Œu�������܂�

���h�b�g(.)�Ŏn�܂�L�[�ɂ���
	�h�b�g�Ŏn�܂�L�[�́A�T�u�L�[���ł��B���߂̃h�b�g�̂Ȃ��L�[�ɘA�����ăL�[�����`�����܂��B
	�T�u�L�[�ɂ̓C���f�b�N�X�����邱�Ƃ͏o���܂���B

	[BASIC]
	HogeValue=10		; HogeValue=10			�L�[=HogeValue �C���f�b�N�X=�Ȃ� �T�u�L�[=�Ȃ�
	.Opt=11				; HogeValue.Opt=11		�L�[=HogeValue �C���f�b�N�X=�Ȃ� �T�u�L�[=Opt
	FugaVelue[1]=20		; FugaValue[1]=20		�L�[=FugaValue �C���f�b�N�X=1    �T�u�L�[=�Ȃ�
	.Opt=21				; FugaValue[1].Opt=21	�L�[=FugaValue �C���f�b�N�X=1    �T�u�L�[=Opt

���C���f�b�N�X�ɂ���
	�C���f�b�N�X�́A
	NUM[1]=100
	NUM[2]=100
	�̂悤�ɁA�L�[�̒���ɂ�10�i���̒l�ł��B
	�C���f�b�N�X���g���ƁA1�̃L�[�ɑ΂��ĘA�ԂŃA�N�Z�X�ł��܂��B

�����������̃L�[�ɂ���

	<BASIC>
	����������
	HOGE=����������		;�R�����g
	����������

	�̂悤�ɁA�^�C�g����<>�ł�����ƁA�s�P�ʂ̓ǂݍ��݂ɂȂ�܂��B
	�s�P�ʂ̓ǂݍ��݂̏ꍇ�A
		�E�L�[�͖�������܂�
		�E�R�����g��;�͔F������܂�
	�L�[�̓Z�N�V�������ł̍s�ԍ��ƂȂ�܂��B�ȉ��̏ꍇ�͍s�ԍ��ɃJ�E���g���܂���B
		�E�R�����g�݂̂̍s

	�L�[�u0�v���l�u�����������v
	�L�[�u1�v���l�uHOGE=�����������v
	�L�[�u2�v���l�u�����������v
	�ɂȂ�܂��B
	
	�L�[���u2�v�ɂ���ȊO�ɁA�L�[���󕶎���Ƃ��āA�C���f�b�N�X��2�Ƃ��Ă��u�����������v�ƂȂ�܂��B
	
�����̃N���X�ɂ���
	mInitFileSection�P�ɂ͂P�̃Z�N�V�������i�[����܂��B
	�L�[�Ɠǂݎ�肽���`�����w�肷��ƁA���̌`���Œl���ǂݎ���܂��B
	PIYO=0xFFFF
	���̗�ł́APIYO�𕶎���Ƃ��ēǂݎ���"0xFFFF"���ǂݎ���܂��B
	����APIYO�𐔒l�Ƃ��ēǂݎ���65535���ǂݎ���܂��B
*/


#ifndef MINITFILESECTION_H_INCLUDED
#define MINITFILESECTION_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"
#include "General/mFileWriteStream.h"
#include "General/mHexdecimal.h"
#include "GDI/mWindowPosition.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

class mInitFileSection
{
public:
	mInitFileSection();
	virtual ~mInitFileSection();
	mInitFileSection( const mInitFileSection& src );
	mInitFileSection& operator=( const mInitFileSection& src );

	//16�i���I�u�W�F�N�g
	//���̕����g��Ȃ��ꍇ�ł��A�������0x�����Ă����16�i���œǂݎ�邪�A
	//���Ă��Ȃ��ꍇ�ł������I��16�i���œǂ݂����ꍇ�͂��̃I�u�W�F�N�g���g��
	typedef mHexdecimal Hexdecimal;

	//------------------------------------------------------
	//���l�̓ǂݎ��֐��Q
	//------------------------------------------------------

	//�w�肵���L�[�����݂��邩�Ԃ�
	//key : ���݂��m�F�������L�[
	//ret : �w�肵���L�[�����݂����true
	bool IsValidKey( const WString& key )const noexcept;

	//�w�肵���L�[�����݂��邩�Ԃ�
	//key : ���݂��m�F�������L�[
	//index : ���Ԗڂ̃L�[�����؂��邩( key + INT2TEXT( index )�̃L�[���m�F���܂�)
	//ret : �w�肵���L�[�����݂����true
	bool IsValidKey( const WString& key , INT index )const noexcept;

	//�w�肵���L�[�����݂��邩�Ԃ�
	//key : ���݂��m�F�������L�[
	//index : ���Ԗڂ̃L�[�����؂��邩( key + INT2TEXT( index )�̃L�[���m�F���܂�)
	//subkey : ���݂��m�F�������T�u�L�[
	//ret : �w�肵���L�[�����݂����true
	bool IsValidKey( const WString& key , INT index , const WString& subkey )const noexcept;

	//�w�肵���L�[�����݂��邩�Ԃ�
	//key : ���݂��m�F�������L�[
	//subkey : ���݂��m�F�������T�u�L�[
	//ret : �w�肵���L�[�����݂����true
	bool IsValidKey( const WString& key , const WString& subkey )const noexcept;

	//�L�[�̒l��32�r�b�gINT�̒l�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	INT GetValue( const WString& key , INT index , const WString& subkey , INT defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��32�r�b�gLONG�̒l�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	LONG GetValue( const WString& key , INT index , const WString& subkey , LONG defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��64�r�b�gINT�̒l�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	LONGLONG GetValue( const WString& key , INT index , const WString& subkey , LONGLONG defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��32�r�b�gUINT�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	UINT GetValue( const WString& key , INT index , const WString& subkey , UINT defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��32�r�b�gDWORD�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	DWORD GetValue( const WString& key , INT index , const WString& subkey , DWORD defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��64�r�b�gUINT�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	ULONGLONG GetValue( const WString& key , INT index , const WString& subkey , ULONGLONG defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��32�r�b�gFLOAT�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	FLOAT GetValue( const WString& key , INT index , const WString& subkey , FLOAT defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��64�r�b�gDOUBLE�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	DOUBLE GetValue( const WString& key , INT index , const WString& subkey , DOUBLE defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��UNICODE������Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	WString GetValue( const WString& key , INT index , const WString& subkey , const WString& defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��ASCII������Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	AString GetValue( const WString& key , INT index , const WString& subkey , const AString& defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��UNICODE������Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	const wchar_t* GetValue( const WString& key , INT index , const WString& subkey , const wchar_t* defvalue , bool* retIsReadable = nullptr )const noexcept;

	//GetValue()��const char*�ł͂���܂���
	//�����\����UNICODE�ɂȂ��Ă���AASCII������𒼐ڕԂ����Ƃ��o���Ȃ����߂ł��B
	//const char* GetValue( const WString& key , const char* defvalue )const noexcept;

	//�L�[�̒l��F�Ƃ��ēǂݎ��
	//�E#�ɑ���16�i��6����#rrggbb�Ƃ��ēǂݎ��܂��B
	//�E#�ɑ���16�i��8����#rrggbbaa�Ƃ��ēǂݎ��܂��Baa��ǂݎ�����ꍇ�ARGBQUAD�\���̂�reserved�����o�Ɋi�[���܂��B
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	RGBQUAD GetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l���u�[���l�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	//��true�Ɣ��肳������
	//�@true�Ayes
	//��false�Ɣ��肳������
	//  false�Ano
	//��defvalue�̒l���Ԃ�ꍇ
	//  �L�[�����݂��Ȃ��ꍇ�A��Ltrue�Efalse�̏����𖞂����Ȃ��ꍇ
	bool GetValue( const WString& key , INT index , const WString& subkey , bool defvalue , bool* retIsReadable = nullptr )const noexcept;

	//�L�[�̒l��16�i���̒l�Ƃ��ēǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	Hexdecimal GetValue( const WString& key , INT index , const WString& subkey , Hexdecimal defvalue , bool* retIsReadable = nullptr )const noexcept;

	//------------------------------------------------------
	//��������˒l�̃e�[�u�����g����GetValue
	//------------------------------------------------------

	//������˒l�̃e�[�u���̃G���g��
	template< class T > struct LookupValuesEntry
	{
		WString String;	//���̕������
		T Value;		//���̒l���֘A�t����(�����񃊃e�����ɂ���ꍇ�́A�e���v���[�g��typename��const�����Y��Ȃ�)
	};

	//LookupValuesEntry��vector��typedef��������
	template< class T > using LookupValues = std::vector< LookupValuesEntry< T > >;

	//������˒l�̃e�[�u�����g���āA�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//values : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
	//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
	//�yvalues�ɓn���\���̂̍쐬��z
	//  const mInitFileSection::LookupValues< bool > BooleanValues =
	//  {
	//		{ L"yes"	, true  },	//�ݒ�l��yes����true���Ԃ�
	//		{ L"no"		, false },	//�ݒ�l��no����false���Ԃ�
	//  };
	template< class T > T GetValue( const WString& key , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		return GetValue( key , 0 , L"" , values , defvalue , retIsReadable );
	}

	//������˒l�̃e�[�u�����g���āA�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//index : ���Ԗڂ̃L�[��ǂݎ�邩( key + INT2TEXT( index )�̃L�[��ǂݎ��܂�)
	//values : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
	//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
	template< class T > T GetValue( const WString& key , INT index , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		return GetValue( key , index , L"" , values , defvalue , retIsReadable );
	}

	//������˒l�̃e�[�u�����g���āA�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//subkey : �ǂݎ�肽���T�u�L�[
	//values : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
	//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
	template< class T > T GetValue( const WString& key , const WString& subkey , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		return GetValue( key , 0 , subkey , values , defvalue , retIsReadable );
	}

	//������˒l�̃e�[�u�����g���āA�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//index : ���Ԗڂ̃L�[��ǂݎ�邩( key + INT2TEXT( index )�̃L�[��ǂݎ��܂�)
	//subkey : �ǂݎ�肽���T�u�L�[
	//values : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
	//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
	template< class T > T GetValue( const WString& key , INT index , const WString& subkey , const LookupValues< T >& values , const T defvalue , bool* retIsReadable = nullptr )const noexcept
	{
		//�ݒ�l���擾
		Key tmpkey;
		tmpkey.key = key;
		tmpkey.index = index;
		tmpkey.subkey = subkey;

		KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
		if( itr == MyKeyValueMap.end() )
		{
			//�L�[�����݂��Ȃ��̂Ńf�t�H���g��Ԃ�
			ResetReadable( retIsReadable );
			return defvalue;
		}
		//�擾�����l��S�ď������ɕϊ�
		WString str1 = itr->second;
		std::transform( str1.cbegin() , str1.cend() , str1.begin() , wchar_tolower );

		//values�̒����X�L�������A��v������̂�������Ί֘A�Â����Ă���l��Ԃ�
		typename LookupValues< T >::const_iterator values_itr = values.begin();
		for( ; values_itr != values.end() ; values_itr++ )
		{
			WString str2 = values_itr->String;
			std::transform( str2.cbegin() , str2.cend() , str2.begin() , wchar_tolower );
			if( str1 == str2 )
			{
				//��v������̂��������̂ł���B
				SetReadable( retIsReadable );
				return values_itr->Value;
			}
		}
		//��v�Ȃ��B
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//------------------------------------------------------
	//�����̑��`���̃e���v���[�g
	//------------------------------------------------------

	//==================================================
	//���ǂݎ�茋�ʂ�Ԃ��Ȃ��o�[�W������
	//==================================================

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	 //���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	inline T GetValue( const WString& key , T defvalue )const noexcept
	{
		return GetValue( key , 0 , L"" , defvalue );
	}

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//index : ���Ԗڂ̃L�[��ǂݎ�邩( key + INT2TEXT( index )�̃L�[��ǂݎ��܂�)
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	 //���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	T GetValue( const WString& key , INT index , T defvalue )const noexcept
	{
		return GetValue( key , index , L"" , defvalue );
	}

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//subkey : �ǂݎ�肽���T�u�L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	 //���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	T GetValue( const WString& key , const WString& subkey , T defvalue )const noexcept
	{
		return GetValue( key , 0 , subkey , defvalue );
	}

	//��舵���s�Ȍ^���w�肳�ꂽ�ꍇ�G���[�ɂ��邽�߂̊֐�
	template< typename T >
	inline T GetValue( const WString& key , INT index , const WString& subkey , T defvalue )const noexcept
	{
		//==================================================
		//�@�@�@�@�@�@�@���̌^�͎g�p�ł��܂���
		//==================================================
		//�Edefvalue�ɔ�Ή��̌^���w�肵�Ă��܂��񂩁H
		//  ���[�U�[��`�̌^�Ȃǂ͑Ή����Ă��܂���B
		//�Edefvalue��ASCII�������Achar*�^�̈������w�肵�Ă��܂��񂩁H
		//�@���̏ꍇ�́AAString�Ɉ�U�ϊ����Ă��������B
		//  �~�@printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , "ABC" ) ) );
		//  ���@printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , AString( "ABC" ) ).c_str() );
		static_assert( false , "This typename is not allowed (see comment)" );
	}

	//==================================================
	//���ǂݎ�茋�ʂ�Ԃ��o�[�W������
	//==================================================

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	//���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	inline T GetValue( bool* retIsReadable , const WString& key , T defvalue )const noexcept
	{
		return GetValue( key , 0 , L"" , defvalue , retIsReadable );
	}

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//index : ���Ԗڂ̃L�[��ǂݎ�邩( key + INT2TEXT( index )�̃L�[��ǂݎ��܂�)
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	//���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	T GetValue( bool* retIsReadable ,  const WString& key , INT index , T defvalue )const noexcept
	{
		return GetValue( key , index , L"" , defvalue , retIsReadable );
	}

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//subkey : �ǂݎ�肽���T�u�L�[
	//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
	//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
	//���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	T GetValue( bool* retIsReadable ,  const WString& key , const WString& subkey , T defvalue )const noexcept
	{
		return GetValue( key , 0 , subkey , defvalue , retIsReadable );
	}

	//��舵���s�Ȍ^���w�肳�ꂽ�ꍇ�G���[�ɂ��邽�߂̊֐�
	template< typename T >
	inline T GetValue( bool* retIsReadable ,  const WString& key , INT index , const WString& subkey , T defvalue )const noexcept
	{
		//==================================================
		//�@�@�@�@�@�@�@���̌^�͎g�p�ł��܂���
		//==================================================
		//�Edefvalue�ɔ�Ή��̌^���w�肵�Ă��܂��񂩁H
		//  ���[�U�[��`�̌^�Ȃǂ͑Ή����Ă��܂���B
		//�Edefvalue��ASCII�������Achar*�^�̈������w�肵�Ă��܂��񂩁H
		//�@���̏ꍇ�́AAString�Ɉ�U�ϊ����Ă��������B
		//  �~�@printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , "ABC" ) ) );
		//  ���@printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , AString( "ABC" ) ).c_str() );
		static_assert( false , "This typename is not allowed (see comment)" );
	}

	//==================================================
	//���ǂݎ�茋�ʂ�Ԃ�l�ŕԂ��o�[�W������
	//==================================================

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//iovalue : �i�[��B�ǂݎ��Ȃ������ꍇ�͕ω����Ȃ��B
	//ret : �ǂݎ�����ꍇ�^
	//���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	inline bool QueryValue( const WString& key , T iovalue )const noexcept
	{
		bool result = false;
		iovalue =  GetValue( key , 0 , L"" , iovalue , &result );
		return result;
	}

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//index : ���Ԗڂ̃L�[��ǂݎ�邩( key + INT2TEXT( index )�̃L�[��ǂݎ��܂�)
	//iovalue : �i�[��B�ǂݎ��Ȃ������ꍇ�͕ω����Ȃ��B
	//ret : �ǂݎ�����ꍇ�^
	//���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	bool QueryValue( const WString& key , INT index , T iovalue )const noexcept
	{
		bool result = false;
		iovalue =  GetValue( key , index , L"" , iovalue , &result );
		return result;
	}

	//�L�[�̒l��ǂݎ��
	//key : �ǂݎ�肽���L�[
	//subkey : �ǂݎ�肽���T�u�L�[
	//iovalue : �i�[��B�ǂݎ��Ȃ������ꍇ�͕ω����Ȃ��B
	//ret : �ǂݎ�����ꍇ�^
	//���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	bool QueryValue( const WString& key , const WString& subkey , T iovalue )const noexcept
	{
		bool result = false;
		iovalue = GetValue( key , 0 , subkey , iovalue , &result );
		return result;
	}

	//��舵���s�Ȍ^���w�肳�ꂽ�ꍇ�G���[�ɂ��邽�߂̊֐�
	template< typename T >
	inline bool QueryValue( const WString& key , INT index , const WString& subkey , T iovalue )const noexcept
	{
		//==================================================
		//�@�@�@�@�@�@�@���̌^�͎g�p�ł��܂���
		//==================================================
		//�Edefvalue�ɔ�Ή��̌^���w�肵�Ă��܂��񂩁H
		//  ���[�U�[��`�̌^�Ȃǂ͑Ή����Ă��܂���B
		//�Edefvalue��ASCII�������Achar*�^�̈������w�肵�Ă��܂��񂩁H
		//�@���̏ꍇ�́AAString�Ɉ�U�ϊ����Ă��������B
		//  �~�@printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , "ABC" ) ) );
		//  ���@printf( "%s\n" , sect->GetValue( L"key" , index , L"subkey" , AString( "ABC" ) ).c_str() );
		static_assert( false , "This typename is not allowed (see comment)" );
	}


	//------------------------------------------------------
	//���l�̃Z�b�g
	//------------------------------------------------------

	//�w��̃L�[��32�r�b�gINT�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , INT newval )noexcept;

	//�w��̃L�[��32�r�b�gLONG�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , LONG newval )noexcept;

	 //�w��̃L�[��64�r�b�gINT�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , LONGLONG newval )noexcept;

	//�w��̃L�[��32�r�b�gUINT�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , DWORD newval )noexcept;

	//�w��̃L�[��64�r�b�gUINT�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , ULONGLONG newval )noexcept;

	//�w��̃L�[��32�r�b�gFLOAT�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , FLOAT newval )noexcept;

	//�w��̃L�[��64�r�b�gDOUBLE�̒l��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , DOUBLE newval )noexcept;

	//�w��̃L�[��UNICODE�������ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , const WString& newval )noexcept;

	//�w��̃L�[��ASCII�������ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , const AString& newval )noexcept;

	//�w��̃L�[��UNICODE�������ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , const wchar_t* newval )noexcept;

	//�w��̃L�[��ASCII�������ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , const char* newval )noexcept;
	 
	 //�w��̃L�[�ɋP�x�l��ݒ肷��
	//�ERGBQUAD�̒l���A#�ɑ���16�i��8����#rrggbbaa�Ƃ��ď������݂܂��B
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& newval )noexcept;

	//�w��̃L�[��GDI���W��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 bool SetValue( const WString& key , INT index , const WString& subkey , const mWindowPosition::POSITION& newval )noexcept;

	//�w��̃L�[��Window���W��ݒ肷��
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	//�w�肵���L�[�ɑ΂��āA�ȉ��̃T�u�L�[�̒l��ݒ肵�܂��B
	//�Etop		�c��� WindowPosition::top�ɑΉ�
	//�Ebottom	�c��� WindowPosition::bottom�ɑΉ� 
	//�Eleft	�c���� WindowPosition::left�ɑΉ�
	//�Eright	�c�E�� WindowPosition::right�ɑΉ�
	 bool SetValue( const WString& key , const mWindowPosition::WindowPosition& newval )noexcept;

	//�w��̃L�[��Window���W��ݒ肷��
	//key : �ݒ肵�����L�[
	//index : ���Ԗڂ̃L�[��ݒ肷�邩( key + INT2TEXT( index )�̃L�[��ݒ肵�܂�)
	//newval : �ݒ肷��l
	//ret : �������^
	//�w�肵���L�[�ɑ΂��āA�ȉ��̃T�u�L�[�̒l��ݒ肵�܂��B
	//�Etop		�c��� WindowPosition::top�ɑΉ�
	//�Ebottom	�c��� WindowPosition::bottom�ɑΉ� 
	//�Eleft	�c���� WindowPosition::left�ɑΉ�
	//�Eright	�c�E�� WindowPosition::right�ɑΉ�
	 bool SetValue( const WString& key , INT index , const mWindowPosition::WindowPosition& newval )noexcept;

	//�w��̃L�[�Ƀu�[���l����������
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	//��true/false�ŏ������݂܂�
	 bool SetValue( const WString& key , INT index , const WString& subkey , bool newval )noexcept;

	//------------------------------------------------------
	//�����̑��`���̃e���v���[�g
	//------------------------------------------------------

	//�w��̃L�[�ɒl����������
	//key : �ݒ肵�����L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 //���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	template< typename T >
	bool SetValue( const WString& key , T newval )noexcept
	{
		return SetValue( key , 0 , L"" , newval );
	}

	//�w��̃L�[�ɒl����������
	//key : �ݒ肵�����L�[
	//index : ���Ԗڂ̃L�[��ݒ肷�邩( key + INT2TEXT( index )�̃L�[��ݒ肵�܂�)
	//newval : �ݒ肷��l
	//ret : �������^
	 //���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	 template< class T >
	 bool SetValue( const WString& key , INT index , T newval )noexcept
	 {
		return SetValue( key , index , L"" , newval );
	 }

	//�w��̃L�[�ɒl����������
	//key : �ݒ肵�����L�[
	//subkey : �ݒ肵�����T�u�L�[
	//newval : �ݒ肷��l
	//ret : �������^
	 //���ʂ̌^�œǂݏ�������ꍇ�̒��ӓ_�́Aindex�Asubkey�����̊�{�`�̊֐��ɃR�����g�������Ă���܂��B
	 template< class T >
	 inline bool SetValue( const WString& key , const WString& subkey , T newval )noexcept
	 {
		return SetValue( key , 0 , subkey , newval );
	 }

	//��舵���s�Ȍ^���w�肳�ꂽ�ꍇ�G���[�ɂ��邽�߂̊֐�
	 template< class T >
	 bool SetValue( const WString& key , INT index , const WString& subkey , T newval )noexcept
	 {
		//==================================================
		//�@�@�@�@�@�@�@���̌^�͎g�p�ł��܂���
		//==================================================
		//�Edefvalue�ɔ�Ή��̌^���w�肵�Ă��܂��񂩁H
		//  ���[�U�[��`�̌^�Ȃǂ͑Ή����Ă��܂���B
		static_assert( false , "This typename is not allowed (see comment)" );
	 }
	 
	 //------------------------------------------------------
	//�����̑�
	//------------------------------------------------------

	 //�����p�L�[
	struct Key
	{
		WString key;		//�L�[
		INT index;			//�C���f�b�N�X
		WString subkey;		//�T�u�L�[

		Key()
		{
			index = 0;
		}
		bool operator==( const Key& src )const
		{
			return ( index == src.index ) && ( key == src.key ) && ( subkey == src.subkey );
		}
		bool operator!=( const Key& src )const
		{
			return !( this->operator==( src ) ); 
		}
		bool operator<( const Key& src )const
		{
			if( key != src.key )
			{
				return key < src.key;
			}
			if( index != src.index )
			{
				return index < src.index;
			}
			return subkey < src.subkey;
		}
	};

	//�L�[�ꗗ�\
	typedef std::deque<Key> KeyList;
	typedef std::deque<INT> IndexList;

	//���݂���L�[�̈ꗗ��Ԃ�
	//�����̊֐��́A�L�[��������Ԃ��܂�
	bool GetKeyList( WStringDeque& retList )const;

	//���݂���L�[�̈ꗗ��Ԃ�
	//�����̊֐��́A�S�ẴL�[��Ԃ��܂�
	//retList : �L�[�̈ꗗ���i�[����
	//ret : �������^
	bool GetKeyList( KeyList& retList )const;

	//���݂���L�[�̈ꗗ��Ԃ�
	//�����̊֐��́A�w�肵���L�[�̃T�u�L�[��Ԃ��܂�
	//retList : �L�[�̈ꗗ���i�[����
	//ret : �������^
	bool GetKeyList( const WString& key , KeyList& retList )const;

	//�w�肵���L�[�ɑ΂���ő�E�ŏ��̃C���f�b�N�X�ԍ��𓾂܂�
	//ret : �������^�A���s���U(�w�肵���L�[�����݂��Ȃ��ꍇ�Ȃ�)
	//���C���f�b�N�X�ԍ��͔�є�т�������Ȃ����Ƃɒ��ӂ��Ă�������
	bool GetIndexRange( const WString& key , INT& ret_min_index , INT& ret_max_index )const;

	//���݂���L�[�Ɋ܂܂��C���f�b�N�X�̃��X�g��Ԃ�
	//retList : �C���f�b�N�X�̈ꗗ���i�[����
	//ret : �������^�A���s���U(�w�肵���L�[�����݂��Ȃ��ꍇ�Ȃ�)
	bool GetIndexList( const WString& key , IndexList& retList )const;

	//�t�@�C���ɃZ�N�V�����̓��e���o�͂���
	// fp : �o�͐�I�u�W�F�N�g
	// ret : �������^
	bool Write( mFileWriteStream& fp )const;

	//���݂���L�[�̐����擾����
	// ret : �i�[����Ă���L�[�̐�
	DWORD GetCount( void )const;

	//�Z�N�V�����̖��O��ݒ肷��
	bool SetSectionName( const WString& newname );

	//�Z�N�V�����̖��O���擾����
	const WString& GetSectionName( void )const;

	//�Z�N�V�����`��
	enum SectionType 
	{
		SECTIONTYPE_NORMAL,		// []�`��
		SECTIONTYPE_ROWBASE,	// <>�`�� �s�P�ʂ̓ǂݍ���
	};

	//�Z�N�V�����`����ύX����
	// newtype : �V�����`��
	// ret : �������^
	bool SetSectionType( SectionType newtype );

	//�Z�N�V�����`�����擾����
	// ret : �Z�N�V�����`��
	SectionType GetSectionType( void )const;

	//�X�V����Ă��邩�ۂ���Ԃ�
	// ret : �X�V����Ă����true
	bool IsModified( void )const;

protected:

	//�e�N���X����̒��ڏ������ݗp
	friend class mInitFile;

	//���̃Z�N�V�����̖��O
	WString MySectionName;

	//�Z�N�V�����`��
	SectionType MySectionType;

	//�L�[�̃n�b�V���擾�֐�
	struct KeyHash {
		typedef std::size_t result_type;
		inline std::size_t operator()( const Key& key ) const
		{
			std::hash<WString> hash_function;
			return hash_function( key.key );
		}
	};

	//���F���ږ��@�E�F���ڂ̐ݒ�l
	typedef std::unordered_map<Key,WString,KeyHash> KeyValueMap;
	KeyValueMap MyKeyValueMap;

	//�X�V����Ă��邩�H
	bool MyIsModified;

	inline void ResetReadable( bool* retIsReadable )const
	{
		if( retIsReadable )
		{
			*retIsReadable = false;
		}
	}
	inline void SetReadable( bool* retIsReadable )const
	{
		if( retIsReadable )
		{
			*retIsReadable = true;
		}
	}

};

//mInitFileSection����w��L�[�̒l�𓾂�B
//�|�C���^���k�����A�w��L�[���Ȃ��ꍇ�́A�f�t�H���g�̒l�𓾂�B
//ptr : mInitFileSection�ւ̃|�C���^
//key : mInitFileSection����l���擾����Ƃ��̃L�[
//defvalue : �|�C���^���k�����A�w��L�[���Ȃ��ꍇ�ɓ���l
//ret : �w�肵���L�[�̒l���Adefvalue�̒l
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , defvalue );
}

//mInitFileSection����w��L�[�̒l�𓾂�B
//�|�C���^���k�����A�w��L�[���Ȃ��ꍇ�́A�f�t�H���g�̒l�𓾂�B
//ptr : mInitFileSection�ւ̃|�C���^
//key : mInitFileSection����l���擾����Ƃ��̃L�[
//index : ���Ԗڂ̃L�[��ǂݎ�邩( key + INT2TEXT( index )�̃L�[��ǂݎ��܂�)
//defvalue : �|�C���^���k�����A�w��L�[���Ȃ��ꍇ�ɓ���l
//ret : �w�肵���L�[�̒l���Adefvalue�̒l
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , INT index , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , index , defvalue );
}

//mInitFileSection����w��L�[�̒l�𓾂�B
//�|�C���^���k�����A�w��L�[���Ȃ��ꍇ�́A�f�t�H���g�̒l�𓾂�B
//key : �ǂݎ�肽���L�[
//values : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , const mInitFileSection::LookupValues< T >& table , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , table , defvalue );
}

//mInitFileSection����w��L�[�̒l�𓾂�B
//�|�C���^���k�����A�w��L�[���Ȃ��ꍇ�́A�f�t�H���g�̒l�𓾂�B
//key : �ǂݎ�肽���L�[
//values : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
template< class T > T GetInitOption( const mInitFileSection* ptr , const WString& key , INT index , const mInitFileSection::LookupValues< T >& table , T defvalue )
{
	if( ptr == nullptr )
	{
		return defvalue;
	}
	return ptr->GetValue( key , index , table , defvalue );
}

#endif	//MINITFILESECTION_H_INCLUDED

