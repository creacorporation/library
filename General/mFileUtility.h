//----------------------------------------------------------------------------
// ファイルユーティリティ関数群
// Copyright (C) 2018- Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
//----------------------------------------------------------------------------


#ifndef MFILEUTILITY_H_INCLUDED
#define MFILEUTILITY_H_INCLUDED

#include "mStandard.h"
#include "mTCHAR.h"
#include "mDateTime.h"

namespace mFileUtility
{

	//指定したファイル名がファイルシステム上許されるかを判定する
	// filename : 判定したいファイル名
	// ret : 許される名前の場合true、許されない名前の場合false
	bool IsSuitableFilename( const WString& filename );

	//指定したファイル名がファイルシステム上許されるかを判定する
	// filename : 判定したいファイル名
	// ret : 許される名前の場合true、許されない名前の場合false
	bool IsSuitableFilename( const wchar_t* filename );

	//パスをドライブ、ディレクトリ、ファイル名、拡張子に分割する
	// path : 分割前のパス
	// retDrive : 【出力】ドライブ名　不要の場合はnullptrでも可
	// retDir : 【出力】ディレクトリ名　不要の場合はnullptrでも可
	// retFile : 【出力】ファイル名　不要の場合はnullptrでも可
	// retExt : 【出力】拡張子　不要の場合はnullptrでも可
	// ret : 成功の場合真
	bool SplitPath( const WString& path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt );

	//パスをドライブ、ディレクトリ、ファイル名、拡張子に分割する
	// path : 分割前のパス
	// retDrive : 【出力】ドライブ名　不要の場合はnullptrでも可
	// retDir : 【出力】ディレクトリ名　不要の場合はnullptrでも可
	// retFile : 【出力】ファイル名　不要の場合はnullptrでも可
	// retExt : 【出力】拡張子　不要の場合はnullptrでも可
	// ret : 成功の場合真
	bool SplitPath( const wchar_t* path , WString* retDrive , WString* retDir , WString* retFile , WString* retExt );

	//ファイルパスの抜出
	//ファイルパスの指定した要素のみを使用して新しいパスを作ります
	// path : 元のパス
	// retResult : 【出力】再構築したパス
	// drive : 元のパスに含まれているドライブ名を使うならtrue
	// dir : 元のパスに含まれているディレクトリ名を使うならtrue
	// filename : 元のパスに含まれているファイル名を使うならtrue
	// ext : 元のパスに含まれている拡張子を使うならtrue
	bool RebuildPath( const WString& path , WString& retResult , bool drive , bool dir , bool filename , bool ext );

	//ファイルパスの抜出
	//ファイルパスの指定した要素のみを使用して新しいパスを作ります
	// path : 元のパス
	// retResult : 【出力】再構築したパス
	// drive : 元のパスに含まれているドライブ名を使うならtrue
	// dir : 元のパスに含まれているディレクトリ名を使うならtrue
	// filename : 元のパスに含まれているファイル名を使うならtrue
	// ext : 元のパスに含まれている拡張子を使うならtrue
	bool RebuildPath( const wchar_t* path , WString& retResult , bool drive , bool dir , bool filename , bool ext );

	//パスをドライブ、ディレクトリ、ファイル名、拡張子を差し替える
	// path : 差し替え前のパス
	// retpath : 差し替えたパス（出力）
	// retDrive : 差し替えるドライブ名　差し替え不要の場合はnullptr
	// retDir : 差し替えるディレクトリ名　差し替え不要の場合はnullptr
	// retFile : 差し替えるファイル名　差し替え不要の場合はnullptr
	// retExt : 差し替える拡張子　差し替え不要の場合はnullptr
	// ret : 成功の場合真
	bool ReplacePath( const WString& path , WString& retPath , const WString* Drive , const WString* Dir , const WString* File , const WString* Ext );

