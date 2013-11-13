#ifndef UIGRID_H
#define UIGRID_H

#pragma once

#include "UIWidget.hpp"
#include "UIWidgetContainer.hpp"


namespace woodman
{
	class UIGrid : public UIWidget
	{
	public:
		UIGrid( UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			float RelativeLayer,
			const Vector2f& relativeCoordinates );

		virtual void Initialize();

		virtual void updateData(UIMouse* currentMouse, const ParentInfo& parent_info);

		virtual void addChild(UIWidget* child);
		virtual void removeChild(UIWidget* child);


	private:

		unsigned int m_maxColumns;
		unsigned int m_maxRows;
		unsigned int m_visibleColumns;
		unsigned int m_visibleRows;

		Direction m_fillDirection;
		Corner m_baseBlock;

		std::vector<UIWidgetContainer*> m_blocks;
	};
}


#endif