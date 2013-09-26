
#include "../stdafx.h"

#include "UIcontroller.hpp"

namespace woodman
{
	UIController::UIController(EventSystem* eventsystem )
		: EventRecipient(eventsystem),
		m_mouse(new UIMouse())
	{
		
	}

	void UIController::Initialize()
	{
		p_eventSystem->RegisterObjectForEvent( this, &UIController::catchKey, "KeyDown" );
		p_eventSystem->RegisterObjectForEvent( this, &UIController::catchLMouseDown, "LMouseDown" );
		p_eventSystem->RegisterObjectForEvent( this, &UIController::catchRMouseDown, "RMouseDown" );
		p_eventSystem->RegisterObjectForEvent( this, &UIController::catchLMouseUp, "LMouseUp" );

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			(*it)->Initialize();
		}

		m_mouse->mouseMenuShader = Shader::CreateOrGetShader(ASSETS + "Shaders\\UI\\MouseMenu");
	}

	void UIController::KeyDown(unsigned int key)
	{

	}

	void UIController::update( const Vector2f& MouseScreenPosition )
	{
		m_mouse->prevScreenPosition = m_mouse->screenPosition;
		m_mouse->screenPosition = MouseScreenPosition;

		m_mouse->hoveringWidget = getUIWidgetatPoint(MouseScreenPosition);

		if(m_mouse->isPressed)
		{
			if(m_mouse->selectedWidget != nullptr)
			{
				m_mouse->selectedWidget->MouseDrag(m_mouse);
			}
		}

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			(*it)->update(m_mouse);
		}
	}

	std::shared_ptr<UIWidget> UIController::getUIWidgetatPoint(const Vector2f& screenPosition)
	{
		std::shared_ptr<UIWidget> tempWidget, HighestFound;

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			tempWidget = (*it)->getUIWidgetatPoint(screenPosition);

			if(tempWidget != nullptr)
			{

				if(HighestFound == nullptr)
				{
					HighestFound = tempWidget;
				}
				else if(HighestFound->getLayer() > tempWidget->getLayer())
				{
					HighestFound = tempWidget;
				}
			}
		}

		return HighestFound;
	}

	void UIController::render()
	{
		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			(*it)->RenderCanvas(m_mouse);
		}

		if(m_mouse->menuOpen)
		{
			m_mouse->render();
		}
	}



	//------------------------------------------------------------------------------------------------------------------------
	//----Event Functions                                                                                               ------
	//------------------------------------------------------------------------------------------------------------------------

	void UIController::catchKey(NamedPropertyContainer& parameters)
	{
		unsigned int key; 
		parameters.getNamedData(HashedString("Key"), key);

		KeyDown(key);
	}

	void UIController::catchLMouseDown(NamedPropertyContainer& parameters)
	{
		m_mouse->isPressed = true;
		m_mouse->prevClickPosition = m_mouse->screenPosition;

		if(m_mouse->menuOpen)
		{
			bool clickedMenu = m_mouse->clickMenu(m_mouse->MainMenu, m_mouse->prevRClickPosition, p_eventSystem);
			if(!clickedMenu)
			{
				m_mouse->menuOpen = false;
			}
		}
		else 
		{
			if(m_mouse->hoveringWidget != nullptr)
			{
				m_mouse->selectedWidget = m_mouse->hoveringWidget;
				m_mouse->hoveringWidget->MouseClick(m_mouse);
			}
			else
			{
				//not selecting a widget, selecting the canvas. find which one
				for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
				{
					if((*it)->getScreenSpace().isPointInsideBounds(m_mouse->screenPosition) )
					{

						if(m_mouse->selectedCanvas == nullptr)
						{
							m_mouse->selectedCanvas = (*it);
						}
						else if(m_mouse->selectedCanvas->getLayer() > (*it)->getLayer() )
						{
							m_mouse->selectedCanvas = (*it);
						}
					}
				}
			}
		}
	}

	void UIController::catchRMouseDown(NamedPropertyContainer& parameters)
	{
		if(!m_mouse->isPressed)
		{
			m_mouse->prevRClickPosition = m_mouse->screenPosition;
			if(m_mouse->hoveringWidget == nullptr)
			{
				m_mouse->menuOpen = true;
			}
			else
			{
				m_mouse->hoveringWidget->MouseRClick(m_mouse);
			}
		}
	}

	void UIController::catchLMouseUp(NamedPropertyContainer& parameters)
	{
		m_mouse->isPressed = false;
		m_mouse->selectedCanvas = nullptr;
		
		if(m_mouse->selectedWidget != nullptr)
		{
			m_mouse->selectedWidget->MouseRelease(m_mouse);
		}
		m_mouse->selectedWidget = m_mouse->hoveringWidget;
	}
}