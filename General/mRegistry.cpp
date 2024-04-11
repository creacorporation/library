//----------------------------------------------------------------------------
// ���W�X�g���A�N�Z�X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#include "mStandard.h"
#include "mRegistry.h"
#include "General/mErrorLogger.h"
#include <memory>

mRegistry::mRegistry()
{
	MyKey = (HKEY)INVALID_HANDLE_VALUE;
}

mRegistry::~mRegistry()
{
	Close();
}

bool mRegistry::Open( ParentKey parent , const WString& path , BOOL access_write )
{
	Close();

	HKEY parent_handle;
	switch( parent )
	{
	case ParentKey::CURRENT_USER:
		parent_handle = HKEY_CURRENT_USER;
		break;
	case ParentKey::LOCAL_MACHINE:
		parent_handle = HKEY_LOCAL_MACHINE;
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�e�L�[�̎w�肪�s���ł�" );
		return false;
	}

	DWORD priv = KEY_READ;
	if( access_write )
	{
		priv |= KEY_WRITE;
	}

	LONG result = RegCreateKeyEx( 
		parent_handle ,				// �J���Ă���e�L�[�̃n���h��
		path.c_str(),				// �J���ׂ��T�u�L�[�̖��O
		0,							// �\��ς�
		0,							// �N���X�̕�����
		REG_OPTION_NON_VOLATILE,	// ���ʂȃI�v�V����
		priv,						// �Z�L�����e�B�A�N�Z�X�}�X�N
		0,							// �p���̎w��
		&MyKey,						// �J�����Ƃɐ��������T�u�L�[�̃n���h��
		0 );						// �������ǂ����������l���i�[�����ϐ�

	if( result != ERROR_SUCCESS )
	{
		MyKey = (HKEY)INVALID_HANDLE_VALUE;
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[�̃n���h�����擾�ł��܂���ł���" );
		return false;
	}

	return true;
}

bool mRegistry::Close( void )
{
	if( MyKey != INVALID_HANDLE_VALUE )
	{
		RegCloseKey( MyKey );
	}
	return true;
}

DWORD mRegistry::GetDword( const WString &entry , DWORD def_value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return def_value;
	}

	//�l�ǂݎ��
	DWORD data_type;
	DWORD data_value;
	DWORD data_size = sizeof( data_value );
	LONG result = RegQueryValueEx( MyKey , entry.c_str() , 0 , &data_type , (LPBYTE)&data_value , &data_size );

	//�`�F�b�N
	if( result != ERROR_SUCCESS )	//�G���[�H
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̓ǂݎ�肪���s���܂���" , entry );
		return def_value;
	}
	if( data_type != REG_DWORD )	//�^�̃`�F�b�N
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̌^���Ⴂ�܂�" , entry );
		return def_value;
	}
	if( data_size != sizeof( data_value ) )	//�T�C�Y�̃`�F�b�N
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̃f�[�^�T�C�Y���Ⴂ�܂�" , entry );
		return def_value;
	}
	return data_value;
}

bool mRegistry::SetDword( const WString &entry , DWORD value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return false;
	}

	//��������
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_DWORD , (LPCBYTE)&value , sizeof( value ) ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���ւ̏������݂����s���܂���" , entry );
	}
	return true;
}

ULONGLONG mRegistry::GetQword( const WString &entry , ULONGLONG def_value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return def_value;
	}

	//�l�ǂݎ��
	DWORD data_type;
	ULONGLONG data_value;
	DWORD data_size = sizeof( data_value );
	LONG result = RegQueryValueEx( MyKey , entry.c_str() , 0 , &data_type , (LPBYTE)&data_value , &data_size );

	//�`�F�b�N
	if( result != ERROR_SUCCESS )	//�G���[�H
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̓ǂݎ�肪���s���܂���" , entry );
		return def_value;
	}
	if( data_type != REG_QWORD )	//�^�̃`�F�b�N
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̌^���Ⴂ�܂�" , entry );
		return def_value;
	}
	if( data_size != sizeof( data_value ) )	//�T�C�Y�̃`�F�b�N
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̃f�[�^�T�C�Y���Ⴂ�܂�" , entry );
		return def_value;
	}
	return data_value;
}

bool mRegistry::SetQword( const WString &entry , ULONGLONG value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return false;
	}

	//��������
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_QWORD , (LPCBYTE)&value , sizeof( value ) ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���ւ̏������݂����s���܂���" , entry );
	}
	return true;
}

