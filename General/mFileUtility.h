//----------------------------------------------------------------------------
// �t�@�C�����[�e�B���e�B�֐��Q
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------


#ifndef MFILEUTILITY_H_INCLUDED
#define MFILEUTILITY_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"

namespace mFileUtility
{

	//�w�肵���t�@�C�������t�@�C���V�X�e���㋖����邩�𔻒肷��
	// filename : ���肵�����t�@�C����
	// ret : ������閼�O�̏ꍇtrue�A������Ȃ����O�̏ꍇfalse
	bool IsSuitableFilename( const WString& filename );

	//�w�肵���t�@�C�������t�@�C���V�X�e���㋖����邩�𔻒肷��
	// filename : ���肵�����t�@�C����
	// ret : ������閼�O�̏ꍇtrue�A������Ȃ����O�̏ꍇfalse
	bool IsSuitableFilename( const wchar_t* filename );

	//�p�X���h���C�u�A�f�B���N�g���A�t�@�C�����A�g���q�ɕ�������
	// path : �����O�̃p�X
	// retDrive : �y�o�́z�h���C�u���@�s�v�̏ꍇ��nullptr�ł���
	// retDir : �y�o�́z�f�B���N�g�����@�s�v�̏ꍇ��nullptr�ł���
	// retFile : �y�o�́z�t�@�C�����@�s�v�̏ꍇ��nullptr�ł���
	// retExt : �y�o�́z�g���q�@�s�v�̏ꍇ��nullptr�ł���
	// ret : �����̏ꍇ�^
	bool SplitPath( const WString& path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt );

	//�p�X���h���C�u�A�f�B���N�g���A�t�@�C�����A�g���q�ɕ�������
	// path : �����O�̃p�X
	// retDrive : �y�o�́z�h���C�u���@�s�v�̏ꍇ��nullptr�ł���
	// retDir : �y�o�́z�f�B���N�g�����@�s�v�̏ꍇ��nullptr�ł���
	// retFile : �y�o�́z�t�@�C�����@�s�v�̏ꍇ��nullptr�ł���
	// retExt : �y�o�́z�g���q�@�s�v�̏ꍇ��nullptr�ł���
	// ret : �����̏ꍇ�^
	bool SplitPath( const wchar_t* path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt );

	//�t�@�C���p�X�̔��o
	//�t�@�C���p�X�̎w�肵���v�f�݂̂��g�p���ĐV�����p�X�����܂�
	// path : ���̃p�X
	// retResult : �y�o�́z�č\�z�����p�X
	// drive : ���̃p�X�Ɋ܂܂�Ă���h���C�u�����g���Ȃ�true
	// dir : ���̃p�X�Ɋ܂܂�Ă���f�B���N�g�������g���Ȃ�true
	// filename : ���̃p�X�Ɋ܂܂�Ă���t�@�C�������g���Ȃ�true
	// ext : ���̃p�X�Ɋ܂܂�Ă���g���q���g���Ȃ�true
	bool RebuildPath( const WString& path , WString& retResult , bool drive , bool dir , bool filename , bool ext );

	//�t�@�C���p�X�̔��o
	//�t�@�C���p�X�̎w�肵���v�f�݂̂��g�p���ĐV�����p�X�����܂�
	// path : ���̃p�X
	// retResult : �y�o�́z�č\�z�����p�X
	// drive : ���̃p�X�Ɋ܂܂�Ă���h���C�u�����g���Ȃ�true
	// dir : ���̃p�X�Ɋ܂܂�Ă���f�B���N�g�������g���Ȃ�true
	// filename : ���̃p�X�Ɋ܂܂�Ă���t�@�C�������g���Ȃ�true
	// ext : ���̃p�X�Ɋ܂܂�Ă���g���q���g���Ȃ�true
	bool RebuildPath( const wchar_t* path , WString& retResult , bool drive , bool dir , bool filename , bool ext );

