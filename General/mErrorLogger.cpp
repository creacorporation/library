//----------------------------------------------------------------------------
// アサート／エラー処理
// Copyright (C) 2016 Fingerling. All rights reserved. 
// Copyright (C) 2020-2023 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------

#define MERRORLOGGER_CPP_COMPILING
#include "mErrorLogger.h"
#pragma comment( lib , "winmm.lib" )

//--------------------------------------
//以下、メソッドの定義
//--------------------------------------
mErrorLogger::mErrorLogger( DWORD max_error )
{
	MyMaxErrorSize = max_error;
	MyLogOutputMode = LogOutputMode::LOG_OUTPUT_NONE;
	MyCurrentId = 0;
	MyHandle = INVALID_HANDLE_VALUE;
	MyIsNoTrace = false;
	MyProxy = nullptr;
	MyCallback = nullptr;
	MyIsEnabled = true;
	Clear();
}

mErrorLogger::~mErrorLogger()
{
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
}

//ログの記録を無効化する
void mErrorLogger::Disable( void )
{
	MyIsEnabled = false;
}

//ログの記録を有効化する
void mErrorLogger::Enable( void )
{
	MyIsEnabled = true;
}


DWORD mErrorLogger::AddEntry( ErrorLevel level , const WString & file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 )
{
	//有効かどうか確認する
	if( !MyIsEnabled )
	{
		return MyCurrentId;
	}
	if( MyProxy )
	{
		return MyProxy->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 , nullptr );
	}
	return AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 , nullptr );
}

threadsafe DWORD mErrorLogger::AddEntry( ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString& mes2 , const mErrorLogger* origin )
{
	//有効かどうか確認する
	if( !MyIsEnabled )
	{
		return MyCurrentId;
	}
	//出元が自分でないか確認する
	if( origin == this )
	{
		//Proxyで一周したと言うことなので抜ける
		return 0;
	}

	//返す値
	DWORD retval;

	//新しいエントリ
	LogEntry entry;
	entry.Level = level;
	entry.File = file;
	entry.Line = line;
	entry.Code1 = ec1;
	entry.Code2 = ec2;
	entry.Message1 = mes1;
	entry.Message2 = mes2;
	entry.Time = timeGetTime();
	entry.ThreadId = GetCurrentThreadId();

	//ログにつっこんで、サイズを確認して最大値超えてたら最後のを消す
	{
		mCriticalSectionTicket Ticket( MyCritical );

		entry.Id = MyCurrentId;
		MyCurrentId++;
		retval = MyCurrentId - 1;

		if( !MyIsNoTrace )
		{
			MyLogError.push_front( entry );
			if( MyMaxErrorSize < MyLogError.size() )
			{
				MyLogError.pop_back();
			}
		}
	}
	//回数カウント
	switch( level )
	{
	case ErrorLevel::LEVEL_ASSERT:
	case ErrorLevel::LEVEL_ERROR:
	case ErrorLevel::LEVEL_EXCEPTION:
	case ErrorLevel::LEVEL_LOGGING:
		MyErrorCount[ level ]++;
	default:
		break;
	}
	//コンソール出力
	switch( MyLogOutputMode )
	{
	case LOG_OUTPUT_CONSOLE:	//コマンドラインに出力する
		OutputLogToConsole( entry );
		break;
	case LOG_OUTPUT_DEBUGGER:	//デバッガに出力する
		OutputLogToDebugger( entry );
		break;
	case LOG_OUTPUT_FILE:		//ファイルに出力する
		OutputLogToFile( entry );
		break;
	case LOG_OUTPUT_CALLBACK:	//コールバック関数を呼ぶ
		if( MyCallback )
		{
			MyCallback( entry );
		}
		break;
	case LOG_OUTPUT_NONE:		//何もしない
	default:
		break;
	}

	if( MyProxy )
	{
		if( origin == nullptr )
		{
			MyProxy->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 , this );
			return retval;
		}
		else
		{
			MyProxy->AddEntry( level , file , line , ec1 , ec2 , mes1 , mes2 , origin );
		}
	}
	return 0;
}


