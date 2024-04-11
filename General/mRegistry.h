//----------------------------------------------------------------------------
// ���W�X�g���A�N�Z�X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#ifndef MREGISTRY_H_INCLUDED
#define MREGISTRY_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include <memory>
#include <setupapi.h>

//���W�X�g���̃��b�p�[
namespace Definitions_mRegistry
{
	enum ParentKey
	{
		CURRENT_USER,	//HKEY_CURRENT_USER
		LOCAL_MACHINE,	//HKEY_LOCAL_MACHINE
	};
};

class mRegistry
{
public:
	mRegistry();
	virtual ~mRegistry();

	using ParentKey = Definitions_mRegistry::ParentKey;

	//���W�X�g���̃p�X���J��
	bool Open( ParentKey parent , const WString& path , BOOL access_write );

	//���W�X�g���̃p�X�����
	bool Close( void );

	//DWORD�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// def_value : �Y���̃G���g�����Ȃ������ꍇ�A���̑��G���[�̏ꍇ�Ɏ擾����f�t�H���g�̒l
	// ret : �ǂݎ�����l�B�G���[�̏ꍇ��def_value�ɐݒ肵���l
	DWORD GetDword( const WString& entry , DWORD def_value );

	//DWORD�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// value : �ݒ肷��l
	// ret : �������^
	bool SetDword( const WString& entry , DWORD value );

	//QWORD�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// def_value : �Y���̃G���g�����Ȃ������ꍇ�A���̑��G���[�̏ꍇ�Ɏ擾����f�t�H���g�̒l
	// ret : �ǂݎ�����l�B�G���[�̏ꍇ��def_value�ɐݒ肵���l
	ULONGLONG GetQword( const WString& entry , ULONGLONG def_value );

	//DWORD�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// value : �ݒ肷��l
	// ret : �������^
	bool SetQword( const WString& entry , ULONGLONG value );


	//WString�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// def_value : �Y���̃G���g�����Ȃ������ꍇ�A���̑��G���[�̏ꍇ�Ɏ擾����f�t�H���g�̒l
	// ret : �ǂݎ�����l�B�G���[�̏ꍇ��def_value�ɐݒ肵���l
	WString GetString( const WString& entry , const WString& def_value );

	//WString�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// value : �ݒ肷��l
	// ret : �������^
	bool SetString( const WString& entry , const WString& value );

	//string_vector�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// ret_value : �ǂݎ�����l
	// ret : �������^
	bool GetMultiString( const WString& entry , WStringVector& ret_value );

	//string_vector�l�̓ǂݏ���
	// entry : �G���g���̖��O
	// value : �ݒ肷��l
	// ret : �������^
	bool SetMultiString( const WString& entry , const WStringVector& value );

	//�G���g���̍폜
	// entry : �G���g���̖��O
	// ret : �������^
	bool DeleteEntry( const WString& entry );

	//���W�X�g���̒l�̈ꗗ���擾����
	bool EnumValues( WStringDeque& retValues );

private:
	mRegistry( const mRegistry& src );
	const mRegistry& operator=( const mRegistry& src );

protected:

	//���W�X�g���̃n���h��
	HKEY MyKey;

};



#endif //MREGISTRY_H_INCLUDED
