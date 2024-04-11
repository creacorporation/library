//----------------------------------------------------------------------------
// INI�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#define MINITFILESECTION_CPP_COMPINLING
#include "mInitFileSection.h"
#include <unordered_set>

mInitFileSection::mInitFileSection()
{
	MySectionType = SectionType::SECTIONTYPE_NORMAL;
	MyIsModified = false;
}

mInitFileSection::~mInitFileSection()
{
}

mInitFileSection::mInitFileSection( const mInitFileSection& src )
{
	*this = src;
}

mInitFileSection& mInitFileSection::operator=( const mInitFileSection& src )
{
	MyIsModified = src.MyIsModified;
	MyKeyValueMap = src.MyKeyValueMap;
	MySectionName = src.MySectionName;
	MySectionType = src.MySectionType;
	return *this;
}


//�w�肵���L�[�����݂��邩�Ԃ�
bool mInitFileSection::IsValidKey( const WString& key )const noexcept
{
	return IsValidKey( key , 0 , L"" );
}

//�w�肵���L�[�����݂��邩�Ԃ�
bool mInitFileSection::IsValidKey( const WString& key , INT index )const noexcept
{
	return IsValidKey( key , index , L"" );
}

//�w�肵���L�[�����݂��邩�Ԃ�
bool mInitFileSection::IsValidKey( const WString& key , INT index , const WString& subkey )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;
	return MyKeyValueMap.count( tmpkey ) != 0;
}

//�w�肵���L�[�����݂��邩�Ԃ�
bool mInitFileSection::IsValidKey( const WString& key , const WString& subkey )const noexcept
{
	return IsValidKey( key , 0 , subkey );
}

//�L�[�̒l��32�r�b�gINT�̒l�Ƃ��ēǂݎ��
INT mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , INT defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	INT val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%x" , &val ) == 1 )
	{
		//0x�Ŏn�܂��Ă���ꍇ��16�i���Ƃ��ēǂݎ��
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%d" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}

//�L�[�̒l��32�r�b�gLONG�̒l�Ƃ��ēǂݎ��
//key : �ǂݎ�肽���L�[
//defvalue : �L�[��ǂݎ��Ȃ������ꍇ�̒l
//ret : �ǂݎ�����l�B�ǂݎ��Ȃ������ꍇ��defvalue�̒l
LONG mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , LONG defvalue , bool* retIsReadable )const noexcept
{
	static_assert( sizeof( INT ) == sizeof( LONG ) , "sizeof int is not equal to sizeof long" );
	return (LONG)GetValue( key , index , subkey , (INT)defvalue );
}

//�L�[�̒l��64�r�b�gINT�̒l�Ƃ��ēǂݎ��
LONGLONG mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , LONGLONG defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	LONGLONG val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%llx" , &val ) == 1 )
	{
		//0x�Ŏn�܂��Ă���ꍇ��16�i���Ƃ��ēǂݎ��
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%lld" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

}

//�L�[�̒l��32�r�b�gUINT�Ƃ��ēǂݎ��
UINT mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , UINT defvalue , bool* retIsReadable )const noexcept
{
	static_assert( sizeof( UINT ) == sizeof( DWORD ) , "unexpected data size" );
	return GetValue( key , index , subkey , static_cast<DWORD>( defvalue ) );
}

//�L�[�̒l��32�r�b�gDWORD�Ƃ��ēǂݎ��
DWORD mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , DWORD defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	DWORD val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%x" , &val ) == 1 )
	{
		//0x�Ŏn�܂��Ă���ꍇ��16�i���Ƃ��ēǂݎ��
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%u" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}

//�L�[�̒l��64�r�b�gUINT�Ƃ��ēǂݎ��
ULONGLONG mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , ULONGLONG defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	ULONGLONG val;
	if( wchar_sscanf( itr->second.c_str() , L"0x%llx" , &val ) == 1 )
	{
		//0x�Ŏn�܂��Ă���ꍇ��16�i���Ƃ��ēǂݎ��
		SetReadable( retIsReadable );
		return val;
	}
	else if( wchar_sscanf( itr->second.c_str() , L"%llu" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}

//�L�[�̒l��64�r�b�gFLOAT�Ƃ��ēǂݎ��
FLOAT mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , FLOAT defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	FLOAT val;
	if( wchar_sscanf( itr->second.c_str() , L"%f" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	ResetReadable( retIsReadable );
	return defvalue;
}

//�L�[�̒l��64�r�b�gDOUBLE�Ƃ��ēǂݎ��
DOUBLE mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , DOUBLE defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	DOUBLE val;
	if( wchar_sscanf( itr->second.c_str() , L"%lf" , &val ) == 1 )
	{
		SetReadable( retIsReadable );
		return val;
	}
	ResetReadable( retIsReadable );
	return defvalue;
}

//�L�[�̒l��UNICODE������Ƃ��ēǂݎ��
WString mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const WString& defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	SetReadable( retIsReadable );
	return itr->second;
}

//�L�[�̒l��ASCII������Ƃ��ēǂݎ��
AString mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const AString& defvalue , bool* retIsReadable )const noexcept
{
	//�����I�ɂ�Unicode�Ȃ̂ŁA��UUnicode�Ƃ��ēǂݎ���Ă���AASCII������ɕϊ�����B

	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	SetReadable( retIsReadable );
	return WString2AString( itr->second );
}