//エラーログを追加します
DWORD mErrorLogger::AddEntry( const Log& toAppend )
{
	//有効かどうか確認する
	if( !MyIsEnabled )
	{
		return MyCurrentId;
	}
	else
	{
		mCriticalSectionTicket Ticket( MyCritical );

		for( Log::const_iterator itr = toAppend.begin() ; itr != toAppend.end() ; itr++ )
		{
			//検索対象
			MyLogError.push_front( *itr );
			MyLogError.front().Id = MyCurrentId;
			MyCurrentId++;

			if( MyMaxErrorSize < MyLogError.size() )
			{
				MyLogError.pop_back();
			}
		}
		return MyCurrentId - 1;
	}
}

DWORD mErrorLogger::GetCurrentId( void )const
{
	//クリティカルセクション不要
	//→取得した値を、呼び出したスレッド自身が使うなら変な値にはならない
	//→他のスレッドを監視する目的でこのIDはそもそも使えない
	//よって不要
	return MyCurrentId;
}

void mErrorLogger::SearchLog( DWORD Id , Log& retLog , const WString& file )const
{
	//ログの数が32bitを超えると正しい検索結果にならないが
	//そもそもそんな長期稼働は想定していないので無視する(1秒に50個ログを生成しても2.7年)

	DWORD tid = GetCurrentThreadId();
	retLog.clear();

	mCriticalSectionTicket Ticket( MyCritical );

	for( Log::const_iterator itr = MyLogError.begin() ; itr != MyLogError.end() ; itr++ )
	{
		if( itr->Id < Id )
		{
			break;
		}
		if( ( tid == itr->ThreadId ) &&
			( file == L"" || file == itr->File ) )
		{

			//検索対象
			retLog.push_back( *itr );
		}
	}
	return;
}

bool mErrorLogger::SearchLog( DWORD Id , ULONG_PTR code , const WString& file )const
{
	DWORD tid = GetCurrentThreadId();
	mCriticalSectionTicket Ticket( MyCritical );

	for( Log::const_iterator itr = MyLogError.begin() ; itr != MyLogError.end() ; itr++ )
	{
		if( itr->Id < Id )
		{
			break;
		}
		if(	( code == itr->Code2 ) &&				//コード一致 ←※パフォーマンスに影響があるので、チェック順注意
			( tid == itr->ThreadId ) &&				//スレッドIDチェック
			( file == L"" || file == itr->File ) )	//ファイル名チェック
		{
			//発見
			return true;
		}
	}
	//該当なし
	return false;
}

//ログを取得する
void mErrorLogger::GetLog( DWORD Id , Log& retLog , const WString& file  )const
{
	retLog.clear();
	mCriticalSectionTicket Ticket( MyCritical );

	for( Log::const_iterator itr = MyLogError.begin() ; itr != MyLogError.end() ; itr++ )
	{
		if( itr->Id < Id )
		{
			break;
		}
		if( file == L"" || file == itr->File )
		{

			//検索対象
			retLog.push_back( *itr );
		}
	}
	return;
}

//ログを取得する
void mErrorLogger::GetLog( DWORD Id_from , DWORD Id_to , Log& retLog , const WString& file )const
{
	retLog.clear();
	mCriticalSectionTicket Ticket( MyCritical );

	for( Log::const_iterator itr = MyLogError.begin() ; itr != MyLogError.end() ; itr++ )
	{
		if( Id_to < itr->Id )
		{
			continue;
		}
		if( itr->Id < Id_from )
		{
			break;
		}
		if( file == L"" || file == itr->File )
		{

			//検索対象
			retLog.push_back( *itr );
		}
	}
}


void mErrorLogger::Clear( void )
{
	mCriticalSectionTicket Ticket( MyCritical );
	MyLogError.clear();
	MyErrorCount[ ErrorLevel::LEVEL_ASSERT ] = 0;
	MyErrorCount[ ErrorLevel::LEVEL_ERROR ] = 0;
	MyErrorCount[ ErrorLevel::LEVEL_EXCEPTION ] = 0;
	MyErrorCount[ ErrorLevel::LEVEL_LOGGING ] = 0;
}


