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
	class UINodeBox;

	struct MouseMenu
	{
		MouseMenu() : open(false) {}

		std::string name;
		std::vector< std::unique_ptr<MouseMenu> > subMenus;
		std::string EventToFire;
		NamedPropertyContainer Parameters;
		bool open;
	};

	struct UIMouse
	{

		UIMouse()
			:menuOpen(false),
			isPressed(false)
		{

		}


		void render();
		bool clickMenu(  MouseMenu* menu, const Vector2f& menuPosition, EventSystem* eventSystem);
		void renderMenu( MouseMenu* menu, const Vector2f& menuPosition,  std::shared_ptr<UIStyle> style );

		Vector2f screenPosition;
		Vector2f prevScreenPosition;
		Vector2f prevClickPosition;
		Vector2f prevRClickPosition;
		Vector2f prevReleasePosition;

		std::weak_ptr<UIWidget> selectedWidget;
		std::weak_ptr<UINodeBox> selectedNodeBox;
		std::weak_ptr<UIWidget> hoveringWidget;

		std::weak_ptr<UICanvas> selectedCanvas;
		std::weak_ptr<UICanvas> hoveringCanvas;

		//where in relation the last Click was to the selected widgets coordinates (canvas space)
		Vector2f relativeOriginalPosition;

		bool menuOpen;
		bool isPressed;


		std::shared_ptr<Shader> mouseMenuShader;
		std::unique_ptr<MouseMenu> MainMenu;
	
	};
}