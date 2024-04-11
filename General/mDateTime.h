//----------------------------------------------------------------------------
// ���t�����Ǘ�
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------
#ifndef MTIMESTAMP_H_INCLUDED
#define MTIMESTAMP_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mException.h"

namespace Definitions_mDateTime
{
	enum DAYOFWEEK
	{
		DAY_SUNDAY,
		DAY_MONDAY,
		DAY_TUESDAY,
		DAY_WEDNESDAY,
		DAY_THURSDAY,
		DAY_FRIDAY,
		DAY_SATURDAY,
		DAY_INVALID,
	};

	enum JAPANESE_ERA
	{
		JPERA_UNKNOWN,		//�s��
		JPERA_REIWA,		//�ߘa
		JPERA_HEISEI,		//����
		JPERA_SHOWA,		//���a
		JPERA_TAISYO,		//�吳
		JPERA_MEIJI,		//����
		eMAX_JAPANESE_ERA
	};

	enum INIT_WITH
	{
		CURRENT_SYSTEMTIME,
		CURRENT_LOCALTIME,
	};
};

namespace mDateTime
{
	using DAYOFWEEK = Definitions_mDateTime::DAYOFWEEK;
	using JAPANESE_ERA = Definitions_mDateTime::JAPANESE_ERA;
	using INIT_WITH = Definitions_mDateTime::INIT_WITH;

	void AdjustYearMonth( INT& ioYear , INT& ioMonth );

	bool IsLeapYear( INT Year );
	INT GetLastDay( INT Year , INT Month );

	struct JapaneseEra
	{
		JAPANESE_ERA Id;
		const wchar_t* Word;
		INT Year;
	};

	struct YearMonth
	{
		INT Year;	//�N(�O���S���I��)
		INT Month;	//��

		void Clear()
		{
			Year = 9999;
			Month = 12;
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		YearMonth( INIT_WITH init_with );
		YearMonth( const AString& src );
		YearMonth( const WString& src );

		YearMonth( void )
		{
			Clear();
		}

		YearMonth( INT year , INT month )
		{
			Set( year , month );
		}

		void Set( INT year , INT month )
		{
			Year = year;
			Month = month;
		}

		bool Set( const AString& src );
		bool Set( const WString& src );

		//�i�[���Ă�����t���L���ł��邩�ۂ���Ԃ��܂�
		//�����Ō����L���Ƃ́A���݂�����t���ǂ����ł�
		//ret : ���t���L���ł���ΐ^
		bool IsValid( void )const;

		//���t�ɑ΂���a������߂܂�
		//�����w�肵�Ȃ��Ƃ��̌��̏����ɂ��ċ��߂܂��B���ɂ����w�肷��Ƃ��̓��ɂ��ċ��߂܂��B
		bool GetJapaneseEra( JapaneseEra& retEra , int day = 1 )const noexcept;

		bool operator<=( const YearMonth& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const YearMonth& src ) const
		{
			if( Year  != src.Year  ) return Year  < src.Year;
			return Month < src.Month;
		}
		bool operator>=( const YearMonth& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const YearMonth& src ) const
		{
			if( Year  != src.Year  ) return Year  > src.Year;
			return Month > src.Month;
		}
		bool operator==( const YearMonth& src ) const
		{
			if( Month != src.Month ) return false;
			if( Year  != src.Year  ) return false;
			return true;
		}
		bool operator!=( const YearMonth& src ) const
		{
			return !( this->operator==( src ) );
		}

		//�����w������������ω����܂�
		//�ω���́u���v�����̍ŏI���𒴂���ƁA�ŏI���ɕ␳���܂��B
		//�ω��O�̔N�������s���ł���ƕω�����s���ɂȂ�܂�
		void AddMonth( int offset );

		//���ƔN�x�����߂܂�
		//StartMonth : ���ƔN�x�̊J�n��(�S�Ȃ�΂S�����痂�N�R���܂ł��P���ƔN�x)
		INT GetBusinessYear( INT StartMonth = 4 )const;

		//���K�����܂�
		// 2020�N13����2021�N1���̂悤�ɕ␳���܂�
		void Normalize( void )noexcept;
	};

