#include "..\stdafx.h"

#include "UIButtonBar.hpp"
#include "..\Utility\Utility.hpp"
#include "UIButton.hpp"

namespace shadow
{
	UIButtonBar::UIButtonBar(const std::string& name, EventSystem* eventsystem)
		:UIWindow(name, eventsystem),
		m_maxHeight(1),
		m_maxWidth(1)
	{

	}


	UIWindow* UIButtonBar::parseWindow(pugi::xml_node& node)
	{
		pugi::xml_node WindowNode = node.first_child();
		ATTACH_TYPE attachTo = ATTACH_TYPE_BOTTOMLEFT;
		ATTACH_TYPE attachFrom = ATTACH_TYPE_BOTTOMLEFT;
		m_startButton = ATTACH_TYPE_TOPLEFT;
		m_drawBorderOnEdges = false;
		m_direction = DIRECTION_DOWN;
		
		Vector2f offset = Vector2f(0.f, 0.f);
		Vector2f size = Vector2f(.1f, .1f);

		m_buttons = std::vector<UIButton*>();
		m_widgets = std::vector<UIWidget*>();

		m_pButtonGroup = new UIButton::UIButtonGroup();
		m_hidden = false;

		m_childrenWindows.clear();

		while(WindowNode)
		{
			std::string name(WindowNode.name());

			//TODO
			//

			

			if(_stricmp(name.c_str(), "MaxHeight") == 0)
			{
				getValueFromString(WindowNode.first_child().value(), m_maxHeight);
			}
			else if(_stricmp(name.c_str(), "MaxWidth") == 0)
			{
					getValueFromString(WindowNode.first_child().value(), m_maxWidth);
			}
			else if(_stricmp(name.c_str(), "ButtonSize") == 0)
			{
				m_buttonSize.x = WindowNode.attribute("x").as_float();
				m_buttonSize.y = WindowNode.attribute("y").as_float();
			}
			else if(_stricmp(name.c_str(), "ButtonSpacing") == 0)
			{
				m_buttonSpacing.x = WindowNode.attribute("x").as_float();
				m_buttonSpacing.y = WindowNode.attribute("y").as_float();
			}
			else if(_stricmp(name.c_str(), "Border") == 0)
			{
				m_borderLength = WindowNode.attribute("length").as_float();
				m_borderBarTextureFile = WindowNode.attribute("barTextureFile").as_string();
				m_borderCornerTextureFile = WindowNode.attribute("cornerTextureFile").as_string();
				m_backgroundTextureFile = WindowNode.attribute("backgroundTexturefile").as_string();
			}
			else if(_stricmp(name.c_str(), "AttachTo") == 0)
			{
				attachTo = getAttachTypeFromString(WindowNode.first_child().value());
			}
			else if(_stricmp(name.c_str(), "AttachFrom") == 0)
			{
				attachFrom = getAttachTypeFromString(WindowNode.first_child().value());
			}
			else if(_stricmp(name.c_str(), "Window") == 0)
			{
				std::string type(WindowNode.attribute("type").as_string());
				std::string name(WindowNode.attribute("name").as_string());

				

				if(_stricmp(type.c_str(), "buttonbar") == 0)
				{
					UIWindow* window = new UIButtonBar( (*this) );
					window->setName(name);
					m_childrenWindows.push_back(window->parseWindow(WindowNode));
				}
			}
			else if(_stricmp(name.c_str(), "Button") == 0)
			{
				//Add Buttons
				//
				std::string name = WindowNode.attribute("name").as_string();
				UIButton* button = new UIButton( name, p_eventSystem, m_pButtonGroup );
				button->parseXMLNode(WindowNode);
				m_widgets.push_back( button );
				m_buttons.push_back( button );
			}
			else if(_stricmp(name.c_str(), "Tags") == 0)
			{
				pugi::xml_node tagNode = WindowNode.first_child();
				
				while(tagNode)
				{
					if(_stricmp(tagNode.value(), "Hidden") == 0 )
					{
						m_hidden = true;
					}
					tagNode = tagNode.next_sibling();
				}
			}


			WindowNode = WindowNode.next_sibling();
		}

		m_collisionBox = AttachedAABB2D( attachFrom, attachTo, offset, size);
		return this;
	}

	void UIButtonBar::initialize( const AABB2D& parentBox )
	{
		int numButtons = m_buttons.size();
		int max;
		Vector2f spacing, size;

		switch(m_direction)
		{
		case DIRECTION_DOWN:
		case DIRECTION_UP:
			max = m_maxHeight;
			spacing.x = m_buttonSpacing.x;
			spacing.y = m_buttonSpacing.y * -1;
			size.x = m_buttonSize.x;
			size.y = m_buttonSize.y;
			break;
		case DIRECTION_LEFT:
		case DIRECTION_RIGHT:
			max = m_maxWidth;
			spacing.x = m_buttonSpacing.x;
			spacing.y = m_buttonSpacing.y * -1;
			size.x = m_buttonSize.x;
			size.y = m_buttonSize.y;
			break;
		}

		
		float x = ( m_buttonSpacing.x ) * (numButtons / max) + ( m_buttonSize.x * ( ( numButtons + (max - 1) ) / max ) ) + m_borderLength * 2;
		float y = (m_buttonSpacing.y * (max) ) + m_buttonSize.y * max + m_borderLength * 2;

		m_collisionBox.setSize(Vector2f(x, y));
		m_collisionBox.setOffset(Vector2f(-m_borderLength * .5f, 0.f) );



		UIWindow::initialize(parentBox);

		for(int i = 0; i < numButtons; ++i)
		{
			Vector2f buttonCoord(static_cast<float>( i / max ), static_cast<float>( i % max ) );
			Vector2f offset(buttonCoord);

			offset.x *= size.x + spacing.x;
			offset.x += spacing.x + m_borderLength * .5f;


			offset.y *= (-size.y) + spacing.y;
			offset.y += (spacing.y - size.y) - m_borderLength * .5f;
		
			m_buttons[i]->setSize(size);

			m_buttons[i]->initialize(m_collisionBox.getAABB2D(), offset );
		}


		
	
	}
}