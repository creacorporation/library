//----------------------------------------------------------------------------
// GUID
// Copyright (C) 2024 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MGUID_H_INCLUDED
#define MGUID_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mGUID
{
public:

	mGUID();
	mGUID( const WString& src );
	mGUID( const AString& src );
	mGUID( const GUID& src );
	mGUID( const mGUID& source );
	mGUID( uint32_t dt1 , uint16_t dt2 , uint16_t dt3 , uint64_t dt4 );

	const mGUID& operator=( const WString& src );
	const mGUID& operator=( const AString& src );
	const mGUID& operator=( const GUID& src );
	const mGUID& operator=( const mGUID& src );

	virtual ~mGUID();

	//�l���O�N���A����(nil GUID�ɂ���)
	void Clear( void );

	//�l��ݒ肷��
	bool Set( const WString& src );
	//�l��ݒ肷��
	bool Set( const AString& src );
	//�l��ݒ肷��
	bool Set( const GUID& src );
	//�l��ݒ肷��
	bool Set( const mGUID& src );
	//�l��ݒ肷��
	bool Set( uint32_t dt1 , uint16_t dt2 , uint16_t dt3 , uint64_t dt4 );

	//�V����GUID�𐶐�����
	bool Create( void );

	//GUID���擾
	operator const GUID&( void )const;
	//GUID���擾
	const GUID& ToGUID( void )const;
	//GUID���擾
	operator AString( void )const;
	//GUID���擾
	const AString ToAString( void )const;
	//GUID���擾
	operator WString( void )const;
	//GUID���擾
	const WString ToWString( void )const;

	bool operator ==( const mGUID& src )const;
	bool operator !=( const mGUID& src )const;
	bool operator <( const mGUID& src )const;

protected:
	GUID MyGUID;

};

namespace std
{
	template<>
	class hash< mGUID >
	{
	public:
		size_t operator() ( const mGUID &src ) const
		{
			return src.ToGUID().Data1;
		}
	};
}

#endif