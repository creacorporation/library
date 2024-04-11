//----------------------------------------------------------------------------
// INI�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#define MINITFILE_CPP_COMPINLING
#include "mInitFile.h"
#include "General/mRegexp.h"

const mInitFileSection mInitFile::EmptySection;

mInitFile::mInitFile()
{
}

mInitFile::~mInitFile()
{
}

mInitFile::mInitFile( const mInitFile& src )
{
	*this = src;
}

mInitFile& mInitFile::operator=( const mInitFile& src )
{
	MySectionDataMap = src.MySectionDataMap;
	return *this;
}

//�ǂݍ��ݍς݂̃f�[�^��j������
void mInitFile::Clear( void )noexcept
{
	//���ݕێ����Ă���f�[�^���L��Δj������B�����ċ󕶎���̃Z�N�V������ǉ�����B
	MySectionDataMap.clear();
	MySectionDataMap[ L"" ];	//�����second�����f�t�H���g�R���X�g���N�^�ō쐬�����
	return;
}

//INI�t�@�C����ǂݍ��݂܂�
bool mInitFile::Read( const WString& filename , bool tolower , Encode enc )noexcept
{
	//�t�@�C�����J���Ƃ��̏��
	mFileReadStream::Option opt;
	opt.AccessRead = true;
	opt.AccessWrite = false;
	opt.Mode = mFileReadStream::CreateMode::OpenExisting;
	opt.ShareRead = true;
	opt.ShareWrite = false;
	opt.Path = filename;

	//�t�@�C�����J��
	mFileReadStream fp;
	if( !fp.Open( opt ) )
	{
		//�t�@�C�����J���Ȃ�����
		RaiseError( g_ErrorLogger , 0 , L"Open file failed" , filename );
		return false;
	}

	//�G���R�[�h�̎w��
	if( !fp.SetEncode( enc ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�G���R�[�h�̎w��Ɏ��s���܂���" , filename );
		return false;
	}

	//�t�@�C�����J�����̂œǂݍ��ݏ���������
	return Read( fp , tolower );
}

bool mInitFile::Read( mFileReadStream& fp , bool tolower )
{
	mRegexpW InitFileCommentPattern( LR"(^\s*(;|$))" );
	mRegexpW InitFileSectionPattern1( LR"(^\[(\S+)\])" );
	mRegexpW InitFileSectionPattern2( LR"(^<(\S+)>)" );
	mRegexpW InitFileKeyValsPattern1( LR"(^\s*([A-Za-z0-9._()@#]+)\s*(\[([0-9]+)\])*\s*=\s*(.*?)\s*(;|$))" );
	mRegexpW InitFileKeyValsPattern2( LR"(^\s*(.*?)\s*(;|$))" );
	mRegexpW InitFileKeyValsPattern3( LR"(^\s*\.([A-Za-z0-9._()@#]+)\s*(\[([0-9]+)\])*\s*=\s*(.*?)\s*(;|$))" );

	//���݂̃Z�N�V������ݒ�
	//���󕶎���̃Z�N�V�������Ȃ��\��������̂ŁA��Ƀ^�b�`���Ă����B
	MySectionDataMap[ L"" ]; //�����second�����f�t�H���g�R���X�g���N�^�ō쐬�����
	SectionDataMap::iterator sect_itr = MySectionDataMap.find( L"" );

	//�t�@�C�����P�s���ǂݎ���ē��e�𔻕ʂ��A�f�[�^��o�^����B
	DWORD line_count = 0;			//�s�ԍ�
	bool line_read_mode = false;	//false=[]�̃Z�N�V���� true=<>�̃Z�N�V����

	WString line;				//�t�@�C������ǂݎ�����s

	mInitFileSection::Key key;	//=�̍����c�L�[
	WString val;				//=�̉E���c�l
	while( !fp.IsEOF() )
	{
		if( !fp.ReadLine( line ) )
		{
			continue;
		}

		//�e�s�̔���
		if( InitFileCommentPattern.Match( line ) )
		{
			//�R�����g or ��s�Ȃ̂Ŗ���
			continue;
		}
		else if( InitFileSectionPattern1.Match( line ) )
		{
			//�Z�N�V������( []�ň͂܂ꂽ�s )�̏���
			WString current_section;
			if( tolower )
			{
				current_section = ToLower( InitFileSectionPattern1.Submatch()[ 1 ] );
			}
			else
			{
				current_section = InitFileSectionPattern1.Submatch()[ 1 ];
			}

			//�����I�u�W�F�N�g�̃N���A
			MySectionDataMap[ current_section ].MySectionName = current_section;
			sect_itr = MySectionDataMap.find( current_section );
			sect_itr->second.MyKeyValueMap.clear();
			sect_itr->second.MySectionType = mInitFileSection::SECTIONTYPE_NORMAL;

			line_read_mode = false;
			line_count = 0;
			continue;
		}
		else if( InitFileSectionPattern2.Match( line ) )
		{
			//�Z�N�V������( <>�ň͂܂ꂽ�s )�̏���
			WString current_section;
			if( tolower )
			{
				current_section = ToLower( InitFileSectionPattern2.Submatch()[ 1 ] );
			}
			else
			{
				current_section = InitFileSectionPattern2.Submatch()[ 1 ];
			}

			//�����I�u�W�F�N�g�̃N���A
			MySectionDataMap[ current_section ].MySectionName = current_section;
			sect_itr = MySectionDataMap.find( current_section );
			sect_itr->second.MyKeyValueMap.clear();
			sect_itr->second.MySectionType = mInitFileSection::SECTIONTYPE_ROWBASE;

			line_read_mode = true;
			line_count = 0;
			continue;
		}

		//�L�[�ƒl�̃y�A�̏���
		if( line_read_mode )
		{
			//�s�P�ʂ̓ǂݍ��݂̏ꍇ
			if( InitFileKeyValsPattern3.Match( line ) )
			{
				key.key = L"";
				key.index = line_count;

				if( tolower )
				{
					key.subkey = ToLower( InitFileKeyValsPattern3.Submatch()[ 1 ] );
				}
				else
				{
					key.subkey = InitFileKeyValsPattern3.Submatch()[ 1 ];
				}
				val = InitFileKeyValsPattern3.Submatch()[ 4 ];
			}
			else if( InitFileKeyValsPattern2.Match( line ) )
			{
				key.key = L"";
				key.index = line_count;
				key.subkey = L"";
				val = InitFileKeyValsPattern2.Submatch()[ 1 ];
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"Ini�t�@�C���̏������Ⴂ�܂�" , line );
				continue;
			}
			line_count++;
		}
		else
		{
			//�L�[=�l �̌`�ɂȂ��Ă���s�H
			if( InitFileKeyValsPattern1.Match( line ) )
			{
				//=�̍����c�L�[
				WString key_str = InitFileKeyValsPattern1.Submatch()[ 1 ];
				if( key_str[ 0 ] == L'.' )	//.�Ŏn�܂�H
				{
					key.index = 0;
					if( key_str.size() == 1 )
					{
						//.�����̏ꍇ
						key.subkey = L"";
					}
					else if( tolower )
					{
						key.subkey = ToLower( key_str.substr( 1 ) );
					}
					else
					{
						key.subkey = key_str.substr( 1 );
					}
				}
				else
				{
					if( tolower )
					{
						key.key = ToLower( key_str );
					}
					else
					{
						key.key = key_str;
					}

					WString index_str = InitFileKeyValsPattern1.Submatch()[ 3 ];
					if( wchar_sscanf( index_str.c_str() , L"%d" , &key.index ) < 0 )
					{
						key.index = 0;
					}
					key.subkey = L"";
				}
				//=�̉E���c�l
				val = InitFileKeyValsPattern1.Submatch()[ 4 ];
			}
			else
			{
				RaiseError( g_ErrorLogger , 0 , L"Ini�t�@�C���̏������Ⴂ�܂�" , line );
				continue;
			}
		}

		//���łɑ��݂���L�[�H
		mInitFileSection::KeyValueMap::iterator val_itr = sect_itr->second.MyKeyValueMap.find( key );
		if( val_itr == sect_itr->second.MyKeyValueMap.end() )
		{
			//�V�K�̃L�[�Ȃ̂Œǉ�
			sect_itr->second.MyKeyValueMap.insert( mInitFileSection::KeyValueMap::value_type( key , val ) );
		}
		else
		{
			//�����̃L�[�Ȃ̂ŏ㏑��
			val_itr->second = val;
		}
	}
	return true;
}

//INI�t�@�C�����������݂܂�
bool mInitFile::Write( const WString& filename )
{
	//�t�@�C�����J���Ƃ��̏��
	mFileWriteStream::Option opt;
	opt.AccessRead = false;
	opt.AccessWrite = true;
	opt.Mode = mFileReadStream::CreateMode::CreateAlways;
	opt.ShareRead = true;
	opt.ShareWrite = false;
	opt.Path = filename;

	//�t�@�C�����J��
	mFileWriteStream fp;
	if( !fp.Open( opt ) )
	{
		//�t�@�C�����J���Ȃ�����
		RaiseError( g_ErrorLogger , 0 , L"Open file failed" , filename );
		return false;
	}

	//�t�@�C�����J�����̂ŏ������ݏ���������
	return Write( fp );
}

//INI�t�@�C�����������݂܂�
bool mInitFile::Write( mFileWriteStream& fp )
{
	bool result = true;

	//�Z�N�V�����ꗗ�̍쐬
	WStringDeque sectionlist;
	if( !GetSectionList( sectionlist ) )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����ꗗ���쐬�ł��܂���ł���" );
		return false;
	}

	//�e�Z�N�V�����̃f�[�^���o��
	for( WStringDeque::const_iterator itr = sectionlist.begin() ; itr != sectionlist.end() ; itr++ )
	{
		const mInitFileSection* section = GetSection( *itr );
		if( section == nullptr )
		{
			RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����̃|�C���^���擾�ł��܂���ł���" , *itr );
			result = false;
		}
		else if( 0 < section->GetCount() )
		{
			//1�ȏ�L�[���܂�ł���ꍇ�̂ݏo��

			if( !section->Write( fp ) )
			{
				RaiseAssert( g_ErrorLogger , 0 , L"�Z�N�V�����̏o�͂����s���܂���" , *itr );
			}
			fp.WriteString( L"\r\n" );
		}
		else
		{
			;	//nothing to do
		}
	}
	return result;
}

