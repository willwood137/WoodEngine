#include "../stdafx.h"

#include "UITextEntry.hpp"
#include "UIWidget.hpp"
#include "UICanvas.hpp"
#include "UIMouse.hpp"
#include "UIController.hpp"

namespace woodman
{
	std::weak_ptr<UITextEntry> UITextEntry::CreateUITextEntry( const std::string& _Title, 
		EventSystem* pEventSystem, 
		UIController* _ParentController, 
		std::weak_ptr<UICanvas> _ParentCanvas, 
		std::weak_ptr<UIWidget> _ParentWidget, 
		const HashedString& _ID, 
		float _RelativeLayer, 
		const Vector2f& _RelativeCoordinates, 
		const Vector2f& _CollisionSize )
	{
		assert(_ParentController != nullptr);
		std::shared_ptr<UITextEntry> newWidget(new UITextEntry(_Title, pEventSystem, _ParentController, _ParentCanvas, _ParentWidget, _ID, _RelativeLayer, _RelativeCoordinates, _CollisionSize) );
		_ParentController->RegisterUIWidget(std::dynamic_pointer_cast<UIWidget>(newWidget) );
		newWidget->Initialize();
		return std::weak_ptr<UITextEntry>(newWidget);
	}

	void UITextEntry::render( std::shared_ptr<UIMouse> currentMouse )
	{

		if(m_renderThisFrame)
		{

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			Vector2f TextScreen;
			m_parentCanvas.lock()->mapPointToScreenSpace(getAbsoluteCoordinates(), TextScreen);

			Font::DrawTextToScreen(m_title + " :", 
				m_style->TitleTextFont,
				RGBA(1.0f, 0.0f, 0.0f, .8f),
				TextScreen,
				m_style->TitleSize,
				ALIGNMENT_LEFT);

			//render box
			float zoomScale = m_parentCanvas.lock()->getZoomScale();
			Vector2f LinkBoxMinScreen, LinkBoxMaxScreen, LinkBoxMin( getAbsoluteCoordinates() + getCollisionOffset()), LinkBoxMax(LinkBoxMin + getCollisionSize() );
			m_parentCanvas.lock()->mapPointToScreenSpace(LinkBoxMin, LinkBoxMinScreen);
			m_parentCanvas.lock()->mapPointToScreenSpace(LinkBoxMax, LinkBoxMaxScreen);
			Vector2f LinkBoxSizeScreen = LinkBoxMaxScreen - LinkBoxMinScreen;
			AABB2D screenSpaceBounds = m_parentCanvas.lock()->getScreenSpace();
			m_boxShader->load();
			glBindBuffer(GL_ARRAY_BUFFER, Shader::QuadBufferID);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_position), LinkBoxMinScreen.x, LinkBoxMinScreen.y );
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_size), LinkBoxSizeScreen.x, LinkBoxSizeScreen.y);
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_screenMin), screenSpaceBounds.m_vMin.x, screenSpaceBounds.m_vMin.y  );
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_screenMax), screenSpaceBounds.m_vMax.x, screenSpaceBounds.m_vMax.y);
			glUniform2f(m_boxShader->getUniformID(HASHED_STRING_u_inverseScreenResolution), 1.0f / static_cast<float>(ScreenSize.x), 1.0f / static_cast<float>(ScreenSize.y) );

			float glLayer = getAbsoluteLayer() / g_MaxLayer;
			m_boxShader->SetUniformFloat(HASHED_STRING_u_layer, glLayer, 1);


			glUniform4f(m_boxShader->getUniformID(HASHED_STRING_u_color), 1.0f, .95f, .9f, 1.0f);


			m_boxShader->setAttribute(HASHED_STRING_in_position, 2, GL_FLOAT, sizeof(Vector2f), 0);

			glDrawArrays( GL_QUADS, 0, 4);

			m_boxShader->disableAttribute(HASHED_STRING_in_position);

			//render entry
			Vector2f entryPos = getAbsoluteCoordinates() + getCollisionOffset() + Vector2f( 4.0f, 6.0f);
			Vector2f entryPosScreen;
			m_parentCanvas.lock()->mapPointToScreenSpace(entryPos, entryPosScreen);
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
		m_blockKeyboard = true;

		UIWidget::Initialize();
	}

	void UITextEntry::MouseClick( std::shared_ptr<UIMouse> currentMouse )
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

	UITextEntry::UITextEntry( const std::string& Title, EventSystem* pEventSystem, UIController* parentController, std::weak_ptr<UICanvas> ParentCanvas, std::weak_ptr<UIWidget> parentWidget, HashedString uniqueID, float RelativeLayer, const Vector2f& relativeCoordinates, const Vector2f& collisionSize )
		: UIWidget( parentController, ParentCanvas, parentWidget, uniqueID, RelativeLayer, relativeCoordinates, collisionSize ),
		EventRecipient(pEventSystem),
		m_title(Title)
	{

	}

	void UITextEntry::setToRender( bool renderThisFrame )
	{
		m_renderThisFrame = renderThisFrame;
	}

	void UITextEntry::update( std::shared_ptr<UIMouse> currentMouse )
	{
		if(!currentMouse->selectedWidget.expired() && currentMouse->selectedWidget.lock() == m_selfPtr.lock())
		{
			m_selectedThisFrame = true;
		}
		else
		{
			m_selectedThisFrame = false;
		}
		if(currentMouse->selectedNodeBox.lock().get() == parentNodeBox)
		{
			m_renderThisFrame = true;
		}
		else
		{
			m_renderThisFrame = false;
		}
		UIWidget::update(currentMouse);
	}

	std::weak_ptr<UIWidget> UITextEntry::getTopWidgetColliding( const Vector2f& PointCanvasSpace )
	{
		if(m_renderThisFrame)
		{
			return UIWidget::getTopWidgetColliding(PointCanvasSpace);
		}

		return std::weak_ptr<UIWidget>();
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