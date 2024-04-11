//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

#define MGDIDC_CPP_COMPILING
#include "mGdiDC.h"
#include "General/mErrorLogger.h"

#pragma comment( lib , "msimg32.lib" )

mGdiDC::mGdiDC()
{
	MyHdc = nullptr;
}

mGdiDC::~mGdiDC()
{
	//MyHdc�̃n���h�����������ӔC�͔h���N���X���ɂ���܂�

	//�f�o�C�X�R���e�L�X�g�̃n���h����������ꂽ�̂ɁA�f�t�H���g�̃I�u�W�F�N�g��
	//�c�����Ă���ꍇ�̓��\�[�X���[�N���Ă������Ȃ̂ŃG���[���L�^���܂��B
	if( MyDefaultObj.size() != 0 )
	{
		//ResetSelectedObject()���Ăяo���Y��Ă���
		RaiseAssert( g_ErrorLogger , MyDefaultObj.size() , L"HGDIOBJ is not detached" );
	}
}

//�I�u�W�F�N�g(�y���A�u���V�A�t�H���g�Ȃ�)��I������
bool mGdiDC::Select( const mGdiHandle& handle )
{
	return Select( handle.GetHandle() );
}

//�I�u�W�F�N�g(�y���A�u���V�A�t�H���g�Ȃ�)��I������
//��SelectObject��̃n���h���̈�����
//�E������֘A�t���悤�Ƃ��Ă���I�u�W�F�N�g�̑f���́H
// [A]MyAttachedObj�ɍڂ��Ă�˂��łɊ֘A�t���ς݁B�ʂ�SelectObject���̂��K�v�Ȃ�
// [B]MyDefaultObj�ɍڂ��Ă�˃f�t�H���g�̃I�u�W�F�N�g�ɖ߂邩��MyDefaultObj����폜
// [C]�ǂ���ɂ��ڂ��ĂȂ��ːV�K�̃I�u�W�F�N�g������MyAttachedObj�ɓo�^
//
//�ESelectObject�Ŋ֘A�t�������ʕԂ��Ă����I�u�W�F�N�g�́H
// [a]MyAttachedObj�ɍڂ��Ă�˃��[�U���O�Ɋ֘A�t�������̂�����MyAttachedObj����폜
// [b]��������Ȃ��˃f�t�H���g�̃I�u�W�F�N�g������MyDefaultObj�ɓo�^
bool mGdiDC::Select( HGDIOBJ new_object )
{
	//ID��������������G���[
	if( new_object == nullptr )
	{
		RaiseAssert( g_ErrorLogger , 0 , L"Object null" );
		return false;
	}

	HGDIOBJ prev_object = ::SelectObject( MyHdc , new_object );
	if( prev_object == nullptr || prev_object == HGDI_ERROR )
	{
		//SelectObject�����s�����ꍇ
		RaiseAssert( g_ErrorLogger , 0 , L"SelectObject failed" );
		return false;
	}

	//�ύX�O�ƌ�̃n���h���̑f���𒲂ׂāAMyAttachedObj���X�V����
	if( MyAttachedObj.count( new_object ) )
	{
		//���łɑI���ς݂̃I�u�W�F�N�g���đI�����Ă���ꍇ�B
		//�����ω����ĂȂ����炻�̂܂ܖ߂�B
		return true;	//��̃R�����g��[A]�̃p�^�[��
	}
	else if( MyDefaultObj.count( new_object ) )
	{
		MyDefaultObj.erase( new_object );	//��̃R�����g��[B]�̃p�^�[��
	}
	else
	{
		if( MyAttachedObj.count( new_object ) == 0 )
		{
			MyAttachedObj.insert( new_object );	//��̃R�����g��[C]�̃p�^�[��
		}
		else
		{
			//���W�b�N�㗈�Ȃ��͂��c
			RaiseAssert( g_ErrorLogger , 0 , L"AttachedObj dupe" );
		}
	}
	//���l��MyDefaultObj���X�V����
	if( MyAttachedObj.count( prev_object ) )
	{
		MyAttachedObj.erase( prev_object );	//��̃R�����g��[a]�̃p�^�[��
	}
	else
	{
		if( MyDefaultObj.count( prev_object ) == 0 )
		{
			MyDefaultObj.insert( prev_object );	//��̃R�����g��[b]�̃p�^�[��
		}
		else
		{
			//���W�b�N�㗈�Ȃ��͂��c
			RaiseAssert( g_ErrorLogger , 0 , L"DefaultObj dupe" );
		}
	}
	return true;
}

