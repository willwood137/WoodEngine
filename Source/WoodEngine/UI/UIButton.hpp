#pragma once

#include <string>

#include "UIWidget.hpp"
#include "..\Math\AABB2D.hpp"
#include "..\Engine\EventSystem.hpp"

namespace shadow
{
	enum BUTTON_STATE
	{
		BUTTON_STATE_UP = 0,
		BUTTON_STATE_HOVER,
		BUTTON_STATE_DOWN,
		BUTTON_STATE_SIZE
	};

	
	struct ButtonRenderInfo
	{
		unsigned int startVBO;
		unsigned int numVerts;
	};

	struct ButtonVertexData
	{
		Vector2f pos;
		Vector2f uv;
	};

	class UIButton : public UIWidget, public EventRecipient<UIButton>
	{
		friend class UIWindow;

	public:

		class UIButtonGroup
		{
		public:

			void addButton(UIButton* button);
			void removeButton(UIButton* button);
			void buttonPressed(UIButton* button);

		private:
			std::vector<UIButton*> m_buttons;
		};

		UIButton(const std::string& name, EventSystem* eventsystem, UIButtonGroup* buttonGroup );
		virtual ~UIButton();
		virtual void render( );
		virtual bool checkMouseClick(const Vector2f& cursorPosition);
		virtual void parseXMLNode( pugi::xml_node& node );
		virtual void initialize(const AABB2D& parentBox, const Vector2f& relativeOffset );
		void createButtonVertexData(std::vector<ButtonVertexData>& verts);
		virtual void update(const Vector2f& cursorPosition);
		
		virtual void push();
		virtual void unPush();

	protected:
		std::string m_name;
		std::string m_textureName;
		BUTTON_STATE m_state;
		BUTTON_STATE m_renderState;
		std::shared_ptr<Texture> m_texture;
		std::vector<ButtonRenderInfo> m_renderData;
		std::vector<EventToCall> m_eventsToCallWhenPushed;
		std::vector<EventToCall> m_eventsToCallWhenReleased;
		unsigned int m_idVBO;
		UIButtonGroup* m_pButtonGroup;
	};
}