//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�v�����^�[�p�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2019 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

#define MGDIPRINTERDC_CPP_COMPILING
#include "mGdiPrinterDC.h"
#include "General/mErrorLogger.h"
#include "../General/mException.h"


mGdiPrinterDC::mGdiPrinterDC() noexcept
{
}

mGdiPrinterDC::~mGdiPrinterDC()
{
	if( MyHdc != nullptr )
	{
		Abort();
		RaiseError( g_ErrorLogger , 0 , L"����r���ŃI�u�W�F�N�g��j�����܂���" );
	}
	return;
}

bool mGdiPrinterDC::Open( const Option& opt ) noexcept
{
	if( MyHdc != nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���d�ɊJ�����Ƃ��܂���" );
		return false;
	}

	//�쐬���@�̎w��ɉ��������@�Ńv�����^���J��
	if( opt.method == Option::CreateMethod::USEOPTION )
	{
		//�ݒ�̕ێ�
		const Option_UseOption* op = (const Option_UseOption*)&opt;
		MyOption.reset( mNew Option_UseOption( *op ) );

		//�f�o�C�X�R���e�L�X�g���J��
		MyHdc = CreateDCW( nullptr , op->PrinterName.c_str() , nullptr , nullptr );
		if( MyHdc == nullptr )
		{
			RaiseError( g_ErrorLogger , 0 , L"�v�����^���J�����Ƃ��ł��܂���" , op->PrinterName );
			return false;
		}

		//�h�L�������g�̊J�n
		DOCINFO doc = {};
		doc.cbSize = sizeof( doc );
		doc.lpszDocName = op->DocumentName.c_str();
		if( op->OutFileName == L"" )
		{
			doc.lpszOutput = nullptr;
		}
		else
		{
			doc.lpszOutput = op->OutFileName.c_str();
		}
		doc.lpszDatatype = nullptr;
		doc.fwType = 0;

		SetLastError( 0 );
		if( StartDocW( MyHdc , &doc ) <= 0 )
		{
			RaiseError( g_ErrorLogger , 0 , L"�h�L�������g���J�����Ƃ��ł��܂���" , op->PrinterName );
			return false;
		}
	}
	else
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^�I�v�V�������s���ł�" );
		return false;
	}
	return true;
}

bool mGdiPrinterDC::Close( void ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	SetLastError( 0 );
	if( ::EndDoc( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"������I���ł��܂���ł���" , MyOption->DocumentName );
		return false;
	}

	//�I������Ă���GDI�I�u�W�F�N�g��S�����ɖ߂�
	//����������Ȃ��ƃ��\�[�X���[�N���邩��
	ResetSelectedObject();

	//�f�o�C�X�R���e�L�X�g�ƃI�v�V�����͂����v��Ȃ�����j������
	DeleteDC( MyHdc );
	MyHdc = nullptr;
	MyOption.reset();
	return true;
}

bool mGdiPrinterDC::Abort( void ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	SetLastError( 0 );
	if( ::AbortDoc( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"����̒��~���������s���܂���" , MyOption->DocumentName );
		return false;
	}

	//�I������Ă���GDI�I�u�W�F�N�g��S�����ɖ߂�
	//����������Ȃ��ƃ��\�[�X���[�N���邩��
	ResetSelectedObject();

	//�f�o�C�X�R���e�L�X�g�ƃI�v�V�����͂����v��Ȃ�����j������
	DeleteDC( MyHdc );
	MyHdc = nullptr;
	MyOption.reset();
	return true;
}

static bool PageSetup_StandardSize( HDC hdc , const mGdiPrinterDC::PageOption_StandardSize& opt )
{
	DEVMODEW devmode = {0};
	devmode.dmSpecVersion = DM_SPECVERSION;
	devmode.dmSize = sizeof( devmode );

	//�p���T�C�Y�̐ݒ�
	devmode.dmFields |= DM_PAPERSIZE;
	devmode.dmPaperSize = static_cast<short>( opt.Paper );

	//�p���̌���
	switch( opt.Orientation )
	{
	case mGdiPrinterDC::PageOption_StandardSize::PaperOrientation::PAPER_LANDSCAPE:
		devmode.dmFields |= DM_ORIENTATION;
		devmode.dmOrientation = DMORIENT_LANDSCAPE;
		break;
	case mGdiPrinterDC::PageOption_StandardSize::PaperOrientation::PAPER_PORTRAIT:
		devmode.dmFields |= DM_ORIENTATION;
		devmode.dmOrientation = DMORIENT_PORTRAIT;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"�p���T�C�Y�̐ݒ肪�Ԉ���Ă��܂�" , opt.Orientation );
		break;
	}

	//�J���[����H
	devmode.dmFields |= DM_COLOR;
	devmode.dmColor = ( opt.IsColor ) ? ( DMCOLOR_COLOR ) : ( DMCOLOR_MONOCHROME );

	//���ʈ���ݒ�
	switch( opt.Duplex )
	{
	case mGdiPrinterDC::PageOption_StandardSize::PaperDuplex::PAPER_SIMPLEX:
		devmode.dmFields |= DM_DUPLEX;
		devmode.dmDuplex = DMDUP_SIMPLEX;
		break;
	case mGdiPrinterDC::PageOption_StandardSize::PaperDuplex::PAPER_DUPLEX_HORIZONTAL:
		devmode.dmFields |= DM_DUPLEX;
		devmode.dmDuplex = DMDUP_HORIZONTAL;
		break;
	case mGdiPrinterDC::PageOption_StandardSize::PaperDuplex::PAPER_DUPLEX_VERTICAL:
		devmode.dmFields |= DM_DUPLEX;
		devmode.dmDuplex = DMDUP_VERTICAL;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"���ʈ���̐ݒ肪�Ԉ���Ă��܂�" , opt.Duplex );
		break;
	}

	//�ݒ�
	ResetDCW( hdc , &devmode );
	return true;
}

