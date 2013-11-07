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

	RGBA getVectorFieldColor(VectorField vField);

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

		virtual void render( UIMouse* currentMouse, float ParentLayer );
		virtual void Initialize();

		void updateInSize(unsigned int size);
		void updateExitSize(unsigned int size);
		void cycleMapValue(unsigned int MapIndex, bool CycleForward );
		
		int getExitSize() { return m_mapping.outSize;}

	private:
		VectorMap m_mapping;
		std::shared_ptr<Shader> m_boxShader;
	};
}