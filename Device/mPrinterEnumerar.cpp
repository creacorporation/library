//----------------------------------------------------------------------------
// プリンタ列挙クラス
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#include "mPrinterEnumerar.h"
#include "General/mErrorLogger.h"
#include <winspool.h>
#pragma comment(lib, "winspool.lib")

mPrinterEnumerar::mPrinterEnumerar() noexcept
	: mDeviceEnumerarBase( GUID_DEVCLASS_PRINTQUEUE )
{
}

mPrinterEnumerar::~mPrinterEnumerar()
{
}

static bool QueryDefaultPrinter( WString& retDefaultPrinterName )
{
	retDefaultPrinterName = L"";

	DWORD sz = 0;
	if( !GetDefaultPrinterW( nullptr , &sz ) )
	{
		switch( GetLastError() )
		{
		case ERROR_FILE_NOT_FOUND:
			//デフォルトプリンターは設定されていない
			return true;
		case ERROR_INSUFFICIENT_BUFFER:
			break;
		default:
			return false;
		}
	}
	std::unique_ptr<wchar_t[]> defprinter( mNew wchar_t[ sz ] );
	
	if( !GetDefaultPrinterW( defprinter.get() , &sz ) )
	{
		return false;
	}

	retDefaultPrinterName = defprinter.get();
	return true;
}

//デフォルトのプリンターの名前を得る
bool mPrinterEnumerar::GetDefaultPrinterName( WString& retName ) noexcept
{
	return QueryDefaultPrinter( retName );
}

bool mPrinterEnumerar::Reload( void )
{
	//前のデータを消す
	MyPrinterInfo.clear();

	//デフォルトのプリンタを取得
	WString default_printer;
	if( !QueryDefaultPrinter( default_printer ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"デフォルトのプリンター情報が取得できません" );
		return false;
	}

	if( !CreateCatalog() )
	{
		RaiseError( g_ErrorLogger , 0 , L"デバイス一覧を作成できません" );
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		//基本的な情報を移行
		bool result = true;
		mPrinterInfo::PrinterInfoEntry entry;

		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.Name );			//コントロールパネルの「フレンドリ名」と同じ
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//コントロールパネルの「デバイスの説明」と同じ

		//アレイに追加
		MyPrinterInfo.push_back( entry );
	}
	return true;
}

bool mPrinterEnumerar::GetPrinterInfo( PrinterInfo& retinfo , bool reload ) noexcept
{
	if( reload || MyPrinterInfo.empty() )
	{
		if( !Reload() )
		{
			retinfo.clear();
			return false;
		}
	}
	retinfo = MyPrinterInfo;
	return true;
}

const mPrinterEnumerar::PrinterInfo& mPrinterEnumerar::GetPrinterInfo( bool reload ) noexcept
{
	if( reload || MyPrinterInfo.empty() )
	{
		Reload();
	}
	return MyPrinterInfo;
}

static_assert( sizeof( PRINTER_INFO_8 ) == sizeof( PRINTER_INFO_9 ) , "there is size difference between info8 and info9" );
static bool GetDevModeStructure( HANDLE handle , DWORD level , std::unique_ptr<PRINTER_INFO_8>& retDevmode )
{
	switch( level )
	{
	case 8: // PRINTER_INFO_8
	case 9: // PRINTER_INFO_9
		break;
	default:
		//対応外
		return false;
	}

	DWORD sz = 0;
	if( !GetPrinterW( handle , level , nullptr , 0 , &sz ) )
	{
		switch( GetLastError() )
		{
		case ERROR_INSUFFICIENT_BUFFER:
			break;
		default:
			RaiseError( g_ErrorLogger , 0 , L"プリンター情報が取得できません" );
			return false;
		}
	}
	if( sz == sizeof( PRINTER_INFO_8 ) )
	{
		//サイズが構造体のサイズそのものの場合データなし
		return false;
	}

	retDevmode.reset( (PRINTER_INFO_8*)mNew BYTE[ sz ] );
	if( !GetPrinterW( handle , level , (BYTE*)retDevmode.get() , sz , &sz ) )
	{
		//取得できず
		return false;
	}
	if( ( sz <= sizeof( PRINTER_INFO_8 ) ) ||
		( ((PRINTER_INFO_8*)retDevmode.get())->pDevMode == nullptr ) )
	{
		//データなしz
		return false;
	}

	return true;
}

