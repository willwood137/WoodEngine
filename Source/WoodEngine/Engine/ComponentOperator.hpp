#include "..\Math\Vector3.hpp"
#include "ComponentSystem.hpp"

#include <set>
#include <memory>

namespace woodman
{

	//class ComponentOperator allows component systems to talk to eachother without having to 
	// know about the other components
	class ComponentOperator
	{

	public:
		
		/*** Regusters a Component System to the Operator ***/
		void RegisterComponentSystem( std::shared_ptr< ComponentSystem> System);

		void UnRegisterComponentSystem( std::shared_ptr< ComponentSystem> System );

		std::shared_ptr<float> getFloatData(unsigned int entityID, const std::string& DataName);

		/*** Get the pointer to a data of type Vector3 that matches the name DataName for entityID ***/
		std::shared_ptr<Vector3f> getVector3Data(unsigned int entityID, const std::string& DataName);

	private:

		std::set<std::shared_ptr< ComponentSystem > > m_componentSystems;

	};
}