	//パスをドライブ、ディレクトリ、ファイル名、拡張子を差し替える
	// path : 差し替え前のパス
	// retpath : 差し替えたパス（出力）
	// retDrive : 差し替えるドライブ名　差し替え不要の場合はnullptr
	// retDir : 差し替えるディレクトリ名　差し替え不要の場合はnullptr
	// retFile : 差し替えるファイル名　差し替え不要の場合はnullptr
	// retExt : 差し替える拡張子　差し替え不要の場合はnullptr
	// ret : 成功の場合真
	bool ReplacePath( const WString& path , WString& retPath , const wchar_t* Drive , const wchar_t* Dir , const wchar_t* File , const wchar_t* Ext );

	//パスをドライブ、ディレクトリ、ファイル名、拡張子を差し替える
	// path1 : 差し替え前のパス
	// path2 : 差し替え前のパス
	// retpath : 差し替えたパス（出力）
	// Drive : true=path2を使う、false=path1を使う
	// Dir : true=path2を使う、false=path1を使う
	// File : true=path2を使う、false=path1を使う
	// Ext : true=path2を使う、false=path1を使う
	// ret : 成功の場合真
	bool ReplacePath( const WString& path1 , const WString& path2 , WString& retPath , bool drive , bool dir , bool filename , bool ext );

	//パスを連結する
	//ディレクトリ名、ファイル名、拡張子を連結して返します。ディレクトリ名の末尾のバックスラッシュがない場合は
	//自動的に補います。
	// dir : ディレクトリ名
	// file : ファイル名
	WString CombinePath( const WString& dir , const WString& file = L"" , const WString& ext = L"" );

	//相対パスを絶対パスに変換します
	// filename : 相対パス
	// ret : 変換した絶対パス(エラーの場合、空文字列)
	WString GetFullPath( const WString& filename )noexcept;

