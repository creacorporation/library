//----------------------------------------------------------------------------
// �������j�����Ƀ[���N���A����A���P�[�^
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
	�E�ȉ��N���X�Ŏg�p
	�@mSecureBinary
	�@mSecureAString
	�@mSecureWString
*/

#ifndef MSECUREALLOCATOR_H_INCLUDED
#define MSECUREALLOCATOR_H_INCLUDED

#include "mStandard.h"
#include <new>

template< class T >
struct mSecureAllocator {

	// �v�f�̌^
	using value_type = T;

	mSecureAllocator()
	{
	}

	template< class U >
	mSecureAllocator( const mSecureAllocator<U>& )
	{
	}

	// �������m��
	T* allocate( std::size_t size )
	{
		return reinterpret_cast< T* >( std::malloc( sizeof( T ) * size ) );
	}

	// ���������
	void deallocate( T* ptr, std::size_t size )
	{
		SecureZeroMemory( static_cast< PVOID >( ptr ) , size );
		std::free( ptr );
	}

	using char_type = T;

};

// ��r���Z�q
template< class T , class U >
bool operator==( const mSecureAllocator< T >& , const mSecureAllocator< U >& )
{
	return true;
}

template< class T , class U >
bool operator!=( const mSecureAllocator< T >& , const mSecureAllocator< U >& )
{
	return false; 
}

#endif
