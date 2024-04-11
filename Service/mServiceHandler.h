//----------------------------------------------------------------------------
// サービスハンドラ
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSERVICEHANDLER_H_INCLUDED
#define MSERVICEHANDLER_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

class mServiceHandler
{
public:

	mServiceHandler();
	virtual ~mServiceHandler();

	//サービスを開始する
	//※サービスが終了するまで制御を返さない
	//ret : サービスの起動に失敗するとfalse。サービスが正常終了した場合はtrue。
	bool Start( void );

private:

	mServiceHandler( const mServiceHandler& source ) = delete;
	void operator=( const mServiceHandler& source ) = delete;

	SERVICE_STATUS_HANDLE MyStatusHandle;
	SERVICE_STATUS MyServiceStatus;

	//以下Windowsから呼ばれるメンバ。ユーザが呼び出すことはない。
	static mServiceHandler* MyPtr;

	static void __stdcall  ServiceMainProxy( DWORD argc , LPWSTR* argv );
	void ServiceMain( DWORD argc , LPTSTR* argv );

	static DWORD __stdcall HandlerProxy( DWORD Control , DWORD EventType , LPVOID EventData , LPVOID Context );
	DWORD Handler( DWORD Control , DWORD EventType , LPVOID EventData );

	bool UpdateServiceStatus();
	bool UpdateServiceStatus( DWORD status );

protected:

	//サービスの情報
	struct ServiceInterfaceInfo
	{
		WString ServiceName;
		bool HandleParamChange;				//サービスのパラメータが変更された
		bool HandleNetBindChange;			//ネットワーク接続の変更
		bool HandleHardwareProfileChange;	//システムのハードウエアプロファイルが変更された
		bool HandlePowerChange;				//システムの電源状態が変更された
		bool HandleSessionChange;			//システムのセッションが変更された
		bool HandlePreShutdown;				//システムのシャットダウン前イベントが発生
		bool HandleTimeChange;				//システム時刻が変更された
		bool HandleTriggerEvent;			//特殊なイベントを受け取る。なお、mServiceControlManagerにおいて、その設定処理は未実装。

		ServiceInterfaceInfo()
		{
			Reset();
		}
		void Reset( void )
		{
			ServiceName = L"";
			HandleParamChange = false;
			HandleNetBindChange = false;
			HandleHardwareProfileChange = false;
			HandlePowerChange = false;
			HandleSessionChange = false;
			HandlePreShutdown = false;
			HandleTimeChange = false;
			HandleTriggerEvent = false;
		}
	};

	//サービスの情報を返します
	virtual void QueryInterface( ServiceInterfaceInfo& retInfo ) = 0;

	//時間がかかる処理を行う場合の進捗報告
	void SetCheckPoint( DWORD WaitHint );

	//サービスの初期化時に呼び出します
	// ret : エラーなく初期化できた場合はNO_ERROR( = 0 )を返します
	//       エラーが発生した場合は、NO_ERROR以外の値を返します
	virtual DWORD OnInitialize( DWORD argc , LPTSTR* argv );

	//サービスのメインルーチン
	// ret : エラーなく初期化できた場合はNO_ERROR( = 0 )を返します
	//       エラーが発生した場合は、NO_ERROR以外の値を返します
	virtual DWORD Main( DWORD argc , LPTSTR* argv ) = 0;

	//サービスの停止が要求された
	virtual void OnStop( void );

	//サービスの一時停止が要求された
	virtual void OnPause( void );

	//一時停止中のサービス再開を要求された
	virtual void OnContinue( void );

	//システムがシャットダウンしようとしている
	virtual void OnShutdown( void );

	//サービスのパラメータが変更された
	virtual void OnParamChange( void );

	//新しいネットワーク接続が検出された
	//※Plug&Playの使用が推奨
	[[deprecated]]
	virtual void OnNetBindAdd( void );

	//有効であったネットワーク接続のうちいずれかが無効になった
	//※Plug&Playの使用が推奨
	[[deprecated]]
	virtual void OnNetBindRemove( void );

	//無効であったネットワーク接続のうちいずれかが有効になった
	//※Plug&Playの使用が推奨
	[[deprecated]]
	virtual void OnNetBindEnable( void );

	//存在していたネットワーク接続が削除された
	//※Plug&Playの使用が推奨
	[[deprecated]]
	virtual void OnNetBindDisable( void );

	//デバイスイベントが発生した
	//※このイベントを取得するには、あらかじめRegisterDeviceNotification()で登録が必要
	virtual DWORD OnDeviceEvent( DWORD type , const void* data);

	//ハードウエアのプロファイルが変わった
	virtual DWORD OnHardwareProfileChange( DWORD type );

	//電源状態の変更
	virtual DWORD OnPowerEvent( DWORD type , const POWERBROADCAST_SETTING& data);

	//セッションが変更された
	virtual void OnSessionChange( DWORD type , const WTSSESSION_NOTIFICATION& data);

	//システムのシャットダウン前イベントが発生
	//※このイベントをハンドルすることで、サービス終了までシャットダウンをブロックできる
	virtual void OnPreShutdown( void );

	//時刻変更
	virtual void OnTimeChange( const  SERVICE_TIMECHANGE_INFO& data );

	//"Service Trigger Events"が発生
	//※このイベントを取得するには、あらかじめChangeServiceConfig2()で登録が必要
	virtual void OnTriggerEvent( void );

	//ユーザー定義
	virtual DWORD OnUserControlCode( DWORD control , DWORD param1 , void* param2 );


};

#endif //MSERVICEHANDLER_H_INCLUDED

