//----------------------------------------------------------------------------
// ���K�\���N���X(std::regexp�̃��b�p�[)
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MREGEXP_H_INCLUDED
#define MREGEXP_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "General/mErrorLogger.h"
#include <regex>

template< class T > class mRegexp
{
public:

	//���K�\�����s���̃I�v�V����
	struct Option
	{
		//true�ɂ���ƁA�啶���E����������ʂ��Ȃ�
		//(���̂Ƃ���Atrue�ɂ���Ɛ��������삵�Ȃ��Ȃ�Bstd::regex���o�O���Ă�C������B)
		bool IgnoreCase;

		//true�ɂ���ƁA�}�b�`����������������i�[���Ȃ�
		bool NoSubstr;

		Option( void )
		{
			IgnoreCase = false;
			NoSubstr = false;
		}
		Option( bool ignore_case , bool nosubstr )
		{
			IgnoreCase = ignore_case;
			NoSubstr = nosubstr;
		}
	};

	//���K�\���I�u�W�F�N�g���\�z����
	//opt : ���K�\���}�b�`�������̃I�v�V����
	//      �ȗ���nullptr�ł��B���̏ꍇ�́A�f�t�H���g�̐ݒ�(Option�̃R���X�g���N�^�ɔC�����Ƃ��Ɠ���)�ɂȂ�܂��B
	//���K�\���̃p�^�[�����܂�ł��Ȃ����߁A���̂܂܂ł͉��Ƃ��}�b�`���܂���B
	//�}�b�`������O�ɁASetPattern()���R�[�����Ă��������B
	mRegexp( const Option* opt = nullptr ) noexcept
	{
		std::basic_string<T> pattern;
		mRegexp( pattern , opt );
	}

	//���K�\���I�u�W�F�N�g���\�z����
	//pattern : ���K�\���̃p�^�[��
	//opt : ���K�\���}�b�`�������̃I�v�V����
	//      �ȗ���nullptr�ł��B���̏ꍇ�́A�f�t�H���g�̐ݒ�(Option�̃R���X�g���N�^�ɔC�����Ƃ��Ɠ���)�ɂȂ�܂��B
	//���K�\�����Ԉ���ĂĂ��G���[�ɂȂ�܂���(g_ErrorLogger�ɋL�^�͂����)
	//���K�\�����G���[�ɂȂ肤��ꍇ��SetPattern()���g���Ă��������B
	mRegexp( std::basic_string<T> pattern , const Option* opt = nullptr ) noexcept
	{
		if( opt != nullptr )
		{
			CreateRegexpObject( pattern , opt );
		}
		else
		{
			Option dummy_opt;
			CreateRegexpObject( pattern , &dummy_opt );
		}
	}

	//���K�\���I�u�W�F�N�g���\�z����
	//pattern : ���K�\���̃p�^�[��
	//opt : ���K�\���}�b�`�������̃I�v�V����
	//���K�\�����Ԉ���ĂĂ��G���[�ɂȂ�܂���(g_ErrorLogger�ɋL�^�͂����)
	//���K�\�����G���[�ɂȂ肤��ꍇ��SetPattern()���g���Ă��������B
	mRegexp( std::basic_string<T> pattern , const Option& opt ) noexcept
	{
		CreateRegexpObject( pattern , &opt );
	}

	//����Ε�����A�f�X�g���N�^
	virtual ~mRegexp() = default;

	//���K�\���p�^�[�����Z�b�g����
	//pattern : ���K�\���̃p�^�[��
	//opt : ���K�\���}�b�`�������̃I�v�V����
	//      �ȗ���nullptr���w�肵���ꍇ�́A����܂ł̃I�v�V�����w�肪�ێ�����܂�
	//ret : ������true
	bool SetPattern( std::basic_string<T> pattern , const Option* opt = nullptr )
	{
		return CreateRegexpObject( pattern , opt );
	}

	//���K�\���p�^�[�����Z�b�g����
	//pattern : ���K�\���̃p�^�[��
	//opt : ���K�\���}�b�`�������̃I�v�V����
	//ret : ������true
	bool SetPattern( std::basic_string<T> pattern , const Option& opt )
	{
		return CreateRegexpObject( pattern , &opt );
	}

	//�����񂪐��K�\���ƃ}�b�`���邩�H
	//target : �}�b�`�����镶����
	//ret : �}�b�`�����ꍇtrue
	//�Etarget�ƕ����I�Ƀ}�b�`�����OK�ł�(�S�̂ƃ}�b�`�������ꍇ��ExactMatch���g�p���܂�)
	//�E�}�b�`���������ƁASubmatch()�ŕ�����������擾�ł��܂�(Option::NoSubstr=false�̏ꍇ)
	bool Match( const std::basic_string<T> target )
	{
		return MatchInternal( target , false );
	}

	//������S�̂����K�\���ƃ}�b�`���邩�H
	//target : �}�b�`�����镶����
	//ret : �}�b�`�����ꍇtrue
	//�Etarget�ƑS�̂��}�b�`�����OK�ł�(�ꕔ���̂݃}�b�`�������ꍇ��Match���g�p���܂�)
	//�E�}�b�`���������ƁASubmatch()�ŕ�����������擾�ł��܂�(Option::NoSubstr=false�̏ꍇ)
	bool ExactMatch( const std::basic_string<T> target )
	{
		return MatchInternal( target , true );
	}

	//������������i�[�����R���e�i
	typedef std::match_results<typename std::basic_string<T>::const_iterator> Matches;

	//������������i�[�����R���e�i�ւ̎Q�Ƃ��擾���܂�
	//ret : ������������i�[�����R���e�i�ւ̎Q��
	const Matches& Submatch( void ) const noexcept
	{
		return MyMatches;
	}

	//������������擾���܂�
	//index : ����������̃C���f�b�N�X�i0=�}�b�`�S�́A1�`=����������j
	//ret : ����������
	std::basic_string<T> Submatch( size_t index ) const noexcept
	{
		return MyMatches[ index ];
	}

	//�G���[�R�[�h���擾����
	std::regex_constants::error_type GetErrorCode( void )const
	{
		return MyErrorCode;
	}

	//�G���[���b�Z�[�W���擾����
	const WString& GetErrorMessage( void )const
	{
		return MyErrorMessage;
	}

	mRegexp( const mRegexp<T>& src )
	{
		MyRegex = src.MyRegex;
		MyTargetCopy = src.MyTargetCopy;
		MyMatches = src.MyMatches;
		MyErrorCode = src.MyErrorCode;
		MyErrorMessage = src.MyErrorMessage;
	}

	mRegexp<T>& operator=( const mRegexp<T>& src )
	{
		MyRegex = src.MyRegex;
		MyTargetCopy = src.MyTargetCopy;
		MyMatches = src.MyMatches;
		MyErrorCode = src.MyErrorCode;
		MyErrorMessage = src.MyErrorMessage;
	}

private:
	mRegexp() = delete;

protected:

	//���K�\���I�u�W�F�N�g
	std::basic_regex<T> MyRegex;

	//�}�b�`�������ꍇ�ɁA�^�[�Q�b�g���������U�R�s�[���邽�߂̂���
	//����������́A�^�[�Q�b�g������̈ꕔ���ւ̎Q�Ƃ�z��ɂ����`�Ŋi�[�����̂ŁA
	//�^�[�Q�b�g���ꎞ�I�u�W�F�N�g�������肷��ƃk�����t�@�����X�ɂȂ��Ă��܂��B
	//���ꂾ�ƍ���̂ŁA������������g���ꍇ�͈�x�R�s�[����B
	std::basic_string<T> MyTargetCopy;

	//����������̊i�[��
	Matches MyMatches;

	//���C�u�������ŃG���[���N�����ꍇ�̃G���[�R�[�h�i�[��
	std::regex_constants::error_type MyErrorCode;

	//���C�u�������ŃG���[���N�����ꍇ�̃G���[���b�Z�[�W�i�[��
	WString MyErrorMessage;

	//���K�\���I�u�W�F�N�g���\�z����
	//pattern : ���K�\���̃p�^�[��
	//opt : ���K�\���}�b�`�������̃I�v�V����
	//      �ȗ���nullptr���w�肵���ꍇ�́A����܂ł̃I�v�V�����w�肪�ێ�����܂�
	//ret : ������true
	bool CreateRegexpObject( typename  std::basic_string<T> pattern , const Option* opt = nullptr )
	{
		//���݂̃I�v�V�����ݒ�l���擾
		DWORD flags = (DWORD)MyRegex.flags();

		//�I�v�V�������ݒ肳��Ă���ꍇ�́A����ɏ]���l���X�V
		//nullptr�ł���ꍇ�́A����ێ��Ȃ̂ŉ������Ȃ�
		if( opt != nullptr )
		{
			//�啶���E�������̋�ʁH
			flags &= ~std::regex_constants::icase;
			flags |= ( opt->IgnoreCase ) ? ( std::regex_constants::icase ) : ( 0 );
			//������������g���H
			flags &= ~std::regex_constants::nosubs;
			flags |= ( opt->NoSubstr ) ? ( std::regex_constants::nosubs ) : ( 0 );
		}

		//���K�\���I�u�W�F�N�g���\�z�B
		//�G���[������ꍇ�͗�O�����ł���̂ŁAtry-catch���g���B
		bool result = true;
		try
		{
			MyRegex.assign( pattern.cbegin() , pattern.cend() , (std::regex_constants::syntax_option_type)flags );
		}
		catch( std::regex_error e )
		{
			//�G���[�̏ꍇ�́A�G���[�̋L�^������false��Ԃ�
			MyErrorCode = e.code();
			MyErrorMessage = AString2WString( e.what() );
			RaiseError( g_ErrorLogger , MyErrorCode , MyErrorMessage );
			return false;
		}
		return true;
	}

	//�����񂪐��K�\���ƃ}�b�`���邩�H
	//target : �}�b�`�����镶����
	//is_exact : true�̏ꍇ��target�S�̂ƃ}�b�`�Afalse�̏ꍇ�͕����}�b�`
	//ret : �}�b�`�����ꍇtrue
	bool MatchInternal( const std::basic_string<T> target , bool is_exact = false )
	{
		bool result = false;	//�}�b�`�������H

		//������������g�p���Ă���ꍇ�́A�^�[�Q�b�g�̕�������R�s�[���Ă���}�b�`����
		//(�^�[�Q�b�g���ꎞ�ϐ��������ꍇ�k�����ɂȂ邽��)
		if( ( MyRegex.flags() & std::regex_constants::nosubs ) == 0 )
		{
			//������������g�p����ꍇ�B
			//������v��regex_search ���S��v��regex_match
			MyTargetCopy = target;
			if( is_exact )
			{
				result = std::regex_match( MyTargetCopy.cbegin() , MyTargetCopy.cend() , MyMatches , MyRegex );
			}
			else
			{
				result = std::regex_search( MyTargetCopy.cbegin() , MyTargetCopy.cend() , MyMatches , MyRegex );
			}
		}
		else
		{
			//������v���g�p���Ȃ��ꍇ
			if( is_exact )
			{
				result = std::regex_match( target.cbegin() , target.cend() , MyRegex );
			}
			else
			{
				result = std::regex_search( target.cbegin() , target.cend() , MyRegex );
			}
		}
		return result;
	}

};

//AString / CHAR�p
typedef mRegexp<CHAR> mRegexpA;

//TString / TCHAR�p
typedef mRegexp<TCHAR> mRegexpT;

//WString / WCHAR�p
typedef mRegexp<WCHAR> mRegexpW;

#endif //MREGEXP_H_INCLUDED