//�Z�N�V�������擾���܂�
const mInitFileSection* mInitFile::GetSection( const WString& section )const noexcept
{
	SectionDataMap::const_iterator itr = MySectionDataMap.find( section );
	if( itr == MySectionDataMap.end() )
	{
		return nullptr;
	}
	return &itr->second;
}

//�Z�N�V�������擾���܂�
mInitFileSection* mInitFile::GetSectionForModification( const WString& section )noexcept
{
	SectionDataMap::iterator itr = MySectionDataMap.find( section );
	if( itr == MySectionDataMap.end() )
	{
		return nullptr;
	}
	return &itr->second;
}

//�Z�N�V������ǉ����܂�
mInitFileSection* mInitFile::CreateNewSection( const WString& section )noexcept
{
	MySectionDataMap[ section ];
	return GetSectionForModification( section );
}

//�Z�N�V�������폜���܂�
bool mInitFile::DeleteSection( const WString& section )noexcept
{
	SectionDataMap::iterator itr = MySectionDataMap.find( section );
	if( itr == MySectionDataMap.end() )
	{
		return false;
	}

	MySectionDataMap.erase( itr );
	return true;
}

//�Z�N�V���������݂��邩�𔻒肵�܂�
bool mInitFile::IsExistSection( const WString& section )const noexcept
{
	return MySectionDataMap.count( section ) != 0;
}

//�Z�N�V�������̈ꗗ���쐬���܂�
bool mInitFile::GetSectionList( WStringDeque& retList )const
{
	retList.clear();

	for( SectionDataMap::const_iterator itr = MySectionDataMap.begin() ; itr != MySectionDataMap.end() ; itr++ )
	{
		retList.push_back( itr->first );
	}
	return true;
}


