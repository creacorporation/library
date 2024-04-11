//----------------------------------------------------------------------------
// WebView2�n���h���[
// Copyright (C) 2021 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
//----------------------------------------------------------------------------


#ifndef MWEBVIEW2_H_INCLUDED
#define MWEBVIEW2_H_INCLUDED
#include <mRevision.h>
#if defined(LIBRARY_ENABLE_WEBVIEW2) || !defined(MWEBVIWEW2_CPP_COMPILING)
#include <WinSock2.h>
#include <wrl.h>
#include <WebView2.h>
#include <mStandard.h>

#include "mWindow.h"
#include "../General/mNotifyOption.h"

namespace Definitions_WebView2
{
	//�i�r�Q�[�V�����J�n���̃p�����[�^
	struct OnNavigationStartingOpt
	{
		//���_�C���N�g�ɂ��J�n���悤�Ƃ��Ă���ꍇtrue
		bool IsRedirected;
		//���[�U�[���J�n��������ł���ꍇtrue
		bool IsUserInitiated;
		//�������J�n����URI
		WString Uri;
		//�i�r�Q�[�V�����̊J�n��������
		//���R�[���o�b�N���ł��̃p�����[�^��false�ɏ���������ƃi�r�Q�[�V�����̓L�����Z������܂�
		bool IsAllowed;
	};

	//�i�r�Q�[�V�����������̃p�����[�^
	struct OnNavigationCompletedOpt
	{
		//����I�����H
		bool IsSuccess;
		//�G���[�R�[�h�������
		COREWEBVIEW2_WEB_ERROR_STATUS Status;
	};

	//InjectOnCreateScript()�Œ��������X�N���v�g�������̃p�����[�^
	struct OnCreateScriptCompletedOpt
	{
		//����I�����H
		bool IsSuccess;
		//���������X�N���v�gID�i�폜����ꍇ�Ɏg���j
		WString ScriptId;
	};

	//WebMessage��M���̃p�����[�^
	struct OnWebMessageRecievedOpt
	{
		//���M��
		WString Sender;
		//���b�Z�[�W(JSON�t�H�[�}�b�g)
		WString Message;
		//���b�Z�[�W���V���v���ȃe�L�X�g�Ƃ��ēǂ߂�ꍇ�Ɍ���A�ǂݎ����������B
		//�ǂ߂Ȃ��ꍇ��nullptr�B
		std::unique_ptr<WString> MessageAsString;
	};

	//Javascript���s�������̃p�����[�^
	struct OnScriptCompletedOpt
	{
		//���s����(JSON�t�H�[�}�b�g)
		WString Message;
	};

};

class mWebView2 : public mWindow 
{
public:

	//�ʒm���̃p�����[�^
	union NotifyFunctionOptPtr
	{
		//WebView2�̏����������������ꍇ�̃C�x���g�ł�
		Definitions_WebView2::OnNavigationStartingOpt* OnNavigationStarting;
		//�y�[�W�J�ڂ����������ꍇ�̃C�x���g�ł�
		Definitions_WebView2::OnNavigationCompletedOpt* OnNavigationCompleted;
		//InjectOnCreateScript()�ŋ������s����X�N���v�g�𒍓����Ă���ꍇ�A���̃X�N���v�g�̎��s����������ƃR�[���o�b�N�����C�x���g�ł��B
		Definitions_WebView2::OnCreateScriptCompletedOpt* OnCreateScriptCompleted;
		//�E�F�u���b�Z�[�W����M�����ꍇ�̃C�x���g�ł�
		Definitions_WebView2::OnWebMessageRecievedOpt* OnWebMessageRecievedOpt;
		//ExceScript()�Ŏ��s�����X�N���v�g�����������ꍇ�̃C�x���g�ł�
		Definitions_WebView2::OnScriptCompletedOpt* OnScriptCompletedOpt;
	};

