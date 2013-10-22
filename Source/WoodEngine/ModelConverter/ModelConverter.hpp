#pragma once

#include "ModelConverterNode.hpp"
#include "ModelData.hpp"

#include "..\UI\UIController.hpp"

namespace woodman
{
	class ModelConverter : public UIController
	{
	public:

	private:

		UICanvas* m_previewCanvas;
		UICanvas* m_workCanvas;
	};
}