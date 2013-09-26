#include "..\Engine\ComponentSystem.hpp"

#include <map>

namespace woodman
{
	struct RenderData
	{

	};

	class RenderComponentSystem : public ComponentSystem
	{
	public:
		~RenderComponentSystem();
	private:
		std::map<unsigned int, RenderData> m_entityData;
	};
}