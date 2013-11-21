#pragma once

#include "UIWidget.hpp"
#include "..\Engine\EventSystem.hpp"

namespace woodman
{
	class UITextEntry : public UIWidget, public EventRecipient<UITextEntry>
	{
	public:

		
		static std::weak_ptr<UITextEntry> CreateUITextEntry(const std::string& _Title,
			EventSystem* pEventSystem, 
			UIController* _ParentController,
			std::weak_ptr<UICanvas> _ParentCanvas,
			std::weak_ptr<UIWidget> _ParentWidget,
			const HashedString& _ID,
			float _RelativeLayer,
			const Vector2f& _RelativeCoordinates,
			const Vector2f& _CollisionSize );

		virtual void render( std::shared_ptr<UIMouse> currentMouse, float ParentLayer );
		virtual void update( std::shared_ptr<UIMouse> currentMouse);
		virtual void Initialize();
		virtual void MouseClick( std::shared_ptr<UIMouse> currentMouse );
		virtual std::weak_ptr<UIWidget> getTopWidgetColliding(const Vector2f& PointCanvasSpace);

		void catchKeyDown(NamedPropertyContainer& parameters);
		void setToRender(bool renderThisFrame);
		void readValue();
		float getValue();

	private:
	
		UITextEntry( const std::string& Title,
			EventSystem* pEventSystem, 
			UIController* parentController,
			std::weak_ptr<UICanvas> ParentCanvas,
			std::weak_ptr<UIWidget> parentWidget,
			HashedString uniqueID,
			float RelativeLayer,
			const Vector2f& relativeCoordinates,
			const Vector2f& collisionSize );

		std::string m_title;
		std::string m_entry;
		float m_number;
		unsigned int m_cursorPosition;
		bool m_renderThisFrame;
		bool m_selectedThisFrame;
		std::shared_ptr<Shader> m_boxShader;
	};
}