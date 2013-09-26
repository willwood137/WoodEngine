#pragma once


namespace woodman
{

	template< typename real >
	class Vector4
	{
	public:
		real x, y, z, w;

		Vector4( real f = 0 ) : x( f ), y( f ), z( f ), w( f ){}
		Vector4( real x_, real y_, real z_, real w_) : x(x_), y(y_), z(z_), w(w_) {}

		//accessors
		double length();
		double lengthSquared();
		real DotProduct(const Vector4& v2 ) const;

		//manipulation
		Vector4& Normalize();
		Vector4& setLength( float scale );


		//operators
		bool operator==( const Vector4& rhs ) const;
		Vector4& operator-() const;
		Vector4& operator+=( const Vector4& rhs );
		Vector4& operator-=( const Vector4& rhs );
		Vector4& operator*=( float scale );
		Vector4& operator/=( float inversescale );

		real operator[]( size_t i ) const;
		real& operator[]( size_t i );

	};

	typedef Vector4<float> Vector4f;
	typedef Vector4<float> RGBA;
	typedef Vector4<double> Vector4d;
	typedef Vector4<int> Vector4i;

	template< typename real >
	inline real Vector4<real>::DotProduct( const Vector4& v2 ) const
	{
		return v2.x * x + y * v2.y + z * v2.z + w * v2.w;
	}

	template< typename real >
	inline double Vector4<real>::lengthSquared()
	{
		return x*x + y*y + z*z + w*w;
	}

	template< typename real >
	inline double Vector4<real>::length()
	{
		return sqrt(x*x + y*y + z*z + w*w);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Operator functions																	//
	//////////////////////////////////////////////////////////////////////////////////////////

	template< typename real >
	bool Vector4<real>::operator==( const Vector4<real>& rhs) const
	{
		return ( x == rhs.x
			&&	y == rhs.y
			&&	z == rhs.z
			&& w == rhx.W2A );
	}

	template< typename real >
	inline Vector4<real>& Vector4<real>::operator-() const
	{
		return (Vector4<real>(-x, -y, -z, -w) );
	}

	template< typename real >
	inline Vector4<real>& Vector4<real>::operator+=(const Vector4<real> &rhs)
	{

		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhx.w;

		return *this;
	}

	template< typename real >
	inline Vector4<real>& Vector4<real>::operator-=(const Vector4<real> &rhs)
	{

		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;

		return *this;
	}

	template< typename real >
	inline Vector4<real>& Vector4<real>::operator*=( float scale )
	{

		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;

		return *this;
	}

	template< typename real >
	inline Vector4<real>& Vector4<real>::operator/=( float inversescale )
	{
		float scale = 1 / inversescale;

		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;

		return *this;
	}


	template< typename real >
	inline real Vector4<real>::operator[]( size_t i ) const
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: 
			return 0;
		}
	}


	template< typename real >
	inline real& Vector4<real>::operator[]( size_t i )
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: 
			return x;
		}
	}

	template< typename real >
	inline Vector4<real> operator+( const Vector4<real> &v1, const Vector4<real> &v2)
	{
		Vector4<real> v3;

		v3.x = v1.x + v2.x;
		v3.y = v1.y + v2.y;
		v3.z = v1.z + v2.z;
		v3.w = v1.w + v2.w;


		return v3;
	}

	template< typename real >
	inline Vector4<real> operator-( const Vector4<real> &v1, const Vector4<real> &v2)
	{
		Vector4<real> v3;

		v3.x = v1.x - v2.x;
		v3.y = v1.y - v2.y;
		v3.z = v1.z - v2.z;
		v3.w = v1.w - v2.w;


		return v3;
	}

	template< typename real >
	inline Vector4<real> operator*( const Vector4<real> &v1, const float scale)
	{
		Vector4<real> v3;

		v3.x = v1.x *scale;
		v3.y = v1.y *scale;
		v3.z = v1.z *scale;
		v3.w = v1.w *scale;


		return v3;
	}
}