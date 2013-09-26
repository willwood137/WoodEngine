#include "..\stdafx.h"

#include "AttachedAABB2D.hpp"

namespace shadow
{
	AttachedAABB2D::AttachedAABB2D()
		: m_attachTo(ATTACH_TYPE_BOTTOMLEFT),
		m_attachFrom(ATTACH_TYPE_BOTTOMRIGHT),
		m_offset(.0f, .0f),
		m_size(.0f, .0f),
		m_attachmentPoint(.0f, .0f),
		m_collisionBox(Vector2f(.0f, .0f), Vector2f(1.f, 1.f))
	{

	}

	AttachedAABB2D::AttachedAABB2D( ATTACH_TYPE from, ATTACH_TYPE to, const Vector2f& offset, const Vector2f& size)
		: m_attachTo(to),
		m_attachFrom(from),
		m_offset(offset),
		m_size(size),
		m_attachmentPoint(.0f, .0f),
		m_collisionBox(Vector2f(.0f, .0f), Vector2f(1.f, 1.f))
	{

	}

	AttachedAABB2D::AttachedAABB2D( ATTACH_TYPE from, ATTACH_TYPE to, const Vector2f& offset, const Vector2f& size, const AABB2D& boxToAttachTo)
		: m_attachTo(to),
		m_attachFrom(from),
		m_offset(offset),
		m_size(size)
	{
		attachToAABB2D(boxToAttachTo);
	}

	AttachedAABB2D::AttachedAABB2D( ATTACH_TYPE from, ATTACH_TYPE to, const Vector2f& offset, const Vector2f& size, const Vector2f& pointToAttachTo)
		: m_attachTo(to),
		m_attachFrom(from),
		m_offset(offset),
		m_size(size),
		m_attachmentPoint(pointToAttachTo)
	{
		attachToAttachmentPoint();
	}

	void AttachedAABB2D::attachToAABB2D(const AABB2D& boxToAttachTo)
	{
		Vector2f attachToPoint(0.f, 0.f);

		switch( m_attachTo )
		{
		case ATTACH_TYPE_BOTTOMLEFT:
			attachToPoint = boxToAttachTo.m_vMin;
			break;
		case ATTACH_TYPE_BOTTOM:
			attachToPoint.x = (boxToAttachTo.m_vMin.x + boxToAttachTo.m_vMax.x) * .5f;
			attachToPoint.y = boxToAttachTo.m_vMin.y;
			break;
		case ATTACH_TYPE_BOTTOMRIGHT:
			attachToPoint.x = boxToAttachTo.m_vMax.x;
			attachToPoint.y = boxToAttachTo.m_vMin.y;
			break;
		case ATTACH_TYPE_RIGHT:
			attachToPoint.x = boxToAttachTo.m_vMax.x;
			attachToPoint.y = (boxToAttachTo.m_vMin.y + boxToAttachTo.m_vMax.y) * .5f;
			break;
		case ATTACH_TYPE_TOPRIGHT:
			attachToPoint = boxToAttachTo.m_vMax;
			break;
		case ATTACH_TYPE_TOP:
			attachToPoint.x = (boxToAttachTo.m_vMin.x + boxToAttachTo.m_vMax.x) * .5f;
			attachToPoint.y = boxToAttachTo.m_vMax.y;
			break;
		case ATTACH_TYPE_TOPLEFT:
			attachToPoint.x = boxToAttachTo.m_vMin.x;
			attachToPoint.y = boxToAttachTo.m_vMax.y;
			break;
		case ATTACH_TYPE_LEFT:
			attachToPoint.x = boxToAttachTo.m_vMin.x;
			attachToPoint.y = (boxToAttachTo.m_vMin.y + boxToAttachTo.m_vMax.y) * .5f;
			break;
		case ATTACH_TYPE_CENTER:
			attachToPoint.x = (boxToAttachTo.m_vMin.x + boxToAttachTo.m_vMax.x) * .5f;
			attachToPoint.y = (boxToAttachTo.m_vMin.y + boxToAttachTo.m_vMax.y) * .5f;
			break;
		}

		m_attachmentPoint = attachToPoint;
		attachToAttachmentPoint();
	}

