//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2016 Fingerling. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------------

/*
���p�r
�f�o�C�X�R���e�L�X�g�̃n���h���ł��B
���̃N���X�͊��N���X�ŁA�n���h���������@�ɂ���Ĕh���N���X��������Ă��܂��B

���R�R�Ŕh���N���X�̂��Љ�ł���
[mGdiWinDC]
�E�C���h�E�̃N���C�A���g�̈�ɒ��ڕ`������邽�߂̃N���X�ł��B
���̃N���X�ł�WinAPI��GetDC()���Ăяo���Ď擾�����f�o�C�X�R���e�L�X�g�̃n���h���������܂��B

[mGdiPaintDC]
WM_PAINT���b�Z�[�W�ɑΉ����āA�E�C���h�E���ĕ`�悷�邽�߂̃N���X�ł��B
BeginPaint()�Ŏ擾�����f�o�C�X�R���e�L�X�g�̃n���h���������܂��B
*/

#ifndef MGDIDC_H_INCLUDED
#define MGDIDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiHandle.h"
#include "mGdiResource.h"
#include <unordered_set>
namespace mGdiDC_Definitions
{
	enum PrintHorizontalAlign
	{
		H_ALIGN_LEFT,		//������
		H_ALIGN_CENTER,		//(����)��������
		H_ALIGN_RIGHT		//�E����
	};
	enum PrintVerticalAlign
	{
		V_ALIGN_TOP,		//�㑵��
		V_ALIGN_CENTER,		//(����)�������� ��������w�肷��Ɖ��s�͖�������܂�
		V_ALIGN_BOTTOM		//������ ��������w�肷��Ɖ��s�͖�������܂�
	};
	enum StrechMode
	{
		STRECH_BLACKONWHITE,
		STRECH_WHITEONBLACK,
		STRECH_COLORONCOLOR,
		STRECH_HALFTONE,
	};
}

class mGdiDC
{
public:
	virtual ~mGdiDC();

	//�I�u�W�F�N�g(�y���A�u���V�A�t�H���g�Ȃ�)��I������
	//handle : �I���������I�u�W�F�N�g���i�[����mGdiHandle�܂��͂��̔h���N���X
	//ret : ���������ꍇtrue
	bool Select( const mGdiHandle& handle );

	//�I�u�W�F�N�g��I�����܂�(HGDIOBJ���w��ver)
	//handle : �֘A�Â������I�u�W�F�N�g
	//ret : ���������ꍇtrue
	bool Select( HGDIOBJ new_object );

	//�I�u�W�F�N�g��I�����܂�(mGdiResource���璊�over ����1)
	//res : ���o���̃��\�[�X�v�[��
	//id : �擾������ID
	//subid : id���Ȃ������ꍇ�Ɏ擾������ID(�s�v�ȏꍇ�͋󕶎����OK)
	//ret : ���������ꍇtrue
	bool Select( const mGdiResource& res , const WString& id , const WString& subid = L"" );

	//�I�u�W�F�N�g��I�����܂�(mGdiResource���璊�over ����2)
	//res : ���o���̃��\�[�X�v�[��
	//id : �擾������ID
	//subid : id���Ȃ������ꍇ�Ɏ擾������ID(�s�v�ȏꍇ�͋󕶎����OK)
	//ret : ���������ꍇtrue
	//�^�Ȃ��łƂ́AmGdiResource����I�u�W�F�N�g�𒊏o�����Ƃ��Ɍ^�̃`�F�b�N������_���Ⴂ�܂��B
	template< class T >
	bool Select( const mGdiResource& res , const WString& id , const WString& subid = L"" )
	{
		const T* object = res.GetItem<T>( id , subid );
		if( object == nullptr )
		{
			return false;
		}
		return Select( *object );
	}

