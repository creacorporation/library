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

#ifndef MSERVICECONTROLMANAGER_H_INCLUDED
#define MSERVICECONTROLMANAGER_H_INCLUDED

#include "mStandard.h"
#include "General/mTCHAR.h"

namespace Definitions_mServiceControlManager
{
	enum ServiceStartType
	{
		STARTTYPE_AUTO = SERVICE_AUTO_START,
		STARTTYPE_ONDEMAND = SERVICE_DEMAND_START,
		STARTTYPE_DISABLED = SERVICE_DISABLED,
	};
	enum ServiceErrorControl
	{
		ONERROR_LOGONLY = SERVICE_ERROR_IGNORE,
		ONERROR_POPUPMESSAGEBOX = SERVICE_ERROR_NORMAL,
		ONERROR_SEVERE = SERVICE_ERROR_SEVERE,
		ONERROR_CRITICAL = SERVICE_ERROR_CRITICAL,
	};
	enum ServiceProcessType
	{
		//自分自身のプロセスを持つ。
		SERVICE_OWN_PROCESS = 1,
		//ホストプロセスを持つ。
		SERVICE_SHARE_PROCESS = 2,
	};

	enum ServiceProcessUser
	{
		SYSTEM_PROCESS,
		LOGONUSER_PROCESS,
	};
};

namespace mServiceControlManager
{
	//オプション構造体
	//実際にフォントを作成するときは、Option構造体を直接使わずに、作りたい物に合わせて以下を使って下さい。
	//・Option_UseOption … メンバ変数を埋めてオプションを設定したいとき
	struct ServiceParam
	{
		using ServiceProcessUser = Definitions_mServiceControlManager::ServiceProcessUser;
		const ServiceProcessUser method;	//RTTIの代用です。変更の必要はありません。

		using ServiceStartType = Definitions_mServiceControlManager::ServiceStartType;
		using ServiceErrorControl = Definitions_mServiceControlManager::ServiceErrorControl;
		using ServiceProcessType = Definitions_mServiceControlManager::ServiceProcessType;

		//サービスの名称（サービスのID）
		WString ServiceName;
		//サービスの表示上の名前
		WString DisplayName;
		//サービスの説明文
		WString Description;
		//サービスのプロセスの種別
		ServiceProcessType ProcessType;
		//ユーザーとの直接対話を許可するか(LocalSystemアカウントで実行するとき限定)
		bool Interactive;
		//サービス起動モード
		ServiceStartType StartType;
		//サービスにエラーが発生したときの挙動
		ServiceErrorControl ErrorControl;
		//サービスのバイナリ
		WString BinaryPathName;
		//依存先のサービス名
		WStringVector Dependencies;

	protected:
		ServiceParam() = delete;
		ServiceParam( ServiceProcessUser method ) : method( method )
		{
			ProcessType = ServiceProcessType::SERVICE_OWN_PROCESS;
			Interactive = false;
			StartType = ServiceStartType::STARTTYPE_DISABLED;
			ErrorControl = ServiceErrorControl::ONERROR_LOGONLY;
		}
	};


	//サービス作成パラメータ（通常のサービス）
	//・サービスが使用するアカウントを指定できる
	//・システム起動と同時に起動できる
	struct ServiceParam_System : public ServiceParam
	{
		//サービスを実行するユーザー名
		WString UserName;
		//サービスを実行するユーザーのパスワード
		WString Password;

		ServiceParam_System() : ServiceParam( ServiceProcessUser::SYSTEM_PROCESS )
		{
		}
	};

	//サービス作成パラメータ
	//・サービスが使用するアカウントは現在ログオンしているユーザー
	//・ユーザーがログオンしていないと使えない
	struct ServiceParam_LogonUser : public ServiceParam
	{
		ServiceParam_LogonUser() : ServiceParam( ServiceProcessUser::LOGONUSER_PROCESS )
		{
		}
	};

	//新しいサービスを作成する
	bool CreateNewService( const ServiceParam& param );

	//既存のサービスを削除する
	bool DeleteExistingService( const WString& ServiceName );

	//既存のサービスを開始する
	bool StartExistingService( const WString& ServiceName , const WStringDeque& args );

	//既存のサービスを中断する
	bool PauseExistingService( const WString& ServiceName );

	//既存のサービスを再開する
	bool ContinueExistingService( const WString& ServiceName );

	//既存のサービスを停止する
	bool StopExistingService( const WString& ServiceName , bool is_planed , const WString& reason );

	//既存のサービスにコントロールコードを送る
	bool ControlExistingService( const WString& ServiceName , DWORD code );

	//ユーザーセッションで動作しているサービスの、現セッションでのサービス名を調べる
	//　※ユーザーセッションで動作しているサービスは、タスクマネージャ等でみると、
	//　　「<サービス名>_xxxxxx」の名前になっていて、xxxxxxの部分はログインするたびに変わる。
	//　　この部分を取得するAPIは非公開のようなので、以下条件のものを検索する。
	// (1)動作中のサービス
	// (2)ユーザーセッションで動作しているサービス
	// (3)サービス名のアンダーバーより前の部分が、探しているサービス名と一致
	// (4)現在のプロセスを実行しているセッションIDと、サービスを実行しているセッションIDが同じ
	// ServiceName = 検索対象のサービス
	// retFound = 発見したサービス
	// ret = 成功時真
	bool SearchUserProcessService( const WString& ServiceName , WString& retFound );

};


#endif