//�I�u�W�F�N�g(�y���A�u���V�A�t�H���g�Ȃ�)��I������
bool mGdiDC::Select( const mGdiResource& res , const WString& id , const WString& subid )
{
	return Select( res.GetItem( id , subid ) );
}

//�f�o�C�X�R���e�L�X�g�Ɋ֘A�t�����Ă���I�u�W�F�N�g��S�����ɖ߂�
bool mGdiDC::ResetSelectedObject( void )
{
	//�S���̃I�u�W�F�N�g���f�t�H���g�ɖ߂�
	for( GdiObjectPool::iterator itr = MyDefaultObj.begin() ; itr != MyDefaultObj.end() ; itr++ )
	{
		::SelectObject( MyHdc , *itr );
	}

	//�S�Ă��f�t�H���g�ɖ߂��Ă���͂�������A�v�[�����Ă���I�u�W�F�N�g�̂��Ƃ͖Y���
	MyAttachedObj.clear();
	MyDefaultObj.clear();
	return true;
}

//����`�悷��
bool mGdiDC::Line( INT to_x , INT to_y )
{
	return ::LineTo( MyHdc , to_x , to_y ) != FALSE;
}

//����`�悷��
bool mGdiDC::Line( INT from_x , INT from_y , INT to_x , INT to_y )
{
	if( !::MoveToEx( MyHdc , from_x , from_y , nullptr ) )
	{
		return false;
	}
	return Line( to_x , to_y );
}


bool mGdiDC::LineOffset( INT from_x , INT from_y , INT offset_x , INT offset_y )
{
	return Line( from_x , from_y , from_x + offset_x , from_y + offset_y );
}


//��`��`�悷��
bool mGdiDC::Rectangle( INT x1 , INT y1 , INT x2 , INT y2 )
{
	//WindowsAPI��Rectangle�͉E�ӂƒ�ӂ͍��W�Ɋ܂܂Ȃ��i1�s�N�Z�������ɉE�ӂƒ�ӂ�`�悷��j
	//�Ƃ����d�l�炵���B�Ȃ񂾂��s���Ƃ��Ȃ��̂ŁA1�s�N�Z���O���ɂ��炵�ĕ␳����B
	//�Ȃ�������āA�葱��BASIC���ゾ����ȁI
	PositionConvert( x1 , y1 , x2 , y2 );

	//�`��n��API�ŃG���[���L�^����ƁA�������O�����ӂ��̂ŋL�^���Ȃ�
	return ::Rectangle( MyHdc , x1 , y1 , x2 , y2 ) != FALSE;
}

bool mGdiDC::RectangleOffset( INT x1 , INT y1 , INT offset_x , INT offset_y )
{
	return Rectangle( x1 , y1 , x1 + offset_x , y1 + offset_y );
}

//�~��`�悷��
bool mGdiDC::Circle( INT x , INT y , INT radius )
{
	return Circle( x - radius , y - radius , x + radius , y + radius );
}

//�~��`�悷��
bool mGdiDC::Circle( INT x1 , INT y1 , INT x2 , INT y2 )
{
	//Rectangle�Ɠ����ŁAEllipse���E�ӂƒ�ӂ͊܂܂Ȃ��̂ŕ␳����
	PositionConvert( x1 , y1 , x2 , y2 );

	//�`��n��API�ŃG���[���L�^����ƁA�������O�����ӂ��̂ŋL�^���Ȃ�
	return ::Ellipse( MyHdc , x1 , y1 , x2 , y2 ) != FALSE;
}

