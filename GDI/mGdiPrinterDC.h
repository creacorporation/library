//----------------------------------------------------------------------------
// �E�C���h�E�Ǘ��i�v�����^�[�p�f�o�C�X�R���e�L�X�g�j
// Copyright (C) 2019 Crea Inc. All rights reserved. 
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// ���쌠�\���⃉�C�Z���X�̉��ς͋֎~����Ă��܂��B
// ���̃\�[�X�R�[�h�Ɋւ��āA��L���C�Z���X�ȊO�̌_�񓙂͈�ؑ��݂��܂���B
// (���炩�̌_�񂪂���ꍇ�ł��A�{�\�[�X�R�[�h�͂��̑ΏۊO�ƂȂ�܂�)
//----------------------------------------------------------------------------

//���p�r
//GDI���o�R���Ĉ�����s���܂�
//�ȉ��T���v��
#if 0
#include "mStandard.h"
#include "General/mTCHAR.h"
#include "Device/mPrinterEnumerar.h"
#include "GDI/mGdiPrinterDC.h"

//�R�}���h���C���I�v�V����
mOptionParser MyOption;

int main( int argc , char** argv )
{
	InitializeLibrary();

	//�v�����^�[�����擾����
	mPrinterEnumerar printers;				//�v�����^�[�񋓃N���X
	mPrinterEnumerar::PrinterInfo info;		//�v�����^�[�ꗗ
	printers.GetPrinterInfo( info );

	//�f�t�H���g�v�����^�[�̖��O���擾����
	WString defprinter;	//�f�t�H���g�v�����^�[�̖��O
	printers.GetDefaultPrinterName( defprinter );

	//�I�������v�����^�[�̏����擾����
	mPrinterEnumerar::PrinterProperty prop;	//�v�����^�[���
	printers.GetPrinterProperty( defprinter , prop );
		 
	//�������
	mGdiPrinterDC printer;
	mGdiPrinterDC::Option_UseOption opt;
	opt.DocumentName = L"TestPage";			//�h�L�������g��
	opt.PrinterName = defprinter;			//�����ł́A��Ɏ擾�����f�t�H���g�v�����^�[�ɂ���

	mGdiPrinterDC::PageOption_StandardSize pageopt;
	pageopt.Paper = DMPAPER_A4;				//����A4���w��(�{����mPrinterEnumerar::PrinterProperty�̒l����R�s�[���ׂ�)

	//�v�����^���I�[�v�����āA1���ڂ̕`����J�n
	printer.Open( opt );
	printer.StartPage( pageopt );

	//���W�ϊ��̐ݒ������
	SIZE sz;
	//�����̍�������_�ɂ��ĕ`�悷��ꍇ��
	// �� ���̒[����̈ʒu�ɑ΂��ĕ`��ł���
	// �~ ����\�͈͂̒[�����W0�ɂȂ�Ȃ�
	printer.GetPaperSize( sz );
	printer.SetView( mGdiPrinterDC::Origin::ORIGIN_PAPEREDGE , sz );
	//��GetPaperSize()��1/10mm�P�ʂŌ��ʂ�Ԃ��̂ŁA
	//  A4(210�~297mm)�Ȃ�΁A(2100,2970)���Ԃ�B
	//  ��������̂܂�SetView�ɓn���Ă���̂ŁA���S�̂�(0,0)-(2099,2969)�̃r�b�g�}�b�v�̂悤�ɕ`��ł���ݒ�ɂȂ�B
	//  �܂�A1�s�N�Z�� = 1/10mm �ɂȂ�B

	//������\�͈͂̍�������_�ɂ��ĕ`�悷��ꍇ��
	// �� (0,0)�̈ʒu������W�v�Z�ł���̂ŁA������₷��
	// �~ �v�����^�ɂ���Ĉ���ʒu�������ɕς��
//	printer.GetPrintableSize( sz );
//	printer.SetView( mGdiPrinterDC::Origin::ORIGIN_PRINTABLEAREA , sz );

	//�`�悷��
	RECT rect;
	printer.Select( GetStockObject( BLACK_PEN ) );
	printer.GetPrintableArea( rect );	//����\�͈͂��擾
	printer.Rectangle( rect.left , rect.top , rect.right , rect.bottom );	//����\�͈͂����ς��ɒ����`��`��
	printer.Rectangle( 0 , 0 , 100 , 100 );	//���W���w�肵�Ē����`��`��
	printer.Print( L"Test" , 200 , 200 );	//���W���w�肵��"Test"�ƕ���������

	//�h�L�������g�I��
	printer.EndPage();
	printer.Close();	//Close()�ɂ����ۂ̃v�����g���͂��܂�

	return 0;
}
#endif

#ifndef MGDIPRINTERDC_H_INCLUDED
#define MGDIPRINTERDC_H_INCLUDED

#include "mStandard.h"
#include "mGdiDC.h"
#include <memory>
#include "../Device/mPrinterInfo.h"

namespace Definitions_mGdiPrinterDC
{
	//���_�ʒu�w��
	enum Origin
	{
		ORIGIN_PAPEREDGE,		//���_�����̍���Ɏw�肵�܂�
		ORIGIN_PRINTABLEAREA,	//���_������\�͈͂̍���Ɏw�肵�܂�
	};

	//�p���̌���
	enum PaperOrientation
	{
		PAPER_PORTRAIT,			//�c��
		PAPER_LANDSCAPE			//����
	};

	//���ʈ��
	enum PaperDuplex
	{
		PAPER_SIMPLEX,				//�Жʈ��
		PAPER_DUPLEX_HORIZONTAL,	//���ӒԂ�
		PAPER_DUPLEX_VERTICAL,		//�Z�ӒԂ�
	};
};

class mGdiPrinterDC : public mGdiDC
{
public:

	//�I�v�V�����\����(�h�L�������g)
	//�EOption_UseOption �c �����o�ϐ��𖄂߂ăI�v�V������ݒ肵�����Ƃ�
	struct Option
	{
		enum CreateMethod
		{
			USEOPTION,
		};
		const CreateMethod method;	//RTTI�̑�p�ł��B�ύX�̕K�v�͂���܂���B

		//�h�L�������g��
		//�E�X�v�[���ɕ\������鍡��W���u�̖��O
		WString DocumentName;

		//�v�����^��
		//�E�����̃v�����^�̖��O
		WString PrinterName;

		//�o�̓t�@�C��
		//�E���z�v�����^(Microsoft Print to PDF��)�ŏo�̓t�@�C�������w��
		WString OutFileName;

		//����\�͈͂ɑ΂���}�[�W��(�f�o�C�X�P��)
		//�E�f�o�C�X���瓾�������\�͈͂҂�����Ɉ������ƒ[�����Ȃ���Ő؂��\��������
		//�E�����ɒl���w�肷��ƁA�f�o�C�X���瓾��ꂽ����\�͈͂ɑ΂��Ďw��ʂ̃}�[�W����ݒ肷��
		DWORD PrintAreaMargin;

	protected:
		Option() = delete;
		Option( CreateMethod create_method ) : method( create_method )
		{
			PrintAreaMargin = 10;
		}
	};

	struct Option_UseOption : public Option
	{
		Option_UseOption() : Option( CreateMethod::USEOPTION )
		{
		}
	};

	//�I�v�V�����\����(�y�[�W)
	struct PageOption
	{
		enum CreateMethod
		{
			STANDARD_SIZE,		//�K�i�T�C�Y�̗p��
			NOCHANGE,			//�O�y�[�W�Ɠ����ݒ�
		};
		const CreateMethod method;	//RTTI�̑�p

	protected:
		PageOption() = delete;
		PageOption( CreateMethod create_method ) : method( create_method )
		{
		}
	};

	//�O�Ɠ����ݒ�ɂ��v�����g
	struct PageOption_Nochange : public PageOption
	{
		PageOption_Nochange() : PageOption( CreateMethod::NOCHANGE )
		{
		}
	};

	//�K�i�����ꂽ�p���T�C�Y�ɂ��v�����g
	struct PageOption_StandardSize : public PageOption
	{
		using PaperId = mPrinterInfo::PaperId;
		using PaperOrientation = Definitions_mGdiPrinterDC::PaperOrientation;
		using PaperDuplex = Definitions_mGdiPrinterDC::PaperDuplex;

		//�p��ID
		//�g�p�ł���l�́AmPrinterEnumerar�N���X���g���āA
		//mPrinterInfo::PrinterProperty::PaperInfo::PaperId�̒l���擾����΂킩��B
		//�܂��A���ۂ̒l��wingdi.h�Œ�`����Ă���"DMPAPER_"�Ŏn�܂�}�N���̒l�Ȃ̂ŁA���ڎw�肵�Ă��悢�B
		PaperId Paper;

		//�p���̌���
		PaperOrientation Orientation;

		//�J���[����Ȃ�^
		//false�ɂ���ƁA�J���[�v�����^�ł����m�N������ɂȂ�܂��B
		bool IsColor;

		//���ʈ���ݒ�
		PaperDuplex Duplex;

		PageOption_StandardSize() : PageOption( CreateMethod::STANDARD_SIZE )
		{
			IsColor = false;
			Paper = DMPAPER_A4;
			Orientation = PaperOrientation::PAPER_PORTRAIT;
			Duplex = PaperDuplex::PAPER_SIMPLEX;
		}
	};

	//�R���X�g���N�^
	mGdiPrinterDC() noexcept;
	virtual ~mGdiPrinterDC();

	//������J�n����
	// opt : ����I�v�V����
	// ret : �������^
	bool Open( const Option& opt ) noexcept;

	//�������������
	//�E���̃��\�b�h�̎��s�ɂ����ۂ̈����Ƃ��J�n����܂�
	//ret : �������^
	bool Close( void ) noexcept;

	//������L�����Z������
	//�EOpen()�����Ă���Close()����܂ł̊ԂɃL�����Z������ꍇ�R�[��
	//ret : �������^
	bool Abort( void ) noexcept;

	//���y�[�W���J�n����
	//�E�ŏ��̃y�[�W�ł��Ăяo�����K�v
	//�EGDI�I�u�W�F�N�g��(�u���V��)�̑I���󋵂̓��Z�b�g�����̂ōĐݒ肪�K�v
	// opt : ����I�v�V����
	// ret : �������^
	bool StartPage( const PageOption& opt ) noexcept;

	//�y�[�W����������
	//�E�Ō�̃y�[�W�ł��Ăяo�����K�v
	// ret : �������^
	bool EndPage( void ) noexcept;

	//�p���̃T�C�Y�𓾂�(0.1�~���P�� / 1cm = 100)
	//retSize : ����ꂽ�p���T�C�Y
	//ret : �������^
	//�EStartPage()���s�O�́A�O�̃y�[�W�̏�񂪓�����
	//�E�[�����o�邱�Ƃ�����܂�(�����ŃC���`�Emm�̕ϊ����s���邽��)
	bool GetPaperSize( SIZE& retSize ) noexcept;

	//����\�͈͂̃T�C�Y�𓾂�(0.1�~���P�� / 1cm = 100)
	//retSize : ����ꂽ����\�T�C�Y
	//ret : �������^
	//�EStartPage()���s�O�́A�O�̃y�[�W�̏�񂪓�����
	//�E�[�����o�邱�Ƃ�����܂�(�����ŃC���`�Emm�̕ϊ����s���邽��)
	//��SetView()�Ŏw�肵���`��Ώۂł͂Ȃ��A
	//�@�v�����^�̎d�l�Ǝ��̎�ނ��瓱�����l�ɂȂ�܂�
	bool GetPrintableSize( SIZE& retSize ) noexcept;

	//���̏㉺���E�̊e�[�������\�͈͂܂ł̃}�[�W���𓾂�(0.1�~���P�� / 1cm = 100)
	//retSize : ����ꂽ�}�[�W���T�C�Y
	//ret : �������^
	//�EStartPage()���s�O�́A�O�̃y�[�W�̏�񂪓�����
	//�E�[�����o�邱�Ƃ�����܂�(�����ŃC���`�Emm�̕ϊ����s���邽��)
	bool GetPrintableMargin( RECT& retMargin ) noexcept;

	//���_�ʒu�w��
	using Origin = Definitions_mGdiPrinterDC::Origin;

	//�`��Ώۂ͈̔͂��A���s�N�Z���l���̃X�N���[���ƌ��Ȃ������w�肵�܂�
	//origin : ���_�ʒu�w��
	// ORIGIN_PAPEREDGE
	//	�E���_�����̍���Ɏw�肵�܂�
	//	�E���S�̂��`��Ώۂ͈̔͂ƂȂ�܂�
	//	�E���ۂɃv�����^������ł���͈͂�GetPrintableArea()�Œ��ׂ܂�
	// ORIGIN_PRINTABLEAREA
	//	�E���_������\�͈͂̍���Ɏw�肵�܂�
	//	�E����\�͈͑S�̂��`��Ώۂ͈̔͂ƂȂ�܂�
	//size : ���s�N�Z���l���̃X�N���[���ƌ��Ȃ���
	bool SetView( Origin origin , const SIZE& size ) noexcept;

	//�`��Ώۂ͈̔͂��A���s�N�Z���l���̃X�N���[���ƌ��Ȃ������w�肵�܂�
	//origin : ���_�ʒu�w��
	// ORIGIN_PAPEREDGE
	//	�E���_�����̍���Ɏw�肵�܂�
	//	�E���S�̂��`��Ώۂ͈̔͂ƂȂ�܂�
	//	�E���ۂɃv�����^������ł���͈͂�GetPrintableArea()�Œ��ׂ܂�
	// ORIGIN_PRINTABLEAREA
	//	�E���_������\�͈͂̍���Ɏw�肵�܂�
	//	�E����\�͈͑S�̂��`��Ώۂ͈̔͂ƂȂ�܂�
	//size : ���s�N�Z���l���̃X�N���[���ƌ��Ȃ���
	//margin : origin�Ŏw�肵���͈͂ɑ΂��Ċm�ۂ���}�[�W��(0.1�~���P�� / 1cm = 100)
	bool SetView( Origin origin , const SIZE& size , const RECT& margin ) noexcept;


private:
	mGdiPrinterDC( const mGdiPrinterDC& src ) = delete;
	mGdiPrinterDC& operator=( const mGdiPrinterDC& src ) = delete;

protected:

	using OptionPtr = std::unique_ptr< Option >;
	OptionPtr MyOption;

};

#endif //MGDIPAINTDC_H_INCLUDED