	Vector2f getAttachmentPointFromAABB2D(const AABB2D& box, ATTACH_TYPE type)
	{

		Vector2f attachToPoint(0.f, 0.f);

		switch( type )
		{
		case ATTACH_TYPE_BOTTOMLEFT:
			attachToPoint = box.m_vMin;
			break;
		case ATTACH_TYPE_BOTTOM:
			attachToPoint.x = (box.m_vMin.x + box.m_vMax.x) * .5f;
			attachToPoint.y = box.m_vMin.y;
			break;
		case ATTACH_TYPE_BOTTOMRIGHT:
			attachToPoint.x = box.m_vMax.x;
			attachToPoint.y = box.m_vMin.y;
			break;
		case ATTACH_TYPE_RIGHT:
			attachToPoint.x = box.m_vMax.x;
			attachToPoint.y = (box.m_vMin.y + box.m_vMax.y) * .5f;
			break;
		case ATTACH_TYPE_TOPRIGHT:
			attachToPoint = box.m_vMax;
			break;
		case ATTACH_TYPE_TOP:
			attachToPoint.x = (box.m_vMin.x + box.m_vMax.x) * .5f;
			attachToPoint.y = box.m_vMax.y;
			break;
		case ATTACH_TYPE_TOPLEFT:
			attachToPoint.x = box.m_vMin.x;
			attachToPoint.y = box.m_vMax.y;
			break;
		case ATTACH_TYPE_LEFT:
			attachToPoint.x = box.m_vMin.x;
			attachToPoint.y = (box.m_vMin.y + box.m_vMax.y) * .5f;
			break;
		case ATTACH_TYPE_CENTER:
			attachToPoint.x = (box.m_vMin.x + box.m_vMax.x) * .5f;
			attachToPoint.y = (box.m_vMin.y + box.m_vMax.y) * .5f;
			break;


		}

		return attachToPoint;
	}

	void AttachedAABB2D::attachToAttachmentPoint()
	{
		Vector2f attachToPoint = m_attachmentPoint + m_offset;
		switch( m_attachFrom )
		{
		case ATTACH_TYPE_BOTTOMLEFT:
			m_collisionBox.m_vMin = attachToPoint;
			m_collisionBox.m_vMax = attachToPoint + m_size; 
			break;
		case ATTACH_TYPE_BOTTOM:
			m_collisionBox.m_vMin.x = attachToPoint.x - m_size.x * .5f;
			m_collisionBox.m_vMin.y = attachToPoint.y;
			m_collisionBox.m_vMax.x = attachToPoint.x + m_size.x * .5f;
			m_collisionBox.m_vMax.y = attachToPoint.y + m_size.y;
			break;
		case ATTACH_TYPE_BOTTOMRIGHT:
			m_collisionBox.m_vMin.x = attachToPoint.x - m_size.x;
			m_collisionBox.m_vMin.y = attachToPoint.y;
			m_collisionBox.m_vMax.x = attachToPoint.x;
			m_collisionBox.m_vMax.y = attachToPoint.y + m_size.y;
			break;
		case ATTACH_TYPE_RIGHT:
			m_collisionBox.m_vMin.x = attachToPoint.x - m_size.x;
			m_collisionBox.m_vMin.y = attachToPoint.y - m_size.y * .5f;
			m_collisionBox.m_vMax.x = attachToPoint.x;
			m_collisionBox.m_vMax.y = attachToPoint.y + m_size.y * .5f;
			break;
		case ATTACH_TYPE_TOPRIGHT:
			m_collisionBox.m_vMin = attachToPoint - m_size;
			m_collisionBox.m_vMax = attachToPoint; 
			break;
		case ATTACH_TYPE_TOP:
			m_collisionBox.m_vMin.x = attachToPoint.x - m_size.x * .5f;
			m_collisionBox.m_vMin.y = attachToPoint.y - m_size.y;
			m_collisionBox.m_vMax.x = attachToPoint.x + m_size.x * .5f;
			m_collisionBox.m_vMax.y = attachToPoint.y;
			break;
		case ATTACH_TYPE_TOPLEFT:
			m_collisionBox.m_vMin.x = attachToPoint.x ;
			m_collisionBox.m_vMin.y = attachToPoint.y - m_size.y;
			m_collisionBox.m_vMax.x = attachToPoint.x + m_size.x;
			m_collisionBox.m_vMax.y = attachToPoint.y;
			break;
		case ATTACH_TYPE_LEFT:
			m_collisionBox.m_vMin.x = attachToPoint.x;
			m_collisionBox.m_vMin.y = attachToPoint.y - m_size.y * .5f;
			m_collisionBox.m_vMax.x = attachToPoint.x + m_size.x;
			m_collisionBox.m_vMax.y = attachToPoint.y + m_size.y * .5f;
			break;
		case ATTACH_TYPE_CENTER:
			m_collisionBox.m_vMin.x = attachToPoint.x - m_size.x * .5f;
			m_collisionBox.m_vMin.y = attachToPoint.y - m_size.y * .5f;
			m_collisionBox.m_vMax.x = attachToPoint.x + m_size.x * .5f;
			m_collisionBox.m_vMax.y = attachToPoint.y + m_size.y * .5f;
			break;
		}
	}
}