#pragma once

#include "Vector3.hpp"

namespace shadow
{

	class Ray
	{

	public:
		Vector3f m_origin;
		Vector3f m_direction;
	
		Ray( Vector3f origin, Vector3f direction, float length = 0) : m_origin(origin), m_direction(direction) {}
		Ray() {}
		
		Ray& lookAt( const Vector3f& target);

	};

}