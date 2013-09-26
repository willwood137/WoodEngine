#include "..\stdafx.h"

#include "UIButton.hpp"

namespace shadow
{
	const std::string SHADER_UIWINDOW_GRID_SOURCE_FILE_PATH = "Shaders\\UIWindow";


		UIButton::UIButton(const std::string& name, EventSystem* eventsystem, UIButtonGroup* buttonGroup )
			: UIWidget(name),
			EventRecipient(eventsystem),
			m_pButtonGroup(buttonGroup)
		{
			m_pButtonGroup->addButton(this);
		}

		UIButton::~UIButton()
		{

		}
		void UIButton::render( )
		{
			m_shader.load();
			glBindBuffer(GL_ARRAY_BUFFER, m_idVBO);
			glUniform2f(m_shader.getUniformID(HASHED_STRING_u_cursorPosition), 0.f, 0.f);
			glUniform2f(m_shader.getUniformID(HASHED_STRING_u_windowPosition), m_containedBox.getBox().m_vMin.x, m_containedBox.getBox().m_vMin.y);
			m_shader.setAttribute(HASHED_STRING_a_position, 2, GL_FLOAT, sizeof(ButtonVertexData), offsetof(ButtonVertexData, pos) );
			m_shader.setAttribute(HASHED_STRING_a_texCoords, 2, GL_FLOAT, sizeof(ButtonVertexData), offsetof(ButtonVertexData, uv) );
			glDisable(GL_DEPTH_TEST);
			
			
			
			Vector2f pos2 = m_containedBox.getBox().m_vMin;

			Vector2f size = m_containedBox.getBox().calcSize();

			Vector3f pos(pos2.x, pos2.y, .00001f);
			glUniform3f(m_shader.getUniformID(HASHED_STRING_u_position), 0.f, 0.f, pos.z);
			glUniform3f(m_shader.getUniformID(HASHED_STRING_u_size), m_size.x, m_size.y, 2.0f);


			glActiveTexture( GL_TEXTURE0 );
			Texture::ApplyTexture(m_texture);
			glUniform1i(m_shader.getUniformID(HASHED_STRING_u_diffuse), 0);

			glDrawArrays(GL_TRIANGLE_STRIP, m_renderData[m_renderState].startVBO, m_renderData[m_renderState].numVerts);

			m_shader.disableAttribute(HASHED_STRING_a_position);
			m_shader.disableAttribute(HASHED_STRING_a_texCoords);

			glEnable(GL_DEPTH_TEST);
		}

		void UIButton::parseXMLNode( pugi::xml_node& node )
		{
			pugi::xml_node ButtonNode = node.first_child();
			ATTACH_TYPE attachTo = ATTACH_TYPE_TOPLEFT;
			ATTACH_TYPE attachFrom = ATTACH_TYPE_TOPLEFT;
			

			Vector2f offset = Vector2f(0.f, 0.f);
			Vector2f size = Vector2f(.1f, .1f);

			while(ButtonNode)
			{
				std::string name(ButtonNode.name());

				//TODO
				//
				if(_stricmp(name.c_str(), "Texture") == 0)
				{
					m_textureName = ButtonNode.attribute("file").as_string();
				}
				else if(_stricmp(name.c_str(), "LinkWindow") == 0)
				{
					std::string windowName = ButtonNode.attribute("name").as_string();
					EventToCall event, event2;
					event.eventName = "ShowWindow";
					event.parameters.insertNamedData("WindowName", windowName);
					m_eventsToCallWhenPushed.push_back(event);
					event2.eventName = "HideWindow";
					event2.parameters.insertNamedData("WindowName", windowName);
					m_eventsToCallWhenReleased.push_back(event2);
				}
				else if(_stricmp(name.c_str(), "Event") == 0)
				{
					EventToCall event;
					event.eventName = ButtonNode.attribute("event").as_string();

					pugi::xml_node eventNode = ButtonNode.first_child();

					while (eventNode)
					{
						std::string ename(eventNode.name());
						if(_stricmp(ename.c_str(), "ParamString") == 0)
						{
							event.parameters.insertNamedData(eventNode.attribute("name").as_string(), std::string(eventNode.attribute("value").as_string() ) );
						}
						eventNode = eventNode.next_sibling();
					}

					std::string call = ButtonNode.attribute("call").as_string();
					if(_stricmp(call.c_str(), "Push") == 0 )
					{
						m_eventsToCallWhenPushed.push_back(event);
					}
					else if(_stricmp(call.c_str(), "Release") == 0 )
					{
						m_eventsToCallWhenReleased.push_back(event);
					}
				}

				ButtonNode = ButtonNode.next_sibling();
			}

			m_containedBox.setMinAttachType(attachFrom);
			m_containedBox.setMaxAttachType(attachFrom);

			
		}



