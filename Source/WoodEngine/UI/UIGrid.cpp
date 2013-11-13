#include "..\stdafx.h"

#include "UIGrid.hpp"

namespace woodman
{
	
	UIGrid::UIGrid( UICanvas* ParentCanvas, 
		UIWidget* parentWidget, 
		const std::string& name, 
		float RelativeLayer, 
		const Vector2f& relativeCoordinates )
		: UIWidget(ParentCanvas, parentWidget, name, RelativeLayer, relativeCoordinates),
		m_maxRows(0),
		m_maxColumns(1),
		m_visibleColumns(1),
		m_visibleRows(0),
		m_fillDirection(DIRECTION_RIGHT),
		m_baseBlock(CORNER_TOP_LEFT)
	{

	}

	void UIGrid::updateData( UIMouse* currentMouse, const ParentInfo& parent_info)
	{
		UIWidget::updateData(currentMouse, parent_info );

		// set Relative positions of the widget containers in the grid
		//
		Vector2f gridPosition(0.0f, 0.0f);
		
		float maxWidth = 0.0f;


		//set relative Positions
		size_t numBlocks = m_blocks.size();
		for(size_t i = 0; i < numBlocks; ++i)
		{
			//TODO implement more powerful grid capabilities
			//

// 			size_t primaryBlockLimit, secondaryBlockLimit;
// 
// 
// 			if(m_fillDirection == DIRECTION_RIGHT || DIRECTION_LEFT)
// 			{
// 				primaryBlockLimit = m_maxColumns; 
// 				secondaryBlockLimit = m_maxRows;
// 			}


			// Warning : only supports 1 column

			m_blocks[i]->setRelativeCoordinates(gridPosition);
			Vector2f tempSize = m_blocks[i]->getSize();
			maxWidth = max(tempSize.x, maxWidth);
			gridPosition.y += tempSize.y;
		}

		//set width and sizes
		for(size_t i = 0; i < numBlocks; ++i)
		{
			Vector2f tempSize = m_blocks[i]->getSize();
			m_blocks[i]->setSize(Vector2f(maxWidth, tempSize.y));
		}
	}

	void UIGrid::addChild( UIWidget* child )
	{
		UIWidgetContainer* container = dynamic_cast<UIWidgetContainer*>(child);

		if(container == nullptr)
		{
			// If its not a container already, put the widget in a container
			//
			container = new UIWidgetContainer( getParentCanvas(), this, child->getName() + "_Container", 0.0, Vector2f(0.0f, 0.0f));
			container->addChild(child);
		}

		UIWidget::addChild(container);
		m_blocks.emplace_back(container);
	}

	void UIGrid::removeChild( UIWidget* child )
	{
		// TODO - IMPLEMENT
		//
		assert(false);
	}

	void UIGrid::Initialize()
	{

	}

}