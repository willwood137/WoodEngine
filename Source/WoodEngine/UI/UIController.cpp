
#include "../stdafx.h"

#include "UIcontroller.hpp"
#include "UIMouse.hpp"

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
		
		if( !m_mouse->hoveringWidget.expired() )
		{
			m_mouse->hoveringCanvas = m_mouse->hoveringWidget.lock()->getParentCanvas();
		}
		else
		{
			m_mouse->hoveringCanvas = getUICanvasAtPoint(MouseScreenPosition);
		}

		if(m_mouse->isPressed)
		{
			if(!m_mouse->selectedWidget.expired())
			{
				if( !m_mouse->hoveringCanvas.expired() && m_mouse->selectedWidget.lock()->getParentCanvas().lock() == m_mouse->hoveringCanvas.lock() )
				{
					m_mouse->selectedWidget.lock()->MouseDrag(m_mouse);
				}
			}
		}

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			(*it)->update(m_mouse);
		}
	}

	std::weak_ptr<UICanvas> UIController::getUICanvasAtPoint(const Vector2f& screenPosition)
	{
		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			if( (*it)->getScreenSpace().isPointInsideBounds(screenPosition) )
				return (*it);
		}

		return std::weak_ptr<UICanvas>();
	}

	std::weak_ptr<UIWidget> UIController::getUIWidgetatPoint(const Vector2f& screenPosition)
	{
		std::weak_ptr<UIWidget> tempWidget;
		std::weak_ptr<UIWidget> HighestFound;

		for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
		{
			tempWidget = (*it)->getUIWidgetatPoint(screenPosition);

			if(!tempWidget.expired())
			{
				if(HighestFound.expired() || HighestFound.lock()->getAbsoluteLayer() > tempWidget.lock()->getAbsoluteLayer() )
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

	std::weak_ptr<UIWidget> UIController::getUIWidgetByID(HashedString ID)
	{
		auto it = m_widgets.find(ID);

		if(it == m_widgets.end())
			return std::weak_ptr<UIWidget>();
		else
			return it->second;

		assert(false);
	}

	//------------------------------------------------------------------------------------------------------------------------
	//----Event Functions                                                                                               ------
	//------------------------------------------------------------------------------------------------------------------------

	void UIController::catchKey(NamedPropertyContainer& parameters)
	{
		unsigned int key; 
		parameters.getNamedData(HashedString("Key"), key);

		if(m_mouse->selectedWidget.expired() || !(m_mouse->selectedWidget.lock()->getLockKeyboard() ) )
		{
			KeyDown(key);
		}
	}

	void UIController::catchLMouseDown(NamedPropertyContainer& parameters)
	{
		m_mouse->isPressed = true;
		m_mouse->prevClickPosition = m_mouse->screenPosition;

		if(m_mouse->menuOpen)
		{
			bool clickedMenu = m_mouse->clickMenu(m_mouse->MainMenu.get(), m_mouse->prevRClickPosition, p_eventSystem);
			if(!clickedMenu)
			{
				m_mouse->menuOpen = false;
			}
		}
		else 
		{
			if(!m_mouse->hoveringWidget.expired())
			{
				m_mouse->selectedWidget = m_mouse->hoveringWidget;
				m_mouse->selectedWidget.lock()->MouseClick(m_mouse);
			}
			else
			{
				//not selecting a widget, selecting the canvas. find which one
				for(auto it = m_canvases.begin(); it != m_canvases.end(); ++it)
				{
					if((*it)->getScreenSpace().isPointInsideBounds(m_mouse->screenPosition) )
					{

						if(m_mouse->selectedCanvas.expired())
						{
							m_mouse->selectedCanvas = (*it);
						}
						else if(m_mouse->selectedCanvas.lock()->getLayer() > (*it)->getLayer() )
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
			if(m_mouse->hoveringWidget.expired())
			{
				m_mouse->menuOpen = true;
			}
			else
			{
				m_mouse->hoveringWidget.lock()->MouseRClick(m_mouse);
			}
		}
	}

	void UIController::catchLMouseUp(NamedPropertyContainer& parameters)
	{
		m_mouse->isPressed = false;
		m_mouse->selectedCanvas.reset();
		
		if(!m_mouse->selectedWidget.expired())
		{
			m_mouse->selectedWidget.lock()->MouseRelease(m_mouse);
		}
		m_mouse->selectedWidget = m_mouse->hoveringWidget;
	}

	void UIController::RegisterUIWidget( std::shared_ptr<UIWidget> widget )
	{
		m_widgets.insert(std::make_pair(widget->getUniqueID(), widget) );
		if(!widget->getParentWidget().expired())
			widget->getParentWidget().lock()->addChild(widget);
	}

	void UIController::UnRegisterUIWidget( HashedString ID )
	{
		auto it = m_widgets.find(ID);
		if(it != m_widgets.end())
		{
			std::shared_ptr<UIWidget> widget = it->second;
			m_widgets.erase(ID);
			if(widget->getParentWidget().expired())
			{
				widget->getParentCanvas().lock()->UnRegisterUIWidget(widget);
			}
			else
			{
				widget->getParentWidget().lock()->removeChild(widget);
			}
			widget.reset();
		}
	}

	woodman::HashedString UIController::getUniqueID( HashedString& ID )
	{
		bool unique = false;
		HashedString uniqueID(ID);
		int i = 1;
		while(!unique)
		{
			auto it = m_widgets.find(uniqueID);
			if(it != m_widgets.end())
			{
				std::stringstream ss;
				ss << i;
				uniqueID = HashedString(ID.m_string + ss.str());
				++i;
			}
			else
			{
				unique = true;
			}
		}

		return uniqueID;
	}

}