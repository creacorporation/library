//----------------------------------------------------------------------------
// STLコンテナ拡張用テンプレート
// Copyright (C) 2026 Crea Inc. All rights reserved.
// This program is released under the MIT License. 
// see http://opensource.org/licenses/mit-license.php
// 著作権表示やライセンスの改変は禁止されています。
// このソースコードに関して、上記ライセンス以外の契約等は一切存在しません。
// (何らかの契約がある場合でも、本ソースコードはその対象外となります)
//----------------------------------------------------------------------------

#ifndef MCONTAINEREXTENDER_H_INCLUDED
#define MCONTAINEREXTENDER_H_INCLUDED

#include <type_traits>
#include <utility>
#include <iterator>
#include <numeric>

template <class base_container>
class mContainerExtender
{
private:
	//存在確認
	template <class...> using void_t = void;

	//push_back
	template <class , class = void>
	struct has_push_back : std::false_type
	{};
	template <class C>
	struct has_push_back<C , void_t<decltype( std::declval<C&>().push_back( std::declval<typename C::value_type>() ) )>> : std::true_type
	{};

	//push_front
	template <class , class = void>
	struct has_push_front : std::false_type
	{};
	template <class C>
	struct has_push_front<C , void_t<decltype( std::declval<C&>().push_front( std::declval<typename C::value_type>() ) )>> : std::true_type
	{};

	//insert()
	template <class , class = void> 
	struct has_insert : std::false_type
	{};
	template <class C> 
	struct has_insert<C , void_t<decltype( std::declval<C&>().insert( std::declval<typename C::value_type>() ) )>> : std::true_type 
	{};

	//reserve()
	template <class , class = void> 
	struct has_reserve : std::false_type
	{};
	template <class C> 
	struct has_reserve<C , void_t<decltype( std::declval<C&>().reserve( std::declval<std::size_t>() ) )>> : std::true_type 
	{};

	//data()
	template <class , class = void>
	struct has_data : std::false_type
	{};
	template <class C>
	struct has_data<C , void_t<decltype( std::declval<C&>().data() )>> : std::true_type
	{};

	//c_str()
	template <class , class = void>
	struct has_c_str : std::false_type
	{};
	template <class C>
	struct has_c_str<C , void_t<decltype( std::declval<C&>().c_str() )>> : std::true_type
	{};

	//operator[]
	template <class , class = void>
	struct has_subscript : std::false_type
	{};
	template <class C>
	struct has_subscript<C , void_t<decltype( std::declval<C&>()[std::declval<std::size_t>()] )>> : std::true_type
	{};

	//front
	template <class , class = void>
	struct has_front : std::false_type
	{};
	template <class C>
	struct has_front<C , void_t<decltype( std::declval<C&>().front() )>> : std::true_type
	{};

	//back
	template <class , class = void>
	struct has_back : std::false_type
	{};
	template <class C>
	struct has_back<C , void_t<decltype( std::declval<C&>().back() )>> : std::true_type
	{};

	//map系かどうか
	template <class , class = void>
	struct is_map_like : std::false_type
	{};
	template <class C> 
	struct is_map_like<C , void_t<typename C::mapped_type>> : std::true_type 
	{};

public:
	using value_type = typename base_container::value_type;
	using iterator = typename base_container::iterator;
	using const_iterator = typename base_container::const_iterator;

	//構築
	mContainerExtender() = default;
	virtual ~mContainerExtender() = default;

	explicit mContainerExtender( base_container c ) : c_( std::move( c ) )
	{
	}

	template <class... args>
	explicit mContainerExtender( std::piecewise_construct_t , args&&... arg_array ) : c_( std::forward<arg_array>( arg_array )... )
	{
	}

	//サイズとイテレーター
	std::size_t    size()   const noexcept { return c_.size();   }
	bool           empty()  const noexcept { return c_.empty();  }
	iterator       begin()        noexcept { return c_.begin();  }
	const_iterator begin()  const noexcept { return c_.begin();  }
	iterator       end()          noexcept { return c_.end();    }
	const_iterator end()    const noexcept { return c_.end();    }
	const_iterator cbegin() const noexcept { return c_.cbegin(); }
	const_iterator cend()   const noexcept { return c_.cend();   }
	void           clear()        noexcept { c_.clear();         }

	//operator[] / at
	template <class C = base_container>
	typename std::enable_if<has_subscript<C>::value , decltype( std::declval<C&>()[std::declval<std::size_t>()] )>::type operator[]( std::size_t i )
	{
		return c_[i];
	}
	template <class C = base_container>
	typename std::enable_if<has_subscript<const C>::value , decltype( std::declval<const C&>()[std::declval<std::size_t>()] )>::type operator[]( std::size_t i ) const
	{
		return c_[i];
	}
	template <class C = base_container>
	typename std::enable_if<has_subscript<C>::value , decltype( std::declval<C&>()[std::declval<std::size_t>()] )>::type at( std::size_t i )
	{
		return c_.at(i);
	}
	template <class C = base_container>
	typename std::enable_if<has_subscript<const C>::value , decltype( std::declval<const C&>()[std::declval<std::size_t>()] )>::type at( std::size_t i ) const
	{
		return c_.at(i);
	}

