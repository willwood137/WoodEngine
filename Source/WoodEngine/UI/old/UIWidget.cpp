#include "..\stdafx.h"

#include "UIWidget.hpp"

namespace shadow
{
	UIWidget::UIWidget(const std::string& name)
		: m_name(name)
	{

	}

	UIWidget::~UIWidget()
	{

	}

	void UIWidget::render()
	{

	}

	void UIWidget::parseXMLNode( pugi::xml_node& node )
	{

	}

	void UIWidget::initialize(const AABB2D& parentBox, const Vector2f& relativeOffset )
	{

	}

	void UIWidget::setSize(const Vector2f& size)
	{
		m_size = size;
	}


	void UIWidget::update(const Vector2f& cursorPosition)
	{
		m_cursorPosition = cursorPosition;
	}

	bool UIWidget::checkMouseClick(const Vector2f& cursorPosition)
	{
		bool collision = m_containedBox.getBox().isPointInsideBounds(cursorPosition);
		return collision;
	}

}