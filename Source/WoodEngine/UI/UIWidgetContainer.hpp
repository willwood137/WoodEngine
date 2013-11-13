#ifndef UIWIDGETCONTAINER_H
#define UIWIDGETCONTAINER_H

#pragma once

#include "UIWidget.hpp"
#include "..\Engine\Shader.hpp"

namespace woodman
{
	class UIWidgetContainer : public UIWidget
	{
	public:
		UIWidgetContainer( UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			float RelativeLayer,
			const Vector2f& relativeCoordinates );


		virtual void render(UIMouse* currentMouse, const AABB2D& CanvasBounds) const;
		virtual void Initialize();


	private:

		Vector2f m_containerSize;
		std::shared_ptr<Shader> m_boxShader;
	};
}


#endif