	//����`�悷��
	//( from_x , from_y )���̍��W������������܂�
	//( to_x , to_y )���̍��W�Ɍ����Đ��������܂�
	//ret : �������^
	//�Efrom���ȗ������ꍇ�́A���݂̈ʒu������������܂��B
	//�E���̊֐������s��́Ato�Ɏw�肵���ʒu���u���݂̈ʒu�v�ƂȂ�܂�
	bool Line( INT to_x , INT to_y );
	bool Line( INT from_x , INT from_y , INT to_x , INT to_y );
	bool LineOffset( INT from_x , INT from_y , INT offset_x , INT offset_y );

	//��`��`�悷��
	//( x1 , y1 )-( x2 , y2 )��Ίp����̒��_�Ƃ��钷���`��`�悵�܂��B
	//ret : �������^
	//�E�O�g�����݂̃y���ŁA���������݂̃u���V�œh��Ԃ����B
	//�E�O�g���v��Ȃ��Ȃ�k���y��(mGdiPen�I�ɂ�TRANSPARENT_PEN)���g��
	//�E�h��Ԃ��Ȃ��Ȃ�k���u���V(mGdiBrush�I�ɂ�TRANSPARENT_BRUSH)���g��
	//�y�d�v�zWinAPI�͉E�ӁA��ӂɂ��āA�w�肵�����W��1�s�N�Z�������ɕ`�悳��܂����A
	//       ���̊֐��͂����␳���Ă��܂��B�w�肵�����W����E�Ӂ���ӂ��ʂ�܂��B
	bool Rectangle( INT x1 , INT y1 , INT x2 , INT y2 );

	//��`��`�悷��
	//( x1 , y1 )-( x1+x2 , y1+y2 )��Ίp����̒��_�Ƃ��钷���`��`�悵�܂��B
	//ret : �������^
	//�E�O�g�����݂̃y���ŁA���������݂̃u���V�œh��Ԃ����B
	//�E�O�g���v��Ȃ��Ȃ�k���y��(mGdiPen�I�ɂ�TRANSPARENT_PEN)���g��
	//�E�h��Ԃ��Ȃ��Ȃ�k���u���V(mGdiBrush�I�ɂ�TRANSPARENT_BRUSH)���g��
	//�y�d�v�zWinAPI�͉E�ӁA��ӂɂ��āA�w�肵�����W��1�s�N�Z�������ɕ`�悳��܂����A
	//       ���̊֐��͂����␳���Ă��܂��B�w�肵�����W����E�Ӂ���ӂ��ʂ�܂��B
	bool RectangleOffset( INT x1 , INT y1 , INT offset_x , INT offset_ );

	//�~��`�悷��
	//�w����W�𒆐S�Ƃ����A�w�蔼�a�̉~��`���܂�
	//( x , y )�~�̒��S
	//radius : �~�̔��a
	//�E�O�g�����݂̃y���ŁA���������݂̃u���V�œh��Ԃ����B
	//�E�O�g���v��Ȃ��Ȃ�k���y��(mGdiPen�I�ɂ�TRANSPARENT_PEN)���g��
	//�E�h��Ԃ��Ȃ��Ȃ�k���u���V(mGdiBrush�I�ɂ�TRANSPARENT_BRUSH)���g��
	//�E�y���Ӂz���Ƃ��΁A���a��5�Ƃ����ꍇ���S����}5�s�N�Z����`�����߁A
	//          �`���ꂽ�~�̐�߂镝�E������11�s�N�Z���ɂȂ�܂��B
	//          �C���[�W�I�ɂ́A�~�̊O��0.5�s�N�Z�����͉~�̊O���ƍl����΂悢���ƁB
	bool Circle( INT x , INT y , INT radius );

	//�~��`�悷��
	//�w����W�𒆐S�Ƃ����A�w�蔼�a�̉~��`���܂�
	//( x1 , y1 )-( x2 , y2 )��Ίp����̒��_�Ƃ��钷���`�ɊO�ڂ���~��`���܂��B
	//�E�O�g�����݂̃y���ŁA���������݂̃u���V�œh��Ԃ����B
	//�E�O�g���v��Ȃ��Ȃ�k���y��(mGdiPen�I�ɂ�TRANSPARENT_PEN)���g��
	//�E�h��Ԃ��Ȃ��Ȃ�k���u���V(mGdiBrush�I�ɂ�TRANSPARENT_BRUSH)���g��
	bool Circle( INT x1 , INT y1 , INT x2 , INT y2 );

