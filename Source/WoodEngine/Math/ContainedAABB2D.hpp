#pragma once

#include "AttachedAABB2D.hpp"

namespace shadow
{
	class ContainedAABB2D
	{
	public:
		ContainedAABB2D();
		ContainedAABB2D(const AABB2D& parentBox, const Vector2f& minOffset, const Vector2f& maxOffset, ATTACH_TYPE minType, ATTACH_TYPE maxType );

	
		void setAttachmentBox( const AABB2D& parentBox );
		void setMinOffset( const Vector2f& offset );
		void setMaxOffset( const Vector2f& offset );
		void setMinAttachType( ATTACH_TYPE type );
		void setMaxAttachType( ATTACH_TYPE type );

		AABB2D& getBox();

	private:

		void AttachMin();
		void AttachMax();

		AABB2D m_box;  // auto-generated

		AABB2D m_boxToAttachTo;
		Vector2f m_vMinOffset;
		Vector2f m_vMaxOffset;
		ATTACH_TYPE m_minAttachType;
		ATTACH_TYPE m_maxAttachType;

	};

	inline ContainedAABB2D::ContainedAABB2D()
		: m_boxToAttachTo(AABB2D(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f) ) ),
		m_vMinOffset(Vector2f(0.0f, 0.0f)),
		m_vMaxOffset(Vector2f(0.0f, 0.0f)),
		m_minAttachType(ATTACH_TYPE_TOPLEFT),
		m_maxAttachType(ATTACH_TYPE_TOPLEFT)
	{

	}
	
	inline ContainedAABB2D::ContainedAABB2D(const AABB2D& parentBox, const Vector2f& minOffset, const Vector2f& maxOffset, ATTACH_TYPE minType, ATTACH_TYPE maxType )
		: m_boxToAttachTo(parentBox),
		m_vMinOffset(minOffset),
		m_vMaxOffset(maxOffset),
		m_minAttachType(minType),
		m_maxAttachType(maxType)
	{
		AttachMin();
		AttachMax();
	}

	inline void ContainedAABB2D::setAttachmentBox( const AABB2D& parentBox )
	{
		m_boxToAttachTo = parentBox;
		AttachMin();
		AttachMax();
	}
	inline void ContainedAABB2D::setMinOffset( const Vector2f& offset )
	{
		m_vMinOffset = offset;
		AttachMin();
	}

	inline void ContainedAABB2D::setMaxOffset( const Vector2f& offset )
	{
		m_vMaxOffset = offset;
		AttachMax();
	}
	inline void ContainedAABB2D::setMinAttachType( ATTACH_TYPE type )
	{
		m_minAttachType = type;
		AttachMin();
	}
	inline void ContainedAABB2D::setMaxAttachType( ATTACH_TYPE type )
	{
		m_maxAttachType = type;
		AttachMax();
	}


	inline void ContainedAABB2D::AttachMin()
	{
		m_box.m_vMin = getAttachmentPointFromAABB2D(m_boxToAttachTo, m_minAttachType) + m_vMinOffset;
	}

	inline void ContainedAABB2D::AttachMax()
	{
		m_box.m_vMax = getAttachmentPointFromAABB2D(m_boxToAttachTo, m_maxAttachType) + m_vMaxOffset;
	}


	inline AABB2D& ContainedAABB2D::getBox()
	{
		return m_box;
	}
}