//���݂̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::Print( const WString& str )
{
	POINT current;
	if( !GetCurrentPositionEx( MyHdc , &current ) )
	{
		return false;
	}
	return Print( str , current.x ,  current.y );
}

//�w��̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::Print( const WString& str , INT x , INT y )
{
	return TextOutW( MyHdc , x , y , str.c_str() , (int)str.length() );
}

//���W�ϊ�
void mGdiDC::PositionConvert( INT& x1 , INT& y1 , INT& x2 , INT&y2 )const
{
	INT left;
	INT right;
	if( x1 < x2 )
	{
		left = x1;
		right = x2 + 1;	//������+1���␳��
	}
	else
	{
		left = x2;
		right = x1 + 1;
	}
	INT top;
	INT bottom;
	if( y1 < y2 )
	{
		top = y1;
		bottom = y2 + 1;
	}
	else
	{
		top = y2;
		bottom = y1 + 1;

	}

	x1 = left;
	y1 = top;
	x2 = right;
	y2 = bottom;
	return;
}

//�w��͈͂��w��͈͂ɃR�s�[����(���̂P)
bool mGdiDC::Copy( const mGdiDC& srcdc , 
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 , 
	INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,
	DWORD raster )
{
	//���W�ϊ�
	PositionConvert( src_x1 , src_y1 , src_x2 , src_y2 );
	PositionConvert( dst_x1 , dst_y1 , dst_x2 , dst_y2 );

	INT src_w = src_x2 - src_x1;	//�\�[�X�̕�
	INT src_h = src_y2 - src_y1;	//�\�[�X�̍���
	INT dst_w = dst_x2 - dst_x1;	//�\��t����̕�
	INT dst_h = dst_y2 - dst_y1;	//�\��t����̍���

	//�R�s�[���ƃR�s�[��ŕ�������ł��邩�𔻒肵�ABitblt���g�����AStretchBlt���g�������߂�
	if( ( src_w == dst_w ) && ( src_h == dst_h ) )
	{
		//���ƍ����������ł��邩��bitblt
		return ::BitBlt( MyHdc, dst_x1 , dst_y1 , src_w , src_h , srcdc.MyHdc , src_x1 , src_y1 , raster ) != FALSE;
	}
	else
	{
		//���ƍ������قȂ��Ă���̂�StretchBlt
		return ::StretchBlt( MyHdc , dst_x1 , dst_y1 , dst_w , dst_h , srcdc.MyHdc , src_x1 , src_y1 , src_w , src_h , raster ) != FALSE;
	}
}


//�w��͈͂��w��͈͂ɃR�s�[����(���̂Q)
bool mGdiDC::Copy( const mGdiDC& srcdc ,
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,
	INT dst_x1 , INT dst_y1 , DWORD raster )
{
	//�R�s�[��̉E�E���̍��W���A�R�s�[���̕��E�������g���ċ��߂�
	INT width;	//��
	if( src_x1 < src_x2 )
	{
		width = src_x2 - src_x1 + 1;
	}
	else
	{
		width = src_x1 - src_x2 + 1;
	}
	INT height;	//����
	if( src_x1 < src_x2 )
	{
		height = src_y2 - src_y1 + 1;
	}
	else
	{
		height = src_y1 - src_y2 + 1;
	}

	//���߂����E���������āA�u���̂P�v���Ăяo���B���s�o�ς��H�ǂ��ł��������B
	return Copy( srcdc , src_x1 , src_y1 , src_x2 , src_y2 , dst_x1 , dst_y1 , dst_x1 + width , dst_y1 + height , raster );
}

//�w��͈͂��w��͈͂ɃR�s�[����(���̂R)
bool mGdiDC::Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , DWORD raster )
{
	//���W�͓����Ȃ̂ŁA�R�s�[��ɂ��R�s�[���̍��W�����̂܂܎g�����Ⴂ�܂�
	return Copy( srcdc , x1 , y1 , x2 , y2 , x1 , y1 , x2 , y2 , raster );
}

bool mGdiDC::GetPrintSize( const WString& str , SIZE& retSize )
{
	return GetTextExtentPoint32W( MyHdc , str.c_str() , (int)str.length() , &retSize );
}