static bool PageSetup_Nochange( HDC hdc , const mGdiPrinterDC::PageOption_Nochange& opt )
{
	return true;
}

bool mGdiPrinterDC::StartPage( const PageOption& opt ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	//�p���ݒ�̕ύX
	bool rc;
	if( opt.method == PageOption::CreateMethod::STANDARD_SIZE )
	{
		//�K�i�T�C�Y�ɕύX����ꍇ
		rc = PageSetup_StandardSize( MyHdc , (PageOption_StandardSize&)opt );
	}
	else if( opt.method == PageOption::CreateMethod::NOCHANGE )
	{
		//�O�y�[�W�Ɠ����̏ꍇ
		rc = PageSetup_Nochange( MyHdc , (PageOption_Nochange&)opt );
	}
	else
	{
		rc = false;
	}
	if( !rc )
	{
		RaiseError( g_ErrorLogger , 0 , L"�p���ݒ��ύX�ł��܂���ł���" , MyOption->DocumentName );
	}

	SetLastError( 0 );
	if( ::StartPage( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�y�[�W���J�n�ł��܂���ł���" , MyOption->DocumentName );
		return false;
	}
	return true;
}

bool mGdiPrinterDC::EndPage( void ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	SetLastError( 0 );
	if( ::EndPage( MyHdc ) <= 0 )
	{
		RaiseError( g_ErrorLogger , 0 , L"�y�[�W���I���ł��܂���ł���" , MyOption->DocumentName );
		return false;
	}
	return true;
}

bool mGdiPrinterDC::GetPrintableSize( SIZE& retSize ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	int sx = GetDeviceCaps( MyHdc , HORZRES );					//��ʂ̕�(�s�N�Z���P��)
	int sy = GetDeviceCaps( MyHdc , VERTRES );					//��ʂ̍���(�s�N�Z���P��)

	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	retSize.cx = ( LONG )( ( 254.0 * sx ) / dpi_x ) - 1;		//�v�Z�l��1�s�N�Z�������܂ł�����\�͈�
	retSize.cy = ( LONG )( ( 254.0 * sy ) / dpi_y ) - 1;		//
	return true;
}

//�p���̃T�C�Y�𓾂�
bool mGdiPrinterDC::GetPaperSize( SIZE& retSize ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	int sx = GetDeviceCaps( MyHdc , PHYSICALWIDTH );			//��ʂ̕�(�f�o�C�X�P��)
	int sy = GetDeviceCaps( MyHdc , PHYSICALHEIGHT );			//��ʂ̍���(�f�o�C�X�P��)

	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	retSize.cx = ( LONG )( ( 254.0 * sx ) / dpi_x + 0.5 );
	retSize.cy = ( LONG )( ( 254.0 * sy ) / dpi_y + 0.5 );
	return true;
}

bool mGdiPrinterDC::GetPrintableMargin( RECT& retMargin ) noexcept
{
	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	int margin_x = GetDeviceCaps( MyHdc , PHYSICALOFFSETX );	//���}�[�W��(�f�o�C�X�P��)
	int margin_y = GetDeviceCaps( MyHdc , PHYSICALOFFSETY );	//��}�[�W��(�f�o�C�X�P��)
	int paper_w = GetDeviceCaps( MyHdc , PHYSICALWIDTH );		//�p���̕�(�f�o�C�X�P��)
	int paper_h = GetDeviceCaps( MyHdc , PHYSICALHEIGHT );		//�p���̍���(�f�o�C�X�P��)
	int print_w = GetDeviceCaps( MyHdc , HORZRES );				//����\�͈͂̕�(�s�N�Z���P��)
	int print_h = GetDeviceCaps( MyHdc , VERTRES );				//����\�͈͂̍���(�s�N�Z���P��)
	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	SIZE papersize;
	papersize.cx = ( LONG )( ( 254.0 * paper_w ) / dpi_x );
	papersize.cy = ( LONG )( ( 254.0 * paper_h ) / dpi_y );

	SIZE printsize;
	printsize.cx = ( LONG )( ( 254.0 * print_w ) / dpi_x );
	printsize.cy = ( LONG )( ( 254.0 * print_h ) / dpi_y );

	retMargin.left = ( LONG )( ( 254.0 * margin_x ) / ( dpi_x ) ) + 1;
	retMargin.top = ( LONG )( ( 254.0 * margin_y ) / ( dpi_y ) ) + 1;
	retMargin.right = papersize.cx - ( retMargin.left + printsize.cx ) + 1;
	retMargin.bottom = papersize.cy - ( retMargin.top + printsize.cy ) + 1;
	return true;
}

bool mGdiPrinterDC::SetView( Origin origin , const SIZE& size ) noexcept
{
	RECT margin;
	margin.left = 0;
	margin.top = 0;
	margin.right = 0;
	margin.bottom = 0;

	return SetView( origin , size , margin );
}

bool mGdiPrinterDC::SetView( Origin origin , const SIZE& size , const RECT& margin ) noexcept
{

	if( MyHdc == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"�v�����^���J����Ă��܂���" );
		return false;
	}

	int dpi_x = GetDeviceCaps( MyHdc , LOGPIXELSX );			//DPI
	int dpi_y = GetDeviceCaps( MyHdc , LOGPIXELSY );			//DPI

	RECT margin_dev;
	margin_dev.left = (LONG)( margin.left / 254.0 * dpi_x );
	margin_dev.top = (LONG)( margin.top / 254.0 * dpi_y );
	margin_dev.right = (LONG)( margin.right / 254.0 * dpi_x );
	margin_dev.bottom = (LONG)( margin.bottom / 254.0 * dpi_y );

	using err = class ErrorClass : public mExceptionBase{};
	try
	{
		if( origin == Origin::ORIGIN_PRINTABLEAREA )
		{
			Check<err>( SetMapMode( MyHdc , MM_ANISOTROPIC) );
			int sx = GetDeviceCaps( MyHdc , HORZRES );				//��ʂ̕�(�f�o�C�X�P��)
			int sy = GetDeviceCaps( MyHdc , VERTRES );				//��ʂ̍���(�f�o�C�X�P��)		
			Check<err>( SetViewportExtEx( MyHdc , 
				sx - MyOption->PrintAreaMargin * 2 - margin_dev.left - margin_dev.right ,
				sy - MyOption->PrintAreaMargin * 2 - margin_dev.top - margin_dev.bottom ,
				nullptr ) );	//�_�����W�n
			Check<err>( SetWindowExtEx( MyHdc , size.cx , size.cy , nullptr ) );	//�������W�n

			Check<err>( SetViewportOrgEx( MyHdc ,
				MyOption->PrintAreaMargin + margin_dev.left ,
				MyOption->PrintAreaMargin + margin_dev.top ,
				nullptr ) );
		}
		else if( origin == Origin::ORIGIN_PAPEREDGE )
		{
			Check<err>( SetMapMode( MyHdc , MM_ANISOTROPIC) );
			int px = GetDeviceCaps( MyHdc , PHYSICALWIDTH );		//��ʂ̕�(�f�o�C�X�P��)
			int py = GetDeviceCaps( MyHdc , PHYSICALHEIGHT );		//��ʂ̍���(�f�o�C�X�P��)		
			Check<err>( SetViewportExtEx( MyHdc ,
				px - MyOption->PrintAreaMargin * 2 - margin_dev.left - margin_dev.right ,
				py - MyOption->PrintAreaMargin * 2 - margin_dev.top - margin_dev.bottom ,
				nullptr ) );	//�_�����W�n
			Check<err>( SetWindowExtEx( MyHdc , size.cx , size.cy , nullptr ) );	//�������W�n

			int margin_l = GetDeviceCaps( MyHdc , PHYSICALOFFSETX );		//���}�[�W��(�f�o�C�X�P��)
			int margin_u = GetDeviceCaps( MyHdc , PHYSICALOFFSETY );		//��}�[�W��(�f�o�C�X�P��)
			Check<err>( SetViewportOrgEx( MyHdc ,
				MyOption->PrintAreaMargin + margin_dev.left - margin_l ,
				MyOption->PrintAreaMargin + margin_dev.top - margin_u,
				nullptr ) );
		}
		else
		{
			RaiseError( g_ErrorLogger , 0 , L"���_�̐ݒ�l���ُ�ł�" , MyOption->DocumentName );
			return false;
		}
	}
	catch( err& e )
	{
		SetLastError( e.GetErrorCode() );
		RaiseError( g_ErrorLogger , 0 , L"���W�ݒ肪���s���܂���" , MyOption->DocumentName );
		return false;
	}

	return true;
}