	//�ʒm�ݒ�
	using NotifyFunction = void(*)( mWebView2& webview , DWORD_PTR parameter , NotifyFunctionOptPtr* opt );
	class NotifyOption : public mNotifyOption< NotifyFunction >
	{
	public:
		//WebView2�̏����������������ꍇ�̃C�x���g�ł�
		NotifierInfo OnInitialized;
		//�y�[�W�J�ڂ��J�n�����ꍇ�̃C�x���g�ł�
		//�E���_�C���N�g���ꂽ�ꍇ�́A������A���Ŕ������邱�Ƃ�����܂�
		NotifierInfo OnNavigationStarting;
		//�y�[�W�J�ڂ����������ꍇ�̃C�x���g�ł�
		//�E���s�̏ꍇ������܂�
		NotifierInfo OnNavigationCompleted;
		//InjectOnCreateScript()�ŋ������s����X�N���v�g�𒍓����Ă���ꍇ�A
		//���̃X�N���v�g�̎��s����������ƃR�[���o�b�N�����C�x���g�ł��B
		NotifierInfo OnCreateScriptCompleted;
		//�E�F�u���b�Z�[�W����M�����ꍇ�̃C�x���g�ł�
		// Javascript������uwindow.chrome.webview.postMessage("MessageFromJavascript");�v�݂����Ȋ����Ń��b�Z�[�W�𑗂�Ƃ������M�ł��܂��B
		NotifierInfo OnWebMessageReceived;
		//ExceScript()�Ŏ��s�����X�N���v�g�����������ꍇ�̃C�x���g�ł�
		NotifierInfo OnScriptCompleted;
	};

	//�R���g���[���������̃I�v�V����
	//���ۂɍ쐬����Ƃ��́AOption�\���̂𒼐ڎg�킸�ɁA�V�`���G�[�V�����ɍ��킹�Ĉȉ����g���ĉ������B
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
	public:
		//�����̕��@
		enum CreateMethod
		{
			USEOPTION,		//�ʏ�̕��@
		};

		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B

		//�ʒm�I�v�V����
		NotifyOption Notify;

		virtual ~Option()
		{
			return;
		}
	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�R���g���[���������̃I�v�V����
	struct Option_UseOption : public Option
	{
	public:
		//�\���ʒu
		WindowPosition Pos;

		//���[�U�[�f�[�^��z�u����p�X
		WString UserDataFolder;	

		//���[�U�[�G�[�W�F���g�̕�����̈���
		//true�̏ꍇ�AUserAgent������UserAgent�̖����ɕt�������܂�
		//false�̏ꍇ�AUserAgent�̒l�Ō���UserAgent��u�������܂�
		bool IsAppendUserAgent;

		//���[�U�[�G�[�W�F���g�̕�����B�󕶎���̏ꍇ��Edge�̃f�t�H���g��UA�B
		WString UserAgent;

		//�������������ɊJ���̂́AURL��HTML��
		//true�̏ꍇ�AOnCreateNavigation��URL
		//false�̏ꍇ�AOnCreateNavigation��HTML
		bool IsNavigationUrl;

		//�������������ɊJ��URL�܂���HTML�B�󕶎���̏ꍇ�͂Ȃɂ����Ȃ��B
		WString OnCreateNavigation;

		//�E�N���b�N���j���[�L���E�����X�C�b�`
		//false�ɂ���ƁA�R���e�N�X�g���j���[�������ɂȂ�
		bool IsEnableContextMenu;

		//�J���c�[����L���E�����X�C�b�`
		//false�ɂ���ƁA�J���c�[���������ɂȂ�
		bool IsEnableDevTool;

		//�X�e�[�^�X�o�[�L���E�����X�C�b�`
		//false�ɂ���ƁA�X�e�[�^�X�o�[�͕\������Ȃ��B
		bool IsEnableStatusBar;

		//�X�N���v�g�L���E�����X�C�b�`
		//false�ɂ���ƁAJavascript�͓��삵�Ȃ��B�������AExecScript()�Œ��ڎ��s�����ꍇ�͓��삷��B
		bool IsEnableScript;

		//�Y�[���L���E�����X�C�b�`
		//false�ɂ���ƁA���[�U�[�ɂ��Y�[���̒����͖���
		bool IsEnableZoom;

		//�r���g�C���G���[�y�[�W�̗L���E�����X�C�b�`
		//false�ɂ���ƁA�r���g�C���G���[�y�[�W�͖���
		bool IsEnableBuiltinErrorPage;

		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
			IsAppendUserAgent = true;
			IsNavigationUrl = true;
			IsEnableContextMenu = true;
			IsEnableDevTool = false;
			IsEnableStatusBar = true;
			IsEnableScript = true;
			IsEnableZoom = true;
			IsEnableBuiltinErrorPage = true;
		}
		virtual ~Option_UseOption()
		{
			return;
		}
	};