static bool MakeDrawTextParameter( const mGdiDC::PrintOptions& opt , UINT& retformat , DRAWTEXTPARAMS& retparams )
{
	bool result = true;
	retformat = 0;
	retformat |= DT_NOPREFIX;

	retparams = { 0 };
	retparams.cbSize = sizeof( DRAWTEXTPARAMS );

	//�����ʒu
	switch( opt.VerticalAlign )
	{
	case mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_TOP:
		//retformat |= DT_TOP;
		break;
	case mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_CENTER:
		retformat |= DT_VCENTER;
		retformat |= DT_SINGLELINE;
		break;
	case mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_BOTTOM:
		retformat |= DT_BOTTOM;
		retformat |= DT_SINGLELINE;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"�����ʒu���s���ł�" );
		result = false;
		break;
	}

	//�����ʒu
	switch( opt.HorizontalAlign )
	{
	case mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_LEFT:
		retformat |= DT_LEFT;
		break;
	case mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_CENTER:
		retformat |= DT_CENTER;
		break;
	case mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_RIGHT:
		retformat |= DT_RIGHT;
		break;
	default:
		RaiseError( g_ErrorLogger , 0 , L"�����ʒu���s���ł�" );
		result = false;
		break;
	}

	//�^�u�̓W�J
	if( 2 <= opt.TabSize )
	{
		retparams.iTabLength = opt.TabSize;
		retformat |= DT_EXPANDTABS;		//�^�u�̓W�J���s��
		retformat |= DT_TABSTOP;		//�^�u�T�C�Y�̐ݒ���s��
	}
	return true;
}

//���݂̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::Print( const WString& str , const PrintOptions& opt )
{
	//���݂̈ʒu���擾����
	POINT current;
	if( !GetCurrentPositionEx( MyHdc , &current ) )
	{
		return false;
	}

	//�`��
	return Print( str , current.x , current.y , opt );
}

//�w��̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::Print( const WString& str , INT x , INT y , const PrintOptions& opt )
{
	//API�ɓn���t�H�[�}�b�g�����쐬
	UINT format ;
	DRAWTEXTPARAMS option;

	MakeDrawTextParameter( opt , format , option );
	format |= DT_NOCLIP;	//�����サ���w�肳��Ă��Ȃ��̂ŁA�N���b�v�Ȃ�

	//���W�w�肵��
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x;
	rect.bottom = y;

	//�`��
	return DrawTextEx( MyHdc , const_cast<LPWSTR>( str.c_str() ) , (int)str.length() ,  &rect , format , &option );
}

//�w��̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::Print( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt )
{
	//API�ɓn���t�H�[�}�b�g�����쐬
	UINT format ;
	DRAWTEXTPARAMS option;

	MakeDrawTextParameter( opt , format , option );
	//����E�E���Ƃ��Ɏw�肳��Ă���̂ŁA�N���b�v����B
	format |= DT_NOCLIP;	

	//���W�w�肵��
	RECT rect;
	rect.left = x1;
	rect.top = y1;
	rect.right = x2;
	rect.bottom = y2;

	//�`��
	return DrawTextEx( MyHdc , const_cast<LPWSTR>( str.c_str() ) , (int)str.length() ,  &rect , format , &option );
}

bool mGdiDC::PrintMultiline( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt )
{
	//�s�P�ʂɃp�[�X
	WStringDeque lines;
	ParseString( str , lines , false );

	return PrintMultiline( lines , x1 , y1 , x2 , y2 , opt );
}