//�L�[�̒l��UNICODE������Ƃ��ēǂݎ��
const wchar_t* mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const wchar_t* defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
	SetReadable( retIsReadable );
	return itr->second.c_str();
}

//�L�[�̒l��F�Ƃ��ēǂݎ��
RGBQUAD mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�ݒ�l������̂ŁA�ǂݎ������݂܂��B
	DWORD r = 0;	//��
	DWORD g = 0;	//��
	DWORD b = 0;	//��
	DWORD a = 0;	//�A���t�@
	if( wchar_sscanf( itr->second.c_str() , L"#%02X%02X%02X%02X" , &r , &g , &b , &a ) < 3 )
	{
		//�ǂݎ��Ȃ������̂ŁA�G���[�ɂ��܂��B
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�ǂ߂��̂ŁA���ꂪ���ʁB
	RGBQUAD result;
	result.rgbBlue = (BYTE)b;
	result.rgbGreen = (BYTE)g;
	result.rgbRed = (BYTE)r;
	result.rgbReserved = (BYTE)a;

	SetReadable( retIsReadable );
	return result;
}

//�u�[���l�Ƃ��āA�L�[�̒l��ǂݎ��
bool mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , bool defvalue , bool* retIsReadable )const noexcept
{
	const mInitFileSection::LookupValues< bool > BooleanValues =
	{
		{ L"yes"	, true  },
		{ L"true"	, true  },
		{ L"no"		, false },
		{ L"false"	, false },
	};
	return GetValue( key , BooleanValues , defvalue , retIsReadable );
}

mInitFileSection::Hexdecimal mInitFileSection::GetValue( const WString& key , INT index , const WString& subkey , Hexdecimal defvalue , bool* retIsReadable )const noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	//�L�[�ɑΉ����镶������擾
	KeyValueMap::const_iterator itr = MyKeyValueMap.find( tmpkey );
	if( itr == MyKeyValueMap.end() )
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}

	//�擾����������𐔒l�ɕϊ�����
	DWORD val;
	if( wchar_sscanf( itr->second.c_str() , L"%x" , &val ) == 1 )
	{
		//0x�Ŏn�܂��Ă���ꍇ��16�i���Ƃ��ēǂݎ��
		SetReadable( retIsReadable );
		return Hexdecimal( val );
	}
	else
	{
		ResetReadable( retIsReadable );
		return defvalue;
	}
}


