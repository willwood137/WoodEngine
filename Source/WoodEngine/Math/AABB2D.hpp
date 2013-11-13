/*
 * HGAM 6311: Software Development I
 * Team Project | Salvation
 *
 * Benjamin Allen, Madhu Kishore, Benjamin Klingler, William Wood
 * 2.28.2012
 *
 * AABB2D.h
 */

#pragma once

#include "Vector2.hpp"
#include <assert.h>

namespace woodman
{

	class AABB2D
	{
		public:
			AABB2D();
			explicit AABB2D( const Vector2f& anyPoint1, const Vector2f& anyPoint2 );
			explicit AABB2D( const Vector2f& point );
			AABB2D::AABB2D( const AABB2D& rhs );

			AABB2D& setMinMax( const Vector2f& anyPoint1, const Vector2f& anyPoint2 );
			AABB2D& setCenterSize( const Vector2f& centerPoint, const Vector2f& sizeVector );
			AABB2D& pad( float padding );

			AABB2D& stretchToIncludePoint( const Vector2f& point);
			AABB2D& stretchToIncludeAABB2D( const AABB2D& box );
			AABB2D& clip(const AABB2D& box );
			AABB2D& Translate( const Vector2f& translation );
			AABB2D& ScaleUniform( float scale );
			AABB2D& ScaleNonUniform( float scaleX, float scaleY );
			AABB2D& ScaleNonUniform( const Vector2f& scale );
			
			Vector2f calcCenter() const;
			Vector2f calcSize() const;
			float calcWidth() const;
			float calcHeight() const;


			bool isOverlappingWith( const AABB2D& TargetBox ) const;
			AABB2D calcOverlapWith( const AABB2D& comparisonAABB ) const;
			bool isPointInsideBounds( const Vector2f& point ) const;
			float calcDistanceSquaredToPoint( const Vector2f& point ) const;

			Vector2f& clampPointToBounds( Vector2f& point ) const;
			AABB2D clampBoxToBounds( const AABB2D& boxToClamp ) const;

			AABB2D calcUniformScaleRequiredToFitInsideBox( const AABB2D& theBoxIWantToFitInside ) const;
			AABB2D calcLargestCenteredInteriorAABB2DOfGivenAspectRatio( float aspect ) const;
			
			AABB2D& operator+=( const Vector2f& Translation );
			AABB2D& operator-=( const Vector2f& antiTranslation );
			AABB2D& operator*=( float UniformScale );
			AABB2D& operator*=( const Vector2f& nonUniformScale );
			AABB2D operator-( const Vector2f& antiTranslation ) const;
			AABB2D operator+( const Vector2f& Translation ) const;
			AABB2D operator*( float uniformScale ) const;
			AABB2D operator*( const Vector2f& nonUniformScale ) const;
			bool operator==( const AABB2D& rhs ) const;
			bool operator!=( const AABB2D& rhs ) const;

			// Mutator Member Functions
			AABB2D& operator=( const AABB2D& rhs );

			// TODO: Everything else.

	
			Vector2f m_vMin;
			Vector2f m_vMax;
	};

	static const AABB2D NULL_AABB = AABB2D( Vector2f(.0f, .0f) );

	inline AABB2D::AABB2D()
	{

	}

	inline AABB2D::AABB2D( const Vector2f& point )
		: m_vMin( point ),
		  m_vMax( point )
	{

	}

	inline AABB2D::AABB2D( const Vector2f& anyPoint1, const Vector2f& anyPoint2 )
		: m_vMin( anyPoint1.calcMinVector2( anyPoint2 ) ),
		  m_vMax( anyPoint1.calcMaxVector2( anyPoint2 ) )
	{
		
	}

	inline AABB2D::AABB2D( const AABB2D& rhs )
		: m_vMin( rhs.m_vMin ),
		  m_vMax( rhs.m_vMax )
	{
		
	}