	//operator=
	template <class C = base_container>
	const mContainerExtender<C>& operator=( const C& src )
	{
		c_ = src;
		return *this;
	}

	template <class C = base_container>
	const mContainerExtender<C>& operator=( const mContainerExtender<C>& src )
	{
		c_ = src.c_;
		return *this;
	}

	//data()/c_str()
	template <class C = base_container>
	typename std::enable_if<has_data<C>::value , decltype( std::declval<C&>().data() )>::type data() noexcept 
	{
		return c_.data();
	}
	template <class C = base_container>
	typename std::enable_if<has_data<const C>::value , decltype( std::declval<const C&>().data() )>::type data() const noexcept
	{
		return c_.data(); 
	}
	template <class C = base_container>
	typename std::enable_if<has_c_str<C>::value , decltype( std::declval<C&>().c_str() )>::type c_str() noexcept 
	{
		return c_.c_str();
	}
	template <class C = base_container>
	typename std::enable_if<has_c_str<const C>::value , decltype( std::declval<const C&>().c_str() )>::type c_str() const noexcept
	{
		return c_.c_str(); 
	}

	//push_back/pop_back/push_front/pop_front
	template <class C = base_container , class T>
	typename std::enable_if<has_push_back<C>::value , void>::type push_back( T&& v )
	{
		c_.push_back( std::forward<T>( v ) );
	}
	template <class C = base_container , class T>
	typename std::enable_if<has_push_back<C>::value , void>::type pop_back( void )
	{
		c_.pop_back();
	}
	template <class C = base_container , class T>
	typename std::enable_if<has_push_front<C>::value , void>::type push_front( T&& v )
	{
		c_.push_front( std::forward<T>( v ) );
	}
	template <class C = base_container , class T>
	typename std::enable_if<has_push_front<C>::value , void>::type pop_front( void )
	{
		c_.pop_front();
	}

	//front/back
	template <class C = base_container>
	typename std::enable_if<has_front<C>::value , decltype( std::declval<C&>().front() )>::type front() noexcept
	{
		return c_.front(); 
	}
	template <class C = base_container>
	typename std::enable_if<has_front<const C>::value , decltype( std::declval<const C&>().front() )>::type front() const noexcept
	{
		return c_.front(); 
	}
	template <class C = base_container>
	typename std::enable_if<has_back<C>::value , decltype( std::declval<C&>().back() )>::type back() noexcept
	{
		return c_.back(); 
	}
	template <class C = base_container>
	typename std::enable_if<has_back<const C>::value , decltype( std::declval<const C&>().back() )>::type back() const noexcept
	{
		return c_.back(); 
	}

	//reserve
	template <class C = base_container>
	typename std::enable_if<has_reserve<C>::value , decltype( std::declval<C&>().reserve( std::declval<std::size_t>() ) )>::type reserve( std::size_t sz )
	{
		return c_.reserve( sz );
	}

	//insert
	template <class C = base_container , class T = value_type>
	typename std::enable_if<has_insert<C>::value , decltype( std::declval<C&>().insert( std::declval<T>() ) )>::type insert( T&& v )
	{
		return c_.insert( std::forward<T>( v ) );
	}

protected:
	base_container c_;

private:
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_subscript<C>::value , decltype( std::declval<C&>()[std::declval<std::size_t>()] )>::type operator[]( std::size_t i );
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_subscript<const C>::value , decltype( std::declval<const C&>()[std::declval<std::size_t>()] )>::type operator[]( std::size_t i ) const;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_data<C>::value , decltype( std::declval<C&>().data() )>::type data() noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_data<const C>::value , decltype( std::declval<const C&>().data() )>::type data() const noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_c_str<C>::value , decltype( std::declval<C&>().c_str() )>::type c_str() noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_c_str<const C>::value , decltype( std::declval<const C&>().c_str() )>::type c_str() const noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container , class T>
	typename std::enable_if<!has_push_back<C>::value , void>::type push_back( T&& v );
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container , class T>
	typename std::enable_if<!has_push_back<C>::value , void>::type pop_back( void );
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container , class T>
	typename std::enable_if<!has_push_front<C>::value , void>::type push_front( T&& v );
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container , class T>
	typename std::enable_if<!has_push_front<C>::value , void>::type pop_front( void );
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_front<C>::value , decltype( std::declval<C&>().front() )>::type front() noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_front<const C>::value , decltype( std::declval<const C&>().front() )>::type front() const noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_back<C>::value , decltype( std::declval<C&>().back() )>::type back() noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_back<const C>::value , decltype( std::declval<const C&>().back() )>::type back() const noexcept;
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container>
	typename std::enable_if<!has_reserve<C>::value , decltype( std::declval<C&>().reserve( std::declval<std::size_t>() ) )>::type reserve( std::size_t sz );
	//【理由】このメソッドは基底クラスのコンテナにありません
	template <class C = base_container , class T = value_type>
	typename std::enable_if<!has_insert<C>::value , decltype( std::declval<C&>().insert( std::declval<T>() ) )>::type insert( T&& v );



};


#endif