		void UIButton::initialize(const AABB2D& parentBox, const Vector2f& relativeOffset )
		{
			m_containedBox.setMinOffset(relativeOffset);
			m_containedBox.setMaxOffset(relativeOffset + m_size);
			m_containedBox.setAttachmentBox(parentBox);

			m_state = BUTTON_STATE_UP;
			m_renderState = BUTTON_STATE_UP;

			std::vector<ButtonVertexData> tempVerts;
			for(int i = 0; i < 3; ++i)
			{
				createButtonVertexData(tempVerts);
			}

			m_idVBO = LoadBufferObject(GL_ARRAY_BUFFER, tempVerts.data(), sizeof(ButtonVertexData) * tempVerts.size() );


			m_texture = Texture::CreateOrGetTexture( ASSETS + "Textures\\UI\\" + m_textureName );

			//Load shader
			m_shader.loadShader(SHADER_UIWINDOW_GRID_SOURCE_FILE_PATH);
			m_shader.addUniform(HASHED_STRING_u_windowPosition);
			m_shader.addUniform(HASHED_STRING_u_position);
			m_shader.addUniform(HASHED_STRING_u_size);
			m_shader.addUniform(HASHED_STRING_u_diffuse);
			m_shader.addUniform(HASHED_STRING_u_cursorPosition);

			m_shader.addAttribute(HASHED_STRING_a_position);
			m_shader.addAttribute(HASHED_STRING_a_texCoords);
		}
	
		void UIButton::createButtonVertexData(std::vector<ButtonVertexData>& verts)
		{
			
			size_t currentState = m_renderData.size();
			m_renderData.push_back(ButtonRenderInfo() );
			m_renderData[currentState].startVBO = verts.size();

			float width = (1.f / BUTTON_STATE_SIZE );

			float x1 = currentState * width;
			float x2 = x1 + width;

			ButtonVertexData tempData;

			tempData.pos = Vector2f(0.0f, 0.0f);
			tempData.uv = Vector2f(x1, 1.0f);
			verts.push_back(tempData);

			tempData.pos = Vector2f(0.0f, 1.0f);
			tempData.uv = Vector2f(x1, 0.0f);
			verts.push_back(tempData);

			tempData.pos = Vector2f(1.0f, 0.0f);
			tempData.uv = Vector2f(x2, 1.0f);
			verts.push_back(tempData);

			tempData.pos = Vector2f(1.0f, 1.0f);
			tempData.uv = Vector2f(x2, 0.0f);
			verts.push_back(tempData);


			m_renderData[currentState].numVerts = verts.size() - m_renderData[currentState].startVBO;
		}


		void UIButton::update(const Vector2f& cursorPosition)
		{
			UIWidget::update(cursorPosition);

			if(m_state == BUTTON_STATE_UP )
			{
				if( m_containedBox.getBox().isPointInsideBounds(m_cursorPosition) )
				{
					m_renderState = BUTTON_STATE_HOVER;
				}
				else
				{
					m_renderState = m_state;
				}
			}
			else
			{
				m_renderState = m_state;
			}
		}

		bool UIButton::checkMouseClick(const Vector2f& cursorPosition)
		{
			bool collision = UIWidget::checkMouseClick(cursorPosition);
			if(collision)
			{
				if(m_state == BUTTON_STATE_DOWN)
				{
					unPush();
				}
				else if(m_state == BUTTON_STATE_UP)
				{
					push();
				}
			}
			return collision;
		}

		void UIButton::push()
		{
			if(m_pButtonGroup != nullptr)
				m_pButtonGroup->buttonPressed(this);
	
			m_state = BUTTON_STATE_DOWN;

			for each (EventToCall event in m_eventsToCallWhenPushed)
			{
				p_eventSystem->FireEvent(event.eventName, event.parameters);
			}
		}

		void UIButton::unPush()
		{
			m_state = BUTTON_STATE_UP;
			for each (EventToCall event in m_eventsToCallWhenReleased)
			{
				p_eventSystem->FireEvent(event.eventName, event.parameters);
			}
		}

		void UIButton::UIButtonGroup::addButton(UIButton* button)
		{
			m_buttons.push_back(button);
		}
		void UIButton::UIButtonGroup::removeButton(UIButton* button)
		{
			//TODO
			//
		}
		void UIButton::UIButtonGroup::buttonPressed(UIButton* button)
		{
			size_t numButtons = m_buttons.size();
			for( size_t i = 0; i < numButtons; ++i)
			{
				if(m_buttons[i] != button)
				{
					m_buttons[i]->unPush();
				}
			}
		}

	}