#pragma once

#include <vector>
#include <string>

#include "UIWindow.hpp"
#include "..\Engine\EventSystem.hpp"

namespace shadow
{
	class UI : public EventRecipient<UI>
	{
	public:
		UI::UI(const std::string& filepath, EventSystem* eventsystem );
		void initialize();
		void render();
		void update(const Vector2f& cursorPosition);
		bool checkMouseClick(const Vector2f& cursorPosition);

	private:
		std::string m_filePath;
		std::vector<UIWindow*> m_Windows;
	};
}