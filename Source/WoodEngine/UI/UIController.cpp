
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
		
		if(m_mouse->hoveringWidget != nullptr)
		{
			m_mouse->hoveringCanvas = m_mouse->hoveringWidget->getParentCanvas();
		}
		else
		{
			m_mouse->hoveringCanvas = getUICanvasAtPoint(MouseScreenPosition);
		}

		if(m_mouse->isPressed)
		{
			UIWidget* prevClicked = m_mouse->EventData[MOUSE_EVENT_LEFT_CLICK].widget;
			if(prevClicked!= nullptr)
			{
				ParentInfo info;
				prevClicked->MouseEvent(m_mouse.get(), MOUSE_EVENT_LEFT_DRAG, info); 
			}
		}

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			(*it)->update(m_mouse.get());
		}
	}

	UICanvas* UIController::getUICanvasAtPoint(const Vector2f& screenPosition)
	{
		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			if( (*it)->getScreenSpace().isPointInsideBounds(screenPosition) )
				return (*it).get();
		}

		return nullptr;
	}

	UIWidget* UIController::getUIWidgetatPoint(const Vector2f& screenPosition)
	{
		UIWidget* tempWidget = nullptr;
		UIWidget* HighestFound = nullptr;

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			tempWidget = (*it)->getUIWidgetatPoint(screenPosition);

			if(tempWidget != nullptr)
			{

				if(HighestFound == nullptr)
				{
					HighestFound = tempWidget;
				}
// 				else if(HighestFound->getLayer() > tempWidget->getLayer())
// 				{
// 					HighestFound = tempWidget;
// 				}
			}
		}

		return HighestFound;
	}

	void UIController::render()
	{
		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			(*it)->RenderCanvas(m_mouse.get());
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
		m_mouse->EventData[MOUSE_EVENT_LEFT_CLICK].prevScreenCoordinates = m_mouse->EventData[MOUSE_EVENT_LEFT_CLICK].screenCoordinates;
		m_mouse->EventData[MOUSE_EVENT_LEFT_CLICK].screenCoordinates = m_mouse->screenPosition;

		if(m_mouse->menuOpen)
		{
			bool clickedMenu = m_mouse->clickMenu(m_mouse->MainMenu.get(), m_mouse->EventData[MOUSE_EVENT_RIGHT_CLICK].screenCoordinates, p_eventSystem);
			if(!clickedMenu)
			{
				m_mouse->menuOpen = false;
			}
		}
		else 
		{
			if(m_mouse->hoveringWidget != nullptr)
			{
				ParentInfo info;
				m_mouse->hoveringWidget->MouseEvent(m_mouse.get(), MOUSE_EVENT_LEFT_CLICK, info);
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
							m_mouse->selectedCanvas = (*it).get();
						}
						else if(m_mouse->selectedCanvas->getLayer() > (*it)->getLayer() )
						{
							m_mouse->selectedCanvas = (*it).get();
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
			m_mouse->EventData[MOUSE_EVENT_RIGHT_CLICK].prevScreenCoordinates = m_mouse->EventData[MOUSE_EVENT_RIGHT_CLICK].screenCoordinates;
			m_mouse->EventData[MOUSE_EVENT_RIGHT_CLICK].screenCoordinates = m_mouse->screenPosition;

			if(m_mouse->hoveringWidget == nullptr)
			{
				m_mouse->menuOpen = true;
			}
			else
			{
				ParentInfo info;
				m_mouse->hoveringWidget->MouseEvent(m_mouse.get(), MOUSE_EVENT_RIGHT_CLICK, info);
			}
		}
	}

	void UIController::catchLMouseUp(NamedPropertyContainer& parameters)
	{
		m_mouse->isPressed = false;
		m_mouse->selectedCanvas = nullptr;

		m_mouse->EventData[MOUSE_EVENT_LEFT_RELEASE].prevScreenCoordinates = m_mouse->EventData[MOUSE_EVENT_LEFT_RELEASE].screenCoordinates;
		m_mouse->EventData[MOUSE_EVENT_LEFT_RELEASE].screenCoordinates = m_mouse->screenPosition;
		

		m_mouse->EventData[MOUSE_EVENT_LEFT_RELEASE].widget = m_mouse->hoveringWidget;
		UIWidget* widget = m_mouse->EventData[MOUSE_EVENT_LEFT_RELEASE].widget;
		if( widget != nullptr)
		{
			ParentInfo info;
			widget->MouseEvent(m_mouse.get(), MOUSE_EVENT_LEFT_RELEASE, info);
		}
	}
}