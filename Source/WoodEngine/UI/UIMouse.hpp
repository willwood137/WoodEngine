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
		std::vector< std::shared_ptr<MouseMenu> > subMenus;
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
		bool clickMenu( std::shared_ptr<MouseMenu> menu, const Vector2f& menuPosition, EventSystem* eventSystem);
		void renderMenu(std::shared_ptr<MouseMenu> menu, const Vector2f& menuPosition,  std::shared_ptr<UIStyle> style );

		Vector2f screenPosition;
		Vector2f prevScreenPosition;
		Vector2f prevClickPosition;
		Vector2f prevRClickPosition;
		Vector2f prevReleasePosition;

		std::shared_ptr<UIWidget> selectedWidget;
		std::shared_ptr<UIWidget> hoveringWidget;

		std::shared_ptr<UICanvas> selectedCanvas;

		//where in relation the last Click was to the selected widgets coordinates (canvas space)
		Vector2f relativeOriginalPosition;

		bool menuOpen;
		bool isPressed;


		std::shared_ptr<Shader> mouseMenuShader;
		std::shared_ptr<MouseMenu> MainMenu;
	
	};
}