//���W�X�g������ϒ��f�[�^��ǂݎ��
static bool ReadInternal( HKEY key , const WString& entry , WString& retData , DWORD& data_type )
{
	DWORD required_size = 0;
	if( RegQueryValueEx( key , entry.c_str() , 0 , nullptr , nullptr , &required_size ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̕�����f�[�^�̃T�C�Y���擾�ł��܂���" , entry );
		return false;
	}

	std::unique_ptr<BYTE> ptr( mNew BYTE[ required_size ] );
	if( RegQueryValueEx( key , entry.c_str() , 0 , &data_type , ptr.get() , &required_size ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̕�����f�[�^���擾�ł��܂���" , entry );
		return false;
	}

	retData.resize( required_size / sizeof( wchar_t ) );
	MoveMemory( const_cast<wchar_t*>( retData.data() ) , ptr.get() , required_size );
	return true;
}


//1�s�ǂݎ��
WString mRegistry::GetString( const WString& entry , const WString& def_value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return def_value;
	}

	//������ǂݎ��
	DWORD data_type;
	WString result_string;
	if( !ReadInternal( MyKey , entry , result_string , data_type ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̃f�[�^��ǂݎ��܂���" , entry );
		return def_value;
	}
	//�l�̎�ʂ�������ł��邩���m�F����
	if( data_type != REG_SZ )
	{
		//������ł͂Ȃ�
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̌^���Ⴂ�܂�" , entry );
		return def_value;
	}

	return result_string;
}

//1�s�����o��
bool mRegistry::SetString( const WString& entry , const WString& value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return false;
	}

	//��������
	DWORD write_size = (DWORD)( sizeof( wchar_t ) * ( value.size() + 1 ) );	//+1��NULL��
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_SZ , (LPCBYTE)value.c_str() , write_size ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���ւ̏������݂����s���܂���" , entry );
		return false;
	}
	return true;
}

//�����s�ǂݎ��
bool mRegistry::GetMultiString( const WString& entry , WStringVector& ret_value )
{
	ret_value.clear();

	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return false;
	}

	//������ǂݎ��
	DWORD data_type;
	WString result_string;
	if( !ReadInternal( MyKey , entry , result_string , data_type ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̃f�[�^��ǂݎ��܂���" , entry );
		return false;
	}
	//�l�̎�ʂ�������ł��邩���m�F����
	if( data_type != REG_MULTI_SZ )
	{
		//������ł͂Ȃ�
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���̌^���Ⴂ�܂�" , entry );
		return false;
	}

	//���ʂ��p�[�X����
	ReadDoubleNullString( result_string , ret_value );
	return true;
}

//�����s�����o��
bool mRegistry::SetMultiString( const WString& entry , const WStringVector& value )
{
	//�L�[���J���Ă��Ȃ��ꍇ�̓G���[
	if( MyKey == INVALID_HANDLE_VALUE )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���L�[���J����Ă��܂���" , entry );
		return false;
	}

	WString str;
	MakeDoubleNullString( value , str );

	//��������
	if( RegSetValueEx( MyKey , entry.c_str() , 0 , REG_MULTI_SZ , (LPCBYTE)str.data() , (DWORD)( str.size() * sizeof( wchar_t ) ) ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���ւ̏������݂����s���܂���" , entry );
		return false;
	}
	return true;
}

//�G���g���̍폜
bool mRegistry::DeleteEntry( const WString& entry )
{
	if( RegDeleteValue( MyKey , entry.c_str() ) != ERROR_SUCCESS )
	{
		RaiseError( g_ErrorLogger , 0 , L"���W�X�g���G���g���̍폜�����s���܂���" , entry );
		return false;
	}
	return true;
}

bool mRegistry::EnumValues( WStringDeque& retValues )
{
	DWORD index = 0;
	bool result = true;

	wchar_t buffer[ 256 ];
	wchar_t* dynamic_buffer = nullptr;
	wchar_t* buffer_ptr = buffer;
	DWORD buffer_size = (DWORD)array_count_of( buffer );

	retValues.clear();

	while( 1 )
	{
		DWORD result_size = buffer_size;
		LSTATUS rc = RegEnumValueW( MyKey , index , buffer_ptr , &result_size , nullptr , nullptr , nullptr , nullptr );

		if( rc == ERROR_SUCCESS )
		{
			retValues.push_back( buffer_ptr );
		}
		else if( rc == ERROR_NO_MORE_ITEMS )
		{
			break;
		}
		else if( rc == ERROR_MORE_DATA )
		{
			if( dynamic_buffer )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"���W�X�g���L�[�̗񋓂Ńo�b�t�@���s�����܂���" );
				result = false;
				break;
			}
			buffer_size = 32767;
			dynamic_buffer = mNew wchar_t[ buffer_size ];	//���W�X�g���L�[�̒��������ő�
			buffer_ptr = dynamic_buffer;
			continue;
		}
		else
		{
			result = false;
			break;
		}
		index++;
	}

	mDelete[] dynamic_buffer;
	return result;
}





