#include "..\stdafx.h"

#include "UIWidget.hpp"
#include "UICanvas.hpp"

namespace woodman
{
	UIWidget::UIWidget(UICanvas* ParentCanvas,
		UIWidget* parentWidget,
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

	UIWidget::UIWidget( UICanvas* ParentCanvas,
		UIWidget* parentWidget,
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


	void UIWidget::addChild(UIWidget* child)
	{
		m_children.insert(std::unique_ptr<UIWidget>(child));
	}

	void UIWidget::removeChild(UIWidget* child)
	{
		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			if((*it).get() == child)
			{
				//we need to get rid of this
				m_children.erase(it);
				
				break;
			}
		}
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

	void UIWidget::render( UIMouse* currentMouse)
	{
		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->render(currentMouse);
		}
	}

	void UIWidget::update( UIMouse* currentMouse)
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
	void UIWidget::getTopWidgetColliding(const Vector2f& PointCanvasSpace, UIWidget*& TopWidget)
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
					TopWidget = this;
				}
				else
				{
					//if the current topWidget is farther from the screen than this, set it to this
					if(TopWidget->m_layer > m_layer)
					{
						TopWidget = this;
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

	void UIWidget::MouseDrag( UIMouse* currentMouse )
	{
	
	}
	void UIWidget::MouseClick( UIMouse* currentMouse )
	{

	}
	void UIWidget::MouseRClick( UIMouse* currentMouse )
	{

	}
	void UIWidget::MouseRelease( UIMouse* currentMouse )
	{

	}



	UICanvas* UIWidget::getParentCanvas()
	{
		return m_parentCanvas;
	}

}