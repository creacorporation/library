//----------------------------------------------------------------------------
// INI�t�@�C���ǂݍ��ݑ���
// Copyright (C) 2013,2016 Fingerling. All rights reserved. 
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------

#ifndef MINITFILE_H_INCLUDED
#define MINITFILE_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "General/mErrorLogger.h"
#include "General/mFileReadStream.h"
#include "General/mFileWriteStream.h"
#include "General/mInitFileSection.h"
#include <unordered_map>

class mInitFile
{
public:
	mInitFile();
	virtual ~mInitFile();
	mInitFile( const mInitFile& src );
	mInitFile& operator=( const mInitFile& src );

	//�ǂݍ��ݍς݂̃f�[�^��j������
	void Clear( void )noexcept;

	//Init�t�@�C���ǂݏ������̃G���R�[�h
	using Encode = mFileReadStream::Encode;

	//INI�t�@�C����ǂݍ��݂܂�
	//���ł�INI�t�@�C����ǂݍ���ł���ꍇ�́A�����̃f�[�^�ƕ������܂��B
	//filename : �ǂݍ��ރt�@�C����
	//tolower : �L�[���������ɕϊ����ēǂݍ��݂܂�
	//ret : ���������true
	//���G���R�[�h���w�肵�Ȃ��ꍇ�AUTF16(BOM�Ȃ�)�Ƃ��ēǂݍ��݂܂�
	bool Read( const WString& filename , bool tolower = false , Encode enc = Encode::ENCODE_ASIS )noexcept;

	//INI�t�@�C����ǂݍ��݂܂�
	//fp : �ǂݍ��ރt�@�C���̃X�g���[��
	//���G���R�[�h��fp�ɐݒ肵�ĉ������B�Ȃ��A�ݒ肵�Ă��Ȃ��C���X�^���X��n�����ꍇ�A
	//  UTF16(BOM�Ȃ�)�Ƃ��ēǂݍ��݂܂�
	bool Read( mFileReadStream& fp , bool tolower = false );

	//INI�t�@�C�����������݂܂�
	// filename : �������ݐ�̃t�@�C����
	// ret : ���������true
	//������t�@�C�������݂���ꍇ�́A�㏑������܂�
	bool Write( const WString& filename );

	//INI�t�@�C�����������݂܂�
	// fp : �������ݐ�̃I�u�W�F�N�g
	// ret : ���������true
	bool Write( mFileWriteStream& fp );

	//�Z�N�V�������擾���܂�
	//section : �擾�����Z�N�V�����̖���
	//ret : �w�肵���Z�N�V�����̓��e���܂�mInitFileSection�N���X�ւ̎Q�ƁB
	//      �Y������Z�N�V�������Ȃ��ꍇ�̓k���|�C���^�B
	const mInitFileSection* GetSection( const WString& section )const noexcept;

	//�Z�N�V�������擾���܂�
	//���X�V�̂��߁Aconst�łȂ��|�C���^���~�����ꍇ�͂�������w�肵�܂��B
	//section : �擾�����Z�N�V�����̖���
	//ret : �w�肵���Z�N�V�����̓��e���܂�mInitFileSection�N���X�ւ̎Q�ƁB
	//      �Y������Z�N�V�������Ȃ��ꍇ�̓k���|�C���^�B
	mInitFileSection* GetSectionForModification( const WString& section )noexcept;

	//�Z�N�V���������݂��邩�𔻒肵�܂�
	//section : ���݂𔻒肵�����Z�N�V�����̖��O
	//ret : �Z�N�V���������݂����true�B���݂��Ȃ����false�B
	bool IsExistSection( const WString& section )const noexcept;

	//�Z�N�V������ǉ����܂�
	//���łɓ����̃Z�N�V����������ꍇ�́A���̃Z�N�V�������擾���܂�
	// section : �쐬�܂��͎擾����Z�N�V�����̖���
	// ret : �쐬�܂��͎擾�����Z�N�V�����̃|�C���^
	mInitFileSection* CreateNewSection( const WString& section )noexcept;

	//�Z�N�V�������폜���܂�
	// section : �폜����Z�N�V�����̖���
	// ret : true �Z�N�V�������폜����
	//       false ���̂悤�ȃZ�N�V�����͂Ȃ�����
	bool DeleteSection( const WString& section )noexcept;

	//�w�肵���Z�N�V�����́A�w��L�[�̒l�𓾂�B
	//�w��Z�N�V�����E�w��L�[���Ȃ��ꍇ�́A�f�t�H���g�̒l�𓾂�B
	//section : �Z�N�V�����̖��O
	//key : �L�[�̖��O
	//defvalue : �w��Z�N�V�����E�w��L�[���Ȃ��ꍇ�ɓ���l
	//ret : �w�肵���L�[�̒l���Adefvalue�̒l
	//���������悭�Ȃ��̂ŁA�����Z�N�V�����ɉ��x���A�N�Z�X����Ƃ��́A
	//�@���̊֐����g�킸��GetSection���g���ăZ�N�V�����̃|�C���^���擾���Ďg���Ă��������B
	template< class T > T GetValue( const WString& section , const WString& key , T defvalue )const
	{
		const mInitFileSection* ptr = GetSection( section );
		if( ptr == nullptr )
		{
			return defvalue;
		}
		return ptr->GetValue( key , defvalue );
	}

	//�w�肵���Z�N�V�����́A�w��L�[�̒l�𓾂�B
	//�w��Z�N�V�����E�w��L�[���Ȃ��ꍇ�́A�f�t�H���g�̒l�𓾂�B
	//section : �Z�N�V�����̖��O
	//key : �L�[�̖��O
	//table : �L�[�̒l�����b�N�A�b�v����e�[�u���B�L�[�̒l�Ɗ��S��v������̂��ꗗ�̒��ɂ���΁A�Ή�����l��Ԃ�
	//�@�@�@�����̃e�[�u���̏ڍׂ�mInitFileSection::LookupValues�̐錾���Q�Ƃ��Ă��������B
	//defvalue : �w��Z�N�V�����E�w��L�[���ǂݎ��Ȃ������ꍇ�Avalues�Ɉ�v������̂��Ȃ������ꍇ�̒l
	//ret : values���烋�b�N�A�b�v�����l�A�܂��́Adefvalue�̒l�B
	//���������悭�Ȃ��̂ŁA�����Z�N�V�����ɉ��x���A�N�Z�X����Ƃ��́A
	//�@���̊֐����g�킸��GetSection���g���ăZ�N�V�����̃|�C���^���擾���Ďg���Ă��������B
	template< class T > T GetValue( const WString& section , const WString& key , const mInitFileSection::LookupValues< T >& table , T defvalue )const
	{
		const mInitFileSection* ptr = GetSection( section );
		if( ptr == nullptr )
		{
			return defvalue;
		}
		return ptr->GetValue( key , table , defvalue );
	}

	//�Z�N�V�������̈ꗗ���擾����
	//retList : �Z�N�V�������̈ꗗ���i�[����
	//ret : �������^
	bool GetSectionList( WStringDeque& retList )const;

	//�_�~�[�p�G���v�e�B�Z�N�V����
	static const mInitFileSection EmptySection;

private:

	//INI�t�@�C���̓ǂݎ��������
	typedef std::unordered_map<WString,mInitFileSection> SectionDataMap;
	SectionDataMap MySectionDataMap;

};

#endif	//MINITFILE_H_INCLUDED

