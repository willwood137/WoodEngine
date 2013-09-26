
#pragma once
#include <math.h>
const double PI = 3.14159265358979323846;

const float CONVERT_TO_RADIANS = (float)PI / 180.0f;

const float CONVERT_TO_DEGREES = 180.0f / (float)PI;

namespace woodman
{
	template< typename real >
	class Vector2
	{
		public:
			Vector2();
			explicit Vector2( real xval, real yval );
			Vector2( const Vector2& rhs );
			
			Vector2& setXY(real x, real y);
			Vector2& setUnitLengthAndYawRadians( float YawRadians);
			Vector2& setUnitLengthAndYawDegrees( float YawDegrees );
			Vector2& setLengthAndYawRadians( real length, float YawRadians );
			Vector2& setLengthAndYawDegrees( real length, float YawDegrees );
			Vector2& setYawDegrees( float yawDegrees );
			Vector2& setYawRadians( float yawRadians );
			Vector2& normalize();
			Vector2& setLength( real length);
			Vector2& scale( float scale );
			Vector2& multiplyComponents( float ScaleX, float ScaleY );
			Vector2& rotateDegrees( float Degrees );
			Vector2& rotateRadians( float Radians );
			Vector2& rotate90Degrees();
			Vector2& rotateMinus90Degrees();
			Vector2& reverse();

			Vector2 calcMinVector2( const Vector2& otherVectorComparingWith ) const;
			Vector2 calcMaxVector2( const Vector2& otherVectorComparingWith ) const;

			float calcLength() const;
			float calcLengthSquared() const;

			float calcYawRadians() const;
			float calcYawDegrees() const;

			float dotProduct( const Vector2& V ) const;
			float crossProduct( const Vector2& V ) const;

			Vector2& operator+=( const Vector2& V );
			Vector2& operator-=( const Vector2& V );
			Vector2& operator*=( float Scale );
			Vector2& operator/=( float InverseScale );
			Vector2 operator-() const;
			Vector2 operator-( const Vector2& V ) const;
			Vector2 operator+( const Vector2& V ) const;
			Vector2 operator*( float Scale ) const;
			Vector2 operator/( float InverseScale ) const;
			bool operator==( const Vector2& V ) const;
			bool operator!=( const Vector2& V ) const;
			Vector2& operator=( const Vector2& rhs );

			// Class Variables
			real x, y;

			//accessor functions
			real operator[]( size_t i ) const;
			real& operator[]( size_t i );
	};

	

	template< typename real >
	inline Vector2<real>::Vector2()
	{

	}

	template< typename real >
	inline Vector2<real>::Vector2( real xval, real yval )
		: x( xval ), 
		  y( yval )
	{

	}

	template< typename real >
	inline Vector2<real>::Vector2( const Vector2& rhs )
		: x( rhs.x ),
		  y( rhs.y )
	{
		
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setXY( real new_x, real new_y )
	{ 
		x = new_x;
		y = new_y;
		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setLength( real length )
	{
		normalize();

		x *= length;
		y *= length;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setUnitLengthAndYawRadians( float YawRadians)
	{
		x = cos(YawRadians);
		y = sin(YawRadians);

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setUnitLengthAndYawDegrees( float YawDegrees )
	{
		float YawRadians = YawDegrees * CONVERT_TO_RADIANS;

		x = cos(YawRadians);
		y = sin(YawRadians);

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setLengthAndYawRadians( real length, float YawRadians )
	{
		x = length * cos(YawRadians);
		y = length * sin(YawRadians);

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setLengthAndYawDegrees( real length, float YawDegrees )
	{
		float YawRadians = YawDegrees * CONVERT_TO_RADIANS;

		x = length * cos(YawRadians);
		y = length * sin(YawRadians);

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setYawDegrees( float yawDegrees )
	{
		const float fYawRadians = yawDegrees * CONVERT_TO_RADIANS;
		const float fLength = ( calcLengthSquared() == 1 ) ? ( 1 ) : ( calcLength() );
		
		x = cos( fYawRadians ) * fLength;
		y = sin( fYawRadians ) * fLength;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::setYawRadians( float yawRadians )
	{
		const float fLength = ( calcLengthSquared() == 1 ) ? ( 1 ) : ( calcLength() );
		
		x = cos( yawRadians ) * fLength;
		y = sin( yawRadians ) * fLength;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::normalize()
	{
		float lengthsquared = x * x + y * y;
		if( lengthsquared != 1 )
		{
			if( lengthsquared != 0)
			{
				float ilength = 1 / sqrt( lengthsquared );
				x *= ilength;
				y *= ilength;
			}
			else
			{
				x = 0.0f; //.707106781f;
				y = 0.0f; //.707106781f;
			}
		}
		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::scale( float scale )
	{
		x *= scale;
		y *= scale;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::multiplyComponents( float ScaleX, float ScaleY )
	{
		x *= ScaleX;
		y *= ScaleY;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::rotateDegrees( float Degrees )
	{
		float radians = Degrees * CONVERT_TO_RADIANS;
		const float cosr = cos( radians );
		const float sinr = sin( radians );

		const float fNewX = x * cosr + ( y * -sinr );
		const float fNewY = x * sinr + ( y * cosr );

		x = fNewX;
		y = fNewY;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::rotateRadians( float Radians )
	{
		const float cosr = cos( Radians );
		const float sinr = sin( Radians );

		const float fNewX = x * cosr + ( y * -sinr );
		const float fNewY = x * sinr + ( y * cosr );

		x = fNewX;
		y = fNewY;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::rotate90Degrees()
	{
		float tempx = x;
		x = -y;
		y = tempx;
		
		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::rotateMinus90Degrees()
	{
		float tempx = x;
		x = y;
		y = -tempx;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::reverse()
	{
		x *= -1;
		y *= -1;

		return *this;
	}

	template< typename real >
	inline float Vector2<real>::calcLength() const
	{
		return sqrt( x * x + y * y );
	}
	
	template< typename real >
	inline float Vector2<real>::calcLengthSquared() const
	{
		return x * x + y * y;	
	}

	template< typename real >
	inline float Vector2<real>::calcYawRadians() const
	{
		return atan2( y, x );
	}

	template< typename real >
	inline float Vector2<real>::calcYawDegrees() const
	{
		
		return atan2( y, x ) * CONVERT_TO_DEGREES;
	}

	template< typename real >
	inline float Vector2<real>::dotProduct( const Vector2& V ) const
	{
		return x * V.y + y * V.x;
	}

	template< typename real >
	inline float Vector2<real>::crossProduct( const Vector2& V ) const
	{
		return x * V.y - y * V.x;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::operator=( const Vector2& rhs )
	{
		x = rhs.x;
		y = rhs.y;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::operator+=( const Vector2& V )
	{
		x += V.x;
		y += V.y;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::operator-=( const Vector2& V )
	{
		x -= V.x;
		y -= V.y;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::operator*=( float Scale )
	{
		x *= Scale;
		y *= Scale;

		return *this;
	}

	template< typename real >
	inline Vector2<real>& Vector2<real>::operator/=( float InverseScale )
	{
		if(InverseScale != 0)
		{
			float Scale = 1 / InverseScale;
			x *= Scale;
			y *= Scale;
		}

		return *this;
	}

	template< typename real >
	inline Vector2<real> Vector2<real>::operator-() const
	{
		return Vector2<real>( -x, -y );
	}

	template< typename real >
	inline Vector2<real> Vector2<real>::operator-( const Vector2& V ) const
	{
		return Vector2<real>(x-V.x, y-V.y);
	}

	template< typename real >
	inline Vector2<real> Vector2<real>::operator+( const Vector2& V ) const 
	{
		return Vector2<real>(x + V.x, y + V.y);
	}

	template< typename real >
	inline Vector2<real> Vector2<real>::operator*( float Scale ) const 
	{
		return Vector2<real>(x * Scale, y * Scale );
	}

	template< typename real >
	inline Vector2<real> Vector2<real>::operator/( float InverseScale ) const 
	{
		if(InverseScale != 0)
		{
			float Scale = 1 / InverseScale;
			return Vector2<real>(x * Scale, y * Scale);
		}

		return *this;
	}

	template< typename real >
	inline bool Vector2<real>::operator==( const Vector2& V ) const
	{
		return ( x == V.x && y == V.y);
	}

	template< typename real >
	inline bool Vector2<real>::operator!=( const Vector2& V ) const 
	{
		return !( x == V.x && y == V.y);
	}

	template< typename real >
	inline real Vector2<real>::operator[]( size_t i ) const
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		default: 
			return 0;
		}
	}


	template< typename real >
	inline real& Vector2<real>::operator[]( size_t i )
	{
		switch(i)
		{
		case 0: return x;
		case 1: return y;
		default: 
			return x;
		}
	}
	template< typename real >
	inline Vector2<real> Vector2<real>::calcMinVector2( const Vector2<real>& otherVectorComparingWith ) const
	{
		return Vector2<real>( calcMin( x, otherVectorComparingWith.x ),
			calcMin( y, otherVectorComparingWith.y ) );
	}

	template< typename real >
	inline Vector2<real> Vector2<real>::calcMaxVector2( const Vector2<real>& otherVectorComparingWith ) const
	{
		return Vector2<real>( calcMax( x, otherVectorComparingWith.x ),
			calcMax( y, otherVectorComparingWith.y ) );
	}


	typedef Vector2<int> Vector2i;
	typedef Vector2<float> Vector2f;
	typedef Vector2<double> Vector2d;
}
