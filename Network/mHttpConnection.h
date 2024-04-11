//----------------------------------------------------------------------------
// HTTP�A�N�Z�X
// Copyright (C) 2013 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------


#ifndef MHTTPCONNECTION_H_INCLUDED
#define MHTTPCONNECTION_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"
#include "mHttpRequest.h"
#include <winhttp.h>
#include <list>

#pragma comment( lib , "winhttp.lib" )

//�Q�l�ɂȂ邩������Ȃ�URL
// http://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx
// http://msdn.microsoft.com/en-us/library/aa384273(v=vs.85).aspx
// http://eternalwindows.jp/network/winhttp/winhttp01.html

namespace Definitions_HttpConnection
{
};

class mHttpConnection
{
public:
	mHttpConnection();
	virtual ~mHttpConnection();

	//-----------------------------------------------------------------------
	// �����ݒ�p�֐�
	//-----------------------------------------------------------------------

	struct ConnectionOption
	{
		//�ڑ���T�[�o�̖��O
		WString ServerName;
		//�|�[�g�ԍ�
		USHORT Port;
	};

	//���N�G�X�g�ݒ�
	using RequestOption = mHttpRequest::RequestOption;

	//�ʒm�ݒ�
	using NotifyOption = mHttpRequest::NotifyOption;

	//���N�G�X�g�𐶐�
	bool NewRequest( const RequestOption& opt , const NotifyOption& notifier , mHttpRequest& retReq );

	//�n���h������������Ă��邩��Ԃ�
	operator bool() const;

private:
	mHttpConnection( const mHttpConnection& source );
	const mHttpConnection& operator=( const mHttpConnection& source );

	friend class mHttpAccess;

protected:

	//�n���h��
	HINTERNET MyConnection;

	//���ݐڑ����Ă���T�[�o�̃A�h���X
	WString MyServerAddress;

	//�ڑ���̃|�[�g
	USHORT MyPort;

};

#endif