//コンソールにログを出力する
void mErrorLogger::OutputLogToConsole( const LogEntry& entry )
{
	mCriticalSectionTicket Ticket( MyCritical );

	UINT codepage = GetConsoleOutputCP();
	if( codepage == 1200 || codepage == 1201 )
	{
		switch( entry.Level )
		{
		case LEVEL_ASSERT:			//アサートが発生したとき(通常操作で発生しない想定のエラー用)
			#ifdef _WIN64
			wchar_fprintf( stderr , L"*ASSERT 0x%08llX\n" , entry.Code2 );
			#else
			wchar_fprintf( stderr , L"*ASSERT 0x%08X\n" , entry.Code2 );
			#endif
			break;
		case LEVEL_ERROR:			//一般的エラーが発生したとき(ファイルが無かった等)
			#ifdef _WIN64
			wchar_fprintf( stderr , L"*ERROR 0x%08llX\n" , entry.Code2 );
			#else
			wchar_fprintf( stderr , L"*ERROR 0x%08X\n" , entry.Code2 );
			#endif
			break;
		case LEVEL_LOGGING:			//正常終了でも記録しておきたい事柄が発生したとき(接続完了などのイベント)
			#ifdef _WIN64
			wchar_fprintf( stderr , L"*LOG 0x%08llX\n" , entry.Code2 );
			#else
			wchar_fprintf( stderr , L"*LOG 0x%08X\n" , entry.Code2 );
			break;
			#endif
		case LEVEL_EXCEPTION:		//例外がスローされたとき(mExceptionが使用します)
			#ifdef _WIN64
			wchar_fprintf( stderr , L"*EXCEPTION 0x%08llX\n" , entry.Code2 );
			#else
			wchar_fprintf( stderr , L"*EXCEPTION 0x%08X\n" , entry.Code2 );
			break;
			#endif
		default:
			#ifdef _WIN64
			wchar_fprintf( stderr , L"*UNKNOWN 0x%08llX\n" , entry.Code2 );
			#else
			wchar_fprintf( stderr , L"*UNKNOWN 0x%08X\n" , entry.Code2 );
			#endif
			break;
		}

		wchar_fprintf( stderr , L" File : %s\n" , entry.File.c_str() );
		wchar_fprintf( stderr , L" Line : %d\n" , entry.Line );
		wchar_fprintf( stderr , L" LastError : 0x%08X\n" , entry.Code1 );
		wchar_fprintf( stderr , L" Message1 : %s\n" , entry.Message1.c_str() );
		wchar_fprintf( stderr , L" Message2 : %s\n" , entry.Message2.c_str() );
		wchar_fprintf( stderr , L" Time : %d\n" , entry.Time );
	}
	else
	{
		switch( entry.Level )
		{
		case LEVEL_ASSERT:			//アサートが発生したとき(通常操作で発生しない想定のエラー用)
			#ifdef _WIN64
			fprintf( stderr , "*ASSERT 0x%08llX\n" , entry.Code2 );
			#else
			fprintf( stderr , "*ASSERT 0x%08X\n" , entry.Code2 );
			#endif
			break;
		case LEVEL_ERROR:			//一般的エラーが発生したとき(ファイルが無かった等)
			#ifdef _WIN64
			fprintf( stderr , "*ERROR 0x%08llX\n" , entry.Code2 );
			#else
			fprintf( stderr , "*ERROR 0x%08X\n" , entry.Code2 );
			#endif
			break;
		case LEVEL_LOGGING:			//正常終了でも記録しておきたい事柄が発生したとき(接続完了などのイベント)
			#ifdef _WIN64
			fprintf( stderr , "*LOG 0x%08llX\n" , entry.Code2 );
			#else
			fprintf( stderr , "*LOG 0x%08X\n" , entry.Code2 );
			break;
			#endif
		case LEVEL_EXCEPTION:		//例外がスローされたとき(mExceptionが使用します)
			#ifdef _WIN64
			fprintf( stderr , "*EXCEPTION 0x%08llX\n" , entry.Code2 );
			#else
			fprintf( stderr , "*EXCEPTION 0x%08X\n" , entry.Code2 );
			break;
			#endif
		default:
			#ifdef _WIN64
			fprintf( stderr , "*UNKNOWN 0x%08llX\n" , entry.Code2 );
			#else
			fprintf( stderr , "*UNKNOWN 0x%08X\n" , entry.Code2 );
			#endif
			break;
		}

		fprintf( stderr , " File : %s\n" , WString2AString( entry.File ).c_str() );
		fprintf( stderr , " Line : %d\n" , entry.Line );
		fprintf( stderr , " LastError : 0x%08X\n" , entry.Code1 );
		fprintf( stderr , " Message1 : %s\n" , WString2AString( entry.Message1 ).c_str() );
		fprintf( stderr , " Message2 : %s\n" , WString2AString( entry.Message2 ).c_str() );
		fprintf( stderr , " Time : %d\n" , entry.Time );
	}
}

