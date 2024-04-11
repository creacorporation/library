//----------------------------------------------------------------------------
// Felica Light-S �J�[�h�n���h��
// Copyright (C) 2021- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#ifndef MSCFELICAMAC_H_INCLUDED
#define MSCFELICAMAC_H_INCLUDED

#include <mStandard.h>
#include <General/mBinary.h>
#include <bcrypt.h>
#include <memory>
#include <Crypto/mSCFelicaDefinitions.h>

class mSCFelicaMac
{
public:
	mSCFelicaMac();
	virtual ~mSCFelicaMac();

	//�Í��v���o�C�_�̏�����
	// ret : �������^
	bool Initialize( void );

	//�J�[�h���̐ݒ�
	// key : �J�[�h��
	//�@�E�҂�����P�U�o�C�g�ł��邱��
	// retChallenge : ���������`�������W(�����FelicaLiteS��RC(80h)�u���b�N�ɑ��M����)
	// ret : �������^
	bool SetKey( const mSecureBinary& key , mBinary& retChallenge );

	//�����폜����
	void ClearKey( void );

	//����ێ����Ă��邩�Ԃ�
	bool IsKeyExist( void )const;

	//���m�̎㌮�łȂ����`�F�b�N���܂�
	// key : ���ؑΏۂ̌�(CK�ɑ��荞�ރo�C�g�I�[�_�[��16�o�C�g)
	//        +0         +7     +8         +15
	//       CK1[0],...,CK1[7],CK2[0],...,CK2[7]
	// ret : �`�F�b�N���i�̏ꍇ�^�B�`�F�b�N�s���i�̏ꍇ(�Ǝ㌮�̏ꍇ)�U
	static bool CheckIsWeakKey( const mSecureBinary& key );

	//�ǂݏ������s���u���b�N�̃G���g��
	using DataBlockEntry = mSCFelicaDefinitions::DataBlockEntry;
	//�ǂݏ������s���u���b�N�̃f�[�^
	using DataBlock = mSCFelicaDefinitions::DataBlock;

	//MAC_A���b�Z�[�W�u���b�N��p�������b�Z�[�W�̔F�؂��s���܂�
	//���炩����SetKey�ŃJ�[�h����ݒ肵�A�擾�����`�������W���J�[�h�ɑ��M������ł��Ƃ肵�����b�Z�[�W�u���b�N���Ώۂł��B
	// data : �F�؂��郁�b�Z�[�W
	//�@�E�Ō�̃u���b�N��MAC_A(91h)�ł��邱��
	//�@�E���̑��ׁX�Ƃ���MAC_A�F�؂̗v���𖞂����Ă��邱��
	// ret : �F�؂���������ΐ^
	bool ValidateMacA( const DataBlock& data );

	//���b�Z�[�W�ɑ΂���MAC_A���b�Z�[�W�u���b�N�𐶐�����
	//���炩����SetKey�ŃJ�[�h����ݒ肵�A�擾�����`�������W���J�[�h�ɑ��M���Ă����K�v������܂��B
	// data : �F�؂��郁�b�Z�[�W
	//�@�E���̃��b�Z�[�W�ɑΉ�����MAC_A�u���b�N���Z�o���܂�
	// retMacA : �Z�o����MAC_A�u���b�N
	// wcnt : ���݂�WCNT�̒l
	// ret : �����ɐ�������ΐ^
	bool CreateMacA( const DataBlockEntry& data , DataBlockEntry& retMacA , DWORD wcnt );

	//�ʉ��}�X�^�[������ʉ��J�[�h���𓱂��܂�
	// master : �}�X�^�[��(�҂�����24�o�C�g�ł���K�v������܂�)
	// id : �J�[�h��ID�u���b�N�̃o�C�i��(16�o�C�g)
	// retcardkey : ���o�����J�[�h��
	static bool CalcDiversifiedKey( const mSecureBinary& master , const mBinary& id , mSecureBinary& retcardkey );

private:

	mSCFelicaMac( const mSCFelicaMac& source );
	const mSCFelicaMac& operator=( const mSCFelicaMac& source ) = delete;

protected:

	//�Í��v���o�C�_�̃n���h��
	BCRYPT_ALG_HANDLE MyAlgHandle;

	//�`�������W�̒l
	mBinary MyChallenge;

	//�Í����̃n���h��(���[�h�p)
	BCRYPT_KEY_HANDLE MyRKeyHandle;

	//�L�[�I�u�W�F�N�g(���[�h�p)
	std::unique_ptr<BYTE> MyRKeyObject;

	//�Í����̃n���h��(���C�g�p)
	BCRYPT_KEY_HANDLE MyWKeyHandle;

	//�L�[�I�u�W�F�N�g(���C�g�p)
	std::unique_ptr<BYTE> MyWKeyObject;

	//�Z�b�V�������̐������s��
	// key : �J�[�h���̃o�C�i��
	// ret : �������^
	bool CreateSessionKey( const mSecureBinary& key );

};
	
#endif
