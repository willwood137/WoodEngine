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

		virtual std::weak_ptr<UIWidget> getUIWidgetatPoint(const Vector2f& screenPosition);
		virtual std::weak_ptr<UICanvas> getUICanvasAtPoint(const Vector2f& screenPosition);

		void getWidgetByID(HashedString ID);
		void RegisterUIWidget( std::shared_ptr<UIWidget> widget );
		void UnRegisterUIWidget( HashedString ID );

		virtual void KeyDown(unsigned int key);

		std::weak_ptr<UIWidget> getUIWidgetByID(HashedString ID);

		void catchKey(NamedPropertyContainer& parameters);
		void catchLMouseDown(NamedPropertyContainer& parameters);
		void catchLMouseUp(NamedPropertyContainer& parameters);
		void catchRMouseDown(NamedPropertyContainer& parameters);
		HashedString getUniqueID(HashedString& ID);

	protected:

		std::map<HashedString, std::shared_ptr<UIWidget> > m_widgets;
		std::list<std::shared_ptr<UICanvas> > m_canvases;
		std::shared_ptr<UIMouse> m_mouse;
	};
}