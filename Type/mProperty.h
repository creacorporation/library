#include "mStandard.h"

template< class T , class F = T >
class mProperty
{
public:
	static_assert( std::is_same<std::remove_const<T>::type,std::remove_const<F>::type>::value , "template class mismatch" );
	static_assert( !std::is_same<T,void>::value , "invalid type" );

	template< class T2 = std::enable_if_t<!std::is_const_v<T>,T> , class F2 = std::enable_if_t<!std::is_const_v<F>,F> >
	mProperty()
	{
		MyOn = true;
	}
	template< class T2 = std::enable_if_t<!std::is_const_v<F>,T> >
	mProperty( T2 t )
		: OnValue( t )
	{
		MyOn = true;
	}
	mProperty( T t , F f )
		: OnValue( t ) , OffValue( f )
	{
		MyOn = true;
	}
	mProperty( const mProperty& src )
		: OnValue( src.OnValue ) , OffValue( src.OffValue )
	{
		MyOn = src.MyOn;
	}

	template< class T2 = T , class F2= F >
	const mProperty& operator=( std::enable_if_t<(!std::is_const_v<T2>)&&(!std::is_const_v<F2>),const mProperty&> src )
	{
		MyOn = src.MyOn;
		OnValue = src.OnValue;
		OffValue = src.OffValue;
		return *this;
	}

	~mProperty() = default;

	void On( bool on = true )
	{
		MyOn = on;
	}
	void Off( void )
	{
		On( false );
	}
	bool IsOn( void )const
	{
		return MyOn;
	}

	T OnValue;
	F OffValue;

	template< class value_type = std::remove_const_t<T> >
	operator value_type( void )const
	{
		return Value();
	}

	template< class value_type = std::remove_const_t<T> >
	value_type Value( void )const
	{
		if( MyOn )
		{
			return OnValue;
		}
		return OffValue;
	}



protected:
	bool MyOn;

};
