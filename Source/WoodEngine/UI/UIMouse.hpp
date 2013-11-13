#pragma once


#include "UIStyle.hpp"

#include "../Math/Vector2.hpp"

#include "../Engine/Shader.hpp"
#include "../Engine/EventSystem.hpp"

#include <memory>
#include <map>
#include <vector>

namespace woodman
{

	class UIWidget;
	class UICanvas;

	struct MouseMenu
	{
		MouseMenu() : open(false) {}

		std::string name;
		std::vector< std::unique_ptr<MouseMenu> > subMenus;
		std::string EventToFire;
		NamedPropertyContainer Parameters;
		bool open;
	};

	enum MOUSE_EVENT_TYPE
	{
		MOUSE_EVENT_LEFT_CLICK = 0,
		MOUSE_EVENT_LEFT_RELEASE,
		MOUSE_EVENT_LEFT_DRAG,
		MOUSE_EVENT_MIDDLE_CLICK,
		MOUSE_EVENT_MIDDLE_RELEASE,
		MOUSE_EVENT_MIDDLE_DRAG,
		MOUSE_EVENT_RIGHT_CLICK,
		MOUSE_EVENT_RIGHT_RELEASE,
		MOUSE_EVENT_RIGHT_DRAG,
		MOUSE_EVENT_SIZE
	};

	struct MouseEventData
	{
		UIWidget*	widget;
		Vector2f	relativeWidgetCoordinates;
		Vector2f	screenCoordinates;
		Vector2f	prevScreenCoordinates;
	};

	struct UIMouse
	{

		UIMouse();

		UIWidget* hoveringWidget;
		UICanvas* hoveringCanvas;
		UICanvas* selectedCanvas;

		void render();
		bool clickMenu(  MouseMenu* menu, const Vector2f& menuPosition, EventSystem* eventSystem);
		void renderMenu( MouseMenu* menu, const Vector2f& menuPosition,  std::shared_ptr<UIStyle> style );

		Vector2f screenPosition;
		Vector2f prevScreenPosition;

		std::array<MouseEventData, MOUSE_EVENT_SIZE> EventData;		

		bool menuOpen;
		bool isPressed;

		std::shared_ptr<Shader> mouseMenuShader;
		std::unique_ptr<MouseMenu> MainMenu;
	
	};
}