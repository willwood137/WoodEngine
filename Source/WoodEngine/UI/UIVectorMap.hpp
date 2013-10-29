#pragma once

#include "UIWidget.hpp"

#include <vector>

namespace woodman
{
	enum VectorField
	{
		VECTORFIELD_X = 1,
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
		UIVectorMap(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID,
			float RelativeLayer);

		void updateInSize(unsigned int i);
		void updateExitSize(unsigned int i);
		void cycleMapValue(unsigned int MapIndex, bool CycleForward );
		

	private:
		VectorMap m_mapping;
	};
}