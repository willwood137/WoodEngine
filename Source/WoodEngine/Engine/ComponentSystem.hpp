
#include <memory>

namespace woodman
{
	template <typename DataType>
	struct ComponentDataField
	{
		std::shared_ptr<DataType> data;
		std::string name;

	};

	class ComponentSystem
	{
		ComponentSystem();

		virtual void Update() = 0;
		virtual ~ComponentSystem() = 0;

	};
}