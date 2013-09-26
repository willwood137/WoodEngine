#pragma once

#include "AABB2D.hpp"

namespace shadow
{
	enum ATTACH_TYPE
	{
		ATTACH_TYPE_FREE = 0,
		ATTACH_TYPE_TOP,
		ATTACH_TYPE_TOPLEFT,
		ATTACH_TYPE_TOPRIGHT,
		ATTACH_TYPE_BOTTOM,
		ATTACH_TYPE_BOTTOMLEFT,
		ATTACH_TYPE_BOTTOMRIGHT,
		ATTACH_TYPE_LEFT,
		ATTACH_TYPE_RIGHT,
		ATTACH_TYPE_CENTER,
	};

	inline ATTACH_TYPE getAttachTypeFromString(const std::string& type)
	{
		if(!_stricmp(type.c_str(), "BottomLeft"))
		{
			return ATTACH_TYPE_BOTTOMLEFT;
		}
		else if(!_stricmp(type.c_str(), "Bottom"))
		{
			return ATTACH_TYPE_BOTTOM;
		}
		else if(!_stricmp(type.c_str(), "BottomRight"))
		{
			return ATTACH_TYPE_BOTTOMRIGHT;
		}
		else if(!_stricmp(type.c_str(), "Right"))
		{
			return ATTACH_TYPE_RIGHT;
		}
		else if(!_stricmp(type.c_str(), "TopRight"))
		{
			return ATTACH_TYPE_TOPRIGHT;
		}
		else if(!_stricmp(type.c_str(), "Top"))
		{
			return ATTACH_TYPE_TOP;
		}
		else if(!_stricmp(type.c_str(), "TopLeft"))
		{
			return ATTACH_TYPE_TOPLEFT;
		}
		else if(!_stricmp(type.c_str(), "Left"))
		{
			return ATTACH_TYPE_LEFT;
		}
		else if(!_stricmp(type.c_str(), "Center"))
		{
			return ATTACH_TYPE_CENTER;
		}
		else if(!_stricmp(type.c_str(), "Free"))
		{
			return ATTACH_TYPE_FREE;
		}
		return ATTACH_TYPE_LEFT;
	}

	Vector2f getAttachmentPointFromAABB2D(const AABB2D& box, ATTACH_TYPE type);


	class AttachedAABB2D
	{
	public:

		AttachedAABB2D();
		AttachedAABB2D( ATTACH_TYPE from, ATTACH_TYPE to, const Vector2f& offset, const Vector2f& size);
		AttachedAABB2D( ATTACH_TYPE from, ATTACH_TYPE to, const Vector2f& offset, const Vector2f& size, const AABB2D& boxToAttachTo );
		AttachedAABB2D( ATTACH_TYPE from, ATTACH_TYPE to, const Vector2f& offset, const Vector2f& size, const Vector2f& pointToAttachTo);

		void attachToAABB2D(const AABB2D& boxToAttachTo);
		void attachToPoint(const Vector2f& pointToAttachTo);
		
		void setSize(const Vector2f& size);
		Vector2f& getSize();
		
		void setOffset( Vector2f offset );
		Vector2f& getOffset();

		void setAttachToType( ATTACH_TYPE type, const AABB2D& boxToAttachTo);
		ATTACH_TYPE getAttachToType();

		void setAttachFromType( ATTACH_TYPE type);
		ATTACH_TYPE getAttachFromType();

		AABB2D& getAABB2D();

	private:
		void attachToAttachmentPoint();

		ATTACH_TYPE m_attachTo;
		ATTACH_TYPE m_attachFrom;
		Vector2f m_offset;
		Vector2f m_size;

		// auto-generated
		AABB2D m_collisionBox;
		Vector2f m_attachmentPoint;
	};

	// SIZE ----------------------------------------------------------------------------------------------------------------

	inline void AttachedAABB2D::setSize(const Vector2f& size)
	{
		m_size = size;
		attachToAttachmentPoint();
	}
	inline Vector2f& AttachedAABB2D::getSize()
	{
		return m_size;
	}
	 
	inline void AttachedAABB2D::setOffset( Vector2f offset )
	{
		m_offset = offset;
		attachToAttachmentPoint();
	}
	
	inline Vector2f& AttachedAABB2D::getOffset()
	{
		return m_offset;
	}
	 
	inline void AttachedAABB2D::setAttachToType( ATTACH_TYPE type, const AABB2D& boxToAttachTo)
	{
		m_attachTo = type;
		attachToAABB2D(boxToAttachTo);
	}
	inline ATTACH_TYPE AttachedAABB2D::getAttachToType()
	{
		return m_attachTo;
	}
	 
	inline void AttachedAABB2D::setAttachFromType( ATTACH_TYPE type)
	{
		m_attachFrom = type;
		attachToAttachmentPoint();
	}

	inline ATTACH_TYPE AttachedAABB2D::getAttachFromType()
	{
		return m_attachFrom;
	}

	inline AABB2D& AttachedAABB2D::getAABB2D()
	{
		return m_collisionBox;
	}
	
}