bool mGdiDC::PrintMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt )
{
	//���[�U�[�̎w�肵�Ă������W�ɂ��ĉ��H
	int top = ( y1 < y2 ) ? ( y1 ) : ( y2 );		//���[�U�[���w�肵�Ă����`��ʒu�̏�[
	int bottom = ( y1 < y2 ) ? ( y2 ) : ( y1 );		//���[�U�[���w�肵�Ă����`��ʒu�̉��[
	SIZE areasize;									//���[�U�[���w�肵�Ă����`��T�C�Y
	areasize.cx = ( x1 < x2 ) ? ( x2 - x1 ) : ( x1 - x2 );
	areasize.cy = bottom - top;

	//�e�s�̃T�C�Y�����߂�
	SIZE reqsize = { 0 };		//���T�C�Y
	MultilineSize reqlinesize;	//�s���̃T�C�Y
	if( !GetPrintSizeMultiline( lines , reqsize , reqlinesize , opt ) )
	{
		return false;
	}

	//���߂��S�̂̕`��ɕK�v�Ȕ͈͂��A�w��G���A�ɔ[�܂��Ă��邩�m�F���āA���܂��Ă��Ȃ��Ȃ�␳����
	//X�i�����j�����ɂ��Ă�API������Ă���邩��AY�i�����j�����ɂ̂ݏ�������
	RECT print_rect;	//���ۂɕ`����s���͈�
	print_rect.left = x1;
	print_rect.right = x2;

	if( areasize.cy < reqsize.cy )
	{
		//����Ȃ�
		int diff = reqsize.cy - areasize.cy;	//���s����
		switch( opt.VerticalAlign )
		{
		case PrintOptions::PrintVerticalAlign::V_ALIGN_TOP:
			print_rect.top = top;
			print_rect.bottom = bottom + diff;
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_CENTER:
			print_rect.top = top - ( diff / 2 );
			print_rect.bottom = bottom + ( diff / 2 );
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_BOTTOM:
			print_rect.top = top - diff;
			print_rect.bottom = bottom;
			break;
		default:
			return false;
		}
	}
	else
	{
		//����
		int diff = areasize.cy - reqsize.cy;	//���]�����X�y�[�X
		switch( opt.VerticalAlign )
		{
		case PrintOptions::PrintVerticalAlign::V_ALIGN_TOP:
			print_rect.top = top;
			print_rect.bottom = top + diff;
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_CENTER:
			print_rect.top = top + ( diff / 2 );
			print_rect.bottom = bottom - ( diff / 2 );
			break;
		case PrintOptions::PrintVerticalAlign::V_ALIGN_BOTTOM:
			print_rect.top = bottom - diff;
			print_rect.bottom = bottom;
			break;
		default:
			return false;
		}
	}

	//API�ɓn���t�H�[�}�b�g�����쐬
	UINT format ;
	DRAWTEXTPARAMS option;
	MakeDrawTextParameter( opt , format , option );

	//�`�悷��
	int current_y = print_rect.top;
	for( size_t i = 0 ; i < lines.size() ; i++ )
	{
		//�`��ʒu������
		RECT area;
		area.left = print_rect.left;
		area.right = print_rect.right;
		area.top = current_y;
		area.bottom = current_y + reqlinesize[ i ].cy;

		if( bottom < area.top )
		{
			//�ȍ~����͈͒����ɂ�����ł��Ȃ�
			break;
		}
		if( top < area.bottom )
		{
			//����\�͈�
			if( !DrawTextEx( MyHdc , const_cast<LPWSTR>( lines[ i ].c_str() ) , (int)lines[ i ].length() , &area , format , &option ) )
			{
				return false;
			}
		}
		current_y += reqlinesize[ i ].cy;
	}
	return true;
}


//�w��̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::PrintOffset( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt )
{
	return Print( str , x1 , y1 , x1 + offset_x , y1 + offset_y , opt );
}

//�`�悵���Ƃ��̃T�C�Y�𓾂�
bool mGdiDC::GetPrintSize( const WString& str , SIZE& retSize , const PrintOptions& opt )
{
	//API�ɓn���t�H�[�}�b�g�����쐬
	PrintOptions tmp_opt = opt;
	tmp_opt.VerticalAlign = mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_TOP;
	tmp_opt.HorizontalAlign = mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_LEFT;

	UINT format ;
	DRAWTEXTPARAMS option;
	MakeDrawTextParameter( tmp_opt , format , option );
	format |= DT_CALCRECT;
	format |= DT_NOCLIP;

	RECT rect = { 0 };
	if( !DrawTextEx( MyHdc , const_cast<LPWSTR>( str.c_str() ) , (int)str.length() ,  &rect , format , &option ) )
	{
		return false;
	}
	retSize.cx = ( rect.left < rect.right ) ? ( rect.right - rect.left ) : ( rect.left - rect.right );
	retSize.cx++;
	retSize.cy = ( rect.top < rect.bottom ) ? ( rect.bottom - rect.top ) : ( rect.top - rect.bottom );
	retSize.cy++;
	return true;
}