//デバッガにログを出力する
void mErrorLogger::OutputLogToDebugger( const LogEntry& entry )
{
	mCriticalSectionTicket Ticket( MyCritical );
	WString str;

	switch( entry.Level )
	{
	case LEVEL_ASSERT:			//アサートが発生したとき(通常操作で発生しない想定のエラー用)
		#ifdef _WIN64
		sprintf( str , L"*ASSERT 0x%08llX\n" , entry.Code2 );
		#else
		sprintf( str , L"*ASSERT 0x%08X\n" , entry.Code2 );
		#endif
		break;
	case LEVEL_ERROR:			//一般的エラーが発生したとき(ファイルが無かった等)
		#ifdef _WIN64
		sprintf( str , L"*ERROR 0x%08llX\n" , entry.Code2 );
		#else
		sprintf( str , L"*ERROR 0x%08X\n" , entry.Code2 );
		#endif
		break;
	case LEVEL_LOGGING:			//正常終了でも記録しておきたい事柄が発生したとき(接続完了などのイベント)
		#ifdef _WIN64
		sprintf( str , L"*LOG 0x%08llX\n" , entry.Code2 );
		#else
		sprintf( str , L"*LOG 0x%08X\n" , entry.Code2 );
		#endif
		break;
	case LEVEL_EXCEPTION:		//例外がスローされたとき(mExceptionが使用します)
		#ifdef _WIN64
		sprintf( str , L"*EXCEPTION 0x%08llX\n" , entry.Code2 );
		#else
		sprintf( str , L"*EXCEPTION 0x%08X\n" , entry.Code2 );
		#endif
		break;
	default:
		#ifdef _WIN64
		sprintf( str , L"*UNKNOWN 0x%08llX\n" , entry.Code2 );
		#else
		sprintf( str , L"*UNKNOWN 0x%08X\n" , entry.Code2 );
		#endif
		break;
	}
	OutputDebugStringW( str.c_str() );

	sprintf( str , L" File : %s\n" , entry.File.c_str() );
	OutputDebugStringW( str.c_str() );
	sprintf( str , L" Line : %d\n" , entry.Line );
	OutputDebugStringW( str.c_str() );
	sprintf( str , L" LastError : 0x%08X\n" , entry.Code1 );
	OutputDebugStringW( str.c_str() );
	sprintf( str , L" Message1 : %s\n" , entry.Message1.c_str() );
	OutputDebugStringW( str.c_str() );
	sprintf( str , L" Message2 : %s\n" , entry.Message2.c_str() );
	OutputDebugStringW( str.c_str() );
	sprintf( str , L" Time : %d\n" , entry.Time );
	OutputDebugStringW( str.c_str() );
}

