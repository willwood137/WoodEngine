#pragma once


namespace woodman
{

	template< typename real >
	class Vector3
	{
	public:
		real x, y, z;

		Vector3( real f = 0 ) : x( f ), y( f ), z( f ){}
		Vector3( real x_, real y_, real z_ ) : x(x_), y(y_), z(z_) {}

		//accessors
		double calcLength();
		double calcLengthSquared();
		Vector3 CrossProduct(const Vector3& v2 ) const;
		real DotProduct(const Vector3& v2 ) const;

		//manipulation
		Vector3& Normalize();
		Vector3& setLength( float scale );
		

		//operators
		bool operator==( const Vector3& rhs ) const;
		Vector3 operator-() const;
		Vector3& operator+=( const Vector3& rhs );
		Vector3& operator-=( const Vector3& rhs );
		Vector3& operator*=( float scale );
		Vector3& operator/=( float inversescale );
		
		real operator[]( size_t i ) const;
		real& operator[]( size_t i );

	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// Calculations/Acessors functions														//
	//////////////////////////////////////////////////////////////////////////////////////////
	template< typename real >
	inline Vector3<real> Vector3<real>::CrossProduct( const Vector3& v2 ) const
	{
		real vx = y * v2.z - z * v2.y;
		real vy = z * v2.x - x * v2.z;
		real vz = x * v2.y - y * v2.x;

		return Vector3(vx, vy, vz );
	}


	template< typename real >
	inline real Vector3<real>::DotProduct( const Vector3& v2 ) const
	{
		return v2.x * x + y * v2.y + z * v2.z;
	}

	template< typename real >
	inline double Vector3<real>::calcLengthSquared()
	{
		return x*x + y*y + z*z;
	}

	template< typename real >
	inline double Vector3<real>::calcLength()
	{
		return sqrt(x*x + y*y + z*z);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Maniuplation functions																//
	//////////////////////////////////////////////////////////////////////////////////////////

	template< typename real >
	inline Vector3<real>& Vector3<real>::Normalize()
	{
		double lengthSquared = x*x + y*y + z*z;
		if( lengthSquared == 0 )
		{
			x = 0;
			y = 0;
			z = 0;
			return *this;
		}
		double iLength = 1 / sqrt( lengthSquared );
		x = static_cast<real>( static_cast<double>(x) * iLength );
		y = static_cast<real>( static_cast<double>(y) * iLength );
		z = static_cast<real>( static_cast<double>(z) * iLength );

		return *this;
	}

	template< typename real >
	inline Vector3<real>& Vector3<real>::setLength( float scale )
	{
		double lengthSquared = x*x + y*y + z*z;
		if( lengthSquared == 0 )
		{
			x = 0;
			y = 0;
			z = 0;
			return *this;
		}

		double iLength = ( 1 / sqrt( lengthSquared ) ) * scale;
		x = static_cast<real>( static_cast<double>(x) * iLength );
		y = static_cast<real>( static_cast<double>(y) * iLength );
		z = static_cast<real>( static_cast<double>(z) * iLength );
		return *this;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	// Operate functions																	//
	//////////////////////////////////////////////////////////////////////////////////////////
	
	template< typename real >
	bool Vector3<real>::operator==( const Vector3<real>& rhs) const
	{
		return ( x == rhs.x
			&&	y == rhs.y
			&&	z == rhs.z );
	}

	template< typename real >
	inline Vector3<real> Vector3<real>::operator-() const
	{
		return (Vector3<real>(-x, -y, -z) );
	}

	template< typename real >
	inline Vector3<real>& Vector3<real>::operator+=(const Vector3<real> &rhs)
	{
		
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}

	template< typename real >
	inline Vector3<real>& Vector3<real>::operator-=(const Vector3<real> &rhs)
	{
		
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;

		return *this;
	}

	template< typename real >
	inline Vector3<real>& Vector3<real>::operator*=( float scale )
	{
		
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}

	template< typename real >
	inline Vector3<real>& Vector3<real>::operator/=( float inversescale )
	{
		float scale = 1 / inversescale;
		
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}


	template< typename real >
	inline real Vector3<real>::operator[]( size_t i ) const
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: 
			return 0;
		}
	}

		
	template< typename real >
	inline real& Vector3<real>::operator[]( size_t i )
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: 
			return x;
		}
	}

	template< typename real >
	inline Vector3<real> operator+( const Vector3<real> &v1, const Vector3<real> &v2)
	{
		Vector3<real> v3;

		v3.x = v1.x + v2.x;
		v3.y = v1.y + v2.y;
		v3.z = v1.z + v2.z;


		return v3;
	}

	template< typename real >
	inline Vector3<real> operator-( const Vector3<real> &v1, const Vector3<real> &v2)
	{
		Vector3<real> v3;

		v3.x = v1.x - v2.x;
		v3.y = v1.y - v2.y;
		v3.z = v1.z - v2.z;


		return v3;
	}
	
	template< typename real >
	inline Vector3<real> operator*( const Vector3<real> &v1, const float scale)
	{
		Vector3<real> v3;

		v3.x = v1.x *scale;
		v3.y = v1.y *scale;
		v3.z = v1.z *scale;


		return v3;
	}

	
	
	typedef Vector3<float> Vector3f;
	typedef Vector3<double> Vector3d;
}