//�w��̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::PrintOffsetMultiline( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt )
{
	return PrintMultiline( str , x1 , y1 , x1 + offset_x , y1 + offset_y , opt );
}

//�w��̈ʒu�Ƀe�L�X�g��`�悷��
bool mGdiDC::PrintOffsetMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt )
{
	return PrintMultiline( lines , x1 , y1 , x1 + offset_x , y1 + offset_y , opt );
}

bool mGdiDC::GetPrintSizeMultiline( const WString& str , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt )
{
	WStringDeque lines;
	ParseString( str , lines , false );
	return GetPrintSizeMultiline( lines , retSize , retLineSize , opt );
}

bool mGdiDC::GetPrintSizeMultiline( const WStringDeque& lines , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt )
{
	retSize.cx = 0;
	retSize.cy = 0;
	retLineSize.clear();

	//API�ɓn���t�H�[�}�b�g�����쐬
	PrintOptions tmp_opt = opt;
	tmp_opt.VerticalAlign = mGdiDC::PrintOptions::PrintVerticalAlign::V_ALIGN_TOP;
	tmp_opt.HorizontalAlign = mGdiDC::PrintOptions::PrintHorizontalAlign::H_ALIGN_LEFT;

	UINT format ;
	DRAWTEXTPARAMS option;
	MakeDrawTextParameter( tmp_opt , format , option );
	format |= DT_CALCRECT;
	format |= DT_NOCLIP;

	//�`��ɕK�v�ȋ�`�����߂�
	for( WStringDeque::const_iterator itr = lines.begin() ; itr != lines.end() ; itr++ )
	{
		//1�s���̃T�C�Y�����߂�
		RECT rect = { 0 };
		if( !DrawTextEx( MyHdc , const_cast<LPWSTR>( (*itr).c_str() ) , (int)(*itr).length() , &rect , format , &option ) )
		{
			return false;
		}

		//�ŁA��������v���āA�S�̂̕`��ɕK�v�Ȕ͈͂����߂�
		SIZE sz;
		sz.cx = ( rect.left < rect.right ) ? ( rect.right - rect.left + 1 ) : ( rect.left - rect.right + 1 );
		sz.cy = ( rect.top < rect.bottom ) ? ( rect.bottom - rect.top + 1 ) : ( rect.top - rect.bottom + 1 );

		if( retSize.cx < sz.cx )
		{
			retSize.cx = sz.cx;
		}
		retSize.cy += sz.cy;

		//����ɁA�e�s�̃T�C�Y�����ʂƂ��ĕԂ�
		retLineSize.push_back( std::move( sz ) );
	}
	return true;
}


//�e�L�X�g�̕����F���w��
bool mGdiDC::SetTextColor( COLORREF color )
{
	::SetTextColor( MyHdc , color );
	return true;
}

//�e�L�X�g�̃o�b�N�O���E���h�F���w��
bool mGdiDC::SetBackgroundColor( COLORREF color )
{
	SetBkMode( MyHdc , OPAQUE );
	SetBkColor( MyHdc , color );
	return true;
}

//�e�L�X�g�̃o�b�N�O���E���h�F���w��
bool mGdiDC::SetBackgroundColor( void )
{
	SetBkMode( MyHdc , TRANSPARENT );
	return true;
}

//���݂̃e�L�X�g�̕����F���擾
bool mGdiDC::GetTextColor( COLORREF& retColor )const noexcept
{
	retColor = ::GetTextColor( MyHdc );
	if( retColor == CLR_INVALID )
	{
		return false;
	}
	return true;
}

