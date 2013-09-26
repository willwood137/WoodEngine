#pragma once

#include "UIWindow.hpp"
#include "UIButton.hpp"
#include "..\Engine\Texture.hpp"

namespace shadow
{

	enum DIRECTION
	{
		DIRECTION_RIGHT = 0,
		DIRECTION_UP,
		DIRECTION_DOWN,
		DIRECTION_LEFT,
		DIRECTION_SIZE
	};

	class UIButtonBar : public UIWindow
	{
	public:
		UIButtonBar(const std::string& name, EventSystem* eventsystem);
		virtual UIWindow* parseWindow(pugi::xml_node& node);
		virtual void initialize(const AABB2D& parentBox );

	protected:
		DIRECTION m_direction;
		ATTACH_TYPE m_startButton;
		int m_maxHeight;
		int m_maxWidth;
		Vector2f m_buttonSize;
		Vector2f m_buttonSpacing;
		std::vector<UIButton*> m_buttons;
		UIButton::UIButtonGroup* m_pButtonGroup;
	};
}