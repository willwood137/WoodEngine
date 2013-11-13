#include "..\stdafx.h"

#include "UIWidget.hpp"
#include "UICanvas.hpp"

namespace woodman
{
	

	UIWidget::UIWidget( UICanvas* ParentCanvas,
		UIWidget* parentWidget,
		const std::string& name,
		float RelativeLayer, 
		const Vector2f& relativeCoordinates)
		: m_parentCanvas(ParentCanvas),
		m_parentWidget(parentWidget),
		m_name(name),
		m_relativeLayer(RelativeLayer),
		m_relativeCoordinates(relativeCoordinates),
		m_size(Vector2f(0.0f, 0.0f))
	{
		m_style = UIStyle::DefaultUIStyle;
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
		m_relativeCoordinates += amountToMove;
		
		calcFullCollisionBox();
	}

	AABB2D UIWidget::calcFullCollisionBox()
	{
		m_absoluteCollisionBox = AABB2D(m_absoluteCoordinates, m_absoluteCoordinates + m_size);

		m_fullCanvasCollisionBox = m_absoluteCollisionBox;

		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			m_fullCanvasCollisionBox.stretchToIncludeAABB2D( (*it)->calcFullCollisionBox() );
		}

		return m_fullCanvasCollisionBox;
	}


	void UIWidget::render(  UIMouse* currentMouse, const AABB2D& CanvasBounds ) const
	{
		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->render( currentMouse, CanvasBounds );
		}
	}

	void UIWidget::update( UIMouse* currentMouse, const ParentInfo& parent_info )
	{
		updateData( currentMouse, parent_info );
		updateAbsoluteData(currentMouse, parent_info);
	}

	void UIWidget::updateData( UIMouse* currentMouse, const ParentInfo& parent_info )
	{
		

		ParentInfo myInfo( parent_info );

		// update absolute Coordinates
		m_absoluteCoordinates = parent_info.Coordinates + m_relativeCoordinates;
		m_absoluteCollisionBox = AABB2D(m_absoluteCoordinates, m_absoluteCoordinates + m_size);
		m_absoluteLayer = parent_info.Layer + m_relativeLayer;

		myInfo.Coordinates = m_absoluteCoordinates;
		myInfo.Layer = m_absoluteLayer;
		myInfo.CanvasBounds.clip(m_absoluteCollisionBox);


		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->updateData(currentMouse, myInfo );	
		}
	}


	void UIWidget::updateAbsoluteData( UIMouse* currentMouse, const ParentInfo& parent_info )
	{

		ParentInfo myInfo( parent_info );
		m_absoluteCoordinates = parent_info.Coordinates + m_relativeCoordinates;
		m_absoluteCollisionBox = AABB2D(m_absoluteCoordinates, m_absoluteCoordinates + m_size);
		m_absoluteLayer = parent_info.Layer + m_relativeLayer;

		myInfo.Coordinates = m_absoluteCoordinates;
		myInfo.Layer = m_absoluteLayer;
		myInfo.CanvasBounds.clip(m_absoluteCollisionBox);

		for(auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			(*it)->updateAbsoluteData(currentMouse, myInfo);
		}

		//calculate the current full size now
		calcFullCollisionBox();
	}

	void UIWidget::Initialize( )
	{
		calcFullCollisionBox();
	}

	UIWidget::~UIWidget()
	{

	}


	//top widget is the widget colliding with the point with the lowest layer
	UIWidget* UIWidget::getTopWidgetColliding(const Vector2f& PointCanvasSpace, const ParentInfo& parent_info, float &HighestLayer)
	{
		//this has a chance of being on us
		AABB2D canvasBoxFull = m_fullCanvasCollisionBox + parent_info.Coordinates;
		if(  canvasBoxFull.isPointInsideBounds(PointCanvasSpace) )
		{
			ParentInfo myInfo;

			myInfo.Layer = parent_info.Layer + m_relativeLayer;
			myInfo.Coordinates = parent_info.Coordinates + m_relativeCoordinates;
			//is it colliding me?

			
			UIWidget* currentHighest = nullptr;
			//check for highest among children
			for(auto it = m_children.begin(); it != m_children.end(); ++it)
			{
				UIWidget* foundWidget = (*it)->getTopWidgetColliding(PointCanvasSpace, myInfo, HighestLayer);
				if(foundWidget != nullptr)
					currentHighest = foundWidget;
			}

			AABB2D canvasBox = m_absoluteCollisionBox + parent_info.Coordinates;
			if(canvasBox.isPointInsideBounds(PointCanvasSpace))
			{
				//if its nothing set it to this
				
				//if the current topWidget is farther from the screen than this, set it to this
				if( HighestLayer < myInfo.Layer )
				{
					HighestLayer = myInfo.Layer;
					currentHighest = this;
				}
				
			}

			return currentHighest;
		}

		return nullptr;
	}


		/*
		* Getters and setters
		*/

		//Relative Coordinates Info
		void UIWidget::setRelativeCoordinates(const Vector2f& relative_coordinates )
		{
			m_relativeCoordinates = relative_coordinates;
		}
		Vector2f UIWidget::getRelativeCoordinates() const
		{
			return m_relativeCoordinates;
		}

		void UIWidget::setRelativeLayer(float relative_layer)
		{
			m_relativeLayer = relative_layer;
		}
		float UIWidget::getRelativeLayer() const
		{
			return m_relativeLayer;
		}

		// CollisionBoxInfo
		void UIWidget::setSize(const Vector2f& size )
		{
			m_size = size;
		}
		Vector2f UIWidget::getSize() const
		{
			return m_size;
		}
		AABB2D UIWidget::getRelativeCollisionBox() const
		{
			return m_absoluteCollisionBox;
		}

		// Style
		void UIWidget::setStyle( std::shared_ptr<UIStyle> style )
		{
			m_style = style;
		}
		std::shared_ptr<UIStyle> UIWidget::getStyle() const
		{
			return m_style;
		}


		//Name
		void UIWidget::setName(const std::string& name)
		{
			m_name = name;
		}
		std::string UIWidget::getName() const
		{
			return m_name;
		}

		//parent info
		UICanvas* UIWidget::getParentCanvas() const
		{
			return m_parentCanvas;
		}
		UIWidget* UIWidget::getParentWidget() const
		{
			return m_parentWidget;
		}

		void UIWidget::MouseEvent( UIMouse* currentMouse, MOUSE_EVENT_TYPE mouse_event, const ParentInfo& parent_info )
		{

		}

		float UIWidget::getAbsoluteLayer() const
		{
			return m_absoluteLayer;
		}

		woodman::AABB2D UIWidget::getAbsoluteCollisionBox() const
		{
			return m_absoluteCollisionBox;
		}

		woodman::Vector2f UIWidget::getAbsoluteCoordinates() const
		{
			return m_absoluteCoordinates;
		}

		void UIWidget::setAbsoluteCoordinates( const Vector2f& coordinates )
		{
			m_absoluteCoordinates = coordinates;
		}



		

		//
		// end Getters/Setters
}