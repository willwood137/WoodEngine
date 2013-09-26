#include "..\Engine\ComponentSystem.hpp"
#include "..\Math\Vector3.hpp"

#include <map>


namespace woodman
{
	struct PhysicsData
	{
		Vector3f	position;
		Vector3f	velocity;
		Vector3f	acceleration;
		float		mass;
		
		PhysicsData() : position(0.0f, 0.0f, 0.0f),
						velocity(0.0f, 0.0f, 0.0f),
						acceleration(0.0f, 0.0f, 0.0f),
						mass(1.0f)
		{

		}
	};

	class PhysicsComponentSystem : public ComponentSystem
	{
	public:
		PhysicsComponentSystem();
		void AddNewEntity(unsigned int EntityID);
		void AddNewEntity(unsigned int EntityID, const PhysicsData& _PhysicsData);
		void AddNewEntity(unsigned int EntityID, const Vector3f& _position);

		void Update();

		~PhysicsComponentSystem();
	private:
		std::map<unsigned int, PhysicsData> m_entityData;
	};
}