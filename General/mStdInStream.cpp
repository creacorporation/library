#include "mStdInStream.h"


mStdInStream::mStdInStream()
{
	MyReadCacheHead.reset();
}

mStdInStream::~mStdInStream()
{
}

//�P�����i�P�o�C�g�j�ǂݍ��݂܂�
//ret : �ǂݎ��������
//��EOF�̏ꍇ�A���ݓǂݎ���f�[�^���Ȃ����Ƃ������܂�
//�i���Ԃ��o�Ă΍ēx�ǂݎ��邩������Ȃ��j
//���X�g���[�������S�ɏI�����Ă��邩��m��ɂ�IsEOF���g���܂�
INT mStdInStream::Read( void )
{
	//UnRead���ꂽ����������ꍇ�̓\����Ԃ�
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return MyUnReadBuffer.Read();
	}

	INT result = fgetc( stdin );
	if( result == EOF )
	{
		MyIsEOF = true;
	}
	return result;
}

//EOF�ɒB���Ă��邩�𒲂ׂ܂�
bool mStdInStream::IsEOF( void )const
{
	if( !MyUnReadBuffer.IsEmpty() )
	{
		return false;
	}
	return MyIsEOF;
}

//�t�@�C�����J���Ă��邩�𔻒肵�܂�
//�J���Ă���ꍇ�͐^���Ԃ�܂�
bool mStdInStream::IsOpen( void )const
{
	return true;
}