	//���t�E�^�C���X�^���v���w�肵�����̏����ŏ���������
	struct InitWithFirstDayOf
	{
		InitWithFirstDayOf()
		{
			SYSTEMTIME tm;
			GetLocalTime( &tm );
			year = tm.wYear;
			month = tm.wMonth;
		}

		//���t�E�^�C���X�^���v���w�肵�����̏����ŏ���������
		//�@month�Ƀ[���ȉ��̐����A13�ȏ�̐��������Ă����܂��܂���B
		//�@month=0,-1������O�N12���A11�����
		//�@month=13,14��������N1���A2�����
		//�@�Ƃ݂Ȃ��܂��B
		InitWithFirstDayOf( INT Year , INT Month )
		{
			year = Year;
			month = Month;
		}

		//���t�E�^�C���X�^���v���w�肵�����̏����ŏ���������
		InitWithFirstDayOf( const YearMonth& ym )
		{
			year = ym.Year;
			month = ym.Month;
		}

		INT year;
		INT month;
	};

	//���t�E�^�C���X�^���v���w�肵�����̖����ŏ���������
	struct InitWithLastDayOf
	{
		InitWithLastDayOf()
		{
			SYSTEMTIME tm;
			GetLocalTime( &tm );
			year = tm.wYear;
			month = tm.wMonth;
		}
		//���t�E�^�C���X�^���v���w�肵�����̖����ŏ���������
		//�@month�Ƀ[���ȉ��̐����A13�ȏ�̐��������Ă����܂��܂���B
		//�@month=0,-1������O�N12���A11�����
		//�@month=13,14��������N1���A2�����
		//�@�Ƃ݂Ȃ��܂��B
		InitWithLastDayOf( INT Year , INT Month )
		{
			year = Year;
			month = Month;
		}

		//���t�E�^�C���X�^���v���w�肵�����̏����ŏ���������
		InitWithLastDayOf( const YearMonth& ym )
		{
			year = ym.Year;
			month = ym.Month;
		}

		INT year;
		INT month;
	};

	//�O���S���I��ɂ����t���i�[���܂�
	//�E���{���O���S���I����̗p�����̂�1873�N1��1���ł��B
	//�@���{�̗�v�Z������ړI�ŁA����ȑO�̓��t���i�[����ƌ�������ʂɂȂ�܂��B
	struct Date
	{
		INT Year;	//�N(�O���S���I��)
		INT Month;	//��
		INT Day;	//��

