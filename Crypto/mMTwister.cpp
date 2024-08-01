//----------------------------------------------------------------------------
//�����Z���k�E�c�C�X�^���������N���X
// Copyright (C) 2005 Fingerling. All rights reserved.
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#include "mMTwister.h"
#include "mRand.h"
#include "General/mErrorLogger.h"
#include <array>

mMTwister::mMTwister()
{
	return;
}

bool mMTwister::Initialize( void )
{
	std::array< std::seed_seq::result_type , RandSource::state_size	> seed_data;	//�V�[�h�̒l

	//�V�[�h�̏�����
	mRand rnd;
	rnd.RandFill( reinterpret_cast<BYTE*>( seed_data.data() ) , DWORD( sizeof( seed_data[ 0 ] ) * seed_data.size() ) );

	//������V�[�h��K�p
	std::seed_seq seq( seed_data.begin() , seed_data.end() );
	MyRandSource.seed( seq );

	//�K���ɓǂݔ�΂����s��
	MyRandSource.discard( ( ( timeGetTime() >> 3 ) & 0x0000FFFFUL ) + 10000 );

	return true;
}

UINT mMTwister::RandInt32( void )
{

	static_assert( std::is_same< RandSource::result_type , UINT >::value , "RNG result type mismatch" );
	return MyRandSource();

}

INT mMTwister::RandUInt31( void )
{

	ULONG y = RandInt32();
	return (LONG)( y >> 1);

}

// # a��x��b�𖞂�������x�̏W�����ԂƂ����A[a,b]�ŕ\��
// # 0�ȏ�1�ȉ��B
DOUBLE mMTwister::RandDouble1( void )
{

	ULONG y = RandInt32();

    return (DOUBLE)y * ( 1.0 / 4294967295.0 ); 
}

DOUBLE mMTwister::RandClose( void )
{

	return RandDouble1();

}

// # 0�ȏ�1�����B
DOUBLE mMTwister::RandDouble2( void )
{

	ULONG y = RandInt32();

    return (DOUBLE)y * ( 1.0 / 4294967296.0 ); 
}

DOUBLE mMTwister::RandSemiOpen( void )
{

	return RandDouble2();

}

// # 0���傫��1��菬�����B
DOUBLE mMTwister::RandDouble3( void )
{

	ULONG y = RandInt32();

    return ( (DOUBLE)y + 0.5 ) * ( 1.0 / 4294967296.0 ); 
}

DOUBLE mMTwister::RandOpen( void )
{

	return RandDouble3();

}

//�n�����o�b�t�@�𗐐��l�Ŗ��߂�
void mMTwister::RandFill( BYTE* buffer , DWORD bufferlen )
{
	while( ( 0 < bufferlen ) && ( (ULONG_PTR)buffer & 0x03 ) )
	{
		*buffer = ( RandInt32() ) & 0xFFu;
		buffer++;
		bufferlen--;
	}
	while( 3 < bufferlen )
	{
		*(UINT*)buffer = RandInt32();
		buffer += 4;
		bufferlen -= 4;
	}
	while( 0 < bufferlen )
	{
		*buffer = ( RandInt32() ) & 0xFFu;
		buffer++;
		bufferlen--;
	}
	return;
}

//�w�肵��2�l�Ԃɋϓ����z����l
UINT mMTwister::RandBetween( UINT val1 , UINT val2 )
{
	if( val1 == val2 )
	{
		return val1;
	}
	else if( val1 == 0 && val2 == UINT_MAX )
	{
		return RandInt32();
	}

	UINT diff = ( val1 < val2 ) ? ( val2 - val1 + 1 ) : ( val1 - val2 + 1 );
	return UINT( RandSemiOpen() * diff );
}

