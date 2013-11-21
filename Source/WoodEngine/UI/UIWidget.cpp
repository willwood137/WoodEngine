#include "..\stdafx.h"

#include "UIWidget.hpp"
#include "UICanvas.hpp"
#include "UIController.hpp"

namespace woodman
{
	UIWidget::UIWidget( UIController* parentController, 
		std::weak_ptr<UICanvas> ParentCanvas, 
		std::weak_ptr<UIWidget> parentWidget, 
		HashedString uniqueID, 
		float RelativeLayer, 
		const Vector2f& relativeCoordinates,
		const Vector2f& collisionSize )
		: m_uniqueID(uniqueID),
		m_parentCanvas( ParentCanvas ),
		m_parentWidget( parentWidget ),
		m_relativeLayer( RelativeLayer ),
		m_relativeCoordinates( relativeCoordinates ),
		m_canvasCollisionBoxSize( collisionSize)
	{

	}

	void UIWidget::Initialize( )
	{
		m_selfPtr = m_parentController->getUIWidgetByID(m_uniqueID);
		if(m_parentWidget.lock() != nullptr)
		{
			UIWidgetParentInfo info(m_parentWidget.lock()->getAbsoluteCoordinates(), m_parentWidget.lock()->getAbsoluteLayer() );
			calcAbsoluteData(info);
		}
		else
		{
			calcAbsoluteData( UIWidgetParentInfo( Vector2f(0.0f, 0.0f), m_parentCanvas.lock()->getLayer() ) );
		}

		calcFullCollisionBox();
	}

	//////////////////////////////////////////////////////////////////////////

	void UIWidget::addChild( std::shared_ptr<UIWidget> child )
	{
		m_children.insert(child);
	}

	void UIWidget::removeChild( std::shared_ptr<UIWidget> child )
	{
		//we need to get rid of this
		m_children.erase(child);
	}

	//////////////////////////////////////////////////////////////////////////

	void UIWidget::move( const Vector2f& amountToMove )
	{
		m_relativeCoordinates += amountToMove;
		
		calcFullCollisionBox();
	}


	void UIWidget::calcAbsoluteData( const UIWidgetParentInfo& info )
	{
		m_absoluteLayer = info.Layer + m_relativeLayer;
		m_absoluteCoordinates = info.Position + m_relativeCoordinates;
		m_absoluteCanvasCollisionBox.m_vMin = m_absoluteCoordinates + m_canvasCollisionBoxOffset;
		m_absoluteCanvasCollisionBox.m_vMax = m_absoluteCanvasCollisionBox.m_vMin + m_canvasCollisionBoxSize;

		UIWidgetParentInfo pInfo(m_absoluteCoordinates, m_absoluteLayer);
		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->calcAbsoluteData(pInfo);
		}