		void Clear()
		{
			Year = 9999;
			Month = 12;
			Day = 31;
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		Date( INIT_WITH init_with );

		//���ӁFInitWithFirstDayOf���ꎞ�I�u�W�F�N�g�Ƃ��č��ꍇ�ɁA������������ϐ��ɂ���ƁA�֐���`�Ɍ�F����邩�璍�ӁB
		//�~�@mDateTime::Date day( mDateTime::InitWithFirstDayOf( year_month ) );
		//���@mDateTime::Date day = mDateTime::InitWithFirstDayOf( year_month );
		Date( const InitWithFirstDayOf& init_with );

		//���ӁFInitWithLastDayOf���ꎞ�I�u�W�F�N�g�Ƃ��č��ꍇ�ɁA������������ϐ��ɂ���ƁA�֐���`�Ɍ�F����邩�璍�ӁB
		//�~�@mDateTime::Date day( mDateTime::InitWithLastDayOf( year_month ) );
		//���@mDateTime::Date day = mDateTime::InitWithLastDayOf( year_month );
		Date( const InitWithLastDayOf& init_with );
		Date( const AString& src );
		Date( const WString& src );

		template< class ExceptionIfFailed >
		Date( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		template< class ExceptionIfFailed >
		Date( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		Date( void )
		{
			Clear();
		}

		Date( INT year , INT month , INT day )
		{
			Set( year , month , day );
		}

		void Set( INT year , INT month , INT day )
		{
			Year = year;
			Month = month;
			Day = day;
		}

		//�������ǂݎ���ē��t���Z�b�g���܂��B
		//���������t�Ƃ��ēǂݎ��Ȃ��ꍇ�́A�e���v���[�g�Ŏw�肵���^�̗�O�𓊂��܂��B���̌^��mException�̔h���^�łȂ���΂Ȃ�܂���B
		// ���Q�l�� using err = class ErrorClass : mException{};
		//src : �ǂݎ�镶����
		template< class ExceptionIfFailed >
		void Set( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//�������ǂݎ���ē��t���Z�b�g���܂��B
		//���������t�Ƃ��ēǂݎ��Ȃ��ꍇ�́A�e���v���[�g�Ŏw�肵���^�̗�O�𓊂��܂��B���̌^��mException�̔h���^�łȂ���΂Ȃ�܂���B
		// ���Q�l�� using err = class ErrorClass : mException{};
		//src : �ǂݎ�镶����
		template< class ExceptionIfFailed >
		void Set( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//�������ǂݎ���ē��t���Z�b�g���܂��B
		//src : �ǂݎ�镶����
		//ret : �������^
		bool Set( const AString& src );

		//�������ǂݎ���ē��t���Z�b�g���܂��B
		//src : �ǂݎ�镶����
		//ret : �������^
		bool Set( const WString& src );

		//�i�[���Ă�����t���L���ł��邩�ۂ���Ԃ��܂�
		//�����Ō����L���Ƃ́A���݂�����t���ǂ����ł�
		//ret : ���t���L���ł���ΐ^
		bool IsValid( void )const;

		//�i�[���Ă�����t�̏C�������E�X�ʓ������߂܂�
		//���L���ȓ��t���i�[���Ă��Ȃ��ꍇ�͕s���Ȍ��ʂɂȂ�܂�
		//ret : �i�[���Ă�����t�̏C�������E�X�ʓ�
		double ToModJulian( void )const;

		//�C�������E�X�ʓ�������t���擾���Ċi�[���܂�
		//julian : �C�������E�X�ʓ�
		void FromModJulian( double julian );

		//���t�ɑ΂���a������߂܂�
		bool GetJapaneseEra( JapaneseEra& retEra )const noexcept;

		//�j�������߂܂�
		DAYOFWEEK DayOfWeek( void ) const noexcept;

		//�w��N���̏����ɐݒ肵�܂�
		//�@month�Ƀ[���ȉ��̐����A13�ȏ�̐��������Ă����܂��܂���B
		//�@month=0,-1������O�N12���A11�����
		//�@month=13,14��������N1���A2�����
		//�@�Ƃ݂Ȃ��܂��B
		void SetFirstDay( INT year , INT month )noexcept;

		//�w��N���̏����ɐݒ肵�܂�
		//�@month�Ƀ[���ȉ��̐����A13�ȏ�̐��������Ă����܂��܂���B
		//�@month=0,-1������O�N12���A11�����
		//�@month=13,14��������N1���A2�����
		//�@�Ƃ݂Ȃ��܂��B
		void SetFirstDay( const YearMonth& ym )noexcept;

		//�w��N���̍ŏI���ɐݒ肵�܂�
		//�@month�Ƀ[���ȉ��̐����A13�ȏ�̐��������Ă����܂��܂���B
		//�@month=0,-1������O�N12���A11�����
		//�@month=13,14��������N1���A2�����
		//�@�Ƃ݂Ȃ��܂��B
		void SetLastDay( INT year , INT month )noexcept;

		//�w��N���̍ŏI���ɐݒ肵�܂�
		//�@month�Ƀ[���ȉ��̐����A13�ȏ�̐��������Ă����܂��܂���B
		//�@month=0,-1������O�N12���A11�����
		//�@month=13,14��������N1���A2�����
		//�@�Ƃ݂Ȃ��܂��B
		void SetLastDay( const YearMonth& ym )noexcept;

		bool operator<=( const Date& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const Date& src ) const
		{
			if( Year  != src.Year  ) return Year  < src.Year;
			if( Month != src.Month ) return Month < src.Month;
			return Day < src.Day;
		}
		bool operator>=( const Date& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const Date& src ) const
		{
			if( Year  != src.Year  ) return Year  > src.Year;
			if( Month != src.Month ) return Month > src.Month;
			return Day > src.Day;
		}
		bool operator==( const Date& src ) const
		{
			if( Day   != src.Day   ) return false;
			if( Month != src.Month ) return false;
			if( Year  != src.Year  ) return false;
			return true;
		}
		bool operator!=( const Date& src ) const
		{
			return !( this->operator==( src ) );
		}

		//���t���w������������ω����܂�
		//�ω��O�̔N�������s���ł���ƕω�����s���ɂȂ�܂�
		void AddDay( int offset );

		//�����w�茎���������ω����܂�
		//�ω���́u���v�����̍ŏI���𒴂���ƁA�ŏI���ɕ␳���܂��B
		//�������A���̕␳���N�������Ƃ��L������킯�ł͂Ȃ��̂ŁA�J��Ԃ��ω�����ꍇ�͒��ӂ��K�v�ł��B
		//��F2020�N1��31����AddMonth(1)��2020�N2��29����AddMonth(1)��2020�N3��29��
		//�ω��O�̔N�������s���ł���ƕω�����s���ɂȂ�܂�
		void AddMonth( int offset );

		//���ƔN�x�����߂܂�
		//StartMonth : ���ƔN�x�̊J�n��(�S�Ȃ�΂S�����痂�N�R���܂ł��P���ƔN�x)
		INT GetBusinessYear( INT StartMonth = 4 )const;

	};

	//�������i�[���܂�
	struct Time
	{
		INT Hour;			//��(0-23)
		INT Minute;			//��(0-59)
		INT Second;			//�b(0-59)
		INT Milliseconds;	//�~���b(0-999)

		void Clear()
		{
		   Hour = 0;
		   Minute = 0;
		   Second = 0;
		   Milliseconds = 0;
		}

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		Time( double src );
		Time( INIT_WITH INIT_WITH );
		Time( const AString& src );
		Time( const WString& src );

		template< class ExceptionIfFailed >
		Time( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		template< class ExceptionIfFailed >
		Time( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		Time( INT hour = 0 , INT minute = 0 , INT second = 0 , INT milliseconds = 0 )
		{
		   Set( hour , minute , second , milliseconds );
		}

		void Set( INT hour = 0 , INT minute = 0 , INT second = 0 , INT milliseconds = 0 )
		{
		   Hour = hour;
		   Minute = minute;
		   Second = second;
		   Milliseconds = milliseconds;
		}

		//�������ǂݎ���Ď������Z�b�g���܂��B
		//src : �ǂݎ�镶����
		//ret : �������^
		bool Set( const AString& src );

		//�������ǂݎ���Ď������Z�b�g���܂��B
		//src : �ǂݎ�镶����
		//ret : �������^
		bool Set( const WString& src );

		//�������ǂݎ���Ď������Z�b�g���܂��B
		//���������t�Ƃ��ēǂݎ��Ȃ��ꍇ�́A�e���v���[�g�Ŏw�肵���^�̗�O�𓊂��܂��B���̌^��mException�̔h���^�łȂ���΂Ȃ�܂���B
		// ���Q�l�� using err = class ErrorClass : mException{};
		//src : �ǂݎ�镶����
		template< class ExceptionIfFailed >
		void Set( const AString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//�������ǂݎ���Ď������Z�b�g���܂��B
		//���������t�Ƃ��ēǂݎ��Ȃ��ꍇ�́A�e���v���[�g�Ŏw�肵���^�̗�O�𓊂��܂��B���̌^��mException�̔h���^�łȂ���΂Ȃ�܂���B
		// ���Q�l�� using err = class ErrorClass : mException{};
		//src : �ǂݎ�镶����
		template< class ExceptionIfFailed >
		void Set( const WString& src )
		{
			static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mException" );
			if( !Set( src ) )
			{
				throw ExceptionIfFailed();
			}
		}

		//�i�[���Ă��鎞�����L���ł��邩�ۂ���Ԃ��܂�
		//�������Ō����L���Ƃ́A0:0:0.000�`23:59:59.999�͈͓̔����A�e�p�[�g���J��オ��E�J�艺����Ȃ��\�L�\���ۂ��ł��B
		//ret : �������L���ł���ΐ^
		bool IsValid( void )const;

		//�i�[���Ă��鎞���ɑ΂��āA1����1.0�Ƃ����䗦�����߂܂�
		//���L���ȓ��t���i�[���Ă��Ȃ��ꍇ�͕s���Ȍ��ʂɂȂ�܂�
		//���C�������E�X�ʓ��̏����_�ȉ��̒l�Ƃ��Ďg�p�ł��܂�
		//ret : �i�[���Ă��鎞����1���ɑ΂���䗦
		double ToValue( void )const;

		//1����1.0�Ƃ����䗦���玞�����擾���Ċi�[���܂�
		// value : �i�[���Ă��鎞����1���ɑ΂���䗦
		void FromValue( double value );

		//���ԒP�ʂɒ����܂�
		double ToHour( void )const;

		//���P�ʂɒ����܂�
		double ToMinute( void )const;

		//�b�P�ʂɒ����܂�
		double ToSecond( void )const;

		//���K�����܂�
		// 1:00:80 �� 1:01:20�̂悤�ɕ␳���܂�
		void Normalize( void )noexcept;

		bool operator<=( const Time& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const Time& src ) const
		{
		   if( Hour   != src.Hour   ) return Hour   < src.Hour  ;
		   if( Minute != src.Minute ) return Minute < src.Minute;
		   if( Second != src.Second ) return Second < src.Second;
		   return Milliseconds < src.Milliseconds;
		}
		bool operator>=( const Time& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const Time& src ) const
		{
		   if( Hour   != src.Hour   ) return Hour   > src.Hour  ;
		   if( Minute != src.Minute ) return Minute > src.Minute;
		   if( Second != src.Second ) return Second > src.Second;
		   return Milliseconds < src.Milliseconds;
		}
		bool operator==( const Time& src ) const
		{
		   if( Second       != src.Second       ) return false;
		   if( Minute       != src.Minute       ) return false;
		   if( Hour         != src.Hour         ) return false;
		   if( Milliseconds != src.Milliseconds ) return false;
		   return true;
		}
		bool operator!=( const Time& src ) const
		{
			return !( this->operator==( src ) );
		}

		const Time operator+( const Time& v )const;
		void operator+=( const Time& v );
		const Time operator-( const Time& v )const;
		void operator-=( const Time& v );
		const Time operator*( double v )const;
		void operator*=( double v );
		const Time operator/( double v )const;
		double operator/( const Time& divider )const;
		void operator/=( double v );
		const Time operator%( double v )const;
		void operator%=( double v );

	};

	//�O���S���I��ɂ��^�C���X�^���v���i�[���܂�
	//�E���{���O���S���I����̗p�����̂�1873�N1��1���ł��B
	//�@���{�̗�v�Z������ړI�ŁA����ȑO�̓��t���i�[����ƌ�������ʂɂȂ�܂��B
	struct Timestamp
	{
		INT Year;			//�N(�O���S���I��)
		INT Month;			//��
		INT Day;				//��
		INT Hour;			//��(0-23)
		INT Minute;			//��(0-59)
		INT Second;			//�b(0-59)
		INT Milliseconds;	//�~���b(0-999)

		void Clear()
		{
		   Year = 9999;
		   Month = 12;
		   Day = 31;
		   Hour = 0;
		   Minute = 0;
		   Second = 0;
		   Milliseconds = 0;
		}

		Timestamp()
		{
			Clear();
		}
		Timestamp( INIT_WITH INIT_WITH );
		Timestamp( const InitWithFirstDayOf& init_with );
		Timestamp( const InitWithLastDayOf& init_with );

		void SetCurrentLocal( void );
		void SetCurrentSystem( void );

		//�i�[���Ă�����t���L���ł��邩�ۂ���Ԃ��܂�
		//ret : ���t���L���ł���ΐ^
		bool IsValid( void )const;

		//�i�[���Ă�����t�̏C�������E�X�ʓ������߂܂�
		//���L���ȓ��t���i�[���Ă��Ȃ��ꍇ�͕s���Ȍ��ʂɂȂ�܂�
		//ret : �i�[���Ă�����t�̏C�������E�X�ʓ�
		double ToModJulian( void )const;

		//�C�������E�X�ʓ�������t���擾���Ċi�[���܂�
		//julian : �C�������E�X�ʓ�
		void FromModJulian( double julian );

		//���t�����o���܂�
		Date GetDate( void )const;

		//���������o���܂�
		Time GetTime( void )const;

		//���t���Z�b�g���܂��B�����͕ω����܂���B
		void Set( const Date& date );

		//�������Z�b�g���܂��B���t�͕ω����܂���B
		void Set( const Time& time );

		//�w��N���̏����ɐݒ肵�܂��B�����͕ω����܂���B
		void SetFirstDay( INT year , INT month )noexcept;

		//�w��N���̍ŏI���ɐݒ肵�܂��B�����͕ω����܂���B
		void SetLastDay( INT year , INT month )noexcept;

		//���t�Ǝ������Z�b�g���܂�
		void Set( const Date& date , const Time& time );

		//�j�������߂܂�
		//�E���t�݂̂ɑ΂���j���ɂȂ�܂��B���j����25���ɑ΂��Ă͐��j����Ԃ��܂��B
		DAYOFWEEK DayOfWeek( void ) const noexcept;

		//���t�ɑ΂���a������߂܂�
		bool GetJapaneseEra( JapaneseEra& retEra )const noexcept;

		//���ƔN�x�����߂܂�
		//StartMonth : ���ƔN�x�̊J�n��(�S�Ȃ�΂S�����痂�N�R���܂ł��P���ƔN�x)
		INT GetBusinessYear( INT StartMonth = 4 )const;

	   //���K�����܂�
		//�E1��1��28����1��2��4���̂悤�ɕ␳���܂�
		//�E�����͂����Ȃ�l�������ĂĂ���肠��܂���B
		//�E���t�͎��݂��Ȃ����������Ă���ƕs���ɂȂ�܂��B
		void Normalize( void )noexcept;

		bool operator<=( const Timestamp& src ) const
		{
			return !( this->operator>( src ) );
		}
		bool operator<( const Timestamp& src ) const
		{
		   if( Year   != src.Year   ) return Year   < src.Year  ;
		   if( Month  != src.Month  ) return Month  < src.Month ;
		   if( Day    != src.Day    ) return Day    < src.Day   ;
		   if( Hour   != src.Hour   ) return Hour   < src.Hour  ;
		   if( Minute != src.Minute ) return Minute < src.Minute;
		   if( Second != src.Second ) return Second < src.Second;
		   return Milliseconds < src.Milliseconds;
		}
		bool operator>=( const Timestamp& src ) const
		{
			return !( this->operator<( src ) );
		}
		bool operator>( const Timestamp& src ) const
		{
		   if( Year   != src.Year   ) return Year   > src.Year  ;
		   if( Month  != src.Month  ) return Month  > src.Month ;
		   if( Day    != src.Day    ) return Day    > src.Day   ;
		   if( Hour   != src.Hour   ) return Hour   > src.Hour  ;
		   if( Minute != src.Minute ) return Minute > src.Minute;
		   if( Second != src.Second ) return Second > src.Second;
		   return Milliseconds > src.Milliseconds;
		}
		bool operator==( const Timestamp& src ) const
		{
		   if( Second       != src.Second       ) return false;
		   if( Minute       != src.Minute       ) return false;
		   if( Hour         != src.Hour         ) return false;
		   if( Day          != src.Day          ) return false;
		   if( Month        != src.Month        ) return false;
		   if( Year         != src.Year         ) return false;
		   if( Milliseconds != src.Milliseconds ) return false;
		   return true;
		}
		bool operator!=( const Timestamp& src ) const
		{
			return !( this->operator==( src ) );
		}
	};
};

namespace std
{
    template<>
    class hash< mDateTime::Date >
	{
        public:
        size_t operator() ( const mDateTime::Date &src ) const
		{
			return ( src.Year << 16 ) + ( (uint8_t)src.Month << 8 ) + ( src.Day );
		}
    };

	template<>
	class hash< mDateTime::YearMonth >
	{
	public:
		size_t operator() ( const mDateTime::YearMonth &src ) const
		{
			return ( src.Year << 16 ) + ( src.Month );
		}
	};

    template<>
    class hash< mDateTime::Time >
	{
        public:
        size_t operator() ( const mDateTime::Time &src ) const
		{
			return ( src.Hour << 16 ) + ( (uint8_t)src.Minute << 8 ) + ( src.Second ) + ( src.Milliseconds );
		}
    };

    template<>
    class hash< mDateTime::Timestamp >
	{
        public:
        size_t operator() ( const mDateTime::Timestamp &src ) const
		{
			size_t d = ( src.Year << 16 ) + ( (uint8_t)src.Month << 8 ) + ( src.Day );
			size_t t = ( src.Hour << 16 ) + ( (uint8_t)src.Minute << 8 ) + ( src.Second ) + ( src.Milliseconds );
#if _WIN64
			return ( d << 32 ) + t;
#else
			return d ^ t;
#endif
		}
    };
};

#endif

