//----------------------------------------------------------------------------
// NDEFエンコード
// Copyright (C) 2025- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MSCNDEF_H_INCLUDED
#define MSCNDEF_H_INCLUDED

#include "mSCBase.h"
#include <deque>
#include <memory>
#include <General/mBinary.h>


class mSCNDEF
{
public:
	mSCNDEF();
	virtual ~mSCNDEF();

	bool Encode( mBinary& retdata )const;

	//NDEF設定テンプレート
	class Template
	{
	public:
		enum class TemplateType
		{
			Url,
			Text,
			MIME,
		};
		const TemplateType MyTemplateType;
		mBinary Id;
	protected:
		Template() = delete;
		Template( TemplateType type ) : MyTemplateType( type )
		{
		}
	};
	//NDEF設定テンプレート(Wellknown@T=3)
	class TemplateWellknown : public Template
	{
	public:
	protected:
		TemplateWellknown() = delete;
		TemplateWellknown( TemplateType type ) : Template( type )
		{
		}
	};
	//NDEF設定テンプレート(URL)
	class TemplateUrl : public TemplateWellknown
	{
	public:
		TemplateUrl() : TemplateWellknown( TemplateType::Url )
		{
		}
		//タグに書き込むURL
		//そのまま書き込むので、URLエンコードなどは別途行うこと
		AString Url;
	};
	//NDEF設定テンプレート(Text)
	class TemplateText : public TemplateWellknown
	{
	public:
		TemplateText() : TemplateWellknown( TemplateType::Text )
		{
		}
		//言語コード
		//en=英語、jp=日本語。
		AString LanguageCode;
		//書き込む文字列
		//※UTF-8に変換して書き込まれます
		AString Text;
	};
	//NDEF設定テンプレート(MIME)
	class TemplateMIME : public Template
	{
	public:
		TemplateMIME() : Template( TemplateType::MIME )
		{
		}
		AString MIME;
		mBinary Data;
	};

	//テンプレート追加
	bool AddTemplate( const Template& t );

private:
	mSCNDEF( const mSCNDEF& source );
	const mSCNDEF& operator=( const mSCNDEF& source ) = delete;

protected:

	using TemplatesEntry = std::unique_ptr< Template >;
	using Templates = std::deque< TemplatesEntry >;
	Templates MyTemplates;

	//NDEF Message TLV
	bool CreateNdefTlv( int i , uint8_t header_tnf , const AString& header_type , size_t payload_size , mBinary& retheader )const;

	bool EncodeUrlPayload( const TemplateUrl& t , mBinary& retpayload )const;
	bool EncodeTextPayload( const TemplateText& t , mBinary& retpayload )const;
	bool EncodeMIMEPayload( const TemplateMIME& t , mBinary& retpayload )const;
};


#endif