	//�p�X���h���C�u�A�f�B���N�g���A�t�@�C�����A�g���q�������ւ���
	// path : �����ւ��O�̃p�X
	// retpath : �����ւ��O�̃p�X�i�o�́j
	// retDrive : �����ւ���h���C�u���@�����ւ��s�v�̏ꍇ��nullptr
	// retDir : �����ւ���f�B���N�g�����@�����ւ��s�v�̏ꍇ��nullptr
	// retFile : �����ւ���t�@�C�����@�����ւ��s�v�̏ꍇ��nullptr
	// retExt : �����ւ���g���q�@�����ւ��s�v�̏ꍇ��nullptr
	// ret : �����̏ꍇ�^
	bool ReplacePath( const WString& path , WString& retPath , const WString* Drive , const WString* Dir , const WString* File , const WString* Ext );

	//�p�X���h���C�u�A�f�B���N�g���A�t�@�C�����A�g���q�������ւ���
	// path : �����ւ��O�̃p�X
	// retpath : �����ւ��O�̃p�X�i�o�́j
	// retDrive : �����ւ���h���C�u���@�����ւ��s�v�̏ꍇ��nullptr
	// retDir : �����ւ���f�B���N�g�����@�����ւ��s�v�̏ꍇ��nullptr
	// retFile : �����ւ���t�@�C�����@�����ւ��s�v�̏ꍇ��nullptr
	// retExt : �����ւ���g���q�@�����ւ��s�v�̏ꍇ��nullptr
	// ret : �����̏ꍇ�^
	bool ReplacePath( const WString& path , WString& retPath , const wchar_t* Drive , const wchar_t* Dir , const wchar_t* File , const wchar_t* Ext );

	//�p�X��A������
	//�f�B���N�g�����A�t�@�C�����A�g���q��A�����ĕԂ��܂��B�f�B���N�g�����̖����̃o�b�N�X���b�V�����Ȃ��ꍇ��
	//�����I�ɕ₢�܂��B
	// dir : �f�B���N�g����
	// file : �t�@�C����
	WString CombinePath( const WString& dir , const WString& file = L"" , const WString& ext = L"" );

	//���΃p�X���΃p�X�ɕϊ����܂�
	// filename : ���΃p�X
	// ret : �ϊ�������΃p�X(�G���[�̏ꍇ�A�󕶎���)
	WString GetFullPath( const WString& filename )noexcept;