//���݂̔w�i�F���擾
bool mGdiDC::GetBackgroundColor( COLORREF& retColor , bool& retIsTransparent )const noexcept
{
	int IsTransparent = ::GetBkMode( MyHdc );
	retIsTransparent = ( IsTransparent == TRANSPARENT );
	retColor = ::GetBkColor( MyHdc );

	if( IsTransparent == 0 || retColor == CLR_INVALID )
	{
		return false;
	}
	return true;
}

//�����F���Ŏw��͈͂��w��͈͂ɃR�s�[����(���̂P)
bool mGdiDC::Copy( const mGdiDC& srcdc ,							//�R�s�[���f�o�C�X�R���e�L�X�g
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//�R�s�[�������`
	INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,		//�R�s�[�撷���`
	const RGBQUAD& transparent_color )
{
	//���W�ϊ�
	PositionConvert( src_x1 , src_y1 , src_x2 , src_y2 );
	PositionConvert( dst_x1 , dst_y1 , dst_x2 , dst_y2 );

	INT src_w = src_x2 - src_x1;	//�\�[�X�̕�
	INT src_h = src_y2 - src_y1;	//�\�[�X�̍���
	INT dst_w = dst_x2 - dst_x1;	//�\��t����̕�
	INT dst_h = dst_y2 - dst_y1;	//�\��t����̍���

	UINT transparent = ( transparent_color.rgbRed   << 16 ) |
					   ( transparent_color.rgbGreen <<  8 ) |
					   ( transparent_color.rgbBlue  <<  0 ) ;

	return ::TransparentBlt( MyHdc , dst_x1 , dst_y1 , dst_w , dst_h , srcdc.MyHdc , src_x1 , src_y1 , src_w , src_h , transparent );
}

//�����F���Ŏw��͈͂��w��͈͂ɃR�s�[����(���̂Q)
bool mGdiDC::Copy( const mGdiDC& srcdc ,							//�R�s�[���f�o�C�X�R���e�L�X�g
	INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//�R�s�[�������`
	INT dst_x1 , INT dst_y1 ,								//�R�s�[����W(����)
	const RGBQUAD& transparent_color )
{
	//�R�s�[��̉E�E���̍��W���A�R�s�[���̕��E�������g���ċ��߂�
	INT width;	//��
	if( src_x1 < src_x2 )
	{
		width = src_x2 - src_x1 + 1;
	}
	else
	{
		width = src_x1 - src_x2 + 1;
	}
	INT height;	//����
	if( src_x1 < src_x2 )
	{
		height = src_y2 - src_y1 + 1;
	}
	else
	{
		height = src_y1 - src_y2 + 1;
	}

	//���߂����E���������āA�u���̂P�v���Ăяo���B
	return Copy( srcdc , src_x1 , src_y1 , src_x2 , src_y2 , dst_x1 , dst_y1 , dst_x1 + width , dst_y1 + height , transparent_color );
}


//�����F���Ŏw��͈͂��w��͈͂ɃR�s�[����(���̂R)
bool mGdiDC::Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , const RGBQUAD& transparent_color )
{
	//���W�͓����Ȃ̂ŁA�R�s�[��ɂ��R�s�[���̍��W�����̂܂܎g�����Ⴂ�܂�
	return Copy( srcdc , x1 , y1 , x2 , y2 , x1 , y1 , x2 , y2 , transparent_color );
}

bool mGdiDC::SetStrechMode( StrechMode mode )
{

	int new_mode;
	switch( mode )
	{
	case StrechMode::STRECH_BLACKONWHITE:
		new_mode = BLACKONWHITE;
		break;
	case StrechMode::STRECH_WHITEONBLACK:
		new_mode = WHITEONBLACK;
		break;
	case StrechMode::STRECH_COLORONCOLOR:
		new_mode = COLORONCOLOR;
		break;
	case StrechMode::STRECH_HALFTONE:
		new_mode = HALFTONE;
		break;
	default:
		return false;
	}
	return SetStretchBltMode( MyHdc , new_mode );
}



