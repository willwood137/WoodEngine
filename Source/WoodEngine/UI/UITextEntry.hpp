#pragma once

#include "UIWidget.hpp"
#include "..\Engine\EventSystem.hpp"

namespace woodman
{
	class UITextEntry : public UIWidget, public EventRecipient<UITextEntry>
	{
	public:

		UITextEntry(UICanvas* ParentCanvas,
			UIWidget* parentWidget,
			const std::string& name,
			HashedString uniqueID,
			float RelativeLayer,
			EventSystem* pEventSystem);

		virtual void render( UIMouse* currentMouse, float ParentLayer );
		virtual void update( UIMouse* currentMouse);
		virtual void Initialize();
		virtual void MouseClick( UIMouse* currentMouse );
		virtual void getTopWidgetColliding(const Vector2f& PointCanvasSpace, UIWidget*& TopWidget, float ParentLayer, float CurrentLayer );

		void catchKeyDown(NamedPropertyContainer& parameters);
		void setToRender(bool renderThisFrame);
		void readValue();
		float getValue();

	private:
		std::string m_title;
		std::string m_entry;
		float m_number;
		unsigned int m_cursorPosition;
		bool m_renderThisFrame;
		bool m_selectedThisFrame;
		std::shared_ptr<Shader> m_boxShader;
	};
}