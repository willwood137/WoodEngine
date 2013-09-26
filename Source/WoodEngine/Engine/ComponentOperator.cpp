#include "..\stdafx.h"
#include "ComponentOperator.hpp"


namespace woodman
{
	void ComponentOperator::RegisterComponentSystem(std::shared_ptr< ComponentSystem> System)
	{
		m_componentSystems.insert(System);
	}

	void ComponentOperator::UnRegisterComponentSystem(std::shared_ptr< ComponentSystem > System)
	{
		m_componentSystems.erase(System);
	}
}