//�w��̃L�[��32�r�b�gINT�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , INT newval )noexcept
{
	WString value;
	sprintf( value , L"%d" , newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��32�r�b�gLONG�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , LONG newval )noexcept
{
	return SetValue( key , index , subkey , (INT)newval );
}

//�w��̃L�[��64�r�b�gINT�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , LONGLONG newval )noexcept
{
	WString value;
	sprintf( value , L"%lld" , newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��32�r�b�gUINT�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , DWORD newval )noexcept
{
	WString value;
	sprintf( value , L"%u" , newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��64�r�b�gUINT�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , ULONGLONG newval )noexcept
{
	WString value;
	sprintf( value , L"%llu" , newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��32�r�b�gFLOAT�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , FLOAT newval )noexcept
{
	WString value;
	sprintf( value , L"%f" , newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��64�r�b�gDOUBLE�̒l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , DOUBLE newval )noexcept
{
	WString value;
	sprintf( value , L"%lf" , newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��UNICODE�������ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const WString& newval )noexcept
{
	//�ݒ�l���擾
	Key tmpkey;
	tmpkey.key = key;
	tmpkey.index = index;
	tmpkey.subkey = subkey;

	MyKeyValueMap[ tmpkey ] = newval;
	MyIsModified = true;
	return true;
}

//�w��̃L�[��ASCII�������ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const AString& newval )noexcept
{
	WString value = AString2WString( newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��UNICODE�������ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const wchar_t* newval )noexcept
{
	WString value( newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��ASCII�������ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const char* newval )noexcept
{
	AString value( newval );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[�ɋP�x�l��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const RGBQUAD& newval )noexcept
{
	WString value;
	sprintf( value , L"#%02X%02X%02X%02X" , newval.rgbRed , newval.rgbGreen , newval.rgbBlue , newval.rgbReserved );
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��GDI���W��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , const mWindowPosition::POSITION& newval )noexcept
{
	WString value;
	if( newval.offset == 0 )
	{
		sprintf( value , L"%0.0f" , newval.rate );
	}
	else
	{
		sprintf( value , L"%0.0f+%d" , newval.rate , newval.offset );
	}
	return SetValue( key , index , subkey , value );
}

//�w��̃L�[��Window���W��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , const mWindowPosition::WindowPosition& newval )noexcept
{
	bool result = true;
	result &= SetValue( key , L"left" , newval.left );
	result &= SetValue( key , L"right" , newval.right );
	result &= SetValue( key , L"top" , newval.top );
	result &= SetValue( key , L"bottom" , newval.bottom );

	return result;
}

//�w��̃L�[��Window���W��ݒ肷��
bool mInitFileSection::SetValue( const WString& key , INT index , const mWindowPosition::WindowPosition& newval )noexcept
{
	bool result = true;
	result &= SetValue( key , index , L"left" , newval.left );
	result &= SetValue( key , index , L"right" , newval.right );
	result &= SetValue( key , index , L"top" , newval.top );
	result &= SetValue( key , index , L"bottom" , newval.bottom );

	return result;
}

//�w��̃L�[�Ƀu�[���l����������
bool mInitFileSection::SetValue( const WString& key , INT index , const WString& subkey , bool newval )noexcept
{
	WString value = ( newval ) ? ( L"true" ) : ( L"false" );
	return SetValue( key , index , subkey , value );
}

//���݂���L�[�̈ꗗ��Ԃ�
bool mInitFileSection::GetKeyList( WStringDeque& retList )const
{
	retList.clear();

	//���łɏo�����Ă���L�[�̈ꗗ
	std::unordered_set< WString > keytable;

	//�L�[�̃X�L����
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( keytable.count( itr->first.key ) == 0 )
		{
			retList.push_back( itr->first.key );
			keytable.insert( itr->first.key );
		}
	}

	//���בւ�
	std::sort( retList.begin() , retList.end() );
	return true;
}

//���݂���L�[�̈ꗗ��Ԃ�
bool mInitFileSection::GetKeyList( KeyList& retList )const
{
	retList.clear();

	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		retList.push_back( itr->first );
	}

	std::sort( retList.begin() , retList.end() );
	return true;
}

//���݂���L�[�̈ꗗ��Ԃ�
//�����̊֐��́A�w�肵���L�[�̃T�u�L�[��Ԃ��܂�
bool mInitFileSection::GetKeyList( const WString& key , KeyList& retList )const
{
	retList.clear();

	//�L�[�̃X�L����
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( key == itr->first.key )
		{
			retList.push_back( itr->first );
		}
	}

	//���בւ�
	std::sort( retList.begin() , retList.end() );
	return true;
}

bool mInitFileSection::GetIndexRange( const WString& key , INT& ret_min_index , INT& ret_max_index )const
{
	ret_min_index = 0;
	ret_max_index = 0;
	bool result = false;

	//�L�[�̃X�L����
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( key == itr->first.key )
		{
			if( itr->first.index < ret_min_index )
			{
				ret_min_index = itr->first.index;
			}
			else if( ret_max_index < itr->first.index )
			{
				ret_max_index = itr->first.index;
			}
			result = true;
		}
	}

	return true;
}

//���݂���L�[�Ɋ܂܂��C���f�b�N�X�̃��X�g��Ԃ�
//retList : �C���f�b�N�X�̈ꗗ���i�[����
//ret : �������^�A���s���U(�w�肵���L�[�����݂��Ȃ��ꍇ�Ȃ�)
bool mInitFileSection::GetIndexList( const WString& key , IndexList& retList )const
{
	retList.clear();
	bool result = false;

	//�L�[�̃X�L����
	for( KeyValueMap::const_iterator itr = MyKeyValueMap.begin() ; itr != MyKeyValueMap.end() ; itr++ )
	{
		if( key == itr->first.key )
		{
			retList.push_back( itr->first.index );
			result = true;
		}
	}
	return true;
}

//�t�@�C���ɃZ�N�V�����̓��e���o�͂���
bool mInitFileSection::Write( mFileWriteStream& fp )const
{
	bool result = true;
	WString str;

	//�^�C�g�����o��
	switch( MySectionType )
	{
	case SectionType::SECTIONTYPE_NORMAL:
		sprintf( str , L"[%ls]\r\n" , MySectionName.c_str() );
		break;
	case SectionType::SECTIONTYPE_ROWBASE:
		sprintf( str , L"<%ls>\r\n" , MySectionName.c_str() );
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����`�����s���ł�" );
		sprintf( str , L"[%ls] ;section type error\r\n" , MySectionName.c_str() );
		break;
	}
	fp.WriteString( str );

	//�L�[�̈ꗗ���쐬�B�L�[���\�[�g���邽�߁AGetKeyList���ĂԁB
	KeyList keylist;
	if( !GetKeyList( keylist ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�L�[�̃��X�g���擾�ł��܂���ł���" );
		return false;
	}

	//�e���ڂ��o��
	for( KeyList::const_iterator itr = keylist.begin() ; itr != keylist.end() ; itr++ )
	{
		KeyValueMap::const_iterator entry = MyKeyValueMap.find( *itr );
		if( entry == MyKeyValueMap.end() )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�ݒ�l���s���s���ɂȂ�܂���" , itr->key );
			result = false;
		}
		else if( MySectionType == SectionType::SECTIONTYPE_NORMAL )
		{
			if( itr->subkey == L"" )
			{
				//�T�u�L�[�ł͂Ȃ��ꍇ
				if( itr->index != 0 )
				{
					sprintf( str , L"%ls[%d] = %ls\r\n" , itr->key.c_str() , itr->index , entry->second.c_str() );
				}
				else
				{
					sprintf( str , L"%ls = %ls\r\n" , itr->key.c_str() , entry->second.c_str() );
				}
			}
			else
			{
				//�T�u�L�[�̏ꍇ
				sprintf( str , L".%ls = %ls\r\n" , itr->subkey.c_str() , entry->second.c_str() );
			}

			if( !fp.WriteString( str ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�t�@�C���ւ̏������݂����s���܂���" , itr->key );
				result = false;
			}
		}
		else if( MySectionType == SectionType::SECTIONTYPE_ROWBASE )
		{
			if( itr->subkey == L"" )
			{
				sprintf( str , L"%ls\r\n" , entry->second.c_str() );
			}
			else
			{
				sprintf( str , L".%ls = %ls\r\n" , itr->subkey.c_str() , entry->second.c_str() );
			}

			if( !fp.WriteString( str ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�t�@�C���ւ̏������݂����s���܂���" , itr->key );
				result = false;
			}
		}
		else
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����`�����s���ł�" );
			return false;
		}
	}

	//�o�͂��ł����̂ŁA�X�V�ς݃t���O���N���A
	//�C���X�^���X��const�ɂȂ��Ă��邪�X�V����
	const_cast< mInitFileSection* >( this )->MyIsModified = false;

	return result;
}

//���݂���L�[�̐����擾����
DWORD mInitFileSection::GetCount( void )const
{
	return static_cast<DWORD>( MyKeyValueMap.size() );
}

bool mInitFileSection::SetSectionName( const WString& newname )
{
	MySectionName = newname;
	return true;
}

const WString& mInitFileSection::GetSectionName( void )const
{
	return MySectionName;
}

//�Z�N�V�����`����ύX����
// newtype : �V�����`��
// ret : �������^
bool mInitFileSection::SetSectionType( SectionType newtype )
{
	switch( newtype )
	{
	case SECTIONTYPE_NORMAL:
	case SECTIONTYPE_ROWBASE:
		MySectionType = newtype;
		return true;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����`�����s���ł�" );
		break;
	}

	return false;
}

//�Z�N�V�����`�����擾����
// ret : �Z�N�V�����`��
mInitFileSection::SectionType mInitFileSection::GetSectionType( void )const
{
	switch( MySectionType )
	{
	case SECTIONTYPE_NORMAL:
	case SECTIONTYPE_ROWBASE:
		break;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����`�����s���ł������̂ŋ����I�ɕ␳����܂���" );
		const_cast<SectionType>( MySectionType ) = SECTIONTYPE_NORMAL;
		break;
	}

	return MySectionType;
}

//�X�V����Ă��邩�ۂ���Ԃ�
bool mInitFileSection::IsModified( void )const
{
	return MyIsModified;
}