bool FillDevModeStructure( const std::unique_ptr<PRINTER_INFO_8>& info , mPrinterEnumerar::PrinterProperty& retprop )
{
	if( !info || info->pDevMode == nullptr )
	{
		return false;
	}
	const DEVMODE* dev = info->pDevMode;

	int rc;
	//紙の情報を取得
	{
		//エントリ数を調べる
		int count = DeviceCapabilities( dev->dmDeviceName , L"" , DC_PAPERNAMES , nullptr , dev );
		if( count <= 0 )
		{
			//紙の数が分からない
			RaiseError( g_ErrorLogger , 0 , L"プリンター情報に用紙情報がありません" );
			return false;
		}
		std::unique_ptr<wchar_t[]> papernames( mNew wchar_t[ count * 64 ] );	//←1エントリ64文字固定
		std::unique_ptr<WORD[]> papers( mNew WORD[ count ] );
		std::unique_ptr<POINT[]> sizes( mNew POINT[ count ] );

		rc = DeviceCapabilities( dev->dmDeviceName , L"" , DC_PAPERNAMES , papernames.get() , dev );
		rc = DeviceCapabilities( dev->dmDeviceName , L"" , DC_PAPERS , (LPWSTR)papers.get() , dev );
		rc = DeviceCapabilities( dev->dmDeviceName , L"" , DC_PAPERSIZE , (LPWSTR)sizes.get() , dev );

		for( int i = 0 ; i < count ; i++ )
		{
			mPrinterInfo::PaperInfoEntry entry;
			if( papernames[ i * 64 + 63 ] == L'\0' )
			{
				//ヌル終端されている
				entry.FriendlyName = &papernames.get()[ i * 64 ];
			}
			else
			{
				//ヌル終端されていないので一旦コピーする
				wchar_t tmp_name[ 65 ];
				MoveMemory( tmp_name , &papernames[ i * 64 ] , 64 * sizeof( wchar_t ) );
				tmp_name[ 64 ] = L'\0';
				entry.FriendlyName = tmp_name;
			}

			entry.Id = papers[ i ];
			entry.Width = sizes[ i ].x;
			entry.Height = sizes[ i ].y;
			retprop.AcceptablePaper.push_back( std::move( entry ) );
		}
	}
	//カラー？
	retprop.IsColor = DeviceCapabilities( dev->dmDeviceName , L"" , DC_COLORDEVICE , nullptr , dev );
	//両面対応？
	retprop.IsDuplex = DeviceCapabilities( dev->dmDeviceName , L"" , DC_DUPLEX , nullptr , dev );
	//DPI
	retprop.Dpi = dev->dmPrintQuality;

	return true;
}

bool mPrinterEnumerar::GetPrinterProperty( const WString& name , PrinterProperty& retProperty ) noexcept
{
	bool result = false;
	HANDLE handle;
	if( !OpenPrinterW( const_cast<wchar_t*>( name.c_str() ) , &handle , nullptr ) )
	{
		return false;
	}

	//https://docs.microsoft.com/ja-jp/windows/win32/printdocs/getprinter
	//まずはLv9 (ユーザーごとの設定)で取得して、
	//取得できなければLv8(グローバルの設定)で取得する。
	//どちらも取れなければギブアップ
	std::unique_ptr<PRINTER_INFO_8> info;
	if( GetDevModeStructure( handle , 9 , info ) )
	{
		//レベル9で取得成功
		;
	}
	else if( GetDevModeStructure( handle , 8 , info ) )
	{
		//レベル8で取得成功
		;
	}
	else
	{
		//取れない
		RaiseError( g_ErrorLogger , 0 , L"プリンター情報の取得が失敗しました" );
		goto err;
	}

	if( !FillDevModeStructure( info , retProperty ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"プリンター情報を読み取れませんでした" );
		goto err;
	}

	result = true;
err:
	ClosePrinter( handle );
	return result;
}