//
void mErrorLogger::OutputLogToFile( const LogEntry& entry )
{
	mCriticalSectionTicket Ticket( MyCritical );
	WString str;

	auto Output = [ this ]( const WString& str ) -> bool
	{
		DWORD writesize = (DWORD)( str.length() * sizeof( wchar_t ) );
		DWORD written = 0;
		return WriteFile( MyHandle , str.c_str() , writesize , &written , nullptr );
	};

	switch( entry.Level )
	{
	case LEVEL_ASSERT:			//アサートが発生したとき(通常操作で発生しない想定のエラー用)
		#ifdef _WIN64
		sprintf( str , L"*ASSERT 0x%08llX\r\n" , entry.Code2 );
		#else
		sprintf( str , L"*ASSERT 0x%08X\r\n" , entry.Code2 );
		#endif
		break;
	case LEVEL_ERROR:			//一般的エラーが発生したとき(ファイルが無かった等)
		#ifdef _WIN64
		sprintf( str , L"*ERROR 0x%08llX\r\n" , entry.Code2 );
		#else
		sprintf( str , L"*ERROR 0x%08X\r\n" , entry.Code2 );
		#endif
		break;
	case LEVEL_LOGGING:			//正常終了でも記録しておきたい事柄が発生したとき(接続完了などのイベント)
		#ifdef _WIN64
		sprintf( str , L"*LOG 0x%08llX\r\n" , entry.Code2 );
		#else
		sprintf( str , L"*LOG 0x%08X\r\n" , entry.Code2 );
		#endif
		break;
	case LEVEL_EXCEPTION:		//例外がスローされたとき(mExceptionが使用します)
		#ifdef _WIN64
		sprintf( str , L"*EXCEPTION 0x%08llX\r\n" , entry.Code2 );
		#else
		sprintf( str , L"*EXCEPTION 0x%08X\r\n" , entry.Code2 );
		#endif
		break;
	default:
		#ifdef _WIN64
		sprintf( str , L"*UNKNOWN 0x%08llX\r\n" , entry.Code2 );
		#else
		sprintf( str , L"*UNKNOWN 0x%08X\r\n" , entry.Code2 );
		#endif
		break;
	}
	Output( str );

	sprintf( str , L" File : %s\r\n" , entry.File.c_str() );
	Output( str );
	sprintf( str , L" Line : %d\r\n" , entry.Line );
	Output( str );
	sprintf( str , L" LastError : 0x%08X\r\n" , entry.Code1 );
	Output( str );
	sprintf( str , L" Message1 : %s\r\n" , entry.Message1.c_str() );
	Output( str );
	sprintf( str , L" Message2 : %s\r\n" , entry.Message2.c_str() );
	Output( str );
	sprintf( str , L" Time : %d\r\n" , entry.Time );
	Output( str );
}

//ログ出力のモードを変更する
bool mErrorLogger::ChangeLogOutputMode( const LogOutputModeOpt& setting )
{
	mCriticalSectionTicket Ticket( MyCritical );

	//モードを更新
	MyLogOutputMode = setting.Mode;
	MyProxy = setting.Proxy;
	MyIsNoTrace = setting.NoTrace;

	//出力先がファイルで、ファイルが開いているならば閉じる
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}
	//コールバック関数が指定されている場合はクリアする
	MyCallback = nullptr;

	//新設定を適用
	if( setting.Mode == LogOutputMode::LOG_OUTPUT_CONSOLE )
	{
		;	//no additional operation required
	}
	else if( setting.Mode == LogOutputMode::LOG_OUTPUT_DEBUGGER )
	{
		;	//no additional operation required
	}
	else if( setting.Mode == LogOutputMode::LOG_OUTPUT_FILE )
	{
		const LogOutputModeOpt_File* setting_file = ( const LogOutputModeOpt_File*)&setting;

		DWORD creation = ( setting_file->IsAppend ) ? ( OPEN_ALWAYS ) : ( CREATE_ALWAYS );
		MyHandle = CreateFileW( setting_file->FileName.c_str() , GENERIC_WRITE , FILE_SHARE_READ , 0 , creation , FILE_FLAG_SEQUENTIAL_SCAN , 0 );
		if( MyHandle == INVALID_HANDLE_VALUE )
		{
			MyLogOutputMode = LogOutputMode::LOG_OUTPUT_NONE;
			return false;
		}
		SetFilePointer( MyHandle , 0 , 0 , FILE_END );
	}
	else if( setting.Mode == LogOutputMode::LOG_OUTPUT_CALLBACK )
	{
		const LogOutputModeOpt_Callback* setting_cb = ( const LogOutputModeOpt_Callback*)&setting;
		MyCallback = setting_cb->Callback;
	}
	else if( setting.Mode == LogOutputMode::LOG_OUTPUT_NONE )
	{
		;	//no additional operation required
	}
	else
	{
		//illegal setting
		return false;
	}
	return true;
}