	//相対パスを絶対パスに変換します
	// filename : 相対パス
	// ret : 変換した絶対パス(エラーの場合例外を投げる)
	template< class ExceptionIfFailed >
	inline WString GetFullPath( const WString& filename )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result = GetFullPath( filename );
		if( result == L"" )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}

	//相対パスを絶対パスに変換します
	// filename : 相対パス
	// ret : 変換した絶対パス(エラーの場合、空文字列)
	WString GetFullPath( const wchar_t* filename )noexcept;

	//相対パスを絶対パスに変換します
	// filename : 相対パス
	// ret : 変換した絶対パス(エラーの場合例外を投げる)
	template< class ExceptionIfFailed >
	inline WString GetFullPath( const wchar_t* filename )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result = GetFullPath( filename );
		if( result == L"" )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}

	//パスが不正の場合のエラーコード
	static const DWORD ERR_BADPATH = 0x20000000;

	//指定したパスが、あるパス以下であるかを判定します
	// path : 検査するパス
	// rootpath : 検査基準になるパス
	// ret : true  = pathはrootpath以下である(pathはrootpathのサブディレクトリである)
	//       false = pathはrootpath以下ではない(pathはrootpathのサブディレクトリではない)
	//               ※GetLastError()はERROR_SUCCESS
	//       false = いずれかのパスが不正
	//               ※GetLastError()はERR_BADPATH
	bool CheckPathTraversal( const WString& path , const WString& rootpath )noexcept;

	//指定したパスが、あるパス以下であるかを判定します
	// path : 検査するパス
	// rootpath : 検査基準になるパス
	// ret : true  = pathはrootpath以下である(pathはrootpathのサブディレクトリである)
	//       false = pathはrootpath以下ではない(pathはrootpathのサブディレクトリではない)
	// エラー発生時は例外
	template< class ExceptionIfFailed >
	inline WString CheckPathTraversal( const WString& path , const WString& rootpath )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		if( !CheckPathTraversal( path , rootpath ) )
		{
			if( GetLastError() == ERR_BADPATH )
			{
				throw ExceptionIfFailed();
			}
			return false;
		}
		return true;
	}

	//指定したパスが、あるパス以下であるかを判定します
	// path : 検査するパス
	// rootpath : 検査基準になるパス
	// ret : true  = pathはrootpath以下である(pathはrootpathのサブディレクトリである)
	//       false = pathはrootpath以下ではない(pathはrootpathのサブディレクトリではない)
	bool CheckPathTraversal( const wchar_t* path , const wchar_t* rootpath ) noexcept;

	//指定したパスが、あるパス以下であるかを判定します
	// path : 検査するパス
	// rootpath : 検査基準になるパス
	// ret : true  = pathはrootpath以下である(pathはrootpathのサブディレクトリである)
	//       false = pathはrootpath以下ではない(pathはrootpathのサブディレクトリではない)
	template< class ExceptionIfFailed >
	inline WString CheckPathTraversal( const wchar_t* path , const wchar_t* rootpath )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		if( !CheckPathTraversal( path , rootpath ) )
		{
			if( GetLastError() == ERR_BADPATH )
			{
				throw ExceptionIfFailed();
			}
			return false;
		}
		return true;
	}

	//テンポラリフォルダのパスを得る
	// ret : テンポラリフォルダのパス
	WString GetTempDirPath( void );

	//テンポラリファイルのパスを得る
	// folder : テンポラリフォルダのパス(空白にするとシステムから取得)
	// prefix : テンポラリファイル名の先頭につけるプレフィクス
	// ret : テンポラリファイルのパス
	WString GetTempFilePath( const WString& folder , const WString& prefix );

	//パス名"from"からパス名"to"への相対パスを返します
	// from : 相対パスの元
	// to : 相対パスの先
	// ret : fromからtoへの相対パス。相対パスが計算不能の場合空文字列。
	WString GetRelativePath( const WString& from , const WString& to ) noexcept;

	//パス名"from"からパス名"to"への相対パスを返します
	// from : 相対パスの元
	// to : 相対パスの先
	// ret : fromからtoへの相対パス。相対パスが計算不能の場合例外
	template< class ExceptionIfFailed >
	inline WString GetRelativePath( const WString& from , const WString& to )
	{
		static_assert( std::is_base_of<mExceptionBase,ExceptionIfFailed>::value == true , "Exception class is not derived from mExceptionBase" );
		WString result = GetRelativePath( from ,  to );
		if( result == L"" )
		{
			throw ExceptionIfFailed();
		}
		return result;
	}

	//指定したディレクトリを（中間ディレクトリを含めて）作成します
	// path : 作成するディレクトリ
	// ret : 成功時真
	bool CreateMiddleDirectory( const WString& path )noexcept;

	//指定したパスが存在するかしないかを返します
	bool IsPathValid( const AString& path );

	//指定したパスが存在するかしないかを返します
	bool IsPathValid( const WString& path );

	//unixのtouchコマンド相当の処理をします（最終更新時刻の更新）
	// path : touchするファイル
	// create : true  もしファイルがなかったらそのファイルを作る（本来のtouchの挙動）
	//          false もしファイルがなかったらエラーにする
	bool Touch( const AString& path , bool create = true );

	//unixのtouchコマンド相当の処理をします（最終更新時刻の更新）
	// path : touchするファイル
	// create : true  もしファイルがなかったらそのファイルを作る（本来のtouchの挙動）
	//          false もしファイルがなかったらエラーにする
	bool Touch( const WString& path , bool create = true );

	//ファイルの更新時刻を得ます
	bool GetFileTime(
		const AString& path,
		mDateTime::Timestamp* retCreationTime,
		mDateTime::Timestamp* retLastAccessTime,
		mDateTime::Timestamp* retLastWriteTime
	);

	//ファイルの更新時刻を得ます
	bool GetFileTime(
		const WString& path,
		mDateTime::Timestamp* retCreationTime,
		mDateTime::Timestamp* retLastAccessTime,
		mDateTime::Timestamp* retLastWriteTime
	);
};

#endif
