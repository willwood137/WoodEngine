#include "..\stdafx.h"

#include "UIWidget.hpp"
#include "UICanvas.hpp"

namespace woodman
{

	std::map<unsigned int, std::shared_ptr<UIWidget> > UIWidget::WidgetDatabase;

	UIWidget::UIWidget(std::shared_ptr<UICanvas> ParentCanvas,
		std::shared_ptr<UIWidget> parentWidget,
		const std::string& name,
		HashedString uniqueID)
		: m_parentCanvas(ParentCanvas),
		m_parentWidget(parentWidget),
		m_name(name),
		m_coordinates(0.0f, 0.0f),
		m_style(UIStyle::DefaultUIStyle),
		m_lockedToParent(false),
		m_canvasCollisionBoxOffset( 0.0f, 0.0f ),
		m_id(0),
		m_uniqueID(uniqueID)
	{

	}

	UIWidget::UIWidget( std::shared_ptr<UICanvas> ParentCanvas,
		std::shared_ptr<UIWidget> parentWidget,
		const std::string& name,
		HashedString uniqueID, 
		const Vector2f& canvasCoordinates)
		: m_parentCanvas(ParentCanvas),
		m_parentWidget(parentWidget),
		m_name(name),
		m_coordinates(canvasCoordinates),
		m_lockedToParent(false),	
		m_canvasCollisionBoxOffset( 0.0f, 0.0f ),
		m_id(0),
		m_uniqueID(uniqueID)

	{

	}


	void UIWidget::addChild(std::shared_ptr<UIWidget> child)
	{
		m_children.insert(child);
	}

	void UIWidget::removeChild(std::shared_ptr<UIWidget> child)
	{
		m_children.erase(child);
	}

	void UIWidget::move( const Vector2f& amountToMove )
	{
		m_coordinates += amountToMove;
		m_canvasCollisionBox += amountToMove;

		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->move(amountToMove);
		}

		calcFullCollisionBox();
	}

	AABB2D UIWidget::calcFullCollisionBox()
	{
		Vector2f vMin = m_coordinates + m_canvasCollisionBoxOffset;
		m_canvasCollisionBox = AABB2D( vMin, vMin + m_canvasCollisionBoxSize);
		m_fullCanvasCollisionBox = m_canvasCollisionBox;

		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			m_fullCanvasCollisionBox.stretchToIncludeAABB2D( (*it)->calcFullCollisionBox() );
		}

		return m_fullCanvasCollisionBox;
	}

	void UIWidget::render( std::shared_ptr<UIMouse> currentMouse)
	{
		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->render(currentMouse);
		}
	}

	void UIWidget::update( std::shared_ptr<UIMouse> currentMouse)
	{
		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->update(currentMouse);		
		}
	}

	void UIWidget::Initialize( )
	{
		calcFullCollisionBox();
	}

	UIWidget::~UIWidget()
	{

	}

	bool UIWidget::isPointInBounds(const Vector2f& point)
	{
		return m_fullCanvasCollisionBox.isPointInsideBounds(point);
	}



	//top widget is the widget colliding with the point with the lowest layer
	void UIWidget::getTopWidgetColliding(const Vector2f& PointCanvasSpace, std::shared_ptr<UIWidget>& TopWidget)
	{
		//this has a chance of being on us
		if( isPointInBounds(PointCanvasSpace) )
		{
			
			//is it colliding me?
			if(m_canvasCollisionBox.isPointInsideBounds(PointCanvasSpace))
			{
				//if its nothing set it to this
				if(TopWidget == nullptr)
				{
					TopWidget = WidgetDatabase[m_id];
				}
				else
				{
					//if the current topWidget is farther from the screen than this, set it to this
					if(TopWidget->m_layer > m_layer)
					{
						TopWidget = WidgetDatabase[m_id];
					}
				}
			}

			//check if theres anything lower in the children
			for(auto it = m_children.begin(); it != m_children.end(); ++it)
			{
				(*it)->getTopWidgetColliding(PointCanvasSpace, TopWidget);
			}
		}
	}


	unsigned int UIWidget::RegisterUIWidget(std::shared_ptr<UIWidget> Widget)
	{
		//First check if it is in database
		if(Widget->m_id != 0)
		{
			//this is registered
			WidgetDatabase[Widget->m_id] = Widget;
			return Widget->m_id;
		}
		else
		{
			
			bool inserted = false;
			while(!inserted)
			{
				unsigned int tempNum = rand();
				
				//check if it exists already
				if( WidgetDatabase.find(tempNum) == WidgetDatabase.end() )
				{
					WidgetDatabase.insert( std::make_pair(tempNum, Widget) );
					Widget->m_id = tempNum;
					inserted = true;
					return tempNum;
				}
			}
		}

		return 0;
	}

	void UIWidget::UnRegisterUIWidget(std::shared_ptr<UIWidget> Widget)
	{
		WidgetDatabase.erase(Widget->m_id);
	}

	void UIWidget::MouseDrag( std::shared_ptr<UIMouse> currentMouse )
	{
	
	}
	void UIWidget::MouseClick( std::shared_ptr<UIMouse> currentMouse )
	{

	}
	void UIWidget::MouseRClick( std::shared_ptr<UIMouse> currentMouse )
	{

	}
	void UIWidget::MouseRelease( std::shared_ptr<UIMouse> currentMouse )
	{

	}


}