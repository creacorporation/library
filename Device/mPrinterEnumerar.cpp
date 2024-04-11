//----------------------------------------------------------------------------
// �v�����^�񋓃N���X
// Copyright (C) 2019- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
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
			//�f�t�H���g�v�����^�[�͐ݒ肳��Ă��Ȃ�
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

//�f�t�H���g�̃v�����^�[�̖��O�𓾂�
bool mPrinterEnumerar::GetDefaultPrinterName( WString& retName ) noexcept
{
	return QueryDefaultPrinter( retName );
}

bool mPrinterEnumerar::Reload( void )
{
	//�O�̃f�[�^������
	MyPrinterInfo.clear();

	//�f�t�H���g�̃v�����^���擾
	WString default_printer;
	if( !QueryDefaultPrinter( default_printer ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�f�t�H���g�̃v�����^�[��񂪎擾�ł��܂���" );
		return false;
	}

	if( !CreateCatalog() )
	{
		RaiseError( g_ErrorLogger , 0 , L"�f�o�C�X�ꗗ���쐬�ł��܂���" );
		return false;
	}

	for( DWORD i = 0 ; i < MyDevInfoData.size() ; i++ )
	{
		//��{�I�ȏ����ڍs
		bool result = true;
		mPrinterInfo::PrinterInfoEntry entry;

		result &= GetProperty( i , SPDRP_FRIENDLYNAME , entry.Name );			//�R���g���[���p�l���́u�t�����h�����v�Ɠ���
		result &= GetProperty( i , SPDRP_DEVICEDESC , entry.Description );		//�R���g���[���p�l���́u�f�o�C�X�̐����v�Ɠ���

		//�A���C�ɒǉ�
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
		//�Ή��O
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
			RaiseError( g_ErrorLogger , 0 , L"�v�����^�[��񂪎擾�ł��܂���" );
			return false;
		}
	}
	if( sz == sizeof( PRINTER_INFO_8 ) )
	{
		//�T�C�Y���\���̂̃T�C�Y���̂��̂̏ꍇ�f�[�^�Ȃ�
		return false;
	}

	retDevmode.reset( (PRINTER_INFO_8*)mNew BYTE[ sz ] );
	if( !GetPrinterW( handle , level , (BYTE*)retDevmode.get() , sz , &sz ) )
	{
		//�擾�ł���
		return false;
	}
	if( ( sz <= sizeof( PRINTER_INFO_8 ) ) ||
		( ((PRINTER_INFO_8*)retDevmode.get())->pDevMode == nullptr ) )
	{
		//�f�[�^�Ȃ�z
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
	//���̏����擾
	{
		//�G���g�����𒲂ׂ�
		int count = DeviceCapabilities( dev->dmDeviceName , L"" , DC_PAPERNAMES , nullptr , dev );
		if( count <= 0 )
		{
			//���̐���������Ȃ�
			RaiseError( g_ErrorLogger , 0 , L"�v�����^�[���ɗp����񂪂���܂���" );
			return false;
		}
		std::unique_ptr<wchar_t[]> papernames( mNew wchar_t[ count * 64 ] );	//��1�G���g��64�����Œ�
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
				//�k���I�[����Ă���
				entry.FriendlyName = &papernames.get()[ i * 64 ];
			}
			else
			{
				//�k���I�[����Ă��Ȃ��̂ň�U�R�s�[����
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
	//�J���[�H
	retprop.IsColor = DeviceCapabilities( dev->dmDeviceName , L"" , DC_COLORDEVICE , nullptr , dev );
	//���ʑΉ��H
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
	//�܂���Lv9 (���[�U�[���Ƃ̐ݒ�)�Ŏ擾���āA
	//�擾�ł��Ȃ����Lv8(�O���[�o���̐ݒ�)�Ŏ擾����B
	//�ǂ�������Ȃ���΃M�u�A�b�v
	std::unique_ptr<PRINTER_INFO_8> info;
	if( GetDevModeStructure( handle , 9 , info ) )
	{
		//���x��9�Ŏ擾����
		;
	}
	else if( GetDevModeStructure( handle , 8 , info ) )
	{
		//���x��8�Ŏ擾����
		;
	}
	else
	{
		//���Ȃ�
		RaiseError( g_ErrorLogger , 0 , L"�v�����^�[���̎擾�����s���܂���" );
		goto err;
	}

	if( !FillDevModeStructure( info , retProperty ) )
	{
		RaiseError( g_ErrorLogger , 0 , L"�v�����^�[����ǂݎ��܂���ł���" );
		goto err;
	}

	result = true;
err:
	ClosePrinter( handle );
	return result;
}




