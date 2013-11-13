#include "../stdafx.h"

#include "UITextEntry.hpp"
#include "UICanvas.hpp"

namespace woodman
{


	void UITextEntry::render( UIMouse* currentMouse, float ParentLayer )
	{

		if(m_renderThisFrame)
		{

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			Vector2f TextScreen;
			m_parentCanvas->mapPointToScreenSpace(m_coordinates, TextScreen);

			Font::DrawTextToScreen(m_name + " :", 
				m_style->TitleTextFont,
				RGBA(1.0f, 0.0f, 0.0f, .8f),
				TextScreen,
				m_style->TitleSize,
				ALIGNMENT_LEFT);

			//render box
			float zoomScale = m_parentCanvas->getZoomScale();
			float layer = ParentLayer - m_relativeLayer;
			Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin( m_coordinates + m_canvasCollisionBoxOffset), LinkBoxMax(LinkBoxMin + m_canvasCollisionBoxSize );
			m_parentCanvas->mapPointToScreenSpace(LinkBoxMin, LinkBoxMinScreen);
			m_parentCanvas->mapPointToScreenSpace(LinkBoxMax, LinkBoxMaxScreen);
			Vector2f LinkBoxSizeScreen = LinkBoxMaxScreen - LinkBoxMinScreen;
			AABB2D screenSpaceBounds = m_parentCanvas->getScreenSpace();
			m_boxShader->load();
			glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_position), LinkBoxMinScreen.x, LinkBoxMinScreen.y );
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_size), LinkBoxSizeScreen.x, LinkBoxSizeScreen.y);
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_screenMin), screenSpaceBounds.m_vMin.x, screenSpaceBounds.m_vMin.y  );
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_screenMax), screenSpaceBounds.m_vMax.x, screenSpaceBounds.m_vMax.y);
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

			float glLayer = layer / g_MaxLayer;
			m_boxShader->SetUniformFloat(HASHED_STRING_u_layer, glLayer, 1);


			glUniform4f(m_boxShader->getUniformID(HASHED_STRING_u_color), 1.0f, .95f, .9f, 1.0f);


			m_boxShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

			glDrawArrays( GL_QUADS, 0, 4);

			m_boxShader->disableAttribute(HASHED_STRING_in_position);

			//render entry
			Vector2f entryPos = m_coordinates + m_canvasCollisionBoxOffset + Vector2f( 4.0f, 6.0f);
			Vector2f entryPosScreen;
			m_parentCanvas->mapPointToScreenSpace(entryPos, entryPosScreen);
			Font::DrawTextToScreen(m_entry, 
				m_style->TitleTextFont,
				RGBA(0.0f, 0.0f, 0.0f, 1.0f),
				entryPosScreen,
				m_style->subTitleSize * .75f,
				ALIGNMENT_LEFT);

		}

		//
	}

	void UITextEntry::Initialize()
	{
		m_boxShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\TextBox");
		p_eventSystem->RegisterObjectForEvent( this, &UITextEntry::catchKeyDown, "KeyDown" );
		m_lockKeyboard = true;
	}

	void UITextEntry::MouseClick( UIMouse* currentMouse )
	{
		m_cursorPosition = m_entry.size();
	}

	void UITextEntry::catchKeyDown( NamedPropertyContainer& parameters )
	{
		if(m_selectedThisFrame)
		{
			unsigned int key; 
			parameters.getNamedData(HashedString("Key"), key);

			char c = key;

			if( c >= '0' && c <= '9')
			{
				if(m_entry.size() < 16)
				{
					if(m_cursorPosition <= m_entry.size() )
					{
						m_entry.insert(m_cursorPosition, 1, c);
						m_cursorPosition++;
					}
				}
			}
			else if( key == VK_OEM_PERIOD)
			{
				if(m_entry.size() < 16)
				{
					if(m_cursorPosition <= m_entry.size() )
					{
						if( m_entry.find('.') == std::string::npos )
						{
							m_entry.insert(m_cursorPosition, 1, '.');
							m_cursorPosition++;
						}
					}
				}

			}
			else if( key == VK_BACK)
			{
				if(m_cursorPosition <= m_entry.size() && m_cursorPosition > 0 )
				{
					m_entry.erase(m_cursorPosition-1, 1);
					m_cursorPosition--;
				}
			}
			else if( key == VK_DELETE)
			{
				if(m_cursorPosition < m_entry.size() && m_cursorPosition >= 0 )
				{
					m_entry.erase(m_cursorPosition, 1);
				}
			}
			else if( key == VK_LEFT )
			{
				m_cursorPosition--;
				m_cursorPosition = clamp<unsigned int>(m_cursorPosition, 0, m_entry.size());
			}
			else if( key == VK_RIGHT)
			{
				m_cursorPosition++;
				m_cursorPosition = clamp<unsigned int>(m_cursorPosition, 0, m_entry.size());
			}
			else if( key == VK_UP )
			{
				m_cursorPosition = 0;
			}
			else if( key == VK_DOWN )
			{
				m_cursorPosition = m_entry.size();
			}

			readValue();
		}
	}

	UITextEntry::UITextEntry( UICanvas* ParentCanvas, UIWidget* parentWidget, const std::string& name, HashedString uniqueID, float RelativeLayer, EventSystem* pEventSystem )
		:UIWidget(ParentCanvas, parentWidget, name, uniqueID, RelativeLayer),
		EventRecipient(pEventSystem),
		m_number(0.0f),
		m_cursorPosition(0),
		m_entry("0.0")
	{

	}

	void UITextEntry::setToRender( bool renderThisFrame )
	{
		m_renderThisFrame = renderThisFrame;
	}

	void UITextEntry::update( UIMouse* currentMouse )
	{
		if(currentMouse->selectedWidget == this)
		{
			m_selectedThisFrame = true;
		}
		else
		{
			m_selectedThisFrame = false;
		}
		m_renderThisFrame = false;
		UIWidget::update(currentMouse);
	}

	void UITextEntry::getTopWidgetColliding( const Vector2f& PointCanvasSpace, UIWidget*& TopWidget, float ParentLayer, float CurrentLayer )
	{
		if(m_renderThisFrame)
		{
			UIWidget::getTopWidgetColliding(PointCanvasSpace, TopWidget, ParentLayer, CurrentLayer);
		}
	}

	void UITextEntry::readValue()
	{
		std::stringstream ss;
		ss << m_entry;
		ss >> m_number;
	}

	float UITextEntry::getValue()
	{
		return m_number;
	}

}