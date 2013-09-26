#ifndef ENTITYOPERATE_H
#define ENTITYOPERATE_H

#include "HashedString.hpp"

#include <vector>

namespace woodman
{
	struct NamedEntityPropertyBase
	{
		virtual ~NamedEntityPropertyBase() {}
	};

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

	template <typename T_ObjType>
	struct NamedEntityProperty : public NamedEntityPropertyBase
	{
		NamedEntityProperty( T_ObjType data) : m_data(data) {}
		T_ObjType* m_data;
	};

	class EntityOpertator
	{


	private:
		std::vector<
	};

}




#endif