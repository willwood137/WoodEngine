#pragma once

#include "UICanvas.hpp"
#include "UIWidget.hpp"
#include "UIMouse.hpp"

#include "../Engine/EventSystem.hpp"

namespace woodman
{
	class UIController : public EventRecipient<UIController> 
	{
	public:

		UIController(EventSystem* eventsystem);

		virtual void Initialize();
		virtual void update(const Vector2f& MouseScreenPosition );
		virtual void render();


		virtual UIWidget* getUIWidgetatPoint(const Vector2f& screenPosition);
		virtual UICanvas* getUICanvasAtPoint(const Vector2f& screenPosition);

		virtual void KeyDown(unsigned int key);

		UIWidget* getUIWidgetByID(HashedString ID);

		void catchKey(NamedPropertyContainer& parameters);
		void catchLMouseDown(NamedPropertyContainer& parameters);
		void catchLMouseUp(NamedPropertyContainer& parameters);
		void catchRMouseDown(NamedPropertyContainer& parameters);

	protected:

		std::set<std::unique_ptr<UICanvas> > m_canvases;
		std::unique_ptr<UIMouse> m_mouse;
	};
}