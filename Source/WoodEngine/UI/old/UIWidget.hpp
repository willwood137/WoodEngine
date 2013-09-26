#pragma once

#include <string>
#include "pugixml.hpp"
#include "..\Math\ContainedAABB2D.hpp"
#include "..\Math\AABB2D.hpp"
#include "..\Math\Vector2.hpp"
#include "..\Engine\Shader.hpp"

namespace shadow
{
	class UIWidget
	{
	public:
		UIWidget(const std::string& name);

		virtual ~UIWidget();

		virtual void initialize(const AABB2D& parentBox, const Vector2f& relativeOffset );
		virtual bool checkMouseClick(const Vector2f& cursorPosition);
		virtual void render();
		virtual void UIWidget::parseXMLNode( pugi::xml_node& node );
		virtual void setSize(const Vector2f& size);
		virtual void update(const Vector2f& cursorPosition);

	protected:
		std::string m_name;
		ContainedAABB2D m_containedBox;
		Vector2f m_size;
		std::vector<UIWidget*> m_children;
		Shader m_shader;
		Vector2f m_cursorPosition;
	};
}