	//���΃p�X���΃p�X�ɕϊ����܂�
	// filename : ���΃p�X
	// ret : �ϊ�������΃p�X(�G���[�̏ꍇ��O�𓊂���)
	template< class ExceptionIfFailed >
	inline WString GetFullPath( const WString& filename )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result = GetFullPath( filename );
		if( result == L"" )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}

	//���΃p�X���΃p�X�ɕϊ����܂�
	// filename : ���΃p�X
	// ret : �ϊ�������΃p�X(�G���[�̏ꍇ�A�󕶎���)
	WString GetFullPath( const wchar_t* filename )noexcept;

	//���΃p�X���΃p�X�ɕϊ����܂�
	// filename : ���΃p�X
	// ret : �ϊ�������΃p�X(�G���[�̏ꍇ��O�𓊂���)
	template< class ExceptionIfFailed >
	inline WString GetFullPath( const wchar_t* filename )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result = GetFullPath( filename );
		if( result == L"" )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}

	//�p�X���s���̏ꍇ�̃G���[�R�[�h
	static const DWORD ERR_BADPATH = 0x20000000;

	//�w�肵���p�X���A����p�X�ȉ��ł��邩�𔻒肵�܂�
	// path : ��������p�X
	// rootpath : ������ɂȂ�p�X
	// ret : true  = path��rootpath�ȉ��ł���(path��rootpath�̃T�u�f�B���N�g���ł���)
	//       false = path��rootpath�ȉ��ł͂Ȃ�(path��rootpath�̃T�u�f�B���N�g���ł͂Ȃ�)
	//               ��GetLastError()��ERROR_SUCCESS
	//       false = �����ꂩ�̃p�X���s��
	//               ��GetLastError()��ERR_BADPATH
	bool CheckPathTraversal( const WString& path , const WString& rootpath )noexcept;

	//�w�肵���p�X���A����p�X�ȉ��ł��邩�𔻒肵�܂�
	// path : ��������p�X
	// rootpath : ������ɂȂ�p�X
	// ret : true  = path��rootpath�ȉ��ł���(path��rootpath�̃T�u�f�B���N�g���ł���)
	//       false = path��rootpath�ȉ��ł͂Ȃ�(path��rootpath�̃T�u�f�B���N�g���ł͂Ȃ�)
	// �G���[�������͗�O
	template< class ExceptionIfFailed >
	inline WString CheckPathTraversal( const WString& path , const WString& rootpath )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		if( !CheckPathTraversal( path , rootpath ) )
		{
			if( GetLastError() == ERR_BADPATH )
			{
				throw ExceptionIfFailed();
			}
			return false;
		}
		return true;
	}

	//�w�肵���p�X���A����p�X�ȉ��ł��邩�𔻒肵�܂�
	// path : ��������p�X
	// rootpath : ������ɂȂ�p�X
	// ret : true  = path��rootpath�ȉ��ł���(path��rootpath�̃T�u�f�B���N�g���ł���)
	//       false = path��rootpath�ȉ��ł͂Ȃ�(path��rootpath�̃T�u�f�B���N�g���ł͂Ȃ�)
	bool CheckPathTraversal( const wchar_t* path , const wchar_t* rootpath ) noexcept;

	//�w�肵���p�X���A����p�X�ȉ��ł��邩�𔻒肵�܂�
	// path : ��������p�X
	// rootpath : ������ɂȂ�p�X
	// ret : true  = path��rootpath�ȉ��ł���(path��rootpath�̃T�u�f�B���N�g���ł���)
	//       false = path��rootpath�ȉ��ł͂Ȃ�(path��rootpath�̃T�u�f�B���N�g���ł͂Ȃ�)
	template< class ExceptionIfFailed >
	inline WString CheckPathTraversal( const wchar_t* path , const wchar_t* rootpath )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		if( !CheckPathTraversal( path , rootpath ) )
		{
			if( GetLastError() == ERR_BADPATH )
			{
				throw ExceptionIfFailed();
			}
			return false;
		}
		return true;
	}

	//�e���|�����t�H���_�̃p�X�𓾂�
	// ret : �e���|�����t�H���_�̃p�X
	WString GetTempDirPath( void );

	//�e���|�����t�@�C���̃p�X�𓾂�
	// folder : �e���|�����t�H���_�̃p�X(�󔒂ɂ���ƃV�X�e������擾)
	// prefix : �e���|�����t�@�C�����̐擪�ɂ���v���t�B�N�X
	// ret : �e���|�����t�@�C���̃p�X
	WString GetTempFilePath( const WString& folder , const WString& prefix );

	//�p�X��"from"����p�X��"to"�ւ̑��΃p�X��Ԃ��܂�
	// from : ���΃p�X�̌�
	// to : ���΃p�X�̐�
	// ret : from����to�ւ̑��΃p�X�B���΃p�X���v�Z�s�\�̏ꍇ�󕶎���B
	WString GetRelativePath( const WString& from , const WString& to ) noexcept;

	//�p�X��"from"����p�X��"to"�ւ̑��΃p�X��Ԃ��܂�
	// from : ���΃p�X�̌�
	// to : ���΃p�X�̐�
	// ret : from����to�ւ̑��΃p�X�B���΃p�X���v�Z�s�\�̏ꍇ��O
	template< class ExceptionIfFailed >
	inline WString GetRelativePath( const WString& from , const WString& to )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result = GetRelativePath( from ,  to );
		if( result == L"" )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}

	//�w�肵���f�B���N�g�����i���ԃf�B���N�g�����܂߂āj�쐬���܂�
	// path : �쐬����f�B���N�g��
	// ret : �������^
	bool CreateMiddleDirectory( const WString& path )noexcept;

};

#endif