bool mErrorLogger::ChangeLogOutputMode( LogOutputMode setting )
{

	mCriticalSectionTicket Ticket( MyCritical );

	//出力先がファイルで、ファイルが開いているならば閉じる
	if( MyHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle( MyHandle );
		MyHandle = INVALID_HANDLE_VALUE;
	}

	switch( setting )
	{
	case LOG_OUTPUT_CONSOLE:	//コマンドラインに出力する
	case LOG_OUTPUT_DEBUGGER:	//デバッガに出力する
	case LOG_OUTPUT_NONE:		//何もしない
		MyLogOutputMode = setting;
		return true;
	default:
		RaiseAssert( g_ErrorLogger , 0 , L"ログのコンソール出力モードに不正値が指定されました" );
		break;
	}
	return false;
}

DWORD RaiseErrorInternalF( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString mes2 , ... )
{
	if( obj == nullptr )
	{
		g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_ASSERT , file , line , 0 , 0 , L"エラー登録先オブジェクトがNULLです" , L"" );
		obj = &g_ErrorLogger;
	}

	//可変長リスト
	va_list args;
	va_start( args , mes2 );

	AString str;
	INT result = sprintf_va( str , mes2.c_str() , args );

	//可変長引数リセット
	va_end( args );

	return obj->AddEntry( level , file , line , ec1 , ec2 , AString2WString( mes1 ) , AString2WString( str ) );
}

DWORD RaiseErrorInternalF( mErrorLogger* obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString mes2 , ... )
{
	if( obj == nullptr )
	{
		g_ErrorLogger.AddEntry( mErrorLogger::ErrorLevel::LEVEL_ASSERT , file , line , 0 , 0 , L"エラー登録先オブジェクトがNULLです" , L"" );
		obj = &g_ErrorLogger;
	}

	//可変長リスト
	va_list args;
	va_start( args , mes2 );

	WString str;
	INT result = sprintf_va( str , mes2.c_str() , args );

	//可変長引数リセット
	va_end( args );

	return obj->AddEntry( level , file , line , ec1 , ec2 , mes1 , str );
}

DWORD RaiseErrorInternalF( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const AString& mes1 , const AString mes2 , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , mes2 );

	AString str;
	INT result = sprintf_va( str , mes2.c_str() , args );

	//可変長引数リセット
	va_end( args );

	return obj.AddEntry( level , file , line , ec1 , ec2 , AString2WString( mes1 ) , AString2WString( str ) );

}

DWORD RaiseErrorInternalF( mErrorLogger& obj , mErrorLogger::ErrorLevel level , const WString& file , DWORD line , DWORD ec1 , ULONG_PTR ec2 , const WString& mes1 , const WString mes2 , ... )
{
	//可変長リスト
	va_list args;
	va_start( args , mes2 );

	WString str;
	INT result = sprintf_va( str , mes2.c_str() , args );

	//可変長引数リセット
	va_end( args );

	return obj.AddEntry( level , file , line , ec1 , ec2 , mes1 , str );

}

DWORD mErrorLogger::GetErrorCount( mErrorLogger::ErrorLevel level )const
{
	switch( level )
	{
	case ErrorLevel::LEVEL_ASSERT:
	case ErrorLevel::LEVEL_ERROR:
	case ErrorLevel::LEVEL_EXCEPTION:
	case ErrorLevel::LEVEL_LOGGING:
		return MyErrorCount[ level ];
	default:
		break;
	}
	return 0;

}
