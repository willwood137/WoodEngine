#include "..\stdafx.h"
#include "PhysicsComponentSystem.hpp"

namespace woodman
{
	void PhysicsComponentSystem::Update()
	{
		for( auto it = m_entityData.begin(); it != m_entityData.end(); ++it)
		{
			// To do make this used with entity clock component?
			float updateTime = .016f;
			it->second.velocity *= .9f;
			it->second.velocity += it->second.acceleration * updateTime;
			it->second.position += it->second.velocity * updateTime;
			it->second.acceleration = 0;
		}
	}

	void PhysicsComponentSystem::AddNewEntity(unsigned int EntityID)
	{
		if( m_entityData.find(EntityID) == m_entityData.end())
		{
			m_entityData.insert( std::make_pair(EntityID, PhysicsData()) );
		}
	}

	void PhysicsComponentSystem::AddNewEntity(unsigned int EntityID, const Vector3f& _position)
	{
		if( m_entityData.find(EntityID) == m_entityData.end())
		{
			m_entityData.insert( std::make_pair(EntityID, PhysicsData()) );
			m_entityData[EntityID].position = _position;
		}
	}

	void PhysicsComponentSystem::AddNewEntity(unsigned int EntityID, const PhysicsData& _PhysicsData)
	{
		if( m_entityData.find(EntityID) == m_entityData.end())
		{
			m_entityData.insert( std::make_pair(EntityID, _PhysicsData) );
		}
	}
}