	//�E�C���h�E�̉�/�s����ύX����
	//newstate : true�ŉ��Afalse�ŕs��
	//ret : ������true
	virtual bool SetVisible( bool newstate )override;

	//�E�C���h�E�̗L��/������ύX����
	//newstate : true�ŗL���Afalse�Ŗ���
	//ret : ������true
	virtual bool SetEnable( bool newstate )override;

	//�E�C���h�E�̈ʒu���X�N���[�����W�œ���
	//ret : �X�N���[�����W�ɂ��E�C���h�E�̈ʒu(�G���[�̏ꍇ0,0,0,0)
	virtual RECT GetRect( void ) const override;

	//�E�C���h�E�̃T�C�Y�𓾂�
	//ret : �E�C���h�E�̃T�C�Y(�G���[�̏ꍇ0,0)
	virtual SIZE GetSize( void ) const override;

	//���̃E�C���h�E�̃N���C�A���g���W���X�N���[�����W�ɕϊ�����
	//client_pos : �N���C�A���g���W
	//ret : �X�N���[�����W(�G���[�̏ꍇ0,0)
	virtual POINT Client2Screen( const POINT& client_pos ) const override;

	//���̃E�C���h�E�̃N���C�A���g���W���X�N���[�����W�ɕϊ�����
	//x : �N���C�A���g���W(X���W)
	//x : �N���C�A���g���W(Y���W)
	//ret : �X�N���[�����W(�G���[�̏ꍇ0,0)
	virtual POINT Client2Screen( INT x , INT y ) const override;

	//�X�N���[�����W�����̃E�C���h�E�̃N���C�A���g���W�ɕϊ�����
	//client_pos : �X�N���[�����W
	//ret : �N���C�A���g���W(�G���[�̏ꍇ0,0)
	virtual POINT Screen2Client( const POINT& client_pos ) const override;

	//�X�N���[�����W�����̃E�C���h�E�̃N���C�A���g���W�ɕϊ�����
	//x : �X�N���[�����W(X���W)
	//x : �X�N���[�����W(Y���W)
	//ret : �N���C�A���g���W(�G���[�̏ꍇ0,0)
	virtual POINT Screen2Client( INT x , INT y ) const override;

	//�t�H�[�J�X��ݒ肷��
	//ret : ������true
	virtual bool SetFocus( void )const override;

	//�w��̃A�h���X�Ƀi�r�Q�[�g����
	bool Navigate( const WString& uri );

	//������Ŏw�肵���g�s�l�k��\������
	bool PutHTML( const WString& html );

	//�����[�h����
	bool Reload( void );

	//�ǂݍ��݂𒆎~����
	bool Stop( void );

	//[�߂�]���g�p�\���m�F����
	bool IsBackAvailable( void );

	//[�i��]���g�p�\���m�F����
	bool IsForwardAvailable( void );

	//[�߂�]
	bool Back( void );

	//[�i��]
	bool Forward( void );

	//�h�L�������g�̃^�C�g�����擾����
	//ret : �h�L�������g�̃^�C�g���B�G���[�̏ꍇ�͋󕶎���B
	WString GetDocumentTitle( void );

	//���݂�URI���擾����
	//ret : ���݂�URI�B�G���[�̏ꍇ�͋󕶎���B
	WString GetURI( void );

	//�h�L�������g�ǂݍ��ݎ��ɋ������s����X�N���v�g��}������
	// scr : ��������javascript
	// ret : �������^
	bool InjectOnCreateScript( const WString& scr );

	//�h�L�������g�ǂݍ��ݎ��ɋ������s����X�N���v�g����������
	//���P����s���������A�R�[���o�b�N�֐����瓾����h�c��m��Ȃ��ƍ폜�ł��Ȃ�
	// id : �폜����X�N���v�g��ID(�R�[���o�b�N�֐��̃p�����[�^���瓾����)
	// ret : �������^
	bool RemoveOnCreateScript( const WString& id );

	//�E�F�u���b�Z�[�W�𑗐M����iJSON�`���j
	// message : ���M���郁�b�Z�[�W
	// ret : �������^
	//HTML������͂���Ȋ����Ń��X�i�[��o�^����ƁA���b�Z�[�W���͂��悤�ɂȂ�B
	//  window.chrome.webview.addEventListener('message', handler)
	//  window.chrome.webview.removeEventListener('message', handler)
	bool SendMessageAsJson( const WString& message );

	//�E�F�u���b�Z�[�W�𑗐M����i������`���j
	// message : ���M���郁�b�Z�[�W
	// ret : �������^
	//HTML������͂���Ȋ����Ń��X�i�[��o�^����ƁA���b�Z�[�W���͂��悤�ɂȂ�B
	//  window.chrome.webview.addEventListener('message', handler)
	//  window.chrome.webview.removeEventListener('message', handler)
	bool SendMessageAsString( const WString& message );

	//�X�N���v�g�����s����
	// ret : �������^
	// scr : ���s����javascript
	bool ExecScript( const WString& scr );

	//�Y�[���{�����擾����
	// errorvalue : �G���[�̏ꍇ�ɕԂ��l
	// ret : �Y�[���{���B�G���[�̏ꍇerrorvalue�Ɏw�肵���l�B
	double GetZoomFactor( double errorvalue = 1.0f );

	//�Y�[���{����ݒ肷��
	bool SetZoomFactor( double factor );

	//���[�U�[�G�[�W�F���g���擾����
	// ret : �擾����UA
	WString GetUserAgant( void );

	//���[�U�[�G�[�W�F���g��ݒ肷��
	// ua : �V����ua
	// ret : �������^
	bool SetUserAgant( const WString& ua );

public:

	//�t�@�N�g�����\�b�h
	static mWindow* Factory( const void * option )throw( )
	{
		return mNew mWebView2;
	}

protected:
	mWebView2();
	virtual ~mWebView2();

	//�E�C���h�E�N���X�̓o�^������
	virtual bool WindowClassSettingCallback( WindowClassSetting& retSetting , const void* opt )override;

	//�E�C���h�E���J��
	virtual bool CreateWindowCallback( CreateWindowSetting& retSetting , const void* opt )override;
	
	//�E�C���h�E���������������ꍇ�ɃR�[�������
	virtual bool OnCreate( const void* opt )override;

	//�E�C���h�E���ړ�����
	virtual void MoveWindowPosition( const RECT& pos )override;

	//WebView���n���h��
	using EnvironmentHandle = Microsoft::WRL::ComPtr<ICoreWebView2Environment>;
	EnvironmentHandle MyEnvironmentHandle;

	//WebView�R���g���[���[�̃n���h��
	using ControllerHandle = Microsoft::WRL::ComPtr<ICoreWebView2Controller>;
	ControllerHandle MyControllerHandle;

	//WebView�̃n���h��
	using ViewHandle = Microsoft::WRL::ComPtr<ICoreWebView2>;
	ViewHandle MyViewHandle;

	//�f���Q�[�g�̍č\�z
	//���s�ɂ��ȉ��̃C�x���g��`�Ŏg�p����f���Q�[�g���č\�z����
	void UpdateDelegate( void );
	//�f���Q�[�g��`���i�r�Q�[�V�����X�^�[�g
	EventRegistrationToken MyOnNavigationStartingToken;
	//�f���Q�[�g��`���i�r�Q�[�V��������
	EventRegistrationToken MyOnNavigationCompletedToken;
	//�f���Q�[�g��`���E�F�u���b�Z�[�W��M
	EventRegistrationToken MyOnWebMessageReceivedToken;

	//�R�[���o�b�N�֐�
	HRESULT OnCreateEnvComplete( HRESULT callback_result , ICoreWebView2Environment* env );
	HRESULT OnCreateCntComplete( HRESULT callback_result , ICoreWebView2Controller* cnt );
	HRESULT OnNavigationStarting( ICoreWebView2* sender , ICoreWebView2NavigationStartingEventArgs* args );
	HRESULT OnNavigationCompleted( ICoreWebView2* sender , ICoreWebView2NavigationCompletedEventArgs* args );
	HRESULT OnCreateScriptCompleted( HRESULT callback_result , LPCWSTR id );
	HRESULT OnWebMessageReceived( ICoreWebView2* sender , ICoreWebView2WebMessageReceivedEventArgs* args );
	HRESULT OnScriptCompleted( HRESULT callback_result , LPCWSTR response );

private:

	mWebView2( const mWebView2& src ) = delete;
	mWebView2& operator=( const mWebView2& src ) = delete;

	//�I�v�V�����f�[�^���ꎞ�ۑ�����
	std::unique_ptr<Option> MyOption;

};

#endif
#endif
