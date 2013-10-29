#pragma once

#include "UIWidget.hpp"

#include <vector>

namespace woodman
{
	enum VectorField
	{
		VECTORFIELD_X,
		VECTORFIELD_Y,
		VECTORFIELD_Z,
		VECTORFIELD_W,
		VECTORFIELD_0,
		VECTORFIELD_1,
		VECTORFIELD_NULL,
		VECTORFIELD_SIZE
	};

	struct VectorMap
	{
		int inSize;
		int outSize;
		std::vector<VectorField> mapping;
	};

	class UIVectorMap : public UIWidget
	{
	public:
		UIVectorMap();


	private:
		VectorMap m_mapping;
	};
}