	inline AABB2D& AABB2D::setMinMax( const Vector2f& anyPoint1, const Vector2f& anyPoint2 )
	{
		m_vMin = anyPoint1.calcMinVector2( anyPoint2 );
		m_vMax = anyPoint1.calcMaxVector2( anyPoint2 );

		return *this;
	}

	inline AABB2D& AABB2D::setCenterSize( const Vector2f& centerPoint, const Vector2f& sizeVector )
	{
		Vector2f halfSizeVector = sizeVector * 0.5f;
		m_vMin = centerPoint - halfSizeVector;
		m_vMax = centerPoint + halfSizeVector;

		return *this;
	}

	inline AABB2D& AABB2D::operator=( const AABB2D& rhs )
	{
		m_vMin = rhs.m_vMin;
		m_vMax = rhs.m_vMax;

		return *this;
	}
	
	inline bool AABB2D::isOverlappingWith( const AABB2D& TargetBox ) const
	{
		return ( !( ( m_vMin.x > TargetBox.m_vMax.x ) || ( m_vMax.x < TargetBox.m_vMin.x )
 				|| ( m_vMin.y > TargetBox.m_vMax.y ) || ( m_vMax.y < TargetBox.m_vMin.y ) ) );
	}

	inline Vector2f AABB2D::calcCenter() const
	{
		return Vector2f( ( m_vMin.x + m_vMax.x ) * 0.5f, ( m_vMin.y + m_vMax.y ) * 0.5f );
	}

	inline Vector2f AABB2D::calcSize() const
	{
		return Vector2f( m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y );
	}

	inline float AABB2D::calcWidth() const
	{
		return m_vMax.x - m_vMin.x;
	}

	inline float AABB2D::calcHeight() const
	{
		return m_vMax.y - m_vMin.y;
	}

	inline AABB2D& AABB2D::pad( float padding )
	{
		float width = calcWidth();
		float height = calcHeight();
		float newArea = width * height + padding;
		float ratio = width / height;
		float x, y;


		x = sqrt( newArea * ratio );
		y = x / ratio;

		float dx = x * .5f;
		float dy = y * .5f;

		m_vMax += Vector2f( dx, dy);
		m_vMin -= Vector2f( dx, dy);

		return *this;
	}

	inline AABB2D& AABB2D::stretchToIncludePoint( const Vector2f& point  )
	{
		if( point.x > m_vMax.x )
			m_vMax.x = point.x;

		if( point.y > m_vMax.y )
			m_vMax.y = point.y;

		if( point.x < m_vMin.x )
			m_vMin.x = point.x;
	
		if( point.y < m_vMin.y )
			m_vMin.y = point.y;

		return *this;
	}

	inline AABB2D& AABB2D::stretchToIncludeAABB2D( const AABB2D& box )
	{
		stretchToIncludePoint( box.m_vMin );
		stretchToIncludePoint( box.m_vMax );

		return *this;
	}

	inline AABB2D& AABB2D::clip( const AABB2D& box )
	{
		m_vMin.x = max(m_vMin.x, box.m_vMin.x);
		m_vMax.x = min(m_vMax.x, box.m_vMax.x);
		if(m_vMin.x > m_vMax.x)
			m_vMax.x = m_vMin.x;


		m_vMin.y = max(m_vMin.y, box.m_vMin.y);
		m_vMax.y = min(m_vMax.y, box.m_vMax.y);
		if(m_vMin.y > m_vMax.y)
			m_vMax.y = m_vMin.y;

		return *this;
	}

	inline AABB2D& AABB2D::Translate( const Vector2f& translation )
	{
		m_vMin += translation;
		m_vMax += translation;

		return *this;
	}

	inline AABB2D& AABB2D::ScaleUniform( float scale )
	{
		float realscale = scale *.25f;
		m_vMin -= Vector2f(calcWidth(), calcHeight() ) * (realscale);
		m_vMax += Vector2f(calcWidth(), calcHeight() ) * (realscale);

		return *this;
	}

	inline AABB2D& AABB2D::ScaleNonUniform( float scaleX, float scaleY )
	{
		Vector2f center = calcCenter(); 
		float dx = calcWidth() * scaleX * .5f;
		float dy = calcHeight() * scaleY * .5f;

		Vector2f delta = Vector2f(dx, dy);
		m_vMin = center - delta;
		m_vMax = center + delta;

		return *this;
	}

	inline AABB2D& AABB2D::ScaleNonUniform( const Vector2f& scale )
	{
		ScaleNonUniform( scale.x, scale.y );

		return *this;
	}

	inline bool AABB2D::isPointInsideBounds(const Vector2f& point ) const
	{
		return ( ( ( m_vMin.x <= point.x ) && ( m_vMax.x >= point.x ) )
 				&& ( ( m_vMin.y <= point.y ) && ( m_vMax.y >= point.y ) ) );
	}

	inline AABB2D AABB2D::calcUniformScaleRequiredToFitInsideBox( const AABB2D& theBoxIWantToFitInside ) const
	{
		return theBoxIWantToFitInside.calcLargestCenteredInteriorAABB2DOfGivenAspectRatio( calcWidth() / calcHeight() );
	}

	inline AABB2D AABB2D::calcOverlapWith( const AABB2D& comparisonBox ) const
	{
		if( isOverlappingWith( comparisonBox ) )
		{
		float minx = ( m_vMin.x > comparisonBox.m_vMin.x? m_vMin.x : comparisonBox.m_vMin.x);
		float miny = ( m_vMin.y > comparisonBox.m_vMin.y? m_vMin.y : comparisonBox.m_vMin.y);
		float maxx = ( m_vMax.x > comparisonBox.m_vMax.x? m_vMax.x : comparisonBox.m_vMax.x);
		float maxy = ( m_vMax.y > comparisonBox.m_vMax.y? m_vMax.y : comparisonBox.m_vMax.y);

		return AABB2D( Vector2f(minx, miny), Vector2f(maxx, maxy) );
		}
		else
			return AABB2D( Vector2f(0, 0), Vector2f(0, 0) );
	}

	inline float AABB2D::calcDistanceSquaredToPoint( const Vector2f& point ) const
	{
		float xdiff;
		float ydiff;;

		if( point.x < m_vMin.x )
			xdiff = m_vMin.x - point.x;
		else if( point.x > m_vMax.x )
			xdiff = point.x - m_vMax.x;
		else
			xdiff = 0.0f;

		if( point.y < m_vMin.y )
			ydiff = m_vMin.y - point.y;
		else if( point.y > m_vMax.y )
			ydiff = point.y - m_vMax.y;
		else
			ydiff = 0.0f;

		return xdiff * xdiff + ydiff * ydiff;
	}

	inline Vector2f& AABB2D::clampPointToBounds( Vector2f& point ) const
	{
		if( point.x < m_vMin.x )
			point.x = m_vMin.x;

		if( point.y < m_vMin.y )
			point.y = m_vMin.y;

		if( point.x > m_vMax.x )
			point.x = m_vMax.x;

		if( point.y > m_vMax.y )
			point.y = m_vMax.y;

		return point;
	}

	
	inline AABB2D AABB2D::clampBoxToBounds( const AABB2D& boxToClamp ) const
	{
		Vector2f center = calcCenter();
		Vector2f boxToClampCenter = boxToClamp.calcCenter();
		
		//float slope = calcHeight() / calcWidth();

		float xmindiff = abs( m_vMin.x - boxToClamp.m_vMax.x );
		float xmaxdiff = abs( m_vMax.x - boxToClamp.m_vMin.x );

		float ymindiff = abs( m_vMin.y - boxToClamp.m_vMax.y );
		float ymaxdiff = abs( m_vMax.y - boxToClamp.m_vMin.y );

		
		float xdiff = (xmindiff < xmaxdiff? xmindiff: xmaxdiff);
		float ydiff = (ymindiff < ymaxdiff? ymindiff: ymaxdiff);
		
		float minx, miny;
		
		if( xdiff < ydiff )
		{
			if( boxToClampCenter.x < center.x )
				minx = m_vMin.x - boxToClamp.calcWidth();
			else
				minx = m_vMax.x;

			miny = boxToClamp.m_vMin.y;

		}

		if( xdiff > ydiff )
		{
			if( boxToClampCenter.y < center.y )
				miny = m_vMin.y - boxToClamp.calcHeight();
			else
				miny = m_vMax.y;

			minx = boxToClamp.m_vMin.x;

		}

		if( xdiff == ydiff )
		{
			if( boxToClampCenter.x < center.x )
				minx = m_vMin.x - boxToClamp.calcWidth();
			else
				minx = m_vMax.x;

			if( boxToClampCenter.y < center.y )
				miny = m_vMin.y - boxToClamp.calcHeight();
			else
				miny = m_vMax.y;

		}
		return AABB2D( Vector2f(minx, miny), Vector2f(minx + boxToClamp.calcWidth(), miny + boxToClamp.calcHeight()) );



	}

	//AABB2D AABB2D::clampBoxToBounds( const AABB2D& boxToClamp ) const
	//{
	//	

	//}

	inline AABB2D AABB2D::calcLargestCenteredInteriorAABB2DOfGivenAspectRatio( float aspect ) const
	{
		assert( aspect > 0 );

		//returns a box with the same center as this
		if( aspect > 0 )
		{
			float xratio = calcWidth() / aspect;
			float yratio = calcHeight();
			float ratio = ( xratio < yratio? xratio : yratio ) * .5f;
			Vector2f center = calcCenter();
			Vector2f delta = Vector2f( ratio * aspect, ratio );

			return AABB2D( center - delta, center + delta );

		}
		return NULL_AABB;
	}

	inline AABB2D& AABB2D::operator+=( const Vector2f& Translation )
	{
		m_vMin += Translation;
		m_vMax += Translation;
		return *this;
	}

	inline AABB2D& AABB2D::operator-=( const Vector2f& antiTranslation )
	{
		m_vMin -= antiTranslation;
		m_vMax -= antiTranslation;
		return *this;
	}

	inline AABB2D& AABB2D::operator*=( float UniformScale )
	{
		return ScaleUniform( UniformScale );
	}

	inline AABB2D& AABB2D::operator*=( const Vector2f& nonUniformScale )
	{
		return ScaleNonUniform( nonUniformScale );
	}

	
	inline AABB2D AABB2D::operator-( const Vector2f& antiTranslation ) const
	{
		Vector2f min = m_vMin - antiTranslation;
		Vector2f max = m_vMax - antiTranslation;

		return AABB2D( min, max );
	}

	inline AABB2D AABB2D::operator+( const Vector2f& Translation ) const
	{
		Vector2f min = m_vMin + Translation;
		Vector2f max = m_vMax + Translation;

		return AABB2D( min, max );
	}

	inline AABB2D AABB2D::operator*( float uniformScale ) const
	{
		 
		float realscale = uniformScale *.25f;
		Vector2f min = m_vMin - Vector2f(calcWidth(), calcHeight() ) * (realscale);
		Vector2f max = m_vMax + Vector2f(calcWidth(), calcHeight() ) * (realscale);

		return AABB2D( min, max );
	}

	inline AABB2D AABB2D::operator*( const Vector2f& nonUniformScale ) const
	{

		Vector2f center = calcCenter(); 
		float dx = calcWidth() * nonUniformScale.x * .5f;
		float dy = calcHeight() * nonUniformScale.y * .5f;
		Vector2f delta = Vector2f(dx, dy);

		Vector2f min = center - delta;
		Vector2f max = center + delta;

		return AABB2D( min, max );
	}

	inline bool AABB2D::operator==( const AABB2D& rhs ) const
	{
		return ( m_vMin == rhs.m_vMin && m_vMax == rhs.m_vMax );
	}
	
	inline bool AABB2D::operator!=( const AABB2D& rhs ) const
	{
		return !( *this == rhs );
	}




}