	//�w��͈͂��w��͈͂ɃR�s�[����(���̂P)
	//srcdc�Ŏw�肵��DC��( src_x1 , src_y1 )-( src_x2 , src_y2 )��Ίp����̒��_�Ƃ��钷���`���A
	//���̃I�u�W�F�N�g�� ( dst_x1 , dst_y1 )-( dst_x2 , dst_y2 )��Ίp����̒��_�Ƃ��钷���`�Ƃ���ʒu�ɓ\��t���܂��B
	//ret : ������true
	//�E�R�s�[���ƃR�s�[��ŕ��E�������Ⴄ�Ɗg��k�����܂�
	bool Copy( const mGdiDC& srcdc ,							//�R�s�[���f�o�C�X�R���e�L�X�g
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//�R�s�[�������`
		INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,		//�R�s�[�撷���`
		DWORD raster = SRCCOPY );

	//�w��͈͂��w��͈͂ɃR�s�[����(���̂Q)
	//srcdc�Ŏw�肵��DC��( src_x1 , src_y1 )-( src_x2 , src_y2 )��Ίp����̒��_�Ƃ��钷���`���A
	//���̃I�u�W�F�N�g�� ( dst_x1 , dst_y1 )������̒��_�Ƃ���ʒu�ɓ\��t���܂��B
	//ret : ������true
	bool Copy( const mGdiDC& srcdc ,							//�R�s�[���f�o�C�X�R���e�L�X�g
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//�R�s�[�������`
		INT dst_x1 , INT dst_y1 ,								//�R�s�[����W(����)
		DWORD raster = SRCCOPY );

	//�w��͈͂��w��͈͂ɃR�s�[����(���̂R)
	//srcdc�Ŏw�肵��DC��( x1 , y1 )-( x2 , y2 )��Ίp����̒��_�Ƃ��钷���`���A
	//���̃I�u�W�F�N�g�̓���ʒu�ɓ\��t���܂��B
	//ret : ������true
	bool Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , DWORD raster = SRCCOPY );

	//�����F���Ŏw��͈͂��w��͈͂ɃR�s�[����(���̂P)
	//srcdc�Ŏw�肵��DC��( src_x1 , src_y1 )-( src_x2 , src_y2 )��Ίp����̒��_�Ƃ��钷���`���A
	//���̃I�u�W�F�N�g�� ( dst_x1 , dst_y1 )-( dst_x2 , dst_y2 )��Ίp����̒��_�Ƃ��钷���`�Ƃ���ʒu�ɓ\��t���܂��B
	//ret : ������true
	//�E�R�s�[���ƃR�s�[��ŕ��E�������Ⴄ�Ɗg��k�����܂�
	bool Copy( const mGdiDC& srcdc ,							//�R�s�[���f�o�C�X�R���e�L�X�g
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//�R�s�[�������`
		INT dst_x1 , INT dst_y1 , INT dst_x2 , INT dst_y2 ,		//�R�s�[�撷���`
		const RGBQUAD& transparent_color );

	//�����F���Ŏw��͈͂��w��͈͂ɃR�s�[����(���̂Q)
	//srcdc�Ŏw�肵��DC��( src_x1 , src_y1 )-( src_x2 , src_y2 )��Ίp����̒��_�Ƃ��钷���`���A
	//���̃I�u�W�F�N�g�� ( dst_x1 , dst_y1 )������̒��_�Ƃ���ʒu�ɓ\��t���܂��B
	//ret : ������true
	bool Copy( const mGdiDC& srcdc ,							//�R�s�[���f�o�C�X�R���e�L�X�g
		INT src_x1 , INT src_y1 , INT src_x2 , INT src_y2 ,		//�R�s�[�������`
		INT dst_x1 , INT dst_y1 ,								//�R�s�[����W(����)
		const RGBQUAD& transparent_color );

	//�����F���Ŏw��͈͂��w��͈͂ɃR�s�[����(���̂R)
	//srcdc�Ŏw�肵��DC��( x1 , y1 )-( x2 , y2 )��Ίp����̒��_�Ƃ��钷���`���A
	//���̃I�u�W�F�N�g�̓���ʒu�ɓ\��t���܂��B
	//ret : ������true
	bool Copy( const mGdiDC& srcdc , INT x1 , INT y1 , INT x2 , INT y2 , const RGBQUAD& transparent_color );

	//���݂̈ʒu�Ƀe�L�X�g��`�悷��
	//�E���s�͖�������܂�
	// str : �`�悷�镶����
	bool Print( const WString& str );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//�E���s�͖�������܂�
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool Print( const WString& str , INT x , INT y );

	//�`�悵���Ƃ��̃T�C�Y�𓾂�
	//�E���s�͖�������܂�
	// str : �`�悷�镶����
	// retSize : �`�悵���Ƃ��̃T�C�Y
	//���t�H���g�Ɋp�x���ݒ肳��Ă���ꍇ�́A���������ʂ�Ԃ��܂���B
	//  �i�p�x�[�����Ɖ��肵�Čv�Z������ۂ��j
	bool GetPrintSize( const WString& str , SIZE& retSize );

	//
	struct PrintOptions
	{
		using PrintHorizontalAlign = mGdiDC_Definitions::PrintHorizontalAlign;
		using PrintVerticalAlign = mGdiDC_Definitions::PrintVerticalAlign;

		//�^�u�̑傫��(1�ɂ���Ƃ����̃X�y�[�X�Ɠ���)
		DWORD TabSize;

		//�����ʒu
		PrintHorizontalAlign HorizontalAlign;

		//�����ʒu
		PrintVerticalAlign VerticalAlign;

		PrintOptions()
		{
			TabSize = 1;
			HorizontalAlign = PrintHorizontalAlign::H_ALIGN_LEFT;
			VerticalAlign = PrintVerticalAlign::V_ALIGN_TOP;
		}
	};

	//���݂̈ʒu�Ƀe�L�X�g��`�悷��
	//�E�����ʒu�������������������̏ꍇ�A���s�͖�������܂�
	// str : �`�悷�镶����
	bool Print( const WString& str , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//�E�����ʒu�������������������̏ꍇ�A���s�͖�������܂�
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool Print( const WString& str , INT x , INT y , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//�E�����ʒu�������������������̏ꍇ�A���s�͖�������܂�
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool Print( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool PrintMultiline( const WString& str , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool PrintMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT x2 , INT y2 , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//�E�����ʒu�������������������̏ꍇ�A���s�͖�������܂�
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool PrintOffset( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool PrintOffsetMultiline( const WString& str , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt );

	//�w��̈ʒu�Ƀe�L�X�g��`�悷��
	//( x , y )�`�悷��ʒu
	// str : �`�悷�镶����
	bool PrintOffsetMultiline( const WStringDeque& lines , INT x1 , INT y1 , INT offset_x , INT offset_y , const PrintOptions& opt );

	//�`�悵���Ƃ��̃T�C�Y�𓾂�
	//�E���s�͖�������܂�
	// str : �`�悷�镶����
	// retSize : �`�悵���Ƃ��̃T�C�Y
	//���t�H���g�Ɋp�x���ݒ肳��Ă���ꍇ�́A���������ʂ�Ԃ��܂���B
	//  �i�p�x�[�����Ɖ��肵�Čv�Z������ۂ��j
	bool GetPrintSize( const WString& str , SIZE& retSize , const PrintOptions& opt );

	//�e�s�̕`��T�C�Y
	using MultilineSize = std::deque< SIZE >;

	//�`�悵���Ƃ��̃T�C�Y�𓾂�
	// str : �`�悷�镶����
	// retSize : �`�悵���Ƃ��̃T�C�Y
	//���t�H���g�Ɋp�x���ݒ肳��Ă���ꍇ�́A���������ʂ�Ԃ��܂���B
	//  �i�p�x�[�����Ɖ��肵�Čv�Z������ۂ��j
	bool GetPrintSizeMultiline( const WString& lines , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt );

	//�`�悵���Ƃ��̃T�C�Y�𓾂�
	// str : �`�悷�镶����
	// retSize : �`�悵���Ƃ��̃T�C�Y
	//���t�H���g�Ɋp�x���ݒ肳��Ă���ꍇ�́A���������ʂ�Ԃ��܂���B
	//  �i�p�x�[�����Ɖ��肵�Čv�Z������ۂ��j
	bool GetPrintSizeMultiline( const WStringDeque& lines , SIZE& retSize , MultilineSize& retLineSize , const PrintOptions& opt );

	//�e�L�X�g�̕����F���w��
	// color : �e�L�X�g�F
	bool SetTextColor( COLORREF color );

	//���݂̃e�L�X�g�̕����F���擾
	// retColor : ���݂̕����F
	// ret : �������^
	bool GetTextColor( COLORREF& retColor )const noexcept;

	//�e�L�X�g�̃o�b�N�O���E���h�F���w��
	// color : �o�b�N�O���E���h�F
	//�@�E�w����ȗ�����Ɠ����ɂȂ�܂�
	bool SetBackgroundColor( COLORREF color );

	//�e�L�X�g�̃o�b�N�O���E���h�F���w��
	// color : �o�b�N�O���E���h�F
	//�@�E�w����ȗ�����Ɠ����ɂȂ�܂�
	bool SetBackgroundColor( void );

	//���݂̔w�i�F���擾
	// retColor : ���݂̔w�i�F
	// retIsTransparent : �����ł���ΐ^
	// ret : �������^
	bool GetBackgroundColor( COLORREF& retColor , bool& retIsTransparent )const noexcept;

	using StrechMode = mGdiDC_Definitions::StrechMode;
	//�g��E�k�����̐L�k���[�h��ݒ肷��
	bool SetStrechMode( StrechMode mode );

private:

	mGdiDC( const mGdiDC& src ) = delete;
	mGdiDC& operator=( const mGdiDC& src ) = delete;

	//MyHdc�ւ̖������A�N�Z�X���K�v�Ȃ���
	//mGdiMemDC : CreateCompatibleDC���ĂԂ���
	//mGdiBitmap : CreateCompatibleBitmap���ĂԂ���
	friend class mGdiMemDC;
	friend class mGdiBitmap;

	//�n���h���v�[��
	typedef std::unordered_set<HGDIOBJ> GdiObjectPool;
	GdiObjectPool MyDefaultObj;		//�ŏ�����f�o�C�X�R���e�L�X�g�Ɋ֘A�t�����Ă����n���h��
	GdiObjectPool MyAttachedObj;	//���݃��[�U�[���f�o�C�X�R���e�L�X�g�Ɋ֘A�t���Ă���n���h��

protected:

	//�h���N���X�ŃC���X�^���X�����O��Ȃ̂ŁA���̃N���X�̃f�t�H���g�R���X�g���N�^�͉B���Ă���
	mGdiDC();

	//�f�o�C�X�R���e�L�X�g�̃n���h��
	HDC MyHdc;

	//�f�o�C�X�R���e�L�X�g�Ɋ֘A�t�����Ă���I�u�W�F�N�g��S�����ɖ߂�
	//ret : ������true
	bool ResetSelectedObject( void );

	//���W�ϊ�
	//�Ex1��x2�Ay1��y2�̈ʒu�֌W�����]���Ă���ꍇ�A����������x1,y1�ɂȂ�悤�ɓ���ւ��܂��B
	//�E�E�ӁA��ӂ̈ʒu��1�s�N�Z�������ɕ␳���܂�
	//x1,y1,x2,y2 : in/out �␳�Ώۂ̍��W
	void PositionConvert( INT& x1 , INT& y1 , INT& x2 , INT&y2 )const;

};

#endif //MGDIDC_H_INCLUDED