		calcFullCollisionBox();
	}

	AABB2D UIWidget::calcFullCollisionBox()
	{
		Vector2f vMin = m_absoluteCoordinates + m_canvasCollisionBoxOffset;
		m_absoluteCanvasCollisionBox = AABB2D( vMin, vMin + m_canvasCollisionBoxSize);
		m_fullCanvasCollisionBox = m_absoluteCanvasCollisionBox;

		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			m_fullCanvasCollisionBox.stretchToIncludeAABB2D( (*it)->calcFullCollisionBox() );
		}

		return m_fullCanvasCollisionBox;
	}

	//////////////////////////////////////////////////////////////////////////

	void UIWidget::render( std::shared_ptr<UIMouse> currentMouse )
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

	UIWidget::~UIWidget()
	{

	}

	bool UIWidget::isPointInBounds(const Vector2f& point)
	{
		return m_fullCanvasCollisionBox.isPointInsideBounds(point);
	}



	//top widget is the widget colliding with the point with the lowest layer
	std::weak_ptr<UIWidget> UIWidget::getTopWidgetColliding( const Vector2f& PointCanvasSpace )
	{
		std::weak_ptr<UIWidget> TopWidget;
		//this has a chance of being on us
		if( isPointInBounds(PointCanvasSpace) )
		{
			//is it colliding me?
			if(m_absoluteCanvasCollisionBox.isPointInsideBounds(PointCanvasSpace))
			{
				TopWidget = m_selfPtr;
			}

			//check if theres anything lower in the children
			for(auto it = m_children.begin(); it != m_children.end(); ++it)
			{
				std::weak_ptr<UIWidget> tempTopWidget = (*it)->getTopWidgetColliding(PointCanvasSpace);
				
				if( !tempTopWidget.expired() )
				{
					if( TopWidget.expired() || tempTopWidget.lock()->getAbsoluteLayer() < TopWidget.lock()->getAbsoluteLayer() )
						TopWidget = tempTopWidget;
				}
			}
		}
		return TopWidget;
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



	

	//////////////////////////////////////////////////////////////////////////
	//			Getters / Setters											//
	//////////////////////////////////////////////////////////////////////////

	std::weak_ptr<UICanvas> UIWidget::getParentCanvas()
	{
		return m_parentCanvas;
	}
	bool UIWidget::getLockKeyboard()
	{
		return m_blockKeyboard;
	}
	void UIWidget::setLockKeyboard( bool lock )
	{
		m_blockKeyboard = lock;
	}

	void UIWidget::setRelativeCoordinates( const Vector2f& RelativeCoordinates )
	{
		m_relativeCoordinates = RelativeCoordinates;
		if(m_parentWidget.lock() != nullptr)
		{
			UIWidgetParentInfo info(m_parentWidget.lock()->getAbsoluteCoordinates(), m_parentWidget.lock()->getAbsoluteLayer() );
			calcAbsoluteData(info);
		}
		else
		{
			calcAbsoluteData( UIWidgetParentInfo( Vector2f(0.0f, 0.0f), m_parentCanvas.lock()->getLayer() ) );
		}
	}

	woodman::Vector2f UIWidget::getRelativeCoordinates() const
	{
		return m_relativeCoordinates;
	}

	void UIWidget::setCollisionSize( const Vector2f& size )
	{
		m_canvasCollisionBoxSize = size;
		if(m_parentWidget.lock() != nullptr)
		{
			UIWidgetParentInfo info(m_parentWidget.lock()->getAbsoluteCoordinates(), m_parentWidget.lock()->getAbsoluteLayer() );
			calcAbsoluteData(info);
		}
		else
		{
			calcAbsoluteData( UIWidgetParentInfo( Vector2f(0.0f, 0.0f), m_parentCanvas.lock()->getLayer() ) );
		}
	}

	woodman::Vector2f UIWidget::getCollisionSize() const
	{
		return m_canvasCollisionBoxSize;
	}

	void UIWidget::setCollisionOffset( const Vector2f& offset )
	{
		m_canvasCollisionBoxOffset = offset;
		if(m_parentWidget.lock() != nullptr)
		{
			UIWidgetParentInfo info(m_parentWidget.lock()->getAbsoluteCoordinates(), m_parentWidget.lock()->getAbsoluteLayer() );
			calcAbsoluteData(info);
		}
		else
		{
			calcAbsoluteData( UIWidgetParentInfo( Vector2f(0.0f, 0.0f), m_parentCanvas.lock()->getLayer() ) );
		}
	}

	woodman::Vector2f UIWidget::getCollisionOffset() const
	{
		return m_canvasCollisionBoxOffset;
	}

	void UIWidget::setStyle( std::shared_ptr<UIStyle> style )
	{
		m_style= style;
	}

	std::shared_ptr<UIStyle> UIWidget::getStyle() const
	{
		return m_style;
	}

	void UIWidget::setLockedToParent( bool lockToParent )
	{
		m_blockKeyboard = lockToParent;
	}

	bool UIWidget::getLockedToParent() const
	{
		return m_blockedToParent;
	}

	woodman::HashedString UIWidget::getUniqueID()
	{
		return m_uniqueID;
	}

	std::weak_ptr<UIWidget> UIWidget::getParentWidget()
	{
		return m_parentWidget;
	}

	float UIWidget::getAbsoluteLayer() const
	{
		return m_absoluteLayer;
	}

	woodman::Vector2f UIWidget::getAbsoluteCoordinates() const
	{
		return m_absoluteCoordinates;
	}

	woodman::AABB2D UIWidget::getAbsoluteCollisionBox() const
	{
		return m_absoluteCanvasCollisionBox;
	}

	UIController* UIWidget::getParentController() const
	{
		return m_parentController;
	}

	//////////////////////////////////////////////////////////////////////////
}