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

			//まだない
			//Wifi,
			//Geolocation,
			//vCard,
			//Binary,
		};
		const TemplateType MyTemplateType;
		mBinary Id;
	protected:
		Template() = delete;
		Template( TemplateType type ) : MyTemplateType( type )
		{
		}
	};
	//NDEF設定テンプレート(URL)
	class TemplateUrl : public Template
	{
	public:
		TemplateUrl() : Template( TemplateType::Url )
		{
		}
		//TemplateUrl( const TemplateUrl& src ) : Template( TemplateType::Url )
		//{
		//	Url = src.Url;
		//}
		//タグに書き込むURL
		//そのまま書き込むので、URLエンコードなどは別途行うこと
		AString Url;
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

	bool EncodeUrlPayload( const TemplateUrl& t , mBinary& retpayload )